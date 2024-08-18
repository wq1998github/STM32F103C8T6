#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "stm32f10x.h"

typedef enum
{
    BSP_LED_OFF = 0,
    BSP_LED_ON = 1,
} BSP_LED_State;

typedef enum
{
    BSP_LED_RED     = 0,    // TIM2_CH1
    BSP_LED_GREEN   = 1,    // TIM2_CH2
    BSP_LED_BLUE    = 2,    // TIM2_CH3
    BSP_LED_YELLOW  = 3,    // TIM2_CH4
} BSP_LED_Color;

#if 0
// 外部可调用函数的声明
void led_init(void);
void led_control(BSP_LED_State state);
#else

void bsp_led_init(void);

// 使用 PWM 的方式来调节 LED 亮度，Bright 取值 0 ~ 99
#define BSP_LED_RED_BRIGHT(Bright)      TIM_SetCompare1(TIM2, Bright)
#define BSP_LED_GREEN_BRIGHT(Bright)    TIM_SetCompare2(TIM2, Bright)
#define BSP_LED_BLUE_BRIGHT(Bright)     TIM_SetCompare3(TIM2, Bright)
#define BSP_LED_YELLOW_BRIGHT(Bright)   TIM_SetCompare4(TIM2, Bright)

#endif

#endif
