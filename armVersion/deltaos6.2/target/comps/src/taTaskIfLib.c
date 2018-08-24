/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * 修改记录：
 * 2016-07-11	李燕，北京科银京成技术有限公司
 *				创建该文件。
 */

/**
 * @file:taTaskIfLib.c
 * @brief:
 *             <li>任务相关接口封装。</li>
 */

/************************头 文 件******************************/
#include <vxWorks.h>
#include <taskLib.h>
#include <taskHookLib.h>
#include <taskLibCommon.h>
#include <cacheLib.h>
#include <vxCpuLib.h>
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/* 核间中断处理函数 */
static void (*taCpuCtrlHandle)(void) = NULL;

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 *@brief
 *     上下文转换，把任务上下文转换为调试上下文
 * @param[in] context:调试上下文
 * @param[in] pRegSet:REG_SET上下文
 *@return
 *    无
 */
void taDebugRegsConvert(REG_SET *pRegSet, T_DB_ExceptionContext *context)
{
#ifdef __X86__
#if(CONFIG_ENABLE_LP64 == true)
	/* 保存通用寄存器 */
	context->registers[R15] = pRegSet->r15;
	context->registers[R14] = pRegSet->r14;
	context->registers[R13] = pRegSet->r13;
	context->registers[R12] = pRegSet->r12;
	context->registers[R11] = pRegSet->r11;
	context->registers[R10] = pRegSet->r10;
	context->registers[R9]	= pRegSet->r9;
	context->registers[R8]	= pRegSet->r8;
	context->registers[RDI] = pRegSet->rdi;
	context->registers[RSI] = pRegSet->rsi;
	context->registers[RBP] = pRegSet->rbp;
	context->registers[RSP] = pRegSet->rsp;
	context->registers[RBX] = pRegSet->rbx;
	context->registers[RDX] = pRegSet->rdx;
	context->registers[RCX] = pRegSet->rcx;
	context->registers[RAX] = pRegSet->rax;
	context->registers[RFLAGS] = pRegSet->rflags;
	context->registers[PC] = (ulong_t)pRegSet->pc;

#else
	context->registers[EDI] = pRegSet->edi;
	context->registers[ESI] = pRegSet->esi;
	context->registers[EBP] = pRegSet->ebp;
	context->registers[ESP] = pRegSet->esp;
	context->registers[EBX] = pRegSet->ebx;
	context->registers[EDX] = pRegSet->edx;
	context->registers[ECX] = pRegSet->ecx;
	context->registers[EAX] = pRegSet->eax;
	context->registers[EFLAGS] = pRegSet->eflags;
	context->registers[PC] = (UINT32)pRegSet->pc;
#endif
#endif
}

/*
 *@brief
 *     上下文转换，把调试上下文转换为任务上下文
 * @param[in] context:调试上下文
 * @param[in] pRegSet:REG_SET上下文
 *@return
 *    无
 */
void taDebugContextConvert(T_DB_ExceptionContext *context, REG_SET *pRegSet)
{
#ifdef __X86__
#if(CONFIG_ENABLE_LP64 == true)
	/* 恢复通用寄存器 */
	pRegSet->r15 = context->registers[R15];
	pRegSet->r14 = context->registers[R14];
	pRegSet->r13 = context->registers[R13];
	pRegSet->r12 = context->registers[R12];
	pRegSet->r11 = context->registers[R11];
	pRegSet->r10 = context->registers[R10];
	pRegSet->r9 = context->registers[R9];
	pRegSet->r8 = context->registers[R8];
	pRegSet->rdi = context->registers[RDI];
	pRegSet->rsi = context->registers[RSI];
	pRegSet->rbp = context->registers[RBP];
	pRegSet->rbx = context->registers[RBX];
	pRegSet->rdx = context->registers[RDX];
	pRegSet->rcx = context->registers[RCX];
	pRegSet->rax = context->registers[RAX];
	pRegSet->rflags = context->registers[RFLAGS];
	pRegSet->pc = (INSTR*)context->registers[PC];		

#else
	pRegSet->edi = context->registers[EDI];
	pRegSet->esi = context->registers[ESI];
	pRegSet->ebp = context->registers[EBP];
	pRegSet->esp = context->registers[ESP];
	pRegSet->ebx = context->registers[EBX];
	pRegSet->edx = context->registers[EDX];
	pRegSet->ecx = context->registers[ECX];
	pRegSet->eax = context->registers[EAX];
	pRegSet->eflags = context->registers[EFLAGS];
	pRegSet->pc = (INSTR*)context->registers[PC];
#endif
#endif
}

/*
 * @brief:
 *     根据任务ID获取任务名
 * @param[in]:taskID:任务ID
 * @return:
 *     OK:获取任务名成功
 *     ERROR:获取任务名失败
 */
char *taTaskName(ulong_t taskID)
{
	return taskName((TASK_ID)taskID);
}

/*
 * @brief:
 *     获取当前任务ID(当前cpu上运行的)
 * @return:
 *     当前任务ID
 */
ulong_t taTaskIdSelf(void)
{
	return (ulong_t)taskIdSelf();
}

/*
 * @brief:
 *     获取系统中任务列表
 * @param[out]:idList:任务ID列表
 * @param[in]:maxTasks:任务列表能够容纳的最大任务数
 * @return:
 *     任务列表中的任务个数
 */
int taTaskIdListGet(ulong_t idList[],int maxTasks)
{
	return taskIdListGet((TASK_ID)idList,maxTasks);
}

/*
 * @brief:
 *     获取任务上下文
 * @param[in]:taskId:任务ID
 * @param[out]:context:任务上下文
 * @return:
 *     OK：获取上下文成功
 *     ERROR：获取上下文失败
 */
int taTaskContextGet(ulong_t taskId, T_DB_ExceptionContext *context)
{
	REG_SET regs;
	STATUS status = OK;

	/* 获取任务上下文 */
	status = taskRegsGet((TASK_ID)taskId, &regs);

	/* 上下文转换，把任务上下文转换为调试上下文 */
	taDebugRegsConvert(&regs, context);

	return status;
}

/*
 * @brief:
 *     设置任务上下文
 * @param[in]:taskId:任务ID
 * @param[in]:context:任务上下文
 * @return:
 *     OK：设置上下文成功
 *     ERROR：设置上下文失败
 */
int taTaskContextSet(ulong_t taskId, T_DB_ExceptionContext *context)
{
	REG_SET regs;

	/* 上下文转换，把调试上下文转换为任务上下文 */
	taDebugContextConvert(context, &regs);

	return taskRegsSet((TASK_ID)taskId, &regs);
}

/*
 * @brief:
 *     获取OS中使能的CPU
 * @return:
 *     OS中使能的CPU
 */
cpuset_t taCpuEnabledGet (void)
{
    return vxCpuEnabledGet();
}

/*
 * @brief:
 *     获取当前CPU ID
 * @return:
 *     CPU ID
 */
UINT32 taGetCpuID(void)
{
    return vxCpuIndexGet();
}

/*
 * @brief:
 *     校验任务是否存在
 * @param[in]:taskId:任务ID
 * @return:
 *     OK:任务存在
 *     ERROR:任务不存在
 */
int taTaskIdVerify(ulong_t taskId)
{
	return taskIdVerify((TASK_ID)taskId);
}

/*
 * @brief:
 *     刷新Cache
 * @param[in]:address:刷新内存地址
 * @param[in]:size:刷新内存大小
 * @return:
 *     无
 */
void taCacheTextUpdata(ulong_t address, UINT32 size)
{
    CACHE_TEXT_UPDATE(address, size);
}

/*
 * @brief:
 *     安装调试IPI中断处理函数
 * @param[in]:pFunction:函数地址
 * @return:
 *     无
 */
void taInstallCpuCtrlHandle(void* pFunction)
{
	taCpuCtrlHandle = pFunction;
}
#ifdef __X86__
#ifdef CONFIG_USR_DEBUG
/*
 * @brief:
 *     保存核间中断上下文
 * @param[in]:pRegSet:核间中断上下文
 * @return:
 *     无
 */

void intVxdbgCpuRegsGet(REG_SET *pRegSet)
{
	/* 任务寄存器转换为调试上下文 */
	taDebugRegsConvert(pRegSet, taSDAExpContextPointerGet(taGetCpuID()));

	/* 核间中断处理函数 */
	if(NULL != taCpuCtrlHandle)
		taCpuCtrlHandle();
}
#endif

#endif
/*
 * @brief:
 *     安装任务创建HOOK，在每个任务创建时调用该HOOK
 * @param[in]:createHook:任务创建HOOK函数指针
 * @return:
 *     ERROR:任务创建HOOK表已满
 *     OK:安装任务创建HOOK成功
 */
STATUS taTaskCreateHookAdd(FUNCPTR createHook)
{
	return taskCreateHookAdd(createHook);
}

/*
 * @brief:
 *     安装任务删除HOOK，在每个任务删除时调用该HOOK
 * @param[in]:deleteHook:任务删除HOOK函数指针
 * @return:
 *     ERROR:任务删除HOOK表已满
 *     OK:安装任务删除HOOK成功
 */
STATUS taTaskDeleteHookAdd(FUNCPTR deleteHook)
{
	return taskDeleteHookAdd(deleteHook);
}

/*
 * @brief:
 *     挂起任务
 * @param[in]:tid:任务ID
 * @return:
 *     OK:暂停任务成功
 *     ERROR:无效任务ID
 */
STATUS taTaskSuspend(ulong_t tid)
{
	return taskSuspend((TASK_ID)tid);
}

/*
 * @brief:
 *     恢复任务运行
 * @param[in]:tid:任务ID
 * @return:
 *     OK:恢复任务运行成功
 *     ERROR:无效任务ID
 */
STATUS taTaskResume(ulong_t tid)
{
	return taskResume((TASK_ID)tid);
}

/*
 * @brief:
 *     激活任务
 * @param[in]:tid:任务ID
 * @return:
 *     OK:激活任务成功
 *     ERROR:无效任务ID
 */
STATUS taTaskActivate(ulong_t tid)
{
	return taskActivate((TASK_ID)tid);
}

/*
 * @brief:
 *     运行任务
 * @param[in]:tid:任务ID
 * @return:
 *     OK:运行任务成功
 *     ERROR:无效任务ID
 */
STATUS taTaskCont(ulong_t tid)
{
	return taskCont((TASK_ID)tid);
}

/*
 * @brief:
 *     禁止本地CPU任务抢占
 * @return:
 *     OK:成功
 *     ERROR:失败
 */
STATUS taTaskCpuLock(void)
{
	return taskCpuLock();
}

/*
 * @brief:
 *     使能本地CPU任务抢占，但不执行调度函数
 * @return:
 *     OK:成功
 *     ERROR:失败
 */
STATUS taTaskCpuUnlockNoResched(void)
{
	return taskCpuUnlockNoResched();
}

/*
 * @brief:
 *     使能本地CPU任务抢占
 * @return:
 *     OK:成功
 *     ERROR:失败
 */
STATUS taTaskCpuUnlock(void)
{
	return taskCpuUnlock();
}

/*
 * @brief:
 *     分配内存
 * @param[in]: nBytes:分配内存大小
 * @return:
 *     内存地址
 */
void *taMalloc(size_t nBytes)
{
	return malloc(nBytes);
}

/*
 * @brief:
 *     释放内存
 * @param[in]: ptr:内存地址
 * @return:
 *     内存地址
 */
void taFree(void *ptr)
{
	free(ptr);
}

/*内存探测接口*/
STATUS taMemArchProbe(void *adrs,int mode,int length,void *pVal)
{
	return vxMemArchProbe ((void *) adrs, mode, length, (void *) pVal);
}

