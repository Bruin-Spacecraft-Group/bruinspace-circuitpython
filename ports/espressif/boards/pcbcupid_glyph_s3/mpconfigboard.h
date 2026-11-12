// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 PCBCupid
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "PCBCupid GLYPH S3"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_LED_STATUS       (&pin_GPIO21)

#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO5)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO4)

#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO35)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO36)
#define DEFAULT_SPI_BUS_MISO        (&pin_GPIO37)

#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)
