/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "misc.h"
#include <stdio.h>

#include "os_delay.h"
#include "bsp_led.h"
#include "bsp_uart.h"

#include "FreeRTOS.h"
#include "task.h"

void vTaskLED(void *pvParameters)
{
    uint8_t dir = 0;
    uint16_t bright = 0;
    bsp_led_init();

    while (1)
    {
        switch (dir)
        {
        case 0:
        {
            BSP_LED_RED_BRIGHT(bright);
            BSP_LED_GREEN_BRIGHT(bright);
            BSP_LED_BLUE_BRIGHT(99 - bright);
            BSP_LED_YELLOW_BRIGHT(99 - bright);

            bright++;

            if (bright > 99)
            {
                dir = 1;
            }
        }
            break;
        
        case 1:
        {
            BSP_LED_RED_BRIGHT(bright);
            BSP_LED_GREEN_BRIGHT(bright);
            BSP_LED_BLUE_BRIGHT(99 - bright);
            BSP_LED_YELLOW_BRIGHT(99 - bright);

            bright--;

            if (bright == 0)
            {
                dir = 0;
            }
        }
            break;
        default:
            break;
        }

        delay_ms(10);
    }
}

void vTaskUart(void *pvParameters)
{
    uart1_init(115200);

    while (1)
    {
        printf("Hello World\r\n");
        delay_ms(1000);
    }
}

int main(void)
{
    BaseType_t xHandle = NULL;
    OS_SysTick_init();

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // FreeRTOS 要求

    xHandle = xTaskCreate(vTaskLED, "LED", 100, (void *)0, 1, NULL);
    xHandle = xTaskCreate(vTaskUart, "Uart", 100, (void *)0, 1, NULL);

    (void)xHandle;

    vTaskStartScheduler();

    //while (1);

    return 0;
}
