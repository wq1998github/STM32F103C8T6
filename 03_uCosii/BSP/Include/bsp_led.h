#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "stm32f10x.h"

typedef enum
{
    BSP_LED_OFF = 0,
    BSP_LED_ON = 1,
} BSP_LED_State;

// 外部可调用函数的声明
void led_init(void);
void led_control(BSP_LED_State state);

#endif
