/* $id:crt1.S  V1.0 2008/01/02 */

/******************************************************************************
 *  This source code has been made available to you by CORETEK  on
 *  AS-IS.Anyone receiving this source is licensed under
 *  CORETEK copyrights to use it in any way he or she deems fit,including
 *  copying it,modifying it,compiling it,and redistributing it either with
 *  or without modifictions.
 *
 *
 *  Any person who transfers this source code or any derivative work must
 *  include the CORETEK copyright notice, this paragraph,and the preceding
 *  two paragraphs in the transferred software.
 *
 *
 *       COPYRIGHT CORETEK CORPORATION 2001
 *     LICENSED MATERIAL - PROGRAM PROPERTY OF CORETEK
 *****************************************************************************/

/******************************************************************************
 *
 *  FILE: crt0.S
 *
 *  MODULE: bsp
 *
 *  PURPOSE: 调试方式下的启动代码
 *
 *  AUTHOR(S):Huang Wei
 *
 *  GROUP:SYSTEM DEPT.
 *
 *  DATE CREATED:2008/01/02
 *
 *  REFERENCE DOCUMENT ID:
 *
 *  MODIFICATIONS:
 *  Date          user Name       Description
 *  2008/01/02    Huang Wei       Create this file
 *
 *********************************************************************************/
#include <dbAsm.h>

#define	IndexWBInvalidate_S	 0x03
#define IndexWBInvalidate_D  0x01
#define IndexWBInvalidate_I  0x04

.text
.globl	_start
.align	2
.ent _start/*,0,*/
_start:
	nop
	nop
	nop
	nop
	
	/* set STATUS */
	mfc0	t0, CP0_STATUS
	andi 	t0, 0x0000		/* 32-bit, kernel mode, clear ERL and EXL, disable irq, disable IM5(i8259) and IM2(Bonito)  */
	mtc0	t0, CP0_STATUS
  
	/* clear CONTEXT */
	mtc0	$0, CONTEXT			/* DeltaOS only use kseg0, all tasks share single space address, no page mode */
	
	/* enable cache again */
	mfc0	t0, CONFIG			/* kseg0 's coherency */
	ori 	t0, 0x07
	xori	t0, 0x07
        ori     t0, 0x03
	mtc0	t0, CONFIG

	/*使能对64位地址的访问*/
	mfc0    t0, CP0_STATUS 
	li      t1, 0x00e0      /*KX(64位Kernel段访问),SX(64位Supervisor段访问),UX(64位User段访问)*/
	or      t0, t0, t1
	mtc0    t0, CP0_STATUS	

	/* build init stack */
	la  sp, _stack_top
	la	gp, _gp	/* set global pointer from compiler */  

	/*go to lmain */
    j		lmain    
    j       .                  		/*  loop forever,if return */
   
.end	_start
.size	_start,.-_start

#define	ssnop		.word 0x00000040
#define CPU_CYCLES_TWO          ssnop; ssnop; ssnop; ssnop

.globl	CPU_FlushCache
.ent        CPU_FlushCache
CPU_FlushCache:
	
    /* flush instruct and data cache */
    move    t3, a0	/* the beginning cached address */
    cache    1, 0(t3)
    CPU_CYCLES_TWO
    cache    1, 1(t3)
    CPU_CYCLES_TWO
    cache    1, 2(t3)
    CPU_CYCLES_TWO
    cache    1, 3(t3)
    CPU_CYCLES_TWO
    cache    0, 0(t3)

    CPU_CYCLES_TWO
    cache	IndexWBInvalidate_S, 0(t3)
    CPU_CYCLES_TWO
    cache	IndexWBInvalidate_S, 1(t3)
    CPU_CYCLES_TWO
    cache	IndexWBInvalidate_S, 2(t3)
    CPU_CYCLES_TWO
    cache	IndexWBInvalidate_S, 3(t3)
    CPU_CYCLES_TWO
    jr		ra
    nop
.end		CPU_FlushCache


.globl	CPU_FlushCacheL2
.ent  CPU_FlushCacheL2
CPU_FlushCacheL2:
	/* flush instruct and data cache */
	li	t3, 0x80000000			/* the beginning cached address */
	li	t4, 0x80000				/* godson2e has 512KB secondary cache */
10:
	cache	IndexWBInvalidate_S, 0(t3)
	cache	IndexWBInvalidate_S, 1(t3)
	cache	IndexWBInvalidate_S, 2(t3)
	cache	IndexWBInvalidate_S, 3(t3)
	addu	t3, 32
	subu		t4, 32
	bgtz		t4, 10b
	nop
	jr		ra
	nop
.end CPU_FlushCacheL2

.globl  CPU_FlushCacheL1Data
.ent        CPU_FlushCacheL1Data
CPU_FlushCacheL1Data:
	/* flush instruct and data cache */
	li  t3, 0x80000000          /* the beginning cached address */
	li  t4, 0x80000             /* godson2e has 512KB secondary cache */
11:
	cache   IndexWBInvalidate_D, 0(t3)
	cache   IndexWBInvalidate_D, 1(t3)
	cache   IndexWBInvalidate_D, 2(t3)
	cache   IndexWBInvalidate_D, 3(t3)
	addu    t3, 32
	subu        t4, 32
	bgtz        t4, 11b
	nop
	jr      ra
	nop
.end  CPU_FlushCacheL1Data

.globl  CPU_FlushCacheL1Int
.ent        CPU_FlushCacheL1Int
CPU_FlushCacheL1Int:
	/* flush instruct and data cache */
	li  t3, 0x80000000          /* the beginning cached address */
	li  t4, 0x80000             /* godson2e has 512KB secondary cache */
12:
	cache   IndexWBInvalidate_I, 0(t3)
	cache   IndexWBInvalidate_I, 1(t3)
	cache   IndexWBInvalidate_I, 2(t3)
	cache   IndexWBInvalidate_I, 3(t3)
	addu    t3, 32
	subu        t4, 32
	bgtz        t4, 12b
	nop
	jr      ra
    nop
.end CPU_FlushCacheL1Int

.globl	sysWbFlush
.ent	sysWbFlush
sysWbFlush:
	.set noreorder
	nop
  	sync 
  	nop
  	j    ra
	nop
  .set reorder
  .end	sysWbFlush
