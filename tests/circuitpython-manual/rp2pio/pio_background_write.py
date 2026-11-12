"""Baseline: rp2pio background_write DMA only (no audio).

Exercises the PIO *write* arm of the shared DMA_IRQ_0 handler, plus the
add/remove of the shared handler as the only channel comes and goes.

Repeatedly starts a one-shot background write and waits for it to finish by
polling `StateMachine.writing`. If `writing` never clears, the DMA completion
interrupt is not being serviced (a regression). Ends with a PASS line.

Wiring: PIO output on GP2 (no connection required).
"""

import array
import time

import board
import rp2pio

# PIO program (hand assembled, no adafruit_pioasm dependency):
#   .wrap_target
#       out pins, 1     ; shift one bit from OSR to the output pin
#   .wrap
# With auto_pull and pull_threshold=32, the OSR is refilled from the TX FIFO
# every 32 bits, so the FIFO (and therefore the DMA) drains continuously.
# Encoding of `out pins, 1`: opcode OUT=0b011, dest PINS=0b000, count 1 -> 0x6001
PROGRAM = array.array("H", [0x6001])

sm = rp2pio.StateMachine(
    PROGRAM,
    frequency=2_000_000,
    first_out_pin=board.GP2,
    auto_pull=True,
    pull_threshold=32,
    out_shift_right=True,
)

# 16 KB of data per transfer, enough that the DMA genuinely runs and completes
# via interrupt rather than fitting in the FIFO.
data = array.array("I", [0x5555AAAA] * 4096)

ITERATIONS = 50
for n in range(ITERATIONS):
    sm.background_write(once=data)
    deadline = time.monotonic() + 2.0
    while sm.writing and time.monotonic() < deadline:
        pass
    if sm.writing:
        raise RuntimeError(f"background_write stuck on iteration {n} (DMA IRQ not serviced?)")
    if n % 10 == 0:
        print("iteration", n, "ok")

sm.deinit()
print("PASS: rp2pio background_write completed", ITERATIONS, "times")
