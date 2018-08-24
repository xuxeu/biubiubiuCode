/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-08-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:taskSllLib.h
 * @brief:
 *             <li>单向链表操作库</li>
 */
#ifndef _TASK_SLL_LIB_H_
#define _TASK_SLL_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头 文 件******************************/
#include "taTypes.h"

/************************类型定义*****************************/

/* 节点定义 */
typedef struct sllnode
{
	struct sllnode *next;
} SLL_NODE;

/* 节点链表 */
typedef struct
{
	SLL_NODE *head;	/* 链表头 */
	SLL_NODE *tail;	/* 链表尾 */
} SLL_LIST;

/************************宏 定 义******************************/

/* 链表初始化 */
#define TA_SLL_INIT(list)                                                    \
{                                                                            \
    ((SLL_LIST *)(list))->head  = NULL;                                      \
    ((SLL_LIST *)(list))->tail  = NULL;                                      \
}

/* 增加节点到链表头 */
#define TA_SLL_PUT_AT_HEAD(list, node)                                       \
{                                                                            \
    if ((((SLL_NODE *)(node))->next = ((SLL_LIST *)(list))->head) == NULL)   \
    {                                                                        \
        ((SLL_LIST *)(list))->head = ((SLL_NODE *)(node));                   \
        ((SLL_LIST *)(list))->tail = ((SLL_NODE *)(node));                   \
    }                                                                        \
    else                                                                     \
	{	                                                                     \
        ((SLL_LIST *)(list))->head = (node);                                 \
	}                                                                        \
}

/* 增加节点到链表尾 */
#define TA_SLL_PUT_AT_TAIL(list, node)                                       \
{                                                                            \
    ((SLL_NODE *)(node))->next = NULL;                                       \
    if (((SLL_LIST *)(list))->head == NULL)                                  \
    {                                                                        \
        ((SLL_LIST *)(list))->head = (SLL_NODE *)(node);                     \
        ((SLL_LIST *)(list))->tail = (SLL_NODE *)(node);                     \
    }                                                                        \
    else                                                                     \
    {                                                                        \
        ((SLL_LIST *)(list))->tail->next = (SLL_NODE *)(node);               \
    }                                                                        \
    ((SLL_LIST *)(list))->tail = (SLL_NODE *)(node);                         \
}

/* 获取头节点,且从链表中删除 */
#define TA_SLL_GET(list, node)                                               \
{                                                                            \
    if (((node) = (void *)((SLL_LIST *)(list))->head) != NULL)               \
        ((SLL_LIST *)(list))->head = ((SLL_NODE *)(node))->next;             \
}

/* 从链表中删除节点 */
#define TA_SLL_REMOVE(list, deleteNode, previousNode)                        \
{                                                                            \
    if (((SLL_NODE *)(previousNode)) == NULL)                                \
    {                                                                        \
        ((SLL_LIST *)(list))->head = ((SLL_NODE *)(deleteNode))->next;       \
        if (((SLL_LIST *)(list))->tail == ((SLL_NODE *)(deleteNode)))        \
            ((SLL_LIST *)(list))->tail = NULL;                               \
    }                                                                        \
    else                                                                     \
    {                                                                        \
       ((SLL_NODE *)(previousNode))->next = ((SLL_NODE *)(deleteNode))->next;\
        if (((SLL_LIST *)(list))->tail == ((SLL_NODE *)(deleteNode)))        \
            ((SLL_LIST *)(list))->tail = ((SLL_NODE *)(previousNode));       \
    }                                                                        \
}

/* 获取头节点 */
#define TA_SLL_FIRST(pList)	                                                 \
(				                                                             \
    (((SLL_LIST *)pList)->head)	                                             \
)

/* 获取尾节点 */
#define TA_SLL_LAST(pList)		                                             \
(				                                                             \
    (((SLL_LIST *)pList)->tail)	                                             \
)

/* 获取节点的下一个节点 */
#define TA_SLL_NEXT(pNode)		                                             \
(				                                                             \
    (((SLL_NODE *)pNode)->next)	                                             \
)

/* 链表是否为空 */
#define TA_SLL_EMPTY(pList)			                                         \
(						                                                     \
    (((SLL_LIST *)pList)->head == NULL)		                                 \
)

/************************接口声明*****************************/

/*
 * @brief:
 *      链表节点个数
 * @param[in]: pList: 链表头
 * @return:
 *     count: 节点个数
 */
int taSllCount(SLL_LIST *pList);

/*
 * @brief:
 *      当前节点的上一节点
 * @param[in]: pList: 链表头
 * @return:
 *     pNode: 节点
 */
SLL_NODE *taSllPrevious(SLL_LIST *pList, SLL_NODE *pNode);

/*
 * @brief:
 *      针对每个节点执行一个操作
 * @param[in]: pList: 链表头
 * @param[in]: routine: 函数指针
 * @param[in]: routineArg: 函数参数
 * @return:
 *     pNode: 结束时的节点
 */
SLL_NODE *taSllEach(SLL_LIST *pList, FUNCPTR routine, ulong_t routineArg);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_SLL_LIB_H_ */
