# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries LLC
# SPDX-License-Identifier: MIT

"""Test rotaryio functionality on native_sim."""

import pytest


ROTARY_CODE_5S = """\
import time
import microcontroller
import rotaryio

encoder = rotaryio.IncrementalEncoder(microcontroller.pin.P_01, microcontroller.pin.P_02)

time.sleep(5.0)  # Sleep long enough for trace events to complete
print(f"position={encoder.position}")
print("done")
"""


ROTARY_CODE_7S = """\
import time
import microcontroller
import rotaryio

encoder = rotaryio.IncrementalEncoder(microcontroller.pin.P_01, microcontroller.pin.P_02)

time.sleep(7.0)  # Sleep long enough for trace events to complete
print(f"position={encoder.position}")
print("done")
"""


CLOCKWISE_TRACE = {
    "gpio_emul.01": [
        (4_000_000_000, 0),  # 4.0s: initial state (low)
        (4_100_000_000, 1),  # 4.1s: A goes high (A leads)
        (4_300_000_000, 0),  # 4.3s: A goes low
    ],
    "gpio_emul.02": [
        (4_000_000_000, 0),  # 4.0s: initial state (low)
        (4_200_000_000, 1),  # 4.2s: B goes high (B follows)
        (4_400_000_000, 0),  # 4.4s: B goes low
    ],
}

COUNTERCLOCKWISE_TRACE = {
    "gpio_emul.01": [
        (4_000_000_000, 0),  # 4.0s: initial state (low)
        (4_200_000_000, 1),  # 4.2s: A goes high (A follows)
        (4_400_000_000, 0),  # 4.4s: A goes low
    ],
    "gpio_emul.02": [
        (4_000_000_000, 0),  # 4.0s: initial state (low)
        (4_100_000_000, 1),  # 4.1s: B goes high (B leads)
        (4_300_000_000, 0),  # 4.3s: B goes low
    ],
}

BOTH_DIRECTIONS_TRACE = {
    "gpio_emul.01": [
        (4_000_000_000, 0),  # Initial state
        # First clockwise detent
        (4_100_000_000, 1),  # A rises (leads)
        (4_300_000_000, 0),  # A falls
        # Second clockwise detent
        (4_500_000_000, 1),  # A rises (leads)
        (4_700_000_000, 0),  # A falls
        # First counter-clockwise detent
        (5_000_000_000, 1),  # A rises (follows)
        (5_200_000_000, 0),  # A falls
        # Second counter-clockwise detent
        (5_400_000_000, 1),  # A rises (follows)
        (5_600_000_000, 0),  # A falls
        # Third counter-clockwise detent
        (5_800_000_000, 1),  # A rises (follows)
        (6_000_000_000, 0),  # A falls
    ],
    "gpio_emul.02": [
        (4_000_000_000, 0),  # Initial state
        # First clockwise detent
        (4_200_000_000, 1),  # B rises (follows)
        (4_400_000_000, 0),  # B falls
        # Second clockwise detent
        (4_600_000_000, 1),  # B rises (follows)
        (4_800_000_000, 0),  # B falls
        # First counter-clockwise detent
        (4_900_000_000, 1),  # B rises (leads)
        (5_100_000_000, 0),  # B falls
        # Second counter-clockwise detent
        (5_300_000_000, 1),  # B rises (leads)
        (5_500_000_000, 0),  # B falls
        # Third counter-clockwise detent
        (5_700_000_000, 1),  # B rises (leads)
        (5_900_000_000, 0),  # B falls
    ],
}


@pytest.mark.circuitpy_drive({"code.py": ROTARY_CODE_5S})
@pytest.mark.input_trace(CLOCKWISE_TRACE)
def test_rotaryio_incrementalencoder_clockwise(circuitpython):
    """Test clockwise rotation increments position."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "position=1" in output
    assert "done" in output


@pytest.mark.circuitpy_drive({"code.py": ROTARY_CODE_5S})
@pytest.mark.input_trace(COUNTERCLOCKWISE_TRACE)
def test_rotaryio_incrementalencoder_counterclockwise(circuitpython):
    """Test counter-clockwise rotation decrements position."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "position=-1" in output
    assert "done" in output


@pytest.mark.duration(12.0)
@pytest.mark.circuitpy_drive({"code.py": ROTARY_CODE_7S})
@pytest.mark.input_trace(BOTH_DIRECTIONS_TRACE)
def test_rotaryio_incrementalencoder_both_directions(circuitpython):
    """Test rotation in both directions: 2 clockwise, then 3 counter-clockwise."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "position=-1" in output
    assert "done" in output
