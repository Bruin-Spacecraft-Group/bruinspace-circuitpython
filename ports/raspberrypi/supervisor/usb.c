// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "lib/tinyusb/src/device/usbd.h"
#include "supervisor/background_callback.h"
#include "supervisor/linker.h"
#include "supervisor/usb.h"
#include "hardware/irq.h"
#include "hardware/timer.h"
#include "pico/platform.h"
#include "hardware/regs/intctrl.h"

void init_usb_hardware(void) {
}

// Override the shared implementation with one that is safe to call from core1.
// TinyUSB's tuh_task_event_ready() calls this, and usb_host polls
// tuh_task_event_ready() from core1's PIO-USB frame loop (see
// common-hal/usb_host/Port.c). tuh_task_event_ready() is deliberately placed in
// RAM by link-rp2*.ld, and everything it calls must be RAM-resident too: on
// RP2350, core1 sets an MPU region that makes flash execute-never, and on
// RP2040 flash may be unavailable while core0 writes to it. time_us_32() is a
// static-inline register read. >>10 yields ~1.024 ms units rather than exact
// milliseconds, which is fine because TinyUSB only uses this API for relative
// delay arithmetic and every use goes through this same function.
uint32_t PLACE_IN_ITCM(tusb_time_millis_api)(void) {
    return time_us_32() >> 10;
}

static void _usb_irq_wrapper(void) {
    usb_irq_handler(0);
}

void post_usb_init(void) {
    irq_add_shared_handler(USBCTRL_IRQ, _usb_irq_wrapper,
        PICO_SHARED_IRQ_HANDLER_LOWEST_ORDER_PRIORITY);

    // There is a small window where the USB interrupt may be handled by the
    // pico-sdk instead of CircuitPython. If that is the case, then we'll have
    // USB events to process that we didn't queue up a background task for. So,
    // queue one up here even if we might not have anything to do.
    usb_background_schedule();
}
