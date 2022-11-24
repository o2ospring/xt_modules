/**
  * @file  xt_comdef.h
  * @brief 常用宏定义集合
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-11-08     o2ospring    原始版本
  */
#ifndef XT_COMDEF_H__
#define XT_COMDEF_H__

#include <stdint.h> //////////////////////// <-使用的数据定义，如: int8_t, uint32_t 等

#ifdef __cplusplus
extern "C" {
#endif

//【标准数据类型】----------------------------------------------------------------------------------------

#if 0
typedef unsigned long int uint32_t;          /* unsigned 32 bit value */
typedef unsigned short    uint16_t;          /* unsigned 16 bit value */
typedef unsigned char     uint8_t;           /* unsigned 8  bit value */
typedef signed long int   int32_t;           /* signed 32 bit value */
typedef signed short      int16_t;           /* signed 16 bit value */
typedef signed char       int8_t;            /* signed 8  bit value */
#endif

//【编译器识别宏】----------------------------------------------------------------------------------------

// 1、IAR-ARM 编译器识别宏（由 IARSystems 公司推出，应用的 IDE 有：IAR-EWARM）
#if   defined(__ICCARM__)
// 2、ARM-CC 编译器识别宏（由 ARM 公司推出，应用的 IDE 有：Keil-MDK）(即将淘汰)
#elif defined(__CC_ARM)
// 3、LLVM-clang 编译器识别宏（由 Apple 公司主导，应用的 IDE 有：Keil-MDK 等）
#elif defined(__clang__)
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) //Keil-MDK
// 4、GNU-gcc 编译器识别宏（由 GNU 推出，应用的 IDE 有：Eclipse、VScode 等等）
#elif defined(__GNUC__)
// 5、Microsoft-MSC 编译器识别宏（由 微软 推出，应用的 IDE 有：Visual Studio）
#elif defined (_MSC_VER)
#endif

//【编译属性】--------------------------------------------------------------------------------------------

// 1、IAR-ARM 编译器识别宏（由 IARSystems 公司推出，应用的 IDE 有：IAR-EWARM）
#if   defined(__ICCARM__)

#define XT_PRAGMA(x)                         _Pragma(#x)
#define XT_SECTION(x)                        @ x                             //数据拼接　（对象名称后明声明）
#define XT_AT(a)                             @ a                             //地址指定　（对象名称后明声明）
#define XT_PACKED                            ???/*无法兼容*/                 //字节对齐　（对象名称后明声明，强烈建议改用 #pragma pack(push, 1) ... #pragma pack(pop) 的兼容性更好）
#define XT_ALIGN(n)                          XT_PRAGMA(data_alignment=n)     //地址对齐　（对象整体最前声明，等效于伪指令 #pragma location=地址 ... 注意：由[static]修饰的变量，[_Pragma]必须放在[static]之前，否则[_Pragma]不生效）
#define XT_UNUSED                            /*无法兼容,影响不大,忽略即可*/  //未用不警告（对象整体最前声明）
#define XT_USED                              __root                          //未用不优化（对象整体最前声明）
#define XT_WEAK                              __weak                          //弱化对象　（对象整体最前声明）
#define XT_INLINE                            static inline                   //内联函数　（对象整体最前声明，c/h文件中直接编写函数(体)，不能外部声明）

// 2、ARM-CC 编译器识别宏（由 ARM 公司推出，应用的 IDE 有：Keil-MDK）(即将淘汰)
#elif defined(__CC_ARM)

#define XT_SECTION(x)                        __attribute__((__section__(x))) //数据拼接　（对象名称后明声明）
#define XT_AT(a)                             __attribute__((__at__(a)))      //地址指定　（对象名称后明声明）
#define XT_PACKED                            __attribute__((__packed__))     //字节对齐　（对象名称后明声明，强烈建议改用 #pragma pack(push, 1) ... #pragma pack(pop) 的兼容性更好）
#define XT_ALIGN(n)                          __attribute__((__aligned__(n))) //地址对齐　（对象整体最前声明）
#define XT_UNUSED                            __attribute__((__unused__))     //未用不警告（对象整体最前声明）
#define XT_USED                              __attribute__((__used__))       //未用不优化（对象整体最前声明）
#define XT_WEAK                              __attribute__((__weak__))       //弱化对象　（对象整体最前声明）
#define XT_INLINE                            static __inline                 //内联函数　（对象整体最前声明，c/h文件中直接编写函数(体)，不能外部声明）

// 3、LLVM-clang 编译器识别宏（由 Apple 公司主导，应用的 IDE 有：Keil-MDK 等）
#elif defined(__clang__) \
||   (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))

#define XT_SECTION(x)                        __attribute__((__section__(x))) //数据拼接　（对象名称后明声明）
#define XT_AT(a)                             __attribute__((__at__(a)))      //地址指定　（对象名称后明声明）
#define XT_PACKED                            __attribute__((__packed__))     //字节对齐　（对象名称后明声明，强烈建议改用 #pragma pack(push, 1) ... #pragma pack(pop) 的兼容性更好）
#define XT_ALIGN(n)                          __attribute__((__aligned__(n))) //地址对齐　（对象整体最前声明）
#define XT_UNUSED                            __attribute__((__unused__))     //未用不警告（对象整体最前声明）
#define XT_USED                              __attribute__((__used__))       //未用不优化（对象整体最前声明）
#define XT_WEAK                              __attribute__((__weak__))       //弱化对象　（对象整体最前声明）
#define XT_INLINE                            static __inline                 //内联函数　（对象整体最前声明，c/h文件中直接编写函数(体)，不能外部声明）

// 4、GNU-gcc 编译器识别宏（由 GNU 推出，应用的 IDE 有：Eclipse、VScode 等等）
#elif defined(__GNUC__)

#define XT_SECTION(x)                        __attribute__((__section__(x))) //数据拼接　（对象名称后明声明）
#define XT_AT(a)                             __attribute__((__at__(a)))      //地址指定　（对象名称后明声明）
#define XT_PACKED                            __attribute__((__packed__))     //字节对齐　（对象名称后明声明，强烈建议改用 #pragma pack(push, 1) ... #pragma pack(pop) 的兼容性更好）
#define XT_ALIGN(n)                          __attribute__((__aligned__(n))) //地址对齐　（对象整体最前声明）
#define XT_UNUSED                            __attribute__((__unused__))     //未用不警告（对象整体最前声明）
#define XT_USED                              __attribute__((__used__))       //未用不优化（对象整体最前声明）
#define XT_WEAK                              __attribute__((__weak__))       //弱化对象　（对象整体最前声明）
#define XT_INLINE                            static __inline                 //内联函数　（对象整体最前声明，c/h文件中直接编写函数(体)，不能外部声明）

#endif

//【位操作】----------------------------------------------------------------------------------------------

//生成第n位为1的立即数(n=0表示bit0)
#define XT_1BIT_GEN(n)                       (1UL<<(n))

//生成第n位为0的立即数(n=0表示bit0)
#define XT_0BIT_GEN(n)                       (~(1UL<<(n)))

//生成第n~m位为1的立即数((n=0表示bit0)
#define XT_1BITS_GEN(n,m)                    (~(((~0UL)>>(32-((m)-(n)+1)))<<(n)))

//生成第n~m位为0的立即数(n=0表示bit0)
#define XT_0BITS_GEN(n,m)                    (((~0UL)>>(32-((m)-(n)+1)))<<(n))

//将变量数x的第n位置位(n=0表示bit0)
#define XT_BIT_SET(x,n)                      ((x) | (1UL<<(n)))

//将变量x的第n位清零(n=0表示bit0)
#define XT_BIT_CLR(x,n)                      ((x) & (~(1UL<<(n))))

//将变量x的第n~m位置位(n=0表示bit0)
#define XT_BITS_SET(x,n,m)                   ((x) | (((~0UL)>>(32-((m)-(n)+1)))<<(n)))

//将变量x的第n~m位清零(n=0表示bit0)
#define XT_BITS_CLR(x,n,m)                   ((x) & (~(((~0UL)>>(32-((m)-(n)+1)))<<(n))))

//获取变量x的第n~m位数(n=0表示bit0)
#define XT_BITS_GET(x,n,m)                   ((x) & (((~0UL)>>(32-((m)-(n)+1)))<<(n)))

//获取变量x的第n~m位并转成低位数(n=0表示bit0)
#define XT_BITS_GET2(x,n,m)                  (((x) & (((~0UL)>>(32-((m)-(n)+1)))<<(n))) >> (n))

//【结构体运算】------------------------------------------------------------------------------------------

//算出结构体某个成员占的空间大小             (使用例子: XT_STRUCT_MB_SZ(dev_obj_t, name/*不能有[0]*/))
#define XT_STRUCT_MB_SZ(s,m)                 (((uint32_t)(sizeof(((s *)0)->m))))

//算出结构体某个成员的前面成员占的空间大小   (使用例子: XT_STRUCT_100PART_SZ(dev_obj_t, name))
#define XT_STRUCT_100PART_SZ(s,m)            (((uint32_t)(&(((s *)0)->m))))

//算出结构体某个成员和前面成员占的空间大小   (使用例子: XT_STRUCT_110PART_SZ(dev_obj_t, name/*不能有[0]*/))
#define XT_STRUCT_110PART_SZ(s,m)            (((uint32_t)(&(((s *)0)->m))) + ((uint32_t)(sizeof(((s *)0)->m))))

//算出结构体某个成员的后面成员占的空间大小   (使用例子: XT_STRUCT_001PART_SZ(dev_obj_t, name/*不能有[0]*/))
#define XT_STRUCT_001PART_SZ(s,m)            (((uint32_t)(sizeof(s))) - ((uint32_t)(&(((s *)0)->m))) - ((uint32_t)(sizeof(((s *)0)->m))))

//算出结构体某个成员和后面成员占的空间大小   (使用例子: XT_STRUCT_011PART_SZ(dev_obj_t, name))
#define XT_STRUCT_011PART_SZ(s,m)            (((uint32_t)(sizeof(s))) - ((uint32_t)(&(((s *)0)->m))))

//算出结构体某个成员在结构体中的位置偏移量   (使用例子: XT_STRUCT_MB_OFFSET(dev_obj_t, name))
#define XT_STRUCT_MB_OFFSET(s,m)             (((uint32_t)(&(((s *)0)->m))))

//算出结构体某个成员后一个成员的位置偏移量   (使用例子: XT_STRUCT_NX_OFFSET(dev_obj_t, name/*不能有[0]*/))
#define XT_STRUCT_NX_OFFSET(s,m)             (((uint32_t)(&(((s *)0)->m))) + ((uint32_t)(sizeof(((s *)0)->m))))

//【数据转换】--------------------------------------------------------------------------------------------

//十进制转换为BCD码
#define XT_DEC_TO_BCD(dec)                   ((((uint8_t)(dec)/10U)<<4)|((uint8_t)(dec)%10U))

//BCD码转换为十进制
#define XT_BCD_TO_DEC(bcd)                   ((((uint8_t)(bcd)>>4)*10U)+((uint8_t)(bcd)&0xF))

//将一个字母转换为大写
#define XT_FS_TOUPPER(c)                     (((c)>='a'&&(c)<='z')?((c)-0x20):(c))

//将一个字母转换为小写
#define XT_FS_TOLOWER(c)                     (((c)>='A'&&(c)<='Z')?((c)+0x20):(c))

//大小端模式转换
#define XT_SWAP16(x)                         (((x)<<8)&0xFF00) | (((x)>>8)&0xFF)

//大小端模式转换
#define XT_SWAP32(x)                         ((((uint32_t)(x)>>24)&0xFF)    \
                                             |(((uint32_t)(x)>> 8)&0xFF00)  \
                                             |(((uint32_t)(x)<< 8)&0xFF0000)\
                                             |(((uint32_t)(x)<<24)&0xFF000000))
//交换a、b值（要求相同数据类型）
#define XT_SWAP(a,b)                         (a) = (a)^(b); \
                                             (b) = (a)^(b); \
                                             (a) = (a)^(b)

//二进制书写格式数据转换                     (使用例子: XT_0XBIN_TO_HEX16(0x00001111,0x11110000))
#define XT_0XBIN_TO_HEX8(n)                  (((n>>(28-7))&0x80)|((n>>(24-6))&0x40)|((n>>(20-5))&0x20)|((n>>(16-4))&0x10)|\
                                              ((n>>(12-3))&0x08)|((n>>( 8-2))&0x04)|((n>>( 4-1))&0x02)|((n)        &0x01))
#define XT_0XBIN_TO_HEX16(n1,n0)             (((uint16_t)(XT_0XBIN_TO_HEX8(n1))<<8)|(XT_0XBIN_TO_HEX8(n0)&0xFF))
#define XT_0XBIN_TO_HEX32(n3,n2,n1,n0)       (((uint32_t)(XT_0XBIN_TO_HEX8(n3))<<24)|((uint32_t)(XT_0XBIN_TO_HEX8(n2))<<16)\
                                             |((uint32_t)(XT_0XBIN_TO_HEX8(n1))<<8)|(XT_0XBIN_TO_HEX8(n0)&0xFF))

//二进制书写格式数据转换                     (使用例子: XT_8BIN_TO_HEX16(00001111,11110000))
#define _XT_8BIN_TO_HEX8(n)                  XT_0XBIN_TO_HEX8(0x##n##UL) //应用程序不能调用本宏,请调用下行宏定义
#define XT_8BIN_TO_HEX8(n)                  _XT_8BIN_TO_HEX8(n)
#define XT_8BIN_TO_HEX16(n1,n0)              (((uint16_t)(_XT_8BIN_TO_HEX8(n1))<<8)|(_XT_8BIN_TO_HEX8(n0)&0xFF))
#define XT_8BIN_TO_HEX32(n3,n2,n1,n0)        (((uint32_t)(_XT_8BIN_TO_HEX8(n3))<<24)|((uint32_t)(_XT_8BIN_TO_HEX8(n2))<<16)\
                                             |((uint32_t)(_XT_8BIN_TO_HEX8(n1))<<8)|(_XT_8BIN_TO_HEX8(n0)&0xFF))

//将[uint8_t]合并成[uint16_t]                (d2作最高位, d1最低位)
#define XT_U8_TO_U16(d2,d1)                  ((((uint16_t)(d2) & 0xFF) << 8) | ((uint16_t)(d1) & 0xFF))

//将[uint16_t]合并成[uint32_t]               (d2作最高位, d1最低位)
#define XT_U16_TO_U32(d2,d1)                 ((((uint32_t)(d2) & 0xFFFF) << 16) | ((uint32_t)(d1) & 0xFFFF))

//将[uint8_t]合并成[uint32_t]                (d4作最高位, d1最低位)
#define XT_U8_TO_U32(d4,d3,d2,d1)            ((((uint32_t)(d4) & 0xFF) << 24)\
                                             |(((uint32_t)(d3) & 0xFF) << 16)\
                                             |(((uint32_t)(d2) & 0xFF) <<  8)\
                                             |(((uint32_t)(d1) & 0xFF)      ))

//【其它常用宏定义】--------------------------------------------------------------------------------------

//得到指定地址上的一个单字节数值
#define XT_MEM_B(x)                          (*((uint8_t *)(x)))

//得到指定地址上的一个双字节数值
#define XT_MEM_W(x)                          (*((uint16_t *)(x)))

//得到指定地址上的一个四字节数值
#define XT_MEM_H(x)                          (*((uint32_t *)(x)))

//求最小值
#define XT_MIN(x,y)                          (((x)<(y))?(x):(y))

//求最大值
#define XT_MAX(x,y)                          (((x)>(y))?(x):(y))

//返回一个比x大的最接近8的倍数
#define XT_RND8(x)                           ((((x)+7)/8)*8)

//判断字符是不是10进值的数字
#define XT_DECCHK(c)                         ((c)>='0'&&(c)<='9')

//判断字符是不是16进值的数字
#define XT_HEXCHK(c)                         (((c)>='0'&&(c)<='9') \
                                            ||((c)>='A'&&(c)<='F') \
                                            ||((c)>='a'&&(c)<='f'))
//防止溢出的一个方法
#define XT_INC_SAT(v)                        (v=((v)+1>(v))?(v)+1:(v))

//返回数组元素的个数
#define XT_ARRAY_SIZE(a)                     (sizeof((a))/sizeof((a[0])))

//【ANSI标准宏】------------------------------------------------------------------------------------------

#if 0

//__FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__ 
//分别表示当前“文件”“行号”“函数”“日期”“时间”，示范如下：
printf("File = %s\nLine = %d\nFunc = %s\nDate = %s\nTime = %s\n", 
      __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__);

//__VA_ARGS__ 是一个可变参数的宏，C99 规范中新增的，示范如下：
#define dev_printf(...)                      rt_kprintf(__VA_ARGS__)

#endif

//【上电初始化机制】--------------------------------------------------------------------------------------

//上电初始化模型
typedef void (*xt_init_fn_t)(void);
typedef struct xt_init_fn_tab_
{
    xt_init_fn_t p_fn;
    const char *name;
}xt_init_fn_tab_t;

//1、内核上电初始化（如：定时器等）
#define XT_CHIP_INIT_S_TAB_EXPORT(func,name) XT_USED const xt_init_fn_tab_t chip_init_fn_##func XT_SECTION(".xt_section_fn.0000.00") = {func, name}
#define XT_CHIP_INIT_1_TAB_EXPORT(func,name) XT_USED const xt_init_fn_tab_t chip_init_fn_##func XT_SECTION(".xt_section_fn.0000.25") = {func, name}
#define XT_CHIP_INIT_2_TAB_EXPORT(func,name) XT_USED const xt_init_fn_tab_t chip_init_fn_##func XT_SECTION(".xt_section_fn.0000.50") = {func, name}
#define XT_CHIP_INIT_3_TAB_EXPORT(func,name) XT_USED const xt_init_fn_tab_t chip_init_fn_##func XT_SECTION(".xt_section_fn.0000.75") = {func, name}
#define XT_CHIP_INIT_E_TAB_EXPORT(func,name) XT_USED const xt_init_fn_tab_t chip_init_fn_##func XT_SECTION(".xt_section_fn.0000.99") = {func, name}

//2、硬件上电初始化（如：IC引脚等）
#define XT_HARD_INIT_S_TAB_EXPORT(func,name) XT_USED const xt_init_fn_tab_t hard_init_fn_##func XT_SECTION(".xt_section_fn.0001.00") = {func, name}
#define XT_HARD_INIT_1_TAB_EXPORT(func,name) XT_USED const xt_init_fn_tab_t hard_init_fn_##func XT_SECTION(".xt_section_fn.0001.25") = {func, name}
#define XT_HARD_INIT_2_TAB_EXPORT(func,name) XT_USED const xt_init_fn_tab_t hard_init_fn_##func XT_SECTION(".xt_section_fn.0001.50") = {func, name}
#define XT_HARD_INIT_3_TAB_EXPORT(func,name) XT_USED const xt_init_fn_tab_t hard_init_fn_##func XT_SECTION(".xt_section_fn.0001.75") = {func, name}
#define XT_HARD_INIT_E_TAB_EXPORT(func,name) XT_USED const xt_init_fn_tab_t hard_init_fn_##func XT_SECTION(".xt_section_fn.0001.99") = {func, name}

//3、器件上电初始化（如：传感器等）
#define XT_DEV_INIT_S_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t dev_init_fn_##func XT_SECTION(".xt_section_fn.0002.00") = {func, name}
#define XT_DEV_INIT_1_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t dev_init_fn_##func XT_SECTION(".xt_section_fn.0002.25") = {func, name}
#define XT_DEV_INIT_2_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t dev_init_fn_##func XT_SECTION(".xt_section_fn.0002.50") = {func, name}
#define XT_DEV_INIT_3_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t dev_init_fn_##func XT_SECTION(".xt_section_fn.0002.75") = {func, name}
#define XT_DEV_INIT_E_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t dev_init_fn_##func XT_SECTION(".xt_section_fn.0002.99") = {func, name}

//4、信息上电初始化（如：信号量等）
#define XT_MSG_INIT_S_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t msg_init_fn_##func XT_SECTION(".xt_section_fn.0003.00") = {func, name}
#define XT_MSG_INIT_1_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t msg_init_fn_##func XT_SECTION(".xt_section_fn.0003.25") = {func, name}
#define XT_MSG_INIT_2_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t msg_init_fn_##func XT_SECTION(".xt_section_fn.0003.50") = {func, name}
#define XT_MSG_INIT_3_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t msg_init_fn_##func XT_SECTION(".xt_section_fn.0003.75") = {func, name}
#define XT_MSG_INIT_E_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t msg_init_fn_##func XT_SECTION(".xt_section_fn.0003.99") = {func, name}

//5、程序上电初始化（如：线程等）
#define XT_APP_INIT_S_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t app_init_fn_##func XT_SECTION(".xt_section_fn.0004.00") = {func, name}
#define XT_APP_INIT_1_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t app_init_fn_##func XT_SECTION(".xt_section_fn.0004.25") = {func, name}
#define XT_APP_INIT_2_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t app_init_fn_##func XT_SECTION(".xt_section_fn.0004.50") = {func, name}
#define XT_APP_INIT_3_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t app_init_fn_##func XT_SECTION(".xt_section_fn.0004.75") = {func, name}
#define XT_APP_INIT_E_TAB_EXPORT(func,name)  XT_USED const xt_init_fn_tab_t app_init_fn_##func XT_SECTION(".xt_section_fn.0004.99") = {func, name}

#ifdef __cplusplus
	}
#endif

#endif  //#ifndef XT_COMDEF_H__
