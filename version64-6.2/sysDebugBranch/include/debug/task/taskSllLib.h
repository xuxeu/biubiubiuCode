/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-08-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:taskSllLib.h
 * @brief:
 *             <li>�������������</li>
 */
#ifndef _TASK_SLL_LIB_H_
#define _TASK_SLL_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ �� ��******************************/
#include "taTypes.h"

/************************���Ͷ���*****************************/

/* �ڵ㶨�� */
typedef struct sllnode
{
	struct sllnode *next;
} SLL_NODE;

/* �ڵ����� */
typedef struct
{
	SLL_NODE *head;	/* ����ͷ */
	SLL_NODE *tail;	/* ����β */
} SLL_LIST;

/************************�� �� ��******************************/

/* �����ʼ�� */
#define TA_SLL_INIT(list)                                                    \
{                                                                            \
    ((SLL_LIST *)(list))->head  = NULL;                                      \
    ((SLL_LIST *)(list))->tail  = NULL;                                      \
}

/* ���ӽڵ㵽����ͷ */
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

/* ���ӽڵ㵽����β */
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

/* ��ȡͷ�ڵ�,�Ҵ�������ɾ�� */
#define TA_SLL_GET(list, node)                                               \
{                                                                            \
    if (((node) = (void *)((SLL_LIST *)(list))->head) != NULL)               \
        ((SLL_LIST *)(list))->head = ((SLL_NODE *)(node))->next;             \
}

/* ��������ɾ���ڵ� */
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

/* ��ȡͷ�ڵ� */
#define TA_SLL_FIRST(pList)	                                                 \
(				                                                             \
    (((SLL_LIST *)pList)->head)	                                             \
)

/* ��ȡβ�ڵ� */
#define TA_SLL_LAST(pList)		                                             \
(				                                                             \
    (((SLL_LIST *)pList)->tail)	                                             \
)

/* ��ȡ�ڵ����һ���ڵ� */
#define TA_SLL_NEXT(pNode)		                                             \
(				                                                             \
    (((SLL_NODE *)pNode)->next)	                                             \
)

/* �����Ƿ�Ϊ�� */
#define TA_SLL_EMPTY(pList)			                                         \
(						                                                     \
    (((SLL_LIST *)pList)->head == NULL)		                                 \
)

/************************�ӿ�����*****************************/

/*
 * @brief:
 *      ����ڵ����
 * @param[in]: pList: ����ͷ
 * @return:
 *     count: �ڵ����
 */
int taSllCount(SLL_LIST *pList);

/*
 * @brief:
 *      ��ǰ�ڵ����һ�ڵ�
 * @param[in]: pList: ����ͷ
 * @return:
 *     pNode: �ڵ�
 */
SLL_NODE *taSllPrevious(SLL_LIST *pList, SLL_NODE *pNode);

/*
 * @brief:
 *      ���ÿ���ڵ�ִ��һ������
 * @param[in]: pList: ����ͷ
 * @param[in]: routine: ����ָ��
 * @param[in]: routineArg: ��������
 * @return:
 *     pNode: ����ʱ�Ľڵ�
 */
SLL_NODE *taSllEach(SLL_LIST *pList, FUNCPTR routine, ulong_t routineArg);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_SLL_LIB_H_ */
