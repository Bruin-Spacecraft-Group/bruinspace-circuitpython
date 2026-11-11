// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiodelays/GranularPitchShift.h"

extern const mp_obj_type_t audiodelays_granular_pitch_shift_type;

void common_hal_audiodelays_granular_pitch_shift_construct(audiodelays_granular_pitch_shift_obj_t *self,
    mp_obj_t semitones, mp_obj_t mix, uint32_t grain_size, uint32_t density,
    mp_float_t spread, uint32_t buffer_size, uint8_t bits_per_sample, bool samples_signed,
    uint8_t channel_count, uint32_t sample_rate);

void common_hal_audiodelays_granular_pitch_shift_deinit(audiodelays_granular_pitch_shift_obj_t *self);

mp_obj_t common_hal_audiodelays_granular_pitch_shift_get_semitones(audiodelays_granular_pitch_shift_obj_t *self);
void common_hal_audiodelays_granular_pitch_shift_set_semitones(audiodelays_granular_pitch_shift_obj_t *self, mp_obj_t semitones);

mp_obj_t common_hal_audiodelays_granular_pitch_shift_get_mix(audiodelays_granular_pitch_shift_obj_t *self);
void common_hal_audiodelays_granular_pitch_shift_set_mix(audiodelays_granular_pitch_shift_obj_t *self, mp_obj_t arg);

mp_float_t common_hal_audiodelays_granular_pitch_shift_get_spread(audiodelays_granular_pitch_shift_obj_t *self);
void common_hal_audiodelays_granular_pitch_shift_set_spread(audiodelays_granular_pitch_shift_obj_t *self, mp_float_t spread);

bool common_hal_audiodelays_granular_pitch_shift_get_playing(audiodelays_granular_pitch_shift_obj_t *self);
void common_hal_audiodelays_granular_pitch_shift_play(audiodelays_granular_pitch_shift_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiodelays_granular_pitch_shift_stop(audiodelays_granular_pitch_shift_obj_t *self);
