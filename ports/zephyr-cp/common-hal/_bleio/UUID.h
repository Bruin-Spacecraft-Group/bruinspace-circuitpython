// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <zephyr/bluetooth/uuid.h>

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t uuid128[16];
    uint8_t size;  // BT_UUID_SIZE_16 (2) or BT_UUID_SIZE_128 (16)
    uint16_t uuid16;
} bleio_uuid_obj_t;
