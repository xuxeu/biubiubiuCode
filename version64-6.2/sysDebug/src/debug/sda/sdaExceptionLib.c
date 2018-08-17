/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * @file:sdaExceptionLib.c
 * @brief:
 *             <li>���Թ����쳣����ʵ��</li>
 */

/************************ͷ �� ��******************************/
#include <string.h>
#include "ta.h"
#include "taPacketComm.h"
#include "taMsgDispatch.h"
#include "sdaCpuLib.h"
#include "dbAtom.h"
#include "taExceptionReport.h"
#include "sdaMain.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *      ϵͳ�������쳣�������
 * @param[in]:vector: �����쳣������
 * @param[in]: context: �쳣������
 * @return:
 *      ��
 */
void taSDAExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context)
{
#ifdef CONFIG_CORE_SMP
    /* ����CPU�����ת״̬ */
if(taCpuEnabledGet()>1)
{
    sdaCpuAllSuspend();
}
#endif

	/* ���������հ�ģʽΪ��ѯ */
    taSetEndToPoll();

    /* ���õ�ǰ�����쳣���߳�ID */
    TA_SET_CUR_THREAD_ID(taSDADebugInfoGet(), taTaskIdSelf());

    /* ���õ�ǰ���ԻỰ������ָ�� */
    taSDASessInfo.sessInfo.context = taSDAExpContextPointerGet(GET_CPU_INDEX());

    /* �����쳣������  */
    memcpy((void *)taSDAExpContextPointerGet(GET_CPU_INDEX()), context, sizeof(T_DB_ExceptionContext));

    /* ����������,����RSPЭ�������ȡ */
    memcpy((void *)&taGdbGetExpContext, context, sizeof(T_DB_ExceptionContext));

	/* �������ͣ״̬������trapָ�� */
    if (TRUE == taDebugGetStopCommandTag())
    {
    	/* �����ͣ��־ */
    	taDebugSetStopCommandTag(FALSE);

    	/* ���������쳣ָ�� */
    	taDebugJumpOverTrap(taSDAExpContextPointerGet(GET_CPU_INDEX()));
    }

    /* ���ûỰ��״̬ΪDEBUG_STATUS_EXCEPTION */
    TA_SET_DEBUG_STATUS(&(taSDASessInfo.sessInfo),DEBUG_STATUS_EXCEPTION);

	/* ����<vector>��taSDASessInfo�� */
	taSDASessInfo.sessInfo.context->vector = vector;

	/* �ϱ������쳣��GDB */
	taReportExceptionToDebugger(&taSDASessInfo, vector);

    /* ����ѭ��������Ϣ */
    taStartLoopDispatch();

    /* ����ѭ���հ� */
    taMessageDispatch();

#ifdef CONFIG_CORE_SMP
	/* ����CPU�˳���ת״̬ */
if(taCpuEnabledGet()>1)
{ 
	sdaCpuAllResume();    
}
#endif

    /* ���ûỰ��״̬ΪDEBUG_STATUS_CONTINUE */
    TA_SET_DEBUG_STATUS(&(taSDASessInfo.sessInfo),DEBUG_STATUS_CONTINUE);

    /* ��յ�ǰ�����쳣���߳�ID */
    TA_SET_CUR_THREAD_ID(&(taSDASessInfo.sessInfo), 0);

	/* ��������Ϊ�ж�ģʽ */
	taSetEndToInt();

    /* �ָ��쳣������ */
    memcpy((void *)context, taSDAExpContextPointerGet(GET_CPU_INDEX()), sizeof(T_DB_ExceptionContext));
}
