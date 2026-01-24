// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include STM32_HAL_H
#include "peripherals/periph.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;

    const mcu_pin_obj_t *pin;
    TIM_HandleTypeDef handle;
    const mcu_tim_pin_obj_t *tim;
    uint8_t tim_channel;

    uint16_t capture_period; // not implemented, directly reacts to rising/falling edges of signal

    volatile uint32_t last_capture;
    volatile uint32_t frequency;
    volatile bool rising_edge;
    volatile bool paused;
} frequencyio_frequencyin_obj_t;
