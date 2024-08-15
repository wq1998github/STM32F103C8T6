#ifndef __BSP_PWM_H__
#define __BSP_PWM_H__

#include "stm32f10x.h"

void bsp_pwm_init(void);
void bsp_pwm_set_duty_cycle(uint8_t percent);

#endif
