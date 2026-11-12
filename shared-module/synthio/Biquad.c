// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <math.h>
#include "shared-bindings/synthio/Biquad.h"
#include "shared-bindings/synthio/BlockBiquad.h"
#include "shared-module/synthio/Biquad.h"

mp_obj_t common_hal_synthio_new_lpf(mp_float_t w0, mp_float_t Q) {
    mp_float_t s = MICROPY_FLOAT_C_FUN(sin)(w0);
    mp_float_t c = MICROPY_FLOAT_C_FUN(cos)(w0);
    mp_float_t alpha = s / (2 * Q);
    mp_float_t a0 = 1 + alpha;
    mp_float_t a1 = -2 * c;
    mp_float_t a2 = 1 - alpha;
    mp_float_t b0 = (1 - c) / 2;
    mp_float_t b1 = 1 - c;
    mp_float_t b2 = (1 - c) / 2;

    mp_obj_t out_args[] = {
        mp_obj_new_float(a1 / a0),
        mp_obj_new_float(a2 / a0),
        mp_obj_new_float(b0 / a0),
        mp_obj_new_float(b1 / a0),
        mp_obj_new_float(b2 / a0),
    };

    return namedtuple_make_new((const mp_obj_type_t *)&synthio_biquad_type_obj, MP_ARRAY_SIZE(out_args), 0, out_args);
}

mp_obj_t common_hal_synthio_new_hpf(mp_float_t w0, mp_float_t Q) {
    mp_float_t s = MICROPY_FLOAT_C_FUN(sin)(w0);
    mp_float_t c = MICROPY_FLOAT_C_FUN(cos)(w0);
    mp_float_t alpha = s / (2 * Q);
    mp_float_t a0 = 1 + alpha;
    mp_float_t a1 = -2 * c;
    mp_float_t a2 = 1 - alpha;
    mp_float_t b0 = (1 + c) / 2;
    mp_float_t b1 = -(1 + c);
    mp_float_t b2 = (1 + c) / 2;

    mp_obj_t out_args[] = {
        mp_obj_new_float(a1 / a0),
        mp_obj_new_float(a2 / a0),
        mp_obj_new_float(b0 / a0),
        mp_obj_new_float(b1 / a0),
        mp_obj_new_float(b2 / a0),
    };

    return namedtuple_make_new((const mp_obj_type_t *)&synthio_biquad_type_obj, MP_ARRAY_SIZE(out_args), 0, out_args);
}

mp_obj_t common_hal_synthio_new_bpf(mp_float_t w0, mp_float_t Q) {
    mp_float_t s = MICROPY_FLOAT_C_FUN(sin)(w0);
    mp_float_t c = MICROPY_FLOAT_C_FUN(cos)(w0);
    mp_float_t alpha = s / (2 * Q);
    mp_float_t a0 = 1 + alpha;
    mp_float_t a1 = -2 * c;
    mp_float_t a2 = 1 - alpha;
    mp_float_t b0 = alpha;
    mp_float_t b1 = 0;
    mp_float_t b2 = -alpha;

    mp_obj_t out_args[] = {
        mp_obj_new_float(a1 / a0),
        mp_obj_new_float(a2 / a0),
        mp_obj_new_float(b0 / a0),
        mp_obj_new_float(b1 / a0),
        mp_obj_new_float(b2 / a0),
    };

    return namedtuple_make_new((const mp_obj_type_t *)&synthio_biquad_type_obj, MP_ARRAY_SIZE(out_args), 0, out_args);
}

static int32_t biquad_scale_arg_obj(mp_obj_t arg) {
    return (int32_t)MICROPY_FLOAT_C_FUN(round)(MICROPY_FLOAT_C_FUN(ldexp)(mp_obj_get_float(arg), BIQUAD_SHIFT));
}
void synthio_biquad_filter_assign(biquad_filter_state *st, mp_obj_t biquad_obj) {
    if (biquad_obj == mp_const_none) {
        return;
    }

    sincos_result_t sc;
    fast_sincos(W0, &sc);

    mp_float_t alpha = sc.s / (2 * Q);

    mp_float_t a0, a1, a2, b0, b1, b2;

    switch (self->mode) {
        default:
            a0 = 1 + alpha;
            a1 = -2 * sc.c;
            a2 = 1 - alpha;

            switch (self->mode) {
                default:
                case SYNTHIO_LOW_PASS:
                    b2 = b0 = (1 - sc.c) * .5;
                    b1 = 1 - sc.c;
                    break;

                case SYNTHIO_HIGH_PASS:
                    b2 = b0 = (1 + sc.c) * .5;
                    b1 = -(1 + sc.c);
                    break;

                case SYNTHIO_BAND_PASS:
                    b0 = alpha;
                    b1 = 0;
                    b2 = -b0;
                    break;

                case SYNTHIO_NOTCH:
                    b0 = 1;
                    b1 = -2 * sc.c;
                    b2 = 1;
            }

            break;

        case SYNTHIO_PEAKING_EQ:
            b0 = 1 + alpha * A;
            b1 = -2 * sc.c;
            b2 = 1 - alpha * A;
            a0 = 1 + alpha / A;
            a1 = -2 * sc.c;
            a2 = 1 - alpha / A;
            break;

        case SYNTHIO_LOW_SHELF: {
            mp_float_t sqrt_A = fast_sqrt(A);
            b0 = A * ((A + 1) - (A - 1) * sc.c + 2 * sqrt_A * alpha);
            b1 = 2 * A * ((A - 1) - (A + 1) * sc.c);
            b2 = A * ((A + 1) - (A - 1) * sc.c - 2 * sqrt_A * alpha);
            a0 = (A + 1) + (A - 1) * sc.c + 2 * sqrt_A * alpha;
            a1 = -2 * ((A - 1) + (A + 1) * sc.c);
            a2 = (A + 1) + (A - 1) * sc.c - 2 * sqrt_A * alpha;
        }
        break;

        case SYNTHIO_HIGH_SHELF: {
            mp_float_t sqrt_A = fast_sqrt(A);
            b0 = A * ((A + 1) + (A - 1) * sc.c + 2 * sqrt_A * alpha);
            b1 = -2 * A * ((A - 1) + (A + 1) * sc.c);
            b2 = A * ((A + 1) + (A - 1) * sc.c - 2 * sqrt_A * alpha);
            a0 = (A + 1) - (A - 1) * sc.c + 2 * sqrt_A * alpha;
            a1 = 2 * ((A - 1) - (A + 1) * sc.c);
            a2 = (A + 1) - (A - 1) * sc.c - 2 * sqrt_A * alpha;
        }
        break;
    }
    if (mp_obj_is_type(biquad_obj, (const mp_obj_type_t *)&synthio_biquad_type_obj)) {
        mp_obj_tuple_t *biquad = (mp_obj_tuple_t *)MP_OBJ_TO_PTR(biquad_obj);
        st->a1 = biquad_scale_arg_obj(biquad->items[0]);
        st->a2 = biquad_scale_arg_obj(biquad->items[1]);
        st->b0 = biquad_scale_arg_obj(biquad->items[2]);
        st->b1 = biquad_scale_arg_obj(biquad->items[3]);
        st->b2 = biquad_scale_arg_obj(biquad->items[4]);
        return;
    }
    mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), MP_QSTR_filter, MP_QSTR_Biquad, MP_QSTR_BlockBiquad, mp_obj_get_type(biquad_obj)->name);

}

void synthio_biquad_filter_reset(biquad_filter_state *st) {
    memset(st, 0, sizeof(*st));
}

static inline int32_t biquad_filter_sample(int32_t input, int32_t a1, int32_t a2, int32_t b0, int32_t b1, int32_t b2, int32_t x0, int32_t x1, int32_t y0, int32_t y1) {
    return synthio_sat16((b0 * input + b1 * x0 + b2 * x1 - a1 * y0 - a2 * y1 + (1 << (BIQUAD_SHIFT - 1))), BIQUAD_SHIFT);
}

void synthio_biquad_filter_samples(biquad_filter_state *st, int32_t *buffer, size_t n_samples) {
    int32_t a1 = st->a1;
    int32_t a2 = st->a2;
    int32_t b0 = st->b0;
    int32_t b1 = st->b1;
    int32_t b2 = st->b2;

    int32_t x0 = st->x[0];
    int32_t x1 = st->x[1];
    int32_t y0 = st->y[0];
    int32_t y1 = st->y[1];

    for (size_t n = n_samples; n; --n, ++buffer) {
        int32_t input = *buffer;
<<<<<<< HEAD
        int32_t output = (b0 * input + b1 * x0 + b2 * x1 - a1 * y0 - a2 * y1 + (1 << (BIQUAD_SHIFT - 1))) >> BIQUAD_SHIFT;
=======
        int32_t output = biquad_filter_sample(input, a1, a2, b0, b1, b2, x0, x1, y0, y1);
>>>>>>> b59812b329febfd6c64d54f157f9f795a910eda5

        x1 = x0;
        x0 = input;
        y1 = y0;
        y0 = output;
        *buffer = output;
    }
    st->x[0] = x0;
    st->x[1] = x1;
    st->y[0] = y0;
    st->y[1] = y1;
}

int32_t synthio_biquad_filter_sample(mp_obj_t self_in, biquad_filter_state *st, int32_t input) {
    synthio_biquad_t *self = MP_OBJ_TO_PTR(self_in);

    int32_t output = biquad_filter_sample(input, self->a1, self->a2, self->b0, self->b1, self->b2, st->x[0], st->x[1], st->y[0], st->y[1]);

    st->x[1] = st->x[0];
    st->x[0] = input;
    st->y[1] = st->y[0];
    st->y[0] = output;

    return output;
}
