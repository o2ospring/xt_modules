/**
  * @file  xt_scom_vofa.c
  * @brief 串口虚拟数字示波器（VOFA+ 伏特加）
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-03     o2ospring    原始版本
  * 2022-12-15     o2ospring    增加示波器发来命令的处理支持
  */
#define   XT_SCOM_VOFA_C__
#include "xt_scom_vofa.h"
#if (defined XT_APP_SCOMVOFA_EN) && (XT_APP_SCOMVOFA_EN == XT_DEF_ENABLED)
#ifdef    XT_SCOM_VOFA_X__
#undef    XT_SCOM_VOFA_H__
#include "xt_scom_vofa.h"
#endif

#ifndef XT_MSG_INIT_2_TAB_EXPORT
#define XT_MSG_INIT_2_TAB_EXPORT(func,name)
#endif
#ifndef XT_MSG_INIT_3_TAB_EXPORT
#define XT_MSG_INIT_3_TAB_EXPORT(func,name)
#endif

#define XT_SCOMVOFA_SWAP32(x) ((((uint32_t)(x)>>24)&0xFF)    \
                              |(((uint32_t)(x)>> 8)&0xFF00)  \
                              |(((uint32_t)(x)<< 8)&0xFF0000)\
                              |(((uint32_t)(x)<<24)&0xFF000000))

xt_scom_obj_t xt_scomvofa_obj;                        //串行通信服务应用对象
#if (XT_SCOMVOFA_CMD_MAX != 0)
uint8_t xt_scomvofa_cmd_buf[XT_SCOMVOFA_CMD_MAX];     //串行通信命令缓冲
uint8_t xt_scomvofa_rx_buf[XT_SCOMVOFA_CMD_MAX];      //串行通信接收缓冲
#else
uint8_t xt_scomvofa_rx_buf[1];                        //串行通信接收缓冲
#endif
uint8_t xt_scomvofa_tx_buf[4*(XT_SCOMVOFA_CH_SUM+1)]; //串行通信发送缓冲
float   xt_scomvofa_data[XT_SCOMVOFA_CH_SUM+1] = {0}; //示波器数据缓存(共4通道)

void xt_scomvofa_init(void);
void xt_scomvofa_open(void);
void xt_scomvofa_close(void);
void xt_scomvofa_open_cb(const xt_scom_obj_t *p_scom);
void xt_scomvofa_close_cb(const xt_scom_obj_t *p_scom);
void xt_scomvofa_send(void);
void xt_scomvofa_tx_complete_cb(const xt_scom_obj_t *p_scom);
int xt_scomvofa_rx_data1_cb(const xt_scom_obj_t *p_scom);
int xt_scomvofa_rx_data_cb(const xt_scom_obj_t *p_scom, uint16_t size);
int xt_scomvofa_1ch_put(uint8_t ch_n, float chx);
int xt_scomvofa_4ch_put(uint8_t ch_f, float ch1, float ch2, float ch3, float ch4);
int xt_scomvofa_xch_put(uint8_t ch_s, float *p_ch, uint8_t s_ch);
void xt_scomvofa_cmd_run(void);

/**
  * @brief  串口虚拟数字示波器初始化
  * @param  void
  * @return void
  */
void xt_scomvofa_init(void)
{
	XT_SCOMVOFA_MUTEX_INIT();
	XT_SCOMVOFA_TX_SEM_INIT();
	*((uint8_t *)(&xt_scomvofa_data[XT_SCOMVOFA_CH_SUM]) + 0) = 0x00;
	*((uint8_t *)(&xt_scomvofa_data[XT_SCOMVOFA_CH_SUM]) + 1) = 0x00;
	*((uint8_t *)(&xt_scomvofa_data[XT_SCOMVOFA_CH_SUM]) + 2) = 0x80;
	*((uint8_t *)(&xt_scomvofa_data[XT_SCOMVOFA_CH_SUM]) + 3) = 0x7F;
	#if (XT_SCOMVOFA_CMD_MAX != 0)
	xt_scomvofa_cmd_buf[0] = 0;
	#endif
	//xt_scomvofa_open();
}
XT_MSG_INIT_2_TAB_EXPORT(xt_scomvofa_init, "xt_scomvofa_init()");

/**
  * @brief  [打开]-串行通信服务
  * @param  void
  * @return void
  */
void xt_scomvofa_open(void)
{
	xt_scomvofa_obj.baud_rate        = XT_SCOMVOFA_BAUD_RATE;      //【串口属性】：通讯波特率（1200,2400,4800,9600,14400,19200,38400,57600,115200,128000,256000bps等）
	xt_scomvofa_obj.stop_bits        = 1;                          //【串口属性】：停止位（1:1位,2:2位）
	xt_scomvofa_obj.parity_bits      = 0;                          //【串口属性】：校验位（0:无,1:奇,2:偶）
	xt_scomvofa_obj.data_bits        = 8;                          //【串口属性】：数据位（8:8位）
	xt_scomvofa_obj.scom_num         = XT_SCOMVOFA_SCOM_NUM;       //【串口属性】：串行通信通道号（驱动层通道号）（＜XT_SCOM_SUM）
	
	xt_scomvofa_obj.section_code[0]  = '\r';                       //【协议属性】：〖数据流〗接收结束码（例如:<CR><LF>）
	xt_scomvofa_obj.section_code[1]  = '\n';                       //【协议属性】：〖数据流〗接收结束码（例如:<CR><LF>）
	xt_scomvofa_obj.section_mode     = 3;                          //【协议属性】：〖数据流〗接收结束码模式（0:无,1:1字节结束码,2:2字节结束码,3:两字节任一结束码）
	xt_scomvofa_obj.section_us       = 0;                          //【协议属性】：分帧重置时间（单位:us，0:无效；可用于超时分帧或超时重置接收或接收完成等）
	xt_scomvofa_obj.p_rx_buf         = xt_scomvofa_rx_buf;         //【协议属性】：接收缓冲（★绝不能为0★）
	xt_scomvofa_obj.p_tx_buf         = xt_scomvofa_tx_buf;         //【协议属性】：发送缓冲（★绝不能为0★）
	xt_scomvofa_obj.rx_buf_size      = sizeof(xt_scomvofa_rx_buf); //【协议属性】：接收缓冲大小（0:不支持接收，★及【硬件层】可强制设置为0★）
	xt_scomvofa_obj.tx_buf_size      = sizeof(xt_scomvofa_tx_buf); //【协议属性】：发送缓冲大小（0:不支持发送，★及【硬件层】可强制设置为0★）
	
	xt_scomvofa_obj.p_scom_open_fn   = xt_scomvofa_open_cb;        //【应用层服务】：串行通信服务成功打开协同操作函数（常用于设置上层应用标志）
	xt_scomvofa_obj.p_scom_close_fn  = xt_scomvofa_close_cb;       //【应用层服务】：串行通信服务成功关闭协同操作函数（常用于设置上层应用标志）
	xt_scomvofa_obj.p_tx_complete_fn = xt_scomvofa_tx_complete_cb; //【应用层服务】：中断中完成发送的通知函数        （内参传递:void）
	xt_scomvofa_obj.p_rx_data1_fn    = xt_scomvofa_rx_data1_cb;    //【应用层服务】：中断中接收到第一字节数据通知函数（内参传递:pob->p_rx_buf）
	xt_scomvofa_obj.p_rx_data_fn     = xt_scomvofa_rx_data_cb;     //【应用层服务】：中断中接收到整一组数据的传递函数（内参传递:pob->p_rx_buf, size）
	
	xt_scomvofa_obj.p_hw_open_fn     = XT_SCOMVOFA_P_HW_OPEN_FN;   //【硬件层服务】：本服务模块提供的硬件驱动程序（0:不提供）
	
	if (xt_scom_open(&xt_scomvofa_obj, XT_SCOM_TYPE_DATA_STREAM/*数据流*/) < 0)
	{
		xt_scomvofa_printf("xt_scom_open return error!\r\n");
	}
}
XT_MSG_INIT_3_TAB_EXPORT(xt_scomvofa_open, "xt_scomvofa_open()");

/**
  * @brief  [关闭]-串行通信服务
  * @param  void
  * @return void
  */
void xt_scomvofa_close(void)
{
	if (xt_scom_close(&xt_scomvofa_obj) < 0)
	{
		xt_scomvofa_printf("xt_scom_close return error!\r\n");
	}
}

/**
  * @brief  串行通信服务[打开]操作成功协同回调
  * @param  *p_scom          指向SCOM对象
  * @return void
  */
void xt_scomvofa_open_cb(const xt_scom_obj_t *p_scom)
{
	return;
}

/**
  * @brief  串行通信服务[关闭]操作成功协同回调
  * @param  *p_scom          指向SCOM对象
  * @return void
  */
void xt_scomvofa_close_cb(const xt_scom_obj_t *p_scom)
{
	return;
}

/**
  * @brief  [通知]-收到第1字节数据（在中断中回调）
  * @param  *p_scom          指向SCOM对象
  * @param  p_scom->p_rx_buf 数据缓冲
  * @return 清除此字节数据（0:清除此字节数据,其它:不清除）
  */
int xt_scomvofa_rx_data1_cb(const xt_scom_obj_t *p_scom)
{
	#if (XT_SCOMVOFA_CMD_MAX != 0)
	if ((p_scom->p_rx_buf[0] == '\r')
	||  (p_scom->p_rx_buf[0] == '\n')
	||  (p_scom->p_rx_buf[0] == ' '))
	{
		return 0;
	}
	#endif
	return 0xFFFF;
}

/**
  * @brief  [接收]-接收到一串数据（在中断中回调）
  * @param  *p_scom          指向SCOM对象
  * @param  p_scom->p_rx_buf 数据缓冲
  * @param  size             收到数据大小
  * @return 已处理数据（≥size:已处理全部数据,建议用0xFFFF表示）
  */
int xt_scomvofa_rx_data_cb(const xt_scom_obj_t *p_scom, uint16_t size)
{
	#if (XT_SCOMVOFA_CMD_MAX != 0)
	if ((xt_scomvofa_cmd_buf[0] == 0)
	&& ((p_scom->p_rx_buf[size - 1] == '\r') || (p_scom->p_rx_buf[size - 1] == '\n')))
	{
		memcpy(xt_scomvofa_cmd_buf, p_scom->p_rx_buf, size);
	}
	#endif
	return 0xFFFF;
}

/**
  * @brief  [通知]-完成发送数据（在中断中回调）
  * @param  *p_scom          指向SCOM对象
  * @return void
  */
void xt_scomvofa_tx_complete_cb(const xt_scom_obj_t *p_scom)
{
	XT_SCOMVOFA_TX_SEM_SEND();
}

/**
  * @brief  向串口虚拟示波器发送曲线数据（修改一路通道）
  * @param  ch_n       修改第几路通道编号 (低7位:0->第1通道…)(★bit7->0表示以阻塞方式发送数据★)
  * @param  chx        通道数值
  * @return ≤0:表示出错, ＞0:表示正确
  */
int xt_scomvofa_1ch_put(uint8_t ch_n, float chx)
{
	int i;
	
	i = ch_n & 0x7F;
	if (i >= XT_SCOMVOFA_CH_SUM) return 0;
	
	XT_SCOMVOFA_MUTEX_LOCKED();  //互斥上锁 >>>>>>>>>>>>
	XT_SCOMVOFA_TX_SEM_RESET();
	#if (XT_SCOMVOFA_BIG_ENDIAN_SW == 0)
	xt_scomvofa_data[i] = chx;   //小端 float 数据格式
	#else
	xt_scomvofa_data[i] = XT_SCOMVOFA_SWAP32(chx);
	#endif
	i = xt_scom_send(&xt_scomvofa_obj, (uint8_t *)xt_scomvofa_data, sizeof(xt_scomvofa_data)/*字节*/, 0/*us*/);
	if ((i > 0) && ((ch_n & 0x80) == 0)) {
	XT_SCOMVOFA_TX_SEM_TAKE(); }
	XT_SCOMVOFA_MUTEX_UNLOCK();  //互斥解锁 <<<<<<<<<<<<
	
	if (i < 0)
	{
		xt_scomvofa_printf("xt_scom_send return error!\r\n");
	}
	return (i);
}

/**
  * @brief  向串口虚拟示波器发送曲线数据（修改多路通道）
  * @param  ch_f       修改哪几路通道标志 (低7位:bit0->第1通道…)(★bit7->0表示以阻塞方式发送数据★)
  * @param  ch1        第1路通道数值
  * @param  ch2        第2路通道数值
  * @param  ch3        第3路通道数值
  * @param  ch4        第4路通道数值
  * @return ≤0:表示出错, ＞0:表示正确
  */
int xt_scomvofa_4ch_put(uint8_t ch_f, float ch1, float ch2, float ch3, float ch4)
{
	int i;
	
	if ((ch_f & 0x0F) == 0) return 0;
	
	XT_SCOMVOFA_MUTEX_LOCKED();  //互斥上锁 >>>>>>>>>>>>
	XT_SCOMVOFA_TX_SEM_RESET();
	#if (XT_SCOMVOFA_BIG_ENDIAN_SW == 0)
		#if (XT_SCOMVOFA_CH_SUM >= 1)
	if ((ch_f & 0x01) != 0) { xt_scomvofa_data[0] = ch1; } //小端 float 数据格式
		#endif
		#if (XT_SCOMVOFA_CH_SUM >= 2)
	if ((ch_f & 0x02) != 0) { xt_scomvofa_data[1] = ch2; } //小端 float 数据格式
		#endif
		#if (XT_SCOMVOFA_CH_SUM >= 3)
	if ((ch_f & 0x04) != 0) { xt_scomvofa_data[2] = ch3; } //小端 float 数据格式
		#endif
		#if (XT_SCOMVOFA_CH_SUM >= 4)
	if ((ch_f & 0x08) != 0) { xt_scomvofa_data[3] = ch4; } //小端 float 数据格式
		#endif
	#else
		#if (XT_SCOMVOFA_CH_SUM >= 1)
	if ((ch_f & 0x01) != 0) { xt_scomvofa_data[0] = XT_SCOMVOFA_SWAP32(ch1); }
		#endif
		#if (XT_SCOMVOFA_CH_SUM >= 2)
	if ((ch_f & 0x02) != 0) { xt_scomvofa_data[1] = XT_SCOMVOFA_SWAP32(ch2); }
		#endif
		#if (XT_SCOMVOFA_CH_SUM >= 3)
	if ((ch_f & 0x04) != 0) { xt_scomvofa_data[2] = XT_SCOMVOFA_SWAP32(ch3); }
		#endif
		#if (XT_SCOMVOFA_CH_SUM >= 4)
	if ((ch_f & 0x08) != 0) { xt_scomvofa_data[3] = XT_SCOMVOFA_SWAP32(ch4); }
		#endif
	#endif
	i = xt_scom_send(&xt_scomvofa_obj, (uint8_t *)xt_scomvofa_data, sizeof(xt_scomvofa_data)/*字节*/, 0/*us*/);
	if ((i > 0) && ((ch_f & 0x80) == 0)) {
	XT_SCOMVOFA_TX_SEM_TAKE(); }
	XT_SCOMVOFA_MUTEX_UNLOCK();  //互斥解锁 <<<<<<<<<<<<
	
	if (i < 0)
	{
		xt_scomvofa_printf("xt_scom_send return error!\r\n");
	}
	return (i);
}

/**
  * @brief  向串口虚拟示波器发送曲线数据（修改多路连续通道）
  * @param  ch_s       修改多少路连续通道 (低7位:1->共1通道)(★bit7->0表示以阻塞方式发送数据★)
  * @param  *p_ch      多路连续通道数值
  * @param  s_ch       修改的起始通道    (0->第1路通道)
  * @return ≤0:表示出错, ＞0:表示正确
  */
int xt_scomvofa_xch_put(uint8_t ch_s, float *p_ch, uint8_t s_ch)
{
	int i;
	uint8_t ch_sum = ch_s & 0x7F;
	
	if (ch_sum == 0) return 0;
	if (s_ch  >= XT_SCOMVOFA_CH_SUM) return -1;
	if (ch_sum > XT_SCOMVOFA_CH_SUM - s_ch) ch_sum = XT_SCOMVOFA_CH_SUM - s_ch;
	
	XT_SCOMVOFA_MUTEX_LOCKED();  //互斥上锁 >>>>>>>>>>>>
	XT_SCOMVOFA_TX_SEM_RESET();
	#if (XT_SCOMVOFA_BIG_ENDIAN_SW == 0)
	for (i=0; i<ch_sum; i++) { xt_scomvofa_data[i + s_ch] = p_ch[i]; }; //小端 float 数据格式
	#else
	for (i=0; i<ch_sum; i++) { xt_scomvofa_data[i + s_ch] = XT_SCOMVOFA_SWAP32(p_ch[i]); }
	#endif
	i = xt_scom_send(&xt_scomvofa_obj, (uint8_t *)xt_scomvofa_data, sizeof(xt_scomvofa_data)/*字节*/, 0/*us*/);
	if ((i > 0) && ((ch_s & 0x80) == 0)) {
	XT_SCOMVOFA_TX_SEM_TAKE(); }
	XT_SCOMVOFA_MUTEX_UNLOCK();  //互斥解锁 <<<<<<<<<<<<
	
	if (i < 0)
	{
		xt_scomvofa_printf("xt_scom_send return error!\r\n");
	}
	return (i);
}

/**
  * @brief  运行示波器发来的控制指令（定时循环调用即可）
  * @param  xt_scomvofa_cmd_buf[]
  * @return void
  */
void xt_scomvofa_cmd_run(void)
{
	#if (XT_SCOMVOFA_CMD_MAX != 0)
	int i;
	extern int msh_exec(char *cmd, rt_size_t length);
	
	if (xt_scomvofa_cmd_buf[0] != 0)
	{
		for (i=0; i<XT_SCOMVOFA_CMD_MAX; i++)
		{
			if ((xt_scomvofa_cmd_buf[i] == ':')
			||  (xt_scomvofa_cmd_buf[i] == ','))
			{
				xt_scomvofa_cmd_buf[i] = ' ';
			}
			else if ((xt_scomvofa_cmd_buf[i] == '\r')
			||       (xt_scomvofa_cmd_buf[i] == '\n'))
			{
				i++; break;
			}
		}
		msh_exec((char *)xt_scomvofa_cmd_buf, i);
		xt_scomvofa_cmd_buf[0] = 0;
	}
	#endif
}

#endif  //#if (XT_APP_SCOMVOFA_EN == XT_DEF_ENABLED)
