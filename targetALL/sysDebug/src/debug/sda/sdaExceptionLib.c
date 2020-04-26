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
#include "taskBpIgnoreLib.h"
/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/
/*�����ϵ㵼�µ���ͣ������ID*/
 int stepThreadId;
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
#ifdef __MIPS__
#ifndef _KERNEL_DEBUG_
	/* ��ǰ�����Ƿ���Ҫ����ϵ����� */
	if (taSdaDoIgnoreGet())
	{
		//�������ж������������
	    context->registers[STATUSREG] = context->registers[STATUSREG] |0x1;
		/* ����ϵ����� */
		taSdaBpDoneIgnore(context);
		/* ����CPU�˳���ת״̬ */
		sdaCpuAllResume();
		return;
	}
#endif
#endif
#ifdef CONFIG_CORE_SMP
    /* ����CPU�����ת״̬ */
    sdaCpuAllSuspend();
#endif
#ifdef __MIPS__
#ifndef _KERNEL_DEBUG_

	//�����Ȼ�ÿ���Ȩ������������ɾ��Z5��Ķϵ� �����жϵ�ǰPC���Ƿ��Ƕϵ㣬���˵������ڴ˵�����
    if(TRUE != taDebugBpInstructCheck(context->registers[PC]))
    {
    	//�ϵ��Ѿ���ɾ��ֱ�ӷ�������
    	/* ����CPU�˳���ת״̬ */
		sdaCpuAllResume();
        return;
    }
    if(taIsSoftStepBreakPoint(context->registers[PC]))
	{
		if(stepThreadId != 0 && stepThreadId != taTaskIdSelf())
		{
			/* ������������ϵ㣬�������ϵ���������쳣����Ҫ�����ϵ� */
			//�������ж���ֹ�������
			context->registers[STATUSREG] = context->registers[STATUSREG] &(~0x1);
			taSdaBpDoIgnore(context);
			return ;
		}
	}
#endif
#endif

	/* ���������հ�ģʽΪ��ѯ */
    taSetEndToPoll();
#if defined __MIPS__ || defined __PPC85XX__
     //�ж��Ƿ����������
     taDebugClrSoftStep();
#endif
    /* ���õ�ǰ�����쳣���߳�ID */
    TA_SET_CUR_THREAD_ID(taSDADebugInfoGet(), taTaskIdSelf());

    /* ���õ�ǰ���ԻỰ������ָ�� */
    taSDASessInfo.sessInfo.context = taSDAExpContextPointerGet(GET_CPU_INDEX());

    /* �����쳣������  */
    memcpy((void *)taSDAExpContextPointerGet(GET_CPU_INDEX()), context, sizeof(T_DB_ExceptionContext));

#ifdef __PPC__
#ifdef TA_ALTIVEC_SUPPORT_DEBUG
    //�����쳣ʸ��������
    taSaveAltiVec();
#endif
#endif

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
#ifdef __PPC__
    taDebugContinue(context);
#endif
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
	sdaCpuAllResume();
#endif

    /* ���ûỰ��״̬ΪDEBUG_STATUS_CONTINUE */
    TA_SET_DEBUG_STATUS(&(taSDASessInfo.sessInfo),DEBUG_STATUS_CONTINUE);

    /* ��յ�ǰ�����쳣���߳�ID */
    TA_SET_CUR_THREAD_ID(&(taSDASessInfo.sessInfo), 0);

	/* ��������Ϊ�ж�ģʽ */
	taSetEndToInt();
	
#ifdef __PPC__
#ifdef TA_ALTIVEC_SUPPORT_DEBUG
	/* �ָ��쳣ʸ�������� */
	taRestoreAltiVec();
#endif
#endif

#if defined __MIPS__ || defined __PPC85XX__
    //�����������
    taDebugSetSoftStep(context);
#endif
    /* �ָ��쳣������ */
    memcpy((void *)context, taSDAExpContextPointerGet(GET_CPU_INDEX()), sizeof(T_DB_ExceptionContext));
}
