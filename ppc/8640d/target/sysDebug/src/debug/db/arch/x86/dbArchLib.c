/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  dbArchLib.c
 * @brief:
 *             <li>ARCH��ص��쳣����</li>
 */
/************************ͷ�ļ�******************************/
#include "dbAtom.h"
#include "taUtil.h"
#include "ta.h"

/************************�궨��******************************/

/************************���Ͷ���****************************/

/* ����̬�쳣����ʱCPUѹջ��Ϣ */
typedef struct
{
    UINT32 pc;
    UINT16 cs;
    UINT16 pad0;
    UINT32 eflags;
} ESF0;

/* �û�̬�쳣����ʱCPUѹջ��Ϣ */
typedef struct
{
	UINT32 pc;
	UINT16 cs;
	UINT16 pad0;
	UINT32 eflags;
	UINT32 esp;
	UINT32 ss;
} ESF2;
/************************ȫ�ֱ���****************************/

/************************ǰ������****************************/

/* �û�̬CS�Ĵ���ֵ */
extern int sysCsUser;

/************************ģ�����****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *      ��ȡ�쳣������
 * @param[in]: info: �쳣����ʱCPUѹջ�������ģ�����EFLAGS, CS, EIP��
 * @param[in]: regs: ջ�ϱ�����쳣������
 * @param[out]: context: ������ָ��
 * @return:
 *     ��
 */
static void taDebugGetExceptionContext(ESF0 *info, int *regs, T_DB_ExceptionContext *context)
{
	/* ����ͨ�üĴ��� */
	context->registers[EDI] = *regs++;
	context->registers[ESI] = *regs++;
	context->registers[EBP] = *regs++;
	context->registers[ESP] = *regs++;
	context->registers[EBX] = *regs++;
	context->registers[EDX] = *regs++;
	context->registers[ECX] = *regs++;
	context->registers[EAX] = *regs++;

#ifdef _TA_CONFIG_RTP
    ESF2 *userInfo = (ESF2 *)info;

    if(info->cs == sysCsUser)
    {
    	context->registers[ESP] = userInfo->esp;
    }
    else
    {
    	context->registers[ESP] = (UINT32)((char *)info + sizeof(ESF0));
    }
#else
    context->registers[ESP] = (UINT32)((char *)info + sizeof(ESF0));
#endif /* _TA_CONFIG_RTP */

    /* ����PC��EFLAGS�Ĵ��� */
    context->registers[PC] = (UINT32)info->pc;
    context->registers[EFLAGS] = info->eflags;
}

/*
 * @brief:
 *      �����쳣������
 * @param[in]: info: �쳣����ʱCPUѹջ�������ģ�����EFLAGS, CS, EIP��
 * @param[in]: regs: ջ�ϱ�����쳣������
 * @param[in]: context: ������ָ��
 * @return:
 *     ��
 */
static void taDebugSetExceptionContext(ESF0 *info, int *regs, T_DB_ExceptionContext *context)
{
	/* �ָ�ͨ�üĴ��� */
    *regs++ = context->registers[EDI];
    *regs++ = context->registers[ESI];
    *regs++ = context->registers[EBP];
     regs++;
    *regs++ = context->registers[EBX];
    *regs++ = context->registers[EDX];
    *regs++ = context->registers[ECX];
    *regs++ = context->registers[EAX];

    /* ����PC��EFLAGS�Ĵ��� */
    info->pc = context->registers[PC];
    info->eflags = context->registers[EFLAGS];
}

/*
 * @brief:
 *      ��ϵ�ṹ��ص��쳣������ں���
 * @param[in]: info: �쳣����ʱCPUѹջ�������ģ�����EFLAGS, CS, EIP��
 * @param[in]: regs: ջ�ϱ�����쳣�����ģ�����eax->ecx->edx->ebx->esp->ebp->esi->edi�ȼĴ���
 * @param[in]: vector: �쳣��
 * @return:
 *     ��
 */
void taAachExceptionHandler(ESF0 *info, int *regs, UINT32 vector)
{
	T_DB_ExceptionContext context;

    /* ����쳣������  */
    ZeroMemory((void *)&context, sizeof(T_DB_ExceptionContext));

    /* ��ȡ�쳣������ */
	taDebugGetExceptionContext(info, regs, &context);

    /* ����ͨ���쳣������  */
	taExceptionHandler(vector, &context);

    /* �����쳣������ */
    taDebugSetExceptionContext(info, regs, &context);
}
