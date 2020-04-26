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
 * @file: taskSessionLib.h
 * @brief:
 *             <li>会话管理</li>
 */
#ifndef _TASK_SESSION_LIB_H_
#define _TASK_SESSION_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头 文 件******************************/
#include "taErrorDefine.h"

/************************类型定义*****************************/

/************************宏 定 义******************************/

/* 调试通道起始号 */
#define TA_DEBUG_CHANNEL_START  6

/* 调试通道结束号 */
#define TA_DEBUG_CHANNEL_END   37

/* 根据会话ID获取调试通道 */
#define TA_GET_CHANNEL_ID(sid) ((sid)+TA_DEBUG_CHANNEL_START)

/* 根据调试通道ID获取会话ID */
#define TA_GET_SESSION_ID(cid) ((cid)-TA_DEBUG_CHANNEL_START)

/* 调试通道号是否无效 */
#define TA_IS_INVALID_CHANNEL(sid) (((sid)<(UINT32)(TA_DEBUG_CHANNEL_START)) || ((sid)>(UINT32)(TA_DEBUG_CHANNEL_END)))

/* 最大的会话个数 */
#define TA_MAX_SESS_NUM 32

/* 获取任务链表 */
#define TA_GET_TASK_LIST(sid) (&(taSessionTable[(sid)].sessInfo.taskList))

/* 获取会话信息 */
#define TA_GET_SESSION_INFO(sid) (&(taSessionTable[(sid)].sessInfo))

/* 获取异常上下文 */
#define TA_GET_EXP_CONTEXT(sid) ((taSessionTable[(sid)].sessInfo.context))

/* 调试会话是否绑定 */
#define TA_SESSION_IS_ATTACH(sid) ((DEBUG_STATUS_ATTACH == taSessionTable[(sid)].sessInfo.attachStatus))

/* 调试对象是否绑定 */
#define TA_OBJECT_IS_ATTACH(objId,sid) ((objId) == taSessionTable[(sid)].sessInfo.objID)

/* 根据会话ID获取上下文ID */
#define TA_GET_CONTEXT_ID(sid) (taSessionTable[(sid)].sessInfo.objID)

/* 获取上下文类型 */
#define TA_GET_CONTEXT_TYPE(sid) (taSessionTable[(sid)].sessInfo.ctxType)

/* 获取调试会话 */
#define TA_GET_DEBUG_SESSION(sid) (&taSessionTable[(sid)])

/* 任务是否自动绑定  */
#define TA_TASK_IS_AUTO_ATTACH(sid) (taSessionTable[(sid)].sessInfo.taskIsAutoAttach)

/************************接口声明*****************************/

/* 调试会话表 */
extern T_TA_Session taSessionTable[TA_MAX_SESS_NUM];

/*
 * @brief:
 *     初始化调试会话
 * @return:
 *     无
 */
void taSessionInit(void);

/*
 * @brief:
 *      分配调试会话
 * @param[in]: objId: 调试的对象ID，实时进程调试为实时进程ID,核心任务调试为kernelId
 * @param[out]: pSid: 会话ID
 * @return:
 *     TA_OK:分配调试会话成功
 *     TA_SESSION_FULL:调试会话分配完
 */
T_TA_ReturnCode taSessionMalloc(TA_CTX_TYPE ctxType, UINT32 objId, UINT32 *pSid);

/*
 * @brief:
 *      释放调试会话
 * @param[in]: sid: 会话ID
 * @return:
 *     无
 */
void taSessionFree(UINT32 sid);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_SESSION_LIB_H_ */
