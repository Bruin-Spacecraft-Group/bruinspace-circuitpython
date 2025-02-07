# TODO: get a VID and PID for RAPID-0
USB_VID = 0x239A
USB_PID = 0x8098
USB_PRODUCT = "RAPID-0"
USB_MANUFACTURER = "Bruin Spacecraft Group"

INTERNAL_FLASH_FILESYSTEM = 1

MCU_SERIES = H7
MCU_VARIANT = STM32H743xx
MCU_PACKAGE = LQFP100_x7

LD_COMMON = boards/common_tcm.ld
LD_FILE = boards/STM32H743_fs.ld

CIRCUITPY_BOARD = 0
