/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file:taTaskIfLib.c
 * @brief:
 *             <li>任务相关接口封装</li>
 */

/************************头 文 件******************************/
#include <vxWorks.h>
#include <taskLibP.h>
#include <taskLib.h>
#include <taskHookLib.h>
#include <taskMemLibP.h>
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

#ifdef __mips__
	context->registers[STATUSREG] = pRegSet->sr;
	context->registers[PC] = pRegSet->pc;
	context->registers[WATCHLOREG] = pRegSet->lo;
	context->registers[WATCHHIREG] = pRegSet->hi;
	memcpy(context->registers,pRegSet->gpreg,GPR_SUM*sizeof(UINT64));
	context->registers[CAUSEREG] = pRegSet->cause;
#endif

#ifdef __PPC__
	context->registers[PC] = pRegSet->pc;
	context->registers[PS] = pRegSet->msr;
	context->registers[CR] = pRegSet->cr;
	context->registers[PLR] = pRegSet->lr;
	context->registers[PCTR] = pRegSet->ctr;
	context->registers[PXER] = pRegSet->xer;
	memcpy(context->registers,pRegSet->gpr,GREG_NUM*sizeof(UINT32));

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

#ifdef __mips__
	pRegSet->sr = context->registers[STATUSREG];
	pRegSet->pc = context->registers[PC];
	pRegSet->lo = context->registers[WATCHLOREG];
	pRegSet->hi = context->registers[WATCHHIREG];
	memcpy(pRegSet->gpreg,context->registers,GPR_SUM*sizeof(UINT64));
	pRegSet->cause = context->registers[CAUSEREG];
#endif

#ifdef __PPC__


	pRegSet->pc = context->registers[PC];
	pRegSet->msr = context->registers[PS];
	pRegSet->cr = context->registers[CR];
	pRegSet->lr = context->registers[PLR];
	pRegSet->ctr = context->registers[PCTR];
	pRegSet->xer = context->registers[PXER];
	memcpy(pRegSet->gpr,context->registers,GREG_NUM*sizeof(UINT32));
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
char *taTaskName(int taskID)
{
	return taskName(taskID);
}

/*
 * @brief:
 *     获取当前任务ID(当前cpu上运行的)
 * @return:
 *     当前任务ID
 */
int taTaskIdSelf(void)
{
	return taskIdSelf();
}

/*
 * @brief:
 *     获取系统中任务列表
 * @param[out]:idList:任务ID列表
 * @param[in]:maxTasks:任务列表能够容纳的最大任务数
 * @return:
 *     任务列表中的任务个数
 */
int taTaskIdListGet(int idList[],int maxTasks)
{
	return taskIdListGet(idList,maxTasks);
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
int taTaskContextGet(int taskId, T_DB_ExceptionContext *context)
{
	REG_SET regs;
	STATUS status = OK;

	/* 获取任务上下文 */
	status = taskRegsGet(taskId, &regs);

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
int taTaskContextSet(int taskId, T_DB_ExceptionContext *context)
{
	REG_SET regs;

	/* 上下文转换，把调试上下文转换为任务上下文 */
	taDebugContextConvert(context, &regs);

	return taskRegsSet(taskId, &regs);
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
int taTaskIdVerify(int taskId)
{
	return taskIdVerify(taskId);
}


/*
 * @brief:
 *     刷新Cache
 * @param[in]:address:刷新内存地址
 * @param[in]:size:刷新内存大小
 * @return:
 *     无
 */
void taCacheTextUpdata(UINT32 address, UINT32 size)
{

	cacheFlush((CACHE_TYPE)DATA_CACHE,(void *)address,size);/*data cache 1*/
	cacheTextUpdate((void *)address,size);
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

/*
 * @brief:
 *     保存核间中断上下文
 *     由于系统及只能通过此方式，调用桥接的IPI处理函数。
 * @param[in]:pRegSet:核间中断上下文
 * @return:
 *     无
 */
void intVxdbgCpuRegsGet()
{
	taCpuCtrlHandle();
}

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
STATUS taTaskSuspend(int tid)
{
	return taskSuspend(tid);
}

/*
 * @brief:
 *     恢复任务运行
 * @param[in]:tid:任务ID
 * @return:
 *     OK:恢复任务运行成功
 *     ERROR:无效任务ID
 */
STATUS taTaskResume(int tid)
{
	return taskResume(tid);
}

/*
 * @brief:
 *     激活任务
 * @param[in]:tid:任务ID
 * @return:
 *     OK:激活任务成功
 *     ERROR:无效任务ID
 */
STATUS taTaskActivate(int tid)
{
	return taskActivate(tid);
}

/*
 * @brief:
 *     运行任务
 * @param[in]:tid:任务ID
 * @return:
 *     OK:运行任务成功
 *     ERROR:无效任务ID
 */
STATUS taTaskCont(int tid)
{
	return taskCont(tid);
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

#ifdef __X86__
STATUS vxIpiEnable(INT32 ipiId)
{
    /*x86 该接口未实现*/
}
#endif

