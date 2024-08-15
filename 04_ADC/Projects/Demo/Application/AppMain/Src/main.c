/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#include "bsp_systick.h"
#include "bsp_adc.h"
#include "bsp_uart.h"

void board_init(void)
{
    bsp_systick_init();
    uart1_init(115200);
#if (CONTINUOUS == 0)
    bsp_adc_init();
// #else
    bsp_adc_continuous_init();
#endif
}

int main(void)
{
    uint16_t adc_value = 0;
    board_init();

    /* Infinite loop */
    while (1)
    {
#if (CONTINUOUS == 0)
        adc_value = bsp_adc_get_value();
        printf("ADC Value: %d\r\n", adc_value);
        delay_ms(1000);
// #else
        adc_value = bsp_adc_get_continuous_value();
        printf("ADC Continuous Value: %d\r\n", adc_value);
        delay_ms(1000);
#endif
    }
}
