// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#pragma once

#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"
#include "shared-module/synthio/block.h"

extern const mp_obj_type_t audiodelays_flanger_type;

typedef struct {
    audiosample_base_t base;

    // Effect parameters.
    synthio_block_slot_t min_delay_ms; // floor of the sweep
    synthio_block_slot_t rate; // LFO frequency in Hz
    synthio_block_slot_t depth; // 0 to 1, portion of the available range swept
    synthio_block_slot_t feedback; // -0.95 to 0.95, regeneration
    synthio_block_slot_t mix; // 0 to 1, dry to wet

    uint32_t max_delay_ms;
    mp_float_t sample_ms; // length of a single sample in milliseconds

    bool invert; // subtract the wet signal instead of adding it

    int8_t *buffer[2];
    uint8_t last_buf_idx;
    uint32_t buffer_len; // max buffer in bytes

    uint8_t *sample_remaining_buffer;
    uint32_t sample_buffer_length;

    bool loop;
    bool more_data;

    // The delay line is always 16-bit and is split into one contiguous region per channel so
    // that read/write positions are plain frame counts and wrapping is a single modulo.
    int16_t *delay_buffer;
    uint32_t delay_buffer_frames; // frames in each per channel region
    uint32_t delay_buffer_pos[2]; // write position in frames, per channel

    // Internal LFO. One phase accumulator per channel so that the sweep advances once per frame
    // whether the channels are interleaved in one call or requested one channel at a time.
    uint32_t lfo_phase[2]; // Q0.32, wraps naturally
    uint32_t lfo_phase_inc; // Q0.32 step per frame, recalculated per chunk from rate

    mp_obj_t sample;
} audiodelays_flanger_obj_t;

void audiodelays_flanger_reset_buffer(audiodelays_flanger_obj_t *self,
    bool single_channel_output,
    uint8_t channel);

audioio_get_buffer_result_t audiodelays_flanger_get_buffer(audiodelays_flanger_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);  // length in bytes
