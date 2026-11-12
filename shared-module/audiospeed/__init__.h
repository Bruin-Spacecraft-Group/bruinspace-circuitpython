// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tod Kurt
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/objproperty.h"
#include "shared-module/audiocore/__init__.h"

// Fixed-point 16.16 format
#define SPEED_SHIFT 16
#define SPEED_DEFAULT (1 << SPEED_SHIFT) // default 1.0

typedef struct {
    // Phase accumulator and rate in 16.16 fixed-point (units: source frames)
    uint32_t phase;
    uint32_t rate_fp; // 16.16 fixed-point rate
} audiospeed_speed_t;

uint32_t audiospeed_rate_to_fp(mp_obj_t rate_obj);
mp_obj_t audiospeed_fp_to_rate(uint32_t rate_fp);

static inline void audiospeed_set_rate(audiospeed_speed_t *self, mp_obj_t rate_obj) {
    if (rate_obj != mp_const_none) {
        self->rate_fp = audiospeed_rate_to_fp(rate_obj);
    } else {
        self->rate_fp = SPEED_DEFAULT;
    }
}

static inline mp_obj_t audiospeed_get_rate(audiospeed_speed_t *self) {
    return audiospeed_fp_to_rate(self->rate_fp);
}

static inline void audiospeed_reset_phase(audiospeed_speed_t *self) {
    self->phase = 0;
}

static inline void audiospeed_increment_phase(audiospeed_speed_t *self) {
    self->phase += self->rate_fp;
}

static inline uint32_t audiospeed_get_index(audiospeed_speed_t *self) {
    return self->phase >> SPEED_SHIFT;
}

#define OUTPUT_BUFFER_FRAMES 128

typedef struct {
    audiosample_base_t base;
    mp_obj_t source;
    uint8_t *output_buffer;
    uint32_t output_buffer_length; // in bytes, allocated size
    // Source buffer cache
    uint8_t *src_buffer;
    uint32_t src_buffer_length; // in bytes
    uint32_t src_sample_count;  // in frames
    audiospeed_speed_t speed;
    bool source_done;  // source returned DONE on last get_buffer
    bool source_exhausted; // source DONE and we consumed all of it
} audiospeed_base_t;

void audiospeed_construct(audiospeed_base_t *self, mp_obj_t source, mp_obj_t rate_obj);
void audiospeed_deinit(audiospeed_base_t *self);
bool audiospeed_fetch_source_buffer(audiospeed_base_t *self);
void audiospeed_reset_buffer(audiospeed_base_t *self, bool single_channel_output, uint8_t channel);
audioio_get_buffer_result_t audiospeed_get_buffer(audiospeed_base_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length);
