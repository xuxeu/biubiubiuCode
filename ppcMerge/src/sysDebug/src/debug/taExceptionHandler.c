/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-28         彭元志，北京科银京成技术有限公司
 *                         创建该文件。
 */

/**
 * @file:taExceptionHandler.c
 * @brief:
 *             <li>调试异常处理函数公共入口</li>
 */

/************************头 文 件******************************/
#include "taDebugMode.h"
#include "dbContext.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/* 系统异常处理函数指针 */
void (*_func_taSysExceptionHandler) (UINT32 vector, T_DB_ExceptionContext *context) = NULL;

/* 任务级异常处理函数指针 */
void (*_func_taTaskExceptionHandler) (UINT32 vector, T_DB_ExceptionContext *context) = NULL;

/************************函数实现*****************************/

/*
 * @brief:
 *      异常处理功公共入口,根据调试模式和安装的异常处理函数进行异常处理
 * @param[in]: vector: 异常号
 * @param[in]: context: 异常上下文
 * @return:
 *     无
 */
void taExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context)
{
	/* 系统级异常处理 */
	if((DEBUGMODE_SYSTEM == taDebugModeGet()) && (_func_taSysExceptionHandler != NULL))
	{
		_func_taSysExceptionHandler(vector, context);
	}

	/* 任务级异常处理 */
	if((DEBUGMODE_TASK == taDebugModeGet()) && (_func_taTaskExceptionHandler != NULL))
	{
		_func_taTaskExceptionHandler(vector, context);
	}
}
