#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include "stm32f10x.h"

/**
 * @brief  使用内部时钟实现定时功能
 */
void bsp_timer_init(void);

/**
 * @brief  使用外部时钟实现计数功能
 */
void bsp_timer_extern_init(void);

uint16_t bsp_timer_get_cnt(void);
uint32_t bsp_timer_get_tick(void);

#endif
