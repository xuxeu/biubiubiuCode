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
#ifndef _KERNEL_DEBUG_
/************************头 文 件*****************************/
#include "sdaBpIgnoreLib.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

BOOL taSdaDoIgnore = FALSE;

/************************全局变量*****************************/

/************************实   现********************************/

/*
 * @brief:
 *     通过单步跳过，忽略当前断点
 * @return:
 *     TA_OK：断点忽略成功
 */
T_TA_ReturnCode taSdaBpDoIgnore(T_DB_ExceptionContext *context)
{
	/* 禁止断点 */
	taDebugBpAllDisable();

	//在下一条指令出打断点
	SetBreakAtNextPc(context);

	/* 设置断点忽略标志 */
	taSdaDoIgnoreSet();

	return TA_OK;
}

/*
 * @brief:
 *     完成"taTaskBpDoIgnore"函数的断点跳过功能
 * @return:
 *     TA_OK：完成断点忽略
 */
T_TA_ReturnCode taSdaBpDoneIgnore(T_DB_ExceptionContext *context)
{

	//删除当前断点
	ClearBreakAtCurPc();

	/* 清除断点忽略标志 */
	taSdaDoIgnoreClr();

	/* 禁止断点 */
	taDebugBpAllDisable(); //删除gdb在断点跳过的过程中打的断点
	
	/* 使能断点 */
	taDebugBpAllEnable();
	
	return TA_OK;
}

/*
 * @brief:
 *     获取忽略标志
 * @return:
 *     忽略标志的值
 */
BOOL taSdaDoIgnoreGet()
{
	return taSdaDoIgnore;
}

/*
 * @brief:
 *     置位忽略标志
 * @return:
 *     无
 */
void taSdaDoIgnoreSet()
{
	taSdaDoIgnore = TRUE;
}

/*
 * @brief:
 *     清除忽略标志
 * @return:
 *     无
 */
void taSdaDoIgnoreClr()
{
	taSdaDoIgnore = FALSE;
}
#endif
