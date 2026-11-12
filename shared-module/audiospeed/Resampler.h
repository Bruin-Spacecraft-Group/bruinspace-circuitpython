// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-module/audiocore/__init__.h"
#include "shared-module/audiospeed/__init__.h"

typedef struct {
    audiospeed_base_t base;
    uint32_t sample_rate;
} audiospeed_resampler_obj_t;

void audiospeed_resampler_reset_buffer(audiospeed_resampler_obj_t *self,
    bool single_channel_output, uint8_t channel);
audioio_get_buffer_result_t audiospeed_resampler_get_buffer(audiospeed_resampler_obj_t *self,
    bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length);
