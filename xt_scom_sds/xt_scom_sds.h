/**
  * @file  xt_scom_sds.h
  * @brief ������������ʾ������Visual Scope��Serial Digital Scope��
  * COPYRIGHT (C) 2022, XT ��������С��
  * Change Logs:
  * Date           Author       Notes
  * 2022-12-01     o2ospring    ԭʼ�汾
  */
#ifndef XT_SCOM_SDS_H__
#define XT_SCOM_SDS_H__

//########################################################################################################
#include "application.h" /////////////////// <- ���涨[application.h]��Ϊ��������������Դ����ͷ�ļ���   ##
#ifdef __XT_SCOM_SDS_REMAP_H /////////////// <- �����[application.h]����˺꣬������ͷ�ļ���ӳ�����   ##
#include "xt_scom_sds_remap.h" ///////////// <- ��������ӳ��,��ͷ�ļ�������ӳ�䵽���ж����ͷ�ļ�����   ##
#else //##################################################################################################

#include <stdint.h> //////////////////////// <- ʹ�õ����ݶ��壬��: int8_t, uint32_t ��
#include <string.h> //////////////////////// <- ʹ�õ��ַ���������: strcpy(), memcpy() ��
#include "rtthread.h" ////////////////////// <- ʹ��RT-Thread����ϵͳ
#include "xt_scom.h" /////////////////////// <- ��������ͨ�ŷ���ģ��!!!!!!!!!!!!!!!!!!!!!

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ �������� ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

#define XT_SCOMSDS_SCOM_NUM         0                                   //����ͨѶ����ͨ�����
#define XT_SCOMSDS_BAUD_RATE        9600  /*��VisualScope�������þ���*/ //ͨѶ������, ��:9600,115200
#define xt_scomsds_printf(...)      rt_kprintf(__VA_ARGS__)             //�쳣��Ϣ��ӡ��Ŀǰ��û�õ��ɱ���������������޸ı��к꼴��֧��C89�﷨��

#if (defined XT_SCOM_SDS_C__) && (XT_APP_SCOMSDS_EN == XT_DEF_ENABLED)
#define XT_SCOMSDS_CRC16(pd,sz)     xt_scomsds_crc16(pd,sz)                              //У��-CRC16
struct rt_mutex                     xt_scomsds_mutex;                                    //ȫ��-����������
#define XT_SCOMSDS_MUTEX_INIT()     rt_mutex_init(&xt_scomsds_mutex, "xt_scomsds_mutex", RT_IPC_FLAG_FIFO)
#define XT_SCOMSDS_MUTEX_LOCKED()   rt_mutex_take(&xt_scomsds_mutex, RT_WAITING_FOREVER) //����-����ʹ��
#define XT_SCOMSDS_MUTEX_UNLOCK()   rt_mutex_release(&xt_scomsds_mutex)                  //����-�����˳�
struct rt_semaphore                 xt_scomsds_sem;                                      //ȫ��-�ź�������
#define XT_SCOMSDS_TX_SEM_INIT()    rt_sem_init(&xt_scomsds_sem, "xt_scomsds_sem", 0, RT_IPC_FLAG_FIFO)
#define XT_SCOMSDS_TX_SEM_RESET()   rt_sem_control(&xt_scomsds_sem, RT_IPC_CMD_RESET, 0) //����-�����ź�
#define XT_SCOMSDS_TX_SEM_TAKE()    rt_sem_take(&xt_scomsds_sem, RT_WAITING_FOREVER)     //����-�ȴ����
#define XT_SCOMSDS_TX_SEM_SEND()    rt_sem_release(&xt_scomsds_sem)                      //����-��ɴ���
#endif

/********************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++ �������� ++++++++++++++++++++++++++++++++++++++++++++++*/
/********************************************************************************************************/

extern void xt_scomsds_init(void);
extern int xt_scomsds_1ch_put(uint8_t ch_n, int16_t chx);
extern int xt_scomsds_4ch_put(uint8_t ch_f, int16_t ch1, int16_t ch2, int16_t ch3, int16_t ch4);

#ifdef __cplusplus
	}
#endif

#endif  //#ifdef __XT_SCOM_SDS_REMAP_H

#endif  //#ifndef XT_SCOM_SDS_H__