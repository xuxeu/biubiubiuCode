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
 * @file:taskBpIgnoreLib.h
 * @brief:
 *             <li>断点忽略相关宏定义和函数声明</li>
 */
#ifndef _TASK_BP_IGNORE_LIB_H_
#define _TASK_BP_IGNORE_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头 文 件******************************/
#include "taTypes.h"
#include "ta.h"

/************************类型定义*****************************/

/************************宏 定 义******************************/

#ifdef	CONFIG_CORE_SMP
/* 断点忽略标志相关操作宏定义 */
#define TA_DO_IGNORE() \
		taDoIgnore[GET_CPU_INDEX()]
#define TA_DO_IGNORE_SET() \
		(taDoIgnore[GET_CPU_INDEX()] = TRUE)
#define TA_DO_IGNORE_CLR() \
		(taDoIgnore[GET_CPU_INDEX()] = FALSE)
#else
#define TA_DO_IGNORE()		taDoIgnore
#define TA_DO_IGNORE_SET()  (taDoIgnore = TRUE)
#define TA_DO_IGNORE_CLR()	(taDoIgnore = FALSE)
#endif

/************************接口声明*****************************/

#ifdef	CONFIG_CORE_SMP
/* 断点忽略标志 */
extern BOOL taDoIgnore[MAX_CPU_NUM];
#else
extern BOOL taDoIgnore;
#endif

/*
 * @brief:
 *     通过单步跳过，忽略当前断点
 * @return:
 *     TA_OK：断点忽略成功
 */
T_TA_ReturnCode taTaskBpDoIgnore(T_DB_ExceptionContext *context);

/*
 * @brief:
 *     完成"taTaskBpDoIgnore"函数的断点跳过功能
 * @return:
 *     TA_OK：完成断点忽略
 */
T_TA_ReturnCode taTaskBpDoneIgnore(T_DB_ExceptionContext *context);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_BP_IGNORE_LIB_H_ */
