/**
  * @file  xt_ir_send_pwm_dma_stm32f1xx_hal.h
  * @brief ���ⷢ�ͷ���ģ��Ӳ����--STM32F103��Ƭ��-HAL�⣨�õ�Ӳ����Դ��TIM_PWM+DMA��
  * COPYRIGHT (C) 2022, XT ��������С��
  * Change Logs:
  * Date           Author       Notes
  * 2022-11-16     o2ospring    ԭʼ�汾
  *                             1.��ԭ����ͨ��DMA��RAM�����PWMռ�ձ����������TIM_PWMͨ����
  *                             2.���ٵ�RAMԽ�������DMA�жϾ�Խ�٣������Կռ任ȡִ��Ч�ʣ�
  *                             3.�ϲ�����PWMƵ��Խ����DMA�ж�Ƶ��ҲԽ�ߣ�Ҫ��������RAM��С
  */
#ifndef XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_H__
#define XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_H__

#include <stdint.h> //////////////////////// <- ʹ�õ����ݶ��壬��: int8_t, uint32_t ��
#ifdef XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_C__
#include <string.h> //////////////////////// <- ʹ�õ��ַ�������: strcpy(), memcpy() ��
#include "application.h" /////////////////// <- �������������Դ����ͷ�ļ�!!!!!!!!!!!!!!!
#include "board.h"   /////////////////////// <- ��������Ӳ����Դ����ͷ�ļ�!!!!!!!!!!!!!!!
#include "stm32f1xx.h" ///////////////////// <- ����STM32F103��Ƭ��HAL��
#include "xt_ir_send.h" //////////////////// <- �������ⷢ�ͷ���ģ��
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ ���Ŷ��� ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

#ifdef XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_C__ // <- ֻ��[xt_ir_send_pwm_dma_stm32f1xx_hal.c]�Ż�������Ŷ���!!!!
#ifndef XT_IRSEND_TIM  /////////////////////// <- �������ģ��[û��]����������ţ����ļ����������ź궨��!!!!

// ����PWM��Ƶ��
#define XT_IRSEND_TIM_DIV            1                                      /* ��ʱ����ʱ�ӷ�Ƶ         */
#define XT_IRSEND_TIM_PWM_HZ(hz)   ((72000000/(XT_IRSEND_TIM_DIV+1)/(hz))-1)/* PWM 100%��ֵ(��0xFFFF)   */

// ����PWM�Ķ�ʱ��
#define XT_IRSEND_TIM                TIM3                               /* ��ʱ��                       */
#define XT_IRSEND_TIM_CLK_DI()     __HAL_RCC_TIM3_CLK_DISABLE()         /* ��ʱ��ʱ�ӽ���               */
#define XT_IRSEND_TIM_CLK_EN()     __HAL_RCC_TIM3_CLK_ENABLE()          /* ��ʱ��ʱ��ʹ��               //
#define XT_IRSEND_TIM_AFIO_REMAP() __HAL_AFIO_REMAP_TIM3_PARTIAL()      // ��ʱ��������ӳ��(����������) */ //ֻ�� TIM1~5 ����ӳ�� (��Ϊ:��ӳ��[TIM4~5]�����ֺ���ȫ��ӳ��TIM1~3)

// ����PWM��ͨ����ֻʹ��1·��
#define XT_IRSEND_PWM_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()         /* PWM �ܽ�ʱ��ʹ��             */
#define XT_IRSEND_PWM_GPIO           GPIOB                              /* PWM ���ڶ˿�                 */
#define XT_IRSEND_PWM_PIN            GPIO_PIN_1                         /* PWM ���ڹܽ�                 */
#define XT_IRSEND_PWM_OCMODE         TIM_OCMODE_PWM1 /*���ܸ�*/         /* PWM �������ģʽ(2�򷴼���)  */ //ʹ����:��0%,��ֹͣ
#define XT_IRSEND_PWM_OCPOLARITY     TIM_OCPOLARITY_HIGH                /* PWM ��Чռ�ձ�����ĵ�ƽ     // //����ͨ��
#define XT_IRSEND_PWM_OCIDLESTATE    TIM_OCIDLESTATE_RESET              // PWM ����ʱ��ƽ(����������)   */ //ֻ���TIM1,������ʱ��Ĭ��Ϊ�͵�ƽ
#define XT_IRSEND_PWM_CCR            CCR4                               /* PWM ռ�ձȼĴ���(TIM3->CCR4) */
#define XT_IRSEND_PWM_TIM_CHANNEL    TIM_CHANNEL_4                      /* PWM ���ڶ�ʱ��ͨ��           */
#define XT_IRSEND_TIM_DMA_ID_CC      TIM_DMA_ID_CC4                     /* ��ʱ��DMA��������ͨ��(��:CC4)*/
#define XT_IRSEND_TIM_DMA_CC         TIM_DMA_CC4                        /* ��ʱ��DMA�����¼�ͨ��(��:CC4)*/

// ����PWM��DMA
#define XT_IRSEND_DMA_CLK_ENABLE() __HAL_RCC_DMA1_CLK_ENABLE()          /* DMA ʱ��ʹ��                 */
#define XT_IRSEND_DMA_CHANNEL        DMA1_Channel3                      /* DMA ͨ��                     */
#define XT_IRSEND_DMA_IRQn           DMA1_Channel3_IRQn                 /* DMA �ж�ͨ��                 */
#define XT_IRSEND_DMA_PRE_INT_PRIO   0                                  /* DMA ͨ����ռ�ж����ȼ�       */
#define XT_IRSEND_DMA_SUB_INT_PRIO   0                                  /* DMA ͨ����Ӧ�ж����ȼ�       */
#define XT_IRSEND_DMA_PRIORITY       DMA_PRIORITY_MEDIUM                /* DMA ͨ�����ȼ����еȣ�       */
#define XT_IRSEND_DMA_IRQHandler     DMA1_Channel3_IRQHandler           /* DMA �ж���������             */
#define XT_IRSEND_RAM_SUM            65 /*�Կռ任ȡִ��Ч��*/          /* DMA ת������Դ��С(���ݸ���) */

/* ���䣺STM32F103 DMA ��Դ����
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
/*++++++++++++++++++++++++++++++++++++++++++++++ �������� ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

//��������ֱ�ӵ��õ�һ����������Ӳ������[������]�Խ�[Ӳ����]��ʼͨ�������������ṩ�����Խӿ�
//extern int xt_irsend_hw_open(xt_irsend_obj_t *p_ob);

extern void xt_irsend_hw_init(void);

#ifdef __cplusplus
	}
#endif

#endif  //#ifndef XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_H__
