#define MICROPY_HW_BOARD_NAME "RAPID-0 CDH"
#define MICROPY_HW_MCU_NAME "samd21g18"

// This is for Rev F which is green

// #define MICROPY_HW_LED_TX   (&pin_PA27)
// #define MICROPY_HW_LED_RX   (&pin_PB06)

// #define MICROPY_HW_LED_STATUS   (&pin_PA16)

// #define MICROPY_HW_NEOPIXEL (&pin_PB22)

#define BOARD_HAS_CRYSTAL 0

#define CAMERA_I2C_SCL (&pin_PB03)
#define CAMERA_I2C_SDA (&pin_PB02)

#define CAMERA_SPI_SCK (&pin_PA05)
#define CAMERA_SPI_MISO (&pin_PA04)
#define CAMERA_SPI_MOSI (&pin_PA07)
#define CAMERA_SPI_CS (&pin_PA06)

#define INTERSUBSYSTEM_SPI_SCK (&pin_PB17)
#define INTERSUBSYSTEM_SPI_MISO (&pin_PA21)
#define INTERSUBSYSTEM_SPI_MOSI (&pin_PB16)

#define ADCS_SPI_CS (&pin_PA20)
#define EPS_SPI_CS (&pin_PB00)
#define PAYLOAD_SPI_CS (&pin_PB01)

// #define DEFAULT_I2C_BUS_SCL (&pin_PB03)
// #define DEFAULT_I2C_BUS_SDA (&pin_PB02)

// #define DEFAULT_SPI_BUS_SCK (&pin_PA13)
// #define DEFAULT_SPI_BUS_MOSI (&pin_PA12)
// #define DEFAULT_SPI_BUS_MISO (&pin_PA14)

// #define DEFAULT_UART_BUS_RX (&pin_PA23)
// #define DEFAULT_UART_BUS_TX (&pin_PA22)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
