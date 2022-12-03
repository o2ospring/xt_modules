/**
  * @file  xt_ir_send.c
  * @brief 红外发送服务模块
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-11-16     o2ospring    原始版本
  */
#define   XT_IR_SEND_C__
#include "xt_ir_send.h"
#if (defined XT_APP_IRSEND_EN) && (XT_APP_IRSEND_EN == XT_DEF_ENABLED)
#ifdef    XT_IR_SEND_X__
#undef    XT_IR_SEND_H__
#include "xt_ir_send.h"
#endif

xt_irsend_obj_t *xt_p_irsend[XT_IRSEND_SUM] = {0};

extern int xt_irsend_hw_open(xt_irsend_obj_t *p_ob);

int xt_irsend_open(xt_irsend_obj_t *p_ob);
int xt_irsend_close(xt_irsend_obj_t *p_ob);
int xt_irsend_send(xt_irsend_obj_t *p_ob, const uint16_t *pd, uint16_t size, uint8_t xus, uint8_t khz);
int xt_irsend_decode_send(xt_irsend_obj_t *p_ob, const uint8_t *pd, uint16_t size, const xt_irsend_decode_fn_t p_fn);
void xt_irsend_wave(xt_irsend_obj_t *p_ob);

/**
  * @brief  打开-红外发送服务
  * @param  *p_ob      红外发送服务对象（★初始相关参数★）
  * @return ＜0:表示出错, ≥0:表示正确
  */
int xt_irsend_open(xt_irsend_obj_t *p_ob)
{
	XT_IRSEND_VARIAB()
	int i;
	
	XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
	if (p_ob->ir_num >= XT_IRSEND_SUM)
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_open->ir_num error!\r\n");
		return -1;
	}
	if (xt_p_irsend[p_ob->ir_num] != 0)
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_open->xt_p_irsend error!\r\n");
		return -2;
	}
	xt_p_irsend[p_ob->ir_num] = (void *)1;
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	if ((p_ob->end_us > 500000)
	|| ((p_ob->end_us < 5000) && (p_ob->end_us != 0)))
	{
		XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_irsend[p_ob->ir_num] = 0;
		XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_open->end_us error!\r\n");
		return -3;
	}
	if (p_ob->ir_khz < 30)
	{
		XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_irsend[p_ob->ir_num] = 0;
		XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_open->ir_khz error!\r\n");
		return -4;
	}
	if (p_ob->send_buf_size == 0)
	{
		XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_irsend[p_ob->ir_num] = 0;
		XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_open->send_buf_size error!\r\n");
		return -5;
	}
	if (p_ob->p_send_buf == 0)
	{
		XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_irsend[p_ob->ir_num] = 0;
		XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_open->p_send_buf error!\r\n");
		return -6;
	}/*
	if ((p_ob->p_irsend_open_fn == 0)
	||  (p_ob->p_irsend_close_fn == 0)
	||  (p_ob->p_irsend_complete_fn == 0))
	{
		XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_irsend[p_ob->ir_num] = 0;
		XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_open->app cb finction error!\r\n");
		return -7;
	}*/
	p_ob->send_run_flag = 0;
	p_ob->send_run_ver += 1;
	p_ob->send_cnt = 0;
	p_ob->send_sum = 0;
	p_ob->p_decode_fn = 0;
	p_ob->p_dv_wave_fn = xt_irsend_wave;
	if (p_ob->p_hw_open_fn == 0)
	{
		#if (XT_IRSEND_HW_DRIVERS_EN == 0)
		XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_irsend[p_ob->ir_num] = 0;
		XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_open->p_hw_open_fn error!\r\n");
		return -7;
		#else
		p_ob->p_hw_open_fn = xt_irsend_hw_open;
		#endif
	}
	if ((i = (*(p_ob->p_hw_open_fn))(p_ob)) < 0)
	{
		XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_irsend[p_ob->ir_num] = 0;
		XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_open->p_hw_open_fn error!\r\n");
		return i;
	}
	else if ((p_ob->p_hw_close_fn == 0)
	||       (p_ob->p_hw_putd_before_fn == 0)
	||       (p_ob->p_hw_putd_a_fn == 0)
	||       (p_ob->p_hw_putd_b_fn == 0)
	||       (p_ob->p_hw_putd_end_fn == 0))
	{
		XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_irsend[p_ob->ir_num] = 0;
		XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_open->p_hw_open_fn error!\r\n");
		return -8;
	}
	else
	{
		XT_IRSEND_TASK_LOCKED(); //>>>>>>>>>>>>>>
		XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
		xt_p_irsend[p_ob->ir_num] = p_ob;
		i = p_ob->ir_num;
		XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		if (p_ob->p_irsend_open_fn != 0) {
		(*(p_ob->p_irsend_open_fn))(p_ob); }
		XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		return i;
	}
}

/**
  * @brief  关闭-红外发送服务
  * @param  *p_ob      红外发送服务对象
  * @return ＜0:表示出错, ≥0:表示正确
  */
int xt_irsend_close(xt_irsend_obj_t *p_ob)
{
	XT_IRSEND_VARIAB()
	int i;
	
	XT_IRSEND_TASK_LOCKED(); //>>>>>>>>>>>>>>
	XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
	if ((p_ob <= (xt_irsend_obj_t *)1)
	||  (p_ob->ir_num >= XT_IRSEND_SUM)
	||  (xt_p_irsend[p_ob->ir_num] != p_ob))
	{
		i = (p_ob->ir_num >= XT_IRSEND_SUM) ? 0 : (int)(xt_p_irsend[p_ob->ir_num]);
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_close->ir_num/xt_p_irsend error!\r\n");
		return (i == 1) ? -1 : -2;
	}
	else
	{
		xt_p_irsend[p_ob->ir_num] = 0;
		i = p_ob->ir_num;
		(*(p_ob->p_hw_close_fn))(p_ob);
		p_ob->send_run_flag = 0;
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		if (p_ob->p_irsend_close_fn != 0) {
		(*(p_ob->p_irsend_close_fn))(p_ob); }
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		return i;
	}
}

/**
  * @brief  发送-红外码
  * @param  *p_ob      红外发送服务对象
  * @param  *pd        红外码数据串  （单位:xus微秒）      ┌──────────→（每个数据的[bit0]同时用作是否有载波标志,bit0=1:表示此个数据为有载波时间）
  * @param  size       红外码数据大小（单位:字节）（特别：bit15=1表示上层应用已为每个数据设置有无载波标志；否则数据默认为:有载波时间，无载波时间，有，无 …）
  * @param  xus        红外码数据单位（单位:xus微秒）（如：4表示单位为4us,推荐:1-4us）
  * @param  khz        红外载波频率  （单位:KHz,≥30KHz）
  * @return ＜0:表示出错, ≥0:表示正确
  */
int xt_irsend_send(xt_irsend_obj_t *p_ob, const uint16_t *pd, uint16_t size, uint8_t xus, uint8_t khz)
{
	XT_IRSEND_VARIAB()
	uint16_t i = (size & 0x8000) ? 1 : 0;
	uint16_t v;
	
	size &= 0x7FFF;
	XT_IRSEND_TASK_LOCKED(); //>>>>>>>>>>>>>>
	XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
	if (size < sizeof(p_ob->p_send_buf[0]))
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		return 0;
	}
	else if ((p_ob <= (xt_irsend_obj_t *)1)
	||       (p_ob->ir_num >= XT_IRSEND_SUM)
	||       (xt_p_irsend[p_ob->ir_num] != p_ob))
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->ir_num/xt_p_irsend error!\r\n");
		return -1;
	}
	else if (p_ob->send_run_flag != 0)
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->send_run_flag error!\r\n");
		return -2;
	}
	else if ((pd == 0)
	||       (size > p_ob->send_buf_size))
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->pd/size error!\r\n");
		return -3;
	}
	else if ((xus == 0)
	||       (khz < 30))
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->xus/khz error!\r\n");
		return -4;
	}
	p_ob->send_run_flag = 1;
	p_ob->send_run_ver += 1;
	v = p_ob->send_run_ver;
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	memcpy((char *)(p_ob->p_send_buf), (char *)pd, size);
	size /= sizeof(p_ob->p_send_buf[0]);
	if (i == 0)
	{
		if ((size & 0x0001) == 0)
		{
			for (i=0; i<size; /*i+=2*/)
			{
				p_ob->p_send_buf[i++] |= 0x0001;
				p_ob->p_send_buf[i++] &= 0xFFFE;
			}
		}
		else
		{
			size--;
			for (i=0; i<size; /*i+=2*/)
			{
				p_ob->p_send_buf[i++] |= 0x0001;
				p_ob->p_send_buf[i++] &= 0xFFFE;
			}
			p_ob->p_send_buf[i] |= 0x0001;
			size++;
		}
	}
	(*(p_ob->p_hw_putd_before_fn))(khz);
	p_ob->ir_xus = xus;
	p_ob->ir_khz = khz;
	p_ob->send_cnt = 0;
	p_ob->send_sum = size;
	if (p_ob->end_us > 0) {
	p_ob->send_sum += 1;    }
	p_ob->p_decode_fn = 0;
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
	
	XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
	if ((p_ob->send_run_flag != 1)
	||  (p_ob->send_run_ver  != v)
	||  (p_ob->ir_num >= XT_IRSEND_SUM)
	||  (xt_p_irsend[p_ob->ir_num] != p_ob))
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->ir_num/xt_p_irsend error!\r\n");
		return -1;
	}
	p_ob->send_run_flag = 2;
	if ((p_ob->p_send_buf[0] & 0x0001) != 0)
	{
		(*(p_ob->p_hw_putd_a_fn))((uint32_t)(p_ob->p_send_buf[0] & 0xFFFE) * (uint32_t)(p_ob->ir_xus), 0);
	}
	else
	{
		(*(p_ob->p_hw_putd_b_fn))((uint32_t)(p_ob->p_send_buf[0]) * (uint32_t)(p_ob->ir_xus), 0);
	}
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	return 1;
}

/**
  * @brief  发送-红外码（即时解码方式）
  * @param  *p_ob      红外发送服务对象
  * @param  *pd        红外码压缩码
  * @param  size       红外码压缩码大小（单位:字节）
  * @param  *p_fn      红外解码函数
  * @return ＜0:表示出错, ≥0:表示正确
  */
int xt_irsend_decode_send(xt_irsend_obj_t *p_ob, const uint8_t *pd, uint16_t size, const xt_irsend_decode_fn_t p_fn)
{
	XT_IRSEND_VARIAB()
	uint16_t d, v;
	uint8_t khz;
	
	XT_IRSEND_TASK_LOCKED(); //>>>>>>>>>>>>>>
	XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
	if (size < sizeof(p_ob->p_send_buf[0]))
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		return 0;
	}
	else if ((p_ob <= (xt_irsend_obj_t *)1)
	||       (p_ob->ir_num >= XT_IRSEND_SUM)
	||       (xt_p_irsend[p_ob->ir_num] != p_ob))
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->ir_num/xt_p_irsend error!\r\n");
		return -1;
	}
	else if (p_ob->send_run_flag != 0)
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->send_run_flag error!\r\n");
		return -2;
	}
	else if ((pd == 0)
	||       (size > p_ob->send_buf_size))
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->pd/size error!\r\n");
		return -3;
	}
	else if (p_fn == 0)
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->p_fn error!\r\n");
		return -4;
	}
	p_ob->send_run_flag = 1;
	p_ob->send_run_ver += 1;
	v = p_ob->send_run_ver;
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	memcpy((char *)(p_ob->p_send_buf), pd, size);
	p_ob->send_cnt = 0;
	p_ob->p_decode_fn = p_fn;
	d = (*(p_ob->p_decode_fn))(p_ob->decode_tmp, p_ob->p_send_buf, &(p_ob->send_cnt));
	p_ob->ir_xus = (d >> 8);
	khz = d & 0xFF;
	if ((p_ob->send_cnt == 0) || (p_ob->ir_xus == 0) || (khz < 30))
	{
		p_ob->send_run_flag = 0;
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->p_decode_fn error!\r\n");
		return -5;
	}
	(*(p_ob->p_hw_putd_before_fn))(khz);
	p_ob->ir_khz = khz;
	p_ob->send_sum = size;
	XT_IRSEND_TASK_UNLOCK(); //<<<<<<<<<<<<<<
	
	XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
	if ((p_ob->send_run_flag != 1)
	||  (p_ob->send_run_ver  != v)
	||  (p_ob->ir_num >= XT_IRSEND_SUM)
	||  (xt_p_irsend[p_ob->ir_num] != p_ob))
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->ir_num/xt_p_irsend error!\r\n");
		return -1;
	}
	d = (*(p_ob->p_decode_fn))(p_ob->decode_tmp, p_ob->p_send_buf, &(p_ob->send_cnt));
	if ((p_ob->send_cnt == 0) || (d <= 1))
	{
		p_ob->send_run_flag = 0;
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_send->p_decode_fn error!\r\n");
		return -5;
	}
	p_ob->send_run_flag = 2;
	if ((d & 0x0001) != 0)
	{
		(*(p_ob->p_hw_putd_a_fn))((uint32_t)(d & 0xFFFE) * (uint32_t)(p_ob->ir_xus), 0);
	}
	else
	{
		(*(p_ob->p_hw_putd_b_fn))((uint32_t)(d) * (uint32_t)(p_ob->ir_xus), 0);
	}
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
	return 1;
}

/**
  * @brief  获取-红外发送状态
  * @param  *p_ob      红外发送服务对象
  * @return ＜0:表示出错, ≥0:表示状态（0:没启动发送或前次已发完,1:准备发送,2:已启动发送）
  */
int xt_irsend_state(xt_irsend_obj_t *p_ob)
{
	XT_IRSEND_VARIAB()
	int i;
	
	XT_IRSEND_LOCKED(); //>>>>>>>>>>>>>>>
	if ((p_ob <= (xt_irsend_obj_t *)1)
	||  (p_ob->ir_num >= XT_IRSEND_SUM)
	||  (xt_p_irsend[p_ob->ir_num] != p_ob))
	{
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		xt_irsend_printf("irsend:xt_irsend_state->ir_num/xt_p_irsend error!\r\n");
		return -1;
	}
	else
	{
		i = p_ob->send_run_flag;
	XT_IRSEND_UNLOCK(); //<<<<<<<<<<<<<<<
		return i;
	}
}

/**
  * @brief  发送-〖方波〗(发送红外码)(在硬件层中断里调用)
  * @param  *p_ob      红外发送服务对象（★注意:由硬件层调用前保证合法性!!!）
  * @return void
  */
void xt_irsend_wave(xt_irsend_obj_t *p_ob)
{
	uint32_t i;
	
	if (p_ob->send_run_flag == 2)
	{
		if (p_ob->p_decode_fn == 0)
		{
			i = ++(p_ob->send_cnt);
			if (i >= p_ob->send_sum)
			{	//最后一信号发送之后
				(*(p_ob->p_hw_putd_end_fn))();
				p_ob->send_run_flag = 0;
				if (p_ob->p_irsend_complete_fn != 0) {
				(*(p_ob->p_irsend_complete_fn))(p_ob); }
				return;
			}
			if ((p_ob->p_send_buf[i] & 0x0001) != 0)
			{	//发送有载波信号
				if ((i + 1) < p_ob->send_sum)
				{
					(*(p_ob->p_hw_putd_a_fn))((uint32_t)(p_ob->p_send_buf[i] & 0xFFFE) * (uint32_t)(p_ob->ir_xus), 1);
				}
				else
				{
					if (p_ob->end_us == 0)
					{
						(*(p_ob->p_hw_putd_a_fn))((uint32_t)(p_ob->p_send_buf[i] & 0xFFFE) * (uint32_t)(p_ob->ir_xus), 2);
					}
					else
					{
						(*(p_ob->p_hw_putd_b_fn))(p_ob->end_us, 2); //强插入的结束间隔必定是无载波!!
					}
				}
			}
			else
			{	//发送无载波信号
				if ((i + 1) < p_ob->send_sum)
				{
					(*(p_ob->p_hw_putd_b_fn))((uint32_t)(p_ob->p_send_buf[i]) * (uint32_t)(p_ob->ir_xus), 1);
				}
				else
				{
					(*(p_ob->p_hw_putd_b_fn))((p_ob->end_us > 0) ? p_ob->end_us : (uint32_t)(p_ob->p_send_buf[i]) * (uint32_t)(p_ob->ir_xus), 2);
				}
			}
		}
		else //-------------------------------------------------------------------------------------------
		{
			if (p_ob->send_cnt == 0)
			{	//最后一信号发送之后
				ir_end:
				(*(p_ob->p_hw_putd_end_fn))();
				p_ob->send_run_flag = 0;
				if (p_ob->p_irsend_complete_fn != 0) {
				(*(p_ob->p_irsend_complete_fn))(p_ob); }
				return;
			}
			else if (p_ob->send_cnt == 0xFFFF)
			{	//强插入的结束间隔
				p_ob->send_cnt = 0;
				if (p_ob->end_us == 0) goto ir_end;
				(*(p_ob->p_hw_putd_b_fn))(p_ob->end_us, 2);
				return;
			}
			i = (*(p_ob->p_decode_fn))(p_ob->decode_tmp, p_ob->p_send_buf, &(p_ob->send_cnt));
			if ((p_ob->send_cnt == 0) || (i <= 1))
			{	//最后一信号发送之后
				(*(p_ob->p_hw_putd_end_fn))();
				p_ob->send_run_flag = 0;
				if (p_ob->p_irsend_complete_fn != 0) {
				(*(p_ob->p_irsend_complete_fn))(p_ob); }
				return;
			}
			if ((i & 0x0001) != 0)
			{	//发送有载波信号
				(*(p_ob->p_hw_putd_a_fn))((uint32_t)(i & 0xFFFE) * (uint32_t)(p_ob->ir_xus), 
				/**/                 ((p_ob->send_cnt == 0xFFFF) && (p_ob->end_us == 0)) ? 2 : 1);
			}
			else
			{	//发送无载波信号
				(*(p_ob->p_hw_putd_b_fn))((uint32_t)(i) * (uint32_t)(p_ob->ir_xus), 
				/**/                 ((p_ob->send_cnt == 0xFFFF) && (p_ob->end_us == 0)) ? 2 : 1);
			}
		}
	}
}

#endif  //#if (XT_APP_IRSEND_EN == XT_DEF_ENABLED)
