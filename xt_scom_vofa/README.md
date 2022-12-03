<!-- +++
author = "XT"
comments = false
date  = "2022-12-03"
draft = false
share = false
image = ""
menu  = ""
slug  = ""
title = "VOFA+ 虚拟数字示波器"
+++ -->

### 一、构架

<details close=""><summary>1、功能简介</summary>

串口虚拟数字示波器模块为我们提供利用串口传输数字曲线到 PC 软件上显示，它提供三种接口发送：一路、多路、多路连续通道发送，方便我们对数据曲线的分析，提高调试效率。

</details>

<details close=""><summary>2、模块架构</summary>

![模块架构](./img/20221203_1_01.png)

</details>

<details close=""><summary>3、环境要求</summary>

|  环境  |  要求  |
| :----- | :----- |
| 软件环境 | 实时操作系统 或 裸机环境 均可 |
| 硬件环境 | STM8 或 STM32F103 及更高性能 MCU |
| 依赖环境 | 依赖【xt_scom】串行通信服务模块 |

</details>

<details close=""><summary>4、软件简介</summary>

我们做项目时，往往需要查看数据曲线。例如：查看单片机使用率的曲线、查看 AD 读出温度值的曲线、查看电机转速的曲线…… 那么有没有像示波器一样显示曲线图的工具呢？当然是有的，今天为大家介绍一款虚拟示波器软件 VOFA+（伏特加）。VOFA+ 虚拟示波器是一个电脑软件，它将通过串口、TCP、UDP 传送到计算机的数据以图形的方式，在计算机屏幕上显示虚拟的示波器曲线，方便我们对数据分析，是您必不可缺的开发助手。VOFA+ 支持 FireWater、JustFloat、RawData 三种数据格式，支持多通道的波形显示，支持 X、Y 轴任意缩放及移动，支持打开与保存波形数据。

? 本段介绍摘自《一春又一春》博文 ? 

</details>

### 二、移植

<details close=""><summary>1、添加源文件</summary>

将模块源文件、文件包含路径添加到工程，示例：

![添加源文件到工程](./img/20221203_2_01.png)

</details>

<details close=""><summary>2、加入初始函数</summary>

在应用初始化函数中加入模块初始化，示例：

```c
void rt_init_thread_entry(void *p_arg)
{
	.
	.
	#if (XT_APP_SCOMVOFA_EN == XT_DEF_ENABLED)
	xt_scomvofa_init();
	#endif
	.
	.
}
```

补充说明：如果工程有自动初始架构，可以由宏`XT_MSG_INIT_2_TAB_EXPORT(func,name)`自动调用函数进行初始化！

</details>

<details close=""><summary>3、包含头文件</summary>

在使用模块的应用程序中加入头文件包含，示例：  

```c
#if (XT_APP_SCOMVOFA_EN == XT_DEF_ENABLED)
#include "xt_scom_vofa.h"
#endif
```

</details>

<details close=""><summary>4、配置模块参数</summary>

根据实际的使用环境配置模块参数，示例：

![配置模块参数](./img/20221203_2_05.png)  

补充说明：由于标准模块是不允许用户修改的，所以在应用时请启用头文件映射，在映射头文件修改配置！

</details>

<details close=""><summary>5、使能模块开关</summary>

在工程头文件《application.h》统一使能模块开关，示例：

```c
#define XT_DEF_DISABLED                 0                               /* 禁用模块                     */
#define XT_DEF_ENABLED                  1                               /* 使能模块                     */

#define XT_APP_DEBUG                    20221203                        /* 开启演示                     */
#define __XT_SCOM_VOFA_REMAP_H
#ifndef XT_APP_SCOMVOFA_EN
#define XT_APP_SCOMVOFA_EN              XT_DEF_ENABLED                  /* 串口虚拟数字示波器(VOFA+)    */
#endif
#define __XT_SCOM_REMAP_H
#ifndef XT_APP_SCOM_EN
#define XT_APP_SCOM_EN                  XT_DEF_ENABLED                  /* 串行通信服务模块             */
#endif
```

</details>

<details close=""><summary>6、依赖模块说明</summary>

本模块依赖于串行通信服务模块【xt_scom】，关于其移植请参考它说明文档！

</details>

### 三、使用

<details close=""><summary>1、应用示例</summary>

本示范只是以最简单方式展示模块的基本使用，并非一个应用实例！

```c
#include "xt_scom_vofa.h"

//直接在应用中调用函数发送，例如显示 CPU 占有率：
xt_scomvofa_1ch_put(0x80, (float)(cpu10000_usage_get(1)));

//应用示例[1]：以【阻塞】方式及只发送 CH1 通道数据到虚拟示波器显示
xt_scomvofa_1ch_put(0, 100);

//应用示例[2]：以【非阻塞】方式及只发送 CH1 通道数据到虚拟示波器显示
xt_scomvofa_1ch_put(0x80, 100);

//应用示例[3]：以【阻塞】方式同时发送 CH1、CH3 通道数据到虚拟示波器显示
xt_scomvofa_4ch_put(0x01|0x04, 100, 0, -100, 0);

//应用示例[4]：以【非阻塞】方式同时发送 CH1、CH3 通道数据到虚拟示波器显示
xt_scomvofa_4ch_put(0x80|0x01|0x04, 100, 0, -100, 0);

//应用示例[5]：以【阻塞】方式同时发送 CH1、CH2 连续通道数据到虚拟示波器显示
xt_scomvofa_xch_put(2, chx2);                 //float chx2[2] = {-100, 100};

//应用示例[6]：以【非阻塞】方式同时发送 CH1、CH2 连续通道数据到虚拟示波器显示
xt_scomvofa_xch_put(0x80|2, chx2);            //float chx2[2] = {-100, 100};

```

</details>