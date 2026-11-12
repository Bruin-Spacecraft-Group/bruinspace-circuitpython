// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tod Kurt
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/audiospeed/SpeedChanger.h"

#include <string.h>
#include "py/runtime.h"
#include "py/gc.h"

#include "shared-module/audiocore/__init__.h"
#include "shared-bindings/audiocore/__init__.h"

void common_hal_audiospeed_speedchanger_construct(audiospeed_speedchanger_obj_t *self,
    mp_obj_t source, mp_obj_t rate_obj) {
    audiospeed_construct(self, source, rate_obj);
}

void common_hal_audiospeed_speedchanger_deinit(audiospeed_speedchanger_obj_t *self) {
    audiospeed_deinit(self);
}

void common_hal_audiospeed_speedchanger_set_rate(audiospeed_speedchanger_obj_t *self, mp_obj_t rate_obj) {
    audiospeed_set_rate(&self->speed, rate_obj);
}

mp_obj_t common_hal_audiospeed_speedchanger_get_rate(audiospeed_speedchanger_obj_t *self) {
    return audiospeed_get_rate(&self->speed);
}

void audiospeed_speedchanger_reset_buffer(audiospeed_speedchanger_obj_t *self,
    bool single_channel_output, uint8_t channel) {
    audiospeed_reset_buffer(self, single_channel_output, channel);
}

audioio_get_buffer_result_t audiospeed_speedchanger_get_buffer(audiospeed_speedchanger_obj_t *self,
    bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {
    return audiospeed_get_buffer(self, single_channel_output, channel, buffer, buffer_length);
}
