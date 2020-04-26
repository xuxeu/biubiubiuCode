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
 * @file:taskCtrlLib.c
 * @brief:
 *             <li>������Ʋ�������Ҫʵ��������԰󶨺�������Խ��</li>
 */

/************************ͷ �� ��*****************************/
#include "taRtIfLib.h"
#include "taskSllLib.h"
#include "taskSessionLib.h"
#include "taExceptionReport.h"
#include "taskCtrLib.h"
#include "ta.h"
#ifdef _TA_CONFIG_RTP
#include "taRtp.h"
#endif

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/

/*
 * @brief:
 *      �ͷ�����ڵ��ڴ�
 * @param[in]: pTaskNode: ����ڵ�
 * @param[in]: arg: ��������
 * @return:
 *     FALSE: ������������
 */
BOOL taTaskNodeFree(SLL_NODE *pTaskNode, INT32 arg)
{
	/* �ͷŽڵ��ڴ� */
	TA_RT_MEM_FREE(pTaskNode);

	return (FALSE);
}

/*
 * @brief:
 *      ��������IDƥ������ڵ�
 * @param[in]: pTaskNode: ����ڵ�
 * @param[in]: tid: ����ID
 * @return:
 *     FALSE: ����ID������ڵ㲻ƥ��
 *     TRUE: ����ID������ڵ�ƥ��
 */
BOOL taTaskNodeMatch(SLL_NODE *pTaskNode, INT32 tid)
{
	return (((T_TASK_NODE *)pTaskNode)->taskId == tid);
}

/*
 * @brief:
 *      ���ݻָ�����ڵ����������
 * @param[in]: pTaskNode: ����ڵ�
 * @param[in]: arg: ��������
 * @return:
 *     FALSE: ��ʾ�ָ�����������������
 */
BOOL taTaskNodeResume(SLL_NODE *pTaskNode, INT32 arg)
{
	INT32 threadId = arg;

	/* �ָ������Ȼָ����������ٻָ��������� */
	if(threadId != (((T_TASK_NODE *)pTaskNode)->taskId))
	{
		/* �ָ����� */
		taTaskResume(((T_TASK_NODE *)pTaskNode)->taskId);
	}

	return (FALSE);
}

/*
 * @brief:
 *      ��������ڵ������
 * @param[in]: pTaskNode: ����ڵ�
 * @param[in]: arg: ��������
 * @return:
 *     FALSE: ��ʾ��������������������
 */
BOOL taTaskNodeSuspend(SLL_NODE *pTaskNode, INT32 arg)
{
	INT32 threadId = arg;

	/* ���������ȹ��������󶨵��������ٹ����쳣���񣬷�����ܵ����޷������������� */
	if(threadId != (((T_TASK_NODE *)pTaskNode)->taskId))
	{
		taTaskSuspend(((T_TASK_NODE *)pTaskNode)->taskId);
	}

	return (FALSE);
}

/*
 * @brief:
 *     �����Ƿ���Ҫ�󶨣����������������Ϊ�󶨵��Ե�ʵʱ���̴������߰󶨵ĵ������񴴽����Զ��󶨸����񵽵��ԻỰ��
 * @param[in]: tid: ����ID
 * @param[out]: pSid: �ỰID
 * @return:
 *     FALSE: ��������񵽵��ԻỰ
 *     TRUE��������񵽵��ԻỰ
 */
BOOL taTaskIsAttach(int tid, UINT32 *pSid)
{
	/* ���Ҵ���������������Ƿ�󶨵��� */
	SLL_NODE *taskNode = taTaskNodeFind(taTaskIdSelf(), pSid);

	return ((taskNode != NULL) && TA_TASK_IS_AUTO_ATTACH(*pSid)) ? TRUE : FALSE;
}

/*
 * @brief:
 *      ��������ڵ�
 * @param[in]: tid: ����ID
 * @param[out]: pSid: �ỰID
 * @return:
 *     NULL:δ�ҵ���Ӧ������ڵ�
 *     T_TASK_NODE:����ڵ�
 */
SLL_NODE* taTaskNodeFind(INT32 tid, UINT32 *pSid)
{
	SLL_NODE *taskNode = NULL;
	UINT32 idx = 0;

	/* ��������ID��������ڵ� */
	for (idx = 0; idx < TA_MAX_SESS_NUM; idx++)
	{
        if(!TA_SESSION_IS_ATTACH(idx))
		{
			continue;
		}

		/* ������������ */
		taskNode = taSllEach(TA_GET_TASK_LIST(idx), taTaskNodeMatch, tid);
		if (NULL != taskNode)
		{
			*pSid = idx;
			break;
		}
	}

	return (taskNode);
}

/*
 * @brief:
 *      ��ȡ���ԻỰ�а󶨵�����ID�б�
 * @param[in]: sid: �ỰID
 * @param[out]: idList: ����ID�б�
 * @param[in]: maxTasks: ���������������
 * @return:
 *     �������
 */
INT32 taGetTaskIdList(UINT32 sid, INT32 idList[], INT32 maxTasks)
{
	INT32 idx = 0;
    SLL_NODE *pNode = TA_SLL_FIRST(TA_GET_TASK_LIST(sid));

    while ((pNode != NULL) && (--maxTasks >= 0))
	{
    	idList[idx++] = ((T_TASK_NODE *)pNode)->taskId;
	    pNode = TA_SLL_NEXT(pNode);
	}

    return (idx);
}

/*
 * @brief:
 *      �󶨵�������
 * @param[in]: sid: �ỰID
 * @param[in]: tid: ����ID
 * @return:
 *     TA_OK:������ɹ�
 *     TA_INVALID_TASK_ID:��Ч����ID
 *     TA_TASK_ALREADY_ATTACH:�����Ѿ���
 *     TA_ALLOC_MEM_FAIL:�����ڴ�ʧ��
 */
T_TA_ReturnCode taAttachTask(UINT32 sid, INT32 tid)
{
	T_TASK_NODE *taskNode = NULL;

	/* У������ID */
	if (OK != taTaskIdVerify(tid))
	{
		return (TA_INVALID_TASK_ID);
	}

	/* ���������Ƿ��Ѿ��� */
	taskNode = (T_TASK_NODE *)taTaskNodeFind(tid, &sid);
	if (NULL != taskNode)
	{
		return (TA_TASK_ALREADY_ATTACH);
	}

	/* ��������ڵ� */
	taskNode = TA_RT_MEM_ALLOC(T_TASK_NODE,1);
	if (NULL == taskNode)
	{
		return (TA_ALLOC_MEM_FAIL);
	}

	/* ��������ID */
	taskNode->taskId = tid;

	/* ��������ڵ㵽��������β */
	TA_SLL_PUT_AT_HEAD(TA_GET_TASK_LIST(sid), (SLL_NODE *)taskNode);

	return (TA_OK);
}

/*
 * @brief:
 *      �������
 * @param[in]: tid: ����ID
 * @return:
 *     TA_OK:�������ɹ�
 *     TA_TASK_NO_ATTACH:����δ��
 */
T_TA_ReturnCode taDetachTask(INT32 tid)
{
	SLL_NODE *deleteNode = NULL;
	UINT32 sid = 0;

    /* �����������ڵĽڵ� */
	deleteNode = taTaskNodeFind(tid, &sid);
    if (NULL == deleteNode)
    {
    	return (TA_TASK_NO_ATTACH);
    }

	/* ������������ɾ������ڵ� */
	TA_SLL_REMOVE(TA_GET_TASK_LIST(sid), deleteNode, taSllPrevious(TA_GET_TASK_LIST(sid),deleteNode));

	/* �ͷ�����ڵ��ڴ� */
	taTaskNodeFree(deleteNode, 0);

	return TA_OK;
}

/*
 * @brief:
 *      ���԰�����
 * @param[in]: ctxType: ����������
 * @param[in]: taskId: ����ID
 * @param[out]: pSid: �ỰID
 * @return:
 *     TA_OK: ������ɹ�
 *     TA_INVALID_TASK_ID:��Ч����ID
 *     TA_TASK_ALREADY_ATTACH:�����Ѿ���
 *     TA_ALLOC_MEM_FAIL:�����ڴ�ʧ��
 *     TA_ALLOCATE_SESSION_FAIL��������ԻỰʧ��
 */
UINT32 taTaskAttach(TA_CTX_TYPE ctxType, UINT32 taskId, UINT32 *pSid)
{
    UINT32 ret = TA_OK;
#ifdef _TA_CONFIG_RTP
    UINT32 objId = taRtpFromTaskGet(taskId);
#else
    UINT32 objId = 0;
#endif

	/* ������ԻỰ */
    ret = taSessionMalloc(ctxType, objId, pSid);
    if (TA_OK != ret)
    {
    	return (TA_ALLOCATE_SESSION_FAIL);
    }

    /* ������ */
    ret = taAttachTask(*pSid, taskId);
    if (TA_OK != ret)
    {
    	/* �ͷŵ��ԻỰ  */
    	taSessionFree(*pSid);

    	return (ret);
    }

	/* ��װ���񼶵���������� */
	taInstallMsgProc(TA_GET_CHANNEL_ID(*pSid), taTaskDebugAgent);

    return (TA_OK);
}

/*
 * @brief:
 *      ���Խ������
 * @param[in]: sid: �ỰID
 * @return:
 *     TA_OK: ���ɹ�
 */
UINT32 taTaskDetach(UINT32 sid)
{
	/* ж�����񼶵���������� */
	taInstallMsgProc(TA_GET_CHANNEL_ID(sid), NULL);

    /* �ͷŵ��ԻỰ */
	taSessionFree(sid);

    return (TA_OK);
}

/*
 * @brief:
 *      ���е��ԻỰ���������е���������
 * @param[in]: sid: �ỰID
 * @return:
 *     TA_OK: ���ɹ�
 */
STATUS taTaskAllResume(INT32 sid)
{
	/* ��ȡ�쳣����ID */
	INT32 threadId = TA_GET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid));

	/* ��ֹ������� */
	taTaskCpuLock();

	/* �ָ����� */
	taTaskResume(threadId);

	/* �ָ��������� */
	taSllEach(TA_GET_TASK_LIST(sid), taTaskNodeResume, threadId);

	/* ʹ��������� */
	taTaskCpuUnlock();

	return (OK);
}

/*
 * @brief:
 *      ��ͣ���ԻỰ���������е���������
 * @param[in]: sid: �ỰID
 * @return:
 *     TA_OK: ���ɹ�
 */
STATUS taTaskAllSuspend(INT32 sid)
{
	/* ��ȡ�쳣����ID */
	INT32 threadId = TA_GET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid));

	/* ���������󶨵������� */
	taSllEach(TA_GET_TASK_LIST(sid), taTaskNodeSuspend, threadId);

	/* �����쳣���� */
	taTaskSuspend(threadId);

	return (OK);
}

/*
 * @brief:
 *      ���񼶵�����ͣ
 * @param[in]: sid: �ỰID
 * @return:
 *     ERROR: ��ͣʧ��
 *     OK: ��ͣ�ɹ�
 */
STATUS taTaskPause(INT32 sid)
{
	T_TASK_NODE *taskNode = (T_TASK_NODE *)TA_SLL_FIRST(TA_GET_TASK_LIST(sid));

	/* ���������Ѿ���ɾ�� */
	if(NULL == taskNode)
	{
		return(ERROR);
	}

	/* ���õ�ǰ�쳣�߳�ID */
	TA_SET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid), taskNode->taskId);

	/* ��ͣ���ԻỰ�������� */
	taTaskAllSuspend(sid);

	/* ��ȡ���������� */
	if (OK != taTaskContextGet(taskNode->taskId, TA_GET_EXP_CONTEXT(sid)))
	{
		/* ��յ�ǰ�쳣�߳�ID */
		TA_SET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid), 0);

		/* �ָ������������� */
		taTaskAllResume(sid);

		return(ERROR);
	}

    /* ����������,����RSPЭ�������ȡ */
    memcpy((void *)&taGdbGetExpContext, TA_GET_EXP_CONTEXT(sid), sizeof(T_DB_ExceptionContext));

	/* ���ûỰ��״̬Ϊ������̬ͣ */
	TA_SET_DEBUG_STATUS(TA_GET_SESSION_INFO(sid),DEBUG_STATUS_PAUSE);

	return (OK);
}
