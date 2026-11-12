"""Audio playback + rp2pio background_write sharing DMA_IRQ_0.

This is the scenario from issue #9868: audio output and a PIO peripheral (e.g. a
status NeoPixel) both want DMA completion interrupts at the same time. Audio
uses the interrupt-driven double-buffered path (`single_buffer=False`) and
rp2pio uses background_write, so both subsystems are serviced by DMA_IRQ_0
concurrently.

The test passes if the PIO writes keep completing AND audio keeps playing for
the whole run. A hang, crash, or audio stopping early indicates a regression.

Wiring: PWM audio on GP13 (speaker optional), PIO output on GP2 (no connection
required).
"""

import array
import math
import time

import audiocore
import audiopwmio
import board
import rp2pio

# --- Audio: signed-16 sine wave, double-buffered so it uses the DMA IRQ path.
SAMPLE_RATE = 16000
SAMPLE_COUNT = 4000  # even count (required for single_buffer=False), several periods
PERIODS = 32
sine = array.array("h", [0] * SAMPLE_COUNT)
for i in range(SAMPLE_COUNT):
    sine[i] = int(math.sin(2 * math.pi * PERIODS * i / SAMPLE_COUNT) * (2**14))
sample = audiocore.RawSample(sine, sample_rate=SAMPLE_RATE, single_buffer=False)

audio = audiopwmio.PWMAudioOut(board.GP13)
audio.play(sample, loop=True)
print("audio playing:", audio.playing)

# --- PIO: drain program that clocks the TX FIFO out on GP2 (see
# pio_background_write.py for the encoding).
PROGRAM = array.array("H", [0x6001])
sm = rp2pio.StateMachine(
    PROGRAM,
    frequency=2_000_000,
    first_out_pin=board.GP2,
    auto_pull=True,
    pull_threshold=32,
    out_shift_right=True,
)
data = array.array("I", [0x5555AAAA] * 4096)

ITERATIONS = 100
for n in range(ITERATIONS):
    sm.background_write(once=data)
    deadline = time.monotonic() + 2.0
    while sm.writing and time.monotonic() < deadline:
        pass
    if sm.writing:
        raise RuntimeError(f"background_write stuck on iteration {n} (shared DMA IRQ wedged?)")
    if not audio.playing:
        raise RuntimeError(f"audio stopped unexpectedly on iteration {n}")
    if n % 20 == 0:
        print("iteration", n, "audio.playing", audio.playing)

audio.stop()
sm.deinit()
audio.deinit()
print("PASS: audio + rp2pio background_write shared DMA_IRQ_0 for", ITERATIONS, "iterations")
