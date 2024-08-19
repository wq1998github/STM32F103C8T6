#ifndef __BSP_SIMU_SPI_H__
#define __BSP_SIMU_SPI_H__

#include "stm32f10x.h"
#include "bsp_system.h"
#include "bsp_systick.h"

typedef enum
{
    BSP_SIMU_SPI_SS_0 = 0,  /* W25Q32 */
    BSP_SIMU_SPI_SS_1 = 1,
    BSP_SIMU_SPI_SS_2 = 2,

    /****************************************/
    BSP_SIMU_SPI_SS_Num
} BSP_SIMU_SPI_SS; /* Slave Select Index */

typedef enum
{
    BSP_SIMU_SPI_Mode_0 = 0,
    BSP_SIMU_SPI_Mode_1 = 1,
    BSP_SIMU_SPI_Mode_2 = 2,
    BSP_SIMU_SPI_Mode_3 = 3,
} BSP_SIMU_SPI_Mode;

// 延时函数
#define SIMU_SPI_DELAY(nus) delay_us(nus)

// SIMU_SPI --> CLK
#define SIMU_SPI_CLK    PBout(13)
#define SIMU_SPI_CLK_0  PBout(13) = 0
#define SIMU_SPI_CLK_1  PBout(13) = 1

// SIMU_SPI --> MOSI
#define SIMU_SPI_MOSI   PBout(15)
#define SIMU_SPI_MOSI_0 PBout(15) = 0
#define SIMU_SPI_MOSI_1 PBout(15) = 1

// SIMU_SPI --> MISO
#define SIMU_SPI_MISO   PBin(14)

// SIMU_SPI --> SS1
#define SIMU_SPI_SS1_EN  PBout(12) = 0  /* 低电平使能 */
#define SIMU_SPI_SS1_DIS PBout(12) = 1  /* 高电平失能 */

/**
 * @brief  设置 SPI 模式，未设置默认为 模式0
 * @param  mode SPI 模式
 */
void bsp_simu_spi_mode_set(BSP_SIMU_SPI_Mode mode);

/**
 * @brief  获取 SPI 模式
 * @return BSP_SIMU_SPI_Mode 当前的 SPI 模式
 */
BSP_SIMU_SPI_Mode bsp_simu_spi_mode_get(void);

/**
 * @brief  模拟 SPI 初始化
 */
void bsp_simu_spi_init(void);

void bsp_simu_spi_deinit(void);

/**
 * @brief  模拟 SPI 开始通讯
 * @param  ss_index 
 */
void bsp_simu_spi_start(BSP_SIMU_SPI_SS ss_index);

/**
 * @brief  模拟 SPI 结束通讯
 * @param  ss_index 
 */
void bsp_simu_spi_stop(BSP_SIMU_SPI_SS ss_index);

/**
 * @brief  模拟 SPI 置换一字节数据
 * @param  swap_byte 
 * @return uint8_t 
 */
uint8_t bsp_simu_spi_swap_byte(uint8_t swap_byte);

#endif
