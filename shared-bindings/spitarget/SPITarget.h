/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BUSIO_SPI_TARGET_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BUSIO_SPI_TARGET_H

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/spitarget/SPITarget.h"

extern const mp_obj_type_t spitarget_spi_target_type;

extern void common_hal_spitarget_spi_target_construct(spitarget_spi_target_obj_t *self,
    const mcu_pin_obj_t *sck, const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso, const mcu_pin_obj_t *ss);
extern void common_hal_spitarget_spi_target_deinit(spitarget_spi_target_obj_t *self);
extern bool common_hal_spitarget_spi_target_deinited(spitarget_spi_target_obj_t *self);

extern void common_hal_spitarget_spi_target_transfer_start(spitarget_spi_target_obj_t *self,
    uint8_t *mosi_packet, const uint8_t *miso_packet, size_t len);
extern bool common_hal_spitarget_spi_target_transfer_is_finished(spitarget_spi_target_obj_t *self);
extern int common_hal_spitarget_spi_target_transfer_close(spitarget_spi_target_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BUSIO_SPI_TARGET_H
