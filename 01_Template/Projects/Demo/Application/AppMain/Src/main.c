/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "sys.h"

typedef enum
{
    BSP_LED_OFF = 0,
    BSP_LED_ON = 1,
} BSP_LED_State;

void delay(uint32_t num)
{
    while (num--)
    {
        /* code */
    }
}

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
        // GPIO_SetBits(GPIOC, GPIO_Pin_13);
        PCout(13) = 1;
    }
    else
    {
        // GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        PCout(13) = 0;
    }
}

int main(void)
{
  led_init();

  /* Infinite loop */
  while (1)
  {
    led_control(BSP_LED_ON);
    delay(0x7ffff);
    led_control(BSP_LED_OFF);
    delay(0x7ffff);
  }
}

