// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Standard HID report descriptors shared between ports. These are plain byte
// arrays with no dependencies, so ports using TinyUSB and ports using other
// USB stacks can both use them. They are defined in report_descriptors.c.
// The explicit sizes let callers use sizeof() and are checked by the compiler
// against the definitions.

#pragma once

#include <stdint.h>

extern const uint8_t keyboard_report_descriptor[67];
extern const uint8_t mouse_report_descriptor[64];
extern const uint8_t consumer_control_report_descriptor[25];
