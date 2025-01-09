#include "shared-bindings/board/__init__.h"

// This mapping only includes functional names because pins broken
// out on connectors are labeled with their MCU name available from
// microcontroller.pin.
STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // { MP_OBJ_NEW_QSTR(MP_QSTR_EXAMPLE_NET_1), MP_ROM_PTR(&pin_PA00) },
    // { MP_OBJ_NEW_QSTR(MP_QSTR_EXAMPLE_NET_2), MP_ROM_PTR(&pin_PA00) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
