/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#include "bsp_systick.h"
#include "bsp_led.h"
#include "bsp_uart.h"

#include "W25Q32/w25q32.h"

void board_init(void)
{
    bsp_systick_init();
    // led_init();
    uart1_init(115200);

    W25Q32_init();
}

int main(void)
{
    uint8_t MID = 0;
    uint16_t DID = 0;
    board_init();

    W25Q32_Get_MID_DID(&MID, &DID);

    /* Infinite loop */
    while (1)
    {
        printf("MID: 0x%02X\r\n", MID);
        printf("DID: 0x%02X\r\n", DID);
        delay_ms(1000);
    }
}
