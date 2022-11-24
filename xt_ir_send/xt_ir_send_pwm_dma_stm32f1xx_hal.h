/**
  * @file  xt_ir_send_pwm_dma_stm32f1xx_hal.h
  * @brief 红外发送服务模块硬件层--STM32F103单片机-HAL库（用到硬件资源：TIM_PWM+DMA）
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-11-16     o2ospring    原始版本
  *                             1.其原理是通过DMA将RAM缓冲的PWM占空比数据输出至TIM_PWM通道；
  *                             2.开辟的RAM越大则产生DMA中断就越少，即：以空间换取执行效率；
  *                             3.上层设置PWM频率越高则DMA中断频率也越高，要合理配置RAM大小
  */
#ifndef XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_H__
#define XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_H__

#include <stdint.h> //////////////////////// <- 使用的数据定义，如: int8_t, uint32_t 等
#ifdef XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_C__
#include <string.h> //////////////////////// <- 使用的字符处理，如: strcpy(), memcpy() 等
#include "application.h" /////////////////// <- 工程所有软件资源管理头文件!!!!!!!!!!!!!!!
#include "board.h"   /////////////////////// <- 工程所有硬件资源管理头文件!!!!!!!!!!!!!!!
#include "stm32f1xx.h" ///////////////////// <- 依赖STM32F103单片机HAL库
#include "xt_ir_send.h" //////////////////// <- 依赖红外发送服务模块
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ 引脚定义 ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

#ifdef XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_C__ // <- 只有[xt_ir_send_pwm_dma_stm32f1xx_hal.c]才会包含引脚定义!!!!
#ifndef XT_IRSEND_TIM  /////////////////////// <- 如果其它模块[没有]定义控制引脚，则本文件才启用引脚宏定义!!!!

// 配置PWM的频率
#define XT_IRSEND_TIM_DIV            1                                      /* 定时器的时钟分频         */
#define XT_IRSEND_TIM_PWM_HZ(hz)   ((72000000/(XT_IRSEND_TIM_DIV+1)/(hz))-1)/* PWM 100%数值(≤0xFFFF)   */

// 配置PWM的定时器
#define XT_IRSEND_TIM                TIM3                               /* 定时器                       */
#define XT_IRSEND_TIM_CLK_DI()     __HAL_RCC_TIM3_CLK_DISABLE()         /* 定时器时钟禁能               */
#define XT_IRSEND_TIM_CLK_EN()     __HAL_RCC_TIM3_CLK_ENABLE()          /* 定时器时钟使能               //
#define XT_IRSEND_TIM_AFIO_REMAP() __HAL_AFIO_REMAP_TIM3_PARTIAL()      // 定时器引脚重映射(不用则屏蔽) */ //只有 TIM1~5 才有映射 (分为:重映射[TIM4~5]、部分和完全重映射TIM1~3)

// 配置PWM的通道（只使用1路）
#define XT_IRSEND_PWM_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()         /* PWM 管脚时钟使能             */
#define XT_IRSEND_PWM_GPIO           GPIOB                              /* PWM 所在端口                 */
#define XT_IRSEND_PWM_PIN            GPIO_PIN_1                         /* PWM 所在管脚                 */
#define XT_IRSEND_PWM_OCMODE         TIM_OCMODE_PWM1 /*不能改*/         /* PWM 输出极性模式(2则反极性)  */ //使用在:非0%,非停止
#define XT_IRSEND_PWM_OCPOLARITY     TIM_OCPOLARITY_HIGH                /* PWM 有效占空比输出的电平     // //正向通道
#define XT_IRSEND_PWM_OCIDLESTATE    TIM_OCIDLESTATE_RESET              // PWM 空闲时电平(不用则屏蔽)   */ //只针对TIM1,其它定时器默认为低电平
#define XT_IRSEND_PWM_CCR            CCR4                               /* PWM 占空比寄存器(TIM3->CCR4) */
#define XT_IRSEND_PWM_TIM_CHANNEL    TIM_CHANNEL_4                      /* PWM 所在定时器通道           */
#define XT_IRSEND_TIM_DMA_ID_CC      TIM_DMA_ID_CC4                     /* 定时器DMA更新请求通道(如:CC4)*/
#define XT_IRSEND_TIM_DMA_CC         TIM_DMA_CC4                        /* 定时器DMA更新事件通道(如:CC4)*/

// 配置PWM的DMA
#define XT_IRSEND_DMA_CLK_ENABLE() __HAL_RCC_DMA1_CLK_ENABLE()          /* DMA 时钟使能                 */
#define XT_IRSEND_DMA_CHANNEL        DMA1_Channel3                      /* DMA 通道                     */
#define XT_IRSEND_DMA_IRQn           DMA1_Channel3_IRQn                 /* DMA 中断通道                 */
#define XT_IRSEND_DMA_PRE_INT_PRIO   0                                  /* DMA 通道抢占中断优先级       */
#define XT_IRSEND_DMA_SUB_INT_PRIO   0                                  /* DMA 通道响应中断优先级       */
#define XT_IRSEND_DMA_PRIORITY       DMA_PRIORITY_MEDIUM                /* DMA 通道优先级（中等）       */
#define XT_IRSEND_DMA_IRQHandler     DMA1_Channel3_IRQHandler           /* DMA 中断向量函数             */
#define XT_IRSEND_RAM_SUM            65 /*以空间换取执行效率*/          /* DMA 转传数据源大小(数据个数) */

/* 补充：STM32F103 DMA 资源介绍
DMA1_Channel1: ADC1,TIM2_CH3,TIM4_CH1
DMA1_Channel2: SPI1_RX,USART3_TX,TIM1_CH1,TIM2_UP,TIM3_CH3
DMA1_Channel3: SPI1_TX,USART3_RX,TIM1_CH2,TIM3_CH4/TIM3_UP
DMA1_Channel4: SPI/I2S2_RX,USART1_TX,I2C2_TX,TIM1_TX4/TIM1_TRIG/TIM1_COM,TIM4_CH2
DMA1_Channel5: SPI/I2S2_TX,USART1_RX,I2C2_RX,TIM1_UP,TIM2_CH1,TIM4_CH3
DMA1_Channel6: USART2_RX,I2C1_TX,TIM1_CH3,TIM3_CH1/TIM3_TRIG
DMA1_Channel7: USART2_TX,I2C1_RX,TIM2_CH2/TIM2_CH4,TIM4_UP
DMA2_Channel1: SPI/I2S3_RX,TIM5_CH4/TIM5_TRIG,TIM8_CH3/TIM8_UP
DMA2_Channel2: SPI/I2S3_TX,TIM5_CH3/TIM5_UP,TIM8_CH4/TIM8_TRIG,TIM8_COM
DMA2_Channel3: UART4_RX,TIM6_UP/DAC_CH1,TIM8_CH1
DMA2_Channel4: SDIO,TIM5_CH2,TIM7_UP/DAC_CH2
DMA2_Channel5: ADC3,UART4_TX,TIM5_CH1,TIM8_CH2
*/
#endif ////////////////////////////////////
#endif ///////////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ 操作函数 ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

//被驱动层直接调用的一个函数：打开硬件，是[驱动层]对接[硬件层]超始通道，并由它来提供其它对接口
//extern int xt_irsend_hw_open(xt_irsend_obj_t *p_ob);

extern void xt_irsend_hw_init(void);

#ifdef __cplusplus
	}
#endif

#endif  //#ifndef XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_H__
