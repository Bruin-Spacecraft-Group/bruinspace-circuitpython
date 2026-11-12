// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

// Rescale an already-normalized signed sample and write it to `buffer` at sample
// index `idx`. `s` is the sign-extended int32 value of the sample at `in_depth`
// scale (the per-port `i2sin_normalize_signed` produces it from the raw FIFO
// word, handling wire-format details like left-justification). This helper does
// the depth conversion + unsigned-WAV flip + container store, which is identical
// across ports.
//
// This path runs only when the caller set `output_bit_depth`, i.e. asked for a
// real bit-depth rescale: upscaling bit-replicates so full-scale input maps to
// full-scale output (e.g. 16-bit 0xFFFF -> 24-bit 0xFFFFFF) and downscaling
// arithmetic-shifts right. This is distinct from the implicit container width
// set by the array typecode (e.g. a 24-bit sample carried in a 32-bit 'i' slot),
// which only sign-extends and is handled by the default (non-converting) path.
// Output element size follows `out_depth`: 1 byte at 8, 2 bytes at 16, 4 bytes
// at 24 or 32.
static inline void shared_audioi2sin_write_converted(void *buffer, uint32_t idx,
    int32_t s, uint8_t in_depth, uint8_t out_depth, bool samples_signed) {
    int32_t shifted;
    if (out_depth > in_depth) {
        // Explicit upscale: left-justify the in_depth-bit sample at the top of a
        // 32-bit word and replicate the pattern downward so full-scale input
        // maps to full-scale output (e.g. 16-bit 0xFFFF -> 24-bit 0xFFFFFF). Two
        // steps cover every supported ratio (widest is 8 -> 32); the guard skips
        // the second step (and the resulting shift-by->=32 UB) when one suffices.
        uint32_t top = (uint32_t)s << (32 - in_depth);
        top |= top >> in_depth;
        if (in_depth * 2 < out_depth) {
            top |= top >> (in_depth * 2);
        }
        // Arithmetic-shift the high out_depth bits down, sign-extending to int32
        // so a 24-bit value in a 32-bit slot decodes correctly (no-op at 32).
        shifted = (int32_t)top >> (32 - out_depth);
    } else if (out_depth == in_depth) {
        shifted = s;
    } else {
        shifted = s >> (in_depth - out_depth);
    }
    uint32_t u = (uint32_t)shifted;
    if (!samples_signed) {
        // I2S delivers signed PCM; flip the sign bit to match the WAV unsigned
        // convention, at the output bit width.
        if (out_depth >= 32) {
            u ^= 0x80000000u;
        } else {
            uint32_t mask = (1u << out_depth) - 1u;
            u = (u & mask) ^ (1u << (out_depth - 1));
        }
    }
    switch (out_depth) {
        case 8:
            ((uint8_t *)buffer)[idx] = (uint8_t)(u & 0xffu);
            break;
        case 16:
            ((uint16_t *)buffer)[idx] = (uint16_t)(u & 0xffffu);
            break;
        default: // 24 or 32
            ((uint32_t *)buffer)[idx] = u;
            break;
    }
}
