/**
  * @file  xt_wiznet_hspi_stm32f1xx_hal.h
  * @brief WIZnet网络芯片引脚驱动(使用硬件SPI控制器)--STM32F103单片机-HAL库
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-10     o2ospring    原始版本
  */
#ifndef XT_WIZNET_HSPI_STM32F1XX_HAL_H__
#define XT_WIZNET_HSPI_STM32F1XX_HAL_H__

#include <stdint.h> //////////////////////// <- 使用的数据定义，如: int8_t, uint32_t 等
#ifdef XT_WIZNET_HSPI_STM32F1XX_HAL_C__
#include <string.h> //////////////////////// <- 使用的字符处理，如: strcpy(), memcpy() 等
#include "application.h" /////////////////// <- 工程所有软件资源管理头文件!!!!!!!!!!!!!!!
#include "board.h"   /////////////////////// <- 工程所有硬件资源管理头文件!!!!!!!!!!!!!!!
#include "stm32f1xx.h" ///////////////////// <- 依赖STM32F103单片机HAL库
#include "xt_wiznet.h" ///////////////////// <- 依赖网络服务模块
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ 引脚定义 ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

#ifdef XT_WIZNET_HSPI_STM32F1XX_HAL_C__ // <- 只有[xt_wiznet_hspi_stm32f1xx_hal.c]文件才会包含控制引脚定义!!
#ifndef XT_WIZ_SPI /////////////////////// <- 如果其它模块[没有]定义芯片控制引脚，则本文件才启用宏定义!!!!!!

// WIZnet网络芯片SPI总线控制延时
#define XT_WIZ_SPI_NSS_EN_HOLD_TM()                                     /* NSS 使能时保持时间(可不延时) */
#define XT_WIZ_SPI_NSS_DI_HOLD_TM()  bsp_delay0us125(2)                 /* NSS 禁止时保持时间(延时50nS) */

// WIZnet网络芯片控制管脚的定义
#define XT_WIZ_SPI                   SPI1                               /* 使用的哪个SPI                */
#define XT_WIZ_SPI_BAUD_RATE         SPI_BAUDRATEPRESCALER_4            /* 速度(分频): 72/4=18Mbit/S    */
#define XT_WIZ_SPI_CLK_ENABLE()    __HAL_RCC_SPI1_CLK_ENABLE()          /* 串口时钟使能                 */
#define XT_WIZ_SPI_CMS_CLK_EN()    __HAL_RCC_GPIOA_CLK_ENABLE()         /* SCK-MOSI-MISO 脚时钟使能     */
#define XT_WIZ_SPI_NSS_CLK_EN()    __HAL_RCC_GPIOA_CLK_ENABLE()         /* NSS  脚时钟使能              */
#define XT_WIZ_SPI_NSS_GPIO          GPIOA                              /* NSS  所在端口(NSS由软件控制) */
#define XT_WIZ_SPI_NSS_PIN           GPIO_PIN_4                         /* NSS  所在管脚(NSS由软件控制) */
#define XT_WIZ_SPI_SCK_GPIO          GPIOA                              /* SCK  所在端口                */
#define XT_WIZ_SPI_SCK_PIN           GPIO_PIN_5                         /* SCK  所在管脚                */
#define XT_WIZ_SPI_MISO_GPIO         GPIOA                              /* MISO 所在端口                */
#define XT_WIZ_SPI_MISO_PIN          GPIO_PIN_6                         /* MISO 所在管脚                */
#define XT_WIZ_SPI_MOSI_GPIO         GPIOA                              /* MOSI 所在端口                */
#define XT_WIZ_SPI_MOSI_PIN          GPIO_PIN_7                         /* MOSI 所在管脚                */
#define XT_WIZ_SPI_RX_IN_MODE        GPIO_NOPULL                        /* MISO 输入模式(下拉/上拉/悬空)*/
#define XT_WIZ_SPI_DIRECTION_LINES   SPI_DIRECTION_2LINES               /* 收发使用几线(全双工/半双工)  //
#define XT_WIZ_SPI_AFIO_REMAP()    __HAL_AFIO_REMAP_SPI1_ENABLE()       // SPI  引脚重映射(不使用则屏蔽)*/
#define XT_WIZ_SPI_HANDLER           xt_wiz_hspi                        /* SPI  句柄                    */
#define XT_WIZ_RST_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()         /* RST  所在外设(使能外设时钟)  */
#define XT_WIZ_RST_GPIO              GPIOB                              /* RST  所在端口                */
#define XT_WIZ_RST_PIN               GPIO_PIN_0                         /* RST  所在管脚                */
#define XT_WIZ_RST_EN()              XT_WIZ_RST_GPIO->BRR      = XT_WIZ_RST_PIN     // RST有效
#define XT_WIZ_RST_DI()              XT_WIZ_RST_GPIO->BSRR     = XT_WIZ_RST_PIN     // RST无效
#define XT_WIZ_SPI_NSS_EN()          XT_WIZ_SPI_NSS_GPIO->BRR  = XT_WIZ_SPI_NSS_PIN // NSS有效
#define XT_WIZ_SPI_NSS_DI()          XT_WIZ_SPI_NSS_GPIO->BSRR = XT_WIZ_SPI_NSS_PIN // NSS无效

#endif //////////////////////////////
#endif ///////////////////////////////////////////////////////////////////////////////////////////////////

extern void xt_wiz_hspi_rst_en(void); ///////
extern void xt_wiz_hspi_rst_di(void); ///////
extern void xt_wiz_hspi_gpio_init(void);
extern void xt_wiz_hspi_cs_en(void);
extern void xt_wiz_hspi_cs_di(void);
extern void xt_wiz_hspi_w_byte(uint8_t byte);
extern uint8_t xt_wiz_hspi_r_byte(void);
extern void xt_wiz_hspi_w_nbyte(uint8_t *pbuf, uint16_t size);
extern void xt_wiz_hspi_r_nbyte(uint8_t *pbuf, uint16_t size);

#ifdef __cplusplus
	}
#endif

#endif  //XT_WIZNET_HSPI_STM32F1XX_HAL_H__
