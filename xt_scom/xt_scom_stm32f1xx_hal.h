/**
  * @file  xt_scom_stm32f1xx_hal.h
  * @brief 串行通信模块硬件层--STM32F103单片机-HAL库（用到硬件资源：串口+独立定时器）
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-11-27     o2ospring    原始版本
  *                             1.串口用于串行数据收发；
  *                             2.定时器用于两字节数据间隔过长检测，最长可达65ms，
  *                               时间精度可达1us，缺点是需要独占一个硬件定时器。
  */
#ifndef XT_SCOM_STM32F1XX_HAL_H__
#define XT_SCOM_STM32F1XX_HAL_H__

#include <stdint.h> //////////////////////// <- 使用的数据定义，如: int8_t, uint32_t 等
#ifdef XT_SCOM_STM32F1XX_HAL_C__
#include <string.h> //////////////////////// <- 使用的字符处理，如: strcpy(), memcpy() 等
#include "application.h" /////////////////// <- 工程所有软件资源管理头文件!!!!!!!!!!!!!!!
#include "board.h"   /////////////////////// <- 工程所有硬件资源管理头文件!!!!!!!!!!!!!!!
#include "stm32f1xx.h" ///////////////////// <- 依赖STM32F103单片机HAL库
#include "xt_scom.h" /////////////////////// <- 依赖串行通信服务模块
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ 引脚定义 ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

#ifdef XT_SCOM_STM32F1XX_HAL_C__ /////////// <- 只有[xt_scom_stm32f1xx_hal.c]源文件才会包含控制引脚定义!!!
#ifndef XT_SCOM1_UART  ///////////////////// <- 如果其它模块[没有]定义控制引脚，则本文件才启用引脚宏定义!!

// 串行通信模块-定时器
#define XT_SCOMx_TIM                 TIM1                               /* 定时器                       */
#define XT_SCOMx_TIM_CLK_EN()      __HAL_RCC_TIM1_CLK_ENABLE()          /* 定时器时钟使能               */
#define XT_SCOMx_TIM_IRQn            TIM1_CC_IRQn                       /* 定时器中断通道               */
#define XT_SCOMx_TIM_PRE_INT_PRIO    1                                  /* 定时器抢占中断优先级         */
#define XT_SCOMx_TIM_SUB_INT_PRIO    1                                  /* 定时器响应中断优先级         */
#define XT_SCOMx_TIM_CCR1            0xFFFF      /*一般不要改*/         /* 定时器CCR1                   */
#define XT_SCOMx_TIM_CCR2            0xFFFF      /*一般不要改*/         /* 定时器CCR2                   */
#define XT_SCOMx_TIM_CCR3            0xFFFF      /*一般不要改*/         /* 定时器CCR3                   */
#define XT_SCOMx_TIM_CCR4            0xFFFF      /*一般不要改*/         /* 定时器CCR4                   */
#define XT_SCOMx_TIM_TICK_US         1           /*绝对不要改*/         /* 定时器计数节拍(如:1->1us)    */
#define XT_SCOMx_TIM_IRQHandler      TIM1_CC_IRQHandler                 /* 中断向量函数                 */

// 串行通信模块-通道1（☆通道总数由[XT_SCOM_SUM]决定☆）
#define XT_SCOM1_UART                USART3                             /* 使用的哪个串口               */
#define XT_SCOM1_UART_CLK_EN()     __HAL_RCC_USART3_CLK_ENABLE()        /* 串口时钟使能                 */
#define XT_SCOM1_UART_CLK_DI()     __HAL_RCC_USART3_CLK_DISABLE()       /* 串口时钟禁能                 */
#define XT_SCOM1_UART_TX_CLK_EN()  __HAL_RCC_GPIOB_CLK_ENABLE()         /* TX脚时钟使能(不用发送则屏蔽) */
#define XT_SCOM1_UART_RX_CLK_EN()  __HAL_RCC_GPIOB_CLK_ENABLE()         /* RX脚时钟使能(不用接收则屏蔽) */
#define XT_SCOM1_UART_IRQn           USART3_IRQn                        /* 中断通道                     */
#define XT_SCOM1_UART_PRE_INT_PRIO   0                                  /* 抢占中断优先级               */
#define XT_SCOM1_UART_SUB_INT_PRIO   0                                  /* 响应中断优先级               */
#define XT_SCOM1_UART_TX_GPIO        GPIOB                              /* TX 所在端口                  */
#define XT_SCOM1_UART_TX_PIN         GPIO_PIN_10                        /* TX 所在管脚                  */
#define XT_SCOM1_UART_RX_GPIO        GPIOB                              /* RX 所在端口                  */
#define XT_SCOM1_UART_RX_PIN         GPIO_PIN_11                        /* RX 所在管脚                  */
#define XT_SCOM1_UART_RX_IN_MODE     GPIO_PULLUP                        /* RX 输入模式(上拉/悬空)       //
#define XT_SCOM1_UART_AFIO_REMAP() __HAL_AFIO_REMAP_USART3_ENABLE()     // 串口引脚重映射(不使用则屏蔽) // //只有 UART1~3 才有映射
#define XT_SCOM1_DE_CLK_EN()       __HAL_RCC_GPIOA_CLK_ENABLE()         // DE 端口时钟使能(RS485才使能) //
#define XT_SCOM1_DE_GPIO             GPIOA                              // DE 所在端口    (RS485才使能) //
#define XT_SCOM1_DE_PIN              GPIO_PIN_1                         // DE 所在管脚    (RS485才使能) //
#define XT_SCOM1_DE_EN()       XT_SCOM1_DE_GPIO->BSRR = XT_SCOM1_DE_PIN // 使能DE控制端   (RS485才使能) //
#define XT_SCOM1_DE_DI()       XT_SCOM1_DE_GPIO->BRR  = XT_SCOM1_DE_PIN // 禁能DE控制端   (RS485才使能) */
#define XT_SCOM1_UART_IRQHandler     USART3_IRQHandler                  /* 中断向量函数                 */

// 串行通信模块-通道2（☆通道总数由[XT_SCOM_SUM]决定☆）
#define XT_SCOM2_UART                UART4                              /* 使用的哪个串口               */
#define XT_SCOM2_UART_CLK_EN()     __HAL_RCC_UART4_CLK_ENABLE()         /* 串口时钟使能                 */
#define XT_SCOM2_UART_CLK_DI()     __HAL_RCC_UART4_CLK_DISABLE()        /* 串口时钟禁能                 */
#define XT_SCOM2_UART_TX_CLK_EN()  __HAL_RCC_GPIOC_CLK_ENABLE()         /* TX脚时钟使能(不用发送则屏蔽) */
#define XT_SCOM2_UART_RX_CLK_EN()  __HAL_RCC_GPIOC_CLK_ENABLE()         /* RX脚时钟使能(不用接收则屏蔽) */
#define XT_SCOM2_UART_IRQn           UART4_IRQn                         /* 中断通道                     */
#define XT_SCOM2_UART_PRE_INT_PRIO   0                                  /* 抢占中断优先级               */
#define XT_SCOM2_UART_SUB_INT_PRIO   0                                  /* 响应中断优先级               */
#define XT_SCOM2_UART_TX_GPIO        GPIOC                              /* TX 所在端口                  */
#define XT_SCOM2_UART_TX_PIN         GPIO_PIN_10                        /* TX 所在管脚                  */
#define XT_SCOM2_UART_RX_GPIO        GPIOC                              /* RX 所在端口                  */
#define XT_SCOM2_UART_RX_PIN         GPIO_PIN_11                        /* RX 所在管脚                  */
#define XT_SCOM2_UART_RX_IN_MODE     GPIO_PULLUP                        /* RX 输入模式(上拉/悬空)       //
#define XT_SCOM2_UART_AFIO_REMAP() __HAL_AFIO_REMAP_USART1_ENABLE()     // 串口引脚重映射(不使用则屏蔽) // //只有 UART1~3 才有映射
#define XT_SCOM2_DE_CLK_EN()       __HAL_RCC_GPIOA_CLK_ENABLE()         // DE 端口时钟使能(RS485才使能) //
#define XT_SCOM2_DE_GPIO             GPIOA                              // DE 所在端口    (RS485才使能) //
#define XT_SCOM2_DE_PIN              GPIO_PIN_2                         // DE 所在管脚    (RS485才使能) //
#define XT_SCOM2_DE_EN()       XT_SCOM2_DE_GPIO->BSRR = XT_SCOM2_DE_PIN // 使能DE控制端   (RS485才使能) //
#define XT_SCOM2_DE_DI()       XT_SCOM2_DE_GPIO->BRR  = XT_SCOM2_DE_PIN // 禁能DE控制端   (RS485才使能) */
#define XT_SCOM2_UART_IRQHandler     UART4_IRQHandler                   /* 中断向量函数                 */

// 串行通信模块-通道3（☆通道总数由[XT_SCOM_SUM]决定☆）
#define XT_SCOM3_UART                USART1                             /* 使用的哪个串口               */
#define XT_SCOM3_UART_CLK_EN()     __HAL_RCC_USART1_CLK_ENABLE()        /* 串口时钟使能                 */
#define XT_SCOM3_UART_CLK_DI()     __HAL_RCC_USART1_CLK_DISABLE()       /* 串口时钟禁能                 */
#define XT_SCOM3_UART_TX_CLK_EN()  __HAL_RCC_GPIOA_CLK_ENABLE()         /* TX脚时钟使能(不用发送则屏蔽) */
#define XT_SCOM3_UART_RX_CLK_EN()  __HAL_RCC_GPIOA_CLK_ENABLE()         /* RX脚时钟使能(不用接收则屏蔽) */
#define XT_SCOM3_UART_IRQn           USART1_IRQn                        /* 中断通道                     */
#define XT_SCOM3_UART_PRE_INT_PRIO   0                                  /* 抢占中断优先级               */
#define XT_SCOM3_UART_SUB_INT_PRIO   0                                  /* 响应中断优先级               */
#define XT_SCOM3_UART_TX_GPIO        GPIOA                              /* TX 所在端口                  */
#define XT_SCOM3_UART_TX_PIN         GPIO_PIN_9                         /* TX 所在管脚                  */
#define XT_SCOM3_UART_RX_GPIO        GPIOA                              /* RX 所在端口                  */
#define XT_SCOM3_UART_RX_PIN         GPIO_PIN_10                        /* RX 所在管脚                  */
#define XT_SCOM3_UART_RX_IN_MODE     GPIO_PULLUP                        /* RX 输入模式(上拉/悬空)       //
#define XT_SCOM3_UART_AFIO_REMAP() __HAL_AFIO_REMAP_USART1_ENABLE()     // 串口引脚重映射(不使用则屏蔽) */ //只有 UART1~3 才有映射
#define XT_SCOM3_DE_CLK_EN()       __HAL_RCC_GPIOA_CLK_ENABLE()         /* DE 端口时钟使能(RS485才使能) */
#define XT_SCOM3_DE_GPIO             GPIOA                              /* DE 所在端口    (RS485才使能) */
#define XT_SCOM3_DE_PIN              GPIO_PIN_3                         /* DE 所在管脚    (RS485才使能) */
#define XT_SCOM3_DE_EN()       XT_SCOM3_DE_GPIO->BSRR = XT_SCOM3_DE_PIN /* 使能DE控制端   (RS485才使能) */
#define XT_SCOM3_DE_DI()       XT_SCOM3_DE_GPIO->BRR  = XT_SCOM3_DE_PIN /* 禁能DE控制端   (RS485才使能) */
#define XT_SCOM3_UART_IRQHandler     USART1_IRQHandler                  /* 中断向量函数                 */

// 串行通信模块-通道4（☆通道总数由[XT_SCOM_SUM]决定☆）
#define XT_SCOM4_UART                UART5                              /* 使用的哪个串口               */
#define XT_SCOM4_UART_CLK_EN()     __HAL_RCC_UART5_CLK_ENABLE()         /* 串口时钟使能                 */
#define XT_SCOM4_UART_CLK_DI()     __HAL_RCC_UART5_CLK_DISABLE()        /* 串口时钟禁能                 */
#define XT_SCOM4_UART_TX_CLK_EN()  __HAL_RCC_GPIOC_CLK_ENABLE()         /* TX脚时钟使能(不用发送则屏蔽) */
#define XT_SCOM4_UART_RX_CLK_EN()  __HAL_RCC_GPIOD_CLK_ENABLE()         /* RX脚时钟使能(不用接收则屏蔽) */
#define XT_SCOM4_UART_IRQn           UART5_IRQn                         /* 中断通道                     */
#define XT_SCOM4_UART_PRE_INT_PRIO   0                                  /* 抢占中断优先级               */
#define XT_SCOM4_UART_SUB_INT_PRIO   0                                  /* 响应中断优先级               */
#define XT_SCOM4_UART_TX_GPIO        GPIOC                              /* TX 所在端口                  */
#define XT_SCOM4_UART_TX_PIN         GPIO_PIN_12                        /* TX 所在管脚                  */
#define XT_SCOM4_UART_RX_GPIO        GPIOD                              /* RX 所在端口                  */
#define XT_SCOM4_UART_RX_PIN         GPIO_PIN_2                         /* RX 所在管脚                  */
#define XT_SCOM4_UART_RX_IN_MODE     GPIO_PULLUP                        /* RX 输入模式(上拉/悬空)       //
#define XT_SCOM4_UART_AFIO_REMAP() __HAL_AFIO_REMAP_USART1_ENABLE()     // 串口引脚重映射(不使用则屏蔽) */ //只有 UART1~3 才有映射
#define XT_SCOM4_DE_CLK_EN()       __HAL_RCC_GPIOA_CLK_ENABLE()         /* DE 端口时钟使能(RS485才使能) */
#define XT_SCOM4_DE_GPIO             GPIOA                              /* DE 所在端口    (RS485才使能) */
#define XT_SCOM4_DE_PIN              GPIO_PIN_4                         /* DE 所在管脚    (RS485才使能) */
#define XT_SCOM4_DE_EN()       XT_SCOM4_DE_GPIO->BSRR = XT_SCOM4_DE_PIN /* 使能DE控制端   (RS485才使能) */
#define XT_SCOM4_DE_DI()       XT_SCOM4_DE_GPIO->BRR  = XT_SCOM4_DE_PIN /* 禁能DE控制端   (RS485才使能) */
#define XT_SCOM4_UART_IRQHandler     UART5_IRQHandler                   /* 中断向量函数                 */

#endif //////////////////////////////
#endif ///////////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ 操作函数 ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

//被驱动层直接调用的一个函数：打开硬件，是[驱动层]对接[硬件层]超始通道，并由它来提供其它对接口
//extern int xt_scom_hw_open(scom_obj_t *p_ob);

extern void xt_scom_hw_init(void);

#ifdef __cplusplus
	}
#endif

#endif  //#ifndef XT_SCOM_STM32F1XX_HAL_H__
