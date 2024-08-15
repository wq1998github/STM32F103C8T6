#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "stm32f10x.h"

#define CONTINUOUS 0

void bsp_adc_init(void);
uint16_t bsp_adc_get_value(void);

void bsp_adc_continuous_init(void);
uint16_t bsp_adc_get_continuous_value(void);

#endif
