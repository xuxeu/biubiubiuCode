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

#ifdef _TA_CONFIG_RTP
#include <taskLib.h>
#include <excLib.h>
#endif

/************************�궨��******************************/

/************************���Ͷ���****************************/
#if 0
#ifdef CONFIG_TA_LP64

/* ����̬�쳣����ʱCPUѹջ��Ϣ */
typedef struct
{		
    ulong_t pc;
    ulong_t cs;
    ulong_t rflags;
    ulong_t rsp;		
    ulong_t ss;	
} ESF0;

#else

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
static void taDebugGetExceptionContext(
#ifdef CONFIG_TA_LP64
ESF2 *info,
#else
ESF0 *info,
#endif
ulong_t *regs,
T_DB_ExceptionContext *context)
{
#ifdef CONFIG_TA_LP64
#ifdef _TA_CONFIG_RTP

	/*64ϵͳ������*/
	REG_SET *regSet;
	regSet = (REG_SET *)regs;
	/* ����ͨ�üĴ��� */
	context->registers[R15] = regSet->r15;
	context->registers[R14] = regSet->r14;
	context->registers[R13] = regSet->r13;
	context->registers[R12] = regSet->r12;
	context->registers[R11] = regSet->r11;
	context->registers[R10] = regSet->r10;
	context->registers[R9]  = regSet->r9;
	context->registers[R8]  = regSet->r8;
	context->registers[RDI] = regSet->rdi;
	context->registers[RSI] = regSet->rsi;
	context->registers[RBP] = regSet->rbp;
	context->registers[RSP] = regSet->rsp;
	context->registers[RBX] = regSet->rbx;
	context->registers[RDX] = regSet->rdx;
	context->registers[RCX] = regSet->rcx;
	context->registers[RAX] = regSet->rax;

	/* ��������쳣ʱ��RSP�Ĵ��� */
	context->registers[RSP] = (ulong_t)info->esp;

	/* ����PC��EFLAGS�Ĵ����� */
	context->registers[PC] = (ulong_t)info->pc;
	context->registers[RFLAGS] = info->rflags;



#else
	/*64λ���ļ�����*/
	/* ����ͨ�üĴ��� */

	context->registers[R15] = *regs++;
	context->registers[R14] = *regs++;
	context->registers[R13] = *regs++;
	context->registers[R12] = *regs++;
	context->registers[R11] = *regs++;
	context->registers[R10] = *regs++;
	context->registers[R9] = *regs++;
	context->registers[R8] = *regs++;
	context->registers[RDI] = *regs++;
	context->registers[RSI] = *regs++;
	context->registers[RBP] = *regs++;
	context->registers[RSP] = *regs++;
	context->registers[RBX] = *regs++;
	context->registers[RDX] = *regs++;
	context->registers[RCX] = *regs++;
	context->registers[RAX] = *regs++;

	/* ��������쳣ʱ��RSP�Ĵ��� */
	context->registers[RSP] = (ulong_t)info->esp;

	/* ����PC��EFLAGS�Ĵ����� */
	context->registers[PC] = (ulong_t)info->pc;
	context->registers[RFLAGS] = info->rflags;
	
#endif	
#else
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
#endif
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
static void taDebugSetExceptionContext
(
#ifdef CONFIG_TA_LP64
ESF2 *info, 
#else
ESF0 *info,
#endif
ulong_t *regs, 
T_DB_ExceptionContext *context
)
{
#ifdef CONFIG_TA_LP64
#ifdef _TA_CONFIG_RTP
	REG_SET *regSet;
	regSet = (REG_SET *)regs;
	/*64λϵͳ������*/
	/* �ָ�ͨ�üĴ��� */
	regSet->r15 = context->registers[R15];
	regSet->r14 = context->registers[R14];
	regSet->r13 = context->registers[R13];
	regSet->r12 = context->registers[R12];
	regSet->r11 = context->registers[R11];
	regSet->r10 = context->registers[R10];
	regSet->r9 = context->registers[R9];
	regSet->r8 = context->registers[R8];
	regSet->rdi = context->registers[RDI];
	regSet->rsi = context->registers[RSI];
	regSet->rbp = context->registers[RBP];
	regSet->rbx = context->registers[RBX];
	regSet->rdx = context->registers[RDX];
	regSet->rcx = context->registers[RCX];
	regSet->rax = context->registers[RAX];
		
		
	/* ����PC��EFLAGS�Ĵ��� */
	info->pc = context->registers[PC];
	info->rflags = context->registers[RFLAGS];


#else

	/*64λ���ļ�����*/
	/* �ָ�ͨ�üĴ��� */

	*regs++ = context->registers[R15];
	*regs++ = context->registers[R14];
	*regs++ = context->registers[R13];
	*regs++ = context->registers[R12];
	*regs++ = context->registers[R11];
	*regs++ = context->registers[R10];
	*regs++ = context->registers[R9];
	*regs++ = context->registers[R8];
	*regs++ = context->registers[RDI];
	*regs++ = context->registers[RSI];
	*regs++ = context->registers[RBP];
	regs++;
	*regs++ = context->registers[RBX];
	*regs++ = context->registers[RDX];
	*regs++ = context->registers[RCX];
	*regs++ = context->registers[RAX];


	/* ����PC��EFLAGS�Ĵ��� */
	info->pc = context->registers[PC];
	info->rflags = context->registers[RFLAGS];
#endif	
#else
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
#endif
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
void taAachExceptionHandler
(
#ifdef CONFIG_TA_LP64
ESF2 *info,
#else
ESF0 *info,
#endif
ulong_t *regs, 
ulong_t vector
)
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


#if defined(CONFIG_TA_LP64) && defined(_TA_CONFIG_RTP)
void taOsExceptionHandler
(
UINT32 vector,   /*�쳣������*/

ESF2 *info,   /*CPU�����쳣��Ϣ��ջ��ַ���쳣����ʱ��ʹ�ô���Ϣ���ص��쳣�㡣
                   �����Ƿ���ڴ����롢�Ƿ������Ȩ�����л���pEsf���������Ϳ�����ESF0-ESF3�е�һ�֡�*/
                   
ulong_t *regs, /*����ļĴ���Ϣ���쳣����ʱ��ʹ�ô���Ϣ�ָ��Ĵ�����*/

BOOL error,/*1��ʾ�д����룬0��ʾû�д����롣*/

ulong_t *pExcInfo /*��װ����쳣��Ϣ*/
)
{
	int ret = 11;

	ret = intCpuLock();


	T_DB_ExceptionContext context;

    /* ����쳣������  */
    ZeroMemory((void *)&context, sizeof(T_DB_ExceptionContext));

    /* ��ȡ�쳣������ */
	taDebugGetExceptionContext(info, regs, &context);

    /* ����ͨ���쳣������  */
	taExceptionHandler(vector, &context);

    /* �����쳣������ */
    taDebugSetExceptionContext(info, regs, &context);

	intCpuUnlock(ret);

}
#endif



