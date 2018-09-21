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
.text


FUNC_LABEL(_atomic_32_clear)
	li	p1, 0
vxAtomicClear_again:
	lwarx   p2, 0, p0      /* load and reserve */
	stwcx.  p1, 0, p0      /* store new value if still reserved */
	bne-    vxAtomicClear_again /* loop if lost reservation */
	mr      p0, p2         /* return old value */
	blr
FUNC_END(_atomic_32_clear)

/*******************************************************************************
*
* vxAtomicSet - atomically set a memory location
*
* This routine atomically sets the contents of <target> to <value> and returns
* the old value that was in <target>.
* Note that all CPU architectures supported by VxWorks can atomically write to
* a variable of size atomic_t without the need to use this routine.  This
* routine is intended for software that needs to atomically fetch and replace
* the value of a memory location. Various CPU architectures may impose
* restrictions with regards to the alignment and cache attributes of the
* atomic_t type.
*
* This routine can be used from both task and interrupt level.
*
* RETURNS: Contents of <target> before the atomic operation
*
* ERRNO: N/A
*
* SEE ALSO: vxAtomicLib
*
* atomicVal_t vxAtomicSet
*    (
*    atomic_t *target,	/@ memory location to set @/
*    atomicVal_t value	/@ set with this value @/
*    )
*
*/


FUNC_LABEL(_atomic_32_set)
	lwarx   p2, 0, p0      /* load and reserve */
	stwcx.  p1, 0, p0      /* store new value if still reserved */
	bne-    _atomic_32_set      /* loop if lost reservation */
	mr      p0, p2         /* return old value */
	blr
FUNC_END(_atomic_32_set)

/******************************************************************************
*
* vxAtomicGet - Get the value of a shared memory atomically
*
* This routine atomically retrieves the value in *target
*
* long vxAtomicGet
*     (
*     atomic_t * target    /@ address of atom to be retrieved @/
*     )
*
* RETURN: value read from address target.
*
*/


FUNC_LABEL(_atomic_32_get)
	lwz	p0, 0(p0)	/* no need for reservation */
	blr
FUNC_END(_atomic_32_get)

/*******************************************************************************
*
* vxAtomicInc - atomically increment a memory location
*
* This routine atomically increments the value in <target>.  The operation is
* done using unsigned integer arithmetic.  Various CPU architectures may impose
* restrictions with regards to the alignment and cache attributes of the
* atomic_t type.
*
* This routine can be used from both task and interrupt level.
*
* RETURNS: Contents of <target> before the atomic operation
*
* ERRNO: N/A
*
* SEE ALSO: vxAtomicLib
*
* atomicVal_t vxAtomicInc
*    (
*    atomic_t *target,	/@ memory location to increment @/
*    )
*
*/

FUNC_LABEL(_atomic_32_inc)
	li	p1, 1
vxAtomicInc_again:
	lwarx	p2, 0, p0      /* load and reserve */
	add	p3, p1, p2     /* add word */
	stwcx.	p3, 0, p0      /* store new value if still reserved */
	bne- 	vxAtomicInc_again    /* loop if lost reservation */
	mr      p0, p2         /* return old value */
	blr
FUNC_END(_atomic_32_inc)

/*******************************************************************************
*
* vxAtomicAdd - atomically add a value to a memory location
*
* This routine atomically adds the contents of <target> and <value>, placing
* the result in <target>. The operation is done using signed integer arithmetic.
* Various CPU architectures may impose restrictions with regards to the
* alignment and cache attributes of the atomic_t type.
*
* This routine can be used from both task and interrupt level.
*
* RETURNS: Contents of <target> before the atomic operation
*
* ERRNO: N/A
*
* atomicVal_t vxAtomicAdd
*    (
*    atomic_t *target,	/@ memory location to add to @/
*    atomicVal_t value	/@ value to add @/
*    )
*/

FUNC_LABEL(_atomic_32_add)
	lwarx	p2, 0, p0      /* load and reserve */
	add	p3, p1, p2     /* add word */
	stwcx.	p3, 0, p0      /* store new value if still reserved */
	bne- 	_atomic_32_add      /* loop if lost reservation */
	mr      p0, p2         /* return old value */
	blr
FUNC_END(_atomic_32_add)

/*******************************************************************************
*
* vxAtomicDec - atomically decrement a memory location
*
* This routine atomically decrements the value in <target>.  The operation is
* done using unsigned integer arithmetic.  Various CPU architectures may impose
* restrictions with regards to the alignment and cache attributes of the
* atomic_t type.
*
* This routine can be used from both task and interrupt level.
*
* RETURNS: Contents of <target> before the atomic operation
*
* ERRNO: N/A
*
* SEE ALSO: vxAtomicLib
*
* atomicVal_t vxAtomicDec
*    (
*    atomic_t *target,	/@ memory location to decrement @/
*    )
*
*/

FUNC_LABEL(_atomic_32_dec)
	li	p1, 1
vxAtomicDec_again:
	lwarx	p2, 0, p0       /* load and reserve */
	subf	p3, p1, p2      /* subtract word */
	stwcx.	p3, 0, p0       /* store new value if still reserved */
	bne- 	vxAtomicDec_again     /* loop if lost reservation */
	mr      p0, p2          /* return old value */
	blr
FUNC_END(_atomic_32_dec)

/*******************************************************************************
*
* vxAtomicSub - atomically subtract a value from a memory location
*
* This routine atomically subtracts <value> from the contents of <target>,
* placing the result in <target>.  The operation is done using signed integer
* arithmetic. Various CPU architectures may impose restrictions with regards to
* the alignment and cache attributes of the atomic_t type.
*
* This routine can be used from both task and interrupt level.
*
* RETURNS: Contents of <target> before the atomic operation
*
* ERRNO: N/A
*
* SEE ALSO: vxAtomicLib
*
* atomicVal_t vxAtomicSub
*    (
*    atomic_t *target,	/@ memory location to subtract from @/
*    atomicVal_t value	/@ value to subtract @/
*    )
*
*/


FUNC_LABEL(_atomic_32_sub)
	lwarx	p2, 0, p0       /* load and reserve */
	subf	p3, p1, p2      /* subtract word */
	stwcx.	p3, 0, p0       /* store new value if still reserved */
	bne- 	_atomic_32_sub       /* loop if lost reservation */
	mr      p0, p2          /* return old value */
	blr
FUNC_END(_atomic_32_sub)

/*******************************************************************************
*
* vxAtomicNand - atomically perform a bitwise NAND on a memory location
*
* This routine atomically performs a bitwise NAND operation of the contents of
* <target> and <value>, placing the result in <target>.
* Various CPU architectures may impose restrictions with regards to the
* alignment and cache attributes of the atomic_t type.
*
* This routine can be used from both task and interrupt level.
*
* RETURNS: Contents of <target> before the atomic operation
*
* ERRNO: N/A
*
* SEE ALSO: vxAtomicLib
*
* atomicVal_t vxAtomicNand
*    (
*    atomic_t *target,	/@ memory location to NAND @/
*    atomicVal_t value	/@ NAND with this value @/
*    )
*
*/

FUNC_LABEL(_atomic_32_nand)
	lwarx   p2, 0, p0       /* load and reserve */
	nand    p3, p1, p2      /* NAND word */
	stwcx.  p3, 0, p0       /* store new value if still reserved */
	bne-	_atomic_32_nand      /* loop if lost reservation */
	mr      p0, p2          /* return old value */
	blr
FUNC_END (_atomic_32_nand)

/*******************************************************************************
*
* vxAtomicAnd - atomically perform a bitwise AND on a memory location
*
* This routine atomically performs a bitwise AND operation of the contents of
* <target> and <value>, placing the result in <target>.
* Various CPU architectures may impose restrictions with regards to the
* alignment and cache attributes of the atomic_t type.
*
* This routine can be used from both task and interrupt level.
*
* RETURNS: Contents of <target> before the atomic operation
*
* ERRNO: N/A
*
* SEE ALSO: vxAtomicLib
*
* atomicVal_t vxAtomicAnd
*    (
*    atomic_t *target,	/@ memory location to AND @/
*    atomicVal_t value	/@ AND with this value @/
*    )
*
*/

FUNC_LABEL(_atomic_32_and)
	lwarx   p2, 0, p0       /* load and reserve */
	and     p3, p1, p2      /* AND word */
	stwcx.  p3, 0, p0       /* store new value if still reserved */
	bne-    _atomic_32_and       /* loop if lost reservation */
	mr      p0, p2          /* return old value */
	blr
FUNC_END (_atomic_32_and)

/*******************************************************************************
*
* vxAtomicOr - atomically perform a bitwise OR on memory location
*
* This routine atomically performs a bitwise OR operation of the contents of
* <target> and <value>, placing the result in <target>.
* Various CPU architectures may impose restrictions with regards to the
* alignment and cache attributes of the atomic_t type.
*
* This routine can be used from both task and interrupt level.
*
* RETURNS: Contents of <target> before the atomic operation
*
* ERRNO: N/A
*
* SEE ALSO: vxAtomicLib
*
* atomicVal_t vxAtomicOr
*    (
*    atomic_t *target,	/@ memory location to OR @/
*    atomicVal_t value	/@ OR with this value @/
*    )
*
*/

FUNC_LABEL(_atomic_32_or)
	lwarx   p2, 0, p0       /* load and reserve */
	or      p3, p1, p2      /* OR word */
	stwcx.  p3, 0, p0       /* store new value if still reserved */
	bne-    _atomic_32_or        /* loop if lost reservation */
	mr      p0, p2          /* return old value */
	blr
FUNC_END (_atomic_32_or)

/*******************************************************************************
*
* vxAtomicXor - atomically perform a bitwise XOR on a memory location
*
* This routine atomically performs a bitwise XOR operation of the contents of
* <target> and <value>, placing the result in <target>.
* Various CPU architectures may impose restrictions with regards to the
* alignment and cache attributes of the atomic_t type.
*
* This routine can be used from both task and interrupt level.
*
* RETURNS: Contents of <target> before the atomic operation
*
* ERRNO: N/A
*
* SEE ALSO: vxAtomicLib
*
* atomicVal_t vxAtomicXor
*    (
*    atomic_t *target,	/@ memory location to XOR @/
*    atomicVal_t value	/@ XOR with this value @/
*    )
*
*/

FUNC_LABEL(_atomic_32_xor)
	lwarx   p2, 0, p0       /* load and reserve */
	xor     p3, p1, p2      /* XOR word */
	stwcx.  p3, 0, p0       /* store new value if still reserved */
	bne-    _atomic_32_xor       /* loop if lost reservation */
	mr      p0, p2          /* return old value */
	blr
FUNC_END (_atomic_32_xor)

/*******************************************************************************
*
* vxCas - atomically compare-and-swap the contents of a memory location
*
* This routine performs an atomic compare-and-swap. testing that the contents of
* <target> contains <oldValue>, and if it does, setting the value of <target>
* to <newValue>. Various CPU architectures may impose restrictions with regards
* to the alignment and cache attributes of the atomic_t type.
*
* This routine can be used from both task and interrupt level.
*
* RETURNS: TRUE if the swap is actually executed, FALSE otherwise.
*
* ERRNO: N/A
*
* SEE ALSO: vxAtomicLib
*
* BOOL vxCas
*    (
*    atomic_t *target,	        /@ memory location to compare-and-swap @/
*    atomicVal_t oldValue,	/@ compare to this value @/
*    atomicVal_t newValue,	/@ swap with this value @/
*    )
*
*/

FUNC_LABEL(_atomic_32_cas)
	lwarx	p3, 0, p0       /* load and reserve */
	cmpw	p3, p1          /* operands equal */
	bne-	vxCasErr        /* skip if not */
	stwcx.	p2, 0, p0       /* store new value if still reserved */
	bne-	_atomic_32_cas       /* loop if lost reservation */
	li	p0, 1           /* cmp and swap successful, return 1 */
	blr
vxCasErr:
	li	p0, 0           /* cmp and swap failed, return 0 */
	blr
FUNC_END(_atomic_32_cas)

#endif
