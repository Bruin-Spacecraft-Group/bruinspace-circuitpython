// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/usb_hid/Device.h"
#include "supervisor/usb.h"

extern usb_hid_device_obj_t usb_hid_devices[];

bool usb_hid_enabled(void);
uint8_t usb_hid_boot_device(void);
void usb_hid_set_defaults(void);
void usb_hid_setup_devices(void);
size_t usb_hid_report_descriptor_length(void);
void usb_hid_build_report_descriptor(void);
bool usb_hid_get_device_with_report_id(uint8_t report_id, usb_hid_device_obj_t **device_out, size_t *id_idx_out);
void usb_hid_gc_collect(void);

// For supervisor/usb.c integration
void usb_hid_init_devices(void);

// For Device.c send_report
bool usb_hid_ready(void);
bool usb_hid_submit_report(uint8_t report_id, const uint8_t *report, uint8_t len);
