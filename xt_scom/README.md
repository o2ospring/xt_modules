<!-- +++
author = "XT"
comments = false
date  = "2022-11-27"
draft = false
share = false
image = ""
menu  = ""
slug  = ""
title = "����ͨ�ŷ���ģ��"
+++ -->

### һ������

<details close=""><summary>1�����ܼ��</summary>

����ͨ��ģ����ΪӦ�ó�����������ṩͳһ���淶�������ӿڡ���֧�� 1��n ������ͨѶ������������`XT_SCOM_SUM`���þ�������֧�־��󲿷ִ��ڴ���Э�飨���ݵ� 9 bit ��Ϊ��֡������ʼ��־���ַ�ʽĿǰ����֧�֣������ڽ���ʱ��Ϊ���ݽ��г����ķ�֡������Ӧ�ò㻹��Ҫ��һ���жϷ�֡�Ƿ���ȷ����  

</details>

<details close=""><summary>2��ģ��ܹ�</summary>

![ģ��ܹ�](./img/20221127_1_01.png)

</details>

<details close=""><summary>3������Ҫ��</summary>

|  ����  |  Ҫ��  |
| :----- | :----- |
| ������� | ʵʱ����ϵͳ �� ������� ���� |
| Ӳ������ | STM8 �� STM32F103 ���������� MCU |

</details>

<details close=""><summary>4��Э��֪ʶ</summary>

����ͨѶЭ���ʽһ���Ϊ���̶�������֡���ɱ䳤����֡�����ı�Э���������͡��������ǵ���ȱ�㼰���Ҫ�㣬�ɲο����ģ�[PC ����Ƕ��ʽ�豸ͨ��Э����Ƶ�ԭ��](https://blog.csdn.net/jiangjunjie_2005/article/details/50273105)��  

</details>

### ������ֲ

<details close=""><summary>1�����Դ�ļ�</summary>

��ģ��Դ�ļ����ļ�����·����ӵ����̣�ʾ����

![���Դ�ļ�������](./img/20221127_2_01.png)

</details>

<details close=""><summary>2���������Ŷ���</summary>

�ڹ���ͷ�ļ���board.h��ͳһ�������Ŷ��壬ʾ����

```c
// ����ͨ�ŷ���+++++++++++++++++++++++
// ����ͨ��ģ��-��ʱ��������+������ʱ����������
extern void xt_scomx_tim_irqhandler(void); //�������Ӳ��������ѡһ��� /* ������ʱ���ж���ÿ1ms����1�� */
// ����ͨ��ģ��-��ʱ��������+������ʱ����������
#define XT_SCOMx_TIM                 TIM1                               /* ��ʱ��                       */
#define XT_SCOMx_TIM_CLK_EN()      __HAL_RCC_TIM1_CLK_ENABLE()          /* ��ʱ��ʱ��ʹ��               */
#define XT_SCOMx_TIM_IRQn            TIM1_CC_IRQn                       /* ��ʱ���ж�ͨ��               */
#define XT_SCOMx_TIM_PRE_INT_PRIO    1                                  /* ��ʱ����ռ�ж����ȼ�         */
#define XT_SCOMx_TIM_SUB_INT_PRIO    1                                  /* ��ʱ����Ӧ�ж����ȼ�         */
#define XT_SCOMx_TIM_CCR1            0xFFFF      /*һ�㲻Ҫ��*/         /* ��ʱ��CCR1                   */
#define XT_SCOMx_TIM_CCR2            0xFFFF      /*һ�㲻Ҫ��*/         /* ��ʱ��CCR2                   */
#define XT_SCOMx_TIM_CCR3            0xFFFF      /*һ�㲻Ҫ��*/         /* ��ʱ��CCR3                   */
#define XT_SCOMx_TIM_CCR4            0xFFFF      /*һ�㲻Ҫ��*/         /* ��ʱ��CCR4                   */
#define XT_SCOMx_TIM_TICK_US         1           /*���Բ�Ҫ��*/         /* ��ʱ����������(��:1->1us)    */
#define XT_SCOMx_TIM_IRQHandler      TIM1_CC_IRQHandler                 /* �ж���������                 */

// ����ͨ��ģ��-ͨ��1����ͨ��������[XT_SCOM_SUM]�����
#define XT_SCOM1_UART                USART1                             /* ʹ�õ��ĸ�����               */
#define XT_SCOM1_UART_CLK_EN()     __HAL_RCC_USART1_CLK_ENABLE()        /* ����ʱ��ʹ��                 */
#define XT_SCOM1_UART_CLK_DI()     __HAL_RCC_USART1_CLK_DISABLE()       /* ����ʱ�ӽ���                 */
#define XT_SCOM1_UART_TX_CLK_EN()  __HAL_RCC_GPIOA_CLK_ENABLE()         /* TX��ʱ��ʹ��(���÷���������) */
#define XT_SCOM1_UART_RX_CLK_EN()  __HAL_RCC_GPIOA_CLK_ENABLE()         /* RX��ʱ��ʹ��(���ý���������) */
#define XT_SCOM1_UART_IRQn           USART1_IRQn                        /* �ж�ͨ��                     */
#define XT_SCOM1_UART_PRE_INT_PRIO   0                                  /* ��ռ�ж����ȼ�               */
#define XT_SCOM1_UART_SUB_INT_PRIO   0                                  /* ��Ӧ�ж����ȼ�               */
#define XT_SCOM1_UART_TX_GPIO        GPIOA                              /* TX ���ڶ˿�                  */
#define XT_SCOM1_UART_TX_PIN         GPIO_PIN_9                         /* TX ���ڹܽ�                  */
#define XT_SCOM1_UART_RX_GPIO        GPIOA                              /* RX ���ڶ˿�                  */
#define XT_SCOM1_UART_RX_PIN         GPIO_PIN_10                        /* RX ���ڹܽ�                  */
#define XT_SCOM1_UART_RX_IN_MODE     GPIO_PULLUP                        /* RX ����ģʽ(����/����)       //
#define XT_SCOM1_UART_AFIO_REMAP() __HAL_AFIO_REMAP_USART1_ENABLE()     // ����������ӳ��(��ʹ��������) // //ֻ�� UART1~3 ����ӳ��
#define XT_SCOM1_DE_CLK_EN()       __HAL_RCC_GPIOA_CLK_ENABLE()         // DE �˿�ʱ��ʹ��(RS485��ʹ��) //
#define XT_SCOM1_DE_GPIO             GPIOA                              // DE ���ڶ˿�    (RS485��ʹ��) //
#define XT_SCOM1_DE_PIN              GPIO_PIN_3                         // DE ���ڹܽ�    (RS485��ʹ��) //
#define XT_SCOM1_DE_EN()       XT_SCOM1_DE_GPIO->BSRR = XT_SCOM1_DE_PIN // ʹ��DE���ƶ�   (RS485��ʹ��) //
#define XT_SCOM1_DE_DI()       XT_SCOM1_DE_GPIO->BRR  = XT_SCOM1_DE_PIN // ����DE���ƶ�   (RS485��ʹ��) */
#define XT_SCOM1_UART_IRQHandler     USART1_IRQHandler                  /* �ж���������                 */
```  

</details>

<details close=""><summary>3�������ʼ����</summary>

�ڹ���Ӳ����ʼ�������м���ģ��Ӳ����ʼ����ʾ����

```c
int bsp_board_init(void)
{
	.
	.
	#if (XT_APP_SCOM_EN == XT_DEF_ENABLED)
	xt_scom_hw_init();
	#endif
	.
	.
}
```

����˵��������������Զ���ʼ�ܹ��������ɺ�`XT_HARD_INIT_2_TAB_EXPORT(func,name)`�Զ����ú������г�ʼ����

</details>

<details close=""><summary>4�����볬ʱ����</summary>

�ڹ���Ӳ����ʱ���м���ģ�鳬ʱ��������ʾ����

```c
void bsp_tim2_init(void)
{
	.
	.
	// [1ms]�ж���ѭ������
	// ֻ���[����+������ʱ��]Ӳ����������
	#if (XT_APP_SCOM_EN == XT_DEF_ENABLED)
	#if (XT_SCOM_HW_DRIVERS_EN == 2)
	xt_scomx_tim_irqhandler();
	#endif
	#endif
	.
	.
}
```

</details>

<details close=""><summary>5������ͷ�ļ�</summary>

��ʹ��ģ���Ӧ�ó����м���ͷ�ļ�������ʾ����  

```c
#if (XT_APP_SCOM_EN == XT_DEF_ENABLED)
#include "xt_scom.h"
#endif
```

</details>

<details close=""><summary>6������ģ�����</summary>

����ʵ�ʵ�ʹ�û�������ģ�������ʾ����

![����ģ�����](./img/20221127_2_05.png)  

����˵�������ڱ�׼ģ���ǲ������û��޸ĵģ�������Ӧ��ʱ������ͷ�ļ�ӳ�䣬��ӳ��ͷ�ļ��޸����ã�

</details>

<details close=""><summary>7��ʹ��ģ�鿪��</summary>

�ڹ���ͷ�ļ���application.h��ͳһʹ��ģ�鿪�أ�ʾ����

```c
#define XT_DEF_DISABLED                 0                               /* ����ģ��                     */
#define XT_DEF_ENABLED                  1                               /* ʹ��ģ��                     */

#define __XT_SCOM_REMAP_H
#ifndef XT_APP_SCOM_EN
#define XT_APP_SCOM_EN                  XT_DEF_ENABLED                  /* ����ͨ�ŷ���ģ��             */
#endif
```

</details>

### ����ʹ��

<details close=""><summary>1��Ӧ��ʾ��</summary>

��ʾ��ֻ������򵥷�ʽչʾģ��Ļ���ʹ�ã�����һ��Ӧ��ʵ����

```c
#include <stdint.h>
#include "xt_scom.h"

xt_scom_obj_t app_scom_obj;  //����ͨ�ŷ���Ӧ�ö���
uint8_t app_scom_rx_buf[30]; //����ͨ�ŷ��ͻ���
uint8_t app_scom_tx_buf[30]; //����ͨ�ŷ��ͻ���
uint8_t app_scom_flag = 0;   //״̬

void app_scom_open(void);
void app_scom_close(void);
void app_scom_open_cb(const xt_scom_obj_t *p_scom);
void app_scom_close_cb(const xt_scom_obj_t *p_scom);
void app_scom_send(void);
void app_scom_tx_complete_cb(const xt_scom_obj_t *p_scom);
int app_scom_rx_data1_cb(const xt_scom_obj_t *p_scom);
int app_scom_rx_data_cb(const xt_scom_obj_t *p_scom, uint16_t size);

/**
  * @brief  [��]-����ͨ�ŷ���
  * @param  void
  * @return void
  */
void app_scom_open(void)
{
	//{��ʼ��    ���� ����      У���   }
	//{0xFF,0x55,0x02,0x00,0x00,0x01,0x56}
	app_scom_obj.baud_rate        = 9600;                    //���������ԡ���ͨѶ�����ʣ�1200,2400,4800,9600,14400,19200,38400,57600,115200,128000,256000bps�ȣ�
	app_scom_obj.stop_bits        = 1;                       //���������ԡ���ֹͣλ��1:1λ,2:2λ��
	app_scom_obj.parity_bits      = 0;                       //���������ԡ���У��λ��0:��,1:��,2:ż��
	app_scom_obj.data_bits        = 8;                       //���������ԡ�������λ��8:8λ��
	app_scom_obj.scom_num         = 0;                       //���������ԡ�������ͨ��ͨ���ţ�������ͨ���ţ�����XT_SCOM_SUM��
	
	app_scom_obj.section_code[0]  = 0xFF;                    //��Э�����ԡ���������֡��������ʼ�루����:{0xFF,0x55}��        /�������������ս����루����:<CR><LF>��
	app_scom_obj.section_code[1]  = 0x55;                    //��Э�����ԡ���������֡��������ʼ�루����:{0xFF,0x55}��        /�������������ս����루����:<CR><LF>��
	app_scom_obj.section_mode     = 2;                       //��Э�����ԡ���������֡��������ʼ���С��0:��,1:1�ֽ�,2:2�ֽڣ�/�������������ս�����ģʽ��0:��,1:1�ֽڽ�����,2:2�ֽڽ�����,3:���ֽ���һ�����룩
	app_scom_obj.len_offset       = 2;                       //��Э�����ԡ���������֡�����ճ�����λ��ƫ�ƣ�������ʼ��������֡λ��ƫ�ƣ�����:5->��֡��6�ֽڣ�0xFF:��Ч��
	app_scom_obj.len_arithm       = 5;                       //��Э�����ԡ���������֡�����ճ�������֡�ܳ��㷨��5:5+������ֵ=��֡�ܳ�,0xFF:ֻ�ܳ�ʱ��֡��/��{������λ��ƫ��=0xFF}ʱ��ʾ���չ̶�����
	app_scom_obj.section_us       = 3000;                    //��Э�����ԡ�����֡����ʱ�䣨��λ:us��0:��Ч�������ڳ�ʱ��֡��ʱ���ý��ջ������ɵȣ�
	app_scom_obj.p_rx_buf         = app_scom_rx_buf;         //��Э�����ԡ������ջ��壨�������Ϊ0�
	app_scom_obj.p_tx_buf         = app_scom_tx_buf;         //��Э�����ԡ������ͻ��壨�������Ϊ0�
	app_scom_obj.rx_buf_size      = sizeof(app_scom_rx_buf); //��Э�����ԡ������ջ����С��0:��֧�ֽ��գ��Ｐ��Ӳ���㡿��ǿ������Ϊ0�
	app_scom_obj.tx_buf_size      = sizeof(app_scom_tx_buf); //��Э�����ԡ������ͻ����С��0:��֧�ַ��ͣ��Ｐ��Ӳ���㡿��ǿ������Ϊ0�
	
	app_scom_obj.p_scom_open_fn   = app_scom_open_cb;        //��Ӧ�ò���񡿣�����ͨ�ŷ���ɹ���Эͬ���������������������ϲ�Ӧ�ñ�־��
	app_scom_obj.p_scom_close_fn  = app_scom_close_cb;       //��Ӧ�ò���񡿣�����ͨ�ŷ���ɹ��ر�Эͬ���������������������ϲ�Ӧ�ñ�־��
	app_scom_obj.p_tx_complete_fn = app_scom_tx_complete_cb; //��Ӧ�ò���񡿣��ж�����ɷ��͵�֪ͨ����        ���ڲδ���:void��
	app_scom_obj.p_rx_data1_fn    = app_scom_rx_data1_cb;    //��Ӧ�ò���񡿣��ж��н��յ���һ�ֽ�����֪ͨ�������ڲδ���:pob->p_rx_buf��
	app_scom_obj.p_rx_data_fn     = app_scom_rx_data_cb;     //��Ӧ�ò���񡿣��ж��н��յ���һ�����ݵĴ��ݺ������ڲδ���:pob->p_rx_buf, size��
	
	if (xt_scom_open(&app_scom_obj, XT_SCOM_TYPE_DATA_FRAME/*����֡*/) < 0)
	{
		rt_kprintf("xt_scom_open return error!\r\n");
	}

	/*----------------------------------------------------------------------------------------------------
	//һ������ģ��ʹ��ATָ��İ�����
	//{"AT#MMbluetooch-name\r\n"}
	app_scom_obj.baud_rate        = 115200;                  //���������ԡ���ͨѶ�����ʣ�1200,2400,4800,9600,14400,19200,38400,57600,115200,128000,256000bps�ȣ�
	app_scom_obj.stop_bits        = 1;                       //���������ԡ���ֹͣλ��1:1λ,2:2λ��
	app_scom_obj.parity_bits      = 0;                       //���������ԡ���У��λ��0:��,1:��,2:ż��
	app_scom_obj.data_bits        = 8;                       //���������ԡ�������λ��8:8λ��
	app_scom_obj.scom_num         = 0;                       //���������ԡ�������ͨ��ͨ���ţ�������ͨ���ţ�����XT_SCOM_SUM��
	
	app_scom_obj.section_code[0]  = '\r';                    //��Э�����ԡ����������������ս����루����:<CR><LF>��
	app_scom_obj.section_code[1]  = '\n';                    //��Э�����ԡ����������������ս����루����:<CR><LF>��
	app_scom_obj.section_mode     = 2;                       //��Э�����ԡ����������������ս�����ģʽ��0:��,1:1�ֽڽ�����,2:2�ֽڽ�����,3:���ֽ���һ�����룩
	app_scom_obj.section_us       = 0;                       //��Э�����ԡ�����֡����ʱ�䣨��λ:us��0:��Ч�������ڳ�ʱ��֡��ʱ���ý��ջ������ɵȣ�
	app_scom_obj.p_rx_buf         = app_scom_rx_buf;         //��Э�����ԡ������ջ��壨�������Ϊ0�
	app_scom_obj.p_tx_buf         = app_scom_tx_buf;         //��Э�����ԡ������ͻ��壨�������Ϊ0�
	app_scom_obj.rx_buf_size      = sizeof(app_scom_rx_buf); //��Э�����ԡ������ջ����С��0:��֧�ֽ��գ��Ｐ��Ӳ���㡿��ǿ������Ϊ0�
	app_scom_obj.tx_buf_size      = sizeof(app_scom_tx_buf); //��Э�����ԡ������ͻ����С��0:��֧�ַ��ͣ��Ｐ��Ӳ���㡿��ǿ������Ϊ0�
	
	app_scom_obj.p_scom_open_fn   = app_scom_open_cb;        //��Ӧ�ò���񡿣�����ͨ�ŷ���ɹ���Эͬ���������������������ϲ�Ӧ�ñ�־��
	app_scom_obj.p_scom_close_fn  = app_scom_close_cb;       //��Ӧ�ò���񡿣�����ͨ�ŷ���ɹ��ر�Эͬ���������������������ϲ�Ӧ�ñ�־��
	app_scom_obj.p_tx_complete_fn = app_scom_tx_complete_cb; //��Ӧ�ò���񡿣��ж�����ɷ��͵�֪ͨ����        ���ڲδ���:void��
	app_scom_obj.p_rx_data1_fn    = app_scom_rx_data1_cb;    //��Ӧ�ò���񡿣��ж��н��յ���һ�ֽ�����֪ͨ�������ڲδ���:pob->p_rx_buf��
	app_scom_obj.p_rx_data_fn     = app_scom_rx_data_cb;     //��Ӧ�ò���񡿣��ж��н��յ���һ�����ݵĴ��ݺ������ڲδ���:pob->p_rx_buf, size��
	
	if (xt_scom_open(&app_scom_obj, XT_SCOM_TYPE_DATA_STREAM//�ַ���//) < 0)
	{
		rt_kprintf("xt_scom_open return error!\r\n");
	}
	------------------------------------------------------------------------------------------------------
	//�̶���������֡������
	//{��ʼ��    ����                У���   }
	//{0xAA,0x55,0x00,0x00,0x00,0x00,0x00,0xFF}
	app_scom_obj.baud_rate        = 9600;                    //���������ԡ���ͨѶ�����ʣ�1200,2400,4800,9600,14400,19200,38400,57600,115200,128000,256000bps�ȣ�
	app_scom_obj.stop_bits        = 1;                       //���������ԡ���ֹͣλ��1:1λ,2:2λ��
	app_scom_obj.parity_bits      = 0;                       //���������ԡ���У��λ��0:��,1:��,2:ż��
	app_scom_obj.data_bits        = 8;                       //���������ԡ�������λ��8:8λ��
	app_scom_obj.scom_num         = 0;                       //���������ԡ�������ͨ��ͨ���ţ�������ͨ���ţ�����XT_SCOM_SUM��
	
	app_scom_obj.section_code[0]  = 0xAA;                    //��Э�����ԡ���������֡��������ʼ�루����:{0xFF,0x55}��        /�������������ս����루����:<CR><LF>��
	app_scom_obj.section_code[1]  = 0x55;                    //��Э�����ԡ���������֡��������ʼ�루����:{0xFF,0x55}��        /�������������ս����루����:<CR><LF>��
	app_scom_obj.section_mode     = 2;                       //��Э�����ԡ���������֡��������ʼ���С��0:��,1:1�ֽ�,2:2�ֽڣ�/�������������ս�����ģʽ��0:��,1:1�ֽڽ�����,2:2�ֽڽ�����,3:���ֽ���һ�����룩
	app_scom_obj.len_offset       = 0xFF;                    //��Э�����ԡ���������֡�����ճ�����λ��ƫ�ƣ�������ʼ��������֡λ��ƫ�ƣ�����:5->��֡��6�ֽڣ�0xFF:��Ч��
	app_scom_obj.len_arithm       = 8;                       //��Э�����ԡ���������֡�����ճ�������֡�ܳ��㷨��5:5+������ֵ=��֡�ܳ�,0xFF:ֻ�ܳ�ʱ��֡��/��{������λ��ƫ��=0xFF}ʱ��ʾ���չ̶�����
	app_scom_obj.section_us       = 2000;                    //��Э�����ԡ�����֡����ʱ�䣨��λ:us��0:��Ч�������ڳ�ʱ��֡��ʱ���ý��ջ������ɵȣ�
	app_scom_obj.p_rx_buf         = app_scom_rx_buf;         //��Э�����ԡ������ջ��壨�������Ϊ0�
	app_scom_obj.p_tx_buf         = app_scom_tx_buf;         //��Э�����ԡ������ͻ��壨�������Ϊ0�
	app_scom_obj.rx_buf_size      = sizeof(app_scom_rx_buf); //��Э�����ԡ������ջ����С��0:��֧�ֽ��գ��Ｐ��Ӳ���㡿��ǿ������Ϊ0�
	app_scom_obj.tx_buf_size      = sizeof(app_scom_tx_buf); //��Э�����ԡ������ͻ����С��0:��֧�ַ��ͣ��Ｐ��Ӳ���㡿��ǿ������Ϊ0�
	
	app_scom_obj.p_scom_open_fn   = app_scom_open_cb;        //��Ӧ�ò���񡿣�����ͨ�ŷ���ɹ���Эͬ���������������������ϲ�Ӧ�ñ�־��
	app_scom_obj.p_scom_close_fn  = app_scom_close_cb;       //��Ӧ�ò���񡿣�����ͨ�ŷ���ɹ��ر�Эͬ���������������������ϲ�Ӧ�ñ�־��
	app_scom_obj.p_tx_complete_fn = app_scom_tx_complete_cb; //��Ӧ�ò���񡿣��ж�����ɷ��͵�֪ͨ����        ���ڲδ���:void��
	app_scom_obj.p_rx_data1_fn    = app_scom_rx_data1_cb;    //��Ӧ�ò���񡿣��ж��н��յ���һ�ֽ�����֪ͨ�������ڲδ���:pob->p_rx_buf��
	app_scom_obj.p_rx_data_fn     = app_scom_rx_data_cb;     //��Ӧ�ò���񡿣��ж��н��յ���һ�����ݵĴ��ݺ������ڲδ���:pob->p_rx_buf, size��
	
	if (xt_scom_open(&app_scom_obj, XT_SCOM_TYPE_DATA_FRAME//����֡//) < 0)
	{
		rt_kprintf("xt_scom_open return error!\r\n");
	}
	----------------------------------------------------------------------------------------------------*/
}

/**
  * @brief  [�ر�]-����ͨ�ŷ���
  * @param  void
  * @return void
  */
void app_scom_close(void)
{
	if (xt_scom_close(&app_scom_obj) < 0)
	{
		rt_kprintf("xt_scom_close return error!\r\n");
	}
}

/**
  * @brief  ����ͨ�ŷ���[��]�����ɹ�Эͬ�ص�
  * @param  *p_scom          ָ��SCOM����
  * @return void
  */
void app_scom_open_cb(const xt_scom_obj_t *p_scom)
{
	//ע�⣺�ϲ��Ѵ������񻥳�!!
	//XT_SCOM_TASK_LOCKED(); //>>>>>>>>>>>>>>>
	app_scom_flag = 1; //����ʵ��֪ͨӦ�ó���...
	//XT_SCOM_TASK_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  ����ͨ�ŷ���[�ر�]�����ɹ�Эͬ�ص�
  * @param  *p_scom          ָ��SCOM����
  * @return void
  */
void app_scom_close_cb(const xt_scom_obj_t *p_scom)
{
	//ע�⣺�ϲ��Ѵ������񻥳�!!
	//XT_SCOM_TASK_LOCKED(); //>>>>>>>>>>>>>>>
	app_scom_flag = 0; //����ʵ��֪ͨӦ�ó���...
	//XT_SCOM_TASK_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  [����]-����ʾ��
  * @param  void
  * @return void
  */
void app_scom_send(void)
{   //	                  {��ʼ��    ���� ����                     У��     }
    uint16_t send_dat[] = {0xFF,0x55,0x05,0x01,0x02,0x03,0x04,0x05,0x01,0x68};
    if (xt_scom_send(&app_scom_obj, send_dat, sizeof(send_dat)/*�ֽ�*/, 0/*us*/) < 0)
    {
        rt_kprintf("xt_scom_send return error!\r\n");
    }
}

/**
  * @brief  [֪ͨ]-��ɷ������ݣ����ж��лص���
  * @param  *p_scom          ָ��SCOM����
  * @return void
  */
void app_scom_tx_complete_cb(const xt_scom_obj_t *p_scom)
{
	//ע�⣺���ж���֪ͨ!!
	//����ʵ��֪ͨӦ�ó���...
}

/**
  * @brief  [֪ͨ]-�յ���1�ֽ����ݣ����ж��лص���
  * @param  *p_scom          ָ��SCOM����
  * @param  p_scom->p_rx_buf ���ݻ���
  * @return ������ֽ����ݣ�0:������ֽ�����,����:�������
  */
int app_scom_rx_data1_cb(const xt_scom_obj_t *p_scom)
{
	//���磺ATָ������ñ������ж���ʼ�ַ�
	//return (p_scom->p_rx_buf[0] != 'A') ? 0 : 0xFFFF;
	//���磺�����㲻�������ݣ�����ֱ�Ӵ���Ӧ�ò�
	//rt_mq_send(&mq, p_scom->p_rx_buf, 1);
	//return 0;
	return 0xFFFF;
}

/**
  * @brief  [����]-���յ�һ�����ݣ����ж��лص���
  * @param  *p_scom          ָ��SCOM����
  * @param  p_scom->p_rx_buf ���ݻ���
  * @param  size             �յ����ݴ�С
  * @return �Ѵ������ݣ���size:�Ѵ���ȫ������,������0xFFFF��ʾ��
  */
int app_scom_rx_data_cb(const xt_scom_obj_t *p_scom, uint16_t size)
{
	//����ʵ�ʰ�����ת����Ӧ�ó���!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//�������ѳ��������ݷ�֡,����Ӧ�ò㻹��Ҫ��һ���жϷ�֡�Ƿ���ȷ!
	//���MODBUS����Э��ͨ�����ֽڼ��ʱ������֡,Ҫ�ж��Ƿ�ɹ���֡��if (p_scom->rx_tim_flag >= 2)
	return 0xFFFF;
}
```

</details>