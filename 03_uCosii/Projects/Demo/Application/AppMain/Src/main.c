/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#include "bsp_systick.h"
#include "bsp_led.h"
#include "bsp_uart.h"

#include "ucos_ii.h"

void board_init(void)
{
    bsp_systick_init();
    led_init();
    uart1_init(115200);
}

#define TASK_LED_PRIO 5
#define TASK_LED_STACK_SIZE 128
static OS_STK task_led_stack[TASK_LED_STACK_SIZE];

static void task_led_entry(void *p_arg)
{
    while (1)
    {
        led_control(BSP_LED_ON);
        OSTimeDly(1000);
        led_control(BSP_LED_OFF);
        OSTimeDly(1000);
    }
}

void led_task_init(void)
{
    OSTaskCreate(task_led_entry, (void *)0, &task_led_stack[TASK_LED_STACK_SIZE - 1], TASK_LED_PRIO);
}

#define TASK_UART1_PRIO 6
#define TASK_UART1_STACK_SIZE 128
static OS_STK task_uart1_stack[TASK_UART1_STACK_SIZE];

static void task_uart1_entry(void *p_arg)
{
    while (1)
    {
        printf("hello world\r\n");
        OSTimeDly(1000);
    }
}

void uart1_task_init(void)
{
    OSTaskCreate(task_uart1_entry, (void *)0, &task_uart1_stack[TASK_UART1_STACK_SIZE - 1], TASK_UART1_PRIO);
}

int main(void)
{
    board_init();

    OSInit();
    led_task_init();
    uart1_task_init();
    OSStart();

    /* Infinite loop */
    while (1)
    {
        // led_control(BSP_LED_ON);
        // printf("LED ON!!!\r\n");
        // delay_ms(1000);

        // led_control(BSP_LED_OFF);
        // printf("LED OFF!!!\r\n");
        // delay_ms(1000);
    }
}
