/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-12-17         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:taskBpIgnoreLib.c
 * @brief:
 *             <li>断点忽略实现，非绑定任务遇见断点需断点跳过，忽略该断点</li>
 */

/************************头 文 件*****************************/
#include "taskBpIgnoreLib.h"
#include "ta.h"

/************************宏 定 义******************************/

#ifdef	CONFIG_CORE_SMP
/* 断点忽略标志 */
BOOL taDoIgnore[MAX_CPU_NUM];
#else
BOOL taDoIgnore = FALSE;
#endif

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************实   现********************************/

/*
 * @brief:
 *     通过单步跳过，忽略当前断点
 * @return:
 *     TA_OK：断点忽略成功
 */
T_TA_ReturnCode taTaskBpDoIgnore(T_DB_ExceptionContext *context)
{
	/* 禁止断点 */
	taDebugBpAllDisable();

#if	TA_SINGLE_STEP
	/* 设置单步位 */
	taDebugStep(context);
#endif

	/* 设置断点忽略标志 */
	TA_DO_IGNORE_SET();

	return TA_OK;
}

/*
 * @brief:
 *     完成"taTaskBpDoIgnore"函数的断点跳过功能
 * @return:
 *     TA_OK：完成断点忽略
 */
T_TA_ReturnCode taTaskBpDoneIgnore(T_DB_ExceptionContext *context)
{
#if	TA_SINGLE_STEP
	/* 清除单步位 */
	taDebugContinue(context);
#endif

	/* 清除断点忽略标志 */
	TA_DO_IGNORE_CLR();

	/* 使能断点 */
	taDebugBpAllEnable();

	return TA_OK;
}
