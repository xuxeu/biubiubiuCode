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
#define OK	0
#define ERROR -1

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/
.globl  taMemProbeSup
.globl	taMemProbeByteRead
.globl	taMemProbeByteWrite
.globl	taMemProbeShortRead
.globl	taMemProbeShortWrite
.globl	taMemProbeWordRead
.globl	taMemProbeWordWrite
.globl	taMemProbeErrorRtn

/************************函数实现*****************************/
.text
.set	reorder

/*
 * 该函数试图读写内存，如果一个异常发生内存探测异常处理函数taMemProbeTrapHandle将
 * 把PC设置为taMemProbeErrorRtn，使其taMemProbeSup函数返回一个错误号，表示内存读写
 * 失败，该内存地址无法读写，函数原型如下：
 * STATUS taMemProbeSup(length, src, dest)
 *     int length: length of cell to test (1, 2, 4)
 *     char *src : address to read
 *     char *dest: address to write
 */
.ent	taMemProbeSup
FUNC_LABEL(taMemProbeSup)
	li	v0, OK		/* default return to OK */
	li	t0,1		/* byte length operation */
	beq	a0,t0,taMemProbeByteRead
	li	t0,2		/* short length operation */
	beq	a0,t0,taMemProbeShortRead
	li	t0,4		/* word length operation */
	beq	a0,t0,taMemProbeWordRead
	b	taMemProbeErrorRtn	/* invalid length specified */

	.set noreorder
taMemProbeByteRead:
	lbu	t0,0(a1)
taMemProbeByteWrite:
	sb	t0,0(a2)
	sync
	b	passRtn
	nop
taMemProbeShortRead:
	lhu	t0,0(a1)
taMemProbeShortWrite:
	sh	t0,0(a2)
	sync
	b	passRtn
	nop
taMemProbeWordRead:
	lw	t0,0(a1)
taMemProbeWordWrite:
	sw	t0,0(a2)
	sync
	b	passRtn
	nop
	.set reorder

taMemProbeErrorRtn:
	li	v0, ERROR
passRtn:
	j	ra
	FUNC_END(taMemProbeSup)
	.end	taMemProbeSup
