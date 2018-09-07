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
#if 0

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

#endif




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


static void taDebugGetAllExceptionContext(ESF0 *info,EXC_INFO *pExcInfo, int *regs, T_DB_ExceptionContext *context)
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

    context->registers[ESP] = pExcInfo->esp0;

    /* ����PC��EFLAGS�Ĵ��� */
    context->registers[PC] = (int) pExcInfo->pc;
    context->registers[EFLAGS] = pExcInfo->eflags;
    context->vector = pExcInfo->vecNum;
    
    
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


#ifdef EXCEPTAKE

void taOsExceptionHandler
(
UINT32 vector,   /*�쳣������*/

ESF0 *info,   /*CPU�����쳣��Ϣ��ջ��ַ���쳣����ʱ��ʹ�ô���Ϣ���ص��쳣�㡣
                   �����Ƿ���ڴ����롢�Ƿ������Ȩ�����л���pEsf���������Ϳ�����ESF0-ESF3�е�һ�֡�*/
                   
UINT32 *regs, /*����ļĴ���Ϣ���쳣����ʱ��ʹ�ô���Ϣ�ָ��Ĵ�����*/

BOOL error,/*1��ʾ�д����룬0��ʾû�д����롣*/

EXC_INFO *pExcInfo /*��װ����쳣��Ϣ*/
)
{
	int ret = 11;

	ret = intCpuLock();


	T_DB_ExceptionContext context;

    /* ����쳣������  */
    ZeroMemory((void *)&context, sizeof(T_DB_ExceptionContext));

    /* ��ȡ�쳣������ */
	taDebugGetAllExceptionContext(info,pExcInfo, regs, &context);

    /* �����쳣�ӹܴ�����  */
	taExceptionTake(vector, &context);

    /* �����쳣������ */
    taDebugSetExceptionContext(info, regs, &context);

	intCpuUnlock(ret);


}

#endif

