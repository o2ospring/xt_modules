/**
  * @file  xt_wiznet_port.c
  * @brief 对接 WIZnet Hardwired TCP/IP Chips (WIZCHIP) 硬件TCP/IP协议栈（如：W5100S 芯片）
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-10     o2ospring    原始版本
  *                             仓库:   https://github.com/Wiznet/ioLibrary_Driver.git
  *                             Date:   Mon Feb 21 09:32:42 2022 +0900
  *                             Fix W5100S getIR()
  */
#define   XT_WIZNET_PORT_C__
#include "wizchip_conf.h"   /*
#include "xt_wiznet_port.h" */
#if (defined XT_APP_WIZNET_EN) && (XT_APP_WIZNET_EN == XT_DEF_ENABLED)
#ifdef    XT_WIZNET_PORT_X__
#undef    XT_WIZNET_PORT_H__
#include "xt_wiznet_port.h"
#endif

// 操作配置寄存器：
//
//   1. 模式寄存器（MR）
//   2. 中断屏蔽寄存器（IMR）
//   3. 重发时间寄存器（RTR）
//   4. 重发计数寄存器（RCR）
//	
// 网络信息寄存器：
//
//   1. 本机物理地址寄存器（SHAR）
//   2. 本机 IP 地址寄存器（SIPR）
//   3. 子网掩码寄存器（SUBR）
//   4. 网关地址寄存器（GAR）
//	
// 套接字信息寄存器：
//
//   1. 接收缓冲器缓存数据大小（RSR）
//   2. 发送缓冲器空闲空间大小（FSR）
//   3. 套接字工作配置寄存器（CR）
//   4. 套接字工作状态寄存器（SR）

extern xt_net_t xt_netcfg;

extern void xt_net_dhcp_reset(void);
extern void xt_net_dhcp_ipget(uint8_t *ip, uint8_t *gw, uint8_t *sn);
extern uint8_t xt_net_chk_socket_reset(void);

void wizchip_cris_en(void);
void wizchip_cris_ex(void);
void wizchip_ip_assign(void);
void wizchip_ip_update(void);
void wizchip_ip_conflict(void);
void xt_socket_ot_rst(uint8_t s);

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ PHY 对接 ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

//1、PHY-引脚初始化
void xt_wiz_pin_init(void)
{
	xt_wiz_hspi_gpio_init();
	reg_wizchip_cris_cbfunc(wizchip_cris_en, wizchip_cris_ex);
	reg_wizchip_spi_cbfunc(xt_wiz_hspi_r_byte, xt_wiz_hspi_w_byte);
	reg_wizchip_spiburst_cbfunc(xt_wiz_hspi_r_nbyte, xt_wiz_hspi_w_nbyte);
	reg_wizchip_cs_cbfunc(xt_wiz_hspi_cs_en, xt_wiz_hspi_cs_di);
}

//2、PHY-复位（硬复位）
void xt_wiz_reset(void)
{
	xt_wiz_hspi_rst_en();
	XT_WIZ_OS_10MS(50);
	xt_wiz_hspi_rst_di();
	XT_WIZ_OS_10MS(150);
}

//3、PHY-初始化（软复位）
void xt_wiz_init(void)
{
	setMR(MR_RST);
}

//4、PHY-初始化收发缓冲区大小（4个socket）
void xt_wiz_set_rxtx_buf_size(uint8_t *p_rx, uint8_t *p_tx)
{
	uint16_t i;
	#if (_WIZCHIP_ != W5300)
	for (i=0; i<XT_SOCK_SUM; i++)
	{
		if ((p_rx[i] != 1)
		&&  (p_rx[i] != 2)
		&&  (p_rx[i] != 4)
		&&  (p_rx[i] != 8))
		{
			while(1);
		}
		if ((p_tx[i] != 1)
		&&  (p_tx[i] != 2)
		&&  (p_tx[i] != 4)
		&&  (p_tx[i] != 8))
		{
			while(1);
		}
	}
	#endif
	if (wizchip_init(p_tx, p_tx) < 0) //其里面还调用了 wizchip_sw_reset()
	{
		while(1);
	}
}

//5、PHY-寄存器读写
void xt_wiz_set_retx_time(uint16_t x100us) { setRTR(x100us);            } //设置重发时间（单位：100us）
void xt_wiz_set_retx_cnt(uint8_t retry)    { setRCR(retry);             } //设置最大重新发送次数
void xt_wiz_set_mac(uint8_t *p_mac)        { setSHAR(p_mac);            } //设置物理地址
void xt_wiz_set_ip(uint8_t *p_ip)          { setSIPR(p_ip);             } //设置IP地址
void xt_wiz_set_sn(uint8_t *p_sn)          { setSUBR(p_sn);             } //设置子网掩码
void xt_wiz_set_gw(uint8_t *p_gw)          { setGAR (p_gw);             } //设置网关
void xt_wiz_get_mac(uint8_t *p_mac)        { getSHAR(p_mac);            } //读取物理地址
void xt_wiz_get_ip(uint8_t *p_ip)          { getSIPR(p_ip);             } //读取IP地址
void xt_wiz_get_sn(uint8_t *p_sn)          { getSUBR(p_sn);             } //读取子网掩码
void xt_wiz_get_gw(uint8_t *p_gw)          { getGAR (p_gw);             } //读取网关
uint8_t xt_wiz_get_link(void)              { return ((wizphy_getphylink() & 0x01) ? 1 : 0); } //读取网口状态

//--芯片基本单元操作互斥进入
void wizchip_cris_en(void)
{
	XT_WIZ_SPI_MUTEX_EN();
}
//--芯片基本单元操作互斥退出
void wizchip_cris_ex(void)
{
	XT_WIZ_SPI_MUTEX_DI();
}

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ DHCP 对接 +++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

//1、DHCP-初始化
void xt_wiz_dhcp_init(void)
{
	setSHAR(xt_netcfg.mac);                       //设置MAC
	DHCP_init(XT_WIZ_SOCK_DHCP, xt_wiz_dhcp_buf); //设置使用socket用于DHCP和给DHCP分配缓冲器
	reg_dhcp_cbfunc(wizchip_ip_assign, wizchip_ip_update, wizchip_ip_conflict);             //DHCP获取到IP回调
	xt_wiz_set_rxtx_buf_size((uint8_t *)xt_wiz_rx_size_tab, (uint8_t *)xt_wiz_tx_size_tab); //设置收发缓冲大小
}

//2、DHCP-运行
uint8_t xt_wiz_dhcp_run(void)
{
	return (DHCP_run());       //check_DHCP_leasedIP() -> setRCR() 操作了芯片全局寄存器，原则上
}	                           //其两个 setRCR() 前后加入互斥处理，只是目前应用上不加也影响不大。

//3、DHCP-计时器
void xt_wiz_dhcp_timer(uint32_t ms)
{
	static uint32_t dhcp_tick_cnt = 0;
	dhcp_tick_cnt += ms;
	if (dhcp_tick_cnt > 1000)
	{
		dhcp_tick_cnt = 0;     //1秒节拍
		DHCP_time_handler();   //1秒计时
	}
}

//4、DHCP-重新ARP
void xt_wiz_dhcp_reset(void)
{
	extern volatile uint32_t dhcp_tick_1s;
	dhcp_tick_1s = 0x7FFFFFFF; //强制DHCP超时
}

//--更新:IP分配
void wizchip_ip_assign(void)
{
	uint8_t ip[4];
	uint8_t gw[4];
	uint8_t sn[4];
	
	extern void default_ip_assign(void);
	uint32_t ef;
	XT_WIZ_IC_MUTEX_EN(ef);    //>>>>>>>>>>>>
	default_ip_assign();       //DHCP内部处理
	getIPfromDHCP(ip);
	getGWfromDHCP(gw);
	getSNfromDHCP(sn);
	xt_net_dhcp_ipget(ip, gw, sn);
	XT_WIZ_IC_MUTEX_DI();     //<<<<<<<<<<<<
}
//--更新:IP更新
void wizchip_ip_update(void)
{
	uint8_t ip[4];
	uint8_t gw[4];
	uint8_t sn[4];
	
	extern void default_ip_update(void);
	uint32_t ef;
	xt_net_dhcp_reset();   ///
	XT_WIZ_IC_MUTEX_EN(ef);    //>>>>>>>>>>>>
	default_ip_update();       //DHCP内部处理
	getIPfromDHCP(ip);
	getGWfromDHCP(gw);
	getSNfromDHCP(sn);
	xt_net_dhcp_ipget(ip, gw, sn);
	XT_WIZ_IC_MUTEX_DI();     //<<<<<<<<<<<<
}
//--更新:IP冲突
void wizchip_ip_conflict(void)
{
	uint8_t ip[4];
	uint8_t gw[4];
	uint8_t sn[4];
	
	extern void default_ip_conflict(void);
	uint32_t ef;
	xt_net_dhcp_reset();   ///
	XT_WIZ_IC_MUTEX_EN(ef);    //>>>>>>>>>>>>
	default_ip_conflict();     //DHCP内部处理
	getIPfromDHCP(ip);
	getGWfromDHCP(gw);
	getSNfromDHCP(sn);
	xt_net_dhcp_ipget(ip, gw, sn);
	XT_WIZ_IC_MUTEX_DI();     //<<<<<<<<<<<<
}

/********************************************************************************************************/
/*+++++++++++++++++++++++++++++++++++++++++++++ socket 对接 ++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

//--判断:网络是否重启
uint8_t xt_wiz_socket_rst_chk(uint8_t s)
{
	uint8_t prio;
	XT_WIZ_OS_THREAD_T pthr;
	if (((xt_wiz_waite_flag[s] & 0x80) == 0) && (xt_net_chk_socket_reset() != 0))
	{	//bit7=1->DHCP专用socket
		uint8_t temp;
		pthr = XT_WIZ_OS_THREAD_GET();
		prio = XT_WIZ_OS_THREAD_PRI_GET(pthr);
		temp = XT_WIZ_OS_THREAD_PRI_LOW;
		XT_WIZ_OS_THREAD_PRI_CHANGE(pthr, temp);
		XT_WIZ_OS_THREAD_YIELD();
		XT_WIZ_OS_THREAD_PRI_CHANGE(pthr, prio);
		return 1;
	}
	else
	{
		return 0;
	}
}

//--判断:等待初始化(fixed=1:强制为固定等待时间,=2:强制为只能等待时间)
void xt_wiz_waite_sta(xt_wizchip_t *pwt, uint8_t s, uint8_t fixed, uint8_t max)
{
	XT_WIZ_CPOINT_VARIAB()
	uint32_t ef = 0;
	fixed = fixed > 2 ? 0 : fixed;
	
	if (pwt->fixed == 0xFF)
	{
		XT_WIZ_SK_MUTEX_EN(s, ef); //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		pwt->scheduler_cnt = 0;
		ef = 0xFF;
	}
	pwt->scheduler_ver = 0;
	pwt->scheduler_max = max;
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	pwt->flag = 0;
	if (ef == 0xFF)
	{
		pwt->wtfg = xt_wiz_waite_flag[s];
	}
	if (xt_wiz_waite_tick[s] == (XT_WIZ_OS_TICK_T)(0xFFFFFFFF))
	{
		pwt->fixed = 0; //强制永远等待
		pwt->tick  = 0;
	}
	else if (fixed == 1)
	{
		pwt->fixed = 1; //强制为等待时间
		pwt->tick  = MS(500) - 1 + XT_WIZ_OS_TICK_GET();
	}
	else if (fixed == 2)
	{
		pwt->fixed = 1; //强制为等待时间
		pwt->tick  = (((pwt->wtfg & 0x10) == 0) ? xt_wiz_waite_tick[s] : xt_wiz_waite_init[s]) - 1 + XT_WIZ_OS_TICK_GET();
	}
	else if ((pwt->wtfg & 0x10) == 0)
	{
		pwt->fixed = 1; //强制为等待时间
		pwt->tick  = xt_wiz_waite_tick[s] - 1 + XT_WIZ_OS_TICK_GET();
	}
	else
	{
		pwt->fixed = 2; //强制为尝试次数
		pwt->tick  = xt_wiz_waite_tick[s];
	}
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
	
	if (((pwt->wtfg & 0x01) == 0) && (ef == 0xFF))
	{	//bit0=0->允许降优先级调度
		pwt->pthr = XT_WIZ_OS_THREAD_GET();
		pwt->prio = XT_WIZ_OS_THREAD_PRI_GET(pwt->pthr);
	}
}

//--判断:等待工作中（ver=0:不改变调度,=0xFF:只更新max,=其它:当版本不同时重置cnt和更新max）
uint8_t xt_wiz_waite_chk(xt_wizchip_t *pwt, uint8_t s, uint8_t ver, uint8_t max)
{
	if ((ver > 0)
	&& ((ver != pwt->scheduler_ver) /*|| (ver == 0xFF)*/))
	{
		if (ver != 0xFF)
		{
			pwt->scheduler_ver = ver;
			pwt->scheduler_cnt = 0;
		}
		pwt->scheduler_max = max;
	}
	if (xt_wiz_waite_tick[s] == 0)
	{
		pwt->flag = 3; //被强制退出等待
		return 3;
	}
	else if (((pwt->wtfg & 0x80) == 0) && (xt_net_chk_socket_reset() != 0))
	{	//bit7=1->DHCP专用socket
		pwt->flag = 2; //因复位退出等待
		return 2;
	}
	else if (((pwt->fixed == 1) && ((pwt->tick - XT_WIZ_OS_TICK_GET()) > XT_WIZ_OS_TICK_OVMAX))
	||       ((pwt->fixed == 2) && ((pwt->tick == 0) || (--(pwt->tick) == 0))))
	{
		//ttest
		ver = XT_WIZ_OS_TICK_GET() - pwt->tick;
		if (ver > 1)
		{
			ver--;
		}
		
		pwt->flag = 1; //因超时退出等待
		return 1;
	}
	else
	{
		if (++(pwt->scheduler_cnt) >= pwt->scheduler_max)
		{
			pwt->scheduler_cnt = 0;
			if (((pwt->wtfg & 0x01) == 0)
			&&   (XT_WIZ_OS_THREAD_PRI_GET(pwt->pthr) == pwt->prio)
			&&   (XT_WIZ_OS_THREAD_PRI_LOW != pwt->prio))
			{
				uint8_t temp = XT_WIZ_OS_THREAD_PRI_LOW;
				XT_WIZ_OS_THREAD_PRI_CHANGE(pwt->pthr, temp);
			}
			XT_WIZ_OS_THREAD_YIELD();
		}
		return 0;
	}
}

//--判断:等待完成
uint8_t xt_wiz_waite_end(xt_wizchip_t *pwt, uint8_t s)
{
	xt_socket_ot_rst(s); //////////
	if ((pwt->wtfg & 0x01) == 0)
	{	//bit0=0->允许降优先级调度
		if (XT_WIZ_OS_THREAD_PRI_GET(pwt->pthr) != pwt->prio)
		{
			XT_WIZ_OS_THREAD_PRI_CHANGE(pwt->pthr, pwt->prio);
		}
		pwt->fixed = 0xFF;
		XT_WIZ_SK_MUTEX_DI(s);    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		return 1;
	}
	else
	{
		pwt->fixed = 0xFF;
		XT_WIZ_SK_MUTEX_DI(s);    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		return 0;
	}
}

/**
  * @brief  重置socket工作超时时间
  * @param  s        : socket通道号（0~7）
  * @return void
  */
void xt_socket_ot_rst(uint8_t s)
{
	XT_WIZ_CPOINT_VARIAB()
	if (s >= XT_SOCK_SUM) return;
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	xt_wiz_waite_tick[s] = xt_wiz_waite_init[s];
	xt_wiz_waite_flag[s] = 0;
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  设置socket工作超时时间
  * @param  s        : socket通道号（0~7）
  * @param  ostick   : 系统节拍数(0:强制退出!!!,0xFFFFFFFF:永久)
  * @param  flag     : bit0=1->禁止降优先级调度
  *                    bit4=1->等待改为循环次数(只针对:xt_socket_recv_chk())
  *                    bit7=1->DHCP专用socket,应用绝对不能使用
  *                    其它标志位必须为0,本模块内部作特别用途
  * @return void
  */
void xt_socket_ot_set(uint8_t s, XT_WIZ_OS_TICK_T ostick, uint8_t flag)
{
	XT_WIZ_CPOINT_VARIAB()
	if (s >= XT_SOCK_SUM) return;
	XT_WIZ_CPOINT_LOCKED(); //>>>>>>>>>>>>>>>
	xt_wiz_waite_tick[s] = ostick;
	xt_wiz_waite_flag[s] = flag & 0x91;
	XT_WIZ_CPOINT_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  检测与等待socket接收数据
  * @param  s        : socket通道号（0~7）
  * @return 0->超时/无数据, 非0->收到一包数据大小
  */
uint16_t xt_socket_recv_chk(uint8_t s)
{
	uint16_t ret;
	xt_wizchip_t wt; wt.fixed = 0xFF;
	if (s >= XT_SOCK_SUM) return (0);
	if (xt_wiz_socket_rst_chk(s) != 0) return (0);
	xt_wiz_waite_sta(&wt, s, 0, 1);
	while ((ret = getSn_RX_RSR(s)) == 0 && (xt_wiz_waite_chk(&wt, s, 0, 0) == 0));
	xt_wiz_waite_end(&wt, s);
	return(ret);
}

/**
  * @brief  检测socket工作是否正常
  * @param  s        : socket通道号（0~7）
  *         protocol : socket模式（Sn_MR_TCP,Sn_MR_UDP）
  *         server   : 针对TCP是服务端还是客户端（0->客户端 或 已连接）
  * @return 0->异常,需要重启socket, 非0->正常
  */
uint8_t xt_socket_chk(uint8_t s, uint8_t protocol, uint8_t server)
{
	uint8_t  status;
	uint8_t  ret;
	if (s >= XT_SOCK_SUM) return (0);
	if (xt_wiz_socket_rst_chk(s) != 0) return (0);
	status = getSn_SR(s);
	switch (protocol & 0x0F)
	{
		case Sn_MR_TCP:
			if (server == 0)
			{
				ret = ((status == SOCK_ESTABLISHED)
				/**/|| (status == SOCK_CLOSE_WAIT)) ? 1 : 0;
			}
			else
			{
				ret = ((status >= SOCK_LISTEN)
				/**/&& (status <= SOCK_CLOSE_WAIT)) ? 1 : 1;
			}
			break;
		case Sn_MR_UDP:
			ret = (status == SOCK_UDP) ? 1 : 0;
			break;
		default:
			ret = 1;
			break;
	}
	return(ret);
}

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ 系统对接 ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

void xt_wiz_ipc_init(void)
{
	XT_WIZ_CPOINT_INIT();
	XT_WIZ_SPI_MUTEX_INIT();
	XT_WIZ_IC_MUTEX_INIT();
}

void xt_wiz_thread_init(void)
{
	XT_WIZ_THREAD_INIT();
}

#else
uint8_t xt_wiz_socket_rst_chk(uint8_t s) {return 0;}
void xt_wiz_waite_sta(xt_wizchip_t *pwt, uint8_t s, uint8_t fixed, uint8_t max) {return;}
uint8_t xt_wiz_waite_chk(xt_wizchip_t *pwt, uint8_t s, uint8_t ver, uint8_t max) {return 1;}
uint8_t xt_wiz_waite_end(xt_wizchip_t *pwt, uint8_t s) {return 0;}
void xt_socket_ot_rst(uint8_t s) {return;}
void xt_socket_ot_set(uint8_t s, XT_WIZ_OS_TICK_T ostick, uint8_t flag) {return;}
uint16_t xt_socket_recv_chk(uint8_t s) {return 0;}
uint8_t xt_socket_chk(uint8_t s, uint8_t protocol, uint8_t server) {return 0;}

#endif //#if (XT_APP_WIZNET_EN == XT_DEF_ENABLED)
