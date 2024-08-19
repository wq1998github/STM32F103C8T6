#include "bsp_system.h"
#include "bsp_systick.h"

static uint8_t fac_us = 0;  // us延时倍乘数

#if (SYSTEM_SUPPORT_OS == 1)
#include "ucos_ii.h" // 使用 ucosii

static uint16_t fac_ms = 0; // ms延时倍乘数,在 ucosii 中,代表每个节拍的ms数

extern void OS_CPU_SysTickHandler(void);

/**
 * @brief  systick中断服务函数,使用 ucosii 时用到
 */
void SysTick_Handler(void)
{
    if (OSRunning == OS_TRUE) // 系统已经运行
    {
        OS_CPU_SysTickHandler();
    }
}
#endif

/**
 * @brief  This function will initial stm32 board.
 */
void bsp_systick_init(void)
{
#if 0
    /* NVIC Configuration */
#define NVIC_VTOR_MASK 0x3FFFFF80
#ifdef VECT_TAB_RAM
    /* Set the Vector Table base location at 0x10000000 */
    SCB->VTOR = (0x10000000 & NVIC_VTOR_MASK);
#else /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    SCB->VTOR = (0x08000000 & NVIC_VTOR_MASK);
#endif
#endif

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    fac_us = SystemCoreClock / 1000000; // 不论是否使用OS,fac_us都需要使用

#if (SYSTEM_SUPPORT_OS == 1)
    fac_ms = 1000 / OS_TICKS_PER_SEC;                     // 代表OS可以延时的最小单位
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;            // 开启SYSTICK中断
    SysTick->LOAD = SystemCoreClock / OS_TICKS_PER_SEC - 1; // 每 1/OS_TICKS_PER_SEC 秒中断一次，这里是每 1ms 中断一次
#else
    SysTick->LOAD = 0xFFFF;                   // 清空计数器
#endif
    
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开始计数
    // RCC_ClocksTypeDef rcc;
    // RCC_GetClocksFreq(&rcc); // 读取系统时钟频率
}

/**
 * @brief  使用内核的 SysTick 实现阻塞的微妙延时
 * @param  nus 要延时的us数
 */
void delay_us(uint32_t nus)
{
    uint32_t told, tnow;
    uint32_t reload = SysTick->LOAD; // 自动重装载的值

    uint32_t ticks = nus * fac_us; // 计算需要的时钟数 = 延迟微秒数 * 每微秒的时钟脉冲数
    uint32_t tcnt = 0;

    // 获取当前的SysTick值
    told = SysTick->VAL;

    while (1)
    {
        // 重复刷新获取当前的SysTick值
        tnow = SysTick->VAL;

        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow; // 注意 SysTick 是一个递减的计数器
            }
            else
            {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if (tcnt >= ticks)
            {
                break; // 如果达到了需要的时钟数，就退出循环
            }
        }
    }
}

#if (SYSTEM_SUPPORT_OS == 1)

/**
 * @brief  使用 ucosii 实现非阻塞的毫秒延时
 * @param  nms 要延时的ms数
 */
void delay_ms(uint32_t nms)
{
    if (OSRunning == OS_TRUE) // 系统已经运行
    {
        if (nms >= fac_ms) // 延时的时间大于OS的最少时间周期
        {
            OSTimeDly(nms / fac_ms); // ucosii 延时
        }
        nms %= fac_ms; // OS已经无法提供这么小的延时了,采用普通方式延时
    }
    delay_us((uint32_t)(nms * 1000)); // 普通方式延时
}

/**
 * @brief  使用内核的 SysTick 实现阻塞的毫秒延时
 * @param  nms 要延时的ms数
 */
void delay_ms_x(uint32_t nms)
{
    int index = 0;

    for (index = 0; index < nms; index++)
    {
        delay_us(1000);
    }
}

#else

/**
 * @brief  使用内核的 SysTick 实现阻塞的毫秒延时
 * @param  nms 要延时的ms数
 */
void delay_ms(uint32_t nms)
{
    int index = 0;

    for (index = 0; index < nms; index++)
    {
        delay_us(1000);
    }
}

#endif
