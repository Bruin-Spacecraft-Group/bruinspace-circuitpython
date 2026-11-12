// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Platform glue for ports that build the PSA crypto core for hashlib but not for ssl.
// The ssl equivalent is mbedtls_port.c; the two are mutually exclusive, because
// CIRCUITPY_HASHLIB_MBEDTLS_ONLY means hashlib without ssl.

#include <py/mpconfig.h>

#if CIRCUITPY_HASHLIB_MBEDTLS_ONLY

#include "psa/crypto.h"

#include "shared-bindings/os/__init__.h"

// Required by MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG. psa_crypto_init() initializes the RNG
// subsystem even in a build that only ever hashes, so this has to exist.
psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output, size_t output_size, size_t *output_length) {
    (void)context;
    if (!common_hal_os_urandom(output, output_size)) {
        return PSA_ERROR_INSUFFICIENT_ENTROPY;
    }
    *output_length = output_size;
    return PSA_SUCCESS;
}

#endif
