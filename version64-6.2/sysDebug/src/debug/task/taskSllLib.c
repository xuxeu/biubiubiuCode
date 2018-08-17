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
 * @file:taskSllLib.c
 * @brief:
 *             <li>��������ӿ�ʵ�֣���Ҫʵ�ֽڵ����ͳ�ƣ��ڵ�ѭ������</li>
 */

/************************ͷ �� ��*****************************/
#include "taskSllLib.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/

/*
 * @brief:
 *      ����ڵ����
 * @param[in]: pList: ����ͷ
 * @return:
 *     count: �ڵ����
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
 *      ��ǰ�ڵ����һ�ڵ�
 * @param[in]: pList: ����ͷ
 * @return:
 *     pNode: �ڵ�
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
 *      ���ÿ���ڵ�ִ��һ������
 * @param[in]: pList: ����ͷ
 * @param[in]: routine: ����ָ��
 * @param[in]: routineArg: ��������
 * @return:
 *     pNode: ����ʱ�Ľڵ�
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
