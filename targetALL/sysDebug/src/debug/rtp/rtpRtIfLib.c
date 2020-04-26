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
 * @file:rtpRtIfLib.c
 * @brief:
 *             <li>RTP运行接口设置</li>
 */

/************************头 文 件*****************************/
#include "taRtIfLib.h"
#include "taRtp.h"
#include "taskCtrLib.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************实   现********************************/

/**
 * @brief:
 * 	  RTP相关运行时接口初始化
 * @return:
 *    无
 */
void taRtpRtIfInit(void)
{
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).resume = (STATUS (*) (int))taTaskAllResume;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).suspend = (STATUS (*) (int))taTaskAllSuspend;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).attach = taRtpTaskAttach;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).detach = taRtpTaskDetach;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).idVerify = (STATUS (*) (int))rtpIdVerify;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).contextGet = taTaskContextGet;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).contextSet = taTaskContextSet;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).resume = NULL;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).suspend = NULL;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).attach = taRtpSystemAttach;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).detach = taRtpSystemDetach;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).idVerify = (STATUS (*) (int))rtpIdVerify;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).contextGet = taTaskContextGet;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).contextSet = taTaskContextSet;
}
