// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiospeed/Resampler.h"

extern const mp_obj_type_t audiospeed_resampler_type;

void common_hal_audiospeed_resampler_construct(audiospeed_resampler_obj_t *self, mp_obj_t source);
void common_hal_audiospeed_resampler_deinit(audiospeed_resampler_obj_t *self);

mp_obj_t common_hal_audiospeed_resampler_get_rate(audiospeed_resampler_obj_t *self);

void audiospeed_resampler_set_sample_rate(audiospeed_resampler_obj_t *self, uint32_t sample_rate);
