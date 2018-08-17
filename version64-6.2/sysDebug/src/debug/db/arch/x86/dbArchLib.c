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

	/*64系统级调试*/
	REG_SET *regSet;
	regSet = (REG_SET *)regs;
	/* 保存通用寄存器 */
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

	/* 保存产生异常时的RSP寄存器 */
	context->registers[RSP] = (ulong_t)info->esp;

	/* 保存PC、EFLAGS寄存器。 */
	context->registers[PC] = (ulong_t)info->pc;
	context->registers[RFLAGS] = info->rflags;



#else
	/*64位核心级调试*/
	/* 保存通用寄存器 */

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

	/* 保存产生异常时的RSP寄存器 */
	context->registers[RSP] = (ulong_t)info->esp;

	/* 保存PC、EFLAGS寄存器。 */
	context->registers[PC] = (ulong_t)info->pc;
	context->registers[RFLAGS] = info->rflags;
	
#endif	
#else
	/* 保存通用寄存器 */
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

    /* 保存PC、EFLAGS寄存器 */
    context->registers[PC] = (UINT32)info->pc;
    context->registers[EFLAGS] = info->eflags;
#endif
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
	/*64位系统级调试*/
	/* 恢复通用寄存器 */
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
		
		
	/* 设置PC、EFLAGS寄存器 */
	info->pc = context->registers[PC];
	info->rflags = context->registers[RFLAGS];


#else

	/*64位核心级调试*/
	/* 恢复通用寄存器 */

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


	/* 设置PC、EFLAGS寄存器 */
	info->pc = context->registers[PC];
	info->rflags = context->registers[RFLAGS];
#endif	
#else
	/* 恢复通用寄存器 */
    *regs++ = context->registers[EDI];
    *regs++ = context->registers[ESI];
    *regs++ = context->registers[EBP];
     regs++;
    *regs++ = context->registers[EBX];
    *regs++ = context->registers[EDX];
    *regs++ = context->registers[ECX];
    *regs++ = context->registers[EAX];

    /* 设置PC、EFLAGS寄存器 */
    info->pc = context->registers[PC];
    info->eflags = context->registers[EFLAGS];
#endif
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

    /* 清空异常上下文  */
    ZeroMemory((void *)&context, sizeof(T_DB_ExceptionContext));

    /* 获取异常上下文 */
	taDebugGetExceptionContext(info, regs, &context);

    /* 调用通用异常处理函数  */
	taExceptionHandler(vector, &context);

    /* 设置异常上下文 */
    taDebugSetExceptionContext(info, regs, &context);

}


#if defined(CONFIG_TA_LP64) && defined(_TA_CONFIG_RTP)
void taOsExceptionHandler
(
UINT32 vector,   /*异常向量号*/

ESF2 *info,   /*CPU保存异常信息的栈地址，异常返回时会使用此信息返回到异常点。
                   根据是否存在错误码、是否存在特权级别切换，pEsf的数据类型可能是ESF0-ESF3中的一种。*/
                   
ulong_t *regs, /*保存的寄存信息，异常返回时会使用此信息恢复寄存器。*/

BOOL error,/*1表示有错误码，0表示没有错误码。*/

ulong_t *pExcInfo /*封装后的异常信息*/
)
{
	int ret = 11;

	ret = intCpuLock();


	T_DB_ExceptionContext context;

    /* 清空异常上下文  */
    ZeroMemory((void *)&context, sizeof(T_DB_ExceptionContext));

    /* 获取异常上下文 */
	taDebugGetExceptionContext(info, regs, &context);

    /* 调用通用异常处理函数  */
	taExceptionHandler(vector, &context);

    /* 设置异常上下文 */
    taDebugSetExceptionContext(info, regs, &context);

	intCpuUnlock(ret);

}
#endif



