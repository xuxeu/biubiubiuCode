/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  dbArchLib.c
 * @brief:
 *             <li>ARCH相关的异常处理</li>
 */
/************************头文件******************************/
#include "dbAtom.h"
#include "taUtil.h"
#include "ta.h"

#ifdef _TA_CONFIG_RTP
#include <taskLib.h>
#include <excLib.h>
#endif

/************************宏定义******************************/

/************************类型定义****************************/
#if 0
#ifdef CONFIG_TA_LP64

/* 核心态异常产生时CPU压栈信息 */
typedef struct
{		
    ulong_t pc;
    ulong_t cs;
    ulong_t rflags;
    ulong_t rsp;		
    ulong_t ss;	
} ESF0;

#else

/* 核心态异常产生时CPU压栈信息 */
typedef struct
{
    UINT32 pc;
    UINT16 cs;
    UINT16 pad0;
    UINT32 eflags;
} ESF0;

/* 用户态异常产生时CPU压栈信息 */
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
/************************全局变量****************************/

/************************前向声明****************************/

/* 用户态CS寄存器值 */
extern int sysCsUser;

/************************模块变量****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *      获取异常上下文
 * @param[in]: info: 异常产生时CPU压栈的上下文，包括EFLAGS, CS, EIP等
 * @param[in]: regs: 栈上保存的异常上下文
 * @param[out]: context: 上下文指针
 * @return:
 *     无
 */
static void taDebugGetExceptionContext(
ESF *info,
ulong_t *regs,
T_DB_ExceptionContext *context)
{

	/*系统级调试*/
	REG_SET *regSet;
	regSet = (REG_SET *)regs;
	/* 保存通用寄存器 */
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

	/* 保存产生异常时的RSP寄存器 */
	//context->registers[RSP] = (ulong_t)info->esp;

	/* 保存PC、EFLAGS寄存器。 */
	context->registers[PC] = ((ulong_t)info->pc);
	context->registers[CPSR] = info->cpsr;


}


/*
 * @brief:
 *      设置异常上下文
 * @param[in]: info: 异常产生时CPU压栈的上下文，包括EFLAGS, CS, EIP等
 * @param[in]: regs: 栈上保存的异常上下文
 * @param[in]: context: 上下文指针
 * @return:
 *     无
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
	/*系统级调试*/
	/* 恢复通用寄存器 */
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
		
		
	/* 设置PC、EFLAGS寄存器 */
	info->pc = context->registers[PC];
	info->cpsr = context->registers[CPSR];

}



/*
 * @brief:
 *      体系结构相关的异常处理入口函数
 * @param[in]: info: 异常产生时CPU压栈的上下文，包括EFLAGS, CS, EIP等
 * @param[in]: regs: 栈上保存的异常上下文，包括eax->ecx->edx->ebx->esp->ebp->esi->edi等寄存器
 * @param[in]: vector: 异常号
 * @return:
 *     无
 */

BOOL taOsExceptionHandler
(
UINT32 vector,   /*异常向量号*/

ESF *info,   /*CPU保存异常信息的栈地址，异常返回时会使用此信息返回到异常点。
                   根据是否存在错误码、是否存在特权级别切换，pEsf的数据类型可能是ESF0-ESF3中的一种。*/
                   
ulong_t *regs, /*保存的寄存信息，异常返回时会使用此信息恢复寄存器。REG_SET*/

ulong_t *pExcInfo /*封装后的异常信息*/
)
{

	//printk("-----this is my own function to handle execption,vector:%x--------\n",vector);
    if (vector == 0xc)
    {

	T_DB_ExceptionContext context;

    /* 清空异常上下文  */
    ZeroMemory((void *)&context, sizeof(T_DB_ExceptionContext));

    /* 获取异常上下文 */
    //printk("-----taOsExceptionHandler----0000--------\n");
    
    //int i = 0;
    
	taDebugGetExceptionContext(info, regs, &context);
    //for(i=0;i<26;i++)
    //{
    //    printk("--------context[%d]:0x%x--------\n",i,context.registers[i]);
    //}

    /* 调用通用异常处理函数  */
	//printk("-----taOsExceptionHandler----1111--------\n");
	taExceptionHandler(vector, &context);
    //for(i=0;i<26;i++)
    //{
    //    printk("--------context[%d]:0x%x--------\n",i,context.registers[i]);
    //}

    /* 设置异常上下文 */
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


