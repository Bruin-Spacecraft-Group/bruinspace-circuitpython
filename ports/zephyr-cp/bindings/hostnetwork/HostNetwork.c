// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/hostnetwork/HostNetwork.h"

#include "py/runtime.h"

//| class HostNetwork:
//|     """Native networking for the host simulator."""
//|
//|     def __init__(self) -> None:
//|         """Create a HostNetwork instance."""
//|         ...
//|
static mp_obj_t hostnetwork_hostnetwork_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    hostnetwork_hostnetwork_obj_t *self = mp_obj_malloc(hostnetwork_hostnetwork_obj_t, &hostnetwork_hostnetwork_type);
    common_hal_hostnetwork_hostnetwork_construct(self);
    return MP_OBJ_FROM_PTR(self);
}

static const mp_rom_map_elem_t hostnetwork_hostnetwork_locals_dict_table[] = {
};
static MP_DEFINE_CONST_DICT(hostnetwork_hostnetwork_locals_dict, hostnetwork_hostnetwork_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    hostnetwork_hostnetwork_type,
    MP_QSTR_HostNetwork,
    MP_TYPE_FLAG_NONE,
    make_new, hostnetwork_hostnetwork_make_new,
    locals_dict, &hostnetwork_hostnetwork_locals_dict
    );
