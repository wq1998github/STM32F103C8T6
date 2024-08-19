#include "W25Q32/w25q32.h"

void W25Q32_init(void)
{
    bsp_simu_spi_mode_set(BSP_SIMU_SPI_Mode_0);
    bsp_simu_spi_init();
}

void W25Q32_Get_MID_DID(uint8_t *MID, uint16_t *DID)
{
    bsp_simu_spi_start(BSP_SIMU_SPI_SS_0);
    bsp_simu_spi_swap_byte(0x9F);
    *MID = bsp_simu_spi_swap_byte(0xFF);
    *DID = bsp_simu_spi_swap_byte(0xFF);
    *DID <<= 8;
    *DID |= bsp_simu_spi_swap_byte(0xFF);
    bsp_simu_spi_stop(BSP_SIMU_SPI_SS_0);
}
