/**
  * @file  xt_wiznet.c
  * @brief 网络服务模块（目前只针对 WIZnet Hardwired TCP/IP Chips (WIZCHIP)）
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-10     o2ospring    原始版本
  */
#define   XT_WIZNET_C__
#include "xt_wiznet.h"
#if (defined XT_APP_WIZNET_EN) && (XT_APP_WIZNET_EN == XT_DEF_ENABLED)
#ifdef    XT_WIZNET_X__
#undef    XT_WIZNET_H__
#include "xt_wiznet.h"
#endif

#ifndef XT_HARD_INIT_2_TAB_EXPORT
#define XT_HARD_INIT_2_TAB_EXPORT(func,name)
#endif
#ifndef XT_DEV_INIT_2_TAB_EXPORT
#define XT_DEV_INIT_2_TAB_EXPORT(func,name)
#endif
#ifndef XT_MSG_INIT_2_TAB_EXPORT
#define XT_MSG_INIT_2_TAB_EXPORT(func,name)
#endif
#ifndef XT_APP_INIT_2_TAB_EXPORT
#define XT_APP_INIT_2_TAB_EXPORT(func,name)
#endif

xt_net_t xt_netcfg;

uint16_t xt_net_get_rstver(void);
uint16_t xt_net_get_lnkver(void);
uint8_t xt_net_get_dhcp(void);
uint8_t xt_net_get_mac(uint8_t *mac);
uint8_t xt_net_get_ip(uint8_t *ip);
uint8_t xt_net_get_gw(uint8_t *gw);
uint8_t xt_net_get_sn(uint8_t *sn);
uint8_t xt_net_get_dns(uint8_t *dns);
uint8_t xt_net_get_ipgwsn(uint8_t *ip, uint8_t *gw, uint8_t *sn);
uint8_t xt_net_chk_ip(uint8_t *ip);
void xt_net_set_restart(void);
void xt_net_set_dhcp(void);
void xt_net_set_ipgwsn(const uint8_t *ip, const uint8_t *gw, const uint8_t *sn);
void xt_net_set_mac(const uint8_t *mac);
void xt_net_set_dns(const uint8_t *dns);
void xt_net_dhcp_reset(void);
void xt_net_dhcp_ipget(uint8_t *ip, uint8_t *gw, uint8_t *sn);
uint8_t xt_net_chk_socket_reset(void);
void xt_net_rst(void);
void xt_net_run(void);

/**
  * @brief  获取:复位版本（bit15只作为标志位,=0表示准备或复位中,=1完成复位）
  * @param  void
  * @return 版本号（当上层发现版本不一致时，表明网络将要或已复位）
  */
uint16_t xt_net_get_rstver(void)
{
	uint16_t tmp;
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	tmp = xt_netcfg.rstver;
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	return (tmp);
}

/**
  * @brief  获取:LINK版本（bit15只作为标志位,=0网线断开或连接中,=1网线连接）
  * @param  void
  * @return 版本号（当上层发现版本不一致时，表明网线有插拔过）
  */
uint16_t xt_net_get_lnkver(void)
{
	uint16_t tmp;
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	tmp = xt_netcfg.lnkver;
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	return (tmp);
}

/**
  * @brief  获取:DHCP是否开启
  * @param  void
  * @return 0->手动设置IP，否则->DHCP
  */
uint8_t xt_net_get_dhcp(void)
{
	uint8_t tmp;
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	tmp = xt_netcfg.dhcp;
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	return (tmp);
}

/**
  * @brief  获取:MAC地址
  * @param  *mac     6字节MAC指针
  * @return 目前固定返回1
  */
uint8_t xt_net_get_mac(uint8_t *mac)
{
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	memcpy(mac, xt_netcfg.mac, sizeof(xt_netcfg.mac));
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	return (1);
}

/**
  * @brief  获取:IP地址
  * @param  *ip      4字节IP指针
  * @return 0->失败，否则->成功
  */
uint8_t xt_net_get_ip(uint8_t *ip)
{
	uint8_t tmp;
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	memcpy(ip, xt_netcfg.ip, sizeof(xt_netcfg.ip));
	if (xt_netcfg.dhcp != 0)
	{
		tmp = ((xt_netcfg.rstver & 0x8000) != 0) ? 1 : 0;
	}
	else
	{
		tmp = 1;
	}
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	return (tmp);
}

/**
  * @brief  获取:网关地址
  * @param  *gw      4字节网关指针
  * @return 0->失败，否则->成功
  */
uint8_t xt_net_get_gw(uint8_t *gw)
{
	uint8_t tmp;
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	memcpy(gw, xt_netcfg.gw, sizeof(xt_netcfg.gw));
	if (xt_netcfg.dhcp != 0)
	{
		tmp = ((xt_netcfg.rstver & 0x8000) != 0) ? 1 : 0;
	}
	else
	{
		tmp = 1;
	}
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	return (tmp);
}

/**
  * @brief  获取:子网掩码
  * @param  *sn      4字节子网掩码指针
  * @return 0->失败，否则->成功
  */
uint8_t xt_net_get_sn(uint8_t *sn)
{
	uint8_t tmp;
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	memcpy(sn, xt_netcfg.sn, sizeof(xt_netcfg.sn));
	if (xt_netcfg.dhcp != 0)
	{
		tmp = ((xt_netcfg.rstver & 0x8000) != 0) ? 1 : 0;
	}
	else
	{
		tmp = 1;
	}
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	return (tmp);
}

/**
  * @brief  获取:DNS地址
  * @param  *dns     4字节DNS指针
  * @return 目前固定返回1
  */
uint8_t xt_net_get_dns(uint8_t *dns)
{
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	memcpy(dns, xt_netcfg.dns, sizeof(xt_netcfg.dns));
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	return (1);
}

/**
  * @brief  获取:网络地址
  * @param  *ip      4字节IP指针
  * @param  *gw      4字节网关指针
  * @param  *sn      4字节子网掩码指针
  * @return 0->失败，否则->成功
  */
uint8_t xt_net_get_ipgwsn(uint8_t *ip, uint8_t *gw, uint8_t *sn)
{
	uint8_t tmp;
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	memcpy(ip, xt_netcfg.ip, sizeof(xt_netcfg.ip));
	memcpy(gw, xt_netcfg.gw, sizeof(xt_netcfg.gw));
	memcpy(sn, xt_netcfg.sn, sizeof(xt_netcfg.sn));
	if (xt_netcfg.dhcp != 0)
	{
		tmp = ((xt_netcfg.rstver & 0x8000) != 0) ? 1 : 0;
	}
	else
	{
		tmp = 1;
	}
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	return (tmp);
}

/**
  * @brief  检测:IP地址是否为同一网段
  * @param  *ip      4字节IP指针
  * @return 0->不相同，1->相同, ≥2->非局域网IP
  */
uint8_t xt_net_chk_ip(uint8_t *ip)
{
	XT_WIZ_CPOINT_VARIAB()
	uint32_t ipdat;
	#define XT_NET_SW_UC_TO_UL(d4,d3,d2,d1)  ((((uint32_t)(d4) & 0xFF) << 24)\
	                                         |(((uint32_t)(d3) & 0xFF) << 16)\
	                                         |(((uint32_t)(d2) & 0xFF) <<  8)\
	                                         |(((uint32_t)(d1) & 0xFF)      ))
	ipdat = XT_NET_SW_UC_TO_UL(ip[0], ip[1], ip[2], ip[3]);
	if      ((ipdat >= 0x0A000000) && (ipdat <= 0x0AFFFFFF)) 
	{	//A类: 10.0.0.0-10.255.255.255      A类: 网络---主机---主机---主机
		XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
		if (0 != memcmp(ip, xt_netcfg.ip, 1))
		{
			ipdat = 0;
		}
		else
		{
			ipdat = 1;
		}
		XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	}
	else if ((ipdat >= 0xAC100000) && (ipdat <= 0xAC1FFFFF))
	{	//B类: 172.16.0.0-172.31.255.255    B类: 网络---网络---主机---主机
		XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
		if (0 != memcmp(ip, xt_netcfg.ip, 2))
		{
			ipdat = 0;
		}
		else
		{
			ipdat = 1;
		}
		XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	}
	else if ((ipdat >= 0xC0A80000) && (ipdat <= 0xC0A8FFFF))
	{	//C类: 192.168.0.0-192.168.255.255  C类: 网络---网络---网络---主机
		XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
		if (0 != memcmp(ip, xt_netcfg.ip, 3))
		{
			ipdat = 0;
		}
		else
		{
			ipdat = 1;
		}
		XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	}
	else
	{
		ipdat = 2; //非局域网IP!!!
	}
	return ((uint8_t)(ipdat));
}

/**
  * @brief  操作:复位网络
  * @param  void
  * @return void
  */
void xt_net_set_restart(void)
{
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	xt_netcfg.newrst = 1;
	xt_netcfg.rstver &= 0x7FFF;
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  操作:使能自动设置IP（关闭手动设置IP）
  * @param  void
  * @return void
  */
void xt_net_set_dhcp(void)
{
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	if (xt_netcfg.dhcp == 0)
	{
		xt_netcfg.dhcp = 1;
		xt_net_set_restart();
	}
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  操作:使能手动设置IP（关闭自动设置IP）
  * @param  *ip      4字节IP指针
  * @param  *gw      4字节网关指针
  * @param  *sn      4字节子网掩码指针
  * @return void
  */
void xt_net_set_ipgwsn(const uint8_t *ip, const uint8_t *gw, const uint8_t *sn)
{
	XT_WIZ_CPOINT_VARIAB()
	if (((ip[0] == 255)
	   &&(ip[1] == 255)
	   &&(ip[2] == 255)
	   &&(ip[3] == 255))
	||  ((gw[0] == 255)
	   &&(gw[1] == 255)
	   &&(gw[2] == 255)
	   &&(gw[3] == 255))
	||  ((sn[0] == 255)
	   &&(sn[1] == 255)
	   &&(sn[2] == 255)
	   &&(sn[3] == 255)))
	{
		return;
	}
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	if ((xt_netcfg.dhcp != 0)
	||  (memcmp(xt_netcfg.ip, ip, sizeof(xt_netcfg.ip)) != 0)
	||  (memcmp(xt_netcfg.gw, gw, sizeof(xt_netcfg.gw)) != 0)
	||  (memcmp(xt_netcfg.sn, sn, sizeof(xt_netcfg.sn)) != 0))
	{
		xt_netcfg.dhcp = 0;
		memcpy(xt_netcfg.ip, ip, sizeof(xt_netcfg.ip));
		memcpy(xt_netcfg.gw, gw, sizeof(xt_netcfg.gw));
		memcpy(xt_netcfg.sn, sn, sizeof(xt_netcfg.sn));
		xt_net_set_restart();
	}
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  操作:设置MAC地址
  * @param  *mac     6字节MAC指针
  * @return void
  */
void xt_net_set_mac(const uint8_t *mac)
{
	uint8_t mmac[6];
	XT_WIZ_CPOINT_VARIAB()
	memcpy(mmac, mac, sizeof(mmac));
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	mmac[0] &= 0xFE;
	if (memcmp(xt_netcfg.mac, mmac, sizeof(xt_netcfg.mac)) != 0)
	{
		memcpy(xt_netcfg.mac, mmac, sizeof(xt_netcfg.mac));
		xt_net_set_restart();
	}
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  操作:设置DNS地址（应用程序才使用，网络服务只负责缓存）
  * @param  *dns     4字节DNS指针
  * @return void
  */
void xt_net_set_dns(const uint8_t *dns)
{
	XT_WIZ_CPOINT_VARIAB()
	if ((dns[0] == 255)
	&&  (dns[1] == 255)
	&&  (dns[2] == 255)
	&&  (dns[3] == 255))
	{
		return;
	}
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	if (memcmp(xt_netcfg.dns, dns, sizeof(xt_netcfg.dns)) != 0)
	{
		memcpy(xt_netcfg.dns, dns, sizeof(xt_netcfg.dns));
	}
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  通知:DHCP即将要重启
  * @param  void
  * @return void
  */
void xt_net_dhcp_reset(void)
{
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
//	xt_netcfg.newrst = 原值;
	xt_netcfg.rstver &= 0x7FFF;
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  通知:DHCP获取到IP（需要本模块来判断是否有更新）
  * @param  *ip      4字节IP指针
  * @param  *gw      4字节网关指针
  * @param  *sn      4字节子网掩码指针
  * @return void
  */
void xt_net_dhcp_ipget(uint8_t *ip, uint8_t *gw, uint8_t *sn)
{
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	if (xt_netcfg.dhcp != 0)
	{
		if (xt_netcfg.newrst == 0)
		{
			if ((xt_netcfg.rstver & 0x8000) == 0)
			{
				xt_netcfg.rstver = ((xt_netcfg.rstver + 1) & 0x7FFF) | 0x8000;
				
				memcpy(xt_netcfg.ip, ip, sizeof(xt_netcfg.ip));
				memcpy(xt_netcfg.gw, gw, sizeof(xt_netcfg.gw));
				memcpy(xt_netcfg.sn, sn, sizeof(xt_netcfg.sn));
			}
			else if ((memcmp(xt_netcfg.ip, ip, sizeof(xt_netcfg.ip)) != 0)
			||       (memcmp(xt_netcfg.gw, gw, sizeof(xt_netcfg.gw)) != 0)
			||       (memcmp(xt_netcfg.sn, sn, sizeof(xt_netcfg.sn)) != 0))
			{
				xt_net_set_restart();
			}
		}
	}
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  检测:socket是否被强制复位
  * @param  void
  * @return 0->不退出，否则->强制复位
  */
uint8_t xt_net_chk_socket_reset(void)
{
	uint8_t tmp;
	XT_WIZ_CPOINT_VARIAB()
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	tmp = ((xt_netcfg.rstver & 0x8000) == 0) ? 1 : 0;
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	return (tmp);
}

/**
  * @brief  网络芯片控制脚初始化
  * @param  void
  * @return void
  */
void xt_net_pin_init(void)
{
	memset((unsigned char *)(&xt_netcfg), 0, sizeof(xt_netcfg));
	xt_netcfg.newrst = 1;
	xt_wiz_pin_init();
	XT_NET_INIT1();
}
#ifndef XT_NET_PIN_INIT
XT_HARD_INIT_2_TAB_EXPORT(xt_net_pin_init, "xt_net_pin_init()");
#endif

/**
  * @brief  网络参数初始化
  * @param  void
  * @return void
  */
void xt_net_cfg_init(void)
{
	XT_NET_INIT2();
}
#ifndef XT_NET_CFG_INIT
XT_DEV_INIT_2_TAB_EXPORT(xt_net_cfg_init, "xt_net_cfg_init()");
#endif

/**
  * @brief  系统IPC初始化
  * @param  void
  * @return void
  */
void xt_net_ipc_init(void)
{
	xt_wiz_ipc_init();
}
XT_MSG_INIT_2_TAB_EXPORT(xt_net_ipc_init, "xt_net_ipc_init()");

/**
  * @brief  系统线程初始化
  * @param  void
  * @return void
  */
void xt_net_thread_init(void)
{
	xt_wiz_thread_init();
}
XT_APP_INIT_2_TAB_EXPORT(xt_net_thread_init, "xt_net_thread_init()");

/**
  * @brief  重启网络
  * @param  void
  * @return void
  */
void xt_net_rst(void)
{
	XT_WIZ_CPOINT_VARIAB()
	uint32_t f;
	extern const uint8_t xt_wiz_tx_size_tab[];
	extern const uint8_t xt_wiz_rx_size_tab[];
	
	xt_net_dhcp_reset();
	XT_WIZ_IC_MUTEX_EN(f);    //>>>>>>>>>>>>>>>>>>>
	
	xt_wiz_reset();                    //WIZnet网络芯片复位  （硬复位）
	xt_wiz_init();                     //WIZnet网络芯片初始化（软复位）
	XT_WIZ_OS_10MS(2);                 //延时20mS
	
	//自动分配IP初始化
	if (xt_netcfg.dhcp != 0)
	{
		#ifdef XT_NET_PRINTF
		XT_NET_PRINTF("dhcp init...\r\n");
		#endif
		xt_wiz_dhcp_init();            //DHCP初始化
		
		xt_wiz_set_retx_time(1000);    //设置重发时间（单位：100us）
		xt_wiz_set_retx_cnt(2);        //设置最大重新发送次数
		
		//由 xt_net_dhcp_ipget() 回调实现更新版本号
	}
	//手动分配IP初始化
	else
	{
		#ifdef XT_NET_PRINTF
		XT_NET_PRINTF("init...\r\n");
		#endif
		xt_wiz_set_mac(xt_netcfg.mac); //设置MAC地址
		xt_wiz_set_ip(xt_netcfg.ip);   //设置IP地址
		xt_wiz_set_sn(xt_netcfg.sn);   //设置子网掩码
		xt_wiz_set_gw(xt_netcfg.gw);   //设置网关
		#ifdef XT_NET_PRINTF
		XT_NET_PRINTF("set ip %d.%d.%d.%d\r\n", xt_netcfg.ip[0], xt_netcfg.ip[1], xt_netcfg.ip[2], xt_netcfg.ip[3]);
		XT_NET_PRINTF("set sn %d.%d.%d.%d\r\n", xt_netcfg.sn[0], xt_netcfg.sn[1], xt_netcfg.sn[2], xt_netcfg.sn[3]);
		XT_NET_PRINTF("set gw %d.%d.%d.%d\r\n", xt_netcfg.gw[0], xt_netcfg.gw[1], xt_netcfg.gw[2], xt_netcfg.gw[3]);
		#endif
		
		//初始化8个socket的收发缓冲区大小
		xt_wiz_set_rxtx_buf_size((uint8_t *)xt_wiz_rx_size_tab, (uint8_t *)xt_wiz_tx_size_tab);
		
		xt_wiz_set_retx_time(1000);    //设置重发时间（单位：100us）
		xt_wiz_set_retx_cnt(2);        //设置最大重新发送次数
		
		XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
		if (xt_netcfg.newrst == 0)
		{
			xt_netcfg.rstver = ((xt_netcfg.rstver + 1) & 0x7FFF) | 0x8000;
		}
		XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	}
	XT_WIZ_IC_MUTEX_DI();     //<<<<<<<<<<<<<<<<<<<
}

/**
  * @brief  网络运行（目前规定50mS调用，要求使用专用线程，其优先级比其它网络操作线程要低）
  * @param  void
  * @return void
  */
void xt_net_run(void)
{
	XT_WIZ_CPOINT_VARIAB()
	uint8_t linkstatus;
	
	xt_wiz_dhcp_timer(50/*ms*/);
	
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	if (xt_netcfg.newrst != 0)
	{
		xt_netcfg.newrst = 0;
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_net_rst(); //复位网络
	}
	else if (xt_netcfg.dhcp != 0)
	{
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_wiz_dhcp_run();  //DHCP运行
	}
	else
	{
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	}
	
	linkstatus = xt_wiz_get_link();
	if (linkstatus == 1)
	{	//LINK连接
		#ifdef XT_NET_PRINTF
		if ((xt_netcfg.lnkver & 0x8000) == 0) {
		XT_NET_PRINTF("link up\r\n");     }
		#endif
		XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
		if ((xt_netcfg.lnkver & 0x8000) == 0)
		{
			xt_netcfg.lnkver = ((xt_netcfg.lnkver + 1) & 0x7FFF) | 0x8000;
			xt_wiz_dhcp_reset(); ///
		}
		XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	}
	else
	{	//LINK断开
		#ifdef XT_NET_PRINTF
		if ((xt_netcfg.lnkver & 0x8000) != 0) {
		XT_NET_PRINTF("link broken!\r\n"); }
		#endif
		XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
		if ((xt_netcfg.lnkver & 0x8000) != 0)
		{
			xt_netcfg.lnkver = ((xt_netcfg.lnkver + 1) & 0x7FFF);
		}
		XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	}
}

/**
  * @brief  网络服务线程
  * @param  *p_arg   线程入口参数
  * @return void
  */
void xt_net_thread_entry(void *p_arg)
{
	#ifdef XT_NET_PIN_INIT
	xt_net_pin_init();
	#endif
	#ifdef XT_NET_CFG_INIT
	xt_net_cfg_init();
	#endif
	
	while (1)
	{
		XT_WIZ_OS_10MS(5);
		xt_net_run();
	}
}

#endif //#if (XT_APP_WIZNET_EN == XT_DEF_ENABLED)
