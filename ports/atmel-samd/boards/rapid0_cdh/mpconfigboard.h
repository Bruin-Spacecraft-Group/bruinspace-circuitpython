#define MICROPY_HW_BOARD_NAME "RAPID-0 CDH"
#define MICROPY_HW_MCU_NAME "samd21g18"

// This is for Rev F which is green

// #define MICROPY_HW_LED_TX   (&pin_PA27)
// #define MICROPY_HW_LED_RX   (&pin_PB06)

// #define MICROPY_HW_LED_STATUS   (&pin_PA16)

// #define MICROPY_HW_NEOPIXEL (&pin_PB22)

// #define SPI_FLASH_MOSI_PIN          &pin_PA08
// #define SPI_FLASH_MISO_PIN          &pin_PA09
// #define SPI_FLASH_SCK_PIN           &pin_PB10
// #define SPI_FLASH_CS_PIN            &pin_PB11

#define BOARD_HAS_CRYSTAL 0

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (64)
#define CIRCUITPY_INTERNAL_NVM_SIZE (64)
