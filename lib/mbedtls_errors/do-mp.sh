#! /bin/bash -e
# Generate mp_mbedtls_errors.c for inclusion in ports that use $MPY/lib/mbedtls
#
# As of mbedtls 4.0 the error #defines live in two trees: TLS and X.509 in
# mbedtls/include/mbedtls, and the legacy crypto modules in tf-psa-crypto, so
# generate_errors.pl now takes the crypto include directory as well.
patch -o mp_generate_errors.pl ../mbedtls/scripts/generate_errors.pl <generate_errors.diff
perl ./mp_generate_errors.pl \
    ../mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls \
    ../mbedtls/include/mbedtls \
    . mp_mbedtls_errors.c
# mp_generate_errors.pl emits "MODULE_A || \n" when a module is guarded by several defines,
# which leaves trailing whitespace that pre-commit would complaint about.
# So remove trailing spaces.
sed -i 's/[[:space:]]*$//' mp_mbedtls_errors.c
