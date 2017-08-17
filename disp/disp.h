/**
 * @file disp.h
 *
 */

#ifndef DISP_H
#define DISP_H

#ifdef __cplusplus
extern "C" {
#endif
#define LVGL_WRAPPER_ENABLE 1
#if LVGL_WRAPPER_ENABLE
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include "hal/disp/stm32_ili9341p16_display.h"
#include "misc/gfx/color.h"


/*********************
 *      DEFINES
 *********************/
#define DISP_HW_ACC 0	/* No HW acceleration available on STM32F407 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize your display here
 */
void disp_init(void);

/**
 * Fill a rectangular area with a color
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color fill color
 */
void disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_t color);

/**
 * Put a color map to a rectangular area
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_p pointer to an array of colors
 */
void disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const color_t * color_p);

/**********************
 *      MACROS
 **********************/
#endif // LVGL_WRAPPER_ENABLE

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // DISP_H