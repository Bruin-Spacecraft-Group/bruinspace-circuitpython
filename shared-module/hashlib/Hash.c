// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/hashlib/Hash.h"
#include "shared-module/hashlib/__init__.h"

#include "psa/crypto.h"

void common_hal_hashlib_hash_update(hashlib_hash_obj_t *self, const uint8_t *data, size_t datalen) {
    psa_hash_update(&self->hash_op, data, datalen);
}

void common_hal_hashlib_hash_digest(hashlib_hash_obj_t *self, uint8_t *data, size_t datalen) {
    if (datalen < common_hal_hashlib_hash_get_digest_size(self)) {
        return;
    }
    // Clone the operation so we can continue to update or get digest again.
    psa_hash_operation_t clone = PSA_HASH_OPERATION_INIT;
    psa_hash_clone(&self->hash_op, &clone);
    size_t hash_len;
    psa_hash_finish(&clone, data, datalen, &hash_len);
}

size_t common_hal_hashlib_hash_get_digest_size(hashlib_hash_obj_t *self) {
    return PSA_HASH_LENGTH(self->hash_alg);
}
