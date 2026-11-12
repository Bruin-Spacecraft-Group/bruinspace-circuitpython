// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Ikea Alpstuga Drop-In ESP32S3 4MB Flash 2MB PSRAM"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO33)

#define MICROPY_HW_LED_STATUS (&pin_GPIO9)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO4)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO3)

#define DOUBLE_TAP_PIN (&pin_GPIO34)
