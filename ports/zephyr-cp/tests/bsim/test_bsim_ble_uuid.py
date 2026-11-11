# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""BLE UUID tests for bsim."""

import pytest


# --- 16-bit UUIDs ---

BSIM_UUID_16BIT_CODE = """\
import _bleio

u16 = _bleio.UUID(0x180F)
print("uuid16", u16.uuid16)
print("size", u16.size)
print("str", str(u16))
print("repr", repr(u16))
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_16BIT_CODE})
def test_bsim_uuid_16bit_basic(bsim_phy, circuitpython):
    """Construct a 16-bit UUID and verify all properties."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "uuid16 6159" in output  # 0x180F = 6159
    assert "size 16" in output
    assert "UUID(0x180f)" in output
    assert "done" in output


BSIM_UUID_16BIT_EQ_CODE = """\
import _bleio

a = _bleio.UUID(0x180F)
b = _bleio.UUID(0x180F)
c = _bleio.UUID(0x180D)

print("eq_same", a == b)
print("eq_diff", a == c)
print("neq_diff", a != c)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_16BIT_EQ_CODE})
def test_bsim_uuid_16bit_equality(bsim_phy, circuitpython):
    """16-bit UUID equality: same value is equal, different values are not."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "eq_same True" in output
    assert "eq_diff False" in output
    assert "neq_diff True" in output
    assert "done" in output


BSIM_UUID_16BIT_HASH_CODE = """\
import _bleio

a = _bleio.UUID(0x180F)
b = _bleio.UUID(0x180F)

# Same UUID should hash the same
print("hash_same", hash(a) == hash(b))

# Can be used as dict keys
d = {a: "battery"}
print("dict_lookup", d[_bleio.UUID(0x180F)])
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_16BIT_HASH_CODE})
def test_bsim_uuid_16bit_hash(bsim_phy, circuitpython):
    """16-bit UUID can be hashed and used as dict keys."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "hash_same True" in output
    assert "dict_lookup battery" in output
    assert "done" in output


BSIM_UUID_16BIT_PACK_CODE = """\
import _bleio

u16 = _bleio.UUID(0x180F)
buf = bytearray(4)
u16.pack_into(buf)
print("packed", list(buf[:2]))
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_16BIT_PACK_CODE})
def test_bsim_uuid_16bit_pack_into(bsim_phy, circuitpython):
    """pack_into for 16-bit UUID writes 2 bytes in little-endian."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    # 0x180F in little-endian: byte[0]=0x0F, byte[1]=0x18
    assert "packed [15, 24]" in output  # 0x0F=15, 0x18=24
    assert "done" in output


BSIM_UUID_16BIT_PACK_OFFSET_CODE = """\
import _bleio

u16 = _bleio.UUID(0x2A19)
buf = bytearray(6)
u16.pack_into(buf, offset=4)
print("packed", list(buf))
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_16BIT_PACK_OFFSET_CODE})
def test_bsim_uuid_16bit_pack_into_offset(bsim_phy, circuitpython):
    """pack_into with offset writes at the correct position."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    # 0x2A19 = 10777, little-endian: [0x19, 0x2A] = [25, 42]
    assert "packed [0, 0, 0, 0, 25, 42]" in output
    assert "done" in output


# --- 128-bit UUID string parsing ---

BSIM_UUID_128BIT_STR_CODE = """\
import _bleio

u128 = _bleio.UUID("12345678-1234-5678-1234-56789abcdef0")
print("uuid16", u128.uuid16)
print("size", u128.size)
print("uuid128_len", len(u128.uuid128))
# Bytes 12-13 are zeroed by shared-bindings (extracted as uuid16)
# First 4 bytes of uuid128 are [f0, de, bc, 9a] in LE
print("bytes_0_3", list(u128.uuid128[:4]))
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_128BIT_STR_CODE})
def test_bsim_uuid_128bit_string(bsim_phy, circuitpython):
    """Construct a 128-bit UUID from a hex string."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "size 128" in output
    # uuid16 extracted from bytes 12-13: 0x5678 = 22136
    assert "uuid16 22136" in output
    assert "uuid128_len 16" in output
    # Bytes 12-13 are restored from uuid16 by common_hal_bleio_uuid_construct
    # First 4 bytes of uuid128 are [f0, de, bc, 9a] in LE
    assert "bytes_0_3 [240, 222, 188, 154]" in output
    assert "done" in output


# --- 128-bit UUID bytes construction ---

BSIM_UUID_128BIT_BYTES_CODE = """\
import _bleio

raw = bytes([0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
             0x34, 0x12, 0x78, 0x56, 0x34, 0x12, 0x78, 0x56])
u128 = _bleio.UUID(raw)
print("size", u128.size)
# uuid16 extracted from raw[12:14] = [0x34, 0x12] → 0x1234 = 4660
print("uuid16", u128.uuid16)
print("uuid128_len", len(u128.uuid128))
# Bytes 12-13 are restored from uuid16 by common_hal_bleio_uuid_construct
print("bytes_12_13", list(u128.uuid128[12:14]))
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_128BIT_BYTES_CODE})
def test_bsim_uuid_128bit_bytes(bsim_phy, circuitpython):
    """Construct a 128-bit UUID from a 16-byte buffer."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "size 128" in output
    assert "uuid16 4660" in output
    assert "uuid128_len 16" in output
    # Bytes 12-13 restored from uuid16: 0x1234 → [0x34, 0x12] = [52, 18]
    assert "bytes_12_13 [52, 18]" in output
    assert "done" in output


# --- 128-bit UUID equality ---

BSIM_UUID_128BIT_EQ_CODE = """\
import _bleio

a = _bleio.UUID("12345678-1234-5678-1234-56789abcdef0")
b = _bleio.UUID("12345678-1234-5678-1234-56789abcdef0")
c = _bleio.UUID("00000000-0000-1000-8000-00805f9b34fb")

print("eq_same", a == b)
print("eq_diff", a == c)
print("neq_diff", a != c)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_128BIT_EQ_CODE})
def test_bsim_uuid_128bit_equality(bsim_phy, circuitpython):
    """128-bit UUID equality: same bytes equal, different not."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "eq_same True" in output
    assert "eq_diff False" in output
    assert "neq_diff True" in output
    assert "done" in output


# --- 16-bit vs 128-bit UUID equality (should not be equal even if same value) ---

BSIM_UUID_CROSS_SIZE_EQ_CODE = """\
import _bleio

# 0x180F as 16-bit
u16 = _bleio.UUID(0x180F)

# 0x180F expanded to 128-bit base UUID
# Bluetooth base: 00000000-0000-1000-8000-00805F9B34FB
# With 0x180F:     0000180F-0000-1000-8000-00805F9B34FB
u128 = _bleio.UUID("0000180f-0000-1000-8000-00805f9b34fb")

print("size_16", u16.size)
print("size_128", u128.size)
# Per spec, 16-bit and 128-bit are NOT equal even if values match
print("cross_eq", u16 == u128)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_CROSS_SIZE_EQ_CODE})
def test_bsim_uuid_cross_size_equality(bsim_phy, circuitpython):
    """16-bit and 128-bit UUIDs are not equal even with same value."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "size_16 16" in output
    assert "size_128 128" in output
    assert "cross_eq False" in output
    assert "done" in output


# --- Invalid UUID string ---

BSIM_UUID_INVALID_STR_CODE = """\
import _bleio

try:
    u = _bleio.UUID("not-a-uuid")
    print("should not reach")
except ValueError as e:
    print("value_error", "not" in str(e) or "UUID" in str(e))
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_INVALID_STR_CODE})
def test_bsim_uuid_invalid_string(bsim_phy, circuitpython):
    """Invalid UUID string raises ValueError."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "value_error True" in output
    assert "done" in output


# --- UUID 128-bit: uuid128 property works, uuid16 returns 16-bit part ---

BSIM_UUID_128BIT_PROPS_CODE = """\
import _bleio

u128 = _bleio.UUID("12345678-1234-5678-1234-56789abcdef0")
print("size", u128.size)
# uuid128 returns 16 bytes (bytes 12-13 are restored from uuid16)
b = u128.uuid128
print("uuid128_len", len(b))
# uuid16 is the 16-bit part from bytes 12-13, extracted and then restored
print("uuid16", u128.uuid16)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_128BIT_PROPS_CODE})
def test_bsim_uuid_128bit_properties(bsim_phy, circuitpython):
    """128-bit UUID exposes uuid128 bytes and uuid16 from bytes 12-13."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "size 128" in output
    assert "uuid128_len 16" in output
    assert "uuid16 22136" in output
    assert "done" in output


# --- 16-bit uuid128 raises AttributeError ---

BSIM_UUID_16BIT_NO_UUID128_CODE = """\
import _bleio

u16 = _bleio.UUID(0x180F)
try:
    _ = u16.uuid128
    print("should not reach")
except AttributeError:
    print("attr_error ok")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_UUID_16BIT_NO_UUID128_CODE})
def test_bsim_uuid_16bit_no_uuid128(bsim_phy, circuitpython):
    """Accessing uuid128 on a 16-bit UUID raises AttributeError."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "attr_error ok" in output
    assert "done" in output
