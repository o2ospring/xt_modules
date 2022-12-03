/**
  * @file  xt_scom_sds.c
  * @brief ������������ʾ������Visual Scope��Serial Digital Scope��
  * COPYRIGHT (C) 2022, XT ��������С��
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-01     o2ospring    ԭʼ�汾
  */
#define   XT_SCOM_SDS_C__
#include "xt_scom_sds.h"
#if (defined XT_APP_SCOMSDS_EN) && (XT_APP_SCOMSDS_EN == XT_DEF_ENABLED)
#ifdef    XT_SCOM_SDS_X__
#undef    XT_SCOM_SDS_H__
#include "xt_scom_sds.h"
#endif

#ifndef XT_MSG_INIT_2_TAB_EXPORT
#define XT_MSG_INIT_2_TAB_EXPORT(func,name)
#endif
#ifndef XT_MSG_INIT_3_TAB_EXPORT
#define XT_MSG_INIT_3_TAB_EXPORT(func,name)
#endif

xt_scom_obj_t xt_scomsds_obj;      //����ͨ�ŷ���Ӧ�ö���
uint8_t xt_scomsds_rx_buf[1/*20*/];//����ͨ�ŷ��ͻ���
uint8_t xt_scomsds_tx_buf[10];     //����ͨ�ŷ��ͻ���
uint8_t xt_scomsds_data[10] = {0}; //ʾ�������ݻ���(��4ͨ��)

void xt_scomsds_init(void);
void xt_scomsds_open(void);
void xt_scomsds_close(void);
void xt_scomsds_open_cb(const xt_scom_obj_t *p_scom);
void xt_scomsds_close_cb(const xt_scom_obj_t *p_scom);
void xt_scomsds_send(void);
void xt_scomsds_tx_complete_cb(const xt_scom_obj_t *p_scom);
int xt_scomsds_rx_data1_cb(const xt_scom_obj_t *p_scom);
int xt_scomsds_rx_data_cb(const xt_scom_obj_t *p_scom, uint16_t size);
int xt_scomsds_1ch_put(uint8_t ch_n, int16_t chx);
int xt_scomsds_4ch_put(uint8_t ch_f, int16_t ch1, int16_t ch2, int16_t ch3, int16_t ch4);
uint16_t xt_scomsds_crc16(uint8_t *p_d, uint8_t size);

/**
  * @brief  ������������ʾ������ʼ��
  * @param  void
  * @return void
  */
void xt_scomsds_init(void)
{
	XT_SCOMSDS_MUTEX_INIT();
	XT_SCOMSDS_TX_SEM_INIT();
	//xt_scomsds_open();
}
XT_MSG_INIT_2_TAB_EXPORT(xt_scomsds_init, "xt_scomsds_init()");

/**
  * @brief  [��]-����ͨ�ŷ���
  * @param  void
  * @return void
  */
void xt_scomsds_open(void)
{
	xt_scomsds_obj.baud_rate        = XT_SCOMSDS_BAUD_RATE;      //���������ԡ���ͨѶ�����ʣ�1200,2400,4800,9600,14400,19200,38400,57600,115200,128000,256000bps�ȣ�
	xt_scomsds_obj.stop_bits        = 1;                         //���������ԡ���ֹͣλ��1:1λ,2:2λ��
	xt_scomsds_obj.parity_bits      = 0;                         //���������ԡ���У��λ��0:��,1:��,2:ż��
	xt_scomsds_obj.data_bits        = 8;                         //���������ԡ�������λ��8:8λ��
	xt_scomsds_obj.scom_num         = XT_SCOMSDS_SCOM_NUM;       //���������ԡ�������ͨ��ͨ���ţ�������ͨ���ţ�����XT_SCOM_SUM��
	
	xt_scomsds_obj.section_mode     = 0;                         //��Э�����ԡ���������֡��������ʼ���С��0:��,1:1�ֽ�,2:2�ֽڣ�/�������������ս�����ģʽ��0:��,1:1�ֽڽ�����,2:2�ֽڽ�����,3:���ֽ���һ�����룩
	xt_scomsds_obj.len_offset       = 0xFF;                      //��Э�����ԡ���������֡�����ճ�����λ��ƫ�ƣ�������ʼ��������֡λ��ƫ�ƣ�����:5->��֡��6�ֽڣ�0xFF:��Ч��
	xt_scomsds_obj.len_arithm       = 0xFF;                      //��Э�����ԡ���������֡�����ճ�������֡�ܳ��㷨��5:5+������ֵ=��֡�ܳ�,0xFF:ֻ�ܳ�ʱ��֡��/��{������λ��ƫ��=0xFF}ʱ��ʾ���չ̶�����
	xt_scomsds_obj.section_us       = 5000;                      //��Э�����ԡ�����֡����ʱ�䣨��λ:us��0:��Ч�������ڳ�ʱ��֡��ʱ���ý��ջ������ɵȣ�
	xt_scomsds_obj.p_rx_buf         = xt_scomsds_rx_buf;         //��Э�����ԡ������ջ��壨�������Ϊ0�
	xt_scomsds_obj.p_tx_buf         = xt_scomsds_tx_buf;         //��Э�����ԡ������ͻ��壨�������Ϊ0�
	xt_scomsds_obj.rx_buf_size      = sizeof(xt_scomsds_rx_buf); //��Э�����ԡ������ջ����С��0:��֧�ֽ��գ��Ｐ��Ӳ���㡿��ǿ������Ϊ0�
	xt_scomsds_obj.tx_buf_size      = sizeof(xt_scomsds_tx_buf); //��Э�����ԡ������ͻ����С��0:��֧�ַ��ͣ��Ｐ��Ӳ���㡿��ǿ������Ϊ0�
	
	xt_scomsds_obj.p_scom_open_fn   = xt_scomsds_open_cb;        //��Ӧ�ò���񡿣�����ͨ�ŷ���ɹ���Эͬ���������������������ϲ�Ӧ�ñ�־��
	xt_scomsds_obj.p_scom_close_fn  = xt_scomsds_close_cb;       //��Ӧ�ò���񡿣�����ͨ�ŷ���ɹ��ر�Эͬ���������������������ϲ�Ӧ�ñ�־��
	xt_scomsds_obj.p_tx_complete_fn = xt_scomsds_tx_complete_cb; //��Ӧ�ò���񡿣��ж�����ɷ��͵�֪ͨ����        ���ڲδ���:void��
	xt_scomsds_obj.p_rx_data1_fn    = xt_scomsds_rx_data1_cb;    //��Ӧ�ò���񡿣��ж��н��յ���һ�ֽ�����֪ͨ�������ڲδ���:pob->p_rx_buf��
	xt_scomsds_obj.p_rx_data_fn     = xt_scomsds_rx_data_cb;     //��Ӧ�ò���񡿣��ж��н��յ���һ�����ݵĴ��ݺ������ڲδ���:pob->p_rx_buf, size��
	
	if (xt_scom_open(&xt_scomsds_obj, XT_SCOM_TYPE_DATA_FRAME/*����֡*/) < 0)
	{
		xt_scomsds_printf("xt_scom_open return error!\r\n");
	}
}
XT_MSG_INIT_3_TAB_EXPORT(xt_scomsds_open, "xt_scomsds_open()");

/**
  * @brief  [�ر�]-����ͨ�ŷ���
  * @param  void
  * @return void
  */
void xt_scomsds_close(void)
{
	if (xt_scom_close(&xt_scomsds_obj) < 0)
	{
		xt_scomsds_printf("xt_scom_close return error!\r\n");
	}
}

/**
  * @brief  ����ͨ�ŷ���[��]�����ɹ�Эͬ�ص�
  * @param  *p_scom          ָ��SCOM����
  * @return void
  */
void xt_scomsds_open_cb(const xt_scom_obj_t *p_scom)
{
	return;
}

/**
  * @brief  ����ͨ�ŷ���[�ر�]�����ɹ�Эͬ�ص�
  * @param  *p_scom          ָ��SCOM����
  * @return void
  */
void xt_scomsds_close_cb(const xt_scom_obj_t *p_scom)
{
	return;
}

/**
  * @brief  [֪ͨ]-�յ���1�ֽ����ݣ����ж��лص���
  * @param  *p_scom          ָ��SCOM����
  * @param  p_scom->p_rx_buf ���ݻ���
  * @return ������ֽ����ݣ�0:������ֽ�����,����:�������
  */
int xt_scomsds_rx_data1_cb(const xt_scom_obj_t *p_scom)
{
	return 0xFFFF;
}

/**
  * @brief  [����]-���յ�һ�����ݣ����ж��лص���
  * @param  *p_scom          ָ��SCOM����
  * @param  p_scom->p_rx_buf ���ݻ���
  * @param  size             �յ����ݴ�С
  * @return �Ѵ������ݣ���size:�Ѵ���ȫ������,������0xFFFF��ʾ��
  */
int xt_scomsds_rx_data_cb(const xt_scom_obj_t *p_scom, uint16_t size)
{
	return 0xFFFF;
}

/**
  * @brief  [֪ͨ]-��ɷ������ݣ����ж��лص���
  * @param  *p_scom          ָ��SCOM����
  * @return void
  */
void xt_scomsds_tx_complete_cb(const xt_scom_obj_t *p_scom)
{
	XT_SCOMSDS_TX_SEM_SEND();
}

/**
  * @brief  �򴮿�����ʾ���������������ݣ�һ·ͨ����
  * @param  ch_n       ���͵ڼ�·ͨ����� (��7λ:0->��1ͨ����)(��bit7->0��ʾ�ȴ����ݴ�����˳�,����������ʽ�������ݡ�)
  * @param  chx        ͨ����ֵ
  * @return ��0:��ʾ����, ��0:��ʾ��ȷ
  */
int xt_scomsds_1ch_put(uint8_t ch_n, int16_t chx)
{
	int i;
	
	i = ch_n & 0x7F;
	if (i >= 4) return 0;
	
	XT_SCOMSDS_MUTEX_LOCKED();  //�������� >>>>>>>>>>>>
	XT_SCOMSDS_TX_SEM_RESET();
	xt_scomsds_data[i * 2    ] = (uint8_t)(chx & 0xFF);
	xt_scomsds_data[i * 2 + 1] = (uint8_t)(chx >> 8);
	i = XT_SCOMSDS_CRC16(xt_scomsds_data, 8);
	xt_scomsds_data[8] = (uint8_t)(i & 0xFF);
	xt_scomsds_data[9] = (uint8_t)(i >> 8);
    i = xt_scom_send(&xt_scomsds_obj, xt_scomsds_data, sizeof(xt_scomsds_data)/*�ֽ�*/, 0/*us*/);
	if ((i > 0) && ((ch_n & 0x80) == 0)) {
	XT_SCOMSDS_TX_SEM_TAKE(); }
	XT_SCOMSDS_MUTEX_UNLOCK();  //������� <<<<<<<<<<<<
	
    if (i < 0)
    {
        xt_scomsds_printf("xt_scom_send return error!\r\n");
    }
	return (i);
}

/**
  * @brief  �򴮿�����ʾ���������������ݣ���·ͨ����
  * @param  ch_f       �����ļ�·ͨ����־ (��7λ:bit0->��1ͨ����)(��bit7->0��ʾ�ȴ����ݴ�����˳�,����������ʽ�������ݡ�)
  * @param  ch1        ��1·ͨ����ֵ
  * @param  ch2        ��2·ͨ����ֵ
  * @param  ch3        ��3·ͨ����ֵ
  * @param  ch4        ��4·ͨ����ֵ
  * @return ��0:��ʾ����, ��0:��ʾ��ȷ
  */
int xt_scomsds_4ch_put(uint8_t ch_f, int16_t ch1, int16_t ch2, int16_t ch3, int16_t ch4)
{
	int i;
	
	if ((ch_f & 0x0F) == 0) return 0;
	
	XT_SCOMSDS_MUTEX_LOCKED();  //�������� >>>>>>>>>>>>
	XT_SCOMSDS_TX_SEM_RESET();
	if ((ch_f & 0x01) != 0) { xt_scomsds_data[0] = (uint8_t)(ch1 & 0xFF);
	                          xt_scomsds_data[1] = (uint8_t)(ch1 >> 8); }
	if ((ch_f & 0x02) != 0) { xt_scomsds_data[2] = (uint8_t)(ch2 & 0xFF);
	                          xt_scomsds_data[3] = (uint8_t)(ch2 >> 8); }
	if ((ch_f & 0x04) != 0) { xt_scomsds_data[4] = (uint8_t)(ch3 & 0xFF);
	                          xt_scomsds_data[5] = (uint8_t)(ch3 >> 8); }
	if ((ch_f & 0x08) != 0) { xt_scomsds_data[6] = (uint8_t)(ch4 & 0xFF);
	                          xt_scomsds_data[7] = (uint8_t)(ch4 >> 8); }
	i = XT_SCOMSDS_CRC16(xt_scomsds_data, 8);
	xt_scomsds_data[8] = (uint8_t)(i & 0xFF);
	xt_scomsds_data[9] = (uint8_t)(i >> 8);
    i = xt_scom_send(&xt_scomsds_obj, xt_scomsds_data, sizeof(xt_scomsds_data)/*�ֽ�*/, 0/*us*/);
	if ((i > 0) && ((ch_f & 0x80) == 0)) {
	XT_SCOMSDS_TX_SEM_TAKE(); }
	XT_SCOMSDS_MUTEX_UNLOCK();  //������� <<<<<<<<<<<<
	
    if (i < 0)
    {
        xt_scomsds_printf("xt_scom_send return error!\r\n");
    }
	return (i);
}

/**
  * @brief  ���׼���������ݵ�CRCУ����
  * @param  p_d        ����
  * @param  size       ���ݴ�С(�ֽ�)
  * @return CRCУ����
  */
uint16_t xt_scomsds_crc16(uint8_t *p_d, uint8_t size)
{
	uint16_t crc_temp;
	uint8_t  i, j;
	crc_temp = 0xffff;
	
	for (i=0; i<size; i++)
	{
		crc_temp ^= p_d[i];
		for (j=0;j<8;j++)
		{
			if (crc_temp & 0x01)
				crc_temp = (crc_temp >> 1) ^ 0xa001;
			else
				crc_temp = (crc_temp >> 1);
		}
	}
	return(crc_temp);
}

#endif  //#if (XT_APP_SCOMSDS_EN == XT_DEF_ENABLED)
