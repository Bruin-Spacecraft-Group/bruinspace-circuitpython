// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include "common-hal/frequencyio/FrequencyIn.h"
#include "shared-bindings/frequencyio/FrequencyIn.h"
#include "py/mpconfig.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/pin.h"

#include "timers.h"

// The HAL is sparse on obtaining register information, so we use the LLs here.
#if (CPY_STM32H7)
#include "stm32h7xx_ll_gpio.h"
#elif (CPY_STM32F7)
#include "stm32f7xx_ll_gpio.h"
#elif (CPY_STM32F4)
#include "stm32f4xx_ll_gpio.h"
#elif (CPY_STM32L4)
#include "stm32l4xx_ll_gpio.h"
#else
    #error unknown MCU for DigitalInOut
#endif

#define TIM_PERIOD 65535
#define STM32_GPIO_PORT_SIZE 16
static frequencyio_frequencyin_obj_t *callback_obj_ref[STM32_GPIO_PORT_SIZE];

void frequencyin_timer_event_handler(void) {
    // iterate through all object refs to find ours
    for (uint8_t i = 0; i < TIM_BANK_ARRAY_LEN; i++) {
        frequencyio_frequencyin_obj_t *self = callback_obj_ref[i];
        if (self == NULL) continue;

        if (__HAL_TIM_GET_FLAG(&htim, TIM_FLAG_CC1) != RESET &&
            __HAL_TIM_GET_IT_SOURCE(&htim, TIM_IT_CC1) != RESET) {

            uint32_t capture = HAL_TIM_ReadCapturedValue(&self->handle, self->tim_channel);

            // check for rising-edge
            if (self->is_first_capture){
                self->last_capture = capture;
                self->is_first_capture = false;
            } else { // falling edge, calculate frequency
                capture = HAL_TIM_ReadCapturedValue(&self->handle, self->tim_channel);

                uint32_t difference = capture - last_capture;
                // TODO: is there overflow? last_capture > capture?

                // freq is timer clock / (prescaler * difference)
                if (difference > 0) {
                    uint32_t timer_clock = stm_peripherals_timer_get_source_freq(self->handle.Instance);
                    uint32_t prescaler = self->handle.Init.Prescaler + 1; // prevent divide by zero
                    self->frequency = timer_clock/(prescaler * difference);
                }
            }

            // clear interrupt bc this is a custom ISR
            __HAL_TIM_CLEAR_IT(&self->handle, TIM_IT_CC1);
        }
    }
}

void common_hal_frequencyio_frequencyin_construct(frequencyio_frequencyin_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint16_t capture_period) {

    bool first_time_setup = true;

    uint8_t tim_index;
    uint8_t tim_channel_index;

    self->tim = NULL;
    for (uint i = 0; i < MP_ARRAY_SIZE(mcu_tim_pin_list); i++) {
        const mcu_tim_pin_obj_t *tim = &mcu_tim_pin_list[i];
        tim_index = tim->tim_index;
        tim_channel_index = tim->channel_index;

        // if pin is same
        if (tim->pin == pin) {
            // check if the timer has a channel active, or is reserved by main timer system
            if (tim_index < TIM_BANK_ARRAY_LEN && tim_channels_taken[tim_index] != 0) {
                // Timer has already been reserved by an internal module
                if (stm_peripherals_timer_is_reserved(mcu_tim_banks[tim_index])) {
                    continue; // keep looking
                }
                // is it the same channel? (or all channels reserved by a var-freq)
                if (tim_channels_taken[tim_index] & (1 << tim_channel_index)) {
                    continue; // keep looking, might be another viable option
                }

                first_time_setup = false; // skip setting up the timer
            }
            // No problems taken, so set it up
            self->tim = tim;
            break;
        }
    }

    TIM_TypeDef *TIMx;

    // handle valid/invalid timer instance
    if (self->tim != NULL) {
        // create instance
        TIMx = mcu_tim_banks[tim_index];

        tim_channels_taken[tim_index] |= 1 << tim_channel_index;
        stm_peripherals_timer_reserve(TIMx);
    } else {
        return;
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(pin->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;        // Alternate function
    GPIO_InitStruct.Pull = GPIO_NOPULL; // clean signal?
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = self->tim->altfn_index;  // Timer alternate function
    HAL_GPIO_Init(pin_port(pin->port), &GPIO_InitStruct);

    // Enable clocks and IRQ, set callback that updates frequency reading
    // TODO: Check priority
    stm_peripherals_timer_preinit(TIMx, 4, frequencyin_timer_event_handler);

    // translate channel into handle value: TIM_CHANNEL_1, _2, _3, _4.
    self->tim_channel = 4 * tim_channel_index;

    // Timer init
    self->handle.Instance = TIMx;
    self->handle.Init.Period = TIM_PERIOD;
    self->handle.Init.Prescaler = 0;
    self->handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    self->handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    self->handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (first_time_setup) {
        if (HAL_TIM_IC_Init(&self->handle) != HAL_OK) {
            return;
        }
    }

    // setting up input capture
    TIM_IC_InitTypeDef TIM_IC_InitStruct = {0}
    TIM_IC_InitStruct.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    TIM_IC_InitStruct.ICSelection = TIM_ICSELECTION_DIRECTTI;
    TIM_IC_InitStruct.ICPrescaler = TIM_ICPSC_DIV1;
    TIM_IC_InitStruct.ICFilter = 0;

    if(HAL_TIM_IC_ConfigChannel(&self->handle, TIM_IC_InitStruct, self->tim_channel) != HAL_OK) {
        return;
    }
    if(HAL_TIM_IC_Start(&self->handle, self->tim_channel) != HAL_OK){
        return;
    }

    // internal variables
    self->capture_period = capture_period;
    self->last_capture = 0;
    self->frequency = 0;
    self->is_first_capture = true;

    // store self for callback
    callback_obj_ref[pin->number] = self;
}

bool common_hal_frequencyio_frequencyin_deinited(frequencyio_frequencyin_obj_t *self) {
    return self->tim == NULL;
}

void common_hal_frequencyio_frequencyin_deinit(frequencyio_frequencyin_obj_t *self) {
    if(common_hal_frequencyio_frequencyin_deinited(self)) {
        return;
    }

    tim_channels_taken[self->tim->tim_index] &= ~(1 << self->tim->channel_index);
    HAL_TIM_IC_Stop(&self->handle, self->tim_channel);
    common_hal_reset_pin(self->pin);

    // if reserved timer has no active channels, we can disable it
    if (tim_channels_taken[self->tim->tim_index] == 0) {
        HAL_TIM_IC_DeInit(&self->handle);
        stm_peripherals_timer_free(self->handle.Instance);
    }

    self->tim = NULL;
}

void common_hal_frequencyio_frequencyin_pause(frequencyio_frequencyin_obj_t *self) {
    HAL_TIM_IC_Stop_IT(&self->tim_handle, self->tim_channel);
    self->paused = true;
}

void common_hal_frequencyio_frequencyin_resume(frequencyio_frequencyin_obj_t *self) {
    self->paused = false;
    self->is_first_capture = true;  // Reset measurement state
    HAL_TIM_IC_Start_IT(&self->tim_handle, self->tim_channel);
}

void common_hal_frequencyio_frequencyin_clear(frequencyio_frequencyin_obj_t* self){
    // concerned about race conditions?
    self->last_capture = 0;
    self->frequency = 0;
    self->is_first_capture = true;
}

uint16_t common_hal_frequencyio_frequencyin_get_capture_period(frequencyio_frequencyin_obj_t *self) {
    return self->capture_period;
}

void common_hal_frequencyio_frequencyin_set_capture_period(frequencyio_frequencyin_obj_t *self, uint16_t capture_period) {
    self->capture_period = capture_period;
}

uint32_t common_hal_frequencyio_frequencyin_get_item(frequencyio_frequencyin_obj_t *self){
    return self->frequency;
}
