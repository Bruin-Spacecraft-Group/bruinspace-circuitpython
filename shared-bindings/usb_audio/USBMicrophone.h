// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/usb_audio/USBMicrophone.h"

extern const mp_obj_type_t usb_audio_USBMicrophone_type;

void common_hal_usb_audio_usbmicrophone_construct(usb_audio_usbmicrophone_obj_t *self);
void common_hal_usb_audio_usbmicrophone_deinit(usb_audio_usbmicrophone_obj_t *self);
bool common_hal_usb_audio_usbmicrophone_deinited(usb_audio_usbmicrophone_obj_t *self);
void common_hal_usb_audio_usbmicrophone_play(usb_audio_usbmicrophone_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_usb_audio_usbmicrophone_stop(usb_audio_usbmicrophone_obj_t *self);
bool common_hal_usb_audio_usbmicrophone_get_playing(usb_audio_usbmicrophone_obj_t *self);
void common_hal_usb_audio_usbmicrophone_pause(usb_audio_usbmicrophone_obj_t *self);
void common_hal_usb_audio_usbmicrophone_resume(usb_audio_usbmicrophone_obj_t *self);
bool common_hal_usb_audio_usbmicrophone_get_paused(usb_audio_usbmicrophone_obj_t *self);
