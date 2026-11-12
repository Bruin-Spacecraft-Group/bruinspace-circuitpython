// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/enum.h"

typedef enum {
    MCU_RESET_REASON_POWER_ON,
    MCU_RESET_REASON_BROWNOUT,
    MCU_RESET_REASON_SOFTWARE,
    MCU_RESET_REASON_DEEP_SLEEP_ALARM,
    MCU_RESET_REASON_RESET_PIN,
    MCU_RESET_REASON_WATCHDOG,
    MCU_RESET_REASON_UNKNOWN,
    MCU_RESET_REASON_RESCUE_DEBUG,
} mcu_reset_reason_t;

extern const mp_obj_type_t mcu_reset_reason_type;
