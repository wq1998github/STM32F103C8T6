/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#include "bsp_systick.h"
#include "bsp_led.h"
#include "bsp_uart.h"

#include "W25Q32/w25q32.h"
#include "W25Q32/W25QXX.h"

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

#define countof(a) (sizeof(a) / sizeof(*(a)))

/* 获取缓冲区的长度 */
#define TxBufferSize1 (countof(TxBuffer1) - 1)
#define RxBufferSize1 (countof(TxBuffer1) - 1)

#define BufferSize (countof(Tx_Buffer) - 1)

#define FLASH_WriteAddress          0x00000
#define FLASH_ReadAddress           FLASH_WriteAddress
#define FLASH_SectorToErase         FLASH_WriteAddress

#define Erase 0
uint8_t Tx_Buffer[] = "123456";

uint8_t Rx_Buffer[BufferSize];
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;

TestStatus Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);

void board_init(void)
{
    bsp_systick_init();
    // led_init();
    uart1_init(115200);

    // W25Q32_init();
    SPI_FLASH_Init();
}

int main(void)
{
    __IO uint32_t Flash_Size = 0;

    board_init();

    /* 获取 Flash Device ID */
    DeviceID = SPI_FLASH_ReadDeviceID();

    /* 获取 SPI Flash ID */
    FlashID = SPI_FLASH_ReadID();
    printf("\r\n FlashID is 0x%X \r\n Device ID is 0x%X\r\n", FlashID, DeviceID);

    /* 检验 SPI Flash ID */
    if (FlashID == W25Q16ID || W25Q32ID || W25Q64ID || W25Q80ID)
    {
        printf("\r\n detect flash Chips!!!!\r\n");
        switch (FlashID)
        {
        case W25Q16ID:
            printf("\r\n flash Chip Type is W25Q16ID!\r\n");
            Flash_Size = 2;
            break;
        case W25Q32ID:
            printf("\r\n flash Chip Type is W25Q32!\r\n");
            Flash_Size = 4;
            break;
        case W25Q64ID:
            printf("\r\n flash Chip Type is W25Q64!\r\n");
            Flash_Size = 8;
            break;
        case W25Q80ID:
            printf("\r\n flash Chip Type is W25Q80!\r\n");
            Flash_Size = 1;
            break;
        default:
            printf("\r\n flash  Chip Type is unknown!!!!\r\n");
            Flash_Size = 0;
            break;
        }

        /* 擦除将要写入的 SPI FLASH 扇区，FLASH写入前要先擦除 */
        // 这里擦除4K，即一个扇区，擦除的最小单位是扇区
        SPI_FLASH_SectorErase(FLASH_SectorToErase);

        /* 将发送缓冲区的数据写到flash中 */
        // 这里写一页，一页的大小为256个字节
        SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
        printf("\r\n write data: %s \r\t", Tx_Buffer);

        /* 将刚刚写入的数据读出来放到接收缓冲区中 */
        SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
        printf("\r\n read data: %s \r\n", Rx_Buffer);

        /* 检查写入的数据与读出的数据是否相等 */
        TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);

        if (PASSED == TransferStatus1)
        {
            printf("\r\n flash test is success!\n\r");
        }
        else
        {
            printf("\r\n flash test is failure!\n\r");
        }
    } // if (FlashID == sFLASH_ID)
    else // if (FlashID == sFLASH_ID)
    {
        printf("\r\n cannot fetch W25Q64 ID!\n\r");
    }

    /* Infinite loop */
    while (1)
    {
        delay_ms(1000);
    }
}

TestStatus Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer1 != *pBuffer2)
        {
            return FAILED;
        }

        pBuffer1++;
        pBuffer2++;
    }
    return PASSED;
}
