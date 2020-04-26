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
 * @file:taskRtIfLib.c
 * @brief:
 *             <li>运行时库设置，提供代理的可移植性</li>
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

/**
 * @brief:
 * 	  运行时公共接口初始化
 * @return:
 * 	   无
 */
void taRtLibInit(TA_RT_IF *pRtIf)
{
	pRtIf->createHookAdd = taTaskCreateHookAdd;
	pRtIf->deleteHookAdd = taTaskDeleteHookAdd;
	pRtIf->memalloc = taMalloc;
	pRtIf->memfree = taFree;
	pRtIf->memRead = taDirectReadMemory;
	pRtIf->memWrite = taDirectWriteMemory;
	pRtIf->memCtxSwitch = (UINT32 (*)(UINT32, int))_func_taskMemCtxSwitch;
	pRtIf->pageSize	= 0;
}

/**
 * @brief:
 * 	  任务相关运行时接口初始化
 * @return:
 * 	   无
 */
void taTaskRtIfInit(void)
{
    (taRtIf.taCtxIf[TA_CTX_TASK]).resume = taTaskAllResume;
    (taRtIf.taCtxIf[TA_CTX_TASK]).suspend = taTaskAllSuspend;
    (taRtIf.taCtxIf[TA_CTX_TASK]).attach = taTaskAttach;
    (taRtIf.taCtxIf[TA_CTX_TASK]).detach = taTaskDetach;
    (taRtIf.taCtxIf[TA_CTX_TASK]).idVerify = taTaskIdVerify;
    (taRtIf.taCtxIf[TA_CTX_TASK]).contextGet = taTaskContextGet;
    (taRtIf.taCtxIf[TA_CTX_TASK]).contextSet = taTaskContextSet;
}
