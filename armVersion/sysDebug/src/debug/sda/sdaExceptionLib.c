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
//ϵͳ�����ԣ������ֵ��˺Ͷ�˵ģ���ͨ��CONFIG_CORE_SMP�������֡�
//Ŀ����Ϊ��������ϵͳͣ����������ˣ�������CPU�����ת��
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
	int ret = 11;

	ret = intCpuLock();

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
    //printk("-----taSDAExceptionHandler----7777--------\n");
    taStartLoopDispatch();

    /* ����ѭ���հ� */
    taMessageDispatch();
    //printk("-----taSDAExceptionHandler----8888--------\n");

#ifdef CONFIG_CORE_SMP
	/* ����CPU�˳���ת״̬ */
if(taCpuEnabledGet()>1)
{ 
	sdaCpuAllResume();    
}
#endif

    /* ���ûỰ��״̬ΪDEBUG_STATUS_CONTINUE */
    //printk("-----taSDAExceptionHandler----9999--------\n");

    TA_SET_DEBUG_STATUS(&(taSDASessInfo.sessInfo),DEBUG_STATUS_CONTINUE);

    /* ��յ�ǰ�����쳣���߳�ID */
    //printk("-----taSDAExceptionHandler----1010--------\n");
    TA_SET_CUR_THREAD_ID(&(taSDASessInfo.sessInfo), 0);

	/* ��������Ϊ�ж�ģʽ */
    //printk("-----taSDAExceptionHandler----1212--------\n");
    intCpuUnlock(ret);
	taSetEndToInt();

    /* �ָ��쳣������ */
    //int i = 0;
    //for(i=0;i<=26;i++)
    //{
    //    printk("--------context[%d]:0x%x--------\n",i,context[i]);
    //}
    //
    //printk("-----taSDAExceptionHandler----1313--------\n");
    memcpy((void *)context, taSDAExpContextPointerGet(GET_CPU_INDEX()), sizeof(T_DB_ExceptionContext));

    //for(i=0;i<=26;i++)
    //{
    //    printf("--------context[%d]:0x%x--------\n",i,context[i]);
    //}

    
    //printk("-----taSDAExceptionHandler----1414--------\n");
}
