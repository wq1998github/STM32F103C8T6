#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"

#include "bsp_adc.h"

#if (CONTINUOUS == 0)

/// @brief 单次、不连续扫描 -- 这里使用的是 ADC1 通道 1
/// @param
void bsp_adc_init(void)
{
    // Open the relative clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // 72 / 6 = 12 MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    // Config the GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN; // Analog Input
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Config the ADC Channel
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_28Cycles5);
    // eg: ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_28Cycles5);

    // Config the ADC
    ADC_InitTypeDef ADC_InitStruct = {0};
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 1; // unused in single mode, useful in scan mode only
    ADC_Init(ADC1, &ADC_InitStruct);

    // Enable ADC
    ADC_Cmd(ADC1, ENABLE);

    // Calibration
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET)
    {
    }
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET)
    {
    }
}

uint16_t bsp_adc_get_value(void)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET)
    {
    }

    return ADC_GetConversionValue(ADC1);
}

// #else

/////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief 单次、连续扫描 -- 这里使用的是 ADC2 通道 0
/// @param
void bsp_adc_continuous_init(void)
{
    // Open the relative clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

    // 72 / 6 = 12 MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    // Config the GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN; // Analog Input
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Config the ADC Channel
    ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5);
    // eg: ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_28Cycles5);

    // Config the ADC
    ADC_InitTypeDef ADC_InitStruct = {0};
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 1; // unused in single mode, useful in scan mode only
    ADC_Init(ADC2, &ADC_InitStruct);

    // Enable ADC
    ADC_Cmd(ADC2, ENABLE);

    // Calibration
    ADC_ResetCalibration(ADC2);
    while (ADC_GetResetCalibrationStatus(ADC2) == SET)
    {
    }
    ADC_StartCalibration(ADC2);
    while (ADC_GetCalibrationStatus(ADC2) == SET)
    {
    }

    // 连续转换，启动一次即可
    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
}

uint16_t bsp_adc_get_continuous_value(void)
{
    return ADC_GetConversionValue(ADC2);
}

#endif
