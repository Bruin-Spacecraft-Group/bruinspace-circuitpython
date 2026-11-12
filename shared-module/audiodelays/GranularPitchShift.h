// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#pragma once

#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"
#include "shared-module/synthio/__init__.h"
#include "shared-module/synthio/block.h"

// Fixed-point fractional bits for grain read cursors and read rate. A grain's
// read_index and read_rate are stored as (words << GRANULAR_PITCH_READ_SHIFT),
// mirroring PITCH_READ_SHIFT from PitchShift.h.
#define GRANULAR_PITCH_READ_SHIFT (8)

// Maximum number of concurrently active grains. `density` is clamped to this at
// construction so the grain pool is a fixed, deterministic allocation.
#define GRANULAR_MAX_GRAINS (8)

extern const mp_obj_type_t audiodelays_granular_pitch_shift_type;

// A single grain: a short, independently-scheduled, enveloped read of the
// capture buffer. Retired when `phase` reaches `length`.
typedef struct {
    bool active;
    uint32_t read_index; // words << GRANULAR_PITCH_READ_SHIFT into the capture buffer
    uint32_t read_rate;  // words << GRANULAR_PITCH_READ_SHIFT, the pitch-shift increment
    uint32_t phase;      // samples elapsed since the grain launched
    uint32_t length;     // grain length in samples (== grain_size)
} audiodelays_granular_grain_t;

typedef struct {
    audiosample_base_t base;
    synthio_block_slot_t semitones;
    mp_float_t current_semitones;
    synthio_block_slot_t mix;

    // Double playback buffers (what we hand back to the audio component).
    int8_t *buffer[2];
    uint8_t last_buf_idx;
    uint32_t buffer_len; // max buffer in bytes

    // Current sample source bookkeeping.
    uint8_t *sample_remaining_buffer;
    uint32_t sample_buffer_length;

    bool loop;
    bool more_data;

    // Capture buffer (the delay line grains read from). Always stored as 16-bit
    // internally, planar per channel: channel c occupies
    // capture_buffer[c * capture_len .. (c + 1) * capture_len).
    int16_t *capture_buffer;
    uint32_t capture_len;  // words per channel
    uint32_t write_index;  // words, per-channel write cursor (0 .. capture_len)

    // Grain pool + scheduler.
    audiodelays_granular_grain_t grains[GRANULAR_MAX_GRAINS];
    uint32_t samples_until_next_grain;

    uint32_t grain_size; // samples per grain
    uint32_t density;    // number of overlapping grains (<= GRANULAR_MAX_GRAINS)

    // Granular jitter: randomizes each grain's start position within the capture
    // buffer. 0.0 is fully deterministic (grains always start grain_size words
    // behind the write cursor); 1.0 spreads the start up to a further grain_size
    // words backward, giving the classic granular "cloud" texture. Jitter is
    // always backward (further behind the write cursor) so it never reads ahead
    // of captured audio.
    mp_float_t spread;
    uint32_t rng_state; // xorshift32 state for grain-start jitter

    // Q15 (0..32768) normalization applied to the enveloped grain sum so the
    // overlap-add gain of `density` Hann grains stays ~unity (Hann satisfies
    // COLA at these hops with a summed gain of density/2, so the factor is
    // 2/density, capped at 1.0 so a single grain is never amplified).
    uint32_t grain_gain;

    // Precomputed amplitude envelope (raised-cosine / Hann), Q15 (0..32767),
    // indexed directly by a grain's phase (envelope_len == grain_size).
    int16_t *envelope_table;
    uint32_t envelope_len;

    // Fixed-point read-rate increment computed from `semitones`, applied to each
    // launched grain's read_rate.
    uint32_t read_rate; // words << GRANULAR_PITCH_READ_SHIFT

    mp_obj_t sample;
} audiodelays_granular_pitch_shift_obj_t;

void granular_pitch_shift_recalculate_rate(audiodelays_granular_pitch_shift_obj_t *self, mp_float_t semitones);

void audiodelays_granular_pitch_shift_reset_buffer(audiodelays_granular_pitch_shift_obj_t *self,
    bool single_channel_output,
    uint8_t channel);

audioio_get_buffer_result_t audiodelays_granular_pitch_shift_get_buffer(audiodelays_granular_pitch_shift_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);  // length in bytes
