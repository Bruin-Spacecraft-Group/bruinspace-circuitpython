// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "py/obj.h"

#include "shared-module/_bleio/Attribute.h"

extern void bleio_attribute_gatts_set_security_mode(ble_gap_conn_sec_mode_t *perm, bleio_attribute_security_mode_t security_mode);

size_t bleio_gatts_read(uint16_t handle, uint16_t conn_handle, uint8_t *buf, size_t len);
void bleio_gatts_write(uint16_t handle, uint16_t conn_handle, mp_buffer_info_t *bufinfo);
size_t bleio_gattc_read(uint16_t handle, uint16_t conn_handle, uint8_t *buf, size_t len);
void bleio_gattc_write(uint16_t handle, uint16_t conn_handle, mp_buffer_info_t *bufinfo, bool write_no_response);
