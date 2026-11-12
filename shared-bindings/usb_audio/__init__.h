// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-module/usb_audio/__init__.h"

// The module globals dict is mutable so the USBMicrophone and USBSpeaker
// singleton instances can be installed at runtime (see usb_audio_setup_singletons).
extern mp_obj_dict_t usb_audio_module_globals;
