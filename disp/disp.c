
/**
 * @file disp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "disp.h"
#if LVGL_WRAPPER_ENABLE
#include <lvgl/lvgl/lv_obj/lv_vdb.h>

/**
 * Initialize your display here
 */
void disp_init(void)
{
  gd_display_init();
}

/**
 * Fill a rectangular area with a color
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color fill color
 */
void disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_t color)
{
}

/**
 * Put a color map to a rectangular area
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_p pointer to an array of colors
 */
void disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const color_t * color_p)
{
  gd_display_bitmap((uint16_t) x1, (uint16_t) y1, (uint16_t) x2, (uint16_t) y2, (uint16_t *) color_p, lv_vdb_flush_ready);
}

#endif // LVGL_WRAPPER_ENABLE