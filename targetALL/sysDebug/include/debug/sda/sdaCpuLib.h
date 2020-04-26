/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-23         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file  sdaCpuLib.h
 * @brief
 *       功能:
 *       <li>CPU控制相关函数声明</li>
 */
#ifdef CONFIG_CORE_SMP

#ifndef _SDA_CPU_LIB_H_
#define _SDA_CPU_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "sdaCpuset.h"
#include "ta.h"

/************************宏定义********************************/

/* 延时函数宏定义 */
#define DEBUG_CPU_DELAY()

/* 调试IPI中断号 */
#define TA_IPI_INTR_ID_DEBUG  (1)

/* IPI中断投递等待时间 */
#define TA_NO_WAIT	      (0)
#define TA_WAIT_FOREVER   (-1)

/************************类型定义******************************/

/* IPI相关函数指针 */
typedef void (*IPI_HANDLER_FUNC)(void);
typedef INT32 (*IPI_INSTALL_HANDLER_FUNC)(INT32 ipiId, IPI_HANDLER_FUNC ipiHandler,void *ipiArg);
typedef INT32 (*IPI_EMIT_HANDLER_FUNC)(INT32 ipiId, UINT32 cpus);
typedef INT32 (*IPI_ENABLE_FUNC)(INT32 ipiId);

/************************接口声明******************************/

/*
 *@brief
 *     功能:IPI初始化，挂接调试处理核间中断
 *@return
 *    TA_OK:执行成功
 *    DA_CONNECT_IPI_FAILED:挂接核间中断处理函数失败
 *	  DA_ENABLE_IPI_FAILED:使能核间中断失败
 */

T_TA_ReturnCode sdaIpiInit(IPI_INSTALL_HANDLER_FUNC IPI_CONNECT, IPI_EMIT_HANDLER_FUNC IPI_EMIT, IPI_ENABLE_FUNC IPI_ENABLE);

/*
 *@brief
 *     功能:挂起所有CPU
 *@return
 *    TA_OK:执行成功
 *    TA_DA_IPI_NO_INIT:IPI未初始化化
 *	  TA_DA_IPI_EMIT_FAILED:投递虚中断失败
 */
T_TA_ReturnCode sdaCpuAllSuspend(void);

/*
 *@brief
 *     功能:恢复所有CPU
 *@return
 *    TA_OK:执行成功
 *    TA_DA_IPI_NO_INIT:IPI未初始化化
 *	  TA_DA_ILLEGAL_OPERATION:非法操作
 */
T_TA_ReturnCode sdaCpuAllResume(void);

/*
 *@brief
 *     功能:通过线程ID获取CPU ID
 *@return
 *    如果成功返回CPU ID，否则返回0xffffffff
 */
UINT32 sdaGetCpuIdByThreadId(UINT32 threadId);

/*
 *@brief
 *     功能:获取处理器ID
 *@return
 *    成功：处理器ID
 */
UINT32 GET_CPU_INDEX();

/*
 *@brief
 *     功能:IPI初始化，挂接调试处理核间中断
 * @param[in]:func:核间中断调用该函数
 * @param[in]:timeout:超时时间
 *@return
 *    TA_OK:执行成功
 *    TA_IPI_EMIT_FAILED:投递核间中断失败
 */
T_TA_ReturnCode sdaIpiEmit(IPI_HANDLER_FUNC	func, UINT32 timeout);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SDA_CPU_LIB_H_ */
#endif /* CONFIG_CORE_SMP */
