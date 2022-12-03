<!-- +++
author = "XT"
comments = false
date  = "2022-12-03"
draft = false
share = false
image = ""
menu  = ""
slug  = ""
title = "VOFA+ ��������ʾ����"
+++ -->

### һ������

<details close=""><summary>1�����ܼ��</summary>

������������ʾ����ģ��Ϊ�����ṩ���ô��ڴ����������ߵ� PC �������ʾ�����ṩ���ֽӿڷ��ͣ�һ·����·����·����ͨ�����ͣ��������Ƕ��������ߵķ�������ߵ���Ч�ʡ�

</details>

<details close=""><summary>2��ģ��ܹ�</summary>

![ģ��ܹ�](./img/20221203_1_01.png)

</details>

<details close=""><summary>3������Ҫ��</summary>

|  ����  |  Ҫ��  |
| :----- | :----- |
| ������� | ʵʱ����ϵͳ �� ������� ���� |
| Ӳ������ | STM8 �� STM32F103 ���������� MCU |
| �������� | ������xt_scom������ͨ�ŷ���ģ�� |

</details>

<details close=""><summary>4��������</summary>

��������Ŀʱ��������Ҫ�鿴�������ߡ����磺�鿴��Ƭ��ʹ���ʵ����ߡ��鿴 AD �����¶�ֵ�����ߡ��鿴���ת�ٵ����ߡ��� ��ô��û����ʾ����һ����ʾ����ͼ�Ĺ����أ���Ȼ���еģ�����Ϊ��ҽ���һ������ʾ������� VOFA+�����ؼӣ���VOFA+ ����ʾ������һ���������������ͨ�����ڡ�TCP��UDP ���͵��������������ͼ�εķ�ʽ���ڼ������Ļ����ʾ�����ʾ�������ߣ��������Ƕ����ݷ����������ز���ȱ�Ŀ������֡�VOFA+ ֧�� FireWater��JustFloat��RawData �������ݸ�ʽ��֧�ֶ�ͨ���Ĳ�����ʾ��֧�� X��Y ���������ż��ƶ���֧�ִ��뱣�沨�����ݡ�

? ���ν���ժ�ԡ�һ����һ�������� ? 

</details>

### ������ֲ

<details close=""><summary>1�����Դ�ļ�</summary>

��ģ��Դ�ļ����ļ�����·����ӵ����̣�ʾ����

![���Դ�ļ�������](./img/20221203_2_01.png)

</details>

<details close=""><summary>2�������ʼ����</summary>

��Ӧ�ó�ʼ�������м���ģ���ʼ����ʾ����

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

����˵��������������Զ���ʼ�ܹ��������ɺ�`XT_MSG_INIT_2_TAB_EXPORT(func,name)`�Զ����ú������г�ʼ����

</details>

<details close=""><summary>3������ͷ�ļ�</summary>

��ʹ��ģ���Ӧ�ó����м���ͷ�ļ�������ʾ����  

```c
#if (XT_APP_SCOMVOFA_EN == XT_DEF_ENABLED)
#include "xt_scom_vofa.h"
#endif
```

</details>

<details close=""><summary>4������ģ�����</summary>

����ʵ�ʵ�ʹ�û�������ģ�������ʾ����

![����ģ�����](./img/20221203_2_05.png)  

����˵�������ڱ�׼ģ���ǲ������û��޸ĵģ�������Ӧ��ʱ������ͷ�ļ�ӳ�䣬��ӳ��ͷ�ļ��޸����ã�

</details>

<details close=""><summary>5��ʹ��ģ�鿪��</summary>

�ڹ���ͷ�ļ���application.h��ͳһʹ��ģ�鿪�أ�ʾ����

```c
#define XT_DEF_DISABLED                 0                               /* ����ģ��                     */
#define XT_DEF_ENABLED                  1                               /* ʹ��ģ��                     */

#define XT_APP_DEBUG                    20221203                        /* ������ʾ                     */
#define __XT_SCOM_VOFA_REMAP_H
#ifndef XT_APP_SCOMVOFA_EN
#define XT_APP_SCOMVOFA_EN              XT_DEF_ENABLED                  /* ������������ʾ����(VOFA+)    */
#endif
#define __XT_SCOM_REMAP_H
#ifndef XT_APP_SCOM_EN
#define XT_APP_SCOM_EN                  XT_DEF_ENABLED                  /* ����ͨ�ŷ���ģ��             */
#endif
```

</details>

<details close=""><summary>6������ģ��˵��</summary>

��ģ�������ڴ���ͨ�ŷ���ģ�顾xt_scom������������ֲ��ο���˵���ĵ���

</details>

### ����ʹ��

<details close=""><summary>1��Ӧ��ʾ��</summary>

��ʾ��ֻ������򵥷�ʽչʾģ��Ļ���ʹ�ã�����һ��Ӧ��ʵ����

```c
#include "xt_scom_vofa.h"

//ֱ����Ӧ���е��ú������ͣ�������ʾ CPU ռ���ʣ�
xt_scomvofa_1ch_put(0x80, (float)(cpu10000_usage_get(1)));

//Ӧ��ʾ��[1]���ԡ���������ʽ��ֻ���� CH1 ͨ�����ݵ�����ʾ������ʾ
xt_scomvofa_1ch_put(0, 100);

//Ӧ��ʾ��[2]���ԡ�����������ʽ��ֻ���� CH1 ͨ�����ݵ�����ʾ������ʾ
xt_scomvofa_1ch_put(0x80, 100);

//Ӧ��ʾ��[3]���ԡ���������ʽͬʱ���� CH1��CH3 ͨ�����ݵ�����ʾ������ʾ
xt_scomvofa_4ch_put(0x01|0x04, 100, 0, -100, 0);

//Ӧ��ʾ��[4]���ԡ�����������ʽͬʱ���� CH1��CH3 ͨ�����ݵ�����ʾ������ʾ
xt_scomvofa_4ch_put(0x80|0x01|0x04, 100, 0, -100, 0);

//Ӧ��ʾ��[5]���ԡ���������ʽͬʱ���� CH1��CH2 ����ͨ�����ݵ�����ʾ������ʾ
xt_scomvofa_xch_put(2, chx2);                 //float chx2[2] = {-100, 100};

//Ӧ��ʾ��[6]���ԡ�����������ʽͬʱ���� CH1��CH2 ����ͨ�����ݵ�����ʾ������ʾ
xt_scomvofa_xch_put(0x80|2, chx2);            //float chx2[2] = {-100, 100};

```

</details>