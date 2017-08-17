//
// Created by niels on 19-12-16.
//

#ifndef GUIDUINO_STM32_ILI9341_P16_DISPLAY_H
#define GUIDUINO_STM32_ILI9341_P16_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#define TFT_HOR_RES  320
#define TFT_VER_RES 240

#include <stdint.h>
#include <CException.h>

#define GD_EXCEPTION_DMA_NOT_READY      90001
#define GD_EXCEPTION_DMA_TRANSFER       90002
#define GD_EXCEPTION_DMA_INIT           90003
#define GD_EXCEPTION_LOCK_NOT_FREE      90004
#define GD_EXCEPTION_POS_OUT_OF_SCREEN  90005


void gd_display_init(void);
void gd_display_fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color,
                     void (*operation_complete_callback)());
void gd_display_bitmap(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *bitmap,
                       void (*operation_complete_callback)());

#ifdef __cplusplus
}
#endif

#endif //GUIDUINO_STM32_ILI9341_P16_DISPLAY_H