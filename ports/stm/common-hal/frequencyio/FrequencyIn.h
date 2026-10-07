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
    const mcu_tim_pin_obj_t *tim;
    TIM_HandleTypeDef handle;
    uint32_t tim_channel;          // HAL TIM_CHANNEL_x
    uint32_t tick_hz;              // timer clock / (PSC + 1)
    uint32_t counter_mask;         // 0xFFFF or 0xFFFFFFFF
    uint16_t capture_period;       // ms; frequency reads 0 after this long with no edges
    volatile uint32_t last_capture;
    volatile uint32_t period_ticks;
    volatile uint32_t last_edge_ms;
    volatile bool have_last;
    volatile bool paused;
} frequencyio_frequencyin_obj_t;
