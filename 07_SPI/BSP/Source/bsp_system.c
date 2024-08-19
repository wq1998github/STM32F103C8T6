#include "bsp_system.h"

void NVIC_Configuration(void)
{
#if (SYSTEM_SUPPORT_OS == 0)
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级
#else
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
#endif
}
