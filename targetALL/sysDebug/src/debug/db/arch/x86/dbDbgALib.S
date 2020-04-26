/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * 修改历史:
 * 2013-11-07         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbDbgALib.S
 * @brief:
 *             <li>断点、单步异常处理</li>
 */

/************************头 文 件*****************************/
#define ASM_USE
#define _ASMLANGUAGE
#include <dbAsm.h>
#ifdef CONFIG_DELTAOS
#include <objLibP.h>
#include <delta.h>
#include <coreSmpI86P.h>
#endif

/************************宏 定 义******************************/

#define CORE_OBJ_CORE_SIZE 0x40

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/
.data

.globl	FUNC(taAachExceptionHandler)/* 异常处理函数 */
.globl	GTEXT(taBpExceptionStub)	/* 断点调试异常入口函数 */
.globl	GTEXT(taTraceExceptionStub)	/* 自陷调试异常入口函数 */

.text
.balign 16

/* 断点异常处理入口函数 */
FUNC_LABEL(taBpExceptionStub)
	cli
	pushl	eax	         /* 保存eax */

#ifdef _TA_CONFIG_RTP
	movl	FUNC(sysCsUser), eax/* 是否是用户模式  */
	cmpl	eax, 0x8(esp)
	jne	taStubSuper
taStubUser:

#ifdef CONFIG_DELTAOS
	pushl	edx	 /* 保存edx */
	
#ifdef _WRS_CONFIG_SMP	
	_IA32_PER_CPU_VALUE_GET(eax,edx,threadIdCurrent) /* 获取当前线程控制块地址 */
#else
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* 获取当前线程控制块地址 */
#endif
	popl    edx  /* 恢复edx */

	incl CORE_TCB_EXC_CNT(eax) /* 异常计数加一，表明当前在异常栈上 */
#endif
	/* 如果将要执行的异常处理例程较小，就要进行栈切换，处理器把被中断的例程的栈段选择子和栈指针压栈。
	        特权级未改变的压栈次序为EFLAGS, CS, EIP，否则为SS, ESP, EFLAGS, CS, EIP*/
	popl	eax     /* 恢复eax */
	pushal		     /* 寄存所有通用寄存器，依次把eax->ecx->edx->ebx->esp->ebp->esi->edi压入栈中 */
	movl	0x2c(esp), eax	/* 获取用户模式的SP */
	movl	eax, 0x0c(esp)	/* 存放用户态的SP在栈上，REG_ESP为REG_SET结构体的偏移  */

	movl	FUNC(sysDsSuper), eax /* 设置段寄存器 */
	movw    ax,ds
	movw    ax,es
	movw    ax,fs
	movw    ax,gs
	jmp	taStubCont
#endif
taStubSuper:
	popl    eax        /* 恢复eax */
	pushal             /* 寄存所有通用寄存器，依次把eax->ecx->edx->ebx->esp->ebp->esi->edi压入栈中 */

taStubCont:
	movl	esp,eax
	pushl	ebp
	movl	esp,ebp
	decl	0x20+0x4(ebp) /* 调整程序寄存器值，由于断点异常属于自陷类异常，指向下一条指令，所以需要调整PC值 */
	pushl	$3			  /* 压栈vector,断点异常号为3 */
	pushl	eax	          /* 将ESP寄存器值压栈,栈上保存了异常上下文  */
	leal	0x20+0x4(ebp),eax
	pushl	eax		      /* 压栈info */
	call	FUNC(taAachExceptionHandler) /* C代码异常处理函数 */

	addl	$0x10,esp  /* 栈指针加16，因为 调用taExceptionHandler函数时压栈了三个参数+ebp */
	popal			   /* 恢复所有通用寄存器 */

	pushl	eax		   /* 保存EAX寄存器，后续代码需要用到该寄存器 */

#ifdef _TA_CONFIG_RTP
	movl	0x08(esp), eax	     /* 获取CS寄存器，当前栈上的寄存器为SS, ESP, EFLAGS, CS, EIP, EAX */
	cmpl	eax, FUNC(sysCsUser) /* 是否是用户模式  */
	jne	    taExitSuper 	     /* 返回 */
taExitUser:
#ifdef CONFIG_DELTAOS
	pushl	edx	 /* 保存edx */
#ifdef _WRS_CONFIG_SMP	
	_IA32_PER_CPU_VALUE_GET(eax,edx,threadIdCurrent) /* 获取当前线程控制块地址 */
#else
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* 获取当前线程控制块地址 */
#endif
	popl    edx  /* 恢复edx */

	decl CORE_TCB_EXC_CNT(eax) /* 异常计数减一，表明当前在异常栈上 */
#endif
	movl	FUNC(sysDsUser), eax /* 设置为系统态段选择子 */
	movw	ax,ds
	movw	ax,es
	movw	ax,fs
	movw	ax,gs
#endif
taExitSuper:
	popl	eax		/* 恢复eax */
	iret			/* 返回到任务 */

/* 硬件断点和自陷异常入口处理
 *  (1) 单步异常	Trap(自陷类异常) DR6寄存器的        BS=1
 *  (2) 硬件指令异常(指令断点) Falt(故障类异常) Bn=1
 *  (3) 数据断点	Trap(自陷类异常)  DR6寄存器的      Bn=1
*/
	.balign 16,0x90
FUNC_LABEL(taTraceExceptionStub)
	cli
	andl	$0xfffffeff,0x8(esp) /* 清除栈上的EFLAGS单步标志，系统态栈上寄存器为EFLAGS, CS, EIP，
									   否则为SS, ESP, EFLAGS, CS, EIP */
	pushl	eax			      /* 保存EAX寄存器 */
#ifdef _TA_CONFIG_RTP
	movl	FUNC(sysCsUser), eax /* 是否是用户模式  */
	cmpl	eax, 0x8(esp)

	jne	taTraceStubSuper /* 不是用户态跳转到taTraceStubSuper执行 */

taTraceStubUser:
#ifdef CONFIG_DELTAOS
	pushl	edx	 /* 保存edx */
#ifdef _WRS_CONFIG_SMP	
	_IA32_PER_CPU_VALUE_GET(eax,edx,threadIdCurrent) /* 获取当前线程控制块地址 */
#else
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* 获取当前线程控制块地址 */
#endif
	popl    edx  /* 恢复edx */

	incl CORE_TCB_EXC_CNT(eax) /* 异常计数加一，表明当前在异常栈上 */
#endif
	popl    eax             /* 恢复EAX寄存器 */
	pushal                  /* 寄存所有通用寄存器，依次把eax->ecx->edx->ebx->esp->ebp->esi->edi压入栈中 */
	movl	0x2c(esp), eax	/* 获取用户模式的SP */
	movl	eax, 0x0c(esp)	/* 存放用户态的SP在栈上，REG_ESP为REG_SET结构体的偏移  */

	movl	FUNC(sysDsSuper), eax /* 设置段寄存器 */
	movw    ax,ds
	movw    ax,es
	movw    ax,fs
	movw    ax,gs

	jmp	taTraceStubCont /* 态跳转到taTraceStubCont执行 */
#endif
taTraceStubSuper:
	popl    eax   /* 恢复EAX寄存器 */
	pushal        /* 寄存所有通用寄存器，依次把eax->ecx->edx->ebx->esp->ebp->esi->edi压入栈中 */

taTraceStubCont:
	movl	esp,eax
	pushl	ebp
	movl	esp,ebp
	pushl	$1	    /* 压栈vector,单步和自陷异常号为1 */
	pushl	eax	    /* 将ESP寄存器值压栈,栈上保存了异常上下文  */
	leal	0x20+0x4(ebp),eax
	pushl	eax		/* 压栈info */

	movl    db6,eax	/* 设置DR6寄存器到EAX */
	btl	$14,eax		/* 是否是自陷异常 */
	jc	taTrace0	/* 如果是跳转到taTrace0 */
	orl	$0x00010000, 0x28+0x10(ebp) /* 设置RF标志，表示异常返回执行第一条指令不产生异常,指令断点为故障类异常，CPU总线在取指时
								            对比设置的断点地址，所以产生异常时指令还未执行，所以恢复时设置RF标志，让其 异常返回执行第一条指令不产生异常*/
	.balign 16,0x90
taTrace0:
	call	FUNC(taAachExceptionHandler) /* C语言异常处理 */

	addl	$0x10,esp /* 栈指针加16，因为 调用taExceptionHandler函数时压栈了三个参数+ebp */

	xorl	eax,eax
	movl	eax,db6	  /* 清除DR6状态位 */

	popal			  /* 恢复所有通用寄存器 */

#ifdef _TA_CONFIG_RTP
	pushl	eax	      /* 保存EAX寄存器，后续代码需要用到该寄存器 */
	movl	0x08(esp), eax	  /* 获取CS寄存器，当前栈上的寄存器为SS, ESP, EFLAGS, CS, EIP，EAX */
	cmpl	eax, FUNC(sysCsUser) /* 是否是用户模式  */
	jne	taTraceExitSuper	      /* 返回 */

taTraceExitUser:
#ifdef CONFIG_DELTAOS
	pushl	edx	 /* 保存edx */
#ifdef _WRS_CONFIG_SMP	
	_IA32_PER_CPU_VALUE_GET(eax,edx,threadIdCurrent) /* 获取当前线程控制块地址 */
#else
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* 获取当前线程控制块地址 */
#endif
	popl    edx  /* 恢复edx */

	decl CORE_TCB_EXC_CNT(eax) /* 异常计数减一，表明当前在异常栈上 */
#endif
	movl	FUNC(sysDsUser), eax /* 恢复为用户态段选择子 */
	movw    ax,ds
	movw    ax,es
	movw    ax,fs
	movw    ax,gs
taTraceExitSuper:
	popl	eax	/* 恢复 eax寄存器 */
#endif
	iret	    /* 返回到任务 */

.global taTrap
taTrap:
    int3
    ret

