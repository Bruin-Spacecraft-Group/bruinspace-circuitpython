// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018-2019 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// mbedtls TLS and X.509 configuration, selected with MBEDTLS_CONFIG_FILE.
//
// As of mbedtls 4.0 this file covers only TLS and X.509. Everything cryptographic --
// algorithms, key types, the platform hooks and the RNG -- is configured in
// tf_psa_crypto_config.h next to this file, and selected with
// TF_PSA_CRYPTO_CONFIG_FILE.

#pragma once

// If you want to debug mbedtls, uncomment the following. SSLSocket.c raises the debug
// threshold to 4 when it is set.
// #define MBEDTLS_DEBUG_C

// Protocol versions

// TLS 1.0 and 1.1 were removed in mbedtls 3.0, and were obsolete long before that.
//
// TLS 1.3 is available in 4.x but is turned off, to match espressif
// It also cost 25648 bytes on Pico W, which has only ~50 KB of firmware
// space left. Enabling it here would also require enabling
// MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED, and PSA_WANT_ALG_HKDF* in
// tf_psa_crypto_config.h for the 1.3 key schedule.
#define MBEDTLS_SSL_PROTO_TLS1_2

// DTLS is deliberately off: common_hal_ssl_sslcontext_wrap_socket() rejects anything
// that is not SOCKETPOOL_SOCK_STREAM, so it could never be reached.

#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TLS_C

// Key exchanges. Without at least one of these there are no TLS 1.2 ciphersuites at
// all, the ClientHello offers nothing, and the server answers with a fatal
// handshake_failure alert. These two are what espressif enables
// (CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_{RSA,ECDSA}) and cover the public web. The PSK
// and ECJPAKE exchanges that 4.x also still offers are not reachable from the ssl
// module, and the static-RSA and DHE exchanges the mbedtls 2.28 config enabled are
// gone from 4.x upstream.
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED

// Extensions

#define MBEDTLS_SSL_SERVER_NAME_INDICATION
#define MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
#define MBEDTLS_SSL_ENCRYPT_THEN_MAC
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET

// Buffers

// Accept a full-size record inbound, since we do not control what the peer sends,
// but use a smaller outbound buffer to reduce the SSL context size.
#define MBEDTLS_SSL_MAX_CONTENT_LEN (16384)
#define MBEDTLS_SSL_IN_CONTENT_LEN  (MBEDTLS_SSL_MAX_CONTENT_LEN)
#define MBEDTLS_SSL_OUT_CONTENT_LEN (4096)

// X.509

#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_RSASSA_PSS_SUPPORT

// MBEDTLS_HAVE_TIME_DATE is deliberately left off, so certificate notBefore/notAfter
// are not checked (see the BADCERT_EXPIRED/BADCERT_FUTURE tests in x509_crt.c, which
// are compiled out without it). CircuitPython does not know the wall clock time unless
// the program sets it explicitly, which often does not happen; with an unset clock,
// checking the dates would reject valid certificates rather than catch expired ones.
// espressif does not set CONFIG_MBEDTLS_HAVE_TIME_DATE either.
//
// Nothing else we enable consumes time -- no session tickets, no context
// serialization, no DTLS, no TLS 1.3 -- so MBEDTLS_HAVE_TIME is off as well, and
// mbedtls_port.c needs neither a wall clock nor mbedtls_ms_time().

// Error strings

// SSLSocket.c keys off MBEDTLS_ERROR_C to decide whether to put a message on the
// OSError it raises. mbedtls's own error.c is not built; lib/mbedtls_errors supplies
// a smaller mbedtls_strerror() instead.
#define MBEDTLS_ERROR_C

// Sanity checks
//
// mbedtls's own mbedtls_check_config.h only validates the prerequisites of options
// that are enabled, so it says nothing when a whole category is missing. This is where
// TF_PSA_CRYPTO_CONFIG_FILE replacing psa/crypto_config.h rather than overlaying it
// bites: every default we rely on has to be restated here, and forgetting one produces
// a build that compiles and links but cannot complete a handshake.
#if !defined(MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED) &&   \
    !defined(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED) && \
    !defined(MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED) &&   \
    !defined(MBEDTLS_KEY_EXCHANGE_PSK_ENABLED) &&         \
    !defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
#error "No MBEDTLS_KEY_EXCHANGE_* enabled: ciphersuite_definitions[] would be empty, " \
    "so the ClientHello would offer nothing and every TLS 1.2 handshake would fail."
#endif
