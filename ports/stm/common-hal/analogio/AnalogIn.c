// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/analogio/AnalogIn.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"

#if CPY_STM32L4
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_adc.h"
#include "stm32l4xx_ll_bus.h"
#define ADC_SAMPLETIME ADC_SAMPLETIME_24CYCLES_5
#define LL_APB2_GRP1_PERIPH_ADC1 LL_AHB2_GRP1_PERIPH_ADC

#elif CPY_STM32H7
#include "stm32h7xx_hal.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_adc.h"
#include "stm32h7xx_ll_bus.h"
#define ADC_SAMPLETIME ADC_SAMPLETIME_8CYCLES_5
#define LL_APB2_GRP1_PERIPH_ADC1 LL_APB2_GRP1_PERIPH_TIM1

#elif CPY_STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_bus.h"
#define ADC_SAMPLETIME ADC_SAMPLETIME_15CYCLES

#else
#error "Unsupported microcontroller series!"
#endif

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self,
    const mcu_pin_obj_t *pin) {

    // No ADC function on pin
    if (pin->adc_unit == 0x00) {
        raise_ValueError_invalid_pin();
    }
    // TODO: add ADC traits to structure?

    // Note that ADC2 is always bundled pin-to-pin with ADC1 if it exists, and used only
    // for dual conversion. For this basic application it is never used.
    LL_GPIO_SetPinMode(pin_port(pin->port), (uint32_t)pin_mask(pin->number), LL_GPIO_MODE_ANALOG);
    if (pin->adc_unit & 0x01) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    } else if (pin->adc_unit == 0x04) {
        #ifdef LL_APB2_GRP1_PERIPH_ADC3
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC3);
        #endif
    } else {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid ADC Unit value"));
    }
    common_hal_mcu_pin_claim(pin);
    self->pin = pin;
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin->port, self->pin->number);
    self->pin = NULL;
}

uint32_t adc_channel(uint32_t channel) {
    #if CPY_STM32L4
    switch (channel) {
        case 0:
            return ADC_CHANNEL_0;
        case 1:
            return ADC_CHANNEL_1;
        case 2:
            return ADC_CHANNEL_2;
        case 3:
            return ADC_CHANNEL_3;
        case 4:
            return ADC_CHANNEL_4;
        case 5:
            return ADC_CHANNEL_5;
        case 6:
            return ADC_CHANNEL_6;
        case 7:
            return ADC_CHANNEL_7;
        case 8:
            return ADC_CHANNEL_8;
        case 9:
            return ADC_CHANNEL_9;
        case 10:
            return ADC_CHANNEL_10;
        case 11:
            return ADC_CHANNEL_11;
        case 12:
            return ADC_CHANNEL_12;
        case 13:
            return ADC_CHANNEL_13;
        case 14:
            return ADC_CHANNEL_14;
        case 15:
            return ADC_CHANNEL_15;
        case 16:
            return ADC_CHANNEL_16;
        case 17:
            return ADC_CHANNEL_17;
        case 18:
            return ADC_CHANNEL_18;
        default:
            return 0;
    }
    #else
    return channel;
    #endif
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    // Something else might have used the ADC in a different way,
    // so we completely re-initialize it.
    ADC_TypeDef *ADCx;

    if (self->pin->adc_unit & 0x01) {
        ADCx = ADC1;
        #if CPY_STM32L4
        __HAL_RCC_ADC_CLK_ENABLE();
        #endif
    } else if (self->pin->adc_unit == 0x04) {
        #ifdef ADC3
        ADCx = ADC3;
        #endif
    } else {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid ADC Unit value"));
    }

    LL_GPIO_SetPinMode(pin_port(self->pin->port), (uint32_t)pin_mask(self->pin->number), LL_GPIO_MODE_ANALOG);
    // LL_GPIO_PIN_0

    // HAL Implementation
    ADC_HandleTypeDef hadc1 = {};
    ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_16B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.Oversampling.Ratio = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    return 0;
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    return 0;
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    return 0;
  }
    
    if (HAL_ADC_Start(&AdcHandle) != HAL_OK) {
        return 0;
    }
    HAL_ADC_PollForConversion(&AdcHandle, 1);
    uint16_t value = (uint16_t)HAL_ADC_GetValue(&AdcHandle);
    HAL_ADC_Stop(&AdcHandle);

    // Stretch 12-bit ADC reading to 16-bit range
    return (value << 4) | (value >> 8);
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 3.3f;
}
