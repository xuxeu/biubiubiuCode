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
 * @file:taskSllLib.c
 * @brief:
 *             <li>链表操作接口实现，主要实现节点个数统计，节点循环操作</li>
 */

/************************头 文 件*****************************/
#include "taskSllLib.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************实   现********************************/

/*
 * @brief:
 *      链表节点个数
 * @param[in]: pList: 链表头
 * @return:
 *     count: 节点个数
 */
int taSllCount(SLL_LIST *pList)
{
    SLL_NODE *pNode = TA_SLL_FIRST(pList);
    int count = 0;

    while (pNode != NULL)
	{
	    count ++;
	    pNode = TA_SLL_NEXT (pNode);
	}

    return (count);
}

/*
 * @brief:
 *      当前节点的上一节点
 * @param[in]: pList: 链表头
 * @return:
 *     pNode: 节点
 */
SLL_NODE *taSllPrevious(SLL_LIST *pList, SLL_NODE *pNode)
{
    SLL_NODE *pTmpNode = pList->head;

    if ((pTmpNode == NULL) || (pTmpNode == pNode))
    {
    	return (NULL);
    }

    while (pTmpNode->next != NULL)
	{
    	if (pTmpNode->next == pNode)
    	{
    		return (pTmpNode);
    	}

    	pTmpNode = pTmpNode->next;
	}

    return (NULL);
}

/*
 * @brief:
 *      针对每个节点执行一个操作
 * @param[in]: pList: 链表头
 * @param[in]: routine: 函数指针
 * @param[in]: routineArg: 函数参数
 * @return:
 *     pNode: 结束时的节点
 */
SLL_NODE *taSllEach(SLL_LIST *pList, FUNCPTR routine, ulong_t routineArg)
{
    SLL_NODE *pNode = TA_SLL_FIRST (pList);
    SLL_NODE *pNext;

    while (pNode != NULL)
	{
	    pNext = TA_SLL_NEXT (pNode);
	    if ((*routine) ((void*)pNode, routineArg) == TRUE)
	    {
	        break;
	    }
	    pNode = pNext;
	}

    return (pNode);
}
