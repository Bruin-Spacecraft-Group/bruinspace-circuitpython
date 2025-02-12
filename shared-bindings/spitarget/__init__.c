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
//| Example that emulates an SPI analog-to-digital converter::
//|
//|   import board
//|   import analogio
//|   from spitarget import SPITarget
//|
//|   ain0 = analogio.AnalogIn(board.A0)
//|   ain1 = analogio.AnalogIn(board.A1)
//|   selected_channel = ain0
//|
//|   def map_adc_channel(index):
//|       return ain0 if (index == 0) else ain1
//|
//|   mosi_buffer = bytearray(2)
//|   miso_buffer = bytearray(2)
//|   with SPITarget(sck=board.D12, mosi=board.D13, miso=board.D11, ss=board.D10) as device:
//|       while True:
//|           # Convert analog signal to array of bytes
//|           reading = selected_channel.value
//|           miso_buffer[0] = (reading >> 8) & 0xFF
//|           miso_buffer[1] = (reading) & 0xFF
//|           # Send array of bytes over SPI to main
//|           device.load_packet(mosi_buffer, miso_buffer)
//|           device.wait_transfer(0)
//|           # Handle command from main, which sets the ADC channel
//|           selected_channel = map_adc_channel((mosi_buffer[0] << 8) | mosi_buffer[1])
//|
//| Communicating with the ADC emulator from the REPL of an attached CircuitPython board might look like ::
//|
//|   >>> import busio
//|   >>> spi = busio.SPI(board.SCK, board.MOSI, board.MISO)
//|   >>> spi.try_lock()
//|   True
//|   >>> spi.write(bytearray([0, 0])) # ADC command: read from A0
//|   >>> adc_result = bytearray(2)
//|   >>> spi.readinto(adc_result)
//|   >>> adc_result
//|   bytearray(b'\xc4\x16')
//|   >>> spi.unlock()
//|
//| """



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
