#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#include "misc.h"

#include "bsp_timer.h"

/*
    1. 首先明确定时器的本质是一个计数器，经过参数配置，计数器会在脉冲的上升沿、下降沿或者双边沿进行计数加1
    2. 其次注意时钟是一个脉冲信号
        - 用稳定规律的脉冲作为时钟可以实现计数定时的功能
        - 用不规律的脉冲也可以计数，但不能定时
    3. 配置定时器，主要就是配置计数器、预分频器和自动重装载寄存器
        - 预分频器对定时器的时钟进行分频
        - 计数器会在脉冲的边沿计数
        - 自动重装载寄存器是计数的终点值
    4. 计数器是16位的，计数的上限就是 2^16 = 65535
    5. 计数器的计数频率 等于 时钟来源（内部或外部） 除 （预分频系数 + 1）
*/

/**
 * @brief  定时器的计时功能
 */
void bsp_timer_init(void)
{
    // Open the relative Clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 选择时钟源，计时使用内部时钟源
    TIM_InternalClockConfig(TIM2);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1; // 预分频结果是 10KHz，取值需要在 0 ~ 65535 之间
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;           // 定时一秒，取值需要在 0 ~ 65535 之间
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 外部信号的采样频率，这里不分频
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;     // 高级定时器才有的
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    // 使能定时更新中断
    TIM_ClearFlag(TIM2, TIM_FLAG_Update); // 保险起见，清除一下
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // 配置定时器 2 的中断优先级
    NVIC_InitTypeDef NVIC_InitStruct = {0};
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // 启动定时器
    TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief  使用外部时钟
 */
void bsp_timer_extern_init(void)
{
    // Open the relative Clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 选择时钟源，计时使用外部时钟源，滤波功能拉满比较好，避免因为抖动导致计数异常
    TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x00);

    // 配置外部通道，PA0 - ETR
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // 官方推荐浮空输入，但是这样容易因为干扰而波动，因此改成上拉
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    TIM_TimeBaseInitStruct.TIM_Prescaler = 1 - 1; // 这里是用按键模拟外部时钟信号，值小一些方便看现象，取值需要在 0 ~ 65535 之间
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 5 - 1;     // 这里是用按键模拟外部时钟信号，值小一些方便看现象，取值需要在 0 ~ 65535 之间
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 外部信号的采样频率，这里不分频
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;     // 高级定时器才有的
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    // 使能定时更新中断
    TIM_ClearFlag(TIM2, TIM_FLAG_Update); // 不加这条，复位会直接触发一次中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // 配置定时器 2 的中断优先级
    NVIC_InitTypeDef NVIC_InitStruct = {0};
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // 启动定时器
    TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief  获取计数器的计数值
 * @return uint16_t
 */
uint16_t bsp_timer_get_cnt(void)
{
    return TIM_GetCounter(TIM2);
}

static uint32_t tick = 0;

/**
 * @brief  获取 tick
 * @return uint32_t
 */
uint32_t bsp_timer_get_tick(void)
{
    return tick;
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        tick = (++tick < 0xFFFFFFFF) ? tick : 0;
    }
}
