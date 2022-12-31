/**
  * @file  xt_wiznet_vofa.c
  * @brief UDP������������ʾ������VOFA+ ���ؼӣ�
  * COPYRIGHT (C) 2022, XT ��������С��
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-16     o2ospring    ԭʼ�汾
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

uint8_t xt_wizvofa_open_f = 0;                         //ʾ�����򿪱�־
uint8_t xt_wizvofa_run_f  = 0;                         //ʾ��������״̬
#if (XT_WIZVOFA_CMD_MAX != 0)
uint8_t xt_wizvofa_cmd_buf[XT_WIZVOFA_CMD_MAX+1];      //ʾ���������
#else
uint8_t xt_wizvofa_cmd_buf[10];
#endif
float   xt_wizvofa_snd_dat[XT_WIZVOFA_CH_SUM+1] = {0}; //ʾ�������ݻ���
XT_WIZVOFA_OS_TICK_T xt_wizvofa_refresh_cn;            //ʾ����ˢ��ʱ�䣨���ļ�ʱ����
XT_WIZVOFA_OS_TICK_T xt_wizvofa_refresh_tk;            //ʾ����ˢ��ʱ�䣨���ļ�ʱֵ��

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
  * @brief  ������������ʾ������ʼ��
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
  * @brief  ��������������ʾ����
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
  * @brief  ����ʾ����ˢ��ʱ�䣨ͬʱ�����̹߳�����
  * @param  tk         ����ʾ����ˢ��ʱ�䣨@ϵͳ���ģ�
  * @return void
  */
void xt_wizvofa_cycl(XT_WIZVOFA_OS_TICK_T tk)
{
	XT_WIZVOFA_VARIAB()
	
	//��ͣʾ����ˢ����ʾ
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
	//���������̲߳�����ʾ����ˢ��ʱ��
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
	//����ˢ����ʾ���޸�ʾ����ˢ��ʱ��
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
  * @brief  �޸�����ʾ���������������ݣ��޸�һ·ͨ����
  * @param  ch_n       �޸ĵڼ�·ͨ����� (��7λ:0->��1ͨ����)(��bit7->0��ʾ��������ʽ�������ݡ�)
  * @param  chx        ͨ����ֵ
  * @return ��0:��ʾ����, ��0:��ʾ��ȷ
  */
int xt_wizvofa_1ch_put(uint8_t ch_n, float chx)
{
	XT_WIZVOFA_VARIAB()
	int i;
	
	i = ch_n & 0x7F;
	if (i >= XT_WIZVOFA_CH_SUM) return -1;
	
	XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
	#if (XT_WIZVOFA_BIG_ENDIAN_SW == 0)
	xt_wizvofa_snd_dat[i] = chx;   //С�� float ���ݸ�ʽ
	#else
	xt_wizvofa_snd_dat[i] = XT_WIZVOFA_SWAP32(chx);
	#endif
	XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
	
	if (xt_wizvofa_open_f == 0) return 0;
	return (1);
}

/**
  * @brief  �޸�����ʾ���������������ݣ��޸Ķ�·ͨ����
  * @param  ch_f       �޸��ļ�·ͨ����־ (��7λ:bit0->��1ͨ����)(��bit7->0��ʾ��������ʽ�������ݡ�)
  * @param  ch1        ��1·ͨ����ֵ
  * @param  ch2        ��2·ͨ����ֵ
  * @param  ch3        ��3·ͨ����ֵ
  * @param  ch4        ��4·ͨ����ֵ
  * @return ��0:��ʾ����, ��0:��ʾ��ȷ
  */
int xt_wizvofa_4ch_put(uint8_t ch_f, float ch1, float ch2, float ch3, float ch4)
{
	XT_WIZVOFA_VARIAB()
	
	if ((ch_f & 0x0F) == 0)     return -1;
	
	XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
	#if (XT_WIZVOFA_BIG_ENDIAN_SW == 0)
		#if (XT_WIZVOFA_CH_SUM >= 1)
	if ((ch_f & 0x01) != 0) { xt_wizvofa_snd_dat[0] = ch1; } //С�� float ���ݸ�ʽ
		#endif
		#if (XT_WIZVOFA_CH_SUM >= 2)
	if ((ch_f & 0x02) != 0) { xt_wizvofa_snd_dat[1] = ch2; } //С�� float ���ݸ�ʽ
		#endif
		#if (XT_WIZVOFA_CH_SUM >= 3)
	if ((ch_f & 0x04) != 0) { xt_wizvofa_snd_dat[2] = ch3; } //С�� float ���ݸ�ʽ
		#endif
		#if (XT_WIZVOFA_CH_SUM >= 4)
	if ((ch_f & 0x08) != 0) { xt_wizvofa_snd_dat[3] = ch4; } //С�� float ���ݸ�ʽ
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
  * @brief  �޸�����ʾ���������������ݣ��޸Ķ�·����ͨ����
  * @param  ch_s       �޸Ķ���·����ͨ��(��7λ:1->��1ͨ��)
  * @param  *p_ch      ��·����ͨ����ֵ
  * @param  s_ch       �޸ĵ���ʼͨ��    (0->��1·ͨ��)
  * @return ��0:��ʾ����, ��0:��ʾ��ȷ
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
	for (i=0; i<ch_sum; i++) { xt_wizvofa_snd_dat[i + s_ch] = p_ch[i]; }; //С�� float ���ݸ�ʽ
	#else
	for (i=0; i<ch_sum; i++) { xt_wizvofa_snd_dat[i + s_ch] = XT_WIZVOFA_SWAP32(p_ch[i]); }
	#endif
	XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
	
	if (xt_wizvofa_open_f == 0) return 0;
	return (1);
}

/**
  * @brief  ����ʾ���������Ŀ���ָ��
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
  * @brief  ����ʾ��������˵�IP
  * @param  argc       ��ڲ�������
  * @param  argv[1]    ����IP���磺192��
  * @param  argv[2]    ����IP���磺168��
  * @param  argv[3]    ����IP���磺 2 ��
  * @param  argv[4]    ����IP���磺221��
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
  * @brief  ����ʾ����ˢ��ʱ�䣨ͬʱ�����̹߳�����
  * @param  argc       ��ڲ�������
  * @param  argv[1]    ����ʾ����ˢ��ʱ�䣨@ϵͳ���ģ�
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
  * @brief  ����ʾ��������
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
  * @brief  ʾ���������߳�
  * @param  void
  * @return void
  */
void xt_wizvofa_thread_entry(void *p_arg)
{
	XT_WIZVOFA_VARIAB()
	XT_WIZVOFA_OS_TICK_T  tk;
	uint16_t    wkver, lkver;
	uint16_t    len, port;    //�˿�
	uint8_t     ip[4];        //IP��ַ
	
	xt_wizvofa_refresh_cn = XT_WIZVOFA_OS_TICK_GET() - 1;
	xt_wizvofa_socket:
	while (((wkver = xt_net_get_rstver()) & 0x8000) == 0)           //�ȴ������ʼ�����
	{
		XT_WIZVOFA_OS_DLY_TICK(1);
	}
	xt_wizvofa_link:
	while (((lkver = xt_net_get_lnkver()) & 0x8000) == 0)           //�ȴ���������
	{
		XT_WIZVOFA_OS_DLY_TICK(1);
	}
	xt_wizvofa_build:
	socket(XT_WIZVOFA_SOCKET, Sn_MR_UDP, XT_WIZVOFA_SOCKET_NUM, 0); //��socket��UDPЭ�飩
	while (1)
	{                                                               //���socket�쳣,��...
		if (xt_socket_chk(XT_WIZVOFA_SOCKET, Sn_MR_UDP, 0) == 0) goto xt_wizvofa_build; 
		if (wkver != xt_net_get_rstver()) goto xt_wizvofa_socket;   //�������������,��...
		if (lkver != xt_net_get_lnkver()) goto xt_wizvofa_link;     //������߰ε���,��...
		
		//��������
		XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
		if ((tk = xt_wizvofa_refresh_cn - XT_WIZVOFA_OS_TICK_GET()) >= XT_WIZVOFA_OS_TICK_OVMAX)
		{
			xt_wizvofa_refresh_cn = xt_wizvofa_refresh_tk + XT_WIZVOFA_OS_TICK_GET();
			xt_wizvofa_run_f = 0;
			xt_socket_ot_set(XT_WIZVOFA_SOCKET, 1/*һ��*/, 0x10);   //�޸ı���socket��ʱ�����Ǵ���
		}
		else
		{	/* ���յȴ��޷�ȷ��׼ȷ��ʱ���������α�����!!!!!!!!!!!!!!!
			xt_wizvofa_run_f = 1;
			xt_socket_ot_set(XT_WIZVOFA_SOCKET, tk, 0);             //�޸ı���socket��ʱ������ʱ��
			*/
			xt_wizvofa_run_f = 0;
			xt_socket_ot_set(XT_WIZVOFA_SOCKET, 1/*һ��*/, 0x10);   //�޸ı���socket��ʱ�����Ǵ���
		}
		XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
		len = xt_socket_recv_chk(XT_WIZVOFA_SOCKET);                //���socket�Ƿ��յ�����
		XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
		if (xt_wizvofa_run_f >= 2)
		{
			xt_socket_ot_rst(XT_WIZVOFA_SOCKET);
		}
		xt_wizvofa_run_f = 0;
		XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
		
		#if (XT_WIZVOFA_CMD_MAX != 0)
		len = len > XT_WIZVOFA_CMD_MAX ? XT_WIZVOFA_CMD_MAX : len;  //�������ݴ�С�뻺���С��ϵ
		#else
		len = sizeof(xt_wizvofa_cmd_buf) - 1;
		#endif
		if (len > 0)
		{                                                           //socket�������ݣ�ͬʱ�õ��Զ˵�IP���˿ڣ�
			len = recvfrom(XT_WIZVOFA_SOCKET, xt_wizvofa_cmd_buf, len, ip, &port);
			xt_wizvofa_cmd_buf[len] = '\n';
			xt_wizvofa_cmd_run();
		}
		
		/* ���յȴ��޷�ȷ��׼ȷ��ʱ�����Թ̶���ʱ����!!!!!!!!!!!!!!!
		*/
		tk = xt_wizvofa_refresh_tk + 1;
		XT_WIZVOFA_OS_DLY_TICK((tk <= XT_WIZVOFA_OS_TICK_LIMIT) ? tk : 1);
		
		//��������
		XT_WIZVOFA_LOCKED();  //>>>>>>>>>>>>
		if ((xt_wizvofa_refresh_cn - XT_WIZVOFA_OS_TICK_GET()) >= XT_WIZVOFA_OS_TICK_OVMAX)
		{
			xt_wizvofa_refresh_cn = xt_wizvofa_refresh_tk + XT_WIZVOFA_OS_TICK_GET();
			
			if (xt_wizvofa_open_f == 1)
			{
				memcpy(ip, xt_wizvofa_ip, 4);
		XT_WIZVOFA_UNLOCK();  //<<<<<<<<<<<<
				
				len = sizeof(xt_wizvofa_snd_dat);                   //socket��������
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
