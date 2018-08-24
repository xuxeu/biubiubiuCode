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
ESF *info,
ulong_t *regs,
T_DB_ExceptionContext *context)
{

	/*ϵͳ������*/
	REG_SET *regSet;
	regSet = (REG_SET *)regs;
	/* ����ͨ�üĴ��� */
	context->registers[LR]  = regSet->r[14];
	context->registers[SP]  = regSet->r[13];
	context->registers[R12] = regSet->r[12];
	context->registers[FP] = regSet->r[11];
	context->registers[R10] = regSet->r[10];
	context->registers[R9]  = regSet->r[9];
	context->registers[R8]  = regSet->r[8];
	context->registers[R7] = regSet->r[7];
	context->registers[R6] = regSet->r[6];
	context->registers[R5] = regSet->r[5];
	context->registers[R4] = regSet->r[4];
	context->registers[R3] = regSet->r[3];
	context->registers[R2] = regSet->r[2];
	context->registers[R1] = regSet->r[1];
	context->registers[R0] = regSet->r[0];

	/* ��������쳣ʱ��RSP�Ĵ��� */
	//context->registers[RSP] = (ulong_t)info->esp;

	/* ����PC��EFLAGS�Ĵ����� */
	context->registers[PC] = ((ulong_t)info->pc);
	context->registers[CPSR] = info->cpsr;


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
ESF *info,
ulong_t *regs, 
T_DB_ExceptionContext *context
)
{
	REG_SET *regSet;
	regSet = (REG_SET *)regs;
	/*ϵͳ������*/
	/* �ָ�ͨ�üĴ��� */
	regSet->r[14] = context->registers[LR];
	regSet->r[13] = context->registers[SP];
	regSet->r[12] = context->registers[R12];
	regSet->r[11] = context->registers[FP];
	regSet->r[10] = context->registers[R10];
	regSet->r[9] = context->registers[R9];
	regSet->r[8] = context->registers[R8];
	regSet->r[7] = context->registers[R7];
	regSet->r[6] = context->registers[R6];
	regSet->r[5] = context->registers[R5];
	regSet->r[4] = context->registers[R4];
	regSet->r[3] = context->registers[R3];
	regSet->r[2] = context->registers[R2];
	regSet->r[1] = context->registers[R1];
	regSet->r[0] = context->registers[R0];
		
		
	/* ����PC��EFLAGS�Ĵ��� */
	info->pc = context->registers[PC];
	info->cpsr = context->registers[CPSR];

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

BOOL taOsExceptionHandler
(
UINT32 vector,   /*�쳣������*/

ESF *info,   /*CPU�����쳣��Ϣ��ջ��ַ���쳣����ʱ��ʹ�ô���Ϣ���ص��쳣�㡣
                   �����Ƿ���ڴ����롢�Ƿ������Ȩ�����л���pEsf���������Ϳ�����ESF0-ESF3�е�һ�֡�*/
                   
ulong_t *regs, /*����ļĴ���Ϣ���쳣����ʱ��ʹ�ô���Ϣ�ָ��Ĵ�����REG_SET*/

ulong_t *pExcInfo /*��װ����쳣��Ϣ*/
)
{

	//printk("-----this is my own function to handle execption,vector:%x--------\n",vector);
    if (vector == 0xc)
    {

	T_DB_ExceptionContext context;

    /* ����쳣������  */
    ZeroMemory((void *)&context, sizeof(T_DB_ExceptionContext));

    /* ��ȡ�쳣������ */
    //printk("-----taOsExceptionHandler----0000--------\n");
    
    //int i = 0;
    
	taDebugGetExceptionContext(info, regs, &context);
    //for(i=0;i<26;i++)
    //{
    //    printk("--------context[%d]:0x%x--------\n",i,context.registers[i]);
    //}

    /* ����ͨ���쳣������  */
	//printk("-----taOsExceptionHandler----1111--------\n");
	taExceptionHandler(vector, &context);
    //for(i=0;i<26;i++)
    //{
    //    printk("--------context[%d]:0x%x--------\n",i,context.registers[i]);
    //}

    /* �����쳣������ */
	//printk("-----taOsExceptionHandler----2222--------\n");
    taDebugSetExceptionContext(info, regs, &context);

    //printk("-----taOsExceptionHandler----3333--------\n");


    return (TRUE);

    }
    else
    {
    return (FALSE);
    }

}

void taTrap()
{
	asm("BKPT 0xc");
}


