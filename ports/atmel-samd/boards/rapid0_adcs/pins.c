// More than one revision of this board is available.
// This board specifies the most up to date PCB Revision

#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_PC15) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_GYRO1_CS), MP_ROM_PTR(&pin_PB19) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GYRO2_CS), MP_ROM_PTR(&pin_PB20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GYRO3_CS), MP_ROM_PTR(&pin_PB21) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_MAG1), MP_ROM_PTR(&pin_PA02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MAG2), MP_ROM_PTR(&pin_PA01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MAG3), MP_ROM_PTR(&pin_PA00) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_MAG_SPI_MOSI), MP_ROM_PTR(&pin_PA16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MAG_SPI_MISO), MP_ROM_PTR(&pin_PA18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MAG_SPI_SLCK), MP_ROM_PTR(&pin_PA17) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_GYRO_SPI_MOSI), MP_ROM_PTR(&pin_PA12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GYRO_SPI_MISO), MP_ROM_PTR(&pin_PA14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GYRO_SPI_SLCK), MP_ROM_PTR(&pin_PA13) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_CDH_SPI_MOSI), MP_ROM_PTR(&pin_PB15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CDH_SPI_MISO), MP_ROM_PTR(&pin_PB12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CDH_SPI_SLCK), MP_ROM_PTR(&pin_PB13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ADCS_SPI_CS), MP_ROM_PTR(&pin_PB14) },

    // { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA04) },
    // { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA06) },
    // { MP_ROM_QSTR(MP_QSTR_A10), MP_ROM_PTR(&pin_PB07) },
    // { MP_ROM_QSTR(MP_QSTR_A11), MP_ROM_PTR(&pin_PC00) },
    // { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PA07) },
    // { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PB08) },
    // { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PB09) },
    // { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PC02) },
    // { MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_PC03) },
    // { MP_ROM_QSTR(MP_QSTR_A7), MP_ROM_PTR(&pin_PB04) },
    // { MP_ROM_QSTR(MP_QSTR_A8), MP_ROM_PTR(&pin_PB05) },
    // { MP_ROM_QSTR(MP_QSTR_A9), MP_ROM_PTR(&pin_PB06) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_1), MP_ROM_PTR(&pin_PA17) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_UART_TX), MP_ROM_PTR(&pin_PA17) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_SPI_MOSI), MP_ROM_PTR(&pin_PA17) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_I2C_SDA), MP_ROM_PTR(&pin_PA17) },
    // { MP_ROM_QSTR(MP_QSTR_ATW01_MOSI), MP_ROM_PTR(&pin_PA17) },
    // { MP_ROM_QSTR(MP_QSTR_ESP01_TX), MP_ROM_PTR(&pin_PA17) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_10), MP_ROM_PTR(&pin_PC01) },
    // { MP_ROM_QSTR(MP_QSTR_ATW01_IRQ), MP_ROM_PTR(&pin_PC01) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_11), MP_ROM_PTR(&pin_PC10) },
    // { MP_ROM_QSTR(MP_QSTR_ATW01_GPIO_3), MP_ROM_PTR(&pin_PC10) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_12), MP_ROM_PTR(&pin_PC11) },
    // { MP_ROM_QSTR(MP_QSTR_ATW01_GPIO_1), MP_ROM_PTR(&pin_PC11) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_3), MP_ROM_PTR(&pin_PA18) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_UART_RTS), MP_ROM_PTR(&pin_PA18) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_SPI_SS), MP_ROM_PTR(&pin_PA18) },
    // { MP_ROM_QSTR(MP_QSTR_ATW01_SS), MP_ROM_PTR(&pin_PA18) },
    // { MP_ROM_QSTR(MP_QSTR_ESP01_GPIO0), MP_ROM_PTR(&pin_PA18) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_4), MP_ROM_PTR(&pin_PC14) },
    // { MP_ROM_QSTR(MP_QSTR_ATW01_RESET), MP_ROM_PTR(&pin_PC14) },
    // { MP_ROM_QSTR(MP_QSTR_ESP01_RESET), MP_ROM_PTR(&pin_PC14) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_5), MP_ROM_PTR(&pin_PA19) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_UART_CTS), MP_ROM_PTR(&pin_PA19) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_SPI_MISO), MP_ROM_PTR(&pin_PA19) },
    // { MP_ROM_QSTR(MP_QSTR_ATW01_MISO), MP_ROM_PTR(&pin_PA19) },
    // { MP_ROM_QSTR(MP_QSTR_ESP01_GPIO2), MP_ROM_PTR(&pin_PA19) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_6), MP_ROM_PTR(&pin_PC15) },
    // { MP_ROM_QSTR(MP_QSTR_ATW01_EN), MP_ROM_PTR(&pin_PC15) },
    // { MP_ROM_QSTR(MP_QSTR_ESP01_CH_PD), MP_ROM_PTR(&pin_PC15) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_8), MP_ROM_PTR(&pin_PA16) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_UART_RX), MP_ROM_PTR(&pin_PA16) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_SPI_SCK), MP_ROM_PTR(&pin_PA16) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_I2C_SCL), MP_ROM_PTR(&pin_PA16) },
    // { MP_ROM_QSTR(MP_QSTR_ATW01_SCK), MP_ROM_PTR(&pin_PA16) },
    // { MP_ROM_QSTR(MP_QSTR_ESP01_RX), MP_ROM_PTR(&pin_PA16) },
    // { MP_ROM_QSTR(MP_QSTR_AUX_9), MP_ROM_PTR(&pin_PA27) },
    // { MP_ROM_QSTR(MP_QSTR_ATW01_WAKE), MP_ROM_PTR(&pin_PA27) },
    // { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PB31) },
    // { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PC16) },
    // { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PC13) },
    // { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PA14) },
    // { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_PA15) },
    // { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PB12) },
    // { MP_ROM_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_PB13) },
    // { MP_ROM_QSTR(MP_QSTR_D15), MP_ROM_PTR(&pin_PA21) },
    // { MP_ROM_QSTR(MP_QSTR_I2S_SDO), MP_ROM_PTR(&pin_PA21) },
    // { MP_ROM_QSTR(MP_QSTR_D16), MP_ROM_PTR(&pin_PA22) },
    // { MP_ROM_QSTR(MP_QSTR_I2S_SDI), MP_ROM_PTR(&pin_PA22) },
    // { MP_ROM_QSTR(MP_QSTR_D17), MP_ROM_PTR(&pin_PA20) },
    // { MP_ROM_QSTR(MP_QSTR_I2S_FS_0), MP_ROM_PTR(&pin_PA20) },
    // { MP_ROM_QSTR(MP_QSTR_D18), MP_ROM_PTR(&pin_PB16) },
    // { MP_ROM_QSTR(MP_QSTR_I2S_SCK_0), MP_ROM_PTR(&pin_PB16) },
    // { MP_ROM_QSTR(MP_QSTR_D19), MP_ROM_PTR(&pin_PB17) },
    // { MP_ROM_QSTR(MP_QSTR_I2S_MCK_0), MP_ROM_PTR(&pin_PB17) },
    // { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_PC17) },
    // { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_PC18) },
    // { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_PC19) },
    // { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PC20) },
    // { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PC21) },
    // { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_PB18) },
    // { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_PB19) },
    // { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PC12) },
    // { MP_ROM_QSTR(MP_QSTR_DAC0), MP_ROM_PTR(&pin_PA02) },
    // { MP_ROM_QSTR(MP_QSTR_DAC1), MP_ROM_PTR(&pin_PA05) },
    // { MP_ROM_QSTR(MP_QSTR_I2C1_SCL), MP_ROM_PTR(&pin_PA12) },
    // { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA12) },
    // { MP_ROM_QSTR(MP_QSTR_I2C1_SDA), MP_ROM_PTR(&pin_PA13) },
    // { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA13) },
    // { MP_ROM_QSTR(MP_QSTR_LED_AUX), MP_ROM_PTR(&pin_PC26) },
    // { MP_ROM_QSTR(MP_QSTR_LED_B), MP_ROM_PTR(&pin_PA23) },
    // { MP_ROM_QSTR(MP_QSTR_LED_STATUS), MP_ROM_PTR(&pin_PA23) },
    // { MP_ROM_QSTR(MP_QSTR_LED_G), MP_ROM_PTR(&pin_PB15) },
    // { MP_ROM_QSTR(MP_QSTR_LED_QSPI), MP_ROM_PTR(&pin_PC07) },
    // { MP_ROM_QSTR(MP_QSTR_LED_R), MP_ROM_PTR(&pin_PB14) },
    // { MP_ROM_QSTR(MP_QSTR_LED_RX), MP_ROM_PTR(&pin_PC05) },
    // { MP_ROM_QSTR(MP_QSTR_LED_TX), MP_ROM_PTR(&pin_PC06) },
    // { MP_ROM_QSTR(MP_QSTR_RS485_RE), MP_ROM_PTR(&pin_PB01) },
    // { MP_ROM_QSTR(MP_QSTR_RS485_RX), MP_ROM_PTR(&pin_PB03) },
    // { MP_ROM_QSTR(MP_QSTR_RS485_TE), MP_ROM_PTR(&pin_PB00) },
    // { MP_ROM_QSTR(MP_QSTR_RS485_TX), MP_ROM_PTR(&pin_PB02) },
    // { MP_ROM_QSTR(MP_QSTR_SPI1_MISO), MP_ROM_PTR(&pin_PB23) },
    // { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PB23) },
    // { MP_ROM_QSTR(MP_QSTR_SPI1_MOSI), MP_ROM_PTR(&pin_PC27) },
    // { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PC27) },
    // { MP_ROM_QSTR(MP_QSTR_SPI1_SCK), MP_ROM_PTR(&pin_PC28) },
    // { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PC28) },
    // { MP_ROM_QSTR(MP_QSTR_SPI1_SS), MP_ROM_PTR(&pin_PB22) },
    // { MP_ROM_QSTR(MP_QSTR_SS), MP_ROM_PTR(&pin_PB22) },
    // { MP_ROM_QSTR(MP_QSTR_UART1_CTS), MP_ROM_PTR(&pin_PC25) },
    // { MP_ROM_QSTR(MP_QSTR_UART1_RTS), MP_ROM_PTR(&pin_PC24) },
    // { MP_ROM_QSTR(MP_QSTR_UART1_RX), MP_ROM_PTR(&pin_PB24) },
    // { MP_ROM_QSTR(MP_QSTR_UART1_TX), MP_ROM_PTR(&pin_PB25) },
    // { MP_ROM_QSTR(MP_QSTR_UART2_RX), MP_ROM_PTR(&pin_PB20) },
    // { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PB20) },
    // { MP_ROM_QSTR(MP_QSTR_I2C2_SCL), MP_ROM_PTR(&pin_PB20) },
    // { MP_ROM_QSTR(MP_QSTR_UART2_TX), MP_ROM_PTR(&pin_PB21) },
    // { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PB21) },
    // { MP_ROM_QSTR(MP_QSTR_I2C2_SDA), MP_ROM_PTR(&pin_PB21) },
    // { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    // { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    // { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
