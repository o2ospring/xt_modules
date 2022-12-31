/**
  * @file  xt_scom_vofa.c
  * @brief ������������ʾ������VOFA+ ���ؼӣ�
  * COPYRIGHT (C) 2022, XT ��������С��
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-03     o2ospring    ԭʼ�汾
  * 2022-12-15     o2ospring    ����ʾ������������Ĵ���֧��
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

xt_scom_obj_t xt_scomvofa_obj;                        //����ͨ�ŷ���Ӧ�ö���
#if (XT_SCOMVOFA_CMD_MAX != 0)
uint8_t xt_scomvofa_cmd_buf[XT_SCOMVOFA_CMD_MAX];     //����ͨ�������
uint8_t xt_scomvofa_rx_buf[XT_SCOMVOFA_CMD_MAX];      //����ͨ�Ž��ջ���
#else
uint8_t xt_scomvofa_rx_buf[1];                        //����ͨ�Ž��ջ���
#endif
uint8_t xt_scomvofa_tx_buf[4*(XT_SCOMVOFA_CH_SUM+1)]; //����ͨ�ŷ��ͻ���
float   xt_scomvofa_data[XT_SCOMVOFA_CH_SUM+1] = {0}; //ʾ�������ݻ���(��4ͨ��)

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
  * @brief  ������������ʾ������ʼ��
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
  * @brief  [��]-����ͨ�ŷ���
  * @param  void
  * @return void
  */
void xt_scomvofa_open(void)
{
	xt_scomvofa_obj.baud_rate        = XT_SCOMVOFA_BAUD_RATE;      //���������ԡ���ͨѶ�����ʣ�1200,2400,4800,9600,14400,19200,38400,57600,115200,128000,256000bps�ȣ�
	xt_scomvofa_obj.stop_bits        = 1;                          //���������ԡ���ֹͣλ��1:1λ,2:2λ��
	xt_scomvofa_obj.parity_bits      = 0;                          //���������ԡ���У��λ��0:��,1:��,2:ż��
	xt_scomvofa_obj.data_bits        = 8;                          //���������ԡ�������λ��8:8λ��
	xt_scomvofa_obj.scom_num         = XT_SCOMVOFA_SCOM_NUM;       //���������ԡ�������ͨ��ͨ���ţ�������ͨ���ţ�����XT_SCOM_SUM��
	
	xt_scomvofa_obj.section_code[0]  = '\r';                       //��Э�����ԡ����������������ս����루����:<CR><LF>��
	xt_scomvofa_obj.section_code[1]  = '\n';                       //��Э�����ԡ����������������ս����루����:<CR><LF>��
	xt_scomvofa_obj.section_mode     = 3;                          //��Э�����ԡ����������������ս�����ģʽ��0:��,1:1�ֽڽ�����,2:2�ֽڽ�����,3:���ֽ���һ�����룩
	xt_scomvofa_obj.section_us       = 0;                          //��Э�����ԡ�����֡����ʱ�䣨��λ:us��0:��Ч�������ڳ�ʱ��֡��ʱ���ý��ջ������ɵȣ�
	xt_scomvofa_obj.p_rx_buf         = xt_scomvofa_rx_buf;         //��Э�����ԡ������ջ��壨�������Ϊ0�
	xt_scomvofa_obj.p_tx_buf         = xt_scomvofa_tx_buf;         //��Э�����ԡ������ͻ��壨�������Ϊ0�
	xt_scomvofa_obj.rx_buf_size      = sizeof(xt_scomvofa_rx_buf); //��Э�����ԡ������ջ����С��0:��֧�ֽ��գ��Ｐ��Ӳ���㡿��ǿ������Ϊ0�
	xt_scomvofa_obj.tx_buf_size      = sizeof(xt_scomvofa_tx_buf); //��Э�����ԡ������ͻ����С��0:��֧�ַ��ͣ��Ｐ��Ӳ���㡿��ǿ������Ϊ0�
	
	xt_scomvofa_obj.p_scom_open_fn   = xt_scomvofa_open_cb;        //��Ӧ�ò���񡿣�����ͨ�ŷ���ɹ���Эͬ���������������������ϲ�Ӧ�ñ�־��
	xt_scomvofa_obj.p_scom_close_fn  = xt_scomvofa_close_cb;       //��Ӧ�ò���񡿣�����ͨ�ŷ���ɹ��ر�Эͬ���������������������ϲ�Ӧ�ñ�־��
	xt_scomvofa_obj.p_tx_complete_fn = xt_scomvofa_tx_complete_cb; //��Ӧ�ò���񡿣��ж�����ɷ��͵�֪ͨ����        ���ڲδ���:void��
	xt_scomvofa_obj.p_rx_data1_fn    = xt_scomvofa_rx_data1_cb;    //��Ӧ�ò���񡿣��ж��н��յ���һ�ֽ�����֪ͨ�������ڲδ���:pob->p_rx_buf��
	xt_scomvofa_obj.p_rx_data_fn     = xt_scomvofa_rx_data_cb;     //��Ӧ�ò���񡿣��ж��н��յ���һ�����ݵĴ��ݺ������ڲδ���:pob->p_rx_buf, size��
	
	xt_scomvofa_obj.p_hw_open_fn     = XT_SCOMVOFA_P_HW_OPEN_FN;   //��Ӳ������񡿣�������ģ���ṩ��Ӳ����������0:���ṩ��
	
	if (xt_scom_open(&xt_scomvofa_obj, XT_SCOM_TYPE_DATA_STREAM/*������*/) < 0)
	{
		xt_scomvofa_printf("xt_scom_open return error!\r\n");
	}
}
XT_MSG_INIT_3_TAB_EXPORT(xt_scomvofa_open, "xt_scomvofa_open()");

/**
  * @brief  [�ر�]-����ͨ�ŷ���
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
  * @brief  ����ͨ�ŷ���[��]�����ɹ�Эͬ�ص�
  * @param  *p_scom          ָ��SCOM����
  * @return void
  */
void xt_scomvofa_open_cb(const xt_scom_obj_t *p_scom)
{
	return;
}

/**
  * @brief  ����ͨ�ŷ���[�ر�]�����ɹ�Эͬ�ص�
  * @param  *p_scom          ָ��SCOM����
  * @return void
  */
void xt_scomvofa_close_cb(const xt_scom_obj_t *p_scom)
{
	return;
}

/**
  * @brief  [֪ͨ]-�յ���1�ֽ����ݣ����ж��лص���
  * @param  *p_scom          ָ��SCOM����
  * @param  p_scom->p_rx_buf ���ݻ���
  * @return ������ֽ����ݣ�0:������ֽ�����,����:�������
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
  * @brief  [����]-���յ�һ�����ݣ����ж��лص���
  * @param  *p_scom          ָ��SCOM����
  * @param  p_scom->p_rx_buf ���ݻ���
  * @param  size             �յ����ݴ�С
  * @return �Ѵ������ݣ���size:�Ѵ���ȫ������,������0xFFFF��ʾ��
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
  * @brief  [֪ͨ]-��ɷ������ݣ����ж��лص���
  * @param  *p_scom          ָ��SCOM����
  * @return void
  */
void xt_scomvofa_tx_complete_cb(const xt_scom_obj_t *p_scom)
{
	XT_SCOMVOFA_TX_SEM_SEND();
}

/**
  * @brief  �򴮿�����ʾ���������������ݣ��޸�һ·ͨ����
  * @param  ch_n       �޸ĵڼ�·ͨ����� (��7λ:0->��1ͨ����)(��bit7->0��ʾ��������ʽ�������ݡ�)
  * @param  chx        ͨ����ֵ
  * @return ��0:��ʾ����, ��0:��ʾ��ȷ
  */
int xt_scomvofa_1ch_put(uint8_t ch_n, float chx)
{
	int i;
	
	i = ch_n & 0x7F;
	if (i >= XT_SCOMVOFA_CH_SUM) return 0;
	
	XT_SCOMVOFA_MUTEX_LOCKED();  //�������� >>>>>>>>>>>>
	XT_SCOMVOFA_TX_SEM_RESET();
	#if (XT_SCOMVOFA_BIG_ENDIAN_SW == 0)
	xt_scomvofa_data[i] = chx;   //С�� float ���ݸ�ʽ
	#else
	xt_scomvofa_data[i] = XT_SCOMVOFA_SWAP32(chx);
	#endif
	i = xt_scom_send(&xt_scomvofa_obj, (uint8_t *)xt_scomvofa_data, sizeof(xt_scomvofa_data)/*�ֽ�*/, 0/*us*/);
	if ((i > 0) && ((ch_n & 0x80) == 0)) {
	XT_SCOMVOFA_TX_SEM_TAKE(); }
	XT_SCOMVOFA_MUTEX_UNLOCK();  //������� <<<<<<<<<<<<
	
	if (i < 0)
	{
		xt_scomvofa_printf("xt_scom_send return error!\r\n");
	}
	return (i);
}

/**
  * @brief  �򴮿�����ʾ���������������ݣ��޸Ķ�·ͨ����
  * @param  ch_f       �޸��ļ�·ͨ����־ (��7λ:bit0->��1ͨ����)(��bit7->0��ʾ��������ʽ�������ݡ�)
  * @param  ch1        ��1·ͨ����ֵ
  * @param  ch2        ��2·ͨ����ֵ
  * @param  ch3        ��3·ͨ����ֵ
  * @param  ch4        ��4·ͨ����ֵ
  * @return ��0:��ʾ����, ��0:��ʾ��ȷ
  */
int xt_scomvofa_4ch_put(uint8_t ch_f, float ch1, float ch2, float ch3, float ch4)
{
	int i;
	
	if ((ch_f & 0x0F) == 0) return 0;
	
	XT_SCOMVOFA_MUTEX_LOCKED();  //�������� >>>>>>>>>>>>
	XT_SCOMVOFA_TX_SEM_RESET();
	#if (XT_SCOMVOFA_BIG_ENDIAN_SW == 0)
		#if (XT_SCOMVOFA_CH_SUM >= 1)
	if ((ch_f & 0x01) != 0) { xt_scomvofa_data[0] = ch1; } //С�� float ���ݸ�ʽ
		#endif
		#if (XT_SCOMVOFA_CH_SUM >= 2)
	if ((ch_f & 0x02) != 0) { xt_scomvofa_data[1] = ch2; } //С�� float ���ݸ�ʽ
		#endif
		#if (XT_SCOMVOFA_CH_SUM >= 3)
	if ((ch_f & 0x04) != 0) { xt_scomvofa_data[2] = ch3; } //С�� float ���ݸ�ʽ
		#endif
		#if (XT_SCOMVOFA_CH_SUM >= 4)
	if ((ch_f & 0x08) != 0) { xt_scomvofa_data[3] = ch4; } //С�� float ���ݸ�ʽ
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
	i = xt_scom_send(&xt_scomvofa_obj, (uint8_t *)xt_scomvofa_data, sizeof(xt_scomvofa_data)/*�ֽ�*/, 0/*us*/);
	if ((i > 0) && ((ch_f & 0x80) == 0)) {
	XT_SCOMVOFA_TX_SEM_TAKE(); }
	XT_SCOMVOFA_MUTEX_UNLOCK();  //������� <<<<<<<<<<<<
	
	if (i < 0)
	{
		xt_scomvofa_printf("xt_scom_send return error!\r\n");
	}
	return (i);
}

/**
  * @brief  �򴮿�����ʾ���������������ݣ��޸Ķ�·����ͨ����
  * @param  ch_s       �޸Ķ���·����ͨ�� (��7λ:1->��1ͨ��)(��bit7->0��ʾ��������ʽ�������ݡ�)
  * @param  *p_ch      ��·����ͨ����ֵ
  * @param  s_ch       �޸ĵ���ʼͨ��    (0->��1·ͨ��)
  * @return ��0:��ʾ����, ��0:��ʾ��ȷ
  */
int xt_scomvofa_xch_put(uint8_t ch_s, float *p_ch, uint8_t s_ch)
{
	int i;
	uint8_t ch_sum = ch_s & 0x7F;
	
	if (ch_sum == 0) return 0;
	if (s_ch  >= XT_SCOMVOFA_CH_SUM) return -1;
	if (ch_sum > XT_SCOMVOFA_CH_SUM - s_ch) ch_sum = XT_SCOMVOFA_CH_SUM - s_ch;
	
	XT_SCOMVOFA_MUTEX_LOCKED();  //�������� >>>>>>>>>>>>
	XT_SCOMVOFA_TX_SEM_RESET();
	#if (XT_SCOMVOFA_BIG_ENDIAN_SW == 0)
	for (i=0; i<ch_sum; i++) { xt_scomvofa_data[i + s_ch] = p_ch[i]; }; //С�� float ���ݸ�ʽ
	#else
	for (i=0; i<ch_sum; i++) { xt_scomvofa_data[i + s_ch] = XT_SCOMVOFA_SWAP32(p_ch[i]); }
	#endif
	i = xt_scom_send(&xt_scomvofa_obj, (uint8_t *)xt_scomvofa_data, sizeof(xt_scomvofa_data)/*�ֽ�*/, 0/*us*/);
	if ((i > 0) && ((ch_s & 0x80) == 0)) {
	XT_SCOMVOFA_TX_SEM_TAKE(); }
	XT_SCOMVOFA_MUTEX_UNLOCK();  //������� <<<<<<<<<<<<
	
	if (i < 0)
	{
		xt_scomvofa_printf("xt_scom_send return error!\r\n");
	}
	return (i);
}

/**
  * @brief  ����ʾ���������Ŀ���ָ���ʱѭ�����ü��ɣ�
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
