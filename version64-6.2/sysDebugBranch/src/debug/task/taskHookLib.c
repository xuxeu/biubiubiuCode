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
 * @file:taskHookLib.c
 * @brief:
 *             <li>任务创建、删除HOOK相关操作</li>
 */

/************************头 文 件*****************************/
#include "taRtIfLib.h"
#include "taskCtrLib.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************实   现********************************/

/*
 * @brief:
 *      任务创建HOOK函数，RTP调试自动绑定RTP创建的任务到RTP调试会话
 * @param[in]: tid: 任务ID
 * @return:
 *     无
 */
static void taTaskCreateHook(ulong_t tid)
{
	UINT32 sid = 0;

	/* 查询当前任务是否调试绑定 */
	if (taTaskIsAttach(tid, &sid))
	{
		/* 绑定任务 */
		taAttachTask(sid, tid);
	}
}

/*
 * @brief:
 *      任务删除HOOK函数，如果是调试任务被删除时自动解除任务绑定
 * @param[in]: tid: 任务ID
 * @return:
 *     无
 */
static void taTaskDeleteHook(ulong_t tid)
{
	/* 解绑任务 */
	taDetachTask(tid);
}

/**
 * @brief:
 * 	   初始化任务HOOK，挂载任务创建HOOK和任务删除HOOK
 * @return:
 * 	   无
 */
void taTaskHookInit(void)
{
    static BOOL	initialized = FALSE;

    /* 初始化任务HOOK */
    if (!initialized)
	{
    	TA_RT_CREATE_HOOK_ADD((FUNCPTR)taTaskCreateHook);
    	TA_RT_DELETE_HOOK_ADD((FUNCPTR)taTaskDeleteHook);
        initialized = TRUE;
	}
}
