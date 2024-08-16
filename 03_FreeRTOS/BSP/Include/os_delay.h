#ifndef __OS_DELAY_H__
#define __OS_DELAY_H__

#include "stm32f10x.h"

void OS_SysTick_init(void);
void delay_ms(u32 nms);
void delay_us(u32 nus);
void delay_xms(u32 nms);

#endif
