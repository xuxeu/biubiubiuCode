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
 * @file:taskSessionLib.c
 * @brief:
 *             <li>���ԻỰ����</li>
 */

/************************ͷ �� ��*****************************/
#include "ta.h"
#include "taskSessionLib.h"
#include "taskCtrLib.h"
#include "taskSllLib.h"
#include "taRtIfLib.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/* �쳣������ */
static T_DB_ExceptionContext taExpContext[TA_MAX_SESS_NUM];
/************************ȫ�ֱ���*****************************/

/* ���ԻỰ�� */
T_TA_Session taSessionTable[TA_MAX_SESS_NUM];
/************************ʵ   ��********************************/

/*
 * @brief:
 *     ��ʼ�����ԻỰ
 * @return:
 *     ��
 */
void taSessionInit(void)
{
    UINT32 idx = 0;

    /* ����쳣������ */
    memset(taExpContext, 0, sizeof(T_DB_ExceptionContext)*TA_MAX_SESS_NUM);

    /* ��յ��ԻỰ��Ϣ */
    memset(taSessionTable, 0, sizeof(T_TA_Session)*TA_MAX_SESS_NUM);

    /* ���õ��ԻỰ������ */
    for (idx = 0; idx < TA_MAX_SESS_NUM; ++idx)
    {
        /* ��ʼ���������� */
    	TA_SLL_INIT(TA_GET_TASK_LIST(idx));

        /* ���������� */
        taSessionTable[idx].sessInfo.context = &taExpContext[idx];

        /* �������ݰ����û�������ʼλ�� */
        taSetPktOffset(&(taSessionTable[idx].packet),TA_PROTOCOL_HEAD_SIZE+TA_PROTOCOL_CHECKSUM_SIZE);
    }
}

/*
 * @brief:
 *      ������ԻỰ
 * @param[in]: objId: ���ԵĶ���ID��ʵʱ���̵���Ϊʵʱ����ID,�����������ΪkernelId
 * @param[out]: pSid: �ỰID
 * @return:
 *     TA_OK:������ԻỰ�ɹ�
 *     TA_SESSION_FULL:���ԻỰ������
 */
T_TA_ReturnCode taSessionMalloc(TA_CTX_TYPE ctxType, ulong_t objId, UINT32 *pSid)
{
	UINT32 idx = 0;
    UINT32 sid = TA_MAX_SESS_NUM;

	/* ��ѯ���еĵ��ԻỰ */
	for (idx = 0; idx < TA_MAX_SESS_NUM; ++idx)
	{
        /* ���õ�һ�����еĵ��ԻỰID */
        if (!TA_SESSION_IS_ATTACH(idx))
        {
        	sid = idx;
        	break;
        }
	}

	/* ���ԻỰ�Ѿ������� */
	if (sid >= TA_MAX_SESS_NUM)
	{
		return (TA_SESSION_FULL);
	}

	/* ���ûỰID */
	*pSid = sid;

	/* ���õ��Զ���ID */
	taSessionTable[sid].sessInfo.objID = objId;

    /* ���������� */
    taSessionTable[idx].sessInfo.context = &taExpContext[idx];

	/* ���ûỰID */
	taSessionTable[sid].sessInfo.sessionID = sid;

	/* �������������� */
	taSessionTable[sid].sessInfo.ctxType = ctxType;

	/* Ĭ���´����������Զ��� */
	taSessionTable[sid].sessInfo.taskIsAutoAttach = FALSE;

	/* ���õ���״̬ */
	taSessionTable[sid].sessInfo.status = DEBUG_STATUS_CONTINUE;

	/* ���ð�״̬ */
	taSessionTable[sid].sessInfo.attachStatus = DEBUG_STATUS_ATTACH;

    /* �������ݰ����û�������ʼλ�� */
    taSetPktOffset(&(taSessionTable[sid].packet),TA_PROTOCOL_HEAD_SIZE+TA_PROTOCOL_CHECKSUM_SIZE);

	return (TA_OK);
}

/*
 * @brief:
 *      �ͷŵ��ԻỰ
 * @param[in]: sid: �ỰID
 * @return:
 *     ��
 */
void taSessionFree(UINT32 sid)
{
	/* ɾ�����ԻỰ���жϵ� */
	taDebugRemoveAllBp(TA_GET_SESSION_INFO(sid));

	/* �ͷ����������е�����ڵ��ڴ� */
	taSllEach(TA_GET_TASK_LIST(sid), taTaskNodeFree, 0);

	/* ��ʼ���������� */
	TA_SLL_INIT(TA_GET_TASK_LIST(sid));

    /* ����쳣������ */
    memset(TA_GET_EXP_CONTEXT(sid), 0, sizeof(T_DB_ExceptionContext));

    /* ��յ��ԻỰ��Ϣ */
    memset(TA_GET_DEBUG_SESSION(sid), 0, sizeof(T_TA_Session));
}
