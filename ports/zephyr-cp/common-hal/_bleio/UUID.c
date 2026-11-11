// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "shared-bindings/_bleio/UUID.h"

void common_hal_bleio_uuid_construct(bleio_uuid_obj_t *self, mp_int_t uuid16, const uint8_t uuid128[16]) {
    self->uuid16 = (uint16_t)uuid16;
    if (uuid128 == NULL) {
        // 16-bit UUID — only bytes 12-13 matter; rest is zero (matching ble_hci).
        self->size = BT_UUID_SIZE_16;
        memset(self->uuid128, 0, 16);
        self->uuid128[12] = uuid16 & 0xff;
        self->uuid128[13] = (uuid16 >> 8) & 0xff;
    } else {
        // 128-bit UUID — uuid128 has bytes 12-13 zeroed, uuid16 is extracted from them
        self->size = BT_UUID_SIZE_128;
        memcpy(self->uuid128, uuid128, 16);
        // Restore the 16-bit portion from uuid16 into bytes 12-13 (little-endian).
        self->uuid128[12] = uuid16 & 0xff;
        self->uuid128[13] = uuid16 >> 8;
    }
}

uint32_t common_hal_bleio_uuid_get_uuid16(bleio_uuid_obj_t *self) {
    return self->uuid16;
}

void common_hal_bleio_uuid_get_uuid128(bleio_uuid_obj_t *self, uint8_t uuid128[16]) {
    memcpy(uuid128, self->uuid128, 16);
}

uint32_t common_hal_bleio_uuid_get_size(bleio_uuid_obj_t *self) {
    return self->size == BT_UUID_SIZE_16 ? 16 : 128;
}

void common_hal_bleio_uuid_pack_into(bleio_uuid_obj_t *self, uint8_t *buf) {
    if (self->size == BT_UUID_SIZE_16) {
        buf[0] = self->uuid128[12];
        buf[1] = self->uuid128[13];
    } else {
        memcpy(buf, self->uuid128, 16);
    }
}
