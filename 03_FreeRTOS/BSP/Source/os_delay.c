#include "os_delay.h"
#include "misc.h"
//////////////////////////////////////////////////////////////////////////////////
// 如果需要使用OS,则包括下面的头文件即可.
#if 1                 // SYSTEM_SUPPORT_OS
#include "FreeRTOS.h" //FreeRTOS使用
#include "task.h"
#endif

static uint8_t fac_us = 0;  // us延时倍乘数
static uint16_t fac_ms = 0; // ms延时倍乘数,在ucos下,代表每个节拍的ms数

extern void xPortSysTickHandler(void);

// systick中断服务函数,使用ucos时用到
void SysTick_Handler(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) // 系统已经运行
    {
        xPortSysTickHandler();
    }
}

// 初始化延迟函数
// SYSTICK的时钟固定为AHB时钟，基础例程里面SYSTICK时钟频率为AHB/8
// 这里为了兼容FreeRTOS，所以将SYSTICK的时钟频率改为AHB的频率！
// SYSCLK:系统时钟频率
void OS_SysTick_init()
{
    uint32_t reload;

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); // 选择不分频的满速外部时钟 HCLK
    fac_us = SystemCoreClock / 1000000;              // 不论是否使用OS,fac_us都需要使用
    reload = SystemCoreClock / configTICK_RATE_HZ;   // 根据 configTICK_RATE_HZ 设定溢出时间
                                                     // reload为24位寄存器,最大值:16777216,在72MHz下,约合0.233s左右
    fac_ms = 1000 / configTICK_RATE_HZ;              // 代表OS可以延时的最小单位

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; // 开启SYSTICK中断
    SysTick->LOAD = reload;                    // 每 1/configTICK_RATE_HZ 秒中断一次，这里是每 1ms 中断一次
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  // 开启SYSTICK
}

// 延时nus
// nus:要延时的us数.
// nus:0~204522252(最大值即2^32/fac_us@fac_us=168)
void delay_us(u32 nus)
{

    u32 told, tnow;
    u32 reload = SysTick->LOAD; // LOAD的值

    u32 ticks = nus * fac_us;   // 需要的 时钟周期 个数
    u32 tcnt = 0;               // 经过的 时钟周期 个数

    told = SysTick->VAL; // 刚进入时的计数器值
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow; // 这里注意一下SYSTICK是一个递减的计数器就可以了.
            }
            else
            {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if (tcnt >= ticks)
            {
                break; // 时间超过/等于要延迟的时间,则退出.
            }
        }
    }
}

// 延时nms
// nms:要延时的ms数
// nms:0~65535
void delay_ms(u32 nms)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) // 系统已经运行
    {
        if (nms >= fac_ms) // 延时的时间大于OS的最少时间周期
        {
            vTaskDelay(nms / fac_ms); // FreeRTOS延时
        }
        nms %= fac_ms; // OS已经无法提供这么小的延时了,采用普通方式延时
    }
    delay_us((u32)(nms * 1000)); // 普通方式延时
}

// 延时nms,不会引起任务调度
// nms:要延时的ms数
void delay_xms(u32 nms)
{
    u32 i;
    for (i = 0; i < nms; i++)
    {
        delay_us(1000);
    }
}

#if 0
void my_delay_init()
{
    u32 reload = 0; // 自动重装载寄存器的值

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); // 选择满速 HCLK ，SysTick的时钟频率是 72MHz
    
    fac_us = 72000000 / 1000000;                     // 计时 1us 所需要的脉冲个数，结果为 72
    reload = 72000000 / configTICK_RATE_HZ;          // 根据 configTICK_RATE_HZ（1000） 设定溢出时间
    fac_ms = 1000 / configTICK_RATE_HZ;              // 代表 vTaskDelay 延时的单位是多少 ms

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; // 允许 SysTick 产生中断
    SysTick->LOAD = reload;                    // 每 1/configTICK_RATE_HZ 秒中断一次，这里是每 1ms 中断一次
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  // 使能 SysTick 运行
}

void delay_us(u32 nus)
{
    u32 told, tnow;

    u32 ticks = nus * fac_us;   // 延时需要的 时钟周期 个数
    u32 tcnt = 0;               // 实际经过的 时钟周期 个数

    u32 reload = SysTick->LOAD; // LOAD的值

    told = SysTick->VAL; // 刚进入时的计数器值
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow; // 这里注意 SysTick 是一个递减的计数器，旧的数值大于新的数值
            }
            else
            {
                tcnt += reload - tnow + told; // 新数值大于旧数值说明递减到0，重新从 reload 开始递减了
            }

            told = tnow;

            if (tcnt >= ticks)
            {
                break; // 时间超过/等于要延迟的时间,则退出.
            }
        }
    }
}
#endif
