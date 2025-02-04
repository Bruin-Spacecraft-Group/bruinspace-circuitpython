#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/spitarget/SPITarget.h"

#include "py/runtime.h"

//| """Serial Peripheral Interface protocol target
//|
//| The `spitarget` module contains classes to support an SPI target.
//|
//| Example emulating a target ::
//|
//|   import board
//|   from spitarget import SPITarget
//|
//|   TODO
//|
//| This example sets up an SPI device that can be accessed from Linux like this::
//|
//|   $ TODO command
//|   TODO result
//|   $ TODO command

STATIC const mp_rom_map_elem_t spitarget_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_spitarget) },
    { MP_ROM_QSTR(MP_QSTR_SPITarget), MP_ROM_PTR(&spitarget_spi_target_type) },
};

STATIC MP_DEFINE_CONST_DICT(spitarget_module_globals, spitarget_module_globals_table);

const mp_obj_module_t spitarget_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&spitarget_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_spitarget, spitarget_module);
