/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file:taTaskIfLib.c
 * @brief:
 *             <li>������ؽӿڷ�װ</li>
 */

/************************ͷ �� ��******************************/
#include <vxWorks.h>
#include <taskLibP.h>
#include <taskLib.h>
#include <taskHookLib.h>
#include <taskMemLibP.h>
#include <taskLibCommon.h>
#include <cacheLib.h>
#include <vxCpuLib.h>
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/* �˼��жϴ����� */
static void (*taCpuCtrlHandle)(void) = NULL;

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 *@brief
 *     ������ת����������������ת��Ϊ����������
 * @param[in] context:����������
 * @param[in] pRegSet:REG_SET������
 *@return
 *    ��
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
 *     ������ת�����ѵ���������ת��Ϊ����������
 * @param[in] context:����������
 * @param[in] pRegSet:REG_SET������
 *@return
 *    ��
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
 *     ��������ID��ȡ������
 * @param[in]:taskID:����ID
 * @return:
 *     OK:��ȡ�������ɹ�
 *     ERROR:��ȡ������ʧ��
 */
char *taTaskName(int taskID)
{
	return taskName(taskID);
}

/*
 * @brief:
 *     ��ȡ��ǰ����ID(��ǰcpu�����е�)
 * @return:
 *     ��ǰ����ID
 */
int taTaskIdSelf(void)
{
	return taskIdSelf();
}

/*
 * @brief:
 *     ��ȡϵͳ�������б�
 * @param[out]:idList:����ID�б�
 * @param[in]:maxTasks:�����б��ܹ����ɵ����������
 * @return:
 *     �����б��е��������
 */
int taTaskIdListGet(int idList[],int maxTasks)
{
	return taskIdListGet(idList,maxTasks);
}

/*
 * @brief:
 *     ��ȡ����������
 * @param[in]:taskId:����ID
 * @param[out]:context:����������
 * @return:
 *     OK����ȡ�����ĳɹ�
 *     ERROR����ȡ������ʧ��
 */
int taTaskContextGet(int taskId, T_DB_ExceptionContext *context)
{
	REG_SET regs;
	STATUS status = OK;

	/* ��ȡ���������� */
	status = taskRegsGet(taskId, &regs);

	/* ������ת����������������ת��Ϊ���������� */
	taDebugRegsConvert(&regs, context);

	return status;
}

/*
 * @brief:
 *     ��������������
 * @param[in]:taskId:����ID
 * @param[in]:context:����������
 * @return:
 *     OK�����������ĳɹ�
 *     ERROR������������ʧ��
 */
int taTaskContextSet(int taskId, T_DB_ExceptionContext *context)
{
	REG_SET regs;

	/* ������ת�����ѵ���������ת��Ϊ���������� */
	taDebugContextConvert(context, &regs);

	return taskRegsSet(taskId, &regs);
}

/*
 * @brief:
 *     ��ȡOS��ʹ�ܵ�CPU
 * @return:
 *     OS��ʹ�ܵ�CPU
 */
cpuset_t taCpuEnabledGet (void)
{
    return vxCpuEnabledGet();
}

/*
 * @brief:
 *     ��ȡ��ǰCPU ID
 * @return:
 *     CPU ID
 */
UINT32 taGetCpuID(void)
{
    return vxCpuIndexGet();
}

/*
 * @brief:
 *     У�������Ƿ����
 * @param[in]:taskId:����ID
 * @return:
 *     OK:�������
 *     ERROR:���񲻴���
 */
int taTaskIdVerify(int taskId)
{
	return taskIdVerify(taskId);
}


/*
 * @brief:
 *     ˢ��Cache
 * @param[in]:address:ˢ���ڴ��ַ
 * @param[in]:size:ˢ���ڴ��С
 * @return:
 *     ��
 */
void taCacheTextUpdata(UINT32 address, UINT32 size)
{

	cacheFlush((CACHE_TYPE)DATA_CACHE,(void *)address,size);/*data cache 1*/
	cacheTextUpdate((void *)address,size);
}

/*
 * @brief:
 *     ��װ����IPI�жϴ�����
 * @param[in]:pFunction:������ַ
 * @return:
 *     ��
 */
void taInstallCpuCtrlHandle(void* pFunction)
{
	taCpuCtrlHandle = pFunction;
}

/*
 * @brief:
 *     ����˼��ж�������
 *     ����ϵͳ��ֻ��ͨ���˷�ʽ�������Žӵ�IPI��������
 * @param[in]:pRegSet:�˼��ж�������
 * @return:
 *     ��
 */
void intVxdbgCpuRegsGet()
{
	taCpuCtrlHandle();
}

/*
 * @brief:
 *     ��װ���񴴽�HOOK����ÿ�����񴴽�ʱ���ø�HOOK
 * @param[in]:createHook:���񴴽�HOOK����ָ��
 * @return:
 *     ERROR:���񴴽�HOOK������
 *     OK:��װ���񴴽�HOOK�ɹ�
 */
STATUS taTaskCreateHookAdd(FUNCPTR createHook)
{
	return taskCreateHookAdd(createHook);
}

/*
 * @brief:
 *     ��װ����ɾ��HOOK����ÿ������ɾ��ʱ���ø�HOOK
 * @param[in]:deleteHook:����ɾ��HOOK����ָ��
 * @return:
 *     ERROR:����ɾ��HOOK������
 *     OK:��װ����ɾ��HOOK�ɹ�
 */
STATUS taTaskDeleteHookAdd(FUNCPTR deleteHook)
{
	return taskDeleteHookAdd(deleteHook);
}

/*
 * @brief:
 *     ��������
 * @param[in]:tid:����ID
 * @return:
 *     OK:��ͣ����ɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskSuspend(int tid)
{
	return taskSuspend(tid);
}

/*
 * @brief:
 *     �ָ���������
 * @param[in]:tid:����ID
 * @return:
 *     OK:�ָ��������гɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskResume(int tid)
{
	return taskResume(tid);
}

/*
 * @brief:
 *     ��������
 * @param[in]:tid:����ID
 * @return:
 *     OK:��������ɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskActivate(int tid)
{
	return taskActivate(tid);
}

/*
 * @brief:
 *     ��������
 * @param[in]:tid:����ID
 * @return:
 *     OK:��������ɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskCont(int tid)
{
	return taskCont(tid);
}

/*
 * @brief:
 *     ��ֹ����CPU������ռ
 * @return:
 *     OK:�ɹ�
 *     ERROR:ʧ��
 */
STATUS taTaskCpuLock(void)
{
	return taskCpuLock();
}

/*
 * @brief:
 *     ʹ�ܱ���CPU������ռ������ִ�е��Ⱥ���
 * @return:
 *     OK:�ɹ�
 *     ERROR:ʧ��
 */
STATUS taTaskCpuUnlockNoResched(void)
{
	return taskCpuUnlockNoResched();
}

/*
 * @brief:
 *     ʹ�ܱ���CPU������ռ
 * @return:
 *     OK:�ɹ�
 *     ERROR:ʧ��
 */
STATUS taTaskCpuUnlock(void)
{
	return taskCpuUnlock();
}

/*
 * @brief:
 *     �����ڴ�
 * @param[in]: nBytes:�����ڴ��С
 * @return:
 *     �ڴ��ַ
 */
void *taMalloc(size_t nBytes)
{
	return malloc(nBytes);
}

/*
 * @brief:
 *     �ͷ��ڴ�
 * @param[in]: ptr:�ڴ��ַ
 * @return:
 *     �ڴ��ַ
 */
void taFree(void *ptr)
{
	free(ptr);
}

#ifdef __X86__
STATUS vxIpiEnable(INT32 ipiId)
{
    /*x86 �ýӿ�δʵ��*/
}
#endif

