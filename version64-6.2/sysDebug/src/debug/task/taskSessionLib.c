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
 * @file:taskSessionLib.c
 * @brief:
 *             <li>调试会话管理</li>
 */

/************************头 文 件*****************************/
#include "ta.h"
#include "taskSessionLib.h"
#include "taskCtrLib.h"
#include "taskSllLib.h"
#include "taRtIfLib.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/* 异常上下文 */
static T_DB_ExceptionContext taExpContext[TA_MAX_SESS_NUM];
/************************全局变量*****************************/

/* 调试会话表 */
T_TA_Session taSessionTable[TA_MAX_SESS_NUM];
/************************实   现********************************/

/*
 * @brief:
 *     初始化调试会话
 * @return:
 *     无
 */
void taSessionInit(void)
{
    UINT32 idx = 0;

    /* 清空异常上下文 */
    memset(taExpContext, 0, sizeof(T_DB_ExceptionContext)*TA_MAX_SESS_NUM);

    /* 清空调试会话信息 */
    memset(taSessionTable, 0, sizeof(T_TA_Session)*TA_MAX_SESS_NUM);

    /* 设置调试会话上下文 */
    for (idx = 0; idx < TA_MAX_SESS_NUM; ++idx)
    {
        /* 初始化任务链表 */
    	TA_SLL_INIT(TA_GET_TASK_LIST(idx));

        /* 设置上下文 */
        taSessionTable[idx].sessInfo.context = &taExpContext[idx];

        /* 设置数据包中用户数据起始位置 */
        taSetPktOffset(&(taSessionTable[idx].packet),TA_PROTOCOL_HEAD_SIZE+TA_PROTOCOL_CHECKSUM_SIZE);
    }
}

/*
 * @brief:
 *      分配调试会话
 * @param[in]: objId: 调试的对象ID，实时进程调试为实时进程ID,核心任务调试为kernelId
 * @param[out]: pSid: 会话ID
 * @return:
 *     TA_OK:分配调试会话成功
 *     TA_SESSION_FULL:调试会话分配完
 */
T_TA_ReturnCode taSessionMalloc(TA_CTX_TYPE ctxType, ulong_t objId, UINT32 *pSid)
{
	UINT32 idx = 0;
    UINT32 sid = TA_MAX_SESS_NUM;

	/* 查询空闲的调试会话 */
	for (idx = 0; idx < TA_MAX_SESS_NUM; ++idx)
	{
        /* 设置第一个空闲的调试会话ID */
        if (!TA_SESSION_IS_ATTACH(idx))
        {
        	sid = idx;
        	break;
        }
	}

	/* 调试会话已经分配完 */
	if (sid >= TA_MAX_SESS_NUM)
	{
		return (TA_SESSION_FULL);
	}

	/* 设置会话ID */
	*pSid = sid;

	/* 设置调试对象ID */
	taSessionTable[sid].sessInfo.objID = objId;

    /* 设置上下文 */
    taSessionTable[idx].sessInfo.context = &taExpContext[idx];

	/* 设置会话ID */
	taSessionTable[sid].sessInfo.sessionID = sid;

	/* 设置上下文类型 */
	taSessionTable[sid].sessInfo.ctxType = ctxType;

	/* 默认新创建的任务不自动绑定 */
	taSessionTable[sid].sessInfo.taskIsAutoAttach = FALSE;

	/* 设置调试状态 */
	taSessionTable[sid].sessInfo.status = DEBUG_STATUS_CONTINUE;

	/* 设置绑定状态 */
	taSessionTable[sid].sessInfo.attachStatus = DEBUG_STATUS_ATTACH;

    /* 设置数据包中用户数据起始位置 */
    taSetPktOffset(&(taSessionTable[sid].packet),TA_PROTOCOL_HEAD_SIZE+TA_PROTOCOL_CHECKSUM_SIZE);

	return (TA_OK);
}

/*
 * @brief:
 *      释放调试会话
 * @param[in]: sid: 会话ID
 * @return:
 *     无
 */
void taSessionFree(UINT32 sid)
{
	/* 删除调试会话所有断点 */
	taDebugRemoveAllBp(TA_GET_SESSION_INFO(sid));

	/* 释放任务链表中的任务节点内存 */
	taSllEach(TA_GET_TASK_LIST(sid), taTaskNodeFree, 0);

	/* 初始化任务链表 */
	TA_SLL_INIT(TA_GET_TASK_LIST(sid));

    /* 清空异常上下文 */
    memset(TA_GET_EXP_CONTEXT(sid), 0, sizeof(T_DB_ExceptionContext));

    /* 清空调试会话信息 */
    memset(TA_GET_DEBUG_SESSION(sid), 0, sizeof(T_TA_Session));
}
