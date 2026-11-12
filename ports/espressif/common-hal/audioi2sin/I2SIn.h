// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"

#include "shared-module/audiocore/__init__.h"

#include "driver/i2s_std.h"

#if CIRCUITPY_AUDIOI2SIN

// Frames handed to the output backend per get_buffer() call; see the matching
// rp2 header for the rationale.
#define AUDIOI2SIN_STREAM_FRAMES (256)

typedef struct {
    // so I2SIn can be used directly as an audiosample source.
    audiosample_base_t base;
    i2s_chan_handle_t rx_chan;
    const mcu_pin_obj_t *bit_clock;
    const mcu_pin_obj_t *word_select;
    const mcu_pin_obj_t *data;
    const mcu_pin_obj_t *mclk;
    uint32_t sample_rate;
    uint8_t bit_depth;
    bool mono;
    bool samples_signed;
    // Owned double-buffer of converted (output-depth, interleaved) samples,
    // allocated lazily on first reset_buffer(). base.max_buffer_length is the
    // whole buffer; get_buffer() returns output_half_bytes of it.
    uint8_t *output_buffer;
    size_t output_half_bytes;
    uint8_t output_index;
} audioi2sin_i2sin_obj_t;

#endif
