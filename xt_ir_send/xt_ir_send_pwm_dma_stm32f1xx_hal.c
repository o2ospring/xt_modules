/**
  * @file  xt_ir_send_pwm_dma_stm32f1xx_hal.c
  * @brief 红外发送服务模块硬件层--STM32F103单片机-HAL库（用到硬件资源：TIM_PWM+DMA）
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-11-16     o2ospring    原始版本
  *                             1.其原理是通过DMA将RAM缓冲的PWM占空比数据输出至TIM_PWM通道；
  *                             2.开辟的RAM越大则产生DMA中断就越少，即：以空间换取执行效率；
  *                             3.上层设置PWM频率越高则DMA中断频率也越高，要合理配置RAM大小
  */
#define   XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_C__
#include "xt_ir_send_pwm_dma_stm32f1xx_hal.h"
#if (XT_IRSEND_HW_DRIVERS_EN != 0)
#if (defined XT_APP_IRSEND_EN) && (XT_APP_IRSEND_EN == XT_DEF_ENABLED)
#ifdef    XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_X__
#undef    XT_IR_SEND_PWM_DMA_STM32F1XX_HAL_H__
#include "xt_ir_send_pwm_dma_stm32f1xx_hal.h"
#endif

#ifndef XT_HARD_INIT_2_TAB_EXPORT
#define XT_HARD_INIT_2_TAB_EXPORT(func,name)
#endif

#if (XT_IRSEND_SUM != 1)
#error "本硬件驱动只支持一路红外码发送!"
#endif

DMA_HandleTypeDef xt_irsend_hdma;
TIM_HandleTypeDef xt_irsend_htim_pwm;
xt_irsend_obj_t *xt_p_irsend_pwm = 0;
uint32_t xt_irsend_pwm_cnt; //发送方波个数计数
uint32_t xt_irsend_pwm_sum; //需要发送方波总数
uint16_t xt_irsend_pwm_val[XT_IRSEND_RAM_SUM + 1];
uint16_t xt_irsend_pwm_100; //发送方波频率100%
uint8_t  xt_irsend_pwm_khz; //发送方波频率大小

void xt_irsend_hw_init(void);
int xt_irsend_hw_open(xt_irsend_obj_t *p_ob);
void xt_irsend_hw_close_cb(xt_irsend_obj_t *p_ob);
void xt_irsend_hw_putd_before_cb(uint8_t khz);
void xt_irsend_hw_putd_a_cb(uint32_t us, uint8_t f);
void xt_irsend_hw_putd_b_cb(uint32_t us, uint8_t f);
void xt_irsend_hw_putd_end_cb(void);
void XT_IRSEND_DMA_IRQHandler(void);
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);

/**
  * @brief  初始化硬件资源-上电
  * @param  void
  * @return void
  */
void xt_irsend_hw_init(void)
{
	__HAL_RCC_AFIO_CLK_ENABLE();
}
XT_HARD_INIT_2_TAB_EXPORT(xt_irsend_hw_init, "xt_irsend_hw_init()");

/**
  * @brief  打开-红外发送服务（硬件）
  * @param  *p_ob      红外发送服务对象
  * @return ＜0:表示出错, ≥0:表示正确
  */
int xt_irsend_hw_open(xt_irsend_obj_t *p_ob)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	if (xt_p_irsend_pwm != 0)
	{
		return -101;
	}
	xt_p_irsend_pwm = p_ob;
	p_ob->p_hw_close_fn       = xt_irsend_hw_close_cb;
	p_ob->p_hw_putd_before_fn = xt_irsend_hw_putd_before_cb;
	p_ob->p_hw_putd_a_fn      = xt_irsend_hw_putd_a_cb;
	p_ob->p_hw_putd_b_fn      = xt_irsend_hw_putd_b_cb;
	p_ob->p_hw_putd_end_fn    = xt_irsend_hw_putd_end_cb;
	
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//__HAL_RCC_AFIO_CLK_ENABLE();
	
	/* TIM,DMA controller clock enable */
	XT_IRSEND_DMA_CLK_ENABLE();
	XT_IRSEND_PWM_CLK_ENABLE();
	XT_IRSEND_TIM_CLK_EN();
	
	memset((char *)(&GPIO_InitStruct), 0, sizeof(GPIO_InitStruct));
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = XT_IRSEND_PWM_PIN;
	HAL_GPIO_Init(XT_IRSEND_PWM_GPIO, &GPIO_InitStruct);
	
	#ifdef XT_IRSEND_TIM_AFIO_REMAP
	XT_IRSEND_TIM_AFIO_REMAP();
	#endif
	
	/* DMAx_Channelx_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(XT_IRSEND_DMA_IRQn, XT_IRSEND_DMA_PRE_INT_PRIO, XT_IRSEND_DMA_SUB_INT_PRIO);
	HAL_NVIC_EnableIRQ(XT_IRSEND_DMA_IRQn);
	
	//标准库使用回调函数设置: void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm) {
	/* TIMx DMA Init */
	/* TIMx_CHx_UP Init */
	xt_irsend_hdma.Instance = XT_IRSEND_DMA_CHANNEL;                      //DMA通道（具体请查手册，如:DMA1_Channel3）
	xt_irsend_hdma.Init.Direction = DMA_MEMORY_TO_PERIPH;                 //数据方向：[RAM]→[外设]
	xt_irsend_hdma.Init.PeriphInc = DMA_PINC_DISABLE;                     //[外设]地址不用自加（只有一个外设）
	xt_irsend_hdma.Init.MemInc = DMA_MINC_ENABLE;                         //[RAM ]地址需要自加
	xt_irsend_hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    //[外设]数据宽度：半个字（2个字节）
	xt_irsend_hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       //[RAM ]数据宽度：半个字（2个字节）
	xt_irsend_hdma.Init.Mode = DMA_NORMAL;                                //正常模式（非循环模式）
	xt_irsend_hdma.Init.Priority = XT_IRSEND_DMA_PRIORITY;                //如:中等优先级
	if (HAL_DMA_Init(&xt_irsend_hdma) != HAL_OK)
	{
		//Error_Handler();
	}
	
	//实测移到定时器初始之后更合理，否则上电后发送第1个PWM波有毛刺：
	/* Several peripheral DMA handle pointers point to the same DMA handle.
	Be aware that there is only one channel to perform all the requested DMAs. */
	//__HAL_LINKDMA(&xt_irsend_htim_pwm, hdma[XT_IRSEND_TIM_DMA_ID_CC], xt_irsend_hdma);
	//__HAL_LINKDMA(&xt_irsend_htim_pwm, hdma[TIM_DMA_ID_UPDATE], xt_irsend_hdma);
	
	xt_irsend_pwm_khz = p_ob->ir_khz;
	p_ob->ir_khz += 1; //强制+1,为下行函数可执行相关硬件初始化┐
	xt_irsend_hw_putd_before_cb(xt_irsend_pwm_khz); //←───┘
	
	return 0;
}

/**
  * @brief  关闭-红外发送服务（硬件）
  * @param  *p_ob      红外发送服务对象
  * @return void
  */
void xt_irsend_hw_close_cb(xt_irsend_obj_t *p_ob)
{
	//XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
	if (xt_p_irsend_pwm == p_ob)
	{
		HAL_TIM_PWM_Stop_DMA(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL);
		XT_IRSEND_TIM_CLK_DI();
		HAL_DMA_DeInit(xt_irsend_htim_pwm.hdma[XT_IRSEND_TIM_DMA_ID_CC]);
		HAL_DMA_DeInit(xt_irsend_htim_pwm.hdma[TIM_DMA_ID_UPDATE]);
		HAL_GPIO_DeInit(XT_IRSEND_PWM_GPIO, XT_IRSEND_PWM_PIN);
		xt_p_irsend_pwm = 0;
	}
	//XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  发送前-相关控制（硬件）
  * @param  khz        红外载波频率（单位:KHz）
  * @return void
  */
void xt_irsend_hw_putd_before_cb(uint8_t khz)
{
	union
	{
		TIM_MasterConfigTypeDef sMasterConfig;
		TIM_OC_InitTypeDef sConfigOC;
		#if (defined XT_IRSEND_PWM_OCIDLESTATE)
		TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
		#endif
	}un;
	
	//XT_IRSEND_TASK_LOCKED(); //>>>>>>>>>>>>>>
	if (xt_p_irsend_pwm == 0)
	{
		return;
	}
	xt_irsend_pwm_khz = khz;
	xt_irsend_pwm_100 = XT_IRSEND_TIM_PWM_HZ((uint32_t)khz * 1000UL);
	
	if (khz != xt_p_irsend_pwm->ir_khz)
	{
		//__HAL_RCC_AFIO_CLK_ENABLE();
		
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
		
		xt_irsend_htim_pwm.Instance = XT_IRSEND_TIM;                      //定时器
		xt_irsend_htim_pwm.Init.Prescaler = XT_IRSEND_TIM_DIV;            //分频系数XT_IRSEND_TIM_DIV+1=分频值
		xt_irsend_htim_pwm.Init.CounterMode = TIM_COUNTERMODE_UP;         //向上计数
		xt_irsend_htim_pwm.Init.Period = xt_irsend_pwm_100;               //计数器翻转最大值←((72000000/(XT_IRSEND_TIM_DIV+1)/((uint32_t)khz*1000UL))-1)
		xt_irsend_htim_pwm.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;   //时钟分频因子
		xt_irsend_htim_pwm.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; //关闭自动装载
		if (HAL_TIM_PWM_Init(&xt_irsend_htim_pwm) != HAL_OK)
		{}
		memset((char *)(&(un.sMasterConfig)), 0, sizeof(un.sMasterConfig));
		un.sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		un.sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&xt_irsend_htim_pwm, &(un.sMasterConfig)) != HAL_OK)
		{}
		
		#if (defined XT_IRSEND_PWM_OCIDLESTATE) //针对TIM1/TIM8
		memset((char *)(&(un.sBreakDeadTimeConfig)), 0, sizeof(un.sBreakDeadTimeConfig));
		un.sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
		un.sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
		un.sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
		un.sBreakDeadTimeConfig.DeadTime = 0;
		un.sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
		un.sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
		un.sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
		if (HAL_TIMEx_ConfigBreakDeadTime(&xt_irsend_htim_pwm, &(un.sBreakDeadTimeConfig)) != HAL_OK)
		{}
		#endif
		
		//PWM 工作参数设置-----------------------------------------------------------------------------------
		HAL_TIM_PWM_Stop(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL); //关闭PWM输出(上电默认是关闭,平时必须关闭才能设置)
		memset((char *)(&(un.sConfigOC)), 0, sizeof(un.sConfigOC));
		un.sConfigOC.OCMode = XT_IRSEND_PWM_OCMODE;                    //是否反极性输出(反板性表现为电平翻转,前提:非停止状态)
		un.sConfigOC.Pulse = xt_irsend_pwm_100 / 2;                    //xt_irsend_pwm_100/2=50%占空比
		un.sConfigOC.OCPolarity = XT_IRSEND_PWM_OCPOLARITY;            //正向通道有效占空比输出电平
		un.sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;                //反向通道有效占空比输出为低电平(只针对TIM1)
		un.sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;              //正向通道空闲（停止）时输出电平(只针对TIM1)(其它定时器默认为低电平)
		un.sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;            //反向通道空闲（停止）时输出电平(只针对TIM1)
		un.sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		#ifdef XT_IRSEND_PWM_OCIDLESTATE
		un.sConfigOC.OCIdleState = XT_IRSEND_PWM_OCIDLESTATE;          //针对TIM1
		#endif
		if (HAL_TIM_PWM_ConfigChannel(&xt_irsend_htim_pwm, &(un.sConfigOC), XT_IRSEND_PWM_TIM_CHANNEL) != HAL_OK)
		{}
		
		//实测移到定时器初始之后更合理，否则上电后发送第1个PWM波有毛刺：
		/* Several peripheral DMA handle pointers point to the same DMA handle.
		Be aware that there is only one channel to perform all the requested DMAs. */
		__HAL_LINKDMA(&xt_irsend_htim_pwm, hdma[XT_IRSEND_TIM_DMA_ID_CC], xt_irsend_hdma);
		__HAL_LINKDMA(&xt_irsend_htim_pwm, hdma[TIM_DMA_ID_UPDATE], xt_irsend_hdma);
	
		//HAL_TIM_PWM_Start(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL);                                                        //启动PWM输出（如果为非DMA由本函数启动）
		//HAL_TIM_PWM_Start_DMA(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL, (uint32_t *)(xt_irsend_pwm_val), XT_IRSEND_RAM_SUM);//启动PWM输出（如果是DMA则由本函数启动）
		
		// 临时修改[50%占空比方波输出]例子!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// XT_IRSEND_TIM->XT_IRSEND_PWM_CCR = xt_irsend_pwm_100/2; -> 50%点空比方波输出
	}
	xt_irsend_pwm_cnt = 0;
	//XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}

/**
  * @brief  发送中-发送有载波一个码（硬件）
  * @param  us         红外发送有载波的时间（单位:us）
  * @param  f          状态标志（0:第1个码,1:中途1个码,2:最后1个码）
  * @return void
  */
void xt_irsend_hw_putd_a_cb(uint32_t us, uint8_t f)
{
	int i;
	uint16_t k;
	
	//XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
	xt_irsend_pwm_sum = (us * (uint32_t)xt_irsend_pwm_khz) / 1000UL;
	if (xt_irsend_pwm_sum < 1)
	{
		xt_irsend_pwm_sum = 1;
	}
	k = xt_irsend_pwm_100 / 2;
	if (xt_irsend_pwm_sum >= XT_IRSEND_RAM_SUM)
	{
		xt_irsend_pwm_cnt = XT_IRSEND_RAM_SUM;
		for (i=0; i<XT_IRSEND_RAM_SUM; i++)
		{
			xt_irsend_pwm_val[i] = k;
		}
		xt_irsend_pwm_val[XT_IRSEND_RAM_SUM] = 0;
		XT_IRSEND_TIM->XT_IRSEND_PWM_CCR = xt_irsend_pwm_val[0];  //特别：帮助DMA设置第一个PWM
		HAL_TIM_PWM_Start_DMA(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL, 
		/**/             (uint32_t *)(xt_irsend_pwm_val), XT_IRSEND_RAM_SUM);    //启动PWM输出
	}
	else
	{
		xt_irsend_pwm_cnt = xt_irsend_pwm_sum;
		for (i=0; i<xt_irsend_pwm_sum; i++)
		{
			xt_irsend_pwm_val[i] = k;
		}
		xt_irsend_pwm_val[xt_irsend_pwm_sum] = 0;
		XT_IRSEND_TIM->XT_IRSEND_PWM_CCR = xt_irsend_pwm_val[0];  //特别：帮助DMA设置第一个PWM
		HAL_TIM_PWM_Start_DMA(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL,
		/**/             (uint32_t *)(xt_irsend_pwm_val), xt_irsend_pwm_sum);    //启动PWM输出
	}
	//XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  发送中-发送无载波一个码（硬件）
  * @param  us         红外发送无载波的时间（单位:us）
  * @param  f          状态标志（0:第1个码,1:中途1个码,2:最后1个码）
  * @return void
  */
void xt_irsend_hw_putd_b_cb(uint32_t us, uint8_t f)
{
	//XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
	xt_irsend_pwm_sum = (us * (uint32_t)xt_irsend_pwm_khz) / 1000UL;
	if (xt_irsend_pwm_sum < 1)
	{
		xt_irsend_pwm_sum = 1;
	}
	if (xt_irsend_pwm_sum >= XT_IRSEND_RAM_SUM)
	{
		xt_irsend_pwm_cnt = XT_IRSEND_RAM_SUM;
		memset((char *)(xt_irsend_pwm_val), 0, XT_IRSEND_RAM_SUM * 2 + 2);
		XT_IRSEND_TIM->XT_IRSEND_PWM_CCR = xt_irsend_pwm_val[0];  //特别：帮助DMA设置第一个PWM
		HAL_TIM_PWM_Start_DMA(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL, 
		/**/             (uint32_t *)(xt_irsend_pwm_val), XT_IRSEND_RAM_SUM);    //启动PWM输出
	}
	else
	{
		xt_irsend_pwm_cnt = xt_irsend_pwm_sum;
		memset((char *)(xt_irsend_pwm_val), 0, xt_irsend_pwm_sum * 2 + 2);
		XT_IRSEND_TIM->XT_IRSEND_PWM_CCR = xt_irsend_pwm_val[0];  //特别：帮助DMA设置第一个PWM
		HAL_TIM_PWM_Start_DMA(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL,
		/**/             (uint32_t *)(xt_irsend_pwm_val), xt_irsend_pwm_sum);    //启动PWM输出
	}
	//XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  发送完-相关处理（硬件）
  * @param  void
  * @return void
  */
void xt_irsend_hw_putd_end_cb(void)
{
	//XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
	HAL_TIM_PWM_Stop_DMA(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL);
	//XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  DMA中断
  * @param  void
  * @return void
  */
void XT_IRSEND_DMA_IRQHandler(void)
{
	/* enter interrupt */
	XT_IRSEND_OS_INT_ENTER();
	
	HAL_DMA_IRQHandler(&xt_irsend_hdma);
	
	/* leave interrupt */
	XT_IRSEND_OS_INT_EXIT();
}

/**
  * @brief  DMA(完成)中断回调函数
  * @param  void
  * @return void
  */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	uint32_t irsend_pwm_seg;
	
	//if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_?) //HAL库中断是统一处理所有通道，如果使用多通道需要判断
	//{
		if (xt_p_irsend_pwm == 0)
		{
			HAL_TIM_PWM_Stop_DMA(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL);
		}
		else if (xt_irsend_pwm_cnt >= xt_irsend_pwm_sum)
		{
			(*(xt_p_irsend_pwm->p_dv_wave_fn))(xt_p_irsend_pwm);
		}
		else
		{	//一次DMA发不完，则分多次发
			if ((irsend_pwm_seg = xt_irsend_pwm_sum - xt_irsend_pwm_cnt) >= XT_IRSEND_RAM_SUM)
			{
				xt_irsend_pwm_cnt += XT_IRSEND_RAM_SUM;
				XT_IRSEND_TIM->XT_IRSEND_PWM_CCR = xt_irsend_pwm_val[0];  //特别：帮助DMA设置第一个PWM
				HAL_TIM_PWM_Start_DMA(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL, 
				/**/             (uint32_t *)(xt_irsend_pwm_val), XT_IRSEND_RAM_SUM);    //启动PWM输出
			}
			else
			{
				xt_irsend_pwm_cnt += irsend_pwm_seg;
				xt_irsend_pwm_val[irsend_pwm_seg] = 0;
				XT_IRSEND_TIM->XT_IRSEND_PWM_CCR = xt_irsend_pwm_val[0];  //特别：帮助DMA设置第一个PWM
				HAL_TIM_PWM_Start_DMA(&xt_irsend_htim_pwm, XT_IRSEND_PWM_TIM_CHANNEL,
				/**/             (uint32_t *)(xt_irsend_pwm_val), irsend_pwm_seg);       //启动PWM输出
			}
		}
	//}
}

#endif  //#if (XT_APP_IRSEND_EN == XT_DEF_ENABLED)
#endif
