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
	int tid = 0;
	UINT32 sid = 0;
	TA_CTX_TYPE ctxType = 0;

#ifdef __MIPS__
	/* ��ֹ�ϵ�  ��ֹ�����ڼ��������strlen ���²��������쳣*/
	taDebugBpAllDisable();
#endif
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

#ifdef __MIPS__
    //�ж��Ƿ����������
	taDebugClrSoftStep();
#endif
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

#ifdef __MIPS__
	taDebugSetSoftStep(context);
#endif

    /* �ָ��쳣������ */
    memcpy((void *)context, TA_GET_EXP_CONTEXT(sid), sizeof(T_DB_ExceptionContext));

    /* ��յ�ǰ�����쳣���߳�ID */
    TA_SET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid), 0);

	/* ���ûỰ��״̬ΪDEBUG_STATUS_CONTINUE */
	TA_SET_DEBUG_STATUS(TA_GET_SESSION_INFO(sid),DEBUG_STATUS_CONTINUE);

#ifdef CONFIG_DELTAOS
    TA_BP_HIT_CLEAR(taskTcb(tid));
#endif

#ifdef __MIPS__
    /* ��ֹ�ϵ� */
    taDebugBpAllDisable(); //ɾ��gdb��ɾ���ϵ���Ķϵ�

    /* ʹ�ܶϵ� */
    taDebugBpAllEnable();
#endif
}

#ifdef EXCEPTAKE

//������ͷ�ļ���Ϊ�˻�ȡRTP��̬����Ϣ�������δ�ɹ�
#include "shlLibP.h"
#include "rtpLibP.h"
extern int rtpEpId;

UINT8 * rtpMsg()
{
	    RTP_SHL_NODE *pRtpShlNode;
        static RTP_SHL_NODE *shlListId[200] = {1,};
        WIND_RTP * rtpId = 0;
        int count = 0,i=0,len = 0;
        UINT8 *buffer=NULL;
        UINT8 *buf=NULL;
        
        buf = buffer = malloc(200);

        rtpId = (WIND_RTP *)rtpEpId;

        if (rtpId != 0)
        {

        len = sprintf(buffer,"textStart:%x,", (UINT32)((WIND_RTP *)rtpId)->binaryInfo.textStart);
        buffer =buffer+len;

        RTP_UNLOCK((WIND_RTP*)rtpId);

	    pRtpShlNode = (RTP_SHL_NODE *) DLL_FIRST(&rtpId->rtpShlNodeList);

        while (pRtpShlNode != NULL)
        {
    	    shlListId[count++] = pRtpShlNode;
    	    pRtpShlNode = (RTP_SHL_NODE *) DLL_NEXT(pRtpShlNode);
	    }


        for (i = 0; i<count;i++)
        {

            len = sprintf(buffer,"%s=%x,", shlListId[i]->pShlNode->name,shlListId[i]->pShlNode->textBase);
            buffer =buffer+len;
        }
        
        }
        return buf;
    }






void taTaskExceptionTake(UINT32 vector, T_DB_ExceptionContext *context)
{
    int tid = 0;
    UINT32 sid = 0;
    TA_CTX_TYPE ctxType = 0;
    static int exceptaskNum = 1;
    UINT8 *buf=NULL;
if(exceptaskNum++ == 1)
{
    /* ��ȡ��ǰ����ID */
    tid = taTaskIdSelf();

    /* ��������ỰID */
    taTaskNodeFind(tid, &sid);

	/* ���õ�ǰ�쳣�߳�ID */
	TA_SET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid), tid);

	/* ���ûỰ��״̬ΪDEBUG_STATUS_EXCEPTION */
	TA_SET_DEBUG_STATUS(TA_GET_SESSION_INFO(sid),DEBUG_STATUS_EXCEPTION);

    /* �����쳣������  */
    //memcpy((void *)TA_GET_EXP_CONTEXT(sid), context, sizeof(T_DB_ExceptionContext));

    buf = rtpMsg();

    /* ���������հ�ģʽΪ��ѯ,�ж�ģʽ��WDB�հ��ӿ�Ϊ�����ӿڣ��ýӿڻ�ȴ��ź��� */
	taSetEndToPoll();

    /* �ϱ������쳣�������� */
    taReportExceptionToHost(TA_GET_DEBUG_SESSION(sid), tid, context, buf, 1);

    /* ��������Ϊ�ж�ģʽ */
	taSetEndToInt();

    /* ��������쳣������ */
    taTaskSuspend(tid);

    /* ��ȡ���������� */
	ctxType = TA_GET_CONTEXT_TYPE(sid);

    /* ��������� */
	taTaskCpuUnlockNoResched();

	/* ���RTP���Թ���RTP�����������񼶵��Թ���󶨵ĵ������� */
	TA_RT_CTX_SUSPEND(ctxType,sid);

    /* �ָ��쳣������ */
    //memcpy((void *)context, TA_GET_EXP_CONTEXT(sid), sizeof(T_DB_ExceptionContext));

    /* ��յ�ǰ�����쳣���߳�ID */
    //TA_SET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid), 0);

    /* ���ûỰ��״̬ΪDEBUG_STATUS_CONTINUE */
    //TA_SET_DEBUG_STATUS(TA_GET_SESSION_INFO(sid),DEBUG_STATUS_CONTINUE);
}
taTaskSuspend(tid);

}

#endif
