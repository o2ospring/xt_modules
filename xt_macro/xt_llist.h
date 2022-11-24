/**
  * @file  xt_llist.h
  * @brief 双向链表服务
  * COPYRIGHT (C) 2022, XT 技术联合小组
  * Change Logs:
  * Date           Author       Notes
  * 2022-11-15     o2ospring    原始版本
  */
#ifndef XT_LLIST_H__
#define XT_LLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

// 双向链表结构体（将它嵌入到你的数据结构体里）
typedef struct xt_llhead_
{
    struct xt_llhead_ *prev;
    struct xt_llhead_ *next;
}xt_llhead_t;

/**
  * @brief  创建索引（创建空表）
  * @param  H               实体:变量名称（创建变量）
  */
#define XT_LL_HEAD(H)       xt_llhead_t H = { &H, &H }

/**
  * @brief  初始节点（重置节点）
  * @param  PN              实体:自由节点->变量地址
  * @return void
  */
#define XT_LL_INIT(PN)      ((PN)->next = (PN)->prev = (PN))

/**
  * @brief  在链表的最前面拼接节点
  * @param  PH              实体:链表索引->变量地址
  * @param  PN              实体:新加节点->变量地址
  * @return void
  */
#define XT_LL_ADD(PH, PN)   do                    \
{                                                 \
    ((PH)->next)->prev = (PN);                    \
    (PN)->next = ((PH)->next);                    \
    (PN)->prev = (PH);                            \
    (PH)->next = (PN);                            \
}   while(0)

/**
  * @brief  在链表的最后面拼接节点
  * @param  PH              实体:链表索引->变量地址
  * @param  PN              实体:新加节点->变量地址
  * @return void
  */
#define XT_LL_TAIL(PH, PN)  do                    \
{                                                 \
    ((PH)->prev)->next = (PN);                    \
    (PN)->prev = ((PH)->prev);                    \
    (PN)->next = (PH);                            \
    (PH)->prev = (PN);                            \
}   while(0)

/**
  * @brief  将节点从链表中删除
  * @param  PN              实体:删除节点->变量地址
  * @return void
  */
#define XT_LL_DEL(PN)       do                    \
{                                                 \
	((PN)->next)->prev = ((PN)->prev);            \
	((PN)->prev)->next = ((PN)->next);            \
	XT_LL_INIT(PN);                               \
}   while(0)

/**
  * @brief  判断节点是否为空节点（或判断链表是否为空表）
  * @param  PN              实体:判断节点->变量地址
  *     或  PH              实体:链表索引->变量地址
  * @return 真->空表
  */
#define XT_LL_EMPTY(PN)     ((PN)->next == (PN)) /*
#define XT_LL_EMPTY(PH)     ((PH)->next == (PH)) */

/**
  * @brief  使用for循环遍历节点（只读）
  * @param  PH              实体:链表索引->变量地址
  * @param  PN              实体:遍历节点->临时指针
  * @return PN->某次遍历的节点，否则直接退出for循环
  */
#define XT_LL_FOREACH(PH, PN)                     \
                            for (PN = (PH)->next; PN != (PH); PN = (PN)->next)
/**
  * @brief  使用for循环遍历节点（可一边遍历一边删除节点）
  * @param  PH              实体:链表索引->变量地址
  * @param  PN              实体:遍历节点->临时指针
  * @param  PT              实体:下一节点->临时指针
  * @return PN->某次遍历的节点，否则直接退出for循环
  */
#define XT_LL_FOREACH_SAFE(PH, PN, PT)            \
                            for (PN = (PH)->next, PT = (PN)->next; PN != (PH); PN = (PT), PT = (PN)->next)
/**
  * @brief  根据链表节点得出用户数据结构体地址
  * @param  PN              实体:链表节点->变量地址
  * @param  T               虚体:用户数据结构体定义
  * @param  N               虚体:用户数据结构体成员->链表节点
  * @return 得出用户数据结构体地址
  */
#define XT_LL_ENTRY(PN, T, N)                     \
                            ((T *)((char *)(PN) - ((char *)(&(((T *)0)->N)))))

#ifdef __cplusplus
	}
#endif

#endif //#ifndef XT_LLIST_H__
