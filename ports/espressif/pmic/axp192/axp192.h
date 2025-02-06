// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Stephen Oliver
// SPDX-FileCopyrightText: Copyright (c) 2023 CDarius
//
// SPDX-License-Identifier: MIT


#pragma once

#include "shared-bindings/busio/I2C.h"

#define AXP192_I2C_ADDRESS 0x34

#define AXP192_INPUT_POWER_STATE 0x00
#define AXP192_INPUT_POWER_STATE_ACIN_IS_PRESENT 0b10000000
#define AXP192_INPUT_POWER_STATE_ACIN_AVAILABLE  0b01000000
#define AXP192_INPUT_POWER_STATE_VBUS_IS_PRESENT 0b00100000
#define AXP192_INPUT_POWER_STATE_VBUS_AVAILABLE  0b00010000
#define AXP192_INPUT_POWER_STATE_VBUS_GREATER_VHOLD_BEFORE_USE  0b00001000
#define AXP192_INPUT_POWER_STATE_BATT_CURRENT_DIR_IS_CHARGING  0b00000100
#define AXP192_INPUT_POWER_STATE_ACIN_VBUS_PCB_SHORTED  0b00000010
#define AXP192_INPUT_POWER_STATE_BOOT_SOURCE_ACIN_OR_VBUS  0b00000001

#define AXP192_DCDC13_LDO23_CTRL 0x12
#define AXP192_DCDC13_LDO23_CTRL_EXTEN 0b01000000
#define AXP192_DCDC13_LDO23_CTRL_DCDC2 0b00010000
#define AXP192_DCDC13_LDO23_CTRL_LDO3  0b00001000
#define AXP192_DCDC13_LDO23_CTRL_LDO2  0b00000100
#define AXP192_DCDC13_LDO23_CTRL_DCDC3 0b00000010
#define AXP192_DCDC13_LDO23_CTRL_DCDC1 0b00000001

#define AXP192_DCDC2_OUT_VOLTAGE 0x23

#define AXP192_DCDC1_OUT_VOLTAGE 0x26
#define AXP192_DCDC1_OUT_VOLTAGE_3_350V 0b01101010

#define AXP192_DCDC3_OUT_VOLTAGE 0x27
#define AXP192_DCDC3_OUT_VOLTAGE_3_0V 0b01011100

#define AXP192_LDO23_OUT_VOLTAGE 0x28
#define AXP192_LDO23_OUT_VOLTAGE_LDO2_3_3V 0b11110000
#define AXP192_LDO23_OUT_VOLTAGE_LDO2_3_0V 0b11000000
#define AXP192_LDO23_OUT_VOLTAGE_LDO2_2_8V 0b10100000
#define AXP192_LDO23_OUT_VOLTAGE_LDO2_MASK 0b11110000
#define AXP192_LDO23_OUT_VOLTAGE_LDO3_3_0V 0b00001100
#define AXP192_LDO23_OUT_VOLTAGE_LDO3_2_8V 0b00001010
#define AXP192_LDO23_OUT_VOLTAGE_LDO3_1_8V 0b00000000
#define AXP192_LDO23_OUT_VOLTAGE_LDO3_MASK 0b00001111

#define AXP192_VBUS_IPSOUT 0x30
#define AXP192_VBUS_IPSOUT_IGNORE_VBUSEN 0b10000000
#define AXP192_VBUS_IPSOUT_VHOLD_LIMIT 0b01000000
#define AXP192_VBUS_IPSOUT_VHOLD_VOLTAGE_4_4V 0b00100000
#define AXP192_VBUS_IPSOUT_VHOLD_VOLTAGE_MASK 0b00111000
#define AXP192_VBUS_IPSOUT_VBUS_LIMIT_CURRENT 0b00000010
#define AXP192_VBUS_IPSOUT_VBUS_LIMIT_CURRENT_100mA 0b00000001
#define AXP192_VBUS_IPSOUT_VBUS_LIMIT_CURRENT_500mA 0b00000000

#define AXP192_POWER_OFF_VOLTAGE 0x31
#define AXP192_POWER_OFF_VOLTAGE_2_6V 0b0000
#define AXP192_POWER_OFF_VOLTAGE_2_7V 0b0001
#define AXP192_POWER_OFF_VOLTAGE_2_8V 0b0010
#define AXP192_POWER_OFF_VOLTAGE_2_9V 0b0011
#define AXP192_POWER_OFF_VOLTAGE_3_0V 0b0100
#define AXP192_POWER_OFF_VOLTAGE_3_1V 0b0101
#define AXP192_POWER_OFF_VOLTAGE_3_2V 0b0110
#define AXP192_POWER_OFF_VOLTAGE_3_3V 0b0111
#define AXP192_POWER_OFF_VOLTAGE_MASK 0b0111

#define AXP192_POWER_OFF_BATT_CHGLED_CTRL 0x32
#define AXP192_POWER_OFF_BATT_CHGLED_CTRL_OFF 0b10000000
#define AXP192_POWER_OFF_BATT_CHGLED_CTRL_BATT_MONITOR_ON 0b01000000
#define AXP192_POWER_OFF_BATT_CHGLED_CTRL_N_OE_SHUTDOWN_DELAY_3_0S 0b00000011
#define AXP192_POWER_OFF_BATT_CHGLED_CTRL_N_OE_SHUTDOWN_DELAY_2_0S 0b00000010
#define AXP192_POWER_OFF_BATT_CHGLED_CTRL_N_OE_SHUTDOWN_DELAY_1_0S 0b00000001
#define AXP192_POWER_OFF_BATT_CHGLED_CTRL_N_OE_SHUTDOWN_DELAY_0_5S 0b00000000

#define AXP192_CHARGING_CTRL1 0x33
#define AXP192_CHARGING_CTRL1_ENABLE 0b10000000
#define AXP192_CHARGING_CTRL1_VOLTAGE_4_36V 0b01100000
#define AXP192_CHARGING_CTRL1_VOLTAGE_4_20V 0b01000000
#define AXP192_CHARGING_CTRL1_VOLTAGE_4_15V 0b00100000
#define AXP192_CHARGING_CTRL1_VOLTAGE_4_10V 0b00000000
#define AXP192_CHARGING_CTRL1_VOLTAGE_MASK 0b01100000
#define AXP192_CHARGING_CTRL1_CHARGING_THRESH_15PERC 0b00010000
#define AXP192_CHARGING_CTRL1_CHARGING_THRESH_10PERC 0b00000000
#define AXP192_CHARGING_CTRL1_CHARGING_THRESH_MASK 0b00010000
#define AXP192_CHARGING_CTRL1_CURRENT_100mA 0b00000000
#define AXP192_CHARGING_CTRL1_CURRENT_190mA 0b00000001
#define AXP192_CHARGING_CTRL1_CURRENT_280mA 0b00000010
#define AXP192_CHARGING_CTRL1_CURRENT_360mA 0b00000011
#define AXP192_CHARGING_CTRL1_CURRENT_450mA 0b00000100
#define AXP192_CHARGING_CTRL1_CURRENT_550mA 0b00000101
#define AXP192_CHARGING_CTRL1_CURRENT_630mA 0b00000110
#define AXP192_CHARGING_CTRL1_CURRENT_700mA 0b00000111
#define AXP192_CHARGING_CTRL1_CURRENT_780mA 0b00001000
#define AXP192_CHARGING_CTRL1_CURRENT_880mA 0b00001001
#define AXP192_CHARGING_CTRL1_CURRENT_960mA 0b00001010
#define AXP192_CHARGING_CTRL1_CURRENT_1000mA 0b00001011

#define AXP192_CHARGING_CTRL1_CURRENT_MASK 0b00001111

#define AXP192_CHARGING_CTRL2 0x34

#define AXP192_BACKUP_BATT 0x35
#define AXP192_BACKUP_BATT_CHARGING_ENABLE 0b10000000
#define AXP192_BACKUP_BATT_CHARGING_VOLTAGE_2_5V 0b01100000
#define AXP192_BACKUP_BATT_CHARGING_VOLTAGE_3_0V 0b00100000
#define AXP192_BACKUP_BATT_CHARGING_VOLTAGE_3_1V 0b00000000
#define AXP192_BACKUP_BATT_CHARGING_VOLTAGE_MASK 0b01100000
#define AXP192_BACKUP_BATT_CHARGING_CURRENT_400uA 0b00000011
#define AXP192_BACKUP_BATT_CHARGING_CURRENT_200uA 0b00000010
#define AXP192_BACKUP_BATT_CHARGING_CURRENT_100uA 0b00000001
#define AXP192_BACKUP_BATT_CHARGING_CURRENT_50uA 0b00000000
#define AXP192_BACKUP_BATT_CHARGING_CURRENT_MASK 0b00000011

#define AXP192_PEK 0x36
#define AXP192_PEK_SHORT_PRESS_1S 0b11000000
#define AXP192_PEK_SHORT_PRESS_512mS 0b10000000
#define AXP192_PEK_SHORT_PRESS_256mS 0b01000000
#define AXP192_PEK_SHORT_PRESS_128mS 0b00000000
#define AXP192_PEK_SHORT_PRESS_MASK 0b11000000
#define AXP192_PEK_LONG_PRESS_2_5S 0b00110000
#define AXP192_PEK_LONG_PRESS_2_0S 0b00100000
#define AXP192_PEK_LONG_PRESS_1_5S 0b00010000
#define AXP192_PEK_LONG_PRESS_1_0S 0b00000000
#define AXP192_PEK_LONG_PRESS_MASK 0b00110000
#define AXP192_PEK_LONG_PRESS_POWER_OFF 0b00001000
#define AXP192_PEK_PWROK_DELAY_64mS 0b00000100
#define AXP192_PEK_PWROK_DELAY_32mS 0b00000000
#define AXP192_PEK_PWROK_DELAY_MASK 0b00000100
#define AXP192_PEK_POWER_OFF_TIME_12S 0b00000011
#define AXP192_PEK_POWER_OFF_TIME_8S 0b00000010
#define AXP192_PEK_POWER_OFF_TIME_6S 0b00000001
#define AXP192_PEK_POWER_OFF_TIME_4S 0b00000000
#define AXP192_PEK_POWER_OFF_TIME_MASK 0b00000011

#define AXP192_BATT_TEMP_LOW_THRESH 0x38
#define AXP192_BATT_TEMP_HIGH_THRESH 0x39
#define AXP192_BATT_TEMP_HIGH_THRESH_DEFAULT 0b11111100

#define AXP192_APS_LOW_BATT_LEVEL_1 0x3A
#define AXP192_APS_LOW_BATT_LEVEL_2 0x3B
#define AXP192_APS_LOW_BATT_VOLTAGE_3_695V 0b10010100
#define AXP192_APS_LOW_BATT_VOLTAGE_3_600V 0b10000011

#define AXP192_IRQ_1_ENABLE 0x40
#define AXP192_IRQ_2_ENABLE 0x41
#define AXP192_IRQ_3_ENABLE 0x42
#define AXP192_IRQ_3_PEK_SHORT_PRESS 0b00000010
#define AXP192_IRQ_3_PEK_LONG_PRESS  0b00000001
#define AXP192_IRQ_4_ENABLE 0x43
#define AXP192_IRQ_4_LOW_VOLTAGE_WARNING 0b00000001
#define AXP192_IRQ_5_ENABLE 0x4a

#define AXP192_IRQ_X_DISABLE_ALL     0b00000000

#define AXP192_IRQ_1_STATUS 0x44
#define AXP192_IRQ_2_STATUS 0x45
#define AXP192_IRQ_3_STATUS 0x46
#define AXP192_IRQ_4_STATUS 0x47
#define AXP192_IRQ_5_STATUS 0x4d

#define AXP192_IRQ_X_CLEAR_STATUS 0b11111111

#define AXP192_ADC_ACIN_VOLTAGE_H 0x56
#define AXP192_ADC_ACIN_VOLTAGE_L 0x57
#define AXP192_ADC_ACIN_CURRENT_H 0x58
#define AXP192_ADC_ACIN_CURRENT_L 0x59
#define AXP192_ADC_VBUS_VOLTAGE_H 0x5a
#define AXP192_ADC_VBUS_VOLTAGE_L 0x5b
#define AXP192_ADC_VBUS_CURRENT_H 0x5c
#define AXP192_ADC_VBUS_CURRENT_L 0x5d
#define AXP192_ADC_INTERNAL_TEMP_H 0x5e
#define AXP192_ADC_INTERNAL_TEMP_L 0x5f

#define AXP192_ADC_BATT_VOLTAGE_H 0x78
#define AXP192_ADC_BATT_VOLTAGE_L 0x79

#define AXP192_ADC_BATT_POWER_H 0x70
#define AXP192_ADC_BATT_POWER_M 0x71
#define AXP192_ADC_BATT_POWER_L 0x72

#define AXP192_ADC_BATT_CHARGE_CURRENT_H 0x7a
#define AXP192_ADC_BATT_CHARGE_CURRENT_L 0x7b
#define AXP192_ADC_BATT_DISCHARGE_CURRENT_H 0x7c
#define AXP192_ADC_BATT_DISCHARGE_CURRENT_L 0x7d
#define AXP192_ADC_APS_VOLTAGE_H 0x7e
#define AXP192_ADC_APS_VOLTAGE_L 0x7f

#define AXP192_ADC_ENABLE_1 0x82
#define AXP192_ADC_ENABLE_1_BATT_VOL 0b10000000
#define AXP192_ADC_ENABLE_1_BATT_CUR 0b01000000
#define AXP192_ADC_ENABLE_1_ACIN_VOL 0b00100000
#define AXP192_ADC_ENABLE_1_ACIN_CUR 0b00010000
#define AXP192_ADC_ENABLE_1_VBUS_VOL 0b00001000
#define AXP192_ADC_ENABLE_1_VBUS_CUR 0b00000100
#define AXP192_ADC_ENABLE_1_APS_VOL 0b00000010
#define AXP192_ADC_ENABLE_1_TS_PIN 0b00000001

#define AXP192_ADC_ENABLE_2 0x83
#define AXP192_ADC_ENABLE_2_TEMP_MON 0b10000000
#define AXP192_ADC_ENABLE_2_GPIO0 0b00001000
#define AXP192_ADC_ENABLE_2_GPIO1 0b00000100
#define AXP192_ADC_ENABLE_2_GPIO2 0b00000010
#define AXP192_ADC_ENABLE_2_GPIO3 0b00000001

#define AXP192_ADC_TS 0x84
#define AXP192_ADC_TS_SAMPLE_200HZ 0b11000000
#define AXP192_ADC_TS_SAMPLE_100HZ 0b10000000
#define AXP192_ADC_TS_SAMPLE_50HZ 0b01000000
#define AXP192_ADC_TS_SAMPLE_25HZ 0b00000000
#define AXP192_ADC_TS_SAMPLE_MASK 0b11000000
#define AXP192_ADC_TS_OUT_CUR_80uA 0b00110000
#define AXP192_ADC_TS_OUT_CUR_60uA 0b00100000
#define AXP192_ADC_TS_OUT_CUR_40uA 0b00010000
#define AXP192_ADC_TS_OUT_CUR_20uA 0b00000000
#define AXP192_ADC_TS_OUT_CUR_MASK 0b00110000
#define AXP192_ADC_TS_PIN_TEMP_MON 0b00000000
#define AXP192_ADC_TS_PIN_EXTERN_ADC 0b00000100
#define AXP192_ADC_TS_PIN_OUT_ALWAYS 0b00000011
#define AXP192_ADC_TS_PIN_OUT_SAVE_ENG 0b00000010
#define AXP192_ADC_TS_PIN_OUT_CHG 0b00000001
#define AXP192_ADC_TS_PIN_OUT_DIS 0b00000000
#define AXP192_ADC_TS_PIN_OUT_MASK 0b00000011

#define AXP192_GPIO0_FUNCTION 0x90
#define AXP192_GPIO0_FUNCTION_MASK 0b00000111
#define AXP192_GPIO0_FUNCTION_FLOATING 0b00000111
#define AXP192_GPIO0_FUNCTION_LOW_OUTPUT 0b00000101
#define AXP192_GPIO0_FUNCTION_ADC_INPUT 0b00000100
#define AXP192_GPIO0_FUNCTION_LDO_OUTPUT 0b00000010
#define AXP192_GPIO0_FUNCTION_GENERAL_INPUT 0b00000001
#define AXP192_GPIO0_FUNCTION_OPEN_DRAIN_OUTPUT 0b00000000

#define AXP192_GPIO0_LDO_VOLTAGE 0x91
#define AXP192_GPIO0_LDO_VOLTAGE_MASK 0b11110000
#define AXP192_GPIO0_LDO_VOLTAGE_3_3V 0b11110000
#define AXP192_GPIO0_LDO_VOLTAGE_2_8V 0b10100000
#define AXP192_GPIO0_LDO_VOLTAGE_1_8V 0b00000000


#define AXP192_GPIO1_FUNCTION 0x92
#define AXP192_GPIO1_FUNCTION_FLOATING 0b00000111
#define AXP192_GPIO1_FUNCTION_LOW_OUTPUT 0b00000101
#define AXP192_GPIO1_FUNCTION_ADC_INPUT 0b00000100
#define AXP192_GPIO1_FUNCTION_PWM1_OUTPUT 0b00000010
#define AXP192_GPIO1_FUNCTION_GENERAL_INPUT 0b00000001
#define AXP192_GPIO1_FUNCTION_OPEN_DRAIN_OUTPUT 0b00000000


#define AXP192_GPIO2_FUNCTION 0x93
#define AXP192_GPIO2_FUNCTION_FLOATING 0b00000111
#define AXP192_GPIO2_FUNCTION_LOW_OUTPUT 0b00000101
#define AXP192_GPIO2_FUNCTION_ADC_INPUT 0b00000100
#define AXP192_GPIO2_FUNCTION_PWM2_OUTPUT 0b00000010
#define AXP192_GPIO2_FUNCTION_GENERAL_INPUT 0b00000001
#define AXP192_GPIO2_FUNCTION_OPEN_DRAIN_OUTPUT 0b00000000

#define AXP192_PWM1_OUTPUT_FREQUECY 0x98
#define AXP192_PWM1_DUTY_RATIO_Y1 0x99
#define AXP192_PWM1_DUTY_RATIO_Y2 0x9A

bool pmic_common_init(busio_i2c_obj_t *i2c);
bool pmic_disable_all_irq(busio_i2c_obj_t *i2c);
bool pmic_clear_all_irq(busio_i2c_obj_t *i2c);
bool pmic_enable_power_key_press_irq(busio_i2c_obj_t *i2c);
bool pmic_enable_low_battery_irq(busio_i2c_obj_t *i2c);
