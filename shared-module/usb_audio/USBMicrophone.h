// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct usb_audio_usbmicrophone_obj {
    mp_obj_base_t base;
    // The audiosample currently bound for streaming, or MP_OBJ_NULL.
    mp_obj_t sample;
    // Cursor into the chunk last returned by audiosample_get_buffer(): the next
    // unread byte and the number of bytes still unread in that chunk.
    uint8_t *buffer;
    uint32_t buffer_length;
    bool loop;
    bool playing;
    bool paused;
    bool deinited;
    // False once audiosample_get_buffer() reported GET_BUFFER_DONE for the
    // current chunk; the chunk is still played out, then we loop or stop.
    bool more_data;
} usb_audio_usbmicrophone_obj_t;

// Pull up to max_bytes of audio (in the negotiated USB PCM format) from whichever
// USBMicrophone is currently playing, writing it into out. Returns the number of
// bytes written, which is < max_bytes (possibly 0) when nothing is streaming or
// the source underruns; the caller pads the remainder with silence. Called from
// the USB background task in shared-module/usb_audio/__init__.c.
size_t usb_audio_usbmicrophone_background_fill(uint8_t *out, size_t max_bytes);
