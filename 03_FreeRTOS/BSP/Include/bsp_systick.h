#ifndef __BSP_SYSTICK_H__
#define __BSP_SYSTICK_H__

#include "stm32f10x.h"

void bsp_systick_init(void);
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);
void delay_ms_x(uint32_t nms);

#endif
