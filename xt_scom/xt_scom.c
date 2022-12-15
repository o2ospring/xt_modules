/**
  * @file  xt_scom.c
  * @brief 串行通信服务模块
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-11-27     o2ospring    原始版本
  */
#define   XT_SCOM_C__
#include "xt_scom.h"
#if (defined XT_APP_SCOM_EN) && (XT_APP_SCOM_EN == XT_DEF_ENABLED)
#ifdef    XT_SCOM_X__
#undef    XT_SCOM_H__
#include "xt_scom.h"
#endif

xt_scom_obj_t *xt_p_scom[XT_SCOM_SUM] = {0};

extern int xt_scom_hw_open(xt_scom_obj_t *p_ob);

int xt_scom_open(xt_scom_obj_t *p_ob, uint8_t scom_type);
int xt_scom_close(xt_scom_obj_t *p_ob);
int xt_scom_send(xt_scom_obj_t *p_ob, const uint8_t *pb, uint16_t size, uint8_t delay);
void xt_scom_recv_reset(xt_scom_obj_t *p_ob);
void xt_scom_uart_rx_reset(xt_scom_obj_t *p_ob);
void xt_scom_uart_frame_tx(xt_scom_obj_t *p_ob);
void xt_scom_uart_frame_rx(xt_scom_obj_t *p_ob);
void xt_scom_time_frame_rx(xt_scom_obj_t *p_ob);
#define xt_scom_uart_stream_tx xt_scom_uart_frame_tx
void xt_scom_uart_stream_rx(xt_scom_obj_t *p_ob);
#define xt_scom_time_stream_rx xt_scom_time_frame_rx

/**
  * @brief  打开-串行通信
  * @param  *p_ob      串行通信对象（★初始相关参数★）
  * @param  scom_type  串行通信类型（0~XT_SCOM_TYPE_SUM-1）
  * @return ＜0:表示出错, ≥0:表示正确
  */
int xt_scom_open(xt_scom_obj_t *p_ob, uint8_t scom_type)
{
	XT_SCOM_VARIAB()
	int i;
	
	XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
	if ((p_ob->scom_num >= XT_SCOM_SUM)
	||  (scom_type >= XT_SCOM_TYPE_SUM))
	{
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->scom_num/scom_type error!\r\n");
		return -1;
	}
	if (xt_p_scom[p_ob->scom_num] != 0)
	{
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->xt_p_scom error!\r\n");
		return -2;
	}
	xt_p_scom[p_ob->scom_num] = (void *)1;
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	if ((p_ob->baud_rate < 1200)
	||  (p_ob->baud_rate > 256000))
	{
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->baud_rate error!\r\n");
		return -3;
	}
	if ((p_ob->stop_bits != 1)
	&&  (p_ob->stop_bits != 2))
	{
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->stop_bits error!\r\n");
		return -4;
	}
	if ((p_ob->parity_bits > 2)
	|| ((p_ob->parity_bits > 0) && (p_ob->data_bits > 8)))
	{
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->parity_bits error!\r\n");
		return -5;
	}
	if ((p_ob->data_bits != 8)
	&&  (p_ob->data_bits != 9))
	{
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->data_bits error!\r\n");
		return -6;
	}
	if (((p_ob->len_offset == 0xFF) && (p_ob->len_arithm < 2))
	||  ((p_ob->section_mode > 2) && (scom_type == XT_SCOM_TYPE_DATA_FRAME))
	||  ((p_ob->section_mode > 3) && (scom_type == XT_SCOM_TYPE_DATA_STREAM)))
	{
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->len_arithm/section_mode error!\r\n");
		return -7;
	}
	if ((p_ob->p_rx_buf == 0)
	||  (p_ob->p_tx_buf == 0))
	{
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->p_rx_buf/p_tx_buf error!\r\n");
		return -8;
	}
	if ((p_ob->rx_buf_size == 0)
	&&  (p_ob->tx_buf_size == 0))
	{
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->rx_buf_size/tx_buf_size error!\r\n");
		return -9;
	}
	if (/*(p_ob->p_scom_open_fn == 0)
	||  (p_ob->p_scom_close_fn == 0)
	||  (p_ob->p_tx_complete_fn == 0)
	||*/(p_ob->p_rx_data1_fn == 0)
	||  (p_ob->p_rx_data_fn == 0))
	{
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->app cb finction error!\r\n");
		return -10;
	}
	if ((p_ob->tx_buf_size > 0xFF) && (scom_type == XT_SCOM_TYPE_DATA_FRAME))
	{
		p_ob->tx_buf_size = 0xFF;
	}
	if ((p_ob->rx_buf_size > 0xFF) && (scom_type == XT_SCOM_TYPE_DATA_FRAME))
	{
		p_ob->rx_buf_size = 0xFF;
	}
	if (scom_type == XT_SCOM_TYPE_DATA_FRAME)
	{
		p_ob->p_dv_uart_rx_reset = xt_scom_uart_rx_reset;
		p_ob->p_dv_uart_tx_fn = xt_scom_uart_frame_tx;
		p_ob->p_dv_uart_rx_fn = xt_scom_uart_frame_rx;
		p_ob->p_dv_time_rx_fn = xt_scom_time_frame_rx;
	}
	else if (scom_type == XT_SCOM_TYPE_DATA_STREAM)
	{
		p_ob->p_dv_uart_rx_reset = xt_scom_uart_rx_reset;
		p_ob->p_dv_uart_tx_fn = xt_scom_uart_stream_tx;
		p_ob->p_dv_uart_rx_fn = xt_scom_uart_stream_rx;
		p_ob->p_dv_time_rx_fn = xt_scom_time_stream_rx;
	}
	else
	{
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->scom_type error!\r\n");
		return -11;
		#if (XT_SCOM_TYPE_SUM > 2)
		#error ""
		#endif
	}
	p_ob->rx_tim_flag = 0;
	p_ob->tx_run_flag = 0;
	p_ob->tx_run_ver += 1;
	p_ob->rx_cnt = 0;
	p_ob->tx_cnt = 0;
	p_ob->tx_sum = 0;
	
	p_ob->p_hw_tx_putc_null_fn = 0;    	
	p_ob->p_hw_tx_putc_fn      = 0;	
	p_ob->p_hw_tx_putc_end_fn  = 0;
	p_ob->p_hw_rx_getc_fn      = 0;
	p_ob->p_hw_rx_overtime_set_fn = 0;
	if (p_ob->p_hw_open_fn == 0)
	{
		#if (XT_SCOM_HW_DRIVERS_EN == 0)
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->p_hw_open_fn error!\r\n");
		return -12;
		#else
		p_ob->p_hw_open_fn = xt_scom_hw_open;
		#endif
	}
	if ((i = (*(p_ob->p_hw_open_fn))(p_ob)) < 0)
	{
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->p_hw_open_fn error!\r\n");
		return i;
	}
	else if ((p_ob->p_hw_tx_putc_fn == 0)
	||       (p_ob->p_hw_tx_putc_end_fn == 0)
	||       (p_ob->p_hw_rx_getc_fn == 0)
	||       (p_ob->p_hw_rx_overtime_set_fn == 0))
	{
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_scom[p_ob->scom_num] = 0;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_open->p_hw_open_fn error!\r\n");
		return -12;
	}
	else
	{
		XT_SCOM_TASK_LOCKED(); //>>>>>>>>>>>>>>
		XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
		(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
		xt_p_scom[p_ob->scom_num] = p_ob;
		i = p_ob->scom_num;
		XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		if (p_ob->p_scom_open_fn != 0) {
		(*(p_ob->p_scom_open_fn))(p_ob); }
		XT_SCOM_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		return i;
	}
}

/**
  * @brief  关闭-串行通信
  * @param  *p_ob      串行通信对象
  * @return ＜0:表示出错, ≥0:表示正确
  */
int xt_scom_close(xt_scom_obj_t *p_ob)
{
	XT_SCOM_VARIAB()
	int i;
	
	XT_SCOM_TASK_LOCKED(); //>>>>>>>>>>>>>>
	XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
	if ((p_ob <= (xt_scom_obj_t *)1)
	||  (p_ob->scom_num >= XT_SCOM_SUM)
	||  (xt_p_scom[p_ob->scom_num] != p_ob))
	{
		i = (p_ob->scom_num >= XT_SCOM_SUM) ? 0 : (int)(xt_p_scom[p_ob->scom_num]);
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_SCOM_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_close->scom_num/xt_p_scom error!\r\n");
		return (i == 1) ? -1 : -2;
	}
	else
	{
		xt_p_scom[p_ob->scom_num] = 0;
		i = p_ob->scom_num;
		(*(p_ob->p_hw_close_fn))(p_ob);
		p_ob->tx_run_flag = 0;
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		if (p_ob->p_scom_close_fn != 0) {
		(*(p_ob->p_scom_close_fn))(p_ob); }
	XT_SCOM_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		return i;
	}
}

/**
  * @brief  启动-发送
  * @param  *p_ob      串行通信对象
  * @param  *pb        数据串
  * @param  size       数据大小
  * @param  delay      发送延时（单位：us，但应该要小于等于5us，否则长时间占用CPU）
  * @return ＜0:表示出错, ≥0:表示正确
  */
int xt_scom_send(xt_scom_obj_t *p_ob, const uint8_t *pb, uint16_t size, uint8_t delay)
{
	XT_SCOM_VARIAB()
	uint16_t v;
	
	XT_SCOM_TASK_LOCKED(); //>>>>>>>>>>>>>>
	XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
	if (size == 0)
	{
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_SCOM_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		return 0;
	}
	else if ((p_ob <= (xt_scom_obj_t *)1)
	||       (p_ob->scom_num >= XT_SCOM_SUM)
	||       (xt_p_scom[p_ob->scom_num] != p_ob))
	{
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_SCOM_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_send->scom_num/xt_p_scom error!\r\n");
		return -1;
	}
	else if (p_ob->tx_run_flag != 0)
	{
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_SCOM_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_send->tx_run_flag error!\r\n");
		return -2;
	}
	else if ((pb == 0)
	||       (size > p_ob->tx_buf_size))
	{
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_SCOM_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_send->pb/size error!\r\n");
		return -3;
	}
	p_ob->tx_run_flag = 1;
	p_ob->tx_run_ver += 1;
	v = p_ob->tx_run_ver;
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	memcpy(p_ob->p_tx_buf, pb, size);
	XT_SCOM_TASK_UNLOCK(); //<<<<<<<<<<<<<<
	
	if (delay > 0)
	{
		XT_SCOM_SEND_DELAY_US(delay);
	}
	
	XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
	if ((p_ob->tx_run_flag != 1)
	||  (p_ob->tx_run_ver  != v)
	||  (p_ob->scom_num >= XT_SCOM_SUM)
	||  (xt_p_scom[p_ob->scom_num] != p_ob))
	{
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_scom_printf("xt_scom_send->scom_num/xt_p_scom error!\r\n");
		return -1;
	}
	if (p_ob->p_hw_tx_putc_null_fn != 0)
	{
		p_ob->tx_cnt = 0;
		p_ob->tx_sum = size;
		(*(p_ob->p_hw_tx_putc_null_fn))();
		p_ob->tx_run_flag = 2;
	}
	else
	{
		p_ob->tx_cnt = 0;
		p_ob->tx_sum = size;
		(*(p_ob->p_hw_tx_putc_fn))(p_ob->p_tx_buf[0], /*(1 >= p_ob->tx_sum) ? 2 : */0);
		p_ob->tx_cnt = 1;
		p_ob->tx_run_flag = 2;
	}
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
	return 1;
}

/**
  * @brief  重置-接收（计数器）
  * @param  *p_ob      串行通信对象
  * @return void
  */
void xt_scom_recv_reset(xt_scom_obj_t *p_ob)
{
	XT_SCOM_VARIAB()
	
	XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
	p_ob->rx_cnt = 0;
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  串口-接收重置（接收异常处理）
  * @param  *p_ob      串行通信对象（★注意:由硬件层调用前保证合法性!!!）
  * @return void
  */
void xt_scom_uart_rx_reset(xt_scom_obj_t *p_ob)
{
	p_ob->rx_cnt = 0;
}

/**
  * @brief  串口-〖帧〗发送数据
  * @param  *p_ob      串行通信对象
  * @return void
  */
void xt_scom_uart_frame_tx(xt_scom_obj_t *p_ob)
{
	int i;
	
	if (p_ob->tx_run_flag == 2)
	{
		if (p_ob->tx_cnt == 0)
		{	//第一字节发送之前
			(*(p_ob->p_hw_tx_putc_fn))(p_ob->p_tx_buf[0], (1 >= p_ob->tx_sum) ? 2 : 0);
			p_ob->tx_cnt = 1;
		}
		else if (p_ob->tx_cnt >= p_ob->tx_sum)
		{	//最后一字节发送之后
			(*(p_ob->p_hw_tx_putc_end_fn))();
			p_ob->tx_run_flag = 0;
			if (p_ob->p_tx_complete_fn != 0) {
			(*(p_ob->p_tx_complete_fn))(p_ob); }
		}
		else
		{	//发送数据之中
			i = p_ob->tx_cnt + 1;
			(*(p_ob->p_hw_tx_putc_fn))(p_ob->p_tx_buf[p_ob->tx_cnt], (i >= p_ob->tx_sum) ? 2 : 1);
			p_ob->tx_cnt = i;
		}
	}
}

/**
  * @brief  串口-〖帧〗接收数据
  * @param  *p_ob      串行通信对象
  * @return void
  */
void xt_scom_uart_frame_rx(xt_scom_obj_t *p_ob)
{
	int i;
	
	p_ob->rx_tim_flag = 1;
	i = (*(p_ob->p_hw_rx_getc_fn))();
	if (p_ob->rx_buf_size == 0)
	{
		(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
		p_ob->rx_cnt = 0;
		p_ob->rx_tim_flag = 0;
		return;
	}
	else if (p_ob->rx_cnt < p_ob->section_mode)
	{	//起始码
		if (p_ob->section_code[p_ob->rx_cnt] != i)
		{
			(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
			p_ob->rx_cnt = 0;
			p_ob->rx_tim_flag = 0;
			return;
		}
	}
	else if (p_ob->len_offset == 0xFF)
	{	//固定帧
		if (p_ob->rx_cnt >= p_ob->len_arithm - 1)
		{
			p_ob->p_rx_buf[p_ob->rx_cnt] = i;
			(p_ob->rx_cnt)++;
			
			i = (*(p_ob->p_rx_data_fn))(p_ob, p_ob->len_arithm);//收完数据(固定长度数据帧)
			if ((i < p_ob->len_arithm) && (i > 0))
			{
				memcpy(p_ob->p_rx_buf, &(p_ob->p_rx_buf[i]), p_ob->len_arithm - i);
				p_ob->rx_cnt = p_ob->len_arithm - i;
				(*(p_ob->p_hw_rx_overtime_set_fn))(p_ob->section_us == 0 ? 0xFFFF : p_ob->section_us);
				p_ob->rx_tim_flag = 2;
			}
			else
			{
				(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
				p_ob->rx_cnt = 0;
				p_ob->rx_tim_flag = 0;
			}
			return;
		}
	}
	else if (p_ob->rx_cnt > p_ob->len_offset)
	{	//长度码
		if ((p_ob->rx_cnt + 1) >= (p_ob->len_arithm + p_ob->p_rx_buf[p_ob->len_offset]))
		{
			p_ob->p_rx_buf[p_ob->rx_cnt] = i;
			(p_ob->rx_cnt)++;
			
			i = (*(p_ob->p_rx_data_fn))(p_ob, p_ob->rx_cnt);//收完数据(有长度值数据帧)
			if ((i < p_ob->rx_cnt) && (i > 0))
			{
				memcpy(p_ob->p_rx_buf, &(p_ob->p_rx_buf[i]), p_ob->rx_cnt - i);
				p_ob->rx_cnt = p_ob->rx_cnt - i;
				(*(p_ob->p_hw_rx_overtime_set_fn))(p_ob->section_us == 0 ? 0xFFFF : p_ob->section_us);
				p_ob->rx_tim_flag = 2;
			}
			else
			{
				(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
				p_ob->rx_cnt = 0;
				p_ob->rx_tim_flag = 0;
			}
			return;
		}
	}
	
	p_ob->p_rx_buf[p_ob->rx_cnt] = i;
	(p_ob->rx_cnt)++;
	
	if (p_ob->rx_cnt == 1)
	{	//发通知
		if (0 == (*(p_ob->p_rx_data1_fn))(p_ob))
		{
			(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
			p_ob->rx_cnt = 0;
			p_ob->rx_tim_flag = 0;
			return;
		}
	}
	
	if (p_ob->rx_cnt >= p_ob->rx_buf_size)
	{	//缓冲满
		i = (*(p_ob->p_rx_data_fn))(p_ob, p_ob->rx_buf_size);//收完数据(缓冲已满)
		if ((i < p_ob->rx_buf_size) && (i > 0))
		{
			memcpy(p_ob->p_rx_buf, &(p_ob->p_rx_buf[i]), p_ob->rx_buf_size - i);
			p_ob->rx_cnt = p_ob->rx_buf_size - i;
			(*(p_ob->p_hw_rx_overtime_set_fn))(p_ob->section_us == 0 ? 0xFFFF : p_ob->section_us);
			p_ob->rx_tim_flag = 2;
		}
		else
		{
			(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
			p_ob->rx_cnt = 0;
			p_ob->rx_tim_flag = 0;
		}
		return;
	}
	
	(*(p_ob->p_hw_rx_overtime_set_fn))(p_ob->section_us == 0 ? 0xFFFF : p_ob->section_us);
	p_ob->rx_tim_flag = 2;
}

/**
  * @brief  定时器-〖帧〗接收超时处理
  * @param  *p_ob      串行通信对象
  * @return void
  */
void xt_scom_time_frame_rx(xt_scom_obj_t *p_ob)
{
	XT_SCOM_VARIAB()
	int i;
	
	XT_SCOM_LOCKED(); //>>>>>>>>>>>>>>>
	if (p_ob->rx_tim_flag == 0)
	{
		(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
	}
	else if (p_ob->rx_tim_flag >= 2)
	{
		if (((*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF) != 0)
		&&     (p_ob->section_us > 0))
		{
			if (p_ob->rx_cnt > 0)
			{
				i = (*(p_ob->p_rx_data_fn))(p_ob, p_ob->rx_cnt);//收完数据(接收间隔超时)
				if ((i < p_ob->rx_cnt) && (i > 0))
				{
					memcpy(p_ob->p_rx_buf, &(p_ob->p_rx_buf[i]), p_ob->rx_cnt - i);
					p_ob->rx_cnt = p_ob->rx_cnt - i;
					(*(p_ob->p_hw_rx_overtime_set_fn))(p_ob->section_us);
					p_ob->rx_tim_flag = 2;
				}
				else
				{
					p_ob->rx_cnt = 0;
					p_ob->rx_tim_flag = 0;
				}
			}
			else
			{
				p_ob->rx_tim_flag = 0;
			}
		}
	}
	XT_SCOM_UNLOCK(); //<<<<<<<<<<<<<<<
}

/**
  * @brief  串口-〖数据流〗接收数据
  * @param  *p_ob      串行通信对象
  * @return void
  */
void xt_scom_uart_stream_rx(xt_scom_obj_t *p_ob)
{
	int i;
	
	p_ob->rx_tim_flag = 1;
	i = (*(p_ob->p_hw_rx_getc_fn))();
	if (p_ob->rx_buf_size == 0)
	{
		(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
		p_ob->rx_cnt = 0;
		p_ob->rx_tim_flag = 0;
		return;
	}
	else if (p_ob->section_mode == 1)
	{	//结束码(一字节结束符)
		if (p_ob->section_code[0] == i)
		{
			p_ob->p_rx_buf[p_ob->rx_cnt] = i;
			(p_ob->rx_cnt)++;
			
			i = (*(p_ob->p_rx_data_fn))(p_ob, p_ob->rx_cnt);//收完数据
			if ((i < p_ob->rx_cnt) && (i > 0))
			{
				memcpy(p_ob->p_rx_buf, &(p_ob->p_rx_buf[i]), p_ob->rx_cnt - i);
				p_ob->rx_cnt = p_ob->rx_cnt - i;
				(*(p_ob->p_hw_rx_overtime_set_fn))(p_ob->section_us == 0 ? 0xFFFF : p_ob->section_us);
				p_ob->rx_tim_flag = 2;
			}
			else
			{
				(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
				p_ob->rx_cnt = 0;
				p_ob->rx_tim_flag = 0;
			}
			return;
		}
	}
	else if (p_ob->section_mode == 2)
	{	//结束码(两字节[与关系]结束符)
		if ((p_ob->section_code[1] == i)
		&&  (p_ob->rx_cnt > 0)
		&&  (p_ob->section_code[0] == p_ob->p_rx_buf[p_ob->rx_cnt - 1]))
		{
			p_ob->p_rx_buf[p_ob->rx_cnt] = i;
			(p_ob->rx_cnt)++;
			
			i = (*(p_ob->p_rx_data_fn))(p_ob, p_ob->rx_cnt);//收完数据
			if ((i < p_ob->rx_cnt) && (i > 0))
			{
				memcpy(p_ob->p_rx_buf, &(p_ob->p_rx_buf[i]), p_ob->rx_cnt - i);
				p_ob->rx_cnt = p_ob->rx_cnt - i;
				(*(p_ob->p_hw_rx_overtime_set_fn))(p_ob->section_us == 0 ? 0xFFFF : p_ob->section_us);
				p_ob->rx_tim_flag = 2;
			}
			else
			{
				(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
				p_ob->rx_cnt = 0;
				p_ob->rx_tim_flag = 0;
			}
			return;
		}
	}
	else if (p_ob->section_mode == 3)
	{	//结束码(两字节[或关系]结束符)
		if ((p_ob->section_code[0] == i)
		||  (p_ob->section_code[1] == i))
		{
			p_ob->p_rx_buf[p_ob->rx_cnt] = i;
			(p_ob->rx_cnt)++;
			
			i = (*(p_ob->p_rx_data_fn))(p_ob, p_ob->rx_cnt);//收完数据
			if ((i < p_ob->rx_cnt) && (i > 0))
			{
				memcpy(p_ob->p_rx_buf, &(p_ob->p_rx_buf[i]), p_ob->rx_cnt - i);
				p_ob->rx_cnt = p_ob->rx_cnt - i;
				(*(p_ob->p_hw_rx_overtime_set_fn))(p_ob->section_us == 0 ? 0xFFFF : p_ob->section_us);
				p_ob->rx_tim_flag = 2;
			}
			else
			{
				(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
				p_ob->rx_cnt = 0;
				p_ob->rx_tim_flag = 0;
			}
			return;
		}
	}
	
	p_ob->p_rx_buf[p_ob->rx_cnt] = i;
	(p_ob->rx_cnt)++;
	
	if (p_ob->rx_cnt == 1)
	{	//发通知
		if (0 == (*(p_ob->p_rx_data1_fn))(p_ob))
		{
			(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
			p_ob->rx_cnt = 0;
			p_ob->rx_tim_flag = 0;
			return;
		}
	}
	
	if (p_ob->rx_cnt >= p_ob->rx_buf_size)
	{	//缓冲满
		i = (*(p_ob->p_rx_data_fn))(p_ob, p_ob->rx_buf_size);//收完数据(缓冲已满)
		if ((i < p_ob->rx_buf_size) && (i > 0))
		{
			memcpy(p_ob->p_rx_buf, &(p_ob->p_rx_buf[i]), p_ob->rx_buf_size - i);
			p_ob->rx_cnt = p_ob->rx_buf_size - i;
			(*(p_ob->p_hw_rx_overtime_set_fn))(p_ob->section_us == 0 ? 0xFFFF : p_ob->section_us);
			p_ob->rx_tim_flag = 2;
		}
		else
		{
			(*(p_ob->p_hw_rx_overtime_set_fn))(0xFFFF);
			p_ob->rx_cnt = 0;
			p_ob->rx_tim_flag = 0;
		}
		return;
	}
	
	(*(p_ob->p_hw_rx_overtime_set_fn))(p_ob->section_us == 0 ? 0xFFFF : p_ob->section_us);
	p_ob->rx_tim_flag = 2;
}

#endif  //#if (XT_APP_SCOM_EN == XT_DEF_ENABLED)
