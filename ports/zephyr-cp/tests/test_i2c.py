# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test I2C functionality on native_sim."""

import pytest

I2C_SCAN_CODE = """\
import board

i2c = board.I2C()
while not i2c.try_lock():
    pass
devices = i2c.scan()
print(f"I2C devices: {[hex(d) for d in devices]}")
i2c.unlock()
i2c.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": I2C_SCAN_CODE})
def test_i2c_scan(circuitpython):
    """Test I2C bus scanning finds emulated devices.

    The AT24 EEPROM emulator responds to zero-length probe writes,
    so it should appear in scan results at address 0x50.
    """
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "I2C devices:" in output
    # AT24 EEPROM should be at address 0x50
    assert "0x50" in output
    assert "done" in output


AT24_READ_CODE = """\
import board

i2c = board.I2C()
while not i2c.try_lock():
    pass

# AT24 EEPROM at address 0x50
AT24_ADDR = 0x50

# Read first byte from address 0
result = bytearray(1)
try:
    i2c.writeto_then_readfrom(AT24_ADDR, bytes([0x00]), result)
    value = result[0]
    print(f"AT24 byte 0: 0x{value:02X}")
    # Fresh EEPROM should be 0xFF
    if value == 0xFF:
        print("eeprom_valid")
    else:
        print(f"unexpected value: expected 0xFF, got 0x{value:02X}")
except OSError as e:
    print(f"I2C error: {e}")

i2c.unlock()
i2c.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": AT24_READ_CODE})
def test_i2c_at24_read(circuitpython):
    """Test reading from AT24 EEPROM emulator."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "AT24 byte 0: 0xFF" in output
    assert "eeprom_valid" in output
    assert "done" in output


@pytest.mark.circuitpy_drive({"code.py": I2C_SCAN_CODE})
@pytest.mark.disable_i2c_devices("eeprom@50")
def test_i2c_device_disabled(circuitpython):
    """Test that disabled I2C device doesn't appear in scan."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "I2C devices:" in output
    # AT24 at 0x50 should NOT appear when disabled
    assert "0x50" not in output
    assert "done" in output


@pytest.mark.circuitpy_drive({"code.py": AT24_READ_CODE})
@pytest.mark.disable_i2c_devices("eeprom@50")
def test_i2c_device_disabled_communication_fails(circuitpython):
    """Test that communication with disabled I2C device fails."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    # Should get an I2C error when trying to communicate
    assert "I2C error" in output
    assert "eeprom_valid" not in output
    assert "done" in output
