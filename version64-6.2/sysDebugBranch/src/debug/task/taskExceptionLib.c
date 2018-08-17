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
 * @file:taskExceptionLib.c
 * @brief:
 *             <li>���񼶵����쳣����</li>
 */

/************************ͷ �� ��*****************************/
#include "taskCtrLib.h"
#include "taskSessionLib.h"
#include "taExceptionReport.h"
#include "taRtIfLib.h"
#include "dbAtom.h"
#include "taskBpIgnoreLib.h"
#include "taskSchedLockLib.h"
#ifdef CONFIG_DELTAOS
#include "taskLib.h"
#endif

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

#ifdef CONFIG_DELTAOS
/* ���ߵ�������ǰ�������쳣״̬,�ָ���������ʱ�ָ���CS�Ĵ���ΪsysCsSuper������sysCsUser */
#define	TA_BP_HIT_SET(tid)	((tid)->pDbgInfo = (void *)-1);
#define	TA_BP_HIT_CLEAR(tid) tid->pDbgInfo = NULL;
#endif

/************************ʵ   ��********************************/

/*
 * @brief:
 *      �����쳣����
 * @param[in]: vector: �쳣��
 * @param[in]: context: �쳣������
 * @return:
 *     ��
 */
void taTaskExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context)
{
	ulong_t tid = 0;
	UINT32 sid = 0;
	TA_CTX_TYPE ctxType = 0;

	/* �Ƿ���Ҫ����ϵ����� */
    if (TA_DO_IGNORE())
    {
    	/* ����ϵ����� */
    	taTaskBpDoneIgnore(context);

    	/* ���������������� */
    	taTaskSchedUnlock();
    	return;
	}

	/* ��ȡ��ǰ����ID */
	tid = taTaskIdSelf();

	/* ��������ỰID */
	SLL_NODE* taskNode = taTaskNodeFind(tid, &sid);
	if (NULL == taskNode)
	{
		/* ��ֹ������������ */
		taTaskSchedLock();

		/* ����δ��,���Զϵ�  */
		taTaskBpDoIgnore(context);

		return;
	}

	/* ��ֹ����CPU������ռ */
	taTaskCpuLock();

#ifdef CONFIG_DELTAOS
	/* ���öϵ����� */
	TA_BP_HIT_SET(taskTcb(tid));
#endif

	/* ���õ�ǰ�쳣�߳�ID */
	TA_SET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid), tid);

	/* ���ûỰ��״̬ΪDEBUG_STATUS_EXCEPTION */
	TA_SET_DEBUG_STATUS(TA_GET_SESSION_INFO(sid),DEBUG_STATUS_EXCEPTION);

    /* �����쳣������  */
    memcpy((void *)TA_GET_EXP_CONTEXT(sid), context, sizeof(T_DB_ExceptionContext));

    /* ����������,����RSPЭ�������ȡ */
    memcpy((void *)&taGdbGetExpContext, context, sizeof(T_DB_ExceptionContext));

	/* ���������հ�ģʽΪ��ѯ,�ж�ģʽ��WDB�հ��ӿ�Ϊ�����ӿڣ��ýӿڻ�ȴ��ź��� */
	taSetEndToPoll();

	/* �ϱ������쳣��GDB */
	taReportExceptionToDebugger(TA_GET_DEBUG_SESSION(sid), vector);

	/* ��������Ϊ�ж�ģʽ */
	taSetEndToInt();

	/* ��ȡ���������� */
	ctxType = TA_GET_CONTEXT_TYPE(sid);

	/* ��������� */
	taTaskCpuUnlockNoResched();

	/* ���RTP���Թ���RTP�����������񼶵��Թ���󶨵ĵ������� */
	TA_RT_CTX_SUSPEND(ctxType,sid);

    /* �ָ��쳣������ */
    memcpy((void *)context, TA_GET_EXP_CONTEXT(sid), sizeof(T_DB_ExceptionContext));

    /* ��յ�ǰ�����쳣���߳�ID */
    TA_SET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid), 0);

	/* ���ûỰ��״̬ΪDEBUG_STATUS_CONTINUE */
	TA_SET_DEBUG_STATUS(TA_GET_SESSION_INFO(sid),DEBUG_STATUS_CONTINUE);

#ifdef CONFIG_DELTAOS
    TA_BP_HIT_CLEAR(taskTcb(tid));
#endif
}
