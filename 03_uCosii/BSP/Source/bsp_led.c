#include "bsp_led.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

void led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void led_control(BSP_LED_State state)
{
    if (BSP_LED_OFF == state)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
    }
    else
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    }
}
