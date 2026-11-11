// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/usb_audio/USBSpeaker.h"

extern const mp_obj_type_t usb_audio_USBSpeaker_type;

void common_hal_usb_audio_usbspeaker_construct(usb_audio_usbspeaker_obj_t *self);
void common_hal_usb_audio_usbspeaker_deinit(usb_audio_usbspeaker_obj_t *self);
bool common_hal_usb_audio_usbspeaker_deinited(usb_audio_usbspeaker_obj_t *self);
bool common_hal_usb_audio_usbspeaker_get_connected(usb_audio_usbspeaker_obj_t *self);
uint32_t common_hal_usb_audio_usbspeaker_read(usb_audio_usbspeaker_obj_t *self, void *buffer, uint32_t length);
