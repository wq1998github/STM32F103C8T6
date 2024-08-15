#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#include "bsp_pwm.h"

/*
    1. 定时器的时钟频率是72MHz，经过预分频之后的频率，如果取预分频系数为（72-1），则时钟周期是 1MHz
    2. PWM的周期是计数到自动重装载值所需要的时间，即 （ARR+1）/（1MHz）
        - 反过来就是PWM的频率，即 时钟周期 / (ARR+1)
        - 因此PWM的频率计算公式为：72MHz / 预分频系数 / （ARR+1）
    3. 

*/

/**
 * @brief  初始化 pwm，产生一个 1KHz的PWM信号
 */
void bsp_pwm_init(void)
{
    // Open the relative Clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Config the GPIOA1 - TIM2_CH2
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    TIM_TimeBaseInitStruct.TIM_Prescaler = 720 - 1; // 定时器的时钟是 1MHz
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 100 - 1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 外部信号的采样频率，这里不分频
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;     // 高级定时器才有的
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    TIM_OCInitTypeDef TIM_OCInitStruct = {0};
    TIM_OCStructInit(&TIM_OCInitStruct); // 输出比较的默认配置
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    // TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    // TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
    // TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
    // TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC2Init(TIM2, &TIM_OCInitStruct);

    // 启动定时器
    TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief  设置输出占空比
 * @param  percent 
 */
void bsp_pwm_set_duty_cycle(uint8_t percent)
{
    uint16_t Compare2 = 100 / 100 * percent;
    TIM_SetCompare2(TIM2, Compare2);
}

