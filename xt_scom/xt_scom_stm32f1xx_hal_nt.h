/**
  * @file  xt_scom_stm32f1xx_hal_nt.h
  * @brief ����ͨ��ģ��Ӳ����--STM32F103��Ƭ��-HAL�⣨�õ�Ӳ����Դ������+������ʱ��
  * COPYRIGHT (C) 2022, XT ��������С��
  * Change Logs:
  * Date           Author       Notes
  * 2022-11-29     o2ospring    ԭʼ�汾
  *                             1.�������ڴ��������շ���
  *                             2.ʹ�ù�����ʱ���������ֽ����ݼ��������⣬�ֻ��65ms��
  *                               ʱ�侫��Ҫ��1ms���ŵ㲻��Ҫ��ռһ��Ӳ����ʱ����ֻ��1ms
  *                               ���ü����������ɡ�ȱ��Ϊ���Ȳ��ߣ���ҪƵ�����ü���������
  *                               ����Ҫ���ϲ�Ӧ���������ʱ��Ϊ������ڵ���2ms��
  */
#ifndef XT_SCOM_STM32F1XX_HAL_NT_H__
#define XT_SCOM_STM32F1XX_HAL_NT_H__

#include <stdint.h> //////////////////////// <- ʹ�õ����ݶ��壬��: int8_t, uint32_t ��
#ifdef XT_SCOM_STM32F1XX_HAL_NT_C__
#include <string.h> //////////////////////// <- ʹ�õ��ַ�������: strcpy(), memcpy() ��
#include "application.h" /////////////////// <- �������������Դ����ͷ�ļ�!!!!!!!!!!!!!!!
#include "board.h"   /////////////////////// <- ��������Ӳ����Դ����ͷ�ļ�!!!!!!!!!!!!!!!
#include "stm32f1xx.h" ///////////////////// <- ����STM32F103��Ƭ��HAL��
#include "xt_scom.h" /////////////////////// <- ��������ͨ�ŷ���ģ��
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ ���Ŷ��� ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

#ifdef XT_SCOM_STM32F1XX_HAL_NT_C__ //////// <- ֻ�� [xt_scom_stm32f1xx_hal_nt.c] Դ�ļ��Ż�������Ŷ���!!
#ifndef XT_SCOM1_UART  ///////////////////// <- �������ģ��[û��]����������ţ����ļ����������ź궨��!!

// ����ͨ��ģ��-��ʱ��
extern void xt_scomx_tim_irqhandler(void);                              /* ������ʱ���ж���ÿ1ms����1�� */

// ����ͨ��ģ��-ͨ��1����ͨ��������[XT_SCOM_SUM]�����
#define XT_SCOM1_UART                USART3                             /* ʹ�õ��ĸ�����               */
#define XT_SCOM1_UART_CLK_EN()     __HAL_RCC_USART3_CLK_ENABLE()        /* ����ʱ��ʹ��                 */
#define XT_SCOM1_UART_CLK_DI()     __HAL_RCC_USART3_CLK_DISABLE()       /* ����ʱ�ӽ���                 */
#define XT_SCOM1_UART_TX_CLK_EN()  __HAL_RCC_GPIOB_CLK_ENABLE()         /* TX��ʱ��ʹ��(���÷���������) */
#define XT_SCOM1_UART_RX_CLK_EN()  __HAL_RCC_GPIOB_CLK_ENABLE()         /* RX��ʱ��ʹ��(���ý���������) */
#define XT_SCOM1_UART_IRQn           USART3_IRQn                        /* �ж�ͨ��                     */
#define XT_SCOM1_UART_PRE_INT_PRIO   0                                  /* ��ռ�ж����ȼ�               */
#define XT_SCOM1_UART_SUB_INT_PRIO   0                                  /* ��Ӧ�ж����ȼ�               */
#define XT_SCOM1_UART_TX_GPIO        GPIOB                              /* TX ���ڶ˿�                  */
#define XT_SCOM1_UART_TX_PIN         GPIO_PIN_10                        /* TX ���ڹܽ�                  */
#define XT_SCOM1_UART_RX_GPIO        GPIOB                              /* RX ���ڶ˿�                  */
#define XT_SCOM1_UART_RX_PIN         GPIO_PIN_11                        /* RX ���ڹܽ�                  */
#define XT_SCOM1_UART_RX_IN_MODE     GPIO_PULLUP                        /* RX ����ģʽ(����/����)       //
#define XT_SCOM1_UART_AFIO_REMAP() __HAL_AFIO_REMAP_USART3_ENABLE()     // ����������ӳ��(��ʹ��������) // //ֻ�� UART1~3 ����ӳ��
#define XT_SCOM1_DE_CLK_EN()       __HAL_RCC_GPIOA_CLK_ENABLE()         // DE �˿�ʱ��ʹ��(RS485��ʹ��) //
#define XT_SCOM1_DE_GPIO             GPIOA                              // DE ���ڶ˿�    (RS485��ʹ��) //
#define XT_SCOM1_DE_PIN              GPIO_PIN_1                         // DE ���ڹܽ�    (RS485��ʹ��) //
#define XT_SCOM1_DE_EN()       XT_SCOM1_DE_GPIO->BSRR = XT_SCOM1_DE_PIN // ʹ��DE���ƶ�   (RS485��ʹ��) //
#define XT_SCOM1_DE_DI()       XT_SCOM1_DE_GPIO->BRR  = XT_SCOM1_DE_PIN // ����DE���ƶ�   (RS485��ʹ��) */
#define XT_SCOM1_UART_IRQHandler     USART3_IRQHandler                  /* �ж���������                 */

// ����ͨ��ģ��-ͨ��2����ͨ��������[XT_SCOM_SUM]�����
#define XT_SCOM2_UART                UART4                              /* ʹ�õ��ĸ�����               */
#define XT_SCOM2_UART_CLK_EN()     __HAL_RCC_UART4_CLK_ENABLE()         /* ����ʱ��ʹ��                 */
#define XT_SCOM2_UART_CLK_DI()     __HAL_RCC_UART4_CLK_DISABLE()        /* ����ʱ�ӽ���                 */
#define XT_SCOM2_UART_TX_CLK_EN()  __HAL_RCC_GPIOC_CLK_ENABLE()         /* TX��ʱ��ʹ��(���÷���������) */
#define XT_SCOM2_UART_RX_CLK_EN()  __HAL_RCC_GPIOC_CLK_ENABLE()         /* RX��ʱ��ʹ��(���ý���������) */
#define XT_SCOM2_UART_IRQn           UART4_IRQn                         /* �ж�ͨ��                     */
#define XT_SCOM2_UART_PRE_INT_PRIO   0                                  /* ��ռ�ж����ȼ�               */
#define XT_SCOM2_UART_SUB_INT_PRIO   0                                  /* ��Ӧ�ж����ȼ�               */
#define XT_SCOM2_UART_TX_GPIO        GPIOC                              /* TX ���ڶ˿�                  */
#define XT_SCOM2_UART_TX_PIN         GPIO_PIN_10                        /* TX ���ڹܽ�                  */
#define XT_SCOM2_UART_RX_GPIO        GPIOC                              /* RX ���ڶ˿�                  */
#define XT_SCOM2_UART_RX_PIN         GPIO_PIN_11                        /* RX ���ڹܽ�                  */
#define XT_SCOM2_UART_RX_IN_MODE     GPIO_PULLUP                        /* RX ����ģʽ(����/����)       //
#define XT_SCOM2_UART_AFIO_REMAP() __HAL_AFIO_REMAP_USART1_ENABLE()     // ����������ӳ��(��ʹ��������) // //ֻ�� UART1~3 ����ӳ��
#define XT_SCOM2_DE_CLK_EN()       __HAL_RCC_GPIOA_CLK_ENABLE()         // DE �˿�ʱ��ʹ��(RS485��ʹ��) //
#define XT_SCOM2_DE_GPIO             GPIOA                              // DE ���ڶ˿�    (RS485��ʹ��) //
#define XT_SCOM2_DE_PIN              GPIO_PIN_2                         // DE ���ڹܽ�    (RS485��ʹ��) //
#define XT_SCOM2_DE_EN()       XT_SCOM2_DE_GPIO->BSRR = XT_SCOM2_DE_PIN // ʹ��DE���ƶ�   (RS485��ʹ��) //
#define XT_SCOM2_DE_DI()       XT_SCOM2_DE_GPIO->BRR  = XT_SCOM2_DE_PIN // ����DE���ƶ�   (RS485��ʹ��) */
#define XT_SCOM2_UART_IRQHandler     UART4_IRQHandler                   /* �ж���������                 */

// ����ͨ��ģ��-ͨ��3����ͨ��������[XT_SCOM_SUM]�����
#define XT_SCOM3_UART                USART1                             /* ʹ�õ��ĸ�����               */
#define XT_SCOM3_UART_CLK_EN()     __HAL_RCC_USART1_CLK_ENABLE()        /* ����ʱ��ʹ��                 */
#define XT_SCOM3_UART_CLK_DI()     __HAL_RCC_USART1_CLK_DISABLE()       /* ����ʱ�ӽ���                 */
#define XT_SCOM3_UART_TX_CLK_EN()  __HAL_RCC_GPIOA_CLK_ENABLE()         /* TX��ʱ��ʹ��(���÷���������) */
#define XT_SCOM3_UART_RX_CLK_EN()  __HAL_RCC_GPIOA_CLK_ENABLE()         /* RX��ʱ��ʹ��(���ý���������) */
#define XT_SCOM3_UART_IRQn           USART1_IRQn                        /* �ж�ͨ��                     */
#define XT_SCOM3_UART_PRE_INT_PRIO   0                                  /* ��ռ�ж����ȼ�               */
#define XT_SCOM3_UART_SUB_INT_PRIO   0                                  /* ��Ӧ�ж����ȼ�               */
#define XT_SCOM3_UART_TX_GPIO        GPIOA                              /* TX ���ڶ˿�                  */
#define XT_SCOM3_UART_TX_PIN         GPIO_PIN_9                         /* TX ���ڹܽ�                  */
#define XT_SCOM3_UART_RX_GPIO        GPIOA                              /* RX ���ڶ˿�                  */
#define XT_SCOM3_UART_RX_PIN         GPIO_PIN_10                        /* RX ���ڹܽ�                  */
#define XT_SCOM3_UART_RX_IN_MODE     GPIO_PULLUP                        /* RX ����ģʽ(����/����)       //
#define XT_SCOM3_UART_AFIO_REMAP() __HAL_AFIO_REMAP_USART1_ENABLE()     // ����������ӳ��(��ʹ��������) */ //ֻ�� UART1~3 ����ӳ��
#define XT_SCOM3_DE_CLK_EN()       __HAL_RCC_GPIOA_CLK_ENABLE()         /* DE �˿�ʱ��ʹ��(RS485��ʹ��) */
#define XT_SCOM3_DE_GPIO             GPIOA                              /* DE ���ڶ˿�    (RS485��ʹ��) */
#define XT_SCOM3_DE_PIN              GPIO_PIN_3                         /* DE ���ڹܽ�    (RS485��ʹ��) */
#define XT_SCOM3_DE_EN()       XT_SCOM3_DE_GPIO->BSRR = XT_SCOM3_DE_PIN /* ʹ��DE���ƶ�   (RS485��ʹ��) */
#define XT_SCOM3_DE_DI()       XT_SCOM3_DE_GPIO->BRR  = XT_SCOM3_DE_PIN /* ����DE���ƶ�   (RS485��ʹ��) */
#define XT_SCOM3_UART_IRQHandler     USART1_IRQHandler                  /* �ж���������                 */

// ����ͨ��ģ��-ͨ��4����ͨ��������[XT_SCOM_SUM]�����
#define XT_SCOM4_UART                UART5                              /* ʹ�õ��ĸ�����               */
#define XT_SCOM4_UART_CLK_EN()     __HAL_RCC_UART5_CLK_ENABLE()         /* ����ʱ��ʹ��                 */
#define XT_SCOM4_UART_CLK_DI()     __HAL_RCC_UART5_CLK_DISABLE()        /* ����ʱ�ӽ���                 */
#define XT_SCOM4_UART_TX_CLK_EN()  __HAL_RCC_GPIOC_CLK_ENABLE()         /* TX��ʱ��ʹ��(���÷���������) */
#define XT_SCOM4_UART_RX_CLK_EN()  __HAL_RCC_GPIOD_CLK_ENABLE()         /* RX��ʱ��ʹ��(���ý���������) */
#define XT_SCOM4_UART_IRQn           UART5_IRQn                         /* �ж�ͨ��                     */
#define XT_SCOM4_UART_PRE_INT_PRIO   0                                  /* ��ռ�ж����ȼ�               */
#define XT_SCOM4_UART_SUB_INT_PRIO   0                                  /* ��Ӧ�ж����ȼ�               */
#define XT_SCOM4_UART_TX_GPIO        GPIOC                              /* TX ���ڶ˿�                  */
#define XT_SCOM4_UART_TX_PIN         GPIO_PIN_12                        /* TX ���ڹܽ�                  */
#define XT_SCOM4_UART_RX_GPIO        GPIOD                              /* RX ���ڶ˿�                  */
#define XT_SCOM4_UART_RX_PIN         GPIO_PIN_2                         /* RX ���ڹܽ�                  */
#define XT_SCOM4_UART_RX_IN_MODE     GPIO_PULLUP                        /* RX ����ģʽ(����/����)       //
#define XT_SCOM4_UART_AFIO_REMAP() __HAL_AFIO_REMAP_USART1_ENABLE()     // ����������ӳ��(��ʹ��������) */ //ֻ�� UART1~3 ����ӳ��
#define XT_SCOM4_DE_CLK_EN()       __HAL_RCC_GPIOA_CLK_ENABLE()         /* DE �˿�ʱ��ʹ��(RS485��ʹ��) */
#define XT_SCOM4_DE_GPIO             GPIOA                              /* DE ���ڶ˿�    (RS485��ʹ��) */
#define XT_SCOM4_DE_PIN              GPIO_PIN_4                         /* DE ���ڹܽ�    (RS485��ʹ��) */
#define XT_SCOM4_DE_EN()       XT_SCOM4_DE_GPIO->BSRR = XT_SCOM4_DE_PIN /* ʹ��DE���ƶ�   (RS485��ʹ��) */
#define XT_SCOM4_DE_DI()       XT_SCOM4_DE_GPIO->BRR  = XT_SCOM4_DE_PIN /* ����DE���ƶ�   (RS485��ʹ��) */
#define XT_SCOM4_UART_IRQHandler     UART5_IRQHandler                   /* �ж���������                 */

#endif //////////////////////////////
#endif ///////////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ �������� ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

//��������ֱ�ӵ��õ�һ����������Ӳ������[������]�Խ�[Ӳ����]��ʼͨ�������������ṩ�����Խӿ�
//extern int xt_scom_hw_open(scom_obj_t *p_ob);

extern void xt_scom_hw_init(void);
extern void xt_scomx_tim_irqhandler(void);

#ifdef __cplusplus
	}
#endif

#endif  //#ifndef XT_SCOM_STM32F1XX_HAL_NT_H__
