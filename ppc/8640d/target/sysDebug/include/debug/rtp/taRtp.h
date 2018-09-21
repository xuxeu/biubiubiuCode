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
 * @file: taRtp.h
 * @brief:
 *             <li>会话管理</li>
 */
#ifndef _TA_RTP_H_
#define _TA_RTP_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头 文 件******************************/
#include <vxWorks.h>
#include <rtpLibP.h>
#include <rtpUtilLibP.h>
#include "taRtIfLib.h"
#include "ta.h"

/************************类型定义*****************************/

/************************宏 定 义******************************/

/* 是否支持RTP */
#define TA_RTP_IS_SUPPORED (_func_rtpSpawn != NULL)

/* 是否支持内存切换 */
#define TA_MEM_SWITCH_IS_SUPPORED() (_func_taskMemCtxSwitch == NULL)

/* 通过任务ID获取实时进程ID */
#define	taRtpFromTaskGet(taskId) ((UINT32)(taskTcb(taskId)->rtpId))

/************************接口声明*****************************/

/* 核心ID */
extern RTP_ID kernelId;

/*
 * @brief:
 *     RTP调试内存操作接口初始化
 * @return:
 *     TA_OK:   操作成功
 */
T_TA_ReturnCode taRtpMemoryInit(void);

/**
 * @brief:
 * 	  RTP相关运行时接口初始化
 * @return:
 *    无
 */
void taRtpRtIfInit(void);

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
UINT32 taRtpTaskAttach(TA_CTX_TYPE ctxType, UINT32 objId, UINT32 *pSid);

/*
 * @brief:
 *      调试解绑RTP
 * @param[in]: sid: 会话ID
 * @return:
 *     TA_OK: 解绑RTP成功
 */
UINT32 taRtpTaskDetach(UINT32 sid);

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
UINT32 taRtpSystemAttach(TA_CTX_TYPE ctxType, UINT32 objId, UINT32 *pSid);

/*
 * @brief:
 *      系统级调试解绑RTP
 * @param[in]: sid: 会话ID
 * @return:
 *     TA_OK: 解绑RTP成功
 */
UINT32 taRtpSystemDetach(UINT32 sid);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TA_RTP_H_ */
