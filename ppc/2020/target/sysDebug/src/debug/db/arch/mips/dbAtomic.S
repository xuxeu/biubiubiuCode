/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * 修改历史:
 * 2013-12-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbAtomic.S
 * @brief:
 *             <li>MIPS相关原子操作接口</li>
 */
#ifdef CONFIG_CORE_SMP

#define ASM_USE

/************************头 文 件*****************************/
#include "dbAsm.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/
.globl  _atomic_32_add
.globl  _atomic_32_sub
.globl  _atomic_32_inc
.globl  _atomic_32_dec
.globl  _atomic_32_get
.globl  _atomic_32_set
.globl  _atomic_32_clear
.globl  _atomic_32_nand
.globl  _atomic_32_and
.globl  _atomic_32_or
.globl  _atomic_32_xor
.globl  _atomic_32_cas
.globl  taTas
.text
.set    reorder

/******************************************************************************
*
* _atomic_32_add - add the value to the specified location
*
* Using an atomic operation, add value to the specified location
*
* RETURNS: previous value at target location
*
* \NOMANUAL
*
* atomicVal_t _atomic_32_add (atomic_t *target, atomicVal_t value)
*
*/
	.ent _atomic_32_add
FUNC_LABEL(_atomic_32_add)
	.set noreorder
1:
	ll	v0, 0(a0)	/* get current value and atomic check */
	addu	t0, a1, v0
	sc	t0, 0(a0)
	beqz	t0, 1b		/* if t1 is 0, atomic op failed */
	nop

	j	ra
	nop
	.set reorder
	FUNC_END(_atomic_32_add)
	.end _atomic_32_add

/******************************************************************************
*
* _atomic_32_sub - subtract the value from the specified location value
*
* Using an atomic operation, subtract value from the specified location
*
* RETURNS: previous value at target location
*
* \NOMANUAL
*
* atomicVal_t _atomic_32_sub (atomic_t *target, atomicVal_t value)
*
*/

	.ent _atomic_32_sub
FUNC_LABEL(_atomic_32_sub)
	.set noreorder
1:
	ll	v0, 0(a0)	/* get current value and atomic check */
	subu	t0, v0, a1
	sc	t0, 0(a0)
	beqz	t0, 1b		/* check if atomic op was successful */
	nop

	j	ra
	nop
	.set reorder
	FUNC_END(_atomic_32_sub)
	.end _atomic_32_sub

/******************************************************************************
*
* _atomic_32_inc - increment the value at the specified location
*
* Using an atomic operation, increment the value at the specified location
*
* RETURNS: previous value at target location
*
* \NOMANUAL
*
* atomicVal_t _atomic_32_inc (atomic_t *target)
*
*/

	.ent _atomic_32_inc
FUNC_LABEL(_atomic_32_inc)
	.set noreorder
1:
	ll	v0, 0(a0)	/* get current value and atomic check */
	addu	t0, v0, 1
	sc	t0, 0(a0)
	beqz	t0, 1b		/* check if atomic op was successful */
	nop

	j	ra
	nop
	.set reorder
	FUNC_END(_atomic_32_inc)
	.end _atomic_32_inc

/******************************************************************************
*
* _atomic_32_nand - nand the value at the specified location with supplied value
*
* Using an atomic operation, nand the value at the specified location with the
* supplied value.
*
* RETURNS: previous value at target location
*
* \NOMANUAL
*
* atomicVal_t _atomic_32_nand (atomic_t *target, atomicVal_t value)
*
*/

	.ent _atomic_32_nand
FUNC_LABEL(_atomic_32_nand)
	.set noreorder
1:
	ll	v0, 0(a0)	/* get current value and atomic check */
	and	t0, a1, v0
	not	t0
	sc	t0, 0(a0)
	beqz	t0, 1b		/* check if atomic op was successful */
	nop

	j	ra
	nop
	.set reorder
	FUNC_END(_atomic_32_nand)
	.end _atomic_32_nand

/******************************************************************************
*
* _atomic_32_and - and the value at the specified location with supplied value
*
* Using an atomic operation, and the value at the specified location with the
* supplied value.
*
* RETURNS: previous value at target location
*
* \NOMANUAL
*
* atomicVal_t _atomic_32_and (atomic_t *target, atomicVal_t value)
*
*/

	.ent _atomic_32_and
FUNC_LABEL(_atomic_32_and)
	.set noreorder
1:
	ll	v0, 0(a0)	/* get current value and atomic check */
	and	t0, a1, v0
	sc	t0, 0(a0)
	beqz	t0, 1b		/* check if atomic op was successful */
	nop

	j	ra
	nop
	.set reorder
	FUNC_END(_atomic_32_and)
	.end _atomic_32_and

/******************************************************************************
*
* _atomic_32_or - or the value at the specified location with supplied value
*
* Using an atomic operation, or the value at the specified location with the
* supplied value.
*
* RETURNS: previous value at target location
*
* \NOMANUAL
*
* atomicVal_t _atomic_32_or (atomic_t *target, atomicVal_t value)
*
*/

	.ent _atomic_32_or
FUNC_LABEL(_atomic_32_or)
	.set noreorder
1:
	ll	v0, 0(a0)	/* get current value and atomic check */
	or	t0, a1, v0
	sc	t0, 0(a0)
	beqz	t0, 1b		/* check if atomic op was successful */
	nop

	j	ra
	nop
	.set reorder
	FUNC_END(_atomic_32_or)
	.end _atomic_32_or


/******************************************************************************
*
* _atomic_32_xor - xor the value at the specified location with supplied value
*
* Using an atomic operation, xor the value at the specified location with the
* supplied value.
*
* RETURNS: previous value at target location
*
* \NOMANUAL
*
* atomicVal_t _atomic_32_xor (atomic_t *target, atomicVal_t value)
*
*/

	.ent _atomic_32_xor
FUNC_LABEL(_atomic_32_xor)
	.set noreorder
1:
	ll	v0, 0(a0)	/* get current value and atomic check */
	xor	t0, a1, v0
	sc	t0, 0(a0)
	beqz	t0, 1b		/* check if atomic op was successful */
	nop

	j	ra
	nop
	.set reorder
	FUNC_END(_atomic_32_xor)
	.end _atomic_32_xor

/******************************************************************************
*
* _atomic_32_dec - decrement the value at the specified location
*
* Using an atomic operation, decrement the value at the specified location
*
* RETURNS: previous value at target location
*
* \NOMANUAL
*
* atomicVal_t _atomic_32_dec (atomic_t *target)
*
*/

	.ent _atomic_32_dec
FUNC_LABEL(_atomic_32_dec)
	.set noreorder
1:
	ll	v0, 0(a0)	/* get current value and atomic check */
	subu	t0, v0, 1
	sc	t0, 0(a0)
	beqz	t0, 1b		/* check if atomic op was successful */
	nop

	j	ra
	nop
	.set reorder
	FUNC_END(_atomic_32_dec)
	.end _atomic_32_dec

/******************************************************************************
*
* _atomic_32_get - get the value at the specified location
*
* Using an atomic operation, get the value at the specified location
*
* RETURNS: long value at specified address
*
* \NOMANUAL
*
* atomicVal_t _atomic_32_get (atomic_t *target)
*
*/

	.ent _atomic_32_get
FUNC_LABEL(_atomic_32_get)
	.set noreorder
	lw	v0, 0(a0)	/* read value */

	j	ra
	nop
	.set reorder
	FUNC_END(_atomic_32_get)
	.end _atomic_32_get

/******************************************************************************
*
* _atomic_32_set - store value at the specified location
*
* Store value at the specified location and return previous value
*
* RETURNS: previous value at target location
*
* \NOMANUAL
*
* atomicVal_t _atomic_32_set (atomic_t *target, atomicVal_t value)
*
*/

	.ent _atomic_32_set
FUNC_LABEL(_atomic_32_set)
	.set noreorder
	move	t0, a1
1:
	ll	v0, 0(a0)	/* get current value */
	sc	t0, 0(a0)	/* save value */
	beqz	t0, 1b
	move	t0, a1

	j	ra
	nop
	.set reorder
	FUNC_END(_atomic_32_set)
	.end _atomic_32_set

/******************************************************************************
*
* _atomic_32_clear - clear memory at the specified location
*
* Using an atomic operation, clear memory at the specified location
*
* RETURNS: previous value at target location
*
* \NOMANUAL
*
* atomicVal_t _atomic_32_clear (atomic_t *target)
*
*/

	.ent _atomic_32_clear
FUNC_LABEL(_atomic_32_clear)
	.set noreorder
	move	t0, zero
1:
	ll	v0, 0(a0)	/* get current value */
	sc	t0, 0(a0)	/* save value */
	beqz	t0, 1b
	move	t0, zero

	j	ra
	nop
	.set reorder
	FUNC_END(_atomic_32_clear)
	.end _atomic_32_clear

/******************************************************************************
*
* _atomic_32_cas - compare old value to current value and modify to new value if same
*
* Using an atomic operation, update the value at the specified location
* to the new value if it is currently equal to the old value
*
* RETURNS: TRUE if swap performed else FALSE
*
* \NOMANUAL
*
* BOOL _atomic_32_cas (atomic_t *target, atomicVal_t old, atomicVal_t new)
*
*/
	.ent _atomic_32_cas
FUNC_LABEL(_atomic_32_cas)
	li	v0, 0	/* set initial result status to FALSE */
1:
	.set noreorder
	ll	t0, 0(a0)	/* get current value and atomic check */
	bne	t0, a1, 2f
	move	t1, a2		/* save a2 if sc fails and retry is needed */
	sc	t1, 0(a0)
	beqz	t1, 1b		/* check if atomic op was successful */
	nop
	.set reorder
	li	v0, 1	/* store worked, return TRUE */
2:
	j	ra
	FUNC_END(_atomic_32_cas)
	.end _atomic_32_cas

/******************************************************************************
*
* taTas -  Perform a test-and-set operation using the MIPS III load-linked,
*		store-conditional instructions.
*
* DESCRIPTION
*	For MIPS CPU_FAMILIES with load-linked, store-conditional support, this
*	routine is used to perform a test-and-set operation.  CPU_FAMILIES with
*	this support have the _WRS_MIPS_LL_SC macro defined.
*
* RETURNS
*	TRUE if the value had not been set, but is now.
*	FALSE if the value was set already.
*
* NOTE
*	This implementation requires that the address be in cacheable coherent
*	space.  There's no easy way to test for that.  (Typically, it will be
*	true if using KSEG0 with Config:K0 set properly, but there are other
*	conditions in which it is true which should not be excluded by this
*	implementation.)
*/
.ent	taTas
FUNC_LABEL(taTas)
	.set noreorder

	ori	t0, a0, 0x3
	andi	t1, a0, 0x3

	xori	t0, t0, 0x3		/* t0 = aligned address */
	xori	t1, t1, 0x3

	li	t2, 0xff
	sll	t1, t1, 0x3		/* t1 = bit shift */

	sll	t2, t2, t1		/* t2 = byte mask */

	sync

	ll	t3, 0(t0)		/* t3 = word containing flag */
	and	t4, t2, t3		/* t4 = flag */
	bne	t4, zero, 1f		/* if already set, just fail. */

	or	t4, t2, t3		/* t4 = word with flag set. */
	sc	t4, 0(t0)		/* store it back, t0 == sc flag */
	beq	t4, zero, 1f
	nop

	/* success: flag was previously unset, and new value written.*/
	jr	ra
	li	v0, 1

	/* failure: flag was previously set. */
1:	jr	ra
	li	v0, 0
	.set reorder
	FUNC_END(taTas)
	.end	taTas
#endif
