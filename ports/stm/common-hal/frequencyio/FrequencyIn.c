// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include "common-hal/frequencyio/FrequencyIn.h"
#include "shared-bindings/frequencyio/FrequencyIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"
#include "supervisor/shared/tick.h"
#include STM32_HAL_H

#include "timers.h"

#define CHANNELS_PER_TIMER 4

// Channels claimed on each timer by THIS module. A nonzero entry means the timer is ours.
static uint8_t tim_channels_taken[TIM_BANK_ARRAY_LEN];
static frequencyio_frequencyin_obj_t *instances[TIM_BANK_ARRAY_LEN][CHANNELS_PER_TIMER];

// timers.h only gives us a void(void) callback, so one handler scans every timer we own.
static void frequencyin_timer_event_handler(void) {
    for (size_t t = 0; t < TIM_BANK_ARRAY_LEN; t++) {
        if (tim_channels_taken[t] == 0) {
            continue;
        }
        TIM_TypeDef *TIMx = mcu_tim_banks[t];
        for (size_t c = 0; c < CHANNELS_PER_TIMER; c++) {
            frequencyio_frequencyin_obj_t *self = instances[t][c];
            if (!self) {
                continue;
            }
            uint32_t flag = TIM_SR_CC1IF << c;
            if (!(TIMx->SR & flag) || !(TIMx->DIER & (TIM_DIER_CC1IE << c))) {
                continue;
            }
            // Read CCRx (this also clears CCxIF), then clear explicitly and drop any overcapture flag.
            uint32_t capture = HAL_TIM_ReadCapturedValue(&self->handle, self->tim_channel);
            TIMx->SR = ~(flag | (TIM_SR_CC1OF << c));

            if (self->have_last) {
                self->period_ticks = (capture - self->last_capture) & self->counter_mask;
            }
            self->last_capture = capture;
            self->have_last = true;
            self->last_edge_ms = supervisor_ticks_ms32();
        }
    }
}

void common_hal_frequencyio_frequencyin_construct(frequencyio_frequencyin_obj_t *self,
    const mcu_pin_obj_t *pin, uint16_t capture_period) {

    const mcu_tim_pin_obj_t *found = NULL;
    bool pin_has_timer = false;

    for (size_t i = 0; i < MP_ARRAY_SIZE(mcu_tim_pin_list); i++) {
        const mcu_tim_pin_obj_t *cand = &mcu_tim_pin_list[i];
        if (cand->pin != pin) {
            continue;
        }
        pin_has_timer = true;
        size_t idx = cand->tim_index;
        if (idx >= TIM_BANK_ARRAY_LEN) {
            continue;
        }
        if (tim_channels_taken[idx] == 0) {
            // Not ours yet: skip if anyone else (PWM, internal use) holds it.
            if (stm_peripherals_timer_is_reserved(mcu_tim_banks[idx])) {
                continue;
            }
        } else if (tim_channels_taken[idx] & (1 << cand->channel_index)) {
            continue; // this channel is already in use by us
        }
        found = cand;
        break;
    }
    if (!pin_has_timer) {
        raise_ValueError_invalid_pin();   // or your tree's equivalent
    }
    if (!found) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All timers in use"));
    }

    size_t tim_index = found->tim_index;
    uint8_t ch = found->channel_index;
    TIM_TypeDef *TIMx = mcu_tim_banks[tim_index];
    bool first_time_setup = (tim_channels_taken[tim_index] == 0);

    claim_pin(pin);
    self->pin = pin;
    self->tim = found;
    self->capture_period = capture_period;
    self->have_last = false;
    self->paused = false;
    self->period_ticks = 0;
    self->last_capture = 0;
    self->last_edge_ms = 0;

    if (first_time_setup) {
        stm_peripherals_timer_reserve(TIMx);
        // Presumably enables the RCC clock and the NVIC line.
        stm_peripherals_timer_preinit(TIMx, 7, frequencyin_timer_event_handler);
    }
    tim_channels_taken[tim_index] |= 1 << ch;
    instances[tim_index][ch] = self;

    // Counter width: use the HAL macro, not a CNT write test.
    #if defined(IS_TIM_32B_COUNTER_INSTANCE)
    self->counter_mask = IS_TIM_32B_COUNTER_INSTANCE(TIMx) ? 0xFFFFFFFFU : 0xFFFFU;
    #else
    self->counter_mask = 0xFFFFU;
    #endif

    uint32_t timer_clock = stm_peripherals_timer_get_source_freq(TIMx);

    self->handle.Instance = TIMx;
    if (first_time_setup) {
        // 32-bit: full resolution. 16-bit: ~1 us ticks (lowest measurable ~15 Hz).
        uint32_t psc = (self->counter_mask == 0xFFFFFFFFU) ? 0 : (timer_clock / 1000000U) - 1;
        self->handle.Init.Prescaler = psc;
        self->handle.Init.Period = self->counter_mask;
        self->handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        self->handle.Init.CounterMode = TIM_COUNTERMODE_UP;
        self->handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_IC_Init(&self->handle) != HAL_OK) {
            common_hal_frequencyio_frequencyin_deinit(self);
            mp_raise_RuntimeError(MP_ERROR_TEXT("Timer init failed"));   // string: check translations
        }
    } else {
        // Shared timer: adopt the live timebase, don't re-init (that would glitch other channels).
        self->handle.Init.Prescaler = TIMx->PSC;
        self->handle.Init.Period = TIMx->ARR;
    }
    self->tick_hz = timer_clock / (TIMx->PSC + 1);

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = pin_mask(pin->number);
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = found->altfn_index;
    HAL_GPIO_Init(pin_port(pin->port), &gpio);

    // HAL_TIM_IC_Start* is skipped on purpose: newer HALs refuse it on a fresh handle for a
    // shared timer. We enable the channel and interrupt directly below.
    self->tim_channel = TIM_CHANNEL_1 + 4 * ch;   // TIM_CHANNEL_x = 0, 4, 8, 12
    TIM_IC_InitTypeDef ic = {0};
    ic.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    ic.ICSelection = TIM_ICSELECTION_DIRECTTI;
    ic.ICPrescaler = TIM_ICPSC_DIV1;
    ic.ICFilter = 0;
    if (HAL_TIM_IC_ConfigChannel(&self->handle, &ic, self->tim_channel) != HAL_OK) {
        common_hal_frequencyio_frequencyin_deinit(self);
        mp_raise_RuntimeError(MP_ERROR_TEXT("Timer init failed"));
    }

    TIMx->SR = ~((TIM_SR_CC1IF | TIM_SR_CC1OF) << ch);
    TIMx->CCER |= TIM_CCER_CC1E << (4 * ch);
    TIMx->DIER |= TIM_DIER_CC1IE << ch;
    if (first_time_setup) {
        TIMx->CR1 |= TIM_CR1_CEN;
    }
}

bool common_hal_frequencyio_frequencyin_deinited(frequencyio_frequencyin_obj_t *self) {
    return self->tim == NULL;
}

void common_hal_frequencyio_frequencyin_deinit(frequencyio_frequencyin_obj_t *self) {
    if (common_hal_frequencyio_frequencyin_deinited(self)) {
        return;
    }
    size_t tim_index = self->tim->tim_index;
    uint8_t ch = self->tim->channel_index;
    TIM_TypeDef *TIMx = mcu_tim_banks[tim_index];

    TIMx->DIER &= ~(TIM_DIER_CC1IE << ch);
    TIMx->CCER &= ~(TIM_CCER_CC1E << (4 * ch));
    instances[tim_index][ch] = NULL;
    tim_channels_taken[tim_index] &= ~(1 << ch);

    if (tim_channels_taken[tim_index] == 0) {
        TIMx->CR1 &= ~TIM_CR1_CEN;
        HAL_TIM_IC_DeInit(&self->handle);
        stm_peripherals_timer_free(TIMx);
    }
    common_hal_reset_pin(self->pin);
    self->tim = NULL;
    self->pin = NULL;
}

void common_hal_frequencyio_frequencyin_pause(frequencyio_frequencyin_obj_t *self) {
    TIM_TypeDef *TIMx = self->handle.Instance;
    TIMx->DIER &= ~(TIM_DIER_CC1IE << self->tim->channel_index);
    self->paused = true;
}

void common_hal_frequencyio_frequencyin_resume(frequencyio_frequencyin_obj_t *self) {
    TIM_TypeDef *TIMx = self->handle.Instance;
    uint8_t ch = self->tim->channel_index;
    self->have_last = false;
    self->period_ticks = 0;
    self->last_edge_ms = supervisor_ticks_ms32();
    TIMx->SR = ~((TIM_SR_CC1IF | TIM_SR_CC1OF) << ch);
    TIMx->DIER |= TIM_DIER_CC1IE << ch;
    self->paused = false;
}

void common_hal_frequencyio_frequencyin_clear(frequencyio_frequencyin_obj_t *self) {
    self->have_last = false;
    self->period_ticks = 0;
}

uint16_t common_hal_frequencyio_frequencyin_get_capture_period(frequencyio_frequencyin_obj_t *self) {
    return self->capture_period;
}

void common_hal_frequencyio_frequencyin_set_capture_period(frequencyio_frequencyin_obj_t *self, uint16_t capture_period) {
    self->capture_period = capture_period;
}

uint32_t common_hal_frequencyio_frequencyin_get_item(frequencyio_frequencyin_obj_t *self) {
    uint32_t period = self->period_ticks;
    if (period == 0 || self->paused) {
        return 0;
    }
    // No edges within the window: report 0 instead of the stale value.
    if ((uint32_t)(supervisor_ticks_ms32() - self->last_edge_ms) > self->capture_period) {
        return 0;
    }
    return self->tick_hz / period;
}
