// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/usb_hid/__init__.h"
#include "shared-bindings/usb_hid/Device.h"
#include "shared-module/usb_hid/Device.h"
#include "supervisor/port.h"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/class/usbd_hid.h>

#define MAX_HID_DEVICES 8

static usb_hid_device_obj_t hid_devices[MAX_HID_DEVICES];
// If 0, USB HID is disabled.
static mp_int_t num_hid_devices;
// Which boot device is available? 0: no boot devices, 1: boot keyboard, 2: boot mouse.
static uint8_t hid_boot_device;
// Whether a boot device was requested by a SET_PROTOCOL request from the host.
static bool hid_boot_device_requested;
// This tuple is stored in usb_hid.devices.
static mp_obj_tuple_t *hid_devices_tuple;

// Whether the HID interface is ready (USB configured).
static bool hid_interface_ready;

static mp_obj_tuple_t default_hid_devices_tuple = {
    .base = {
        .type = &mp_type_tuple,
    },
    .len = 3,
    .items = {
        MP_OBJ_FROM_PTR(&usb_hid_device_keyboard_obj),
        MP_OBJ_FROM_PTR(&usb_hid_device_mouse_obj),
        MP_OBJ_FROM_PTR(&usb_hid_device_consumer_control_obj),
    },
};

// These describe the standard descriptors used for boot keyboard and mouse,
// which don't use report IDs.
static const usb_hid_device_obj_t boot_keyboard_obj = {
    .base = {
        .type = &usb_hid_device_type,
    },
    .report_descriptor = NULL,
    .report_descriptor_length = 0,
    .usage_page = 0x01,
    .usage = 0x06,
    .num_report_ids = 1,
    .report_ids = { 0, },
    .in_report_lengths = { 8, },
    .out_report_lengths = { 1, },
};

static const usb_hid_device_obj_t boot_mouse_obj = {
    .base = {
        .type = &usb_hid_device_type,
    },
    .report_descriptor = NULL,
    .report_descriptor_length = 0,
    .usage_page = 0x01,
    .usage = 0x02,
    .num_report_ids = 1,
    .report_ids = { 0, },
    .in_report_lengths = { 4, },
    .out_report_lengths = { 0, },
};

bool usb_hid_enabled(void) {
    return num_hid_devices > 0;
}

uint8_t usb_hid_boot_device(void) {
    return hid_boot_device;
}

// Returns 1 or 2 if host requested a boot device and boot protocol was enabled.
uint8_t common_hal_usb_hid_get_boot_device(void) {
    return hid_boot_device_requested ? hid_boot_device : 0;
}

void usb_hid_set_defaults(void) {
    hid_boot_device = 0;
    hid_boot_device_requested = false;
    hid_interface_ready = false;
    common_hal_usb_hid_enable(
        CIRCUITPY_USB_HID_ENABLED_DEFAULT ? &default_hid_devices_tuple : mp_const_empty_tuple, 0);
}

// Make up a fresh tuple containing the device objects saved in the static
// devices table. Save the tuple in usb_hid.devices.
static void usb_hid_set_devices_from_hid_devices(void) {
    mp_obj_t tuple_items[num_hid_devices];
    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        tuple_items[i] = &hid_devices[i];
    }
    hid_devices_tuple = mp_obj_new_tuple(num_hid_devices, tuple_items);
    usb_hid_set_devices(hid_devices_tuple);
}

bool common_hal_usb_hid_disable(void) {
    return common_hal_usb_hid_enable(mp_const_empty_tuple, 0);
}

bool common_hal_usb_hid_enable(const mp_obj_t devices, uint8_t boot_device) {
    // We can't change the devices once USB is connected.
    if (hid_interface_ready) {
        return false;
    }

    const mp_int_t num_devices = MP_OBJ_SMALL_INT_VALUE(mp_obj_len(devices));
    mp_arg_validate_length_max(num_devices, MAX_HID_DEVICES, MP_QSTR_devices);

    num_hid_devices = num_devices;
    hid_boot_device = boot_device;

    // Remember the devices in static storage so they live across VMs.
    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        usb_hid_device_obj_t *device =
            MP_OBJ_TO_PTR(mp_obj_subscr(devices, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL));
        memcpy(&hid_devices[i], device, sizeof(usb_hid_device_obj_t));
    }

    usb_hid_set_devices_from_hid_devices();

    return true;
}

// Called when HID devices are ready to be used, when code.py or the REPL starts running.
void usb_hid_setup_devices(void) {
    // If the host requested a boot device, replace the current list of devices
    // with a single-element tuple containing the proper boot device.
    if (hid_boot_device_requested) {
        memcpy(&hid_devices[0],
            hid_boot_device == 1 ? &boot_keyboard_obj : &boot_mouse_obj,
            sizeof(usb_hid_device_obj_t));
        num_hid_devices = 1;
    }

    usb_hid_set_devices_from_hid_devices();

    // Create report buffers on the heap.
    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        usb_hid_device_create_report_buffers(&hid_devices[i]);
    }
}

// Build the combined HID report descriptor from all devices.
static uint8_t *hid_report_descriptor = NULL;

size_t usb_hid_report_descriptor_length(void) {
    size_t total_hid_report_descriptor_length = 0;
    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        total_hid_report_descriptor_length += hid_devices[i].report_descriptor_length;
    }
    return total_hid_report_descriptor_length;
}

void usb_hid_build_report_descriptor(void) {
    if (!usb_hid_enabled()) {
        return;
    }
    size_t report_length = usb_hid_report_descriptor_length();
    hid_report_descriptor = port_malloc(report_length, false);
    if (hid_report_descriptor == NULL) {
        return;
    }

    uint8_t *report_descriptor_start = hid_report_descriptor;
    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        usb_hid_device_obj_t *device = &hid_devices[i];
        memcpy(report_descriptor_start, device->report_descriptor, device->report_descriptor_length);
        report_descriptor_start += device->report_descriptor_length;
        // Clear the heap pointer to the bytes of the descriptor.
        device->report_descriptor = NULL;
    }
}

void usb_hid_gc_collect(void) {
    gc_collect_ptr(hid_devices_tuple);

    for (mp_int_t device_idx = 0; device_idx < num_hid_devices; device_idx++) {
        gc_collect_ptr((void *)hid_devices[device_idx].report_descriptor);

        for (size_t id_idx = 0; id_idx < hid_devices[device_idx].num_report_ids; id_idx++) {
            gc_collect_ptr(hid_devices[device_idx].in_report_buffers[id_idx]);
            gc_collect_ptr(hid_devices[device_idx].out_report_buffers[id_idx]);
        }
    }
}

bool usb_hid_get_device_with_report_id(uint8_t report_id, usb_hid_device_obj_t **device_out, size_t *id_idx_out) {
    for (uint8_t device_idx = 0; device_idx < num_hid_devices; device_idx++) {
        usb_hid_device_obj_t *device = &hid_devices[device_idx];
        for (size_t id_idx = 0; id_idx < device->num_report_ids; id_idx++) {
            if (device->report_ids[id_idx] == report_id) {
                *device_out = device;
                *id_idx_out = id_idx;
                return true;
            }
        }
    }
    return false;
}

// ===== Zephyr USBD HID Device Callbacks =====

static void hid_iface_ready(const struct device *dev, const bool ready) {
    hid_interface_ready = ready;
}

static int hid_get_report(const struct device *dev,
    const uint8_t type, const uint8_t id,
    const uint16_t len, uint8_t *const buf) {
    // Support Input Report and Feature Report
    if (type != HID_REPORT_TYPE_INPUT && type != HID_REPORT_TYPE_FEATURE) {
        return -ENOTSUP;
    }

    usb_hid_device_obj_t *hid_device;
    size_t id_idx;
    if (!usb_hid_get_device_with_report_id(id, &hid_device, &id_idx)) {
        return 0;
    }
    if (!hid_device->in_report_buffers[id_idx]) {
        return 0;
    }

    uint8_t in_len = hid_device->in_report_lengths[id_idx];
    const uint8_t *src = hid_device->in_report_buffers[id_idx];

    // When the descriptor uses Report IDs (id != 0), the on-wire report is
    // prefixed with the Report ID byte. Zephyr sends the buffer verbatim, so
    // prepend the ID here. id == 0 means no Report ID item -> no prefix.
    if (id != 0) {
        if (len == 0) {
            return 0;
        }
        buf[0] = id;
        uint16_t copy_len = len - 1;
        if (copy_len > in_len) {
            copy_len = in_len;
        }
        memcpy(&buf[1], src, copy_len);
        return 1 + copy_len;
    }

    uint16_t copy_len = len;
    if (copy_len > in_len) {
        copy_len = in_len;
    }
    memcpy(buf, src, copy_len);
    return copy_len;
}

static int hid_set_report(const struct device *dev,
    const uint8_t type, const uint8_t id,
    const uint16_t len, const uint8_t *const buf) {
    uint8_t report_id = id;
    const uint8_t *data = buf;
    uint16_t data_len = len;

    if (report_id != 0 && len > 0) {
        // Descriptor uses Report IDs: the on-wire SET_REPORT data stage is
        // prefixed with the Report ID byte. Zephyr passes the raw payload, so
        // strip the leading ID byte (it should match the id from wValue).
        if (buf[0] == report_id) {
            data = &buf[1];
            data_len = len - 1;
        }
    } else if (report_id == 0 && len > 0) {
        // id==0 normally means no Report ID item (no prefix). But if no device
        // has report ID 0 with a matching out length, treat the first byte as a
        // report ID (host sent a prefixed report despite id==0).
        usb_hid_device_obj_t *hid0 = NULL;
        size_t idx0;
        if (!(usb_hid_get_device_with_report_id(0, &hid0, &idx0) &&
              hid0 && hid0->out_report_buffers[idx0] &&
              hid0->out_report_lengths[idx0] == len)) {
            report_id = buf[0];
            data = &buf[1];
            data_len = len - 1;
        }
    }

    // Look up the device for the resolved report_id.
    usb_hid_device_obj_t *hid_device = NULL;
    size_t id_idx;
    if (!usb_hid_get_device_with_report_id(report_id, &hid_device, &id_idx)) {
        return -EINVAL;
    }

    if (hid_device->out_report_buffers[id_idx] &&
        hid_device->out_report_lengths[id_idx] == data_len) {
        memcpy(hid_device->out_report_buffers[id_idx], data, data_len);
        hid_device->out_report_buffers_updated[id_idx] = true;
        return 0;
    }

    return -EINVAL;
}

static void hid_set_protocol(const struct device *dev, const uint8_t proto) {
    hid_boot_device_requested = (proto == HID_PROTOCOL_BOOT);
}

static struct hid_device_ops hid_ops = {
    .iface_ready = hid_iface_ready,
    .get_report = hid_get_report,
    .set_report = hid_set_report,
    .set_protocol = hid_set_protocol,
};

// ===== HID Device Registration =====

// Get the Zephyr HID device for submitting reports
static const struct device *hid_zephyr_dev;

const struct device *usb_hid_get_zephyr_device(void) {
    return hid_zephyr_dev;
}

void usb_hid_init_devices(void) {
    if (!usb_hid_enabled()) {
        return;
    }

    hid_zephyr_dev = DEVICE_DT_GET_ONE(zephyr_hid_device);
    if (!device_is_ready(hid_zephyr_dev)) {
        return;
    }

    usb_hid_build_report_descriptor();
    if (hid_report_descriptor == NULL) {
        return;
    }

    int ret = hid_device_register(hid_zephyr_dev,
        hid_report_descriptor,
        usb_hid_report_descriptor_length(),
        &hid_ops);
    if (ret != 0) {
        return;
    }
}

bool usb_hid_ready(void) {
    return hid_zephyr_dev != NULL && hid_interface_ready;
}

// Submit an input report through Zephyr's HID device.
// Called from common_hal_usb_hid_device_send_report().
bool usb_hid_submit_report(uint8_t report_id, const uint8_t *report, uint8_t len) {
    if (hid_zephyr_dev == NULL || !hid_interface_ready) {
        return false;
    }

    // Send report_id zero without a prefix
    if (report_id == 0) {
        return hid_device_submit_report(hid_zephyr_dev, len, report) == 0;
    }

    uint8_t __attribute__((aligned(sizeof(void *)))) report_buffer[1 + len];
    report_buffer[0] = report_id;
    memcpy(&report_buffer[1], report, len);
    return hid_device_submit_report(hid_zephyr_dev, len + 1, report_buffer) == 0;
}
