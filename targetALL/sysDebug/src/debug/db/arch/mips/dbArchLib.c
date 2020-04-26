/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  dbArchLib.c
 * @brief:
 *             <li>ARCH相关的异常处理</li>
 */
/************************头文件******************************/
#include "dbAtom.h"
#include "taUtil.h"
#include "ta.h"
#include <taskLibCommon.h>

/************************宏定义******************************/

/************************类型定义****************************/

/************************全局变量****************************/

/************************前向声明****************************/
/* 异常上下文 */
extern T_DB_ExceptionContext taSDASmpExpContext[MAX_CPU_NUM];

/************************模块变量****************************/
/************************函数实现*****************************/

#ifdef _KERNEL_DEBUG_

/*
 * @brief:
 *      体系结构相关的异常处理入口函数
 * @param[in]: vector: 异常号
 * @return:
 *     无
 */
void taAachExceptionHandler(UINT32 vector)
{
	T_DB_ExceptionContext *context = taSDAExpContextPointerGet(GET_CPU_INDEX());

    /* 调用通用异常处理函数  */
	taExceptionHandler(vector, context);
}

#else

/*
 * @brief:
 *      体系结构相关的异常处理入口函数
 * @param[in]: vector: 异常号
 * @param[in]: pInfo: 异常产生时CPU压栈的上下文
 * @param[in]: pRegs: 栈上保存的异常上下文
 * @return:
 *     无
 */
void taBpExceptionStub(int vector, ESFMIPS *pInfo, REG_SET *pRegs)
{
	T_DB_ExceptionContext context;

    /* 关CPU中断 */
    TA_INT_CPU_LOCK(0);

    /* 清空异常上下文  */
    ZeroMemory((void *)&context, sizeof(T_DB_ExceptionContext));

	/* 设置状态寄存器 */
	pRegs->cause = pInfo->cause;

    /* 获取异常上下文 */
	taDebugRegsConvert(pRegs, &context);

    /* 调用通用异常处理函数  */
	taExceptionHandler(vector, &context);

    /* 设置异常上下文 */
	taDebugContextConvert(&context, pRegs);
}
#endif
