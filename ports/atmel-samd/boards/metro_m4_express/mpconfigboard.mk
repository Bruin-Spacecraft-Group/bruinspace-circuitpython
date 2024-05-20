USB_VID = 0x239A
USB_PID = 0x8021
USB_PRODUCT = "Metro M4 Express"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD51J19A
CHIP_FAMILY = samd51

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "S25FL116K, S25FL216K, GD25Q16C, W25Q16JVxQ"
LONGINT_IMPL = MPZ

CIRCUITPY__EVE = 1
<<<<<<< HEAD
<<<<<<< HEAD
CIRCUITPY_CODEOP = 0
=======
CIRCUITPY_CANIO = 0
>>>>>>> working edits to spi and can attempts
=======
>>>>>>> added board configurations for RAPID-0 CDH, ADCS, and EPS boards. reverted Metro M4 Express board configurations to original
CIRCUITPY_FLOPPYIO = 0
CIRCUITPY_JPEGIO = 0
CIRCUITPY_SYNTHIO = 0
CIRCUITPY_VECTORIO = 0

# We don't have room for the fonts for terminalio for certain languages,
# so turn off terminalio, and if it's off and displayio is on,
# force a clean build.
# Note that we cannot test $(CIRCUITPY_DISPLAYIO) directly with an
# ifeq, because it's not set yet.
ifneq (,$(filter $(TRANSLATION),ja ko ru))
CIRCUITPY_TERMINALIO = 0
RELEASE_NEEDS_CLEAN_BUILD = $(CIRCUITPY_DISPLAYIO)
endif
