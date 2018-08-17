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
 * @file:taskCtrlLib.c
 * @brief:
 *             <li>任务控制操作，主要实现任务调试绑定和任务调试解绑</li>
 */

/************************头 文 件*****************************/
#include "taRtIfLib.h"
#include "taskSllLib.h"
#include "taskSessionLib.h"
#include "taExceptionReport.h"
#include "taskCtrLib.h"
#include "ta.h"
#ifdef _TA_CONFIG_RTP
#include "taRtp.h"
#endif

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************实   现********************************/

/*
 * @brief:
 *      释放任务节点内存
 * @param[in]: pTaskNode: 任务节点
 * @param[in]: arg: 函数参数
 * @return:
 *     FALSE: 继续操作链表
 */
BOOL taTaskNodeFree(SLL_NODE *pTaskNode, INT32 arg)
{
	/* 释放节点内存 */
	TA_RT_MEM_FREE(pTaskNode);

	return (FALSE);
}

/*
 * @brief:
 *      根据任务ID匹配任务节点
 * @param[in]: pTaskNode: 任务节点
 * @param[in]: tid: 任务ID
 * @return:
 *     FALSE: 任务ID与任务节点不匹配
 *     TRUE: 任务ID与任务节点匹配
 */
BOOL taTaskNodeMatch(SLL_NODE *pTaskNode, INT32 tid)
{
	return (((T_TASK_NODE *)pTaskNode)->taskId == tid);
}

/*
 * @brief:
 *      根据恢复任务节点的任务运行
 * @param[in]: pTaskNode: 任务节点
 * @param[in]: arg: 函数参数
 * @return:
 *     FALSE: 表示恢复链表所有任务运行
 */
BOOL taTaskNodeResume(SLL_NODE *pTaskNode, INT32 arg)
{
	INT32 threadId = arg;

	/* 恢复任务，先恢复调试任务，再恢复其他任务 */
	if(threadId != (((T_TASK_NODE *)pTaskNode)->taskId))
	{
		/* 恢复任务 */
		taTaskResume(((T_TASK_NODE *)pTaskNode)->taskId);
	}

	return (FALSE);
}

/*
 * @brief:
 *      挂起任务节点的任务
 * @param[in]: pTaskNode: 任务节点
 * @param[in]: arg: 函数参数
 * @return:
 *     FALSE: 表示挂起链表所有任务运行
 */
BOOL taTaskNodeSuspend(SLL_NODE *pTaskNode, INT32 arg)
{
	INT32 threadId = arg;

	/* 挂起任务，先挂起其它绑定调试任务，再挂起异常任务，否则可能导致无法挂起其它任务 */
	if(threadId != (((T_TASK_NODE *)pTaskNode)->taskId))
	{
		taTaskSuspend(((T_TASK_NODE *)pTaskNode)->taskId);
	}

	return (FALSE);
}

/*
 * @brief:
 *     任务是否需要绑定，如果创建的新任务为绑定调试的实时进程创建或者绑定的调试任务创建，自动绑定该任务到调试会话中
 * @param[in]: tid: 任务ID
 * @param[out]: pSid: 会话ID
 * @return:
 *     FALSE: 无需绑定任务到调试会话
 *     TRUE：需绑定任务到调试会话
 */
BOOL taTaskIsAttach(int tid, UINT32 *pSid)
{
	/* 查找创建新任务的任务是否绑定调试 */
	SLL_NODE *taskNode = taTaskNodeFind(taTaskIdSelf(), pSid);

	return ((taskNode != NULL) && TA_TASK_IS_AUTO_ATTACH(*pSid)) ? TRUE : FALSE;
}

/*
 * @brief:
 *      查找任务节点
 * @param[in]: tid: 任务ID
 * @param[out]: pSid: 会话ID
 * @return:
 *     NULL:未找到对应的任务节点
 *     T_TASK_NODE:任务节点
 */
SLL_NODE* taTaskNodeFind(INT32 tid, UINT32 *pSid)
{
	SLL_NODE *taskNode = NULL;
	UINT32 idx = 0;

	/* 根据任务ID查找任务节点 */
	for (idx = 0; idx < TA_MAX_SESS_NUM; idx++)
	{
        if(!TA_SESSION_IS_ATTACH(idx))
		{
			continue;
		}

		/* 遍历任务链表 */
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
 *      获取调试会话中绑定的任务ID列表
 * @param[in]: sid: 会话ID
 * @param[out]: idList: 任务ID列表
 * @param[in]: maxTasks: 保存的最大任务个数
 * @return:
 *     任务个数
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
 *      绑定单个任务
 * @param[in]: sid: 会话ID
 * @param[in]: tid: 任务ID
 * @return:
 *     TA_OK:绑定任务成功
 *     TA_INVALID_TASK_ID:无效任务ID
 *     TA_TASK_ALREADY_ATTACH:任务已经绑定
 *     TA_ALLOC_MEM_FAIL:分配内存失败
 */
T_TA_ReturnCode taAttachTask(UINT32 sid, INT32 tid)
{
	T_TASK_NODE *taskNode = NULL;

	/* 校验任务ID */
	if (OK != taTaskIdVerify(tid))
	{
		return (TA_INVALID_TASK_ID);
	}

	/* 查找任务是否已经绑定 */
	taskNode = (T_TASK_NODE *)taTaskNodeFind(tid, &sid);
	if (NULL != taskNode)
	{
		return (TA_TASK_ALREADY_ATTACH);
	}

	/* 分配任务节点 */
	taskNode = TA_RT_MEM_ALLOC(T_TASK_NODE,1);
	if (NULL == taskNode)
	{
		return (TA_ALLOC_MEM_FAIL);
	}

	/* 设置任务ID */
	taskNode->taskId = tid;

	/* 增加任务节点到任务链表尾 */
	TA_SLL_PUT_AT_HEAD(TA_GET_TASK_LIST(sid), (SLL_NODE *)taskNode);

	return (TA_OK);
}

/*
 * @brief:
 *      解绑任务
 * @param[in]: tid: 任务ID
 * @return:
 *     TA_OK:解绑任务成功
 *     TA_TASK_NO_ATTACH:任务未绑定
 */
T_TA_ReturnCode taDetachTask(INT32 tid)
{
	SLL_NODE *deleteNode = NULL;
	UINT32 sid = 0;

    /* 查找任务所在的节点 */
	deleteNode = taTaskNodeFind(tid, &sid);
    if (NULL == deleteNode)
    {
    	return (TA_TASK_NO_ATTACH);
    }

	/* 从任务链表中删除任务节点 */
	TA_SLL_REMOVE(TA_GET_TASK_LIST(sid), deleteNode, taSllPrevious(TA_GET_TASK_LIST(sid),deleteNode));

	/* 释放任务节点内存 */
	taTaskNodeFree(deleteNode, 0);

	return TA_OK;
}

/*
 * @brief:
 *      调试绑定任务
 * @param[in]: ctxType: 上下文类型
 * @param[in]: taskId: 任务ID
 * @param[out]: pSid: 会话ID
 * @return:
 *     TA_OK: 绑定任务成功
 *     TA_INVALID_TASK_ID:无效任务ID
 *     TA_TASK_ALREADY_ATTACH:任务已经绑定
 *     TA_ALLOC_MEM_FAIL:分配内存失败
 *     TA_ALLOCATE_SESSION_FAIL：分配调试会话失败
 */
UINT32 taTaskAttach(TA_CTX_TYPE ctxType, UINT32 taskId, UINT32 *pSid)
{
    UINT32 ret = TA_OK;
#ifdef _TA_CONFIG_RTP
    UINT32 objId = taRtpFromTaskGet(taskId);
#else
    UINT32 objId = 0;
#endif

	/* 分配调试会话 */
    ret = taSessionMalloc(ctxType, objId, pSid);
    if (TA_OK != ret)
    {
    	return (TA_ALLOCATE_SESSION_FAIL);
    }

    /* 绑定任务 */
    ret = taAttachTask(*pSid, taskId);
    if (TA_OK != ret)
    {
    	/* 释放调试会话  */
    	taSessionFree(*pSid);

    	return (ret);
    }

	/* 安装任务级调试命令处理函数 */
	taInstallMsgProc(TA_GET_CHANNEL_ID(*pSid), taTaskDebugAgent);

    return (TA_OK);
}

/*
 * @brief:
 *      调试解绑任务
 * @param[in]: sid: 会话ID
 * @return:
 *     TA_OK: 解绑成功
 */
UINT32 taTaskDetach(UINT32 sid)
{
	/* 卸载任务级调试命令处理函数 */
	taInstallMsgProc(TA_GET_CHANNEL_ID(sid), NULL);

    /* 释放调试会话 */
	taSessionFree(sid);

    return (TA_OK);
}

/*
 * @brief:
 *      运行调试会话任务链表中的所有任务
 * @param[in]: sid: 会话ID
 * @return:
 *     TA_OK: 解绑成功
 */
STATUS taTaskAllResume(INT32 sid)
{
	/* 获取异常任务ID */
	INT32 threadId = TA_GET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid));

	/* 禁止任务调度 */
	taTaskCpuLock();

	/* 恢复任务 */
	taTaskResume(threadId);

	/* 恢复所有任务 */
	taSllEach(TA_GET_TASK_LIST(sid), taTaskNodeResume, threadId);

	/* 使能任务调度 */
	taTaskCpuUnlock();

	return (OK);
}

/*
 * @brief:
 *      暂停调试会话任务链表中的所有任务
 * @param[in]: sid: 会话ID
 * @return:
 *     TA_OK: 解绑成功
 */
STATUS taTaskAllSuspend(INT32 sid)
{
	/* 获取异常任务ID */
	INT32 threadId = TA_GET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid));

	/* 挂起其它绑定调试任务 */
	taSllEach(TA_GET_TASK_LIST(sid), taTaskNodeSuspend, threadId);

	/* 挂起异常任务 */
	taTaskSuspend(threadId);

	return (OK);
}

/*
 * @brief:
 *      任务级调试暂停
 * @param[in]: sid: 会话ID
 * @return:
 *     ERROR: 暂停失败
 *     OK: 暂停成功
 */
STATUS taTaskPause(INT32 sid)
{
	T_TASK_NODE *taskNode = (T_TASK_NODE *)TA_SLL_FIRST(TA_GET_TASK_LIST(sid));

	/* 调试任务已经被删除 */
	if(NULL == taskNode)
	{
		return(ERROR);
	}

	/* 设置当前异常线程ID */
	TA_SET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid), taskNode->taskId);

	/* 暂停调试会话所有任务 */
	taTaskAllSuspend(sid);

	/* 获取任务上下文 */
	if (OK != taTaskContextGet(taskNode->taskId, TA_GET_EXP_CONTEXT(sid)))
	{
		/* 清空当前异常线程ID */
		TA_SET_CUR_THREAD_ID(TA_GET_SESSION_INFO(sid), 0);

		/* 恢复所有任务运行 */
		taTaskAllResume(sid);

		return(ERROR);
	}

    /* 拷贝上下文,用于RSP协议命令读取 */
    memcpy((void *)&taGdbGetExpContext, TA_GET_EXP_CONTEXT(sid), sizeof(T_DB_ExceptionContext));

	/* 设置会话的状态为调试暂停态 */
	TA_SET_DEBUG_STATUS(TA_GET_SESSION_INFO(sid),DEBUG_STATUS_PAUSE);

	return (OK);
}
