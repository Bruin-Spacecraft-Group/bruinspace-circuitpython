// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "psa/crypto.h"

typedef struct {
    mp_obj_base_t base;
    psa_hash_operation_t hash_op;
    psa_algorithm_t hash_alg;
} hashlib_hash_obj_t;
