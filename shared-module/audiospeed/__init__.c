// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tod Kurt
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/gc.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-module/audiocore/__init__.h"
#include "shared-module/audiospeed/__init__.h"
#include "shared-bindings/audiocore/__init__.h"

// Convert a Python float to 16.16 fixed-point rate
uint32_t audiospeed_rate_to_fp(mp_obj_t rate_obj) {
    mp_float_t rate = mp_arg_validate_obj_float_range(rate_obj, 0.001, 1000.0, MP_QSTR_rate);
    return (uint32_t)(rate * (1 << SPEED_SHIFT));
}

// Convert 16.16 fixed-point rate to Python float
mp_obj_t audiospeed_fp_to_rate(uint32_t rate_fp) {
    return mp_obj_new_float((mp_float_t)rate_fp / (1 << SPEED_SHIFT));
}

void audiospeed_construct(audiospeed_base_t *self, mp_obj_t source, mp_obj_t rate_obj) {
    audiosample_base_t *src_base = audiosample_check(source);

    self->source = source;
    self->src_buffer = NULL;
    self->src_buffer_length = 0;
    self->src_sample_count = 0;
    self->source_done = false;
    self->source_exhausted = false;


    // Copy format from source
    self->base.sample_rate = src_base->sample_rate;
    self->base.channel_count = src_base->channel_count;
    self->base.bits_per_sample = src_base->bits_per_sample;
    self->base.samples_signed = src_base->samples_signed;
    self->base.single_buffer = false;

    audiospeed_set_rate(&self->speed, rate_obj);
    audiospeed_reset_phase(&self->speed);

    uint8_t bytes_per_frame = (src_base->bits_per_sample / 8) * src_base->channel_count;
    self->output_buffer_length = OUTPUT_BUFFER_FRAMES * bytes_per_frame;
    self->base.max_buffer_length = self->output_buffer_length;

    self->output_buffer = m_malloc_without_collect(self->output_buffer_length);
    if (self->output_buffer == NULL) {
        m_malloc_fail(self->output_buffer_length);
    }
}

void audiospeed_deinit(audiospeed_base_t *self) {
    self->output_buffer = NULL;
    self->source = MP_OBJ_NULL;
    audiosample_mark_deinit(&self->base);
}

// Fetch the next buffer from the source. Returns false if no data available.
bool audiospeed_fetch_source_buffer(audiospeed_base_t *self) {
    if (self->source_exhausted || self->source == NULL) {
        return false;
    }
    uint8_t *buf = NULL;
    uint32_t len = 0;
    audioio_get_buffer_result_t result = audiosample_get_buffer(self->source, false, 0, &buf, &len);
    if (result == GET_BUFFER_ERROR) {
        self->source_exhausted = true;
        return false;
    }
    if (len == 0) {
        self->source_exhausted = true;
        return false;
    }
    self->src_buffer = buf;
    self->src_buffer_length = len;
    uint8_t bytes_per_frame = (self->base.bits_per_sample / 8) * self->base.channel_count;
    self->src_sample_count = len / bytes_per_frame;
    self->source_done = (result == GET_BUFFER_DONE);
    // Reset phase to index within this new buffer
    audiospeed_reset_phase(&self->speed);
    return true;
}

void audiospeed_reset_buffer(audiospeed_base_t *self, bool single_channel_output, uint8_t channel) {
    if (single_channel_output && channel == 1) {
        return;
    }
    audiosample_reset_buffer(self->source, false, 0);
    audiospeed_reset_phase(&self->speed);
    self->src_buffer = NULL;
    self->src_buffer_length = 0;
    self->src_sample_count = 0;
    self->source_done = false;
    self->source_exhausted = false;
}

audioio_get_buffer_result_t audiospeed_get_buffer(audiospeed_base_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {

    // Ensure we have a source buffer
    if (self->src_buffer == NULL) {
        if (!audiospeed_fetch_source_buffer(self)) {
            *buffer = NULL;
            *buffer_length = 0;
            return GET_BUFFER_DONE;
        }
    }

    uint8_t bytes_per_sample = self->base.bits_per_sample / 8;
    uint8_t channels = self->base.channel_count;
    uint8_t bytes_per_frame = bytes_per_sample * channels;
    uint32_t out_frames = 0;
    uint32_t max_out_frames = self->output_buffer_length / bytes_per_frame;

    if (bytes_per_sample == 1) {
        // 8-bit samples
        uint8_t *out = self->output_buffer;
        while (out_frames < max_out_frames) {
            uint32_t src_index = audiospeed_get_index(&self->speed);
            // Advance to next source buffer if needed
            if (src_index >= self->src_sample_count) {
                if (self->source_done) {
                    self->source_exhausted = true;
                    break;
                }
                if (!audiospeed_fetch_source_buffer(self)) {
                    break;
                }
                src_index = 0; // phase was reset by fetch
            }
            uint8_t *src = self->src_buffer + src_index * bytes_per_frame;
            for (uint8_t c = 0; c < channels; c++) {
                *out++ = src[c];
            }
            out_frames++;
            audiospeed_increment_phase(&self->speed);
        }
    } else {
        // 16-bit samples
        int16_t *out = (int16_t *)self->output_buffer;
        while (out_frames < max_out_frames) {
            uint32_t src_index = audiospeed_get_index(&self->speed);
            if (src_index >= self->src_sample_count) {
                if (self->source_done) {
                    self->source_exhausted = true;
                    break;
                }
                if (!audiospeed_fetch_source_buffer(self)) {
                    break;
                }
                src_index = 0;
            }
            int16_t *src = (int16_t *)(self->src_buffer + src_index * bytes_per_frame);
            for (uint8_t c = 0; c < channels; c++) {
                *out++ = src[c];
            }
            out_frames++;
            audiospeed_increment_phase(&self->speed);
        }
    }

    *buffer = self->output_buffer;
    *buffer_length = out_frames * bytes_per_frame;

    if (out_frames == 0) {
        return GET_BUFFER_DONE;
    }
    return self->source_exhausted ? GET_BUFFER_DONE : GET_BUFFER_MORE_DATA;
}
