/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#include "misc.h"

#include "bsp_systick.h"
#include "bsp_timer.h"
#include "bsp_uart.h"

void NVIC_Config(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

void board_init(void)
{
    bsp_systick_init();
    uart1_init(115200);
    // bsp_timer_init();
    bsp_timer_extern_init();
}

int main(void)
{
    uint32_t last_tick = 0;
    uint32_t tick = 0;

    uint32_t last_cnt = 0;
    uint32_t cnt = 0;

    board_init();
    NVIC_Config();

    /* Infinite loop */
    while (1)
    {
        // tick = bsp_timer_get_tick();
        // if (tick != last_tick)
        // {
        //     last_tick = tick;
        //     printf("tick: %d\r\n", tick);
        // }

        cnt = bsp_timer_get_cnt();
        if (cnt != last_cnt)
        {
            last_cnt = cnt;
            printf("cnt: %d\r\n", cnt);
        }
    }
}
