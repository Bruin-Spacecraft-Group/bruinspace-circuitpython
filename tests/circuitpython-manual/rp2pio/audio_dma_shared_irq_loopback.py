"""Audio + rp2pio background write AND read sharing DMA_IRQ_0.

Exercises all three DMA_IRQ_0 dispatch arms at once:
  * audio playback (double-buffered, interrupt-driven),
  * a continuous PIO background_write (write-completion interrupts), and
  * repeated PIO background_read one-shots (read-completion interrupts).

The continuous background_write keeps the state machine clocking so the read
side always has data to capture. Each background_read is a one-shot whose
completion is detected by polling `StateMachine.reading`; if that never clears,
the read-completion interrupt is not being serviced (a regression). Audio must
also keep playing throughout.

Wiring: PWM audio on GP13 (speaker optional), PIO output on GP2, PIO input on
GP3. Jumper GP2 -> GP3 for a true loopback (then the captured data reflects what
was written); without the jumper the read still completes, so the DMA path is
still exercised.
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
SAMPLE_COUNT = 4000  # even count (required for single_buffer=False)
PERIODS = 32
sine = array.array("h", [0] * SAMPLE_COUNT)
for i in range(SAMPLE_COUNT):
    sine[i] = int(math.sin(2 * math.pi * PERIODS * i / SAMPLE_COUNT) * (2**14))
sample = audiocore.RawSample(sine, sample_rate=SAMPLE_RATE, single_buffer=False)

audio = audiopwmio.PWMAudioOut(board.GP13)
audio.play(sample, loop=True)
print("audio playing:", audio.playing)

# --- PIO loopback program (hand assembled):
#   .wrap_target
#       out pins, 1     ; 0x6001  shift one bit OSR -> output pin (GP2)
#       in  pins, 1     ; 0x4001  shift one bit input pin (GP3) -> ISR
#   .wrap
# auto_pull/auto_push at 32 bits keep both FIFOs (and their DMA) moving.
PROGRAM = array.array("H", [0x6001, 0x4001])
sm = rp2pio.StateMachine(
    PROGRAM,
    frequency=2_000_000,
    first_out_pin=board.GP2,
    first_in_pin=board.GP3,
    auto_pull=True,
    pull_threshold=32,
    out_shift_right=True,
    auto_push=True,
    push_threshold=32,
    in_shift_right=True,
)

out_buf = array.array("I", [0x0F0F0F0F] * 1024)
in_buf = array.array("I", [0] * 1024)

# Continuous background write to keep the state machine clocking.
sm.background_write(loop=out_buf)

ITERATIONS = 100
for n in range(ITERATIONS):
    sm.background_read(once=in_buf)
    deadline = time.monotonic() + 2.0
    while sm.reading and time.monotonic() < deadline:
        pass
    if sm.reading:
        raise RuntimeError(f"background_read stuck on iteration {n} (shared DMA IRQ wedged?)")
    if not audio.playing:
        raise RuntimeError(f"audio stopped unexpectedly on iteration {n}")
    if n % 20 == 0:
        print("iteration", n, "audio.playing", audio.playing, "first read word", hex(in_buf[0]))

sm.stop_background_write()
audio.stop()
sm.deinit()
audio.deinit()
print("PASS: audio + rp2pio background write/read shared DMA_IRQ_0 for", ITERATIONS, "iterations")
