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
#include <vxWorks.h>
#include <objLibP.h>
//#include <delta.h>
//#include <coreSmpI86P.h>
#include <vxSmpP.h>
#include <private/offsetMacrosP.h>
#endif
/************************宏 定 义******************************/

#define CORE_OBJ_CORE_SIZE 0x40

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/
#ifdef CONFIG_TA_LP64
.data

.globl	FUNC(taAachExceptionHandler)/* 异常处理函数 */
.globl	GTEXT(taBpExceptionStub)	/* 断点调试异常入口函数 */
.globl	GTEXT(taTraceExceptionStub)	/* 自陷调试异常入口函数 */

.text
.balign 16

/* 断点异常处理入口函数 */
FUNC_LABEL(taBpExceptionStub)
	cli

	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rsp
	push	%rbp
	push	%rsi
	push	%rdi
	push    %r8
	push    %r9
	push    %r10
	push    %r11
	push    %r12
	push    %r13
	push    %r14
	push    %r15

taStubCont:
	mov	    %rsp,%rax

	mov	    %rsp,%rbp         /*RBP指向异常上下文*/
	
	decq	    0x80(%rbp)          /*调整程序寄存器值，由于断点异常属于自陷类异常，指向下一条指令，所以需要调整PC值。*/

	/*在64位下，从左到右的前六个参数，传参不是通过压栈，而是通过寄存器RDI,RSI,RDX,RCX,R8,R9传参。 */
	
	mov     $3,%rdx			/* 传入vector，断点异常号为3 。*/
	                      
	mov     %rax,%rsi             /*传入RSP寄存器的值，栈上保存了异常上下文 。 */

	lea	    0x80(%rbp),%rdi   /*传入info*/

	call	FUNC(taAachExceptionHandler) /* C代码异常处理函数 */

	pop     %r15
	pop     %r14
	pop     %r13
	pop     %r12
	pop     %r11
	pop     %r10
	pop     %r9
	pop     %r8
	pop	    %rdi
	pop     %rsi
	pop	    %rbp
	pop	    %rsp
	pop     %rbx
	pop     %rdx
	pop     %rcx
	pop 	    %rax

	iretq			/* 返回到任务 */

/* 硬件断点和自陷异常入口处理
 *  (1) 单步异常	Trap(自陷类异常) DR6寄存器的        BS=1
 *  (2) 硬件指令异常(指令断点) Falt(故障类异常) Bn=1
 *  (3) 数据断点	Trap(自陷类异常)  DR6寄存器的      Bn=1
*/
.text
.code64
.balign 16
FUNC_LABEL(taTraceExceptionStub)
	cli
	andq	$0xfffffffffffffeff,0x16(%rsp) /* 清除栈上的EFLAGS单步标志，系统态栈上寄存器为EFLAGS, CS, EIP，
									   否则为SS, ESP, EFLAGS, CS, EIP */
	/*pushal         寄存所有通用寄存器，依次把eax->ecx->edx->ebx->esp->ebp->esi->edi压入栈中 */
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rsp
	push	%rbp
	push	%rsi
	push	%rdi
	push      %r8
	push      %r9
	push      %r10
	push      %r11
	push      %r12
	push      %r13
	push      %r14
	push      %r15

taTraceStubCont:
	mov 	%rsp,%rax

	mov		%rsp,%rbp
									
	mov     $1,%rdx     /*传入vector，单步和自陷异常号为1 。*/
	mov     %rax,%rsi  /*传入RSP寄存器的值,栈上保存了异常上下文。  */
	
	lea	0x80(%rbp),%rax
	
	mov     %rax,%rdi   /*传入info*/

	mov    db6,%rax	/* 设置DR6寄存器到RAX */
	bt	$14,%rax		/* 是否是自陷异常 */
	
	jc	taTrace0	/* 如果是自陷异常，则跳转到taTrace0 */
	
	orl	$0x00010000, 0x90(%rbp) /* 设置RF标志，表示异常返回执行第一条指令不产生调试异常。指令断点为故障类异常，CPU总线在取指时
								   对比设置的断点地址，所以产生调试异常时指令还未执行，所以恢复时设置RF标志，让其 异常返回执行
								   第一条指令不产生异常。返回时指令成功执行完后，处理器会清除RF标志，这样又可以使能调试异常。*/
	.balign 16,0x90
taTrace0:
	call	FUNC(taAachExceptionHandler) /* C语言异常处理 */

	xor		%rax,%rax
	mov	    %rax,db6	  /* 清除DR6状态位 */

      /* 恢复所有通用寄存器 */
	pop     %r15
	pop     %r14
	pop     %r13
	pop     %r12
	pop     %r11
	pop     %r10
	pop     %r9
	pop     %r8
	pop	    %rdi
	pop     %rsi
	pop	    %rbp
	pop	    %rsp
	pop     %rbx
	pop     %rdx
	pop     %rcx
	pop 	    %rax

	iretq	    /* 返回到任务 */

.text
.code64
.balign 16

.global taTrap
taTrap:
    int3
    ret



.global testTrap
testTrap:
	mov	    $1,%rax
	mov	    $2,%rcx
	mov	    $3,%rdx
	mov	    $4,%rbx
	mov	    $7,%rsi
	mov	    $8,%rdi
	mov     $9,%r8
	mov     $10,%r9
	mov     $11,%r10
	mov     $12,%r11
	mov     $13,%r12
	mov     $14,%r13
	mov     $15,%r14
	mov     $16,%r15
	int3
	ret



#else


.data

.globl	FUNC(taAachExceptionHandler)/* 异常处理函数 */
.globl	GTEXT(taBpExceptionStub)	/* 断点调试异常入口函数 */
.globl	GTEXT(taTraceExceptionStub)	/* 自陷调试异常入口函数 */


.text
.code32
.balign 16

/* 断点异常处理入口函数 */
FUNC_LABEL(taBpExceptionStub)
	cli

#ifdef _TA_CONFIG_RTP
	pushl	eax	         /* 保存eax */
	
	movl	FUNC(sysCsUser), eax/* 是否是用户模式  */
	cmpl	eax, 0x8(esp)
	jne	taStubSuper
taStubUser:

#ifdef CONFIG_DELTAOS
	pushl	edx	 /* 保存edx */
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* 获取当前线程控制块地址 */
	popl    edx  /* 恢复edx */

	incl WIND_TCB_excCnt_OFFSET(eax) /* 异常计数加一，表明当前在异常栈上 */
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


#ifndef CONFIG_TA_SMP
	movw    ax,gs      //gs是一个比较特殊的寄存器，如果改了它需要把它恢复成基址，这在之前的6132是不存在的，但是现在需要这样考虑。我用多核宏控制，因为只有单核才会用到它。
#else
	movl	FUNC(sysDsSuper), eax
	movw	ax,gs
    _IA32_CPU_ADRS_BASE_GET (eax, ecx) 

    /* set %eax = gs base register for cpu */
    movl    _WIND_VARS_cpu_cpuGsBaseCurrent_OFFSET (eax), eax
    movl    $0, edx
    movl    $IA32_GS_BASE,ecx
    wrmsr

#endif
	jmp	taStubCont
	
taStubSuper:
	popl    eax        /* 恢复eax */
		
#endif

	pushal             /* 保存所有通用寄存器，依次把eax->ecx->edx->ebx->esp(执行pushal时的esp)->ebp->esi->edi压入栈中 */

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

#ifdef _TA_CONFIG_RTP
	pushl	eax		   /* 保存EAX寄存器，后续代码需要用到该寄存器 */

	movl	0x08(esp), eax	     /* 获取CS寄存器，当前栈上的寄存器为SS, ESP, EFLAGS, CS, EIP, EAX */
	cmpl	eax, FUNC(sysCsUser) /* 是否是用户模式  */
	jne	    taExitSuper 	     /* 返回 */
taExitUser:
#ifdef CONFIG_DELTAOS
	pushl	edx	 /* 保存edx */
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* 获取当前线程控制块地址 */
	popl    edx  /* 恢复edx */

	decl WIND_TCB_excCnt_OFFSET(eax) /* 异常计数减一，表明当前在异常栈上 */
#endif
	movl	FUNC(sysDsUser), eax /* 设置为系统态段选择子 */
	movw	ax,ds
	movw	ax,es
	movw	ax,fs
#ifndef CONFIG_TA_SMP
	movw	ax,gs
#else
	movl  	FUNC(sysDsSuper), eax
	movw	ax,gs
	_IA32_CPU_ADRS_BASE_GET (eax, ecx) 
	
	/* set %eax = gs base register for cpu */
	movl	_WIND_VARS_cpu_cpuGsBaseCurrent_OFFSET (eax), eax
	movl	$0, edx
	movl	$IA32_GS_BASE,ecx
	wrmsr
#endif
	

taExitSuper:
	popl	eax		/* 恢复eax */
#endif

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
#ifdef _TA_CONFIG_RTP
	pushl	eax			      /* 保存EAX寄存器 */
	movl	FUNC(sysCsUser), eax /* 是否是用户模式  */
	cmpl	eax, 0x8(esp)

	jne	taTraceStubSuper /* 不是用户态跳转到taTraceStubSuper执行 */

taTraceStubUser:
#ifdef CONFIG_DELTAOS
	pushl	edx	 /* 保存edx */
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* 获取当前线程控制块地址 */
	popl    edx  /* 恢复edx */

	incl WIND_TCB_excCnt_OFFSET(eax) /* 异常计数加一，表明当前在异常栈上 */
#endif
	popl    eax             /* 恢复EAX寄存器 */
	pushal                  /* 保存所有通用寄存器，依次把eax->ecx->edx->ebx->esp->ebp->esi->edi压入栈中 */
	movl	0x2c(esp), eax	/* 获取用户模式的SP */
	movl	eax, 0x0c(esp)	/* 存放用户态的SP在栈上，REG_ESP为REG_SET结构体的偏移  */

	movl	FUNC(sysDsSuper), eax /* 设置段寄存器 */
	movw    ax,ds
	movw    ax,es
	movw    ax,fs
#ifndef CONFIG_TA_SMP
	movw    ax,gs
#else
	movl	FUNC(sysDsSuper), eax
	movw	ax,gs
	_IA32_CPU_ADRS_BASE_GET (eax, ecx) 
		
	/* set %eax = gs base register for cpu */
	movl	_WIND_VARS_cpu_cpuGsBaseCurrent_OFFSET (eax), eax
	movl	$0, edx
	movl	$IA32_GS_BASE,ecx
	wrmsr
#endif

	jmp	taTraceStubCont /* 态跳转到taTraceStubCont执行 */
	
taTraceStubSuper:
	popl    eax   /* 恢复EAX寄存器 */	
#endif

	pushal        /* 保存所有通用寄存器，依次把eax->ecx->edx->ebx->esp->ebp->esi->edi压入栈中 */

taTraceStubCont:
	movl  	esp,eax
	pushl	ebp
	movl	       esp,ebp
	pushl	$1	    /* 压栈vector,单步和自陷异常号为1 */
	pushl	eax	    /* 将ESP寄存器值压栈,栈上保存了异常上下文  */
	leal	      0x20+0x4(ebp),eax
	pushl	eax		/* 压栈info */

	movl    db6,eax	/* 保存DR6寄存器到EAX */
	btl	$14,eax		/* 是否是自陷(单步)异常 */
	
	jc	taTrace0	      /* 如果是自陷异常，则跳转到taTrace0 。*/
	
	orl	$0x00010000, 0x28+0x10(ebp) /* 设置RF标志，表示异常返回执行第一条指令不产生调试异常。指令断点为故障类异常，CPU总线在取指时
								        对比设置的断点地址，所以产生调试异常时指令还未执行，所以恢复时设置RF标志，让其 异常返回执行
								        第一条指令不产生异常。返回时指令成功执行完后，处理器会清除RF标志，这样又可以使能调试异常。*/
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
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* 获取当前线程控制块地址 */
	popl    edx  /* 恢复edx */

	decl WIND_TCB_excCnt_OFFSET(eax) /* 异常计数减一，表明当前在异常栈上 */
#endif
	movl	FUNC(sysDsUser), eax /* 恢复为用户态段选择子 */
	movw    ax,ds
	movw    ax,es
	movw    ax,fs
#ifndef CONFIG_TA_SMP
	movw    ax,gs
#else
	movl	FUNC(sysDsSuper), eax
	movw	ax,gs
	_IA32_CPU_ADRS_BASE_GET (eax, ecx) 
	
	/* set %eax = gs base register for cpu */
	movl	_WIND_VARS_cpu_cpuGsBaseCurrent_OFFSET (eax), eax
	movl	$0, edx
	movl	$IA32_GS_BASE,ecx
	wrmsr
#endif
taTraceExitSuper:
	popl	eax	/* 恢复 eax寄存器 */
#endif

	iret	    /* 返回到任务 */

.global taTrap
taTrap:
    int3
    ret
#endif
