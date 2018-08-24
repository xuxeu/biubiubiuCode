/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * �޸ļ�¼��
 * 2016-07-11	���࣬�����������ɼ������޹�˾
 *				�������ļ���
 */

/**
 * @file:taTaskIfLib.c
 * @brief:
 *             <li>������ؽӿڷ�װ��</li>
 */

/************************ͷ �� ��******************************/
#include <vxWorks.h>
#include <taskLib.h>
#include <taskHookLib.h>
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
#if(CONFIG_ENABLE_LP64 == true)
	/* ����ͨ�üĴ��� */
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
 *     ������ת�����ѵ���������ת��Ϊ����������
 * @param[in] context:����������
 * @param[in] pRegSet:REG_SET������
 *@return
 *    ��
 */
void taDebugContextConvert(T_DB_ExceptionContext *context, REG_SET *pRegSet)
{
#ifdef __X86__
#if(CONFIG_ENABLE_LP64 == true)
	/* �ָ�ͨ�üĴ��� */
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
 *     ��������ID��ȡ������
 * @param[in]:taskID:����ID
 * @return:
 *     OK:��ȡ�������ɹ�
 *     ERROR:��ȡ������ʧ��
 */
char *taTaskName(ulong_t taskID)
{
	return taskName((TASK_ID)taskID);
}

/*
 * @brief:
 *     ��ȡ��ǰ����ID(��ǰcpu�����е�)
 * @return:
 *     ��ǰ����ID
 */
ulong_t taTaskIdSelf(void)
{
	return (ulong_t)taskIdSelf();
}

/*
 * @brief:
 *     ��ȡϵͳ�������б�
 * @param[out]:idList:����ID�б�
 * @param[in]:maxTasks:�����б��ܹ����ɵ����������
 * @return:
 *     �����б��е��������
 */
int taTaskIdListGet(ulong_t idList[],int maxTasks)
{
	return taskIdListGet((TASK_ID)idList,maxTasks);
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
int taTaskContextGet(ulong_t taskId, T_DB_ExceptionContext *context)
{
	REG_SET regs;
	STATUS status = OK;

	/* ��ȡ���������� */
	status = taskRegsGet((TASK_ID)taskId, &regs);

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
int taTaskContextSet(ulong_t taskId, T_DB_ExceptionContext *context)
{
	REG_SET regs;

	/* ������ת�����ѵ���������ת��Ϊ���������� */
	taDebugContextConvert(context, &regs);

	return taskRegsSet((TASK_ID)taskId, &regs);
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
int taTaskIdVerify(ulong_t taskId)
{
	return taskIdVerify((TASK_ID)taskId);
}

/*
 * @brief:
 *     ˢ��Cache
 * @param[in]:address:ˢ���ڴ��ַ
 * @param[in]:size:ˢ���ڴ��С
 * @return:
 *     ��
 */
void taCacheTextUpdata(ulong_t address, UINT32 size)
{
    CACHE_TEXT_UPDATE(address, size);
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
#ifdef __X86__
#ifdef CONFIG_USR_DEBUG
/*
 * @brief:
 *     ����˼��ж�������
 * @param[in]:pRegSet:�˼��ж�������
 * @return:
 *     ��
 */

void intVxdbgCpuRegsGet(REG_SET *pRegSet)
{
	/* ����Ĵ���ת��Ϊ���������� */
	taDebugRegsConvert(pRegSet, taSDAExpContextPointerGet(taGetCpuID()));

	/* �˼��жϴ����� */
	if(NULL != taCpuCtrlHandle)
		taCpuCtrlHandle();
}
#endif

#endif
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
STATUS taTaskSuspend(ulong_t tid)
{
	return taskSuspend((TASK_ID)tid);
}

/*
 * @brief:
 *     �ָ���������
 * @param[in]:tid:����ID
 * @return:
 *     OK:�ָ��������гɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskResume(ulong_t tid)
{
	return taskResume((TASK_ID)tid);
}

/*
 * @brief:
 *     ��������
 * @param[in]:tid:����ID
 * @return:
 *     OK:��������ɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskActivate(ulong_t tid)
{
	return taskActivate((TASK_ID)tid);
}

/*
 * @brief:
 *     ��������
 * @param[in]:tid:����ID
 * @return:
 *     OK:��������ɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskCont(ulong_t tid)
{
	return taskCont((TASK_ID)tid);
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

/*�ڴ�̽��ӿ�*/
STATUS taMemArchProbe(void *adrs,int mode,int length,void *pVal)
{
	return vxMemArchProbe ((void *) adrs, mode, length, (void *) pVal);
}

