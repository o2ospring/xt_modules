/**
  * @file  xt_wiznet_vofa.c
  * @brief UDP网络虚拟数字示波器（VOFA+ 伏特加）
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-16     o2ospring    原始版本
  */
#define   XT_WIZNET_VOFA_C__
#include "xt_wiznet_vofa.h"
#if (defined XT_APP_WIZNETVOFA_EN) && (XT_APP_WIZNETVOFA_EN == XT_DEF_ENABLED)
#ifdef    XT_WIZNET_VOFA_X__
#undef    XT_WIZNET_VOFA_H__
#include "xt_wiznet_vofa.h"
#endif

#ifndef XT_MSG_INIT_2_TAB_EXPORT
#define XT_MSG_INIT_2_TAB_EXPORT(func,name)
#endif
#ifndef XT_APP_INIT_2_TAB_EXPORT
#define XT_APP_INIT_2_TAB_EXPORT(func,name)
#endif
#ifndef FINSH_FUNCTION_EXPORT
#define FINSH_FUNCTION_EXPORT(func,txt)
#endif
#ifndef MSH_CMD_EXPORT
#define MSH_CMD_EXPORT(func,txt)
#endif

#define XT_WIZVOFA_SWAP32(x) ((((uint32_t)(x)>>24)&0xFF)    \
                             |(((uint32_t)(x)>> 8)&0xFF00)  \
                             |(((uint32_t)(x)<< 8)&0xFF0000)\
                             |(((uint32_t)(x)<<24)&0xFF000000))

uint8_t xt_wizvofa_open_f = 0;                         //示波器打开标志
uint8_t xt_wizvofa_run_f  = 0;                         //示波器运行状态
#if (XT_WIZVOFA_CMD_MAX != 0)
uint8_t xt_wizvofa_cmd_buf[XT_WIZVOFA_CMD_MAX+1];      //示波器命令缓冲
#else
uint8_t xt_wizvofa_cmd_buf[10];
#endif
float   xt_wizvofa_snd_dat[XT_WIZVOFA_CH_SUM+1] = {0}; //示波器数据缓存
XT_WIZVOFA_OS_TICK_T xt_wizvofa_refresh_cn;            //示波器刷新时间（节拍计时器）
XT_WIZVOFA_OS_TICK_T xt_wizvofa_refresh_tk;            //示波器刷新时间（节拍计时值）

void xt_wizvofa_init(void);
void xt_wizvofa_open(void);
void xt_wizvofa_cycl(XT_WIZVOFA_OS_TICK_T tk);

int xt_wizvofa_1ch_put(uint8_t ch_n, float chx);
int xt_wizvofa_4ch_put(uint8_t ch_f, float ch1, float ch2, float ch3, float ch4);
int xt_wizvofa_xch_put(uint8_t ch_s, float *p_ch, uint8_t s_ch);
void xt_wizvofa_cmd_run(void);

void xt_wizvofa_ip_set(int argc, char *argv[]);
void xt_wizvofa_cycl_set(int argc, char *argv[]);
void xt_wizvofa_help(void);

void xt_wizvofa_thread_entry(void *p_arg);

/**
  * @brief  网络虚拟数字示波器初始化
  * @param  void
  * @return void
  */
void xt_wizvofa_init(void)
{
	*((uint8_t *)(&xt_wizvofa_snd_dat[XT_WIZVOFA_CH_SUM]) + 0) = 0x00;
	*((uint8_t *)(&xt_wizvofa_snd_dat[XT_WIZVOFA_CH_SUM]) + 1) = 0x00;
	*((uint8_t *)(&xt_wizvofa_snd_dat[XT_WIZVOFA_CH_SUM]) + 2) = 0x80;
	*((uint8_t *)(&xt_wizvofa_snd_dat[XT_WIZVOFA_CH_SUM]) + 3) = 0x7F;
	xt_wizvofa_refresh_tk = XT_WIZVOFA_REFRESH_TK - 1;
	xt_wizvofa_cmd_buf[0] = 0;
}
XT_MSG_INIT_2_TAB_EXPORT(xt_wizvofa_init, "xt_wizvofa_init()");

/**
  * @brief  打开网络虚拟数字示波器
  * @param  void
  * @return void
  */
#if (XT_WIZVOFA_POWERON_EN != 0)
void xt_wizvofa_open(void)
{
	XT_WIZVOFA_OS_THREAD_T p = 0;
	XT_WIZVOFA_THREAD_CREATE(p);
	if (p != 0)
	{
		xt_wizvofa_open_f = 1;
	}
}
XT_APP_INIT_2_TAB_EXPORT(xt_wizvofa_open, "xt_wizvofa_open()");
#endif

/**
  * @brief  设置示波器刷新时间（同时启动线程工作）
  * @param  tk         设置示波器刷新时间（@系统节拍）
  * @return void
  */
void xt_wizvofa_cycl(XT_WIZVOFA_OS_TICK_T tk)
{
	XT_WIZVOFA_VARIAB()
	
	//暂停示波器刷新显示
	if (tk == 0)
	{
		XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
		if (xt_wizvofa_open_f != 0)
		{
			xt_wizvofa_open_f = 2; ///
			if (xt_wizvofa_run_f == 1)
			{
				xt_wizvofa_run_f = 2;
				xt_socket_ot_set(XT_WIZVOFA_SOCKET, 0, 0);
			}
			xt_wizvofa_refresh_tk = 1000;
		}
		XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
	}
	//创建工作线程并设置示波器刷新时间
	else if (xt_wizvofa_open_f == 0)
	{
		XT_WIZVOFA_OS_THREAD_T p = 0;
		XT_WIZVOFA_THREAD_CREATE(p);
		if (p != 0)
		{
			XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
			xt_wizvofa_open_f = 1;
			xt_wizvofa_run_f  = 0;
			xt_wizvofa_refresh_tk = tk - 1;
			xt_wizvofa_refresh_cn = XT_WIZVOFA_OS_TICK_GET() - 1;
			XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
		}
		else
		{
			xt_wizvofa_printf("thread create error!\r\n");
		}
	}
	//启动刷新显示并修改示波器刷新时间
	else
	{
		XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
		xt_wizvofa_open_f = 1;
		xt_wizvofa_refresh_tk = tk - 1;
		xt_wizvofa_refresh_cn = XT_WIZVOFA_OS_TICK_GET() - 1;
		if (xt_wizvofa_run_f == 1)
		{
			xt_wizvofa_run_f = 2;
			xt_socket_ot_set(XT_WIZVOFA_SOCKET, 0, 0);
		}
		XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
	}
}

/**
  * @brief  修改虚拟示波器发送曲线数据（修改一路通道）
  * @param  ch_n       修改第几路通道编号 (低7位:0->第1通道…)(★bit7->0表示以阻塞方式发送数据★)
  * @param  chx        通道数值
  * @return ≤0:表示出错, ＞0:表示正确
  */
int xt_wizvofa_1ch_put(uint8_t ch_n, float chx)
{
	XT_WIZVOFA_VARIAB()
	int i;
	
	i = ch_n & 0x7F;
	if (i >= XT_WIZVOFA_CH_SUM) return -1;
	
	XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
	#if (XT_WIZVOFA_BIG_ENDIAN_SW == 0)
	xt_wizvofa_snd_dat[i] = chx;   //小端 float 数据格式
	#else
	xt_wizvofa_snd_dat[i] = XT_WIZVOFA_SWAP32(chx);
	#endif
	XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
	
	if (xt_wizvofa_open_f == 0) return 0;
	return (1);
}

/**
  * @brief  修改虚拟示波器发送曲线数据（修改多路通道）
  * @param  ch_f       修改哪几路通道标志 (低7位:bit0->第1通道…)(★bit7->0表示以阻塞方式发送数据★)
  * @param  ch1        第1路通道数值
  * @param  ch2        第2路通道数值
  * @param  ch3        第3路通道数值
  * @param  ch4        第4路通道数值
  * @return ≤0:表示出错, ＞0:表示正确
  */
int xt_wizvofa_4ch_put(uint8_t ch_f, float ch1, float ch2, float ch3, float ch4)
{
	XT_WIZVOFA_VARIAB()
	
	if ((ch_f & 0x0F) == 0)     return -1;
	
	XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
	#if (XT_WIZVOFA_BIG_ENDIAN_SW == 0)
		#if (XT_WIZVOFA_CH_SUM >= 1)
	if ((ch_f & 0x01) != 0) { xt_wizvofa_snd_dat[0] = ch1; } //小端 float 数据格式
		#endif
		#if (XT_WIZVOFA_CH_SUM >= 2)
	if ((ch_f & 0x02) != 0) { xt_wizvofa_snd_dat[1] = ch2; } //小端 float 数据格式
		#endif
		#if (XT_WIZVOFA_CH_SUM >= 3)
	if ((ch_f & 0x04) != 0) { xt_wizvofa_snd_dat[2] = ch3; } //小端 float 数据格式
		#endif
		#if (XT_WIZVOFA_CH_SUM >= 4)
	if ((ch_f & 0x08) != 0) { xt_wizvofa_snd_dat[3] = ch4; } //小端 float 数据格式
		#endif
	#else
		#if (XT_WIZVOFA_CH_SUM >= 1)
	if ((ch_f & 0x01) != 0) { xt_wizvofa_snd_dat[0] = XT_WIZVOFA_SWAP32(ch1); }
		#endif
		#if (XT_WIZVOFA_CH_SUM >= 2)
	if ((ch_f & 0x02) != 0) { xt_wizvofa_snd_dat[1] = XT_WIZVOFA_SWAP32(ch2); }
		#endif
		#if (XT_WIZVOFA_CH_SUM >= 3)
	if ((ch_f & 0x04) != 0) { xt_wizvofa_snd_dat[2] = XT_WIZVOFA_SWAP32(ch3); }
		#endif
		#if (XT_WIZVOFA_CH_SUM >= 4)
	if ((ch_f & 0x08) != 0) { xt_wizvofa_snd_dat[3] = XT_WIZVOFA_SWAP32(ch4); }
		#endif
	#endif
	XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
	
	if (xt_wizvofa_open_f == 0) return 0;
	return (1);
}

/**
  * @brief  修改虚拟示波器发送曲线数据（修改多路连续通道）
  * @param  ch_s       修改多少路连续通道(低7位:1->共1通道)
  * @param  *p_ch      多路连续通道数值
  * @param  s_ch       修改的起始通道    (0->第1路通道)
  * @return ≤0:表示出错, ＞0:表示正确
  */
int xt_wizvofa_xch_put(uint8_t ch_s, float *p_ch, uint8_t s_ch)
{
	XT_WIZVOFA_VARIAB()
	int i;
	uint8_t ch_sum = ch_s & 0x7F;
	
	if (ch_sum == 0) return -1;
	if (s_ch  >= XT_WIZVOFA_CH_SUM) return -1;
	if (ch_sum > XT_WIZVOFA_CH_SUM - s_ch) ch_sum = XT_WIZVOFA_CH_SUM - s_ch;
	
	XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
	#if (XT_WIZVOFA_BIG_ENDIAN_SW == 0)
	for (i=0; i<ch_sum; i++) { xt_wizvofa_snd_dat[i + s_ch] = p_ch[i]; }; //小端 float 数据格式
	#else
	for (i=0; i<ch_sum; i++) { xt_wizvofa_snd_dat[i + s_ch] = XT_WIZVOFA_SWAP32(p_ch[i]); }
	#endif
	XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
	
	if (xt_wizvofa_open_f == 0) return 0;
	return (1);
}

/**
  * @brief  运行示波器发来的控制指令
  * @param  xt_wizvofa_cmd_buf[]
  * @return void
  */
void xt_wizvofa_cmd_run(void)
{
	#if (XT_WIZVOFA_CMD_MAX != 0)
	int i;
	extern int msh_exec(char *cmd, rt_size_t length);
	
	if (xt_wizvofa_cmd_buf[0] != 0)
	{
		for (i=0; i<XT_WIZVOFA_CMD_MAX; i++)
		{
			if ((xt_wizvofa_cmd_buf[i] == ':')
			||  (xt_wizvofa_cmd_buf[i] == ','))
			{
				xt_wizvofa_cmd_buf[i] = ' ';
			}
			else if ((xt_wizvofa_cmd_buf[i] == '\r')
			||       (xt_wizvofa_cmd_buf[i] == '\n'))
			{
				i++; break;
			}
		}
		msh_exec((char *)xt_wizvofa_cmd_buf, i);
		xt_wizvofa_cmd_buf[0] = 0;
	}
	#endif
}

#if (XT_WIZVOFA_CMD_MAX != 0)
/**
  * @brief  设置示波器服务端的IP
  * @param  argc       入口参数数量
  * @param  argv[1]    设置IP（如：192）
  * @param  argv[2]    设置IP（如：168）
  * @param  argv[3]    设置IP（如： 2 ）
  * @param  argv[4]    设置IP（如：221）
  * @return void
  */
void xt_wizvofa_ip_set(int argc, char *argv[])
{
	XT_WIZVOFA_VARIAB()
	uint32_t n;
	uint8_t ip[4];
	
	if (argc < 5)
	{
		rt_kprintf("xt_wizvofa_ip_set input argc error!\r\n");
	}
	if ((n = atoi(argv[1])) > 255) { goto xt_wizvofa_ip_set_error; }
	ip[0] = (uint8_t)(n);
	if ((n = atoi(argv[2])) > 255) { goto xt_wizvofa_ip_set_error; }
	ip[1] = (uint8_t)(n);
	if ((n = atoi(argv[3])) > 255) { goto xt_wizvofa_ip_set_error; }
	ip[2] = (uint8_t)(n);
	if ((n = atoi(argv[4])) > 255) { goto xt_wizvofa_ip_set_error; }
	ip[3] = (uint8_t)(n);
	if (xt_net_chk_ip(ip) != 1)
	{
		xt_wizvofa_ip_set_error:
		rt_kprintf("xt_wizvofa_ip_set input ip error!\r\n");
		return;
	}
	XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
	memcpy(xt_wizvofa_ip, ip, 4);
	XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
}
FINSH_FUNCTION_EXPORT(xt_wizvofa_ip_set, xt_wizvofa_ip_set);
MSH_CMD_EXPORT(xt_wizvofa_ip_set, xt_wizvofa_ip_set);

/**
  * @brief  设置示波器刷新时间（同时启动线程工作）
  * @param  argc       入口参数数量
  * @param  argv[1]    设置示波器刷新时间（@系统节拍）
  * @return void
  */
void xt_wizvofa_cycl_set(int argc, char *argv[])
{
	if (argc < 2)
	{
		rt_kprintf("xt_wizvofa_cycl_set input argc error!\r\n");
	}
	xt_wizvofa_cycl((XT_WIZVOFA_OS_TICK_T)(atoi(argv[1])));
}
FINSH_FUNCTION_EXPORT(xt_wizvofa_cycl_set, xt_wizvofa_cycl_set);
MSH_CMD_EXPORT(xt_wizvofa_cycl_set, xt_wizvofa_cycl_set);

/**
  * @brief  设置示波器帮助
  * @param  void
  * @return void
  */
void xt_wizvofa_help(void)
{
	rt_kprintf("xt_wizvofa_ip_set 192 168 3 221 \r\n");
	rt_kprintf("xt_wizvofa_cycl_set 10 \r\n");
}
FINSH_FUNCTION_EXPORT(xt_wizvofa_help, xt_wizvofa_help);
MSH_CMD_EXPORT(xt_wizvofa_help, xt_wizvofa_help);
#endif

/**
  * @brief  示波器服务线程
  * @param  void
  * @return void
  */
void xt_wizvofa_thread_entry(void *p_arg)
{
	XT_WIZVOFA_VARIAB()
	XT_WIZVOFA_OS_TICK_T  tk;
	uint16_t    wkver, lkver;
	uint16_t    len, port;    //端口
	uint8_t     ip[4];        //IP地址
	
	xt_wizvofa_refresh_cn = XT_WIZVOFA_OS_TICK_GET() - 1;
	xt_wizvofa_socket:
	while (((wkver = xt_net_get_rstver()) & 0x8000) == 0)           //等待网络初始化完成
	{
		XT_WIZVOFA_OS_DLY_TICK(1);
	}
	xt_wizvofa_link:
	while (((lkver = xt_net_get_lnkver()) & 0x8000) == 0)           //等待插上网线
	{
		XT_WIZVOFA_OS_DLY_TICK(1);
	}
	xt_wizvofa_build:
	socket(XT_WIZVOFA_SOCKET, Sn_MR_UDP, XT_WIZVOFA_SOCKET_NUM, 0); //打开socket（UDP协议）
	while (1)
	{                                                               //如果socket异常,则...
		if (xt_socket_chk(XT_WIZVOFA_SOCKET, Sn_MR_UDP, 0) == 0) goto xt_wizvofa_build; 
		if (wkver != xt_net_get_rstver()) goto xt_wizvofa_socket;   //如果网络重启了,则...
		if (lkver != xt_net_get_lnkver()) goto xt_wizvofa_link;     //如果网线拔掉了,则...
		
		//接收数据
		XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
		if ((tk = xt_wizvofa_refresh_cn - XT_WIZVOFA_OS_TICK_GET()) >= XT_WIZVOFA_OS_TICK_OVMAX)
		{
			xt_wizvofa_refresh_cn = xt_wizvofa_refresh_tk + XT_WIZVOFA_OS_TICK_GET();
			xt_wizvofa_run_f = 0;
			xt_socket_ot_set(XT_WIZVOFA_SOCKET, 1/*一次*/, 0x10);   //修改本次socket超时操作是次数
		}
		else
		{	/* 接收等待无法确保准确延时，所以屏蔽本功能!!!!!!!!!!!!!!!
			xt_wizvofa_run_f = 1;
			xt_socket_ot_set(XT_WIZVOFA_SOCKET, tk, 0);             //修改本次socket超时操作是时间
			*/
			xt_wizvofa_run_f = 0;
			xt_socket_ot_set(XT_WIZVOFA_SOCKET, 1/*一次*/, 0x10);   //修改本次socket超时操作是次数
		}
		XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
		len = xt_socket_recv_chk(XT_WIZVOFA_SOCKET);                //检测socket是否收到数据
		XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
		if (xt_wizvofa_run_f >= 2)
		{
			xt_socket_ot_rst(XT_WIZVOFA_SOCKET);
		}
		xt_wizvofa_run_f = 0;
		XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
		
		#if (XT_WIZVOFA_CMD_MAX != 0)
		len = len > XT_WIZVOFA_CMD_MAX ? XT_WIZVOFA_CMD_MAX : len;  //处理数据大小与缓冲大小关系
		#else
		len = sizeof(xt_wizvofa_cmd_buf) - 1;
		#endif
		if (len > 0)
		{                                                           //socket接收数据（同时得到对端的IP及端口）
			len = recvfrom(XT_WIZVOFA_SOCKET, xt_wizvofa_cmd_buf, len, ip, &port);
			xt_wizvofa_cmd_buf[len] = '\n';
			xt_wizvofa_cmd_run();
		}
		
		/* 接收等待无法确保准确延时，所以固定延时操作!!!!!!!!!!!!!!!
		*/
		tk = xt_wizvofa_refresh_tk + 1;
		XT_WIZVOFA_OS_DLY_TICK((tk <= XT_WIZVOFA_OS_TICK_LIMIT) ? tk : 1);
		
		//发送数据
		XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
		if ((xt_wizvofa_refresh_cn - XT_WIZVOFA_OS_TICK_GET()) >= XT_WIZVOFA_OS_TICK_OVMAX)
		{
			xt_wizvofa_refresh_cn = xt_wizvofa_refresh_tk + XT_WIZVOFA_OS_TICK_GET();
			
			if (xt_wizvofa_open_f == 1)
			{
				memcpy(ip, xt_wizvofa_ip, 4);
		XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
				
				len = sizeof(xt_wizvofa_snd_dat);                   //socket发送数据
				if (sendto(XT_WIZVOFA_SOCKET, (uint8_t *)xt_wizvofa_snd_dat, len, ip, XT_WIZVOFA_SOCKET_NUM) != len)
				{
					xt_wizvofa_printf("sendto return error!\r\n");
				}
			}
			else
			{
		XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
			}
		}
		else
		{
		XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
		}
	}
}

#endif  //#if (XT_APP_WIZNETVOFA_EN == XT_DEF_ENABLED)
