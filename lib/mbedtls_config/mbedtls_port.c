// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018-2019 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries

#include "py/mpconfig.h"

#if CIRCUITPY_SSL_MBEDTLS

#include "psa/crypto.h"

#include "shared-bindings/os/__init__.h"

// The RNG behind MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG. mbedtls asks for randomness through
// this instead of seeding its own entropy accumulator and CTR-DRBG, so the port's TRNG
// is the only source. `context` is unused; mbedtls initializes it to 0 and we keep no
// state of our own.
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
