/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "misc.h"
#include <stdio.h>

#include "bsp_systick.h"
#include "bsp_led.h"
#include "bsp_uart.h"

#include "FreeRTOS.h"
#include "task.h"

void vTaskLED(void *pvParameters)
{
    led_init();

    while (1)
    {
        led_control(BSP_LED_ON);
        delay_ms(1000);
        led_control(BSP_LED_OFF);
        delay_ms(1000);
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
    bsp_systick_init();

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // FreeRTOS 要求

    xHandle = xTaskCreate(vTaskLED, "LED", 100, (void *)0, 1, NULL);
    xHandle = xTaskCreate(vTaskUart, "Uart", 100, (void *)0, 1, NULL);

    (void)xHandle;

    vTaskStartScheduler();

    //while (1);

    return 0;
}
