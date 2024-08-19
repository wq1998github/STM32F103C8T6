#include "bsp_simu_spi.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

static BSP_SIMU_SPI_Mode s_spi_mode = BSP_SIMU_SPI_Mode_0;

/**
 * @brief  设置 SPI 模式
 * @param  mode
 */
void bsp_simu_spi_mode_set(BSP_SIMU_SPI_Mode mode)
{
    s_spi_mode = mode;
}

/**
 * @brief  获取 SPI 模式
 * @return BSP_SIMU_SPI_Mode
 */
BSP_SIMU_SPI_Mode bsp_simu_spi_mode_get(void)
{
    return s_spi_mode;
}

/*
    1. 接线说明：ABrobot 开发板板载 W25Q32，接线如下
        PB12 --> CS
        PB13 --> CLK
        PB14 --> DO (MISO)
        PB15 --> DI (MOSI)
*/

void bsp_simu_spi_init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 根据 MCU 和 W25Q32 的接线，开启 GPIOB 外设时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 初始化 PB12 --> CS
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 初始化 PB13 --> CLK
    // GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    // GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 初始化 PB15 --> MOSI
    // GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
    // GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 初始化 PB14 --> MISO
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    switch (s_spi_mode)
    {
    case BSP_SIMU_SPI_Mode_0:
    case BSP_SIMU_SPI_Mode_1:
        SIMU_SPI_CLK_0; // 空闲时刻是低电平
        break;

    case BSP_SIMU_SPI_Mode_2:
    case BSP_SIMU_SPI_Mode_3:
        SIMU_SPI_CLK_1; // 空闲时刻是高电平
        break;
    default:
        break;
    }

    // 失能所有 SPI 从设备
    SIMU_SPI_SS1_DIS;

    // MOSI 初始化状态，SPI 协议没有硬性规定
    SIMU_SPI_MOSI_0;
}

void bsp_simu_spi_deinit()
{
    // 不需要使能 GPIO 配置，重新初始化即可
    // https://www.armbbs.cn/forum.php?mod=viewthread&tid=88876
}

static uint8_t ss_mutex = 0;

void bsp_simu_spi_start(BSP_SIMU_SPI_SS ss_index)
{
    if (ss_mutex > 0)
    {
        return;
    }

    ss_mutex++;

    switch (ss_index)
    {
    case BSP_SIMU_SPI_SS_0:
        SIMU_SPI_SS1_EN;
        break;

    default:
        break;
    }
}

void bsp_simu_spi_stop(BSP_SIMU_SPI_SS ss_index)
{
    if (ss_mutex == 0)
    {
        return;
    }

    ss_mutex--;

    switch (ss_index)
    {
    case BSP_SIMU_SPI_SS_0:
        SIMU_SPI_SS1_DIS;
        break;

    default:
        break;
    }
}

uint8_t bsp_simu_spi_swap_byte(uint8_t swap_byte)
{
    uint8_t index = 0;

    switch (s_spi_mode)
    {
    case BSP_SIMU_SPI_Mode_0:
    case BSP_SIMU_SPI_Mode_3:
    {
        for (index = 0; index < 8; index++)
        {
            // 1. 模式0 要提前放置 MSB
            if (swap_byte & 0x80)
            {
                SIMU_SPI_MOSI = 1;
            }
            else
            {
                SIMU_SPI_MOSI = 0;
            }
            swap_byte <<= 1; // 为发送下一个 bit 位做准备
            SIMU_SPI_DELAY(5);

            // 2. 模式0 CLK 空闲时为低电平，因此这里要拉高；模式3 CLK 空闲时为高电平，因此这里要拉低
            // SIMU_SPI_CLK = (s_spi_mode + 1) % 2; // 模式0 这里置1，模式3 这里置0
            if (s_spi_mode == BSP_SIMU_SPI_Mode_0)
            {
                SIMU_SPI_CLK = 1;
            }
            else
            {
                SIMU_SPI_CLK = 0;
            }

            // 3. 从 MISO 中接收数据
            if (SIMU_SPI_MISO == 1)
            {
                swap_byte |= (uint8_t)0x01;
            }
            SIMU_SPI_DELAY(5);

            // 4. 准备接收下一个 bit 位
            // SIMU_SPI_CLK = s_spi_mode % 2; // 模式0 这里置0，模式3 这里置1
            if (s_spi_mode == BSP_SIMU_SPI_Mode_0)
            {
                SIMU_SPI_CLK = 0;
            }
            else
            {
                SIMU_SPI_CLK = 1;
            }
        }
    }
        break;

    case BSP_SIMU_SPI_Mode_1:
    case BSP_SIMU_SPI_Mode_2:
    {
        // 1. 修改 CLK，模式1 空闲时为低电平，因此这里拉高；模式2 空闲时为高电平，因此这里拉低
        if (s_spi_mode == BSP_SIMU_SPI_Mode_1)
        {
            SIMU_SPI_CLK = 1;
        }
        else
        {
            SIMU_SPI_CLK = 0;
        }

        // 2. MOSI 输出一 bit 数据
        if (swap_byte & 0x80)
        {
            SIMU_SPI_MOSI = 1;
        }
        else
        {
            SIMU_SPI_MOSI = 0;
        }
        swap_byte <<= 1;
        SIMU_SPI_DELAY(5);

        // 3. 修改 CLK，模式1 改成低电平；模式2 改成高电平
        if (s_spi_mode == BSP_SIMU_SPI_Mode_1)
        {
            SIMU_SPI_CLK = 0;
        }
        else
        {
            SIMU_SPI_CLK = 1;
        }

        // 4. 从 MISO 中读取一 bit 数据
        if (SIMU_SPI_MISO == 1)
        {
            swap_byte |= (uint8_t)0x01;
        }
        SIMU_SPI_DELAY(5);
    }
        break;

#if 0
    case BSP_SIMU_SPI_Mode_2:
        break;


    case BSP_SIMU_SPI_Mode_3:
    {
        // 1. 发送一个 bit
        if (swap_byte & 0x80)
        {
            SIMU_SPI_MOSI = 1;
        }
        else
        {
            SIMU_SPI_MOSI = 0;
        }

        swap_byte <<= 1;

        // 2. 模式3 CLK 空闲时为高电平，因此这里要拉低
        SIMU_SPI_DELAY(5);
        SIMU_SPI_CLK = 0;

        // 3. 接收一个 bit
        if (SIMU_SPI_MISO == 1)
        {
            swap_byte |= (uint8_t)0x01;
        }

        // 4. 为接收下一个 bit 做准备
        SIMU_SPI_DELAY(5);
        SIMU_SPI_CLK = 1;
    }
        break;
#endif
    default:
        break;
    }

    return swap_byte;
}
