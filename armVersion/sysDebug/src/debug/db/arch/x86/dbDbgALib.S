/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * �޸���ʷ:
 * 2013-11-07         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbDbgALib.S
 * @brief:
 *             <li>�ϵ㡢�����쳣����</li>
 */

/************************ͷ �� ��*****************************/
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
/************************�� �� ��******************************/

#define CORE_OBJ_CORE_SIZE 0x40

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/
#ifdef CONFIG_TA_LP64
.data

.globl	FUNC(taAachExceptionHandler)/* �쳣�������� */
.globl	GTEXT(taBpExceptionStub)	/* �ϵ�����쳣��ں��� */
.globl	GTEXT(taTraceExceptionStub)	/* ���ݵ����쳣��ں��� */

.text
.balign 16

/* �ϵ��쳣������ں��� */
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

	mov	    %rsp,%rbp         /*RBPָ���쳣������*/
	
	decq	    0x80(%rbp)          /*��������Ĵ���ֵ�����ڶϵ��쳣�����������쳣��ָ����һ��ָ�������Ҫ����PCֵ��*/

	/*��64λ�£������ҵ�ǰ�������������β���ͨ��ѹջ������ͨ���Ĵ���RDI,RSI,RDX,RCX,R8,R9���Ρ� */
	
	mov     $3,%rdx			/* ����vector���ϵ��쳣��Ϊ3 ��*/
	                      
	mov     %rax,%rsi             /*����RSP�Ĵ�����ֵ��ջ�ϱ������쳣������ �� */

	lea	    0x80(%rbp),%rdi   /*����info*/

	call	FUNC(taAachExceptionHandler) /* C�����쳣�������� */

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

	iretq			/* ���ص����� */

/* Ӳ���ϵ�������쳣��ڴ���
 *  (1) �����쳣	Trap(�������쳣) DR6�Ĵ�����        BS=1
 *  (2) Ӳ��ָ���쳣(ָ��ϵ�) Falt(�������쳣) Bn=1
 *  (3) ���ݶϵ�	Trap(�������쳣)  DR6�Ĵ�����      Bn=1
*/
.text
.code64
.balign 16
FUNC_LABEL(taTraceExceptionStub)
	cli
	andq	$0xfffffffffffffeff,0x16(%rsp) /* ���ջ�ϵ�EFLAGS������־��ϵͳ̬ջ�ϼĴ���ΪEFLAGS, CS, EIP��
									   ����ΪSS, ESP, EFLAGS, CS, EIP */
	/*pushal         �Ĵ�����ͨ�üĴ��������ΰ�eax->ecx->edx->ebx->esp->ebp->esi->ediѹ��ջ�� */
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
									
	mov     $1,%rdx     /*����vector�������������쳣��Ϊ1 ��*/
	mov     %rax,%rsi  /*����RSP�Ĵ�����ֵ,ջ�ϱ������쳣�����ġ�  */
	
	lea	0x80(%rbp),%rax
	
	mov     %rax,%rdi   /*����info*/

	mov    db6,%rax	/* ����DR6�Ĵ�����RAX */
	bt	$14,%rax		/* �Ƿ��������쳣 */
	
	jc	taTrace0	/* ����������쳣������ת��taTrace0 */
	
	orl	$0x00010000, 0x90(%rbp) /* ����RF��־����ʾ�쳣����ִ�е�һ��ָ����������쳣��ָ��ϵ�Ϊ�������쳣��CPU������ȡָʱ
								   �Ա����õĶϵ��ַ�����Բ��������쳣ʱָ�δִ�У����Իָ�ʱ����RF��־������ �쳣����ִ��
								   ��һ��ָ������쳣������ʱָ��ɹ�ִ����󣬴����������RF��־�������ֿ���ʹ�ܵ����쳣��*/
	.balign 16,0x90
taTrace0:
	call	FUNC(taAachExceptionHandler) /* C�����쳣���� */

	xor		%rax,%rax
	mov	    %rax,db6	  /* ���DR6״̬λ */

      /* �ָ�����ͨ�üĴ��� */
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

	iretq	    /* ���ص����� */

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

.globl	FUNC(taAachExceptionHandler)/* �쳣�������� */
.globl	GTEXT(taBpExceptionStub)	/* �ϵ�����쳣��ں��� */
.globl	GTEXT(taTraceExceptionStub)	/* ���ݵ����쳣��ں��� */


.text
.code32
.balign 16

/* �ϵ��쳣������ں��� */
FUNC_LABEL(taBpExceptionStub)
	cli

#ifdef _TA_CONFIG_RTP
	pushl	eax	         /* ����eax */
	
	movl	FUNC(sysCsUser), eax/* �Ƿ����û�ģʽ  */
	cmpl	eax, 0x8(esp)
	jne	taStubSuper
taStubUser:

#ifdef CONFIG_DELTAOS
	pushl	edx	 /* ����edx */
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* ��ȡ��ǰ�߳̿��ƿ��ַ */
	popl    edx  /* �ָ�edx */

	incl WIND_TCB_excCnt_OFFSET(eax) /* �쳣������һ��������ǰ���쳣ջ�� */
#endif
	/* �����Ҫִ�е��쳣�������̽�С����Ҫ����ջ�л����������ѱ��жϵ����̵�ջ��ѡ���Ӻ�ջָ��ѹջ��
	        ��Ȩ��δ�ı��ѹջ����ΪEFLAGS, CS, EIP������ΪSS, ESP, EFLAGS, CS, EIP*/
	popl	eax     /* �ָ�eax */
	pushal		     /* �Ĵ�����ͨ�üĴ��������ΰ�eax->ecx->edx->ebx->esp->ebp->esi->ediѹ��ջ�� */
	movl	0x2c(esp), eax	/* ��ȡ�û�ģʽ��SP */
	movl	eax, 0x0c(esp)	/* ����û�̬��SP��ջ�ϣ�REG_ESPΪREG_SET�ṹ���ƫ��  */

	movl	FUNC(sysDsSuper), eax /* ���öμĴ��� */
	movw    ax,ds
	movw    ax,es
	movw    ax,fs


#ifndef CONFIG_TA_SMP
	movw    ax,gs      //gs��һ���Ƚ�����ļĴ����������������Ҫ�����ָ��ɻ�ַ������֮ǰ��6132�ǲ����ڵģ�����������Ҫ�������ǡ����ö�˺���ƣ���Ϊֻ�е��˲Ż��õ�����
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
	popl    eax        /* �ָ�eax */
		
#endif

	pushal             /* ��������ͨ�üĴ��������ΰ�eax->ecx->edx->ebx->esp(ִ��pushalʱ��esp)->ebp->esi->ediѹ��ջ�� */

taStubCont:
	movl	esp,eax
	pushl	ebp
	movl	esp,ebp
	decl	0x20+0x4(ebp) /* ��������Ĵ���ֵ�����ڶϵ��쳣�����������쳣��ָ����һ��ָ�������Ҫ����PCֵ */
	pushl	$3			  /* ѹջvector,�ϵ��쳣��Ϊ3 */
	pushl	eax	          /* ��ESP�Ĵ���ֵѹջ,ջ�ϱ������쳣������  */
	leal	0x20+0x4(ebp),eax
	pushl	eax		      /* ѹջinfo */
	call	FUNC(taAachExceptionHandler) /* C�����쳣�������� */

	addl	$0x10,esp  /* ջָ���16����Ϊ ����taExceptionHandler����ʱѹջ����������+ebp */
	popal			   /* �ָ�����ͨ�üĴ��� */

#ifdef _TA_CONFIG_RTP
	pushl	eax		   /* ����EAX�Ĵ���������������Ҫ�õ��üĴ��� */

	movl	0x08(esp), eax	     /* ��ȡCS�Ĵ�������ǰջ�ϵļĴ���ΪSS, ESP, EFLAGS, CS, EIP, EAX */
	cmpl	eax, FUNC(sysCsUser) /* �Ƿ����û�ģʽ  */
	jne	    taExitSuper 	     /* ���� */
taExitUser:
#ifdef CONFIG_DELTAOS
	pushl	edx	 /* ����edx */
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* ��ȡ��ǰ�߳̿��ƿ��ַ */
	popl    edx  /* �ָ�edx */

	decl WIND_TCB_excCnt_OFFSET(eax) /* �쳣������һ��������ǰ���쳣ջ�� */
#endif
	movl	FUNC(sysDsUser), eax /* ����Ϊϵͳ̬��ѡ���� */
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
	popl	eax		/* �ָ�eax */
#endif

	iret			/* ���ص����� */

/* Ӳ���ϵ�������쳣��ڴ���
 *  (1) �����쳣	Trap(�������쳣) DR6�Ĵ�����        BS=1
 *  (2) Ӳ��ָ���쳣(ָ��ϵ�) Falt(�������쳣) Bn=1
 *  (3) ���ݶϵ�	Trap(�������쳣)  DR6�Ĵ�����      Bn=1
*/
	.balign 16,0x90
FUNC_LABEL(taTraceExceptionStub)
	cli
	andl	$0xfffffeff,0x8(esp) /* ���ջ�ϵ�EFLAGS������־��ϵͳ̬ջ�ϼĴ���ΪEFLAGS, CS, EIP��
									   ����ΪSS, ESP, EFLAGS, CS, EIP */
#ifdef _TA_CONFIG_RTP
	pushl	eax			      /* ����EAX�Ĵ��� */
	movl	FUNC(sysCsUser), eax /* �Ƿ����û�ģʽ  */
	cmpl	eax, 0x8(esp)

	jne	taTraceStubSuper /* �����û�̬��ת��taTraceStubSuperִ�� */

taTraceStubUser:
#ifdef CONFIG_DELTAOS
	pushl	edx	 /* ����edx */
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* ��ȡ��ǰ�߳̿��ƿ��ַ */
	popl    edx  /* �ָ�edx */

	incl WIND_TCB_excCnt_OFFSET(eax) /* �쳣������һ��������ǰ���쳣ջ�� */
#endif
	popl    eax             /* �ָ�EAX�Ĵ��� */
	pushal                  /* ��������ͨ�üĴ��������ΰ�eax->ecx->edx->ebx->esp->ebp->esi->ediѹ��ջ�� */
	movl	0x2c(esp), eax	/* ��ȡ�û�ģʽ��SP */
	movl	eax, 0x0c(esp)	/* ����û�̬��SP��ջ�ϣ�REG_ESPΪREG_SET�ṹ���ƫ��  */

	movl	FUNC(sysDsSuper), eax /* ���öμĴ��� */
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

	jmp	taTraceStubCont /* ̬��ת��taTraceStubContִ�� */
	
taTraceStubSuper:
	popl    eax   /* �ָ�EAX�Ĵ��� */	
#endif

	pushal        /* ��������ͨ�üĴ��������ΰ�eax->ecx->edx->ebx->esp->ebp->esi->ediѹ��ջ�� */

taTraceStubCont:
	movl  	esp,eax
	pushl	ebp
	movl	       esp,ebp
	pushl	$1	    /* ѹջvector,�����������쳣��Ϊ1 */
	pushl	eax	    /* ��ESP�Ĵ���ֵѹջ,ջ�ϱ������쳣������  */
	leal	      0x20+0x4(ebp),eax
	pushl	eax		/* ѹջinfo */

	movl    db6,eax	/* ����DR6�Ĵ�����EAX */
	btl	$14,eax		/* �Ƿ�������(����)�쳣 */
	
	jc	taTrace0	      /* ����������쳣������ת��taTrace0 ��*/
	
	orl	$0x00010000, 0x28+0x10(ebp) /* ����RF��־����ʾ�쳣����ִ�е�һ��ָ����������쳣��ָ��ϵ�Ϊ�������쳣��CPU������ȡָʱ
								        �Ա����õĶϵ��ַ�����Բ��������쳣ʱָ�δִ�У����Իָ�ʱ����RF��־������ �쳣����ִ��
								        ��һ��ָ������쳣������ʱָ��ɹ�ִ����󣬴����������RF��־�������ֿ���ʹ�ܵ����쳣��*/
	.balign 16,0x90
taTrace0:
	call	FUNC(taAachExceptionHandler) /* C�����쳣���� */

	addl	$0x10,esp /* ջָ���16����Ϊ ����taExceptionHandler����ʱѹջ����������+ebp */

	xorl	eax,eax
	movl	eax,db6	  /* ���DR6״̬λ */

	popal			  /* �ָ�����ͨ�üĴ��� */

#ifdef _TA_CONFIG_RTP
	pushl	eax	      /* ����EAX�Ĵ���������������Ҫ�õ��üĴ��� */
	movl	0x08(esp), eax	  /* ��ȡCS�Ĵ�������ǰջ�ϵļĴ���ΪSS, ESP, EFLAGS, CS, EIP��EAX */
	cmpl	eax, FUNC(sysCsUser) /* �Ƿ����û�ģʽ  */
	jne	taTraceExitSuper	      /* ���� */

taTraceExitUser:
#ifdef CONFIG_DELTAOS
	pushl	edx	 /* ����edx */
	_IA32_PER_CPU_VALUE_GET(eax,edx,taskIdCurrent) /* ��ȡ��ǰ�߳̿��ƿ��ַ */
	popl    edx  /* �ָ�edx */

	decl WIND_TCB_excCnt_OFFSET(eax) /* �쳣������һ��������ǰ���쳣ջ�� */
#endif
	movl	FUNC(sysDsUser), eax /* �ָ�Ϊ�û�̬��ѡ���� */
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
	popl	eax	/* �ָ� eax�Ĵ��� */
#endif

	iret	    /* ���ص����� */

.global taTrap
taTrap:
    int3
    ret
#endif