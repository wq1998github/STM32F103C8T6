#ifndef __W25Q32_H__
#define __W25Q32_H__

#include "bsp_simu_spi.h"

void W25Q32_init(void);
void W25Q32_Get_MID_DID(uint8_t *MID, uint16_t *DID);

#endif
