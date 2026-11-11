// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiodelays/Flanger.h"

extern const mp_obj_type_t audiodelays_flanger_type;

void common_hal_audiodelays_flanger_construct(audiodelays_flanger_obj_t *self, uint32_t max_delay_ms,
    mp_obj_t min_delay_ms, mp_obj_t rate, mp_obj_t depth, mp_obj_t feedback, mp_obj_t mix, bool invert,
    uint32_t buffer_size, uint8_t bits_per_sample,
    bool samples_signed, uint8_t channel_count, uint32_t sample_rate);

void common_hal_audiodelays_flanger_deinit(audiodelays_flanger_obj_t *self);
bool common_hal_audiodelays_flanger_deinited(audiodelays_flanger_obj_t *self);

mp_obj_t common_hal_audiodelays_flanger_get_min_delay_ms(audiodelays_flanger_obj_t *self);
void common_hal_audiodelays_flanger_set_min_delay_ms(audiodelays_flanger_obj_t *self, mp_obj_t min_delay_ms);

mp_obj_t common_hal_audiodelays_flanger_get_rate(audiodelays_flanger_obj_t *self);
void common_hal_audiodelays_flanger_set_rate(audiodelays_flanger_obj_t *self, mp_obj_t rate);

mp_obj_t common_hal_audiodelays_flanger_get_depth(audiodelays_flanger_obj_t *self);
void common_hal_audiodelays_flanger_set_depth(audiodelays_flanger_obj_t *self, mp_obj_t depth);

mp_obj_t common_hal_audiodelays_flanger_get_feedback(audiodelays_flanger_obj_t *self);
void common_hal_audiodelays_flanger_set_feedback(audiodelays_flanger_obj_t *self, mp_obj_t feedback);

mp_obj_t common_hal_audiodelays_flanger_get_mix(audiodelays_flanger_obj_t *self);
void common_hal_audiodelays_flanger_set_mix(audiodelays_flanger_obj_t *self, mp_obj_t arg);

mp_float_t common_hal_audiodelays_flanger_get_lfo_value(audiodelays_flanger_obj_t *self);

bool common_hal_audiodelays_flanger_get_invert(audiodelays_flanger_obj_t *self);
void common_hal_audiodelays_flanger_set_invert(audiodelays_flanger_obj_t *self, bool invert);

bool common_hal_audiodelays_flanger_get_playing(audiodelays_flanger_obj_t *self);
void common_hal_audiodelays_flanger_play(audiodelays_flanger_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiodelays_flanger_stop(audiodelays_flanger_obj_t *self);
