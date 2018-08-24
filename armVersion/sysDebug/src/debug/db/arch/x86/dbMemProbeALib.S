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
 * @file:dbMemProbeALib.S
 * @brief:
 *             <li>内存探测汇编代码支持</li>
 */

/************************头 文 件*****************************/
#define ASM_USE

#include <dbAsm.h>
/************************宏 定 义******************************/

/************************类型定义*****************************/
#define ARG1	8
#define ARG2	12
#define ARG3	16

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/
.globl GTEXT(taMemProbeSup)
.globl GTEXT(taMemProbeByteRead)
.globl GTEXT(taMemProbeByteWrite)
.globl GTEXT(taMemProbeWordRead)
.globl GTEXT(taMemProbeWordWrite)
.globl GTEXT(taMemProbeLongRead)
.globl GTEXT(taMemProbeLongWrite)
.globl GTEXT(taMemProbeErrorRtn)

/************************函数实现*****************************/

.text
.code32
.balign 16

/*
 * 该函数试图读写内存，如果一个异常发生内存探测异常处理函数taMemProbeTrapHandle将
 * 把PC设置为taMemProbeErrorRtn，使其taMemProbeSup函数返回一个错误号，表示内存读写
 * 失败，该内存地址无法读写，函数原型如下：
 * STATUS taMemProbeSup(length, src, dest)
 *     int length: length of cell to test (1, 2, 4)
 *     char *src : address to read
 *     char *dest: address to write
 */
FUNC_LABEL(taMemProbeSup)
	pushl	ebp
	movl	esp,ebp

	pushl	esi  /* 保存非易失寄存器 */
	pushl	edi

	movl	ARG2(ebp),esi	/* 获取src参数 */
	movl	ARG3(ebp),edi	/* 获取dest参数 */

	xorl	eax,eax	        /* 设置当前状态为OK */

	movl	ARG1(ebp),edx   /* 获取length参数 */

	cmpl	$1,edx
    je      taMemProbeByteRead
	cmpl	$2,edx
    je      taMemProbeWordRead
	cmpl	$4,edx
    je      taMemProbeLongRead

	jmp     taMemProbeErrorRtn

	.balign 16,0x90
FUNC_LABEL(taMemProbeByteRead)
	movb	(esi),cl
FUNC_LABEL(taMemProbeByteWrite)
	movb	cl,(edi)

	jmp	tapRtn

	.balign 16,0x90
FUNC_LABEL(taMemProbeWordRead)
	movw	(esi),cx
FUNC_LABEL(taMemProbeWordWrite)
	movw	cx,(edi)

	jmp	tapRtn

	.balign 16,0x90
FUNC_LABEL(taMemProbeLongRead)
	movl	(esi),ecx
FUNC_LABEL(taMemProbeLongWrite)
	movl	ecx,(edi)

	jmp	tapRtn

	.balign 16,0x90
FUNC_LABEL(taMemProbeErrorRtn)
	movl	$-1,eax	 /* 设置错误号为ERROR */

tapRtn:
	popl	edi	  /* 恢复非易失寄存器 */
	popl	esi

    leave
	ret
