/**
  * @file  xt_cef2.c
  * @brief 控制事件标志服务模块（Control Event Flag）
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-05     o2ospring    原始版本
  */
#define   XT_CEF2_C__
#include "xt_cef2.h"
#if (defined(XT_APP_CEF2_EN) && (XT_APP_CEF2_EN == XT_DEF_ENABLED))
#ifdef    XT_CEF2_X__
#undef    XT_CEF2_H__
#include "xt_cef2.h"
#endif

void xt_cef2_init(uint8_t *f, uint8_t size);                         //（全清00）
void xt_cef2_reset_flag(uint8_t *f, uint8_t size, uint8_t i);        //（XX→00）
void xt_cef2_set_new_flag(uint8_t *f, uint8_t size, uint8_t i);      //（XX→1X）
uint8_t xt_cef2_is_goto_run(uint8_t *f, uint8_t size, uint8_t i);    //（10→01）
uint8_t xt_cef2_set_end_flag(uint8_t *f, uint8_t size, uint8_t i);   //（X1→X0）
uint8_t xt_cef2_set_retry_flag(uint8_t *f, uint8_t size, uint8_t i); //（X1→10）
uint8_t xt_cef2_is_new(uint8_t *f, uint8_t size, uint8_t i);         //（1X）
uint8_t xt_cef2_is_run(uint8_t *f, uint8_t size, uint8_t i);         //（X1）
uint8_t xt_cef2_is_nul(uint8_t *f, uint8_t size, uint8_t i);         //（00）
uint8_t xt_cef2_is_any_new(uint8_t *f, uint8_t size);                //（任一1X）
uint8_t xt_cef2_is_any_run(uint8_t *f, uint8_t size);                //（任一X1）
uint8_t xt_cef2_is_any_tag(uint8_t *f, uint8_t size);                //（非全00）

/**
  * @brief （全清00）重置[所有]输入输出标志（初始化）
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）
  * @return void
  */
void xt_cef2_init(uint8_t *f, uint8_t size) //（全清00）
{
	XT_CEF2_VARIAB()
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	memset((char *)(f), 0, size);
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
}

/**
  * @brief （XX→00）重置[单个]输入输出标志
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）（特别：0->不检测内存越界）
  * @param  i        第几种标志（0:第1种标志,1:第2种,…。注:每种占用 2 bit）
  * @return void
  */
void xt_cef2_reset_flag(uint8_t *f, uint8_t size, uint8_t i) //（XX→00）
{
	XT_CEF2_VARIAB()
	if ((size > 0)
	&&  (size < ((i>>2) + 1)))
	{
		return;
	}
	i <<= 1;
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	f[i/8] &= ~(0x03<<(i%8));
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
}

/**
  * @brief （XX→1X）[单个]输入置位（新的控制）
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）（特别：0->不检测内存越界）
  * @param  i        第几种标志（0:第1种标志,1:第2种,…。注:每种占用 2 bit）
  * @return void
  */
void xt_cef2_set_new_flag(uint8_t *f, uint8_t size, uint8_t i) //（XX→1X）
{
	XT_CEF2_VARIAB()
	if ((size > 0)
	&&  (size < ((i>>2) + 1)))
	{
		return;
	}
	i <<= 1;
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	f[i/8] |= (0x02<<(i%8));
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
}

/**
  * @brief （10→01）[单个]输出置位（执行控制）
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）（特别：0->不检测内存越界）
  * @param  i        第几种标志（0:第1种标志,1:第2种,…。注:每种占用 2 bit）
  * @return 0:无新事件，1:有新事件（随后上层应用程序必须要执行动作）
  */
uint8_t xt_cef2_is_goto_run(uint8_t *f, uint8_t size, uint8_t i) //（10→01）
{
	XT_CEF2_VARIAB()
	if ((size > 0)
	&&  (size < ((i>>2) + 1)))
	{
		return 0;
	}
	i <<= 1;
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	if (((f[i/8] >> (i%8)) & 0x03) == 0x02)
	{
		f[i/8] ^= (0x03<<(i%8));
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 1;
	}
	else
	{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 0;
	}
}

/**
  * @brief （X1→X0）[单个]输出完成（控制完成）
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）（特别：0->不检测内存越界）
  * @param  i        第几种标志（0:第1种标志,1:第2种,…。注:每种占用 2 bit）
  * @return 0:无完成，1:有完成
  */
uint8_t xt_cef2_set_end_flag(uint8_t *f, uint8_t size, uint8_t i) //（X1→X0）
{
	XT_CEF2_VARIAB()
	if ((size > 0)
	&&  (size < ((i>>2) + 1)))
	{
		return 0;
	}
	i <<= 1;
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	if ((f[i/8] & (0x01<<(i%8))) != 0x00)
	{
		f[i/8] &= ~(0x01<<(i%8));
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 1;
	}
	else
	{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 0;
	}
}

/**
  * @brief （X1→10）[单个]输出重置（控制重置）
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）（特别：0->不检测内存越界）
  * @param  i        第几种标志（0:第1种标志,1:第2种,…。注:每种占用 2 bit）
  * @return 0:无重置，1:有重置
  */
uint8_t xt_cef2_set_retry_flag(uint8_t *f, uint8_t size, uint8_t i) //（X1→10）
{
	XT_CEF2_VARIAB()
	if ((size > 0)
	&&  (size < ((i>>2) + 1)))
	{
		return 0;
	}
	i <<= 1;
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	if ((f[i/8] & (0x01<<(i%8))) != 0x00)
	{
		f[i/8] &= ~(0x01<<(i%8));
		f[i/8] |=  (0x02<<(i%8));
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 1;
	}
	else
	{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 0;
	}
}

/**
  * @brief （1X）扩展功能：判断[单个]输入是否有效
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）（特别：0->不检测内存越界）
  * @param  i        第几种标志（0:第1种标志,1:第2种,…。注:每种占用 2 bit）
  * @return 0:无新事件，1:有新事件
  */
uint8_t xt_cef2_is_new(uint8_t *f, uint8_t size, uint8_t i) //（1X）
{
	XT_CEF2_VARIAB()
	if ((size > 0)
	&&  (size < ((i>>2) + 1)))
	{
		return 0;
	}
	i <<= 1;
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	if ((f[i/8] & (0x02<<(i%8))) != 0x00)
	{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 1;
	}
	else
	{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 0;
	}
}

/**
  * @brief （X1）扩展功能：判断[单个]输出是否有效
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）（特别：0->不检测内存越界）
  * @param  i        第几种标志（0:第1种标志,1:第2种,…。注:每种占用 2 bit）
  * @return 0:没运行，1:有运行
  */
uint8_t xt_cef2_is_run(uint8_t *f, uint8_t size, uint8_t i) //（X1）
{
	XT_CEF2_VARIAB()
	if ((size > 0)
	&&  (size < ((i>>2) + 1)))
	{
		return 0;
	}
	i <<= 1;
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	if ((f[i/8] & (0x01<<(i%8))) != 0x00)
	{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 1;
	}
	else
	{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 0;
	}
}

/**
  * @brief （00）扩展功能：判断[单个]输入输出是否都[无效]
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）（特别：0->不检测内存越界）
  * @param  i        第几种标志（0:第1种标志,1:第2种,…。注:每种占用 2 bit）
  * @return 0:有标志，1:无标志
  */
uint8_t xt_cef2_is_nul(uint8_t *f, uint8_t size, uint8_t i) //（00）
{
	XT_CEF2_VARIAB()
	if ((size > 0)
	&&  (size < ((i>>2) + 1)))
	{
		return 1;
	}
	i <<= 1;
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	if (((f[i/8] >> (i%8)) & 0x03) == 0x00)
	{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 1;
	}
	else
	{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
		return 0;
	}
}

/**
  * @brief （任一1X）扩展功能：判断[任一]输入是否有效
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）
  * @return 0:无新事件，≥1:有新事件（备注:可用于定位哪字节有标志）
  */
uint8_t xt_cef2_is_any_new(uint8_t *f, uint8_t size) //（任一1X）
{
	XT_CEF2_VARIAB()
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	while (size--)
	{
		if ((*f & 0xAA) != 0)
		{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
			return size + 1;
		}
		f++;
	}
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
	return 0;
}

/**
  * @brief （任一X1）扩展功能：判断[任一]输出是否有效
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）
  * @return 0:没运行，≥1:有运行（备注:可用于定位哪字节有标志）
  */
uint8_t xt_cef2_is_any_run(uint8_t *f, uint8_t size) //（任一X1）
{
	XT_CEF2_VARIAB()
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	while (size--)
	{
		if ((*f & 0x55) != 0)
		{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
			return size + 1;
		}
		f++;
	}
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
	return 0;
}

/**
  * @brief （非全00）扩展功能：判断[任一]输入输出是否有效
  * @param  *f       标志寄存器（一段多字节数组）
  * @param  size     标志寄存器大小（字节）
  * @return 0:无标志，≥1:有标志（备注:可用于定位哪字节有标志）
  */
uint8_t xt_cef2_is_any_tag(uint8_t *f, uint8_t size) //（非全00）
{
	XT_CEF2_VARIAB()
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	while (size--)
	{
		if (*f != 0)
		{
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
			return size + 1;
		}
		f++;
	}
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
	return 0;
}

#endif  //#if (XT_APP_CEF2_EN == XT_DEF_ENABLED)
