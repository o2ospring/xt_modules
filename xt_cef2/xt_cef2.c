/**
  * @file  xt_cef2.c
  * @brief �����¼���־����ģ�飨Control Event Flag��
  * COPYRIGHT (C) 2022, XT ��������С��
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-05     o2ospring    ԭʼ�汾
  */
#define   XT_CEF2_C__
#include "xt_cef2.h"
#if (defined(XT_APP_CEF2_EN) && (XT_APP_CEF2_EN == XT_DEF_ENABLED))
#ifdef    XT_CEF2_X__
#undef    XT_CEF2_H__
#include "xt_cef2.h"
#endif

void xt_cef2_init(uint8_t *f, uint8_t size);                         //��ȫ��00��
void xt_cef2_reset_flag(uint8_t *f, uint8_t size, uint8_t i);        //��XX��00��
void xt_cef2_set_new_flag(uint8_t *f, uint8_t size, uint8_t i);      //��XX��1X��
uint8_t xt_cef2_is_goto_run(uint8_t *f, uint8_t size, uint8_t i);    //��10��01��
uint8_t xt_cef2_set_end_flag(uint8_t *f, uint8_t size, uint8_t i);   //��X1��X0��
uint8_t xt_cef2_set_retry_flag(uint8_t *f, uint8_t size, uint8_t i); //��X1��10��
uint8_t xt_cef2_is_new(uint8_t *f, uint8_t size, uint8_t i);         //��1X��
uint8_t xt_cef2_is_run(uint8_t *f, uint8_t size, uint8_t i);         //��X1��
uint8_t xt_cef2_is_nul(uint8_t *f, uint8_t size, uint8_t i);         //��00��
uint8_t xt_cef2_is_any_new(uint8_t *f, uint8_t size);                //����һ1X��
uint8_t xt_cef2_is_any_run(uint8_t *f, uint8_t size);                //����һX1��
uint8_t xt_cef2_is_any_tag(uint8_t *f, uint8_t size);                //����ȫ00��

/**
  * @brief ��ȫ��00������[����]���������־����ʼ����
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ�
  * @return void
  */
void xt_cef2_init(uint8_t *f, uint8_t size) //��ȫ��00��
{
	XT_CEF2_VARIAB()
	XT_CEF2_LOCKED(); //>>>>>>>>>>
	memset((char *)(f), 0, size);
	XT_CEF2_UNLOCK(); //<<<<<<<<<<
}

/**
  * @brief ��XX��00������[����]���������־
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ����ر�0->������ڴ�Խ�磩
  * @param  i        �ڼ��ֱ�־��0:��1�ֱ�־,1:��2��,����ע:ÿ��ռ�� 2 bit��
  * @return void
  */
void xt_cef2_reset_flag(uint8_t *f, uint8_t size, uint8_t i) //��XX��00��
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
  * @brief ��XX��1X��[����]������λ���µĿ��ƣ�
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ����ر�0->������ڴ�Խ�磩
  * @param  i        �ڼ��ֱ�־��0:��1�ֱ�־,1:��2��,����ע:ÿ��ռ�� 2 bit��
  * @return void
  */
void xt_cef2_set_new_flag(uint8_t *f, uint8_t size, uint8_t i) //��XX��1X��
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
  * @brief ��10��01��[����]�����λ��ִ�п��ƣ�
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ����ر�0->������ڴ�Խ�磩
  * @param  i        �ڼ��ֱ�־��0:��1�ֱ�־,1:��2��,����ע:ÿ��ռ�� 2 bit��
  * @return 0:�����¼���1:�����¼�������ϲ�Ӧ�ó������Ҫִ�ж�����
  */
uint8_t xt_cef2_is_goto_run(uint8_t *f, uint8_t size, uint8_t i) //��10��01��
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
  * @brief ��X1��X0��[����]�����ɣ�������ɣ�
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ����ر�0->������ڴ�Խ�磩
  * @param  i        �ڼ��ֱ�־��0:��1�ֱ�־,1:��2��,����ע:ÿ��ռ�� 2 bit��
  * @return 0:����ɣ�1:�����
  */
uint8_t xt_cef2_set_end_flag(uint8_t *f, uint8_t size, uint8_t i) //��X1��X0��
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
  * @brief ��X1��10��[����]������ã��������ã�
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ����ر�0->������ڴ�Խ�磩
  * @param  i        �ڼ��ֱ�־��0:��1�ֱ�־,1:��2��,����ע:ÿ��ռ�� 2 bit��
  * @return 0:�����ã�1:������
  */
uint8_t xt_cef2_set_retry_flag(uint8_t *f, uint8_t size, uint8_t i) //��X1��10��
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
  * @brief ��1X����չ���ܣ��ж�[����]�����Ƿ���Ч
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ����ر�0->������ڴ�Խ�磩
  * @param  i        �ڼ��ֱ�־��0:��1�ֱ�־,1:��2��,����ע:ÿ��ռ�� 2 bit��
  * @return 0:�����¼���1:�����¼�
  */
uint8_t xt_cef2_is_new(uint8_t *f, uint8_t size, uint8_t i) //��1X��
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
  * @brief ��X1����չ���ܣ��ж�[����]����Ƿ���Ч
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ����ر�0->������ڴ�Խ�磩
  * @param  i        �ڼ��ֱ�־��0:��1�ֱ�־,1:��2��,����ע:ÿ��ռ�� 2 bit��
  * @return 0:û���У�1:������
  */
uint8_t xt_cef2_is_run(uint8_t *f, uint8_t size, uint8_t i) //��X1��
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
  * @brief ��00����չ���ܣ��ж�[����]��������Ƿ�[��Ч]
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ����ر�0->������ڴ�Խ�磩
  * @param  i        �ڼ��ֱ�־��0:��1�ֱ�־,1:��2��,����ע:ÿ��ռ�� 2 bit��
  * @return 0:�б�־��1:�ޱ�־
  */
uint8_t xt_cef2_is_nul(uint8_t *f, uint8_t size, uint8_t i) //��00��
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
  * @brief ����һ1X����չ���ܣ��ж�[��һ]�����Ƿ���Ч
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ�
  * @return 0:�����¼�����1:�����¼�����ע:�����ڶ�λ���ֽ��б�־��
  */
uint8_t xt_cef2_is_any_new(uint8_t *f, uint8_t size) //����һ1X��
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
  * @brief ����һX1����չ���ܣ��ж�[��һ]����Ƿ���Ч
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ�
  * @return 0:û���У���1:�����У���ע:�����ڶ�λ���ֽ��б�־��
  */
uint8_t xt_cef2_is_any_run(uint8_t *f, uint8_t size) //����һX1��
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
  * @brief ����ȫ00����չ���ܣ��ж�[��һ]��������Ƿ���Ч
  * @param  *f       ��־�Ĵ�����һ�ζ��ֽ����飩
  * @param  size     ��־�Ĵ�����С���ֽڣ�
  * @return 0:�ޱ�־����1:�б�־����ע:�����ڶ�λ���ֽ��б�־��
  */
uint8_t xt_cef2_is_any_tag(uint8_t *f, uint8_t size) //����ȫ00��
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
