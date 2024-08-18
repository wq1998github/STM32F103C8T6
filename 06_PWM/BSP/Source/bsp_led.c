#include "bsp_led.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#if 0
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
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  采用 PWM 的方式控制 LED 小灯，支持呼吸灯、亮度调节等功能
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void bsp_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};

    // 开启外设时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // TIM2_CH1 remap PA15, TIM2_CH2 remap PB3
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    // 配置 GPIOA - Pin15 --> TIM2_CH1
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置 GPIOB - Pin3  --> TIM2_CH2
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 配置 GPIOA - Pin2 --> TIM2_CH3
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置 GPIOA - Pin3 --> TIM2_CH4
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置 TIM2 的时基
    TIM_TimeBaseInitStruct.TIM_Prescaler = 720 - 1; // 100 KHz
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 100 - 1; // 1ms，PWM的频率是 1KHz
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    // 配置输出比较
    TIM_OCStructInit(&TIM_OCInitStruct);

    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    // TIM_OCInitStruct.TIM_OutputNState = ;
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    // TIM_OCInitStruct.TIM_OCNPolarity = ;
    // TIM_OCInitStruct.TIM_OCIdleState = ;
    // TIM_OCInitStruct.TIM_OCNIdleState = ;

    TIM_OC1Init(TIM2, &TIM_OCInitStruct);
    TIM_OC2Init(TIM2, &TIM_OCInitStruct);
    TIM_OC3Init(TIM2, &TIM_OCInitStruct);
    TIM_OC4Init(TIM2, &TIM_OCInitStruct);

    // 使能 TIM2
    TIM_Cmd(TIM2, ENABLE);
}
