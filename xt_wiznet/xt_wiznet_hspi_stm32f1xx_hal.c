/**
  * @file  xt_wiznet_hspi_stm32f1xx_hal.c
  * @brief WIZnet网络芯片引脚驱动(使用硬件SPI控制器)--STM32F103单片机-HAL库
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-10     o2ospring    原始版本
  */
#define   XT_WIZNET_HSPI_STM32F1XX_HAL_C__
#include "xt_wiznet_hspi_stm32f1xx_hal.h"
#if (XT_NET_HW_DRIVERS_EN == 1)
#if (defined XT_APP_WIZNET_EN) && (XT_APP_WIZNET_EN == XT_DEF_ENABLED)
#ifdef    XT_WIZNET_HSPI_STM32F1XX_HAL_X__
#undef    XT_WIZNET_HSPI_STM32F1XX_HAL_H__
#include "xt_wiznet_hspi_stm32f1xx_hal.h"
#endif

SPI_HandleTypeDef  xt_wiz_hspi;

// 被上层调用的几个函数
void xt_wiz_hspi_rst_en(void); ///////////////////////////////////////////////////////////////////////////
void xt_wiz_hspi_rst_di(void); ///////////////////////////////////////////////////////////////////////////
void xt_wiz_hspi_gpio_init(void);
void xt_wiz_hspi_cs_en(void);
void xt_wiz_hspi_cs_di(void);
void xt_wiz_hspi_w_byte(uint8_t byte);
uint8_t xt_wiz_hspi_r_byte(void);
void xt_wiz_hspi_w_nbyte(uint8_t *pbuf, uint16_t size);
void xt_wiz_hspi_r_nbyte(uint8_t *pbuf, uint16_t size);

/**
  * @brief  RST引脚控制
  * @param  void
  * @return void
  */
void xt_wiz_hspi_rst_en(void) ////////////////////////////////////////////////////////////////////////////
{                             ////////////////////////////////////////////////////////////////////////////
	XT_WIZ_RST_EN();          ////////////////////////////////////////////////////////////////////////////
}                             ////////////////////////////////////////////////////////////////////////////
void xt_wiz_hspi_rst_di(void) ////////////////////////////////////////////////////////////////////////////
{                             ////////////////////////////////////////////////////////////////////////////
	XT_WIZ_RST_DI();          ////////////////////////////////////////////////////////////////////////////
}                             ////////////////////////////////////////////////////////////////////////////

/**
  * @brief  SPI总线引脚初始化
  * @param  void
  * @return void
  */
void xt_wiz_hspi_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	XT_WIZ_SPI_NSS_CLK_EN();
	XT_WIZ_SPI_CMS_CLK_EN();
	XT_WIZ_SPI_CLK_ENABLE();
	XT_WIZ_RST_CLK_ENABLE(); /////////////////////////////////////////////////////////////////////////////
	
	XT_WIZ_SPI_NSS_DI();
	XT_WIZ_RST_DI();                                  ////////////////////////////////////////////////////
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      ////////////////////////////////////////////////////
	GPIO_InitStruct.Pull  = GPIO_NOPULL;              ////////////////////////////////////////////////////
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;      ////////////////////////////////////////////////////
	GPIO_InitStruct.Pin   = XT_WIZ_RST_PIN;           ////////////////////////////////////////////////////
	HAL_GPIO_Init(XT_WIZ_RST_GPIO, &GPIO_InitStruct); ////////////////////////////////////////////////////
	
	GPIO_InitStruct.Pin = XT_WIZ_SPI_NSS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(XT_WIZ_SPI_NSS_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = XT_WIZ_SPI_SCK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(XT_WIZ_SPI_SCK_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = XT_WIZ_SPI_MOSI_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(XT_WIZ_SPI_MOSI_GPIO, &GPIO_InitStruct);
	
	#if (XT_WIZ_SPI_DIRECTION_LINES == SPI_DIRECTION_2LINES)
	GPIO_InitStruct.Pin = XT_WIZ_SPI_MISO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = XT_WIZ_SPI_RX_IN_MODE;
	HAL_GPIO_Init(XT_WIZ_SPI_MISO_GPIO, &GPIO_InitStruct);
	#endif
	
	#ifdef XT_WIZ_SPI_AFIO_REMAP
	XT_WIZ_SPI_AFIO_REMAP();
	#endif
	
	XT_WIZ_SPI_HANDLER.Instance = XT_WIZ_SPI;                            //第几SPI口
	XT_WIZ_SPI_HANDLER.Init.Mode = SPI_MODE_MASTER;                      //主从机选择(Master:主机模式)
	XT_WIZ_SPI_HANDLER.Init.Direction = XT_WIZ_SPI_DIRECTION_LINES;      //收发使用几线(2:全双工，1:半双工)
	XT_WIZ_SPI_HANDLER.Init.DataSize = SPI_DATASIZE_8BIT;                //数据长度是8bit还是16bit
	XT_WIZ_SPI_HANDLER.Init.CLKPolarity = SPI_POLARITY_HIGH;             //High:时钟非工作时为高电平，Low:则为低电平
	XT_WIZ_SPI_HANDLER.Init.CLKPhase = SPI_PHASE_2EDGE;                  //2Edge:时钟偶数跳变沿传输数据，1Edge:则为奇数跳变沿
	XT_WIZ_SPI_HANDLER.Init.NSS = SPI_NSS_SOFT;                          //由软件控制NSS片选动作
	XT_WIZ_SPI_HANDLER.Init.BaudRatePrescaler = XT_WIZ_SPI_BAUD_RATE;    //分频(速度)，如: 72/4=18Mbit/S
	XT_WIZ_SPI_HANDLER.Init.FirstBit = SPI_FIRSTBIT_MSB;                 //MSB:先传数据高位，LSB:先传数据低位
	XT_WIZ_SPI_HANDLER.Init.TIMode = SPI_TIMODE_DISABLE;
	XT_WIZ_SPI_HANDLER.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; //CRC校验关闭
	XT_WIZ_SPI_HANDLER.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&XT_WIZ_SPI_HANDLER) != HAL_OK) {}
	
	XT_WIZ_SPI_NSS_DI();
}

/**
  * @brief  SPI片选使能
  * @param  void
  * @return void
  */
void xt_wiz_hspi_cs_en(void)
{
	XT_WIZ_SPI_NSS_EN();
	XT_WIZ_SPI_NSS_EN_HOLD_TM();
}

/**
  * @brief  SPI片选禁能
  * @param  void
  * @return void
  */
void xt_wiz_hspi_cs_di(void)
{
	XT_WIZ_SPI_NSS_DI();
	XT_WIZ_SPI_NSS_DI_HOLD_TM();
}

/**
  * @brief  向SPI总线写入1字节数据
  * @param  byte      写入的1字节数据
  * @return void
  */
void xt_wiz_hspi_w_byte(uint8_t byte)
{
	HAL_SPI_Transmit(&XT_WIZ_SPI_HANDLER, &byte, 1, 20);
}

/**
  * @brief  从SPI总线读出1字节数据
  * @param  void
  * @return uint8_t   读到的1字节数据
  */
uint8_t xt_wiz_hspi_r_byte(void)
{
	uint8_t byte;
	HAL_SPI_Receive(&XT_WIZ_SPI_HANDLER, &byte, 1, 20);
	return byte;
}

/**
  * @brief  向SPI总线写入n字节数据
  * @param  *pbuf     要写入的数据缓存
  * @param  size      数据大小
  * @return void
  */
void xt_wiz_hspi_w_nbyte(uint8_t *pbuf, uint16_t size)
{
	HAL_SPI_Transmit(&XT_WIZ_SPI_HANDLER, pbuf, size, (uint32_t)size * 20U);
}

/**
  * @brief  从SPI总线读出n字节数据
  * @param  *pbuf     要读出的数据缓存
  * @param  size      数据大小
  * @return void
  */
void xt_wiz_hspi_r_nbyte(uint8_t *pbuf, uint16_t size)
{
	HAL_SPI_Receive(&XT_WIZ_SPI_HANDLER, pbuf, size, (uint32_t)size * 20U);
}

#endif //#if (XT_APP_WIZNET_EN == XT_DEF_ENABLED)
#endif
