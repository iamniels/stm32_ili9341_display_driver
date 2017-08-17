# ILI9341 display driver for the STM32F4 microcontroller
This repository contains a display driver for the STM32F4 microcontroller.
The ILI9341 should be connected to the STM32F4 in 16-bit parallel mode.

Because the 16-bit parallel mode is used in combination with the MCU's
FSMC, the data transfer to the display is very fast.

The driver only supports 16-bit (RGB565) colors.

## Usage
The driver offers three functions for display memory access. Include
`stm32_ili9341p16_display.h` to use the driver.

`gd_display_init()` should be called before any other display operations.

`gd_display_fill(...)` fills a rectangle on the display with a single
color.

`gd_display_bitmap(...)` transfers data from the memory to the display.

## LVGL wrapper
This driver can be used with the
[LittleV Graphics Library](https://github.com/littlevgl/lvgl). This is a
library to build GUI's.

To use this driver with LVGL, enable double buffering in LVGL with
`#define LV_VDB_DOUBLE 1` in `lv_conf.h`.

`disp_fill` is not implemented yet.

## Dependencies
This driver requires the following libraries:

* [ThrowTheSwitch's CException](https://github.com/ThrowTheSwitch/CException)
* [STM32F4 HAL](http://www.st.com/en/embedded-software/stm32cubef4.html)
* CMSIS
