// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include <zephyr/sys/ring_buffer.h>

#include "py/obj.h"
#include "shared-bindings/_bleio/Characteristic.h"

typedef struct {
    mp_obj_base_t base;
    bleio_characteristic_obj_t *characteristic;
    uint32_t timeout_ms;
    struct ring_buf ringbuf;
    uint8_t *ringbuf_data;
    bool watch_for_interrupt_char;
} bleio_characteristic_buffer_obj_t;

// Called from GATT callbacks (system workqueue context) to push
// data into the CharacteristicBuffer ring buffer.
void bleio_characteristic_buffer_extend(bleio_characteristic_buffer_obj_t *self,
    const uint8_t *data, size_t len);
