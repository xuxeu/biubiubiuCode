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
 * @file:taskExceptionLib.c
 * @brief:
 *             <li>任务级调试异常处理</li>
 */

/************************头 文 件*****************************/
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

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

#ifdef CONFIG_DELTAOS
/* 告诉调度器当前任务处于异常状态,恢复任务运行时恢复的CS寄存器为sysCsSuper而不是sysCsUser */
#define	TA_BP_HIT_SET(tid)	((tid)->pDbgInfo = (void *)-1);
#define	TA_BP_HIT_CLEAR(tid) tid->pDbgInfo = NULL;
#endif

/************************实   现********************************/

/*
 * @brief:
 *      任务级异常处理
 * @param[in]: vector: 异常号
 * @param[in]: context: 异常上下文
 * @return:
 *     无
 */
void taTaskExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context)
{
	ulong_t tid = 0;
	UINT32 sid = 0;
	TA_CTX_TYPE ctxType = 0;

	/* 是否需要处理断点跳过 */
    if (TA_DO_IGNORE())
    {
    	/* 处理断点跳过 */
    	taTaskBpDoneIgnore(context);

    	/* 允许其它任务运行 */
    	taTaskSchedUnlock();
    	return;
	}

	/* 获取当前任务ID */
	tid = taTaskIdSelf();

	/* 查找任务会话ID */
	SLL_NODE* taskNode = taTaskNodeFind(tid, &sid);
	if (NULL == taskNode)
	{
		/* 阻止其它任务运行 */
		taTaskSchedLock();

		/* 任务未绑定,忽略断点  */
		taTaskBpDoIgnore(context);

		return;
	}

	/* 禁止本地CPU任务抢占 */
	taTaskCpuLock();

#ifdef CONFIG_DELTAOS
	/* 设置断点命中 */
	TA_BP_HIT_SET(taskTcb(tid));
#endif

	/* 设置当前异常线程ID */
	TA_SET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid), tid);

	/* 设置会话的状态为DEBUG_STATUS_EXCEPTION */
	TA_SET_DEBUG_STATUS(TA_GET_SESSION_INFO(sid),DEBUG_STATUS_EXCEPTION);

    /* 保存异常上下文  */
    memcpy((void *)TA_GET_EXP_CONTEXT(sid), context, sizeof(T_DB_ExceptionContext));

    /* 拷贝上下文,用于RSP协议命令读取 */
    memcpy((void *)&taGdbGetExpContext, context, sizeof(T_DB_ExceptionContext));

	/* 设置网卡收包模式为轮询,中断模式下WDB收包接口为阻塞接口，该接口会等待信号量 */
	taSetEndToPoll();

	/* 上报调试异常给GDB */
	taReportExceptionToDebugger(TA_GET_DEBUG_SESSION(sid), vector);

	/* 设置网卡为中断模式 */
	taSetEndToInt();

	/* 获取上下文类型 */
	ctxType = TA_GET_CONTEXT_TYPE(sid);

	/* 开任务调度 */
	taTaskCpuUnlockNoResched();

	/* 如果RTP调试挂起RTP所有任务，任务级调试挂起绑定的调试任务 */
	TA_RT_CTX_SUSPEND(ctxType,sid);

    /* 恢复异常上下文 */
    memcpy((void *)context, TA_GET_EXP_CONTEXT(sid), sizeof(T_DB_ExceptionContext));

    /* 清空当前产生异常的线程ID */
    TA_SET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid), 0);

	/* 设置会话的状态为DEBUG_STATUS_CONTINUE */
	TA_SET_DEBUG_STATUS(TA_GET_SESSION_INFO(sid),DEBUG_STATUS_CONTINUE);

#ifdef CONFIG_DELTAOS
    TA_BP_HIT_CLEAR(taskTcb(tid));
#endif
}
