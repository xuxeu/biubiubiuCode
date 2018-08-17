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
 *  FILE: crt1.S
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
#include <regdef.h>
#include <mips_cp0.h>
/*#include <lmain.h>*/

#define	IndexWBInvalidate_S	0x03

.text

.globl	_start
.align	2
.type	_start,@function
.ent _start/*,0,*/
_start:
	nop
	nop
	nop
	nop
	
	
	/* set STATUS */
	mfc0	t0, STATUS
	andi 	t0, 0x0000		/* 32-bit, kernel mode, clear ERL and EXL, disable irq, disable IM5(i8259) and IM2(Bonito)  */
	/*ori		t0, 0x2000*/		/* ???? */
	mtc0	t0, STATUS
  
	/* clear CONTEXT */
	mtc0	$0, CONTEXT			/* DeltaOS only use kseg0, all tasks share single space address, no page mode */
	
	
	/* enable cache again */
	mfc0	t0, CONFIG			/* kseg0 's coherency */
	ori 	t0, 0x07
	xori	t0, 0x07
	ori		t0, 0x03
	mtc0	t0, CONFIG

	/* build init stack */
	la  sp, _stack_top 
	
  
	/*go to lmain*/
	
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
	move		t3, a0			/* the beginning cached address */

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


.globl	sysWbFlush
.ent	sysWbFlush
sysWbFlush:
	.set noreorder
	nop
	#if 0 /*wangfq*/
        la      v0,sysWbFlush
        or      v0,DRAM_NONCACHE_VIRT_BASE
        sw      zero,0(v0)
        lw      v0,0(v0)
    #endif 
    	sync /*wangfq*/
    	nop
        j       ra
	nop
	.set reorder
	.end	sysWbFlush

.globl taTrap
taTrap:
	break

.end taTrap
