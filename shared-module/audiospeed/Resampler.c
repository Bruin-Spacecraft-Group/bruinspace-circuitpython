// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/audiospeed/Resampler.h"

static void calculate_rate(audiospeed_base_t *self, uint32_t sample_rate) {
    if (self->source != NULL && sample_rate) {
        self->speed.rate_fp = (uint32_t)((mp_float_t)self->base.sample_rate / sample_rate * (1 << SPEED_SHIFT));
    } else {
        audiospeed_set_rate(&self->speed, mp_const_none);
    }
}

void common_hal_audiospeed_resampler_construct(audiospeed_resampler_obj_t *self, mp_obj_t source) {
    audiospeed_construct(&self->base, source, mp_const_none); // default rate 1.0
    self->sample_rate = 0;
}

void common_hal_audiospeed_resampler_deinit(audiospeed_resampler_obj_t *self) {
    audiospeed_deinit(&self->base);
}

mp_obj_t common_hal_audiospeed_resampler_get_rate(audiospeed_resampler_obj_t *self) {
    return audiospeed_get_rate(&self->base.speed);
}

void audiospeed_resampler_set_sample_rate(audiospeed_resampler_obj_t *self, uint32_t sample_rate) {
    self->sample_rate = sample_rate;
    calculate_rate(&self->base, self->sample_rate);
}

void audiospeed_resampler_reset_buffer(audiospeed_resampler_obj_t *self,
    bool single_channel_output, uint8_t channel) {
    audiospeed_reset_buffer(&self->base, single_channel_output, channel);
}

audioio_get_buffer_result_t audiospeed_resampler_get_buffer(audiospeed_resampler_obj_t *self,
    bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {
    return audiospeed_get_buffer(&self->base, single_channel_output, channel, buffer, buffer_length);
}
