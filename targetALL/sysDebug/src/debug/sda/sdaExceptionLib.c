/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * @file:sdaExceptionLib.c
 * @brief:
 *             <li>调试管理异常处理实现</li>
 */

/************************头 文 件******************************/
#include <string.h>
#include "ta.h"
#include "taPacketComm.h"
#include "taMsgDispatch.h"
#include "sdaCpuLib.h"
#include "dbAtom.h"
#include "taExceptionReport.h"
#include "sdaMain.h"
#include "taskBpIgnoreLib.h"
/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/
/*单步断点导致的暂停的任务ID*/
 int stepThreadId;
/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *      系统级调试异常处理程序
 * @param[in]:vector: 调试异常向量号
 * @param[in]: context: 异常上下文
 * @return:
 *      无
 */
void taSDAExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context)
{
#ifdef __MIPS__
#ifndef _KERNEL_DEBUG_
	/* 当前任务是否需要处理断点跳过 */
	if (taSdaDoIgnoreGet())
	{
		//开任务中断允许任务调度
	    context->registers[STATUSREG] = context->registers[STATUSREG] |0x1;
		/* 处理断点跳过 */
		taSdaBpDoneIgnore(context);
		/* 所有CPU退出空转状态 */
		sdaCpuAllResume();
		return;
	}
#endif
#endif
#ifdef CONFIG_CORE_SMP
    /* 其他CPU进入空转状态 */
    sdaCpuAllSuspend();
#endif
#ifdef __MIPS__
#ifndef _KERNEL_DEBUG_

	//由于先获得控制权的任务处理完后会删除Z5打的断点 ，故判断当前PC处是否是断点，过滤掉阻塞在此的任务
    if(TRUE != taDebugBpInstructCheck(context->registers[PC]))
    {
    	//断点已经被删除直接返回运行
    	/* 所有CPU退出空转状态 */
		sdaCpuAllResume();
        return;
    }
    if(taIsSoftStepBreakPoint(context->registers[PC]))
	{
		if(stepThreadId != 0 && stepThreadId != taTaskIdSelf())
		{
			/* 对于软件单步断点，其他核上的任务进入异常后需要跳过断点 */
			//关任务中断阻止任务调度
			context->registers[STATUSREG] = context->registers[STATUSREG] &(~0x1);
			taSdaBpDoIgnore(context);
			return ;
		}
	}
#endif
#endif

	/* 设置网卡收包模式为轮询 */
    taSetEndToPoll();
#if defined __MIPS__ || defined __PPC85XX__
     //判断是否是软件单步
     taDebugClrSoftStep();
#endif
    /* 设置当前产生异常的线程ID */
    TA_SET_CUR_THREAD_ID(taSDADebugInfoGet(), taTaskIdSelf());

    /* 设置当前调试会话上下文指针 */
    taSDASessInfo.sessInfo.context = taSDAExpContextPointerGet(GET_CPU_INDEX());

    /* 保存异常上下文  */
    memcpy((void *)taSDAExpContextPointerGet(GET_CPU_INDEX()), context, sizeof(T_DB_ExceptionContext));

#ifdef __PPC__
#ifdef TA_ALTIVEC_SUPPORT_DEBUG
    //保存异常矢量上下文
    taSaveAltiVec();
#endif
#endif

    /* 拷贝上下文,用于RSP协议命令读取 */
    memcpy((void *)&taGdbGetExpContext, context, sizeof(T_DB_ExceptionContext));

	/* 如果是暂停状态，跳过trap指令 */
    if (TRUE == taDebugGetStopCommandTag())
    {
    	/* 清除暂停标志 */
    	taDebugSetStopCommandTag(FALSE);

    	/* 跳过产生异常指令 */
    	taDebugJumpOverTrap(taSDAExpContextPointerGet(GET_CPU_INDEX()));
    }
#ifdef __PPC__
    taDebugContinue(context);
#endif
    /* 设置会话的状态为DEBUG_STATUS_EXCEPTION */
    TA_SET_DEBUG_STATUS(&(taSDASessInfo.sessInfo),DEBUG_STATUS_EXCEPTION);

	/* 保存<vector>到taSDASessInfo中 */
	taSDASessInfo.sessInfo.context->vector = vector;

	/* 上报调试异常给GDB */
	taReportExceptionToDebugger(&taSDASessInfo, vector);

    /* 启动循环接收消息 */
    taStartLoopDispatch();

    /* 进行循环收包 */
    taMessageDispatch();

#ifdef CONFIG_CORE_SMP
	/* 所有CPU退出空转状态 */
	sdaCpuAllResume();
#endif

    /* 设置会话的状态为DEBUG_STATUS_CONTINUE */
    TA_SET_DEBUG_STATUS(&(taSDASessInfo.sessInfo),DEBUG_STATUS_CONTINUE);

    /* 清空当前产生异常的线程ID */
    TA_SET_CUR_THREAD_ID(&(taSDASessInfo.sessInfo), 0);

	/* 设置网卡为中断模式 */
	taSetEndToInt();
	
#ifdef __PPC__
#ifdef TA_ALTIVEC_SUPPORT_DEBUG
	/* 恢复异常矢量上下文 */
	taRestoreAltiVec();
#endif
#endif

#if defined __MIPS__ || defined __PPC85XX__
    //设置软件单步
    taDebugSetSoftStep(context);
#endif
    /* 恢复异常上下文 */
    memcpy((void *)context, taSDAExpContextPointerGet(GET_CPU_INDEX()), sizeof(T_DB_ExceptionContext));
}
