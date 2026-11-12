#!/usr/bin/env python3
# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar
#
# SPDX-License-Identifier: MIT
"""Post-link check: core1-executed code must not reach into flash.

On raspberrypi-port boards with USB host, core1 runs the PIO-USB frame loop
and may only execute (or read) RAM-resident code: core1_main enables an MPU
region that makes flash inaccessible (see common-hal/usb_host/Port.c), and the
linker script deliberately RAM-places tuh_task_event_ready() and the PIO-USB
code. Nothing verified their *callees* stay in RAM, which is how issue #10243
happened: an upstream TinyUSB change added a flash-resident call inside
RAM-placed tuh_task_event_ready(), core1 faulted at the first device attach,
and USB host went silent.

This tool disassembles the linked ELF, walks the static call graph from the
core1 entry points, and fails if any reachable function lives in flash. Linker
veneers (long-call trampolines) are followed through their literal-pool
targets, so a flash callee hidden behind a RAM veneer is still detected.

Usage:
    check_core1_flash_calls.py firmware.elf [--root SYMBOL]... [--allow SYMBOL]...

Default root: core1_main. --allow skips a symbol (and everything only
reachable through it); use it for calls that provably run before the MPU is
enabled.

Limitation: indirect calls (blx rN / function pointers) cannot be traced
statically. They are counted and reported per function so the gaps are
visible.
"""

import argparse
import re
import subprocess
import sys
from collections import deque

OBJDUMP = "arm-none-eabi-objdump"

FLASH_LO, FLASH_HI = 0x10000000, 0x20000000

# Symbols that are reachable from a core1 root but are known never to execute
# with flash locked out. Keep this list short and commented.
DEFAULT_ALLOW = [
    # usb_host core1_main calls this while configuring SysTick, before
    # enabling the MPU.
    "common_hal_mcu_processor_get_frequency",
    # picodvi Framebuffer_RP2040 core1_main calls these during startup,
    # before enabling the MPU.
    "dvi_register_irqs_this_core",
    "dvi_start",
    # The flash-resident print half of the RAM-resident __wrap_panic
    # (supervisor/port.c); only called after a get_core_num() == 0 check,
    # and core0 never locks flash out.
    "panic_core0",
]


def in_flash(addr):
    return FLASH_LO <= addr < FLASH_HI


def main():
    parser = argparse.ArgumentParser(
        description="Check that core1-reachable code is RAM-resident."
    )
    parser.add_argument("elf", help="linked firmware ELF")
    parser.add_argument(
        "--root",
        action="append",
        default=[],
        help="core1 entry point symbol (default: core1_main)",
    )
    parser.add_argument(
        "--allow",
        action="append",
        default=[],
        help="symbol to skip (e.g. runs before the MPU is enabled)",
    )
    args = parser.parse_args()
    roots = list(dict.fromkeys(args.root or ["core1_main"]))
    allow = set(DEFAULT_ALLOW) | set(args.allow)

    dis = subprocess.run(
        [OBJDUMP, "-d", args.elf], capture_output=True, text=True, check=True
    ).stdout

    func_re = re.compile(r"^([0-9a-f]+) <([^>]+)>:$")
    # e.g. "10001234:  f7ff fffe   bl  10005678 <foo>"
    branch_re = re.compile(
        r"^\s*[0-9a-f]+:\s+[0-9a-f ]+\t(bl|blx|b|b\.n|b\.w|"
        r"b(?:eq|ne|cs|cc|mi|pl|vs|vc|hi|ls|ge|lt|gt|le)(?:\.n|\.w)?)"
        r"\s+([0-9a-f]+)\s<([^>+]+)(\+0x[0-9a-f]+)?>"
    )
    indirect_re = re.compile(r"^\s*[0-9a-f]+:\s+[0-9a-f ]+\tblx\s+(r\d+|ip|lr)\b")
    # Veneer bodies jump through a literal pool word rather than a branch.
    word_re = re.compile(r"^\s*[0-9a-f]+:\s+([0-9a-f]{8})\s+\.word\s")

    funcs = {}  # name -> addr
    edges = {}  # name -> {target name}
    veneer_words = {}  # veneer name -> {literal addresses}
    indirects = {}  # name -> count of untraceable indirect calls
    cur = None
    for line in dis.splitlines():
        m = func_re.match(line)
        if m:
            cur = m.group(2)
            funcs[cur] = int(m.group(1), 16)
            edges.setdefault(cur, set())
            indirects.setdefault(cur, 0)
            continue
        if cur is None:
            continue
        if cur.endswith("_veneer"):
            m = word_re.match(line)
            if m:
                veneer_words.setdefault(cur, set()).add(int(m.group(1), 16) & ~1)
            continue
        if indirect_re.match(line):
            indirects[cur] += 1
            continue
        m = branch_re.match(line)
        if m:
            target = m.group(3)
            if target != cur:  # ignore intra-function branches
                edges[cur].add(target)

    # Resolve veneer literal addresses to function names.
    addr_to_name = {}
    for name, addr in funcs.items():
        addr_to_name.setdefault(addr, name)
    for veneer, words in veneer_words.items():
        for w in words:
            tgt = addr_to_name.get(w)
            if tgt is not None:
                edges.setdefault(veneer, set()).add(tgt)

    # A root may be absent from a given build (e.g. core1_scanline_callback
    # only exists on picodvi builds); check whichever roots are present.
    missing = [r for r in roots if r not in funcs]
    if missing:
        print(f"{args.elf}: root symbol(s) not present, skipping: {', '.join(missing)}")
    roots = [r for r in roots if r in funcs]
    if not roots:
        return 0

    # BFS from roots, remembering one call chain per function for reporting.
    parent = {r: None for r in roots}
    queue = deque(roots)
    seen = set(roots)
    violations = []
    indirect_notes = []
    while queue:
        fn = queue.popleft()
        if fn in allow:
            continue
        addr = funcs.get(fn)
        if addr is not None and in_flash(addr):
            violations.append(fn)
            continue  # don't walk further into flash
        if indirects.get(fn):
            indirect_notes.append((fn, indirects[fn]))
        for tgt in sorted(edges.get(fn, ())):
            if tgt not in seen and tgt in funcs:
                seen.add(tgt)
                parent[tgt] = fn
                queue.append(tgt)

    def chain(fn):
        parts = []
        while fn is not None:
            parts.append(fn)
            fn = parent[fn]
        return " <- ".join(parts)

    print(f"{args.elf}: walked {len(seen)} functions from roots: {', '.join(roots)}")
    if indirect_notes:
        print(
            f"note: {len(indirect_notes)} reachable function(s) make indirect "
            f"calls that cannot be traced statically:"
        )
        for fn, n in sorted(indirect_notes):
            print(f"  {fn} ({n} indirect call site(s))")
    if violations:
        print(f"\nFAIL: {len(violations)} flash-resident function(s) reachable from core1:")
        for fn in sorted(violations):
            print(f"  {fn} @ {funcs[fn]:#010x}")
            print(f"    via: {chain(fn)}")
        return 1
    print("PASS: no flash-resident code reachable from core1")
    return 0


if __name__ == "__main__":
    sys.exit(main())
