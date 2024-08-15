/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#include "bsp_systick.h"
#include "bsp_pwm.h"
#include "bsp_uart.h"

void board_init(void)
{
    bsp_systick_init();
    uart1_init(115200);
    bsp_pwm_init();
}

int main(void)
{
    board_init();

    /* Infinite loop */
    while (1)
    {
        for (int i = 0; i < 100; i++)
        {
            bsp_pwm_set_duty_cycle(i);
            delay_ms(10);
        }
        for (int i = 0; i < 100; i++)
        {
            bsp_pwm_set_duty_cycle(100 - i);
            delay_ms(10);
        }
    }
}
