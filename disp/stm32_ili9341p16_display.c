//
// Created by niels on 19-12-16.
//

#include <stm32f407xx.h>
#include "stm32f4xx_hal.h"
#include "stm32_ili9341p16_display.h"
#include "lvgl/hal/semaphore/semaphore.h"

/* Private defines */
#define ILI9341_RESET				0x01
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_GAMMA				0x26
#define ILI9341_DISPLAY_OFF			0x28
#define ILI9341_DISPLAY_ON			0x29
#define ILI9341_COLUMN_ADDR			0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM				0x2C
#define ILI9341_MAC					0x36
#define ILI9341_PIXEL_FORMAT		0x3A
#define ILI9341_WDB					0x51
#define ILI9341_WCD					0x53
#define ILI9341_RGB_INTERFACE		0xB0
#define ILI9341_FRC					0xB1
#define ILI9341_BPC					0xB5
#define ILI9341_DFC					0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1				0xC5
#define ILI9341_VCOM2				0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA				0xE8
#define ILI9341_DTCB				0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC					0xF7

#define LCD_REG (*((volatile unsigned short *) 0x60000000))
#define LCD_RAM (*((volatile unsigned short *) 0x60020000))

typedef enum {
  INCREASE_ENABLE,
  INCREASE_DISABLE
} Transfer_Source_Pointer_Increase;

typedef struct
{
  uint16_t color_buffer;
  uint16_t * mem_source;
  uint32_t pixel_count;
  uint32_t dma_index;
  Transfer_Source_Pointer_Increase increase_mode;
  void (*user_transfer_complete_callback)(void);
} Display_Operation;

DMA_HandleTypeDef     dma_handle;
Display_Operation current_operation;

uint32_t transfer_lock = 0;

void init_display_pins(void);
void init_fsmc(void);
void init_dma(void);
void delay_ms(uint32_t delay);
void dma_transfer_complete(DMA_HandleTypeDef * dmah);
void dma_transfer_error(DMA_HandleTypeDef * dmah);
void send_command(uint8_t command);
void send_data(uint16_t data);

void set_cursor_position(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void transfer_pixels(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *pixels,
                     Transfer_Source_Pointer_Increase, void (*operation_complete_callback)());

void dma_transfer_complete(DMA_HandleTypeDef * dmah){
  uint32_t dma_continu;
  uint32_t dma_source_address;

  if(current_operation.dma_index < current_operation.pixel_count){
    dma_continu = current_operation.dma_index;
    if(current_operation.dma_index + 0xFFFF < current_operation.pixel_count) {
      current_operation.dma_index = current_operation.dma_index + 0xFFFF;
    } else {
      current_operation.dma_index = current_operation.pixel_count;
    }
    if(INCREASE_ENABLE == current_operation.increase_mode){
      dma_source_address = (uint32_t) current_operation.mem_source + dma_continu;
    } else {
      dma_source_address = (uint32_t) current_operation.mem_source;
    }
    HAL_DMA_Start_IT(&dma_handle, dma_source_address, (uint32_t)&LCD_RAM, current_operation.dma_index - dma_continu);
  } else {
    free_lock(&transfer_lock);
    current_operation.user_transfer_complete_callback();
  }
}

void dma_transfer_error(DMA_HandleTypeDef * dmah){
  free_lock(&transfer_lock);
  Throw(GD_EXCEPTION_DMA_NOT_READY);
}

void init_dma(void){
  __HAL_RCC_DMA2_CLK_ENABLE();

  /*##-2- Select the DMA functional Parameters ###############################*/
  dma_handle.Init.Channel = DMA_CHANNEL_0;
  dma_handle.Init.Direction = DMA_MEMORY_TO_MEMORY;
  dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;               /* Peripheral increment mode */
  dma_handle.Init.MemInc = DMA_MINC_DISABLE;                  /* Memory increment mode */
  dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; /* Peripheral data alignment */
  dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;    /* memory data alignment */
  dma_handle.Init.Mode = DMA_NORMAL;                         /* Normal DMA mode */
  dma_handle.Init.Priority = DMA_PRIORITY_HIGH;              /* priority level */
  dma_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;           /* FIFO mode disabled */
  dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst */
  dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst */

  dma_handle.Instance = DMA2_Stream0;

  if(HAL_DMA_Init(&dma_handle) != HAL_OK)
  {
    Throw(GD_EXCEPTION_DMA_INIT);
    return;
  }

  HAL_DMA_RegisterCallback(&dma_handle, HAL_DMA_XFER_CPLT_CB_ID, dma_transfer_complete);
  HAL_DMA_RegisterCallback(&dma_handle, HAL_DMA_XFER_ERROR_CB_ID, dma_transfer_error);

  /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
  /* Set Interrupt Group Priority */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 1, 0);

  /* Enable the DMA STREAM global Interrupt */
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

void init_fsmc(void){

  __HAL_RCC_FSMC_CLK_ENABLE();

  int FSMC_Bank = 0;
  FSMC_Bank1->BTCR[FSMC_Bank+1] = FSMC_BTR1_ADDSET_1 | FSMC_BTR1_DATAST_1;

  // Bank1 NOR/SRAM control register configuration
  FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
}

void delay_ms(uint32_t delay){
  uint32_t t;

  for (t = HAL_GetTick() + delay; t > HAL_GetTick();) {}
}

void send_command(uint8_t command){
  LCD_REG = command;
}

void send_data(uint16_t data){
  LCD_RAM = data;
}

/**
 * xD0 : PD14 (also connected to Red LED on discovery)
 * xD1 : PD15 (also connected to Blue LED on discovery)
 * xD2 : PD0
 * xD3 : PD1
 * xD4 : PE7
 * xD5 : PE8
 * xD6 : PE9
 * xD7 : PE10
 * xD8 : PE11
 * xD9 : PE12
 * xD10 : PE13
 * xD11 : PE14
 * xD12 : PE15
 * xD13 : PD8
 * xD14 : PD9
 * xD15 : PD10
 * xNOE (RD) : PD4 (also connected to CS43L22 RESET on Discovery)
 * xNWE (W/R) : PD5 (also connected to USB overcurrent on Discovery)
 * A0 (RS) (D/C) : PF0 -> No FSMC_A0 on LQFP100 F407vg device -> FSMC_A16 (PD11)
 * NE4 LCD/CS CE3 LCD/CS : PG12 -> No PG12 on LQFP100 F407vg -> connect LCD CS to GND
 * Connect LCD reset to VDD
 */
void init_display_pins(void){
  GPIO_InitTypeDef gpio_init_struct;

  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  gpio_init_struct.Mode = GPIO_MODE_AF_PP;
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init_struct.Alternate = GPIO_AF12_FSMC;

  gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 |
                         GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &gpio_init_struct);

  gpio_init_struct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                         GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &gpio_init_struct);
}

void gd_display_init(void){

  if(false == get_lock(&transfer_lock)){
    Throw(GD_EXCEPTION_LOCK_NOT_FREE);
  }

  init_display_pins();
  init_fsmc();

  /* HW reset of display */
  delay_ms(20); // Allows display to start up.

  /* Software reset */
  send_command(ILI9341_RESET);
  delay_ms(10);

  send_command(ILI9341_POWERA);
  send_data(0x39);
  send_data(0x2C);
  send_data(0x00);
  send_data(0x34);
  send_data(0x02);
  send_command(ILI9341_POWERB);
  send_data(0x00);
  send_data(0xC1);
  send_data(0x30);
  send_command(ILI9341_DTCA);
  send_data(0x85);
  send_data(0x00);
  send_data(0x78);
  send_command(ILI9341_DTCB);
  send_data(0x00);
  send_data(0x00);
  send_command(ILI9341_POWER_SEQ);
  send_data(0x64);
  send_data(0x03);
  send_data(0x12);
  send_data(0x81);
  send_command(ILI9341_PRC);
  send_data(0x20);
  send_command(ILI9341_POWER1);
  send_data(0x23);
  send_command(ILI9341_POWER2);
  send_data(0x10);
  send_command(ILI9341_VCOM1);
  send_data(0x3E);
  send_data(0x28);
  send_command(ILI9341_VCOM2);
  send_data(0x86);
  send_command(ILI9341_MAC);
  send_data(0xE8);
  send_command(ILI9341_PIXEL_FORMAT);
  send_data(0x55); // 16-bit interface & 16-bit format
  send_command(ILI9341_FRC);
  send_data(0x00);
  send_data(0x18);
  send_command(ILI9341_DFC);
  send_data(0x08);
  send_data(0x82);
  send_data(0x27);
  send_command(0xF2);
  send_data(0x00);
  send_command(ILI9341_SLEEP_OUT);

  delay_ms(10);

  send_command(ILI9341_DISPLAY_ON);
  init_dma();

  free_lock(&transfer_lock);
}

void set_cursor_position(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  if(x0 > x1 || x1 > TFT_HOR_RES || y0 > y1 || y1 > TFT_VER_RES){
    Throw(GD_EXCEPTION_POS_OUT_OF_SCREEN);
  }
  send_command(ILI9341_COLUMN_ADDR);
  send_data(x0 >> 8);
  send_data(x0 & 0xFF);
  send_data(x1 >> 8);
  send_data(x1 & 0xFF);

  send_command(ILI9341_PAGE_ADDR);
  send_data(y0 >> 8);
  send_data(y0 & 0xFF);
  send_data(y1 >> 8);
  send_data(y1 & 0xFF);
}

void gd_display_fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color,
                     void (*operation_complete_callback)()) {
  if(false == get_lock(&transfer_lock)){
    Throw(GD_EXCEPTION_LOCK_NOT_FREE);
  }
  current_operation.color_buffer = color;
  transfer_pixels(x0, y0, x1, y1, &current_operation.color_buffer, INCREASE_DISABLE, operation_complete_callback);
}

void gd_display_bitmap(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *bitmap,
                       void (*operation_complete_callback)()){
  if(false == get_lock(&transfer_lock)){
    Throw(GD_EXCEPTION_LOCK_NOT_FREE);
  }
  transfer_pixels(x0, y0, x1, y1, bitmap, INCREASE_ENABLE, operation_complete_callback);
}

void transfer_pixels(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *pixels,
                     Transfer_Source_Pointer_Increase source_pointer_increase, void (*operation_complete_callback)()){
  if(dma_handle.State != HAL_DMA_STATE_READY){
    Throw(GD_EXCEPTION_DMA_NOT_READY);
  }
  current_operation.pixel_count = (x1 - x0 + 1) * (y1 - y0 + 1);
  current_operation.mem_source = pixels;
  current_operation.user_transfer_complete_callback = operation_complete_callback;
  current_operation.increase_mode = source_pointer_increase;
  if(current_operation.pixel_count > 0xFFFF){
    current_operation.dma_index = 0xFFFF;
  } else {
    current_operation.dma_index = current_operation.pixel_count;
  }

  if(INCREASE_DISABLE == source_pointer_increase){
    dma_handle.Instance->CR &= ~((uint32_t)1<<9); // Disable PINC bit (9) in CR register
  } else {
    dma_handle.Instance->CR |= DMA_PINC_ENABLE;
  }

  set_cursor_position(x0, y0, x1, y1);
  send_command(ILI9341_GRAM);
  HAL_DMA_Start_IT(&dma_handle, (uint32_t)current_operation.mem_source, (uint32_t)&LCD_RAM, current_operation.dma_index);

}

void DMA2_Stream0_IRQHandler(void)
{
  /* Check the interrupt and clear flag */
  HAL_DMA_IRQHandler(&dma_handle);
}