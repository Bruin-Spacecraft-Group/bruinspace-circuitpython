# rp2pio + audio DMA shared-IRQ manual tests

These programs exercise the `DMA_IRQ_0` interrupt handlers on the RP2040 /
RP2350, which are shared between `audiocore`/`audiopwmio` (audio DMA) and
`rp2pio` (PIO background read/write DMA).

They were written to validate the change in issue #9992 (moving `audio_dma.c`
off the fixed `isr_dma_0()` linker symbol and onto `irq_add_shared_handler()`,
with `rp2pio` registering its own shared handler). **They use only public APIs
that predate that change, so each program should behave identically before and
after it** — that is the point: they are regression tests, not feature tests.

## What each program covers

| Program | Audio DMA | PIO write DMA | PIO read DMA |
|---|---|---|---|
| `pio_background_write.py`        |     | ✓ |   |
| `audio_dma_shared_irq_write.py`  | ✓   | ✓ |   |
| `audio_dma_shared_irq_loopback.py` | ✓ | ✓ | ✓ |

Each goes through the background-DMA path (`background_write` /
`background_read`), which is interrupt-driven. The blocking `write`/`readinto`/
`write_readinto` calls poll instead and do **not** use the DMA IRQ, so they are
deliberately not used here. The audio sample is created with
`single_buffer=False` so audio also uses the interrupt-driven (double-buffered)
DMA path rather than the no-interrupt single-buffer chaining path.

## Wiring (Raspberry Pi Pico / Pico 2)

- `GP13` — PWM audio output. Connect to an amplifier/speaker, or just leave it;
  the test does not require you to hear anything, only that playback keeps
  running.
- `GP2`  — PIO output pin.
- `GP3`  — PIO input pin (loopback test only). For a meaningful loopback,
  connect `GP2` to `GP3` with a jumper. Without the jumper the read still
  completes (it just reads whatever the floating/pulled pin sees), so the DMA
  path is still exercised.

## Running

Copy one file at a time to `CIRCUITPY/code.py` and watch the serial console.
A successful run ends with a `PASS:` line. A hang (no further output) or a
`RuntimeError`/crash/safe-mode indicates a regression in the shared DMA IRQ
handling.
