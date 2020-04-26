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
 * @file:rtpCtrlLib.c
 * @brief:
 *             <li>rtp控制操作，主要实现RTP调试绑定和RTP调试解绑</li>
 */

/************************头 文 件*****************************/
#include "taRtIfLib.h"
#include "taskCtrLib.h"
#include "taRtp.h"
#include "taskSessionLib.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************实   现********************************/

/*
 * @brief:
 *      绑定RTP所有任务
 * @param[in]: rtpId: 实时进程ID
 * @param[out]: sid: 会话ID
 * @return:
 *     TA_OK: 绑定RTP所有任务成功
 *     TA_INVALID_RTP_ID：无效实时进程ID
 */
static UINT32 taRtpAllTaskAttach(RTP_ID rtpId, UINT32 sid)
{
    WIND_TCB *tid = 0;

	//int taInitTaskId = 0;
	//taInitTaskId = (((WIND_RTP *)rtpId)->initTaskId);


    if (RTP_VERIFIED_LOCK(rtpId) != OK)
    {
    	return (TA_INVALID_RTP_ID);
    }

    FOR_EACH_TASK_OF_RTP(rtpId, tid);
    taAttachTask(sid, (INT32)tid);
    END_FOR_EACH_TASK_OF_RTP;

    RTP_UNLOCK (rtpId);


    return (TA_OK);
}

/*
 * @brief:
 *      任务级调试绑定RTP
 * @param[in]: ctxType: 上下文类型
 * @param[in]: objId: 调试对象ID
 * @param[out]: pSid: 会话ID
 * @return:
 *     TA_OK: 绑定RTP成功
 *     TA_INVALID_RTP_ID：无效ID
 *     TA_ALLOCATE_SESSION_FAIL：分配调试会话失败
 */
UINT32 taRtpTaskAttach(TA_CTX_TYPE ctxType, UINT32 objId, UINT32 *pSid)
{
    T_TA_ReturnCode ret = TA_OK;

    /* 校验RTP ID是否有效 */
	if (OK != TA_RT_CTX_ID_VERIFY(ctxType,objId))
	{
		return (TA_INVALID_RTP_ID);
	}

	/* 分配调试会话 */
	ret = taSessionMalloc(ctxType, objId, pSid);
	if (TA_OK != ret)
	{
		return (TA_ALLOCATE_SESSION_FAIL);
	}

	/* 绑定当前RTP所有任务 */
	taRtpAllTaskAttach((RTP_ID)objId, *pSid);

	/* 安装任务级调试命令处理函数 */
	taInstallMsgProc(TA_GET_CHANNEL_ID(*pSid), taTaskDebugAgent);

    return (TA_OK);
}

/*
 * @brief:
 *      任务级调试解绑RTP
 * @param[in]: sid: 会话ID
 * @return:
 *     TA_OK: 解绑RTP成功
 */
UINT32 taRtpTaskDetach(UINT32 sid)
{
	/* 卸载任务级调试命令处理函数 */
	taInstallMsgProc(TA_GET_CHANNEL_ID(sid), NULL);

    /* 释放调试会话 */
	taSessionFree(sid);

    return (TA_OK);
}

/*
 * @brief:
 *      系统级调试绑定RTP
 * @param[in]: ctxType: 上下文类型
 * @param[in]: objId: 调试对象ID
 * @param[out]: pSid: 会话ID
 * @return:
 *     TA_OK: 绑定RTP成功
 *     TA_INVALID_RTP_ID：无效ID
 */
UINT32 taRtpSystemAttach(TA_CTX_TYPE ctxType, UINT32 objId, UINT32 *pSid)
{
    /* 校验RTP ID是否有效 */
	if (OK != TA_RT_CTX_ID_VERIFY(ctxType,objId))
	{
		return (TA_INVALID_RTP_ID);
	}

    /* 设置调试对象ID */
	taSDASessInfo.sessInfo.objID = objId;

	/* 设置上下文类型 */
	taSDASessInfo.sessInfo.ctxType = ctxType;

	/* 设置为绑定状态 */
	taSDASessInfo.sessInfo.attachStatus = DEBUG_STATUS_ATTACH;

	/* 设置当前线程ID为初始化任务ID */
	taSDASessInfo.sessInfo.curThreadId = (((WIND_RTP *)objId)->initTaskId);

    return (TA_OK);
}

/*
 * @brief:
 *      系统级调试解绑RTP
 * @param[in]: sid: 会话ID
 * @return:
 *     TA_OK: 解绑RTP成功
 */
UINT32 taRtpSystemDetach(UINT32 sid)
{
	/* 清除调试对象ID */
	taSDASessInfo.sessInfo.objID = 0;

	/* 设置上下文类型 */
	taSDASessInfo.sessInfo.ctxType = TA_CTX_SYSTEM;

	/* 设置为解绑状态 */
	taSDASessInfo.sessInfo.attachStatus = DEBUG_STATUS_DETACH;

	/* 设置当前线程为0 */
	taSDASessInfo.sessInfo.curThreadId = 0;

    return (TA_OK);
}
