/**
  * @file  xt_scom_stm32f1xx_hal.c
  * @brief 串行通信模块硬件层--STM32F103单片机-HAL库（用到硬件资源：串口+独立定时器）
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-11-27     o2ospring    原始版本
  *                             1.串口用于串行数据收发；
  *                             2.定时器用于两字节数据间隔过长检测，最长可达65ms，
  *                               时间精度可达1us，缺点是需要独占一个硬件定时器。
  */
#define   XT_SCOM_STM32F1XX_HAL_C__
#include "xt_scom_stm32f1xx_hal.h"
#if (XT_SCOM_HW_DRIVERS_EN == 1)
#if (defined XT_APP_SCOM_EN) && (XT_APP_SCOM_EN == XT_DEF_ENABLED)
#ifdef    XT_SCOM_STM32F1XX_HAL_X__
#undef    XT_SCOM_STM32F1XX_HAL_H__
#include "xt_scom_stm32f1xx_hal.h"
#endif

#ifndef XT_HARD_INIT_2_TAB_EXPORT
#define XT_HARD_INIT_2_TAB_EXPORT(func,name)
#endif

#if (XT_SCOM_SUM > 4)
#error "本硬件最多支持4路串行通信通道"
#endif

xt_scom_obj_t *xt_p_scom_uart[XT_SCOM_SUM] = {0};

void xt_scom_hw_init(void);
int xt_scom_hw_open(xt_scom_obj_t *p_ob);
void xt_scom_hw_close(xt_scom_obj_t *p_ob);
void xt_scom_putc_null_ch1(void);
void xt_scom_putc_null_ch2(void);
void xt_scom_putc_null_ch3(void);
void xt_scom_putc_null_ch4(void);
void xt_scom_putc_ch1(uint16_t d, uint8_t f);
void xt_scom_putc_ch2(uint16_t d, uint8_t f);
void xt_scom_putc_ch3(uint16_t d, uint8_t f);
void xt_scom_putc_ch4(uint16_t d, uint8_t f);
void xt_scom_putc_end_ch1(void);
void xt_scom_putc_end_ch2(void);
void xt_scom_putc_end_ch3(void);
void xt_scom_putc_end_ch4(void);
int xt_scom_getc_ch1(void);
int xt_scom_getc_ch2(void);
int xt_scom_getc_ch3(void);
int xt_scom_getc_ch4(void);
uint8_t xt_scom_set_rx_overtime_ch1(uint16_t us);
uint8_t xt_scom_set_rx_overtime_ch2(uint16_t us);
uint8_t xt_scom_set_rx_overtime_ch3(uint16_t us);
uint8_t xt_scom_set_rx_overtime_ch4(uint16_t us);
void XT_SCOM1_UART_IRQHandler(void);
void XT_SCOM2_UART_IRQHandler(void);
void XT_SCOM3_UART_IRQHandler(void);
void XT_SCOM4_UART_IRQHandler(void);
void XT_SCOMx_TIM_IRQHandler(void);

/**
  * @brief  初始化硬件资源-上电
  * @param  void
  * @return void
  */
void xt_scom_hw_init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	//定时器（对定时器整体初始化。策略:定时器一直工作不关闭）■■■■■■■■■■■■■■■■■■■■■■■
	/* ---------------------------------------------------------------
	TIM1 Configuration: Output Compare Timing Mode:
	TIM1CLK = 72 MHz, Prescaler = 720, TIM1 counter clock = 0.1 MHz
	CC1 update rate = TIM1 counter clock / _TIM1_CCR1_ = Hz
	CC2 update rate = TIM1 counter clock / _TIM1_CCR2_ = Hz
	CC3 update rate = TIM1 counter clock / _TIM1_CCR3_ = Hz
	CC4 update rate = TIM1 counter clock / _TIM1_CCR4_ = Hz
	--------------------------------------------------------------- */
	// 系统主时钟在: board.c/SystemClock_Config()/HAL_RCC_OscConfig() 设置
	// APBx预分频在: board.c/SystemClock_Config()/HAL_RCC_ClockConfig() 设置
	//----------------------------------------------------------------
	// TIM2,3,4,5,6,7
	// 如果当APB1预分频=1时,则定时器x1倍频输出;
	// 否则是其它预分频值时,则定时器x2倍频输出.
	// 例: APB1预分频=1: 72(APB1频率) * 1(定时器倍频) = 72MHz(定时器时钟频率) ->非法,因APB1最高限制为36MHz
	//     APB1预分频=2: 36(APB1频率) * 2(定时器倍频) = 72MHz(定时器时钟频率) √
	// TIM1,8
	// 如果当APB2预分频=1时,则定时器x1倍频输出;
	// 否则是其它预分频值时,则定时器x2倍频输出.
	// 例: APB2预分频=1: 72(APB2频率) * 1(定时器倍频) = 72MHz(定时器时钟频率) √
	//     APB2预分频=2: 36(APB2频率) * 2(定时器倍频) = 36MHz(定时器时钟频率)

	TIM_HandleTypeDef htimx;                //定时器句柄（HAL库标准用法则要求为全局变量）
	TIM_OC_InitTypeDef TIM_OCInitStructure; //定时器通道句柄

	//__HAL_RCC_AFIO_CLK_ENABLE();

	// > 下面三行代码的HAL库内部实现写法为：
	// > `HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim)`
	// >> `if (htim->State == HAL_TIM_STATE_RESET)`
	// >> `HAL_TIM_Base_MspInit(htim)`      //回调用户提供的回调函数(1.使能时钟, 2.设置引脚和中断优先级)
	XT_SCOMx_TIM_CLK_EN();
	HAL_NVIC_SetPriority(XT_SCOMx_TIM_IRQn, XT_SCOMx_TIM_PRE_INT_PRIO, XT_SCOMx_TIM_SUB_INT_PRIO);/* 设置中断优先级 */
	HAL_NVIC_EnableIRQ(XT_SCOMx_TIM_IRQn);                                          /* 使能中断通道     */

	htimx.State = HAL_TIM_STATE_RESET;                                              /* 初始化           */
	htimx.Instance = XT_SCOMx_TIM;                                                  /* 定时器           */
	htimx.Init.Prescaler = (72000000UL / (1000000/XT_SCOMx_TIM_TICK_US)) - 1;       /* 分频系数         */
	htimx.Init.CounterMode = TIM_COUNTERMODE_UP;                                    /* 向上计数器       */
	htimx.Init.Period = 0xFFFF;                                                     /* 计数器翻转最大值 */
	htimx.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;                              /* 时钟分频因子     */
	htimx.Init.RepetitionCounter = 0;  /*0->1次翻转进入溢出中断*/                   /* 多少次翻转才中断 */
	htimx.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;                   /* 自动重装载使能   */
	HAL_TIM_Base_Init(&htimx);                                                      /* 定时器初始化     */
	//HAL_TIM_Base_Start_IT(&htimx);                                                /* 溢出中断使能     */

	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_LOW;    
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;  
	TIM_OCInitStructure.OCNIdleState= TIM_OCNIDLESTATE_RESET;
	TIM_OCInitStructure.OCFastMode  = TIM_OCFAST_DISABLE;
	TIM_OCInitStructure.OCMode      = TIM_OCMODE_TIMING;                            /* 只定时,引脚不输出*/
	TIM_OCInitStructure.OCPolarity  = TIM_OCPOLARITY_HIGH;                          /* 高电平有效       */
	//CCR1
	TIM_OCInitStructure.Pulse = XT_SCOMx_TIM_CCR1;                                  /* 比较通道的计数值 */
	HAL_TIM_OC_ConfigChannel(&htimx, &TIM_OCInitStructure, TIM_CHANNEL_1);          /* 比较通道的初始化 */
	HAL_TIM_OC_Start_IT(&htimx, TIM_CHANNEL_1);                                     /* 比较通道中断使能 */
	HAL_TIM_OC_Start(&htimx, TIM_CHANNEL_1);                                        /* 比较通道工作使能 */
	//CCR2
	TIM_OCInitStructure.Pulse = XT_SCOMx_TIM_CCR2;                                  /* 比较通道的计数值 */
	HAL_TIM_OC_ConfigChannel(&htimx, &TIM_OCInitStructure, TIM_CHANNEL_2);          /* 比较通道的初始化 */
	HAL_TIM_OC_Start_IT(&htimx, TIM_CHANNEL_2);                                     /* 比较通道中断使能 */
	HAL_TIM_OC_Start(&htimx, TIM_CHANNEL_2);                                        /* 比较通道工作使能 */
	//CCR3
	TIM_OCInitStructure.Pulse = XT_SCOMx_TIM_CCR3;                                  /* 比较通道的计数值 */
	HAL_TIM_OC_ConfigChannel(&htimx, &TIM_OCInitStructure, TIM_CHANNEL_3);          /* 比较通道的初始化 */
	HAL_TIM_OC_Start_IT(&htimx, TIM_CHANNEL_3);                                     /* 比较通道中断使能 */
	HAL_TIM_OC_Start(&htimx, TIM_CHANNEL_3);                                        /* 比较通道工作使能 */
	//CCR4
	TIM_OCInitStructure.Pulse = XT_SCOMx_TIM_CCR4;                                  /* 比较通道的计数值 */
	HAL_TIM_OC_ConfigChannel(&htimx, &TIM_OCInitStructure, TIM_CHANNEL_4);          /* 比较通道的初始化 */
	HAL_TIM_OC_Start_IT(&htimx, TIM_CHANNEL_4);                                     /* 比较通道中断使能 */
	HAL_TIM_OC_Start(&htimx, TIM_CHANNEL_4);                                        /* 比较通道工作使能 */

	//串口（对串口引脚进行初始化。策略:工作时才初始化串口控制器）■■■■■■■■■■■■■■■■■■■■■
	//__HAL_RCC_AFIO_CLK_ENABLE();
	
	#if (XT_SCOM_SUM >= 1)
		#ifdef XT_SCOM1_UART_TX_CLK_EN
	XT_SCOM1_UART_TX_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM1_UART_TX_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_AF_PP;      //复用推挽输出
	GPIO_Initure.Pull  = GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM1_UART_TX_GPIO, &GPIO_Initure);
		#endif
		#ifdef XT_SCOM1_UART_RX_CLK_EN
	XT_SCOM1_UART_RX_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM1_UART_RX_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_INPUT;      //输入模式
	GPIO_Initure.Pull  = XT_SCOM1_UART_RX_IN_MODE;//上拉/悬空
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM1_UART_RX_GPIO, &GPIO_Initure);
		#endif
		
		#ifdef XT_SCOM1_UART_AFIO_REMAP
	XT_SCOM1_UART_AFIO_REMAP();
		#endif
		
		#ifdef XT_SCOM1_DE_CLK_EN
	XT_SCOM1_DE_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM1_DE_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull  = GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM1_DE_GPIO, &GPIO_Initure);
	
	XT_SCOM1_DE_DI();
		#endif
	#endif
	
	#if (XT_SCOM_SUM >= 2)
		#ifdef XT_SCOM2_UART_TX_CLK_EN
	XT_SCOM2_UART_TX_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM2_UART_TX_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_AF_PP;      //复用推挽输出
	GPIO_Initure.Pull  = GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM2_UART_TX_GPIO, &GPIO_Initure);
		#endif
		#ifdef XT_SCOM2_UART_RX_CLK_EN
	XT_SCOM2_UART_RX_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM2_UART_RX_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_INPUT;      //输入模式
	GPIO_Initure.Pull  = XT_SCOM2_UART_RX_IN_MODE;//上拉/悬空
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM2_UART_RX_GPIO, &GPIO_Initure);
		#endif
		
		#ifdef XT_SCOM2_UART_AFIO_REMAP
	XT_SCOM2_UART_AFIO_REMAP();
		#endif
		
		#ifdef XT_SCOM2_DE_CLK_EN
	XT_SCOM2_DE_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM2_DE_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull  = GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM2_DE_GPIO, &GPIO_Initure);
	
	XT_SCOM2_DE_DI();
		#endif
	#endif
	
	#if (XT_SCOM_SUM >= 3)
		#ifdef XT_SCOM3_UART_TX_CLK_EN
	XT_SCOM3_UART_TX_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM3_UART_TX_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_AF_PP;      //复用推挽输出
	GPIO_Initure.Pull  = GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM3_UART_TX_GPIO, &GPIO_Initure);
		#endif
		#ifdef XT_SCOM3_UART_RX_CLK_EN
	XT_SCOM3_UART_RX_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM3_UART_RX_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_INPUT;      //输入模式
	GPIO_Initure.Pull  = XT_SCOM3_UART_RX_IN_MODE;//上拉/悬空
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM3_UART_RX_GPIO, &GPIO_Initure);
		#endif
		
		#ifdef XT_SCOM3_UART_AFIO_REMAP
	XT_SCOM3_UART_AFIO_REMAP();
		#endif
		
		#ifdef XT_SCOM3_DE_CLK_EN
	XT_SCOM3_DE_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM3_DE_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull  = GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM3_DE_GPIO, &GPIO_Initure);
	
	XT_SCOM3_DE_DI();
		#endif
	#endif
	
	#if (XT_SCOM_SUM >= 4)
		#ifdef XT_SCOM4_UART_TX_CLK_EN
	XT_SCOM4_UART_TX_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM4_UART_TX_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_AF_PP;      //复用推挽输出
	GPIO_Initure.Pull  = GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM4_UART_TX_GPIO, &GPIO_Initure);
		#endif
		#ifdef XT_SCOM4_UART_RX_CLK_EN
	XT_SCOM4_UART_RX_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM4_UART_RX_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_INPUT;      //输入模式
	GPIO_Initure.Pull  = XT_SCOM4_UART_RX_IN_MODE;//上拉/悬空
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM4_UART_RX_GPIO, &GPIO_Initure);
		#endif
		
		#ifdef XT_SCOM4_UART_AFIO_REMAP
	XT_SCOM4_UART_AFIO_REMAP();
		#endif
		
		#ifdef XT_SCOM4_DE_CLK_EN
	XT_SCOM4_DE_CLK_EN();
	GPIO_Initure.Pin   = XT_SCOM4_DE_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull  = GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
	HAL_GPIO_Init(XT_SCOM4_DE_GPIO, &GPIO_Initure);
	
	XT_SCOM4_DE_DI();
		#endif
	#endif
}
XT_HARD_INIT_2_TAB_EXPORT(xt_scom_hw_init, "xt_scom_hw_init()");

/**
  * @brief  打开-串行通信（硬件）
  * @param  *p_ob      串行通信对象（相关初始参数）
  * @return ＜0:表示出错, ≥0:表示正确
  */
int xt_scom_hw_open(xt_scom_obj_t *p_ob)
{
	UART_HandleTypeDef huartx;      //串口句柄（HAL库标准用法则要求为全局变量）
	
	switch (p_ob->scom_num)
	{
		case 0: //串行通信-通道1
			#if (XT_SCOM_SUM >= 1)
				#ifndef XT_SCOM1_UART_TX_CLK_EN
			p_ob->tx_buf_size = 0; //本硬件不支持发送!!!
				#endif
				#ifndef XT_SCOM1_UART_RX_CLK_EN
			p_ob->rx_buf_size = 0; //本硬件不支持接收!!!
				#endif
			p_ob->p_hw_close_fn           = xt_scom_hw_close;
			p_ob->p_hw_tx_putc_null_fn    = xt_scom_putc_null_ch1;
			p_ob->p_hw_tx_putc_fn         = xt_scom_putc_ch1;
			p_ob->p_hw_tx_putc_end_fn     = xt_scom_putc_end_ch1;
			p_ob->p_hw_rx_getc_fn         = xt_scom_getc_ch1;
			p_ob->p_hw_rx_overtime_set_fn = xt_scom_set_rx_overtime_ch1;
			XT_SCOM1_UART_CLK_EN();
			HAL_NVIC_SetPriority(XT_SCOM1_UART_IRQn, XT_SCOM1_UART_PRE_INT_PRIO, XT_SCOM1_UART_SUB_INT_PRIO);
			HAL_NVIC_EnableIRQ(XT_SCOM1_UART_IRQn);
			huartx.Instance = XT_SCOM1_UART;                //串口1
			xt_p_scom_uart[0] = p_ob;
			break;
			#else
			return -101;
			#endif
		case 1: //串行通信-通道2
			#if (XT_SCOM_SUM >= 2)
				#ifndef XT_SCOM2_UART_TX_CLK_EN
			p_ob->tx_buf_size = 0; //本硬件不支持发送!!!
				#endif
				#ifndef XT_SCOM2_UART_RX_CLK_EN
			p_ob->rx_buf_size = 0; //本硬件不支持接收!!!
				#endif
			p_ob->p_hw_close_fn           = xt_scom_hw_close;
			p_ob->p_hw_tx_putc_null_fn    = xt_scom_putc_null_ch2;
			p_ob->p_hw_tx_putc_fn         = xt_scom_putc_ch2;
			p_ob->p_hw_tx_putc_end_fn     = xt_scom_putc_end_ch2;
			p_ob->p_hw_rx_getc_fn         = xt_scom_getc_ch2;
			p_ob->p_hw_rx_overtime_set_fn = xt_scom_set_rx_overtime_ch2;
			XT_SCOM2_UART_CLK_EN();
			HAL_NVIC_SetPriority(XT_SCOM2_UART_IRQn, XT_SCOM2_UART_PRE_INT_PRIO, XT_SCOM2_UART_SUB_INT_PRIO);
			HAL_NVIC_EnableIRQ(XT_SCOM2_UART_IRQn);
			huartx.Instance = XT_SCOM2_UART;                //串口2
			xt_p_scom_uart[1] = p_ob;
			break;
			#else
			return -101;
			#endif
		case 2: //串行通信-通道3
			#if (XT_SCOM_SUM >= 3)
				#ifndef XT_SCOM3_UART_TX_CLK_EN
			p_ob->tx_buf_size = 0; //本硬件不支持发送!!!
				#endif
				#ifndef XT_SCOM3_UART_RX_CLK_EN
			p_ob->rx_buf_size = 0; //本硬件不支持接收!!!
				#endif
			p_ob->p_hw_close_fn           = xt_scom_hw_close;
			p_ob->p_hw_tx_putc_null_fn    = xt_scom_putc_null_ch3;
			p_ob->p_hw_tx_putc_fn         = xt_scom_putc_ch3;
			p_ob->p_hw_tx_putc_end_fn     = xt_scom_putc_end_ch3;
			p_ob->p_hw_rx_getc_fn         = xt_scom_getc_ch3;
			p_ob->p_hw_rx_overtime_set_fn = xt_scom_set_rx_overtime_ch3;
			XT_SCOM3_UART_CLK_EN();
			HAL_NVIC_SetPriority(XT_SCOM3_UART_IRQn, XT_SCOM3_UART_PRE_INT_PRIO, XT_SCOM3_UART_SUB_INT_PRIO);
			HAL_NVIC_EnableIRQ(XT_SCOM3_UART_IRQn);
			huartx.Instance = XT_SCOM3_UART;                //串口3
			xt_p_scom_uart[2] = p_ob;
			break;
			#else
			return -101;
			#endif
		case 3: //串行通信-通道4
			#if (XT_SCOM_SUM >= 4)
				#ifndef XT_SCOM4_UART_TX_CLK_EN
			p_ob->tx_buf_size = 0; //本硬件不支持发送!!!
				#endif
				#ifndef XT_SCOM4_UART_RX_CLK_EN
			p_ob->rx_buf_size = 0; //本硬件不支持接收!!!
				#endif
			p_ob->p_hw_close_fn           = xt_scom_hw_close;
			p_ob->p_hw_tx_putc_null_fn    = xt_scom_putc_null_ch4;
			p_ob->p_hw_tx_putc_fn         = xt_scom_putc_ch4;
			p_ob->p_hw_tx_putc_end_fn     = xt_scom_putc_end_ch4;
			p_ob->p_hw_rx_getc_fn         = xt_scom_getc_ch4;
			p_ob->p_hw_rx_overtime_set_fn = xt_scom_set_rx_overtime_ch4;
			XT_SCOM4_UART_CLK_EN();
			HAL_NVIC_SetPriority(XT_SCOM4_UART_IRQn, XT_SCOM4_UART_PRE_INT_PRIO, XT_SCOM4_UART_SUB_INT_PRIO);
			HAL_NVIC_EnableIRQ(XT_SCOM4_UART_IRQn);
			huartx.Instance = XT_SCOM4_UART;                //串口4
			xt_p_scom_uart[3] = p_ob;
			break;
			#else
			return -101;
			#endif
		default:
			return -101;
	}
	
	switch (p_ob->parity_bits)
	{
		case 0: //无奇偶                                 //数据长度:9位
			huartx.Init.WordLength = (p_ob->data_bits == 9) ? UART_WORDLENGTH_9B : UART_WORDLENGTH_8B;
			huartx.Init.Parity = UART_PARITY_NONE;       //奇偶校验:无
			break;
		case 1: //奇
			huartx.Init.WordLength = UART_WORDLENGTH_9B; //数据长度:9位
			huartx.Init.Parity = UART_PARITY_ODD;        //奇偶校验:奇
			break;
		case 2: //偶
		default:
			huartx.Init.WordLength = UART_WORDLENGTH_9B; //数据长度:9位
			huartx.Init.Parity = UART_PARITY_EVEN;       //奇偶校验:偶
			break;
	}
	switch (p_ob->stop_bits)
	{
		case 1:
			huartx.Init.StopBits = UART_STOPBITS_1;      //停止位:1位
			break;
		case 2:
		default:
			huartx.Init.StopBits = UART_STOPBITS_2;      //停止位:2位
			break;
	}
	huartx.gState = HAL_UART_STATE_RESET;                //初始化
	huartx.Init.BaudRate = p_ob->baud_rate;              //波特率:9600等
	huartx.Init.Mode = UART_MODE_TX_RX;                  //发送及接收使能(后面会关闭发送中断)
	huartx.Init.HwFlowCtl = UART_HWCONTROL_NONE;         //流控制:无
	huartx.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huartx) != HAL_OK) {}
	
	__HAL_UART_DISABLE_IT(&huartx, UART_IT_TC);          //关发送中断 
	__HAL_UART_DISABLE_IT(&huartx, UART_IT_TXE);         //关发送中断
	__HAL_UART_DISABLE_IT(&huartx, UART_IT_RXNE);        //关接收中断
	__HAL_UART_CLEAR_FLAG(&huartx, UART_FLAG_TC);        //清除发送完成中断标志
	__HAL_UART_CLEAR_FLAG(&huartx, UART_FLAG_RXNE);      //清除接收非空中断标志
	if (p_ob->rx_buf_size != 0)
	{
		__HAL_UART_ENABLE_IT(&huartx, UART_IT_RXNE);     //开接收中断
	}
	return (p_ob->scom_num);
}

/**
  * @brief  关闭-串行通信（硬件）
  * @param  *p_ob      串行通信对象
  * @return void
  */
void xt_scom_hw_close(xt_scom_obj_t *p_ob)
{
	//XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
	switch (p_ob->scom_num)
	{
		case 0: //串行通信-通道1
		{
			#if (XT_SCOM_SUM >= 1)
			struct { USART_TypeDef *Instance; } huart = { XT_SCOM1_UART };
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);   //关发送中断 
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);  //关发送中断
			__HAL_UART_DISABLE_IT(&huart, UART_IT_RXNE); //关接收中断
			XT_SCOM1_UART_CLK_DI();
			xt_p_scom_uart[0] = 0;
			break;
			#endif
		}
		case 1: //串行通信-通道2
		{
			#if (XT_SCOM_SUM >= 2)
			struct { USART_TypeDef *Instance; } huart = { XT_SCOM2_UART };
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);   //关发送中断 
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);  //关发送中断
			__HAL_UART_DISABLE_IT(&huart, UART_IT_RXNE); //关接收中断
			XT_SCOM2_UART_CLK_DI();
			xt_p_scom_uart[1] = 0;
			break;
			#endif
		}
		case 2: //串行通信-通道3
		{
			#if (XT_SCOM_SUM >= 3)
			struct { USART_TypeDef *Instance; } huart = { XT_SCOM3_UART };
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);   //关发送中断 
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);  //关发送中断
			__HAL_UART_DISABLE_IT(&huart, UART_IT_RXNE); //关接收中断
			XT_SCOM3_UART_CLK_DI();
			xt_p_scom_uart[2] = 0;
			break;
			#endif
		}
		case 3: //串行通信-通道4
		default:
		{
			#if (XT_SCOM_SUM >= 4)
			struct { USART_TypeDef *Instance; } huart = { XT_SCOM4_UART };
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);   //关发送中断 
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);  //关发送中断
			__HAL_UART_DISABLE_IT(&huart, UART_IT_RXNE); //关接收中断
			XT_SCOM4_UART_CLK_DI();
			xt_p_scom_uart[3] = 0;
			break;
			#endif
		}
	}
	//XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  发送前-相关控制（硬件）
  * @param  void
  * @return void
  */
#if (XT_SCOM_SUM >= 1)
void xt_scom_putc_null_ch1(void)
{
	#ifdef XT_SCOM1_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM1_UART };
	
	//XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		#ifdef XT_SCOM1_DE_CLK_EN
	XT_SCOM1_DE_EN();
		#endif
	
	__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
	__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
	__HAL_UART_ENABLE_IT(&huart, UART_IT_TXE); //启动发送中断：利用发送空中断触发,提高发送连贯性
	//XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	#endif
}
#endif
#if (XT_SCOM_SUM >= 2)
void xt_scom_putc_null_ch2(void)
{
	#ifdef XT_SCOM2_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM2_UART };
	
	//XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		#ifdef XT_SCOM2_DE_CLK_EN
	XT_SCOM2_DE_EN();
		#endif
	
	__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
	__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
	__HAL_UART_ENABLE_IT(&huart, UART_IT_TXE); //启动发送中断：利用发送空中断触发,提高发送连贯性
	//XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	#endif
}
#endif
#if (XT_SCOM_SUM >= 3)
void xt_scom_putc_null_ch3(void)
{
	#ifdef XT_SCOM3_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM3_UART };
	
	//XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		#ifdef XT_SCOM3_DE_CLK_EN
	XT_SCOM3_DE_EN();
		#endif
	
	__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
	__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
	__HAL_UART_ENABLE_IT(&huart, UART_IT_TXE); //启动发送中断：利用发送空中断触发,提高发送连贯性
	//XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	#endif
}
#endif
#if (XT_SCOM_SUM >= 4)
void xt_scom_putc_null_ch4(void)
{
	#ifdef XT_SCOM4_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM4_UART };
	
	//XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		#ifdef XT_SCOM4_DE_CLK_EN
	XT_SCOM4_DE_EN();
		#endif
	
	__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
	__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
	__HAL_UART_ENABLE_IT(&huart, UART_IT_TXE); //启动发送中断：利用发送空中断触发,提高发送连贯性
	//XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	#endif
}
#endif

/**
  * @brief  发送中-1字节数据（硬件）
  * @param  d          一字节数据（预留数据第9bit发送）
  * @param  f          状态标志（0:第1字节,1:中途1字节,2:最后1字节）
  * @return void
  */
#if (XT_SCOM_SUM >= 1)
void xt_scom_putc_ch1(uint16_t d, uint8_t f)
{
	#ifdef XT_SCOM1_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM1_UART };
	
	//XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
	XT_SCOM1_UART->DR = d; //对DR的写操作可将空中断标志清零
	if (f == 2)
	{                      //设置最后一字节发送完成中断
		__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);
		__HAL_UART_ENABLE_IT(&huart, UART_IT_TC);
	}
	//XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	#endif
}
#endif
#if (XT_SCOM_SUM >= 2)
void xt_scom_putc_ch2(uint16_t d, uint8_t f)
{
	#ifdef XT_SCOM2_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM2_UART };
	
	//XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
	XT_SCOM2_UART->DR = d; //对DR的写操作可将空中断标志清零
	if (f == 2)
	{                      //设置最后一字节发送完成中断
		__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);
		__HAL_UART_ENABLE_IT(&huart, UART_IT_TC);
	}
	//XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	#endif
}
#endif
#if (XT_SCOM_SUM >= 3)
void xt_scom_putc_ch3(uint16_t d, uint8_t f)
{
	#ifdef XT_SCOM3_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM3_UART };
	
	//XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
	XT_SCOM3_UART->DR = d; //对DR的写操作可将空中断标志清零
	if (f == 2)
	{                      //设置最后一字节发送完成中断
		__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);
		__HAL_UART_ENABLE_IT(&huart, UART_IT_TC);
	}
	//XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	#endif
}
#endif
#if (XT_SCOM_SUM >= 4)
void xt_scom_putc_ch4(uint16_t d, uint8_t f)
{
	#ifdef XT_SCOM4_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM4_UART };
	
	//XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
	XT_SCOM4_UART->DR = d; //对DR的写操作可将空中断标志清零
	if (f == 2)
	{                      //设置最后一字节发送完成中断
		__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);
		__HAL_UART_ENABLE_IT(&huart, UART_IT_TC);
	}
	//XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	#endif
}
#endif

/**
  * @brief  发送完-相关控制（硬件）
  * @param  void
  * @return void
  */
#if (XT_SCOM_SUM >= 1)
void xt_scom_putc_end_ch1(void)
{
	#ifdef XT_SCOM1_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM1_UART };
	
	__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
	__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
	
		#ifdef XT_SCOM1_DE_CLK_EN
	XT_SCOM1_DE_DI();
		#endif
	#endif
}
#endif
#if (XT_SCOM_SUM >= 2)
void xt_scom_putc_end_ch2(void)
{
	#ifdef XT_SCOM2_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM2_UART };
	
	__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
	__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
	
		#ifdef XT_SCOM2_DE_CLK_EN
	XT_SCOM2_DE_DI();
		#endif
	#endif
}
#endif
#if (XT_SCOM_SUM >= 3)
void xt_scom_putc_end_ch3(void)
{
	#ifdef XT_SCOM3_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM3_UART };
	
	__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
	__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
	
		#ifdef XT_SCOM3_DE_CLK_EN
	XT_SCOM3_DE_DI();
		#endif
	#endif
}
#endif
#if (XT_SCOM_SUM >= 4)
void xt_scom_putc_end_ch4(void)
{
	#ifdef XT_SCOM4_UART_TX_CLK_EN
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM4_UART };
	
	__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
	__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
	
		#ifdef XT_SCOM4_DE_CLK_EN
	XT_SCOM4_DE_DI();
		#endif
	#endif
}
#endif

/**
  * @brief  获取-1字节数据（硬件）
  * @param  void
  * @return 获取到的1字节数据（预留数据第9bit获取）
  */
#if (XT_SCOM_SUM >= 1)
int xt_scom_getc_ch1(void)
{
	return (XT_SCOM1_UART->DR & 0x01FF); //对DR的读操作可将RXNE位清零
}
#endif
#if (XT_SCOM_SUM >= 2)
int xt_scom_getc_ch2(void)
{
	return (XT_SCOM2_UART->DR & 0x01FF); //对DR的读操作可将RXNE位清零
}
#endif
#if (XT_SCOM_SUM >= 3)
int xt_scom_getc_ch3(void)
{
	return (XT_SCOM3_UART->DR & 0x01FF); //对DR的读操作可将RXNE位清零
}
#endif
#if (XT_SCOM_SUM >= 4)
int xt_scom_getc_ch4(void)
{
	return (XT_SCOM4_UART->DR & 0x01FF); //对DR的读操作可将RXNE位清零
}
#endif

/**
  * @brief  设置-接收超时时间（硬件）
  * @param  us         超时时间（单位:us）
  * @return 是否存在超时中断标志(如果没有，表明被串口中断处理清除了，则无需处理超时事件)
  */
#if (XT_SCOM_SUM >= 1)
uint8_t xt_scom_set_rx_overtime_ch1(uint16_t us)
{
	uint8_t it = (XT_SCOMx_TIM->SR & TIM_IT_CC1) ? 1 : 0;
	XT_SCOMx_TIM->SR = ~TIM_IT_CC1;
	XT_SCOMx_TIM->CCR1 = XT_SCOMx_TIM->CNT + us;
	return it;
}
#endif
#if (XT_SCOM_SUM >= 2)
uint8_t xt_scom_set_rx_overtime_ch2(uint16_t us)
{
	uint8_t it = (XT_SCOMx_TIM->SR & TIM_IT_CC2) ? 1 : 0;
	XT_SCOMx_TIM->SR = ~TIM_IT_CC2;
	XT_SCOMx_TIM->CCR2 = XT_SCOMx_TIM->CNT + us;
	return it;
}
#endif
#if (XT_SCOM_SUM >= 3)
uint8_t xt_scom_set_rx_overtime_ch3(uint16_t us)
{
	uint8_t it = (XT_SCOMx_TIM->SR & TIM_IT_CC3) ? 1 : 0;
	XT_SCOMx_TIM->SR = ~TIM_IT_CC3;
	XT_SCOMx_TIM->CCR3 = XT_SCOMx_TIM->CNT + us;
	return it;
}
#endif
#if (XT_SCOM_SUM >= 4)
uint8_t xt_scom_set_rx_overtime_ch4(uint16_t us)
{
	uint8_t it = (XT_SCOMx_TIM->SR & TIM_IT_CC4) ? 1 : 0;
	XT_SCOMx_TIM->SR = ~TIM_IT_CC4;
	XT_SCOMx_TIM->CCR4 = XT_SCOMx_TIM->CNT + us;
	return it;
}
#endif

/**
  * @brief  串口中断
  * @param  void
  * @return void
  */
#if (XT_SCOM_SUM >= 1)
void XT_SCOM1_UART_IRQHandler(void)
{
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM1_UART };
	
	/* enter interrupt */
	XT_SCOM_OS_INT_ENTER();
	
	//接收数据
	if (((XT_SCOM1_UART->SR & USART_SR_RXNE) != RESET) 
	&&  ((XT_SCOM1_UART->CR1 & USART_CR1_RXNEIE) != RESET))
	{
		if (xt_p_scom_uart[0] != 0)
		{
			(*(xt_p_scom_uart[0]->p_dv_uart_rx_fn))(xt_p_scom_uart[0]); //中断里操作串行通信对象
		}
		else
		{
			if ((XT_SCOM1_UART->DR & 0x01FF) == 0){} else{}         //对DR的读操作可将RXNE位清零
		}
	}
	
	//发送数据
	if (((XT_SCOM1_UART->SR & USART_SR_TXE) != RESET)
	&&  ((XT_SCOM1_UART->CR1 & USART_CR1_TXEIE) != RESET))
	{
		if (xt_p_scom_uart[0] != 0)
		{
			(*(xt_p_scom_uart[0]->p_dv_uart_tx_fn))(xt_p_scom_uart[0]); //中断里操作串行通信对象
		}
		else
		{
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);
		}
	}
	if (((XT_SCOM1_UART->SR & USART_SR_TC) != RESET)
	&&  ((XT_SCOM1_UART->CR1 & USART_CR1_TCIE) != RESET))
	{
		if (xt_p_scom_uart[0] != 0)
		{
			(*(xt_p_scom_uart[0]->p_dv_uart_tx_fn))(xt_p_scom_uart[0]); //中断里操作串行通信对象
		}
		else
		{
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
			__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
		}
	}
	
	//接收溢出
	if ((XT_SCOM1_UART->SR & USART_SR_ORE) != RESET)
	{
		if (XT_SCOM1_UART->SR & USART_SR_ORE){} else{} //读SR其实就是清除标志
		if (XT_SCOM1_UART->DR & 0x01FF)      {} else{} //读DR
		
		(*(xt_p_scom_uart[0]->p_dv_uart_rx_reset))(xt_p_scom_uart[0]); //中断里操作串行通信对象
	}
	
	/* leave interrupt */
	XT_SCOM_OS_INT_EXIT();
}
#endif
#if (XT_SCOM_SUM >= 2)
void XT_SCOM2_UART_IRQHandler(void)
{
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM2_UART };
	
	/* enter interrupt */
	XT_SCOM_OS_INT_ENTER();
	
	//接收数据
	if (((XT_SCOM2_UART->SR & USART_SR_RXNE) != RESET) 
	&&  ((XT_SCOM2_UART->CR1 & USART_CR1_RXNEIE) != RESET))
	{
		if (xt_p_scom_uart[1] != 0)
		{
			(*(xt_p_scom_uart[1]->p_dv_uart_rx_fn))(xt_p_scom_uart[1]); //中断里操作串行通信对象
		}
		else
		{
			if ((XT_SCOM2_UART->DR & 0x01FF) == 0){} else{}         //对DR的读操作可将RXNE位清零
		}
	}
	
	//发送数据
	if (((XT_SCOM2_UART->SR & USART_SR_TXE) != RESET)
	&&  ((XT_SCOM2_UART->CR1 & USART_CR1_TXEIE) != RESET))
	{
		if (xt_p_scom_uart[1] != 0)
		{
			(*(xt_p_scom_uart[1]->p_dv_uart_tx_fn))(xt_p_scom_uart[1]); //中断里操作串行通信对象
		}
		else
		{
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);
		}
	}
	if (((XT_SCOM2_UART->SR & USART_SR_TC) != RESET)
	&&  ((XT_SCOM2_UART->CR1 & USART_CR1_TCIE) != RESET))
	{
		if (xt_p_scom_uart[1] != 0)
		{
			(*(xt_p_scom_uart[1]->p_dv_uart_tx_fn))(xt_p_scom_uart[1]); //中断里操作串行通信对象
		}
		else
		{
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
			__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
		}
	}
	
	//接收溢出
	if ((XT_SCOM2_UART->SR & USART_SR_ORE) != RESET)
	{
		if (XT_SCOM2_UART->SR & USART_SR_ORE){} else{} //读SR其实就是清除标志
		if (XT_SCOM2_UART->DR & 0x01FF)      {} else{} //读DR
		
		(*(xt_p_scom_uart[1]->p_dv_uart_rx_reset))(xt_p_scom_uart[1]); //中断里操作串行通信对象
	}
	
	/* leave interrupt */
	XT_SCOM_OS_INT_EXIT();
}
#endif
#if (XT_SCOM_SUM >= 3)
void XT_SCOM3_UART_IRQHandler(void)
{
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM3_UART };
	
	/* enter interrupt */
	XT_SCOM_OS_INT_ENTER();
	
	//接收数据
	if (((XT_SCOM3_UART->SR & USART_SR_RXNE) != RESET) 
	&&  ((XT_SCOM3_UART->CR1 & USART_CR1_RXNEIE) != RESET))
	{
		if (xt_p_scom_uart[2] != 0)
		{
			(*(xt_p_scom_uart[2]->p_dv_uart_rx_fn))(xt_p_scom_uart[2]); //中断里操作串行通信对象
		}
		else
		{
			if ((XT_SCOM3_UART->DR & 0x01FF) == 0){} else{}         //对DR的读操作可将RXNE位清零
		}
	}
	
	//发送数据
	if (((XT_SCOM3_UART->SR & USART_SR_TXE) != RESET)
	&&  ((XT_SCOM3_UART->CR1 & USART_CR1_TXEIE) != RESET))
	{
		if (xt_p_scom_uart[2] != 0)
		{
			(*(xt_p_scom_uart[2]->p_dv_uart_tx_fn))(xt_p_scom_uart[2]); //中断里操作串行通信对象
		}
		else
		{
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);
		}
	}
	if (((XT_SCOM3_UART->SR & USART_SR_TC) != RESET)
	&&  ((XT_SCOM3_UART->CR1 & USART_CR1_TCIE) != RESET))
	{
		if (xt_p_scom_uart[2] != 0)
		{
			(*(xt_p_scom_uart[2]->p_dv_uart_tx_fn))(xt_p_scom_uart[2]); //中断里操作串行通信对象
		}
		else
		{
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
			__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
		}
	}
	
	//接收溢出
	if ((XT_SCOM3_UART->SR & USART_SR_ORE) != RESET)
	{
		if (XT_SCOM3_UART->SR & USART_SR_ORE){} else{} //读SR其实就是清除标志
		if (XT_SCOM3_UART->DR & 0x01FF)      {} else{} //读DR
		
		(*(xt_p_scom_uart[2]->p_dv_uart_rx_reset))(xt_p_scom_uart[2]); //中断里操作串行通信对象
	}
	
	/* leave interrupt */
	XT_SCOM_OS_INT_EXIT();
}
#endif
#if (XT_SCOM_SUM >= 4)
void XT_SCOM4_UART_IRQHandler(void)
{
	struct { USART_TypeDef *Instance; } huart = { XT_SCOM4_UART };
	
	/* enter interrupt */
	XT_SCOM_OS_INT_ENTER();
	
	//接收数据
	if (((XT_SCOM4_UART->SR & USART_SR_RXNE) != RESET) 
	&&  ((XT_SCOM4_UART->CR1 & USART_CR1_RXNEIE) != RESET))
	{
		if (xt_p_scom_uart[3] != 0)
		{
			(*(xt_p_scom_uart[3]->p_dv_uart_rx_fn))(xt_p_scom_uart[3]); //中断里操作串行通信对象
		}
		else
		{
			if ((XT_SCOM4_UART->DR & 0x01FF) == 0){} else{}         //对DR的读操作可将RXNE位清零
		}
	}
	
	//发送数据
	if (((XT_SCOM4_UART->SR & USART_SR_TXE) != RESET)
	&&  ((XT_SCOM4_UART->CR1 & USART_CR1_TXEIE) != RESET))
	{
		if (xt_p_scom_uart[3] != 0)
		{
			(*(xt_p_scom_uart[3]->p_dv_uart_tx_fn))(xt_p_scom_uart[3]); //中断里操作串行通信对象
		}
		else
		{
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TXE);
		}
	}
	if (((XT_SCOM4_UART->SR & USART_SR_TC) != RESET)
	&&  ((XT_SCOM4_UART->CR1 & USART_CR1_TCIE) != RESET))
	{
		if (xt_p_scom_uart[3] != 0)
		{
			(*(xt_p_scom_uart[3]->p_dv_uart_tx_fn))(xt_p_scom_uart[3]); //中断里操作串行通信对象
		}
		else
		{
			__HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
			__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_TC);
		}
	}
	
	//接收溢出
	if ((XT_SCOM4_UART->SR & USART_SR_ORE) != RESET)
	{
		if (XT_SCOM4_UART->SR & USART_SR_ORE){} else{} //读SR其实就是清除标志
		if (XT_SCOM4_UART->DR & 0x01FF)      {} else{} //读DR
		
		(*(xt_p_scom_uart[3]->p_dv_uart_rx_reset))(xt_p_scom_uart[3]); //中断里操作串行通信对象
	}
	
	/* leave interrupt */
	XT_SCOM_OS_INT_EXIT();
}
#endif

/**
  * @brief  定时器中断
  * @param  void
  * @return void
  */
void XT_SCOMx_TIM_IRQHandler(void)
{
	/* enter interrupt */
	XT_SCOM_OS_INT_ENTER();

	if (((XT_SCOMx_TIM->SR & TIM_FLAG_CC1) != RESET)
	&&  ((XT_SCOMx_TIM->DIER & TIM_IT_CC1) != RESET))
	{
		#if (XT_SCOM_SUM >= 1)
		if (xt_p_scom_uart[0] != 0)
		{
			(*(xt_p_scom_uart[0]->p_dv_time_rx_fn))(xt_p_scom_uart[0]); //中断里操作串行通信对象
		}
		else
		#endif
		{
			XT_SCOMx_TIM->SR = ~TIM_IT_CC1;
			XT_SCOMx_TIM->CCR1 = XT_SCOMx_TIM->CNT + XT_SCOMx_TIM_CCR1;
		}
	}
	if (((XT_SCOMx_TIM->SR & TIM_FLAG_CC2) != RESET)
	&&  ((XT_SCOMx_TIM->DIER & TIM_IT_CC2) != RESET))
	{
		#if (XT_SCOM_SUM >= 2)
		if (xt_p_scom_uart[1] != 0)
		{
			(*(xt_p_scom_uart[1]->p_dv_time_rx_fn))(xt_p_scom_uart[1]); //中断里操作串行通信对象
		}
		else
		#endif
		{
			XT_SCOMx_TIM->SR = ~TIM_IT_CC2;
			XT_SCOMx_TIM->CCR2 = XT_SCOMx_TIM->CNT + XT_SCOMx_TIM_CCR2;
		}
	}
	if (((XT_SCOMx_TIM->SR & TIM_FLAG_CC3) != RESET)
	&&  ((XT_SCOMx_TIM->DIER & TIM_IT_CC3) != RESET))
	{
		#if (XT_SCOM_SUM >= 3)
		if (xt_p_scom_uart[2] != 0)
		{
			(*(xt_p_scom_uart[2]->p_dv_time_rx_fn))(xt_p_scom_uart[2]); //中断里操作串行通信对象
		}
		else
		#endif
		{
			XT_SCOMx_TIM->SR = ~TIM_IT_CC3;
			XT_SCOMx_TIM->CCR3 = XT_SCOMx_TIM->CNT + XT_SCOMx_TIM_CCR3;
		}
	}
	if (((XT_SCOMx_TIM->SR & TIM_FLAG_CC4) != RESET)
	&&  ((XT_SCOMx_TIM->DIER & TIM_IT_CC4) != RESET))
	{
		#if (XT_SCOM_SUM >= 4)
		if (xt_p_scom_uart[3] != 0)
		{
			(*(xt_p_scom_uart[3]->p_dv_time_rx_fn))(xt_p_scom_uart[3]); //中断里操作串行通信对象
		}
		else
		#endif
		{
			XT_SCOMx_TIM->SR = ~TIM_IT_CC4;
			XT_SCOMx_TIM->CCR4 = XT_SCOMx_TIM->CNT + XT_SCOMx_TIM_CCR4;
		}
	}
	
	/* leave interrupt */
	XT_SCOM_OS_INT_EXIT();
}

#endif  //#if (XT_APP_SCOM_EN == XT_DEF_ENABLED)
#endif
