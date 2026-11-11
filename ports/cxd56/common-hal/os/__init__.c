// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include <stdlib.h>

#include "genhdr/mpversion.h"
#include "py/objstr.h"
#include "py/objtuple.h"

// No HW TRNG.
bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length) {
    return false;
}
