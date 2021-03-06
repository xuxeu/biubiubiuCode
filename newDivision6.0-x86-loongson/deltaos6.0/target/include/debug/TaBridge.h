/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2013-06-28         彭元志，北京科银京成技术有限公司
 *                               创建该文件。
*/

/*
 * @file： taBridge.h
 * @brief：
 *	    <li>桥结构体定义，操作系统启动后使用到的操作系统相关接口</li>
 */

#ifndef _TA_BRIDGE_H
#define _TA_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************头文件********************************/
#include "sdaCpuLib.h"
#include "dbContext.h"

/************************宏定义********************************/

#ifdef __MIPS__
/* 桥初始化函数地址存放于此 */
#define TA_BRIGDE_ADDR 0x81EFFFE0

/* TA是否启动标志地址 */
#define TA_START_FLAG_ADDR 0x81EFFFF0

#elif defined(__X86__)
/* 桥初始化函数地址存放于此 */
#define TA_BRIGDE_ADDR 0x14FFF8

/* TA是否启动标志地址 */
#define TA_START_FLAG_ADDR 0x14FFF0
#endif

/* TA是否启动标志值 */
#define TA_START_FLAG_VALUE 0x55aa55aa
/************************类型定义******************************/

/* 操作系统相关接口定义 */
typedef struct
{
	T_TA_ReturnCode (*sdaIpiInit)(TA_IPI_INSTALL_HANDLER_FUNC IPI_CONNECT, TA_IPI_EMIT_HANDLER_FUNC IPI_EMIT, TA_IPI_ENABLE_FUNC IPI_ENABLE);
	void  (*taTraceExceptionStub)(void); 
	void  (*taBpExceptionStub)(void); 
	int   (*taskIdSelf)(void); 
	int   (*taskIdVerify)(int tid); 
	char* (*taskName)(int tid);
	int   (*taskIdListGet)(int idList[],int maxTasks);
	int   (*taskContextGet)(int tid, T_DB_ExceptionContext *context);
	int   (*taskContextSet)(int tid, T_DB_ExceptionContext *context);
	cpuset_t (*cpuEnabledGet)(void);
	void (*cpuCtrlHandler)(T_DB_ExceptionContext *context);
	unsigned int (*getCpuID)(void);
	STATUS (*taMemProbeInit)(FUNCPTR *_func_excBaseHook);
	int (*taUdpRead)(UINT8* buf,UINT32 cnt,UINT32* dlen);
	int (*taUdpWrite)(UINT8* buf,UINT32 cnt,UINT32* dlen);
	void (*TaPacketNumGet)(UINT32* pTaTxPacketNum, UINT32* pTaRxPacketNum);
	int (*taGetSaid)();
	unsigned int* (*taGetBreakInfo)();
	BOOL (*taGetMultiThreadInfoTag)();
	void (*printUart)(char *fmt, ...);
}T_TA_BRIDGE;

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _TA_BRIDGE_H */
