/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2013-07-29         彭元志，北京科银京成技术有限公司
 *                              创建该文件。
 */

/*
 * @file： dbAsm.h
 * @brief：
 *	    <li>提供	MIPS汇编相关的宏定义</li>
 */
#ifndef _DB_ASM_H
#define _DB_ASM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
/************************头文件********************************/

/************************宏定义********************************/

#ifndef ASM_USE
#define ASM_USE
#endif

#ifdef __AS_REGISTER_PREFIX__
#undef __AS_REGISTER_PREFIX__
#endif

#define __AS_REGISTER_PREFIX__


#ifdef __AS_USER_LABEL_PREFIX__
#undef __AS_USER_LABEL_PREFIX__
#endif

#define __AS_USER_LABEL_PREFIX__

/* ANSI concatenation macros.  */
#define CONCAT1(a, b) CONCAT2(a, b)
#define CONCAT2(a, b) a ## b

/* Use the right prefix for global labels.  */
#define SYM(x) CONCAT1 (__AS_USER_LABEL_PREFIX__, x)

/* Use the right prefix for registers.  */
#define REG(x) CONCAT1 (__AS_REGISTER_PREFIX__, x)

/* Hazard macros */
/* Hazard definitions */

/* A MIPS Hazard is defined as any combination of instructions which
 * would cause unpredictable behavior in terms of pipeline delays,
 * cache misses, and exceptions.  Hazards are defined by the number
 * of CPU cycles that must pass between certain combinations of
 * instructions.  Because some MIPS CPUs single-issue nop instructions
 * while others dual-issue, the CPU cycles defined below are done so
 * according to the instruction issue mechanism available.
 */
#define SINGLE_ISSUE 0 	
#define DUAL_ISSUE   1 
#define CPU_CYCLES              DUAL_ISSUE

/* Using the issue mechanism definitions above, the MIPS CPU cycles
 * are defined below.
 */

#define	ssnop		.word 0x00000040

#if (CPU_CYCLES == SINGLE_ISSUE)
#define CPU_CYCLES_ONE          ssnop
#define CPU_CYCLES_TWO          ssnop; ssnop
#elif (CPU_CYCLES == DUAL_ISSUE)
#define CPU_CYCLES_ONE          ssnop; ssnop
#define CPU_CYCLES_TWO          ssnop; ssnop; ssnop; ssnop
#endif

/* Sixteen instructions are required to handle the VR5432 errata in
 * order to fill its instruction prefetch.  See HAZARD_VR5400 macro
 * for details.
 */
#define CPU_CYCLES_SIXTEEN      ssnop; ssnop; ssnop; ssnop; \
                                ssnop; ssnop; ssnop; ssnop; \
                                ssnop; ssnop; ssnop; ssnop; \
                                ssnop; ssnop; ssnop; ssnop				    

/* To assist with handling MIPS hazards, a number of categories of
 * hazards have been defined here.  
 *
 * HAZARD_TLB        After modifying tlb CP0 registers, do not use the
 *                   TLB for two CPU cycles.
 * HAZARD_ERET       After modifying the SR, do not return from an
 *                   exception for two CPU cycles.
 * HAZARD_INTERRUPT  After modifying the SR, interrupts do not lock
 *                   for two CPU cycles.
 * HAZARD_CP_READ    After a read from a Coprocessor register, the
 *                   result is not available for one CPU cycle.
 * HAZARD_CP_WRITE   After a write to a Coprocessor register, the
 *                   result is not effective for two CPU cycles.
 * HAZARD_CACHE_TAG  Cache TAG load and store instructions should
 *                   not be used withing one CPU cycle of modifying
 *                   the TAG registers.
 * HAZARD_CACHE      Cache instructions should not be used within 2
 *                   CPU cycles of each other.
 * HAZARD_VR5400     For the VR5432 CPU only.  Serialized instructions
 *                   (mtc0, ctc0, mfc0, cfc0, etc) must not appear within
 *                   16 instructions after a conditional branch or label.
 *
 * These hazard macros are intended for use with MIPS architecture-
 * dependent assembly files which require handling hazards.  For example,
 * suppose interrupts are being locked, to address the hazard, please
 * do the following:
 *
 * mtc0     t0, C0_SR
 * HAZARD_INTERRUPT
 * lw       t0, 0(a0)
 *
 * Similarly, when reading from a coprocessor register, please do the
 * following:
 *
 * mfc0     t0, C0_SR
 * HAZARD_CP_READ
 * and      t0, t0, t1
 * mtc0     t0, C0_SR
 * HAZARD_CP_WRITE
 *
 *
 * For more details on these categories, please refer to MIPS hazard
 * documentation.
 */

/* Hazard macros */
/* If this arch requires non-standard Hazard defs, include the hazard file
 * defined for this arch.  Otherwise, use the defaults.
 */

#ifdef HAZARD_FILE
#include HAZARD_FILE

#else
#define HAZARD_TLB       CPU_CYCLES_TWO
#define HAZARD_ERET      CPU_CYCLES_TWO
#define HAZARD_CP_READ   CPU_CYCLES_ONE
#define HAZARD_CP_WRITE  CPU_CYCLES_TWO
#define HAZARD_CACHE_TAG CPU_CYCLES_ONE
#define HAZARD_CACHE     CPU_CYCLES_TWO
#define HAZARD_INTERRUPT CPU_CYCLES_TWO
#endif

#define EXCEPTION_FRAME_SIZE  0xAC
#define MINIMUM_EXCHANDLE_STACK_SIZE (128)

/*
 * register names
 */
#define r0	$0
#define r1	$1
#define r2	$2
#define r3	$3
#define r4	$4
#define r5	$5
#define r6	$6
#define r7	$7
#define r8	$8
#define r9	$9
#define r10	$10
#define r11	$11
#define r12	$12
#define r13	$13
#define r14	$14
#define r15	$15
#define r16	$16
#define r17	$17
#define r18	$18
#define r19	$19
#define r20	$20
#define r21	$21
#define r22	$22
#define r23	$23
#define r24	$24
#define r25	$25
#define r26	$26
#define r27	$27
#define r28	$28
#define r29	$29
#define r30	$30
#define r31	$31

/*
 * FPU register names
 */
#define f0	$0
#define f1	$1
#define f2	$2
#define f3	$3
#define f4	$4
#define f5	$5
#define f6	$6
#define f7	$7
#define f8	$8
#define f9	$9
#define f10	$10
#define f11	$11
#define f12	$12
#define f13	$13
#define f14	$14
#define f15	$15
#define f16	$16
#define f17	$17
#define f18	$18
#define f19	$19
#define f20	$20
#define f21	$21
#define f22	$22
#define f23	$23
#define f24	$24
#define f25	$25
#define f26	$26
#define f27	$27
#define f28	$28
#define f29	$29
#define f30	$30
#define f31	$31

#define FP0_OFFSET  0 
#define FP1_OFFSET  1 
#define FP2_OFFSET  2 
#define FP3_OFFSET  3 
#define FP4_OFFSET  4 
#define FP5_OFFSET  5 
#define FP6_OFFSET  6 
#define FP7_OFFSET  7 
#define FP8_OFFSET  8 
#define FP9_OFFSET  9 
#define FP10_OFFSET 10 
#define FP11_OFFSET 11 
#define FP12_OFFSET 12 
#define FP13_OFFSET 13 
#define FP14_OFFSET 14 
#define FP15_OFFSET 15 
#define FP16_OFFSET 16 
#define FP17_OFFSET 17 
#define FP18_OFFSET 18 
#define FP19_OFFSET 19 
#define FP20_OFFSET 20 
#define FP21_OFFSET 21 
#define FP22_OFFSET 22 
#define FP23_OFFSET 23 
#define FP24_OFFSET 24 
#define FP25_OFFSET 25 
#define FP26_OFFSET 26 
#define FP27_OFFSET 27 
#define FP28_OFFSET 28 
#define FP29_OFFSET 29 
#define FP30_OFFSET 30 
#define FP31_OFFSET 31 
#define FPCS_OFFSET 32

#define zero $0	/* wired zero */
#define AT	$at	/* assembler temp */
#define v0	$2	/* return value */
#define v1	$3
#define a0	$4	/* argument registers a0-a3 */
#define a1	$5
#define a2	$6
#define a3	$7
#define t0	$8	/* caller saved  t0-t9 */
#define t1	$9
#define t2	$10
#define t3	$11
#define t4	$12
#define t5	$13
#define t6	$14
#define t7	$15
#define s0	$16	/* callee saved s0-s8 */
#define s1	$17
#define s2	$18
#define s3	$19
#define s4	$20
#define s5	$21
#define s6	$22
#define s7	$23
#define t8	$24
#define t9	$25
#define k0	$26	/* kernel usage */
#define k1	$27	/* kernel usage */
#define gp	$28	/* sdata pointer */
#define sp	$29	/* stack pointer */
#define s8	$30	/* yet another saved reg for the callee */
#define fp	$30	/* frame pointer - this is being phased out by MIPS */
#define ra	$31	/* return address */

#define	R_ZERO		R_R0
#define	R_AT		R_R1
#define	R_V0		R_R2
#define	R_V1		R_R3
#define	R_A0		R_R4
#define	R_A1		R_R5
#define	R_A2		R_R6
#define	R_A3		R_R7
#define	R_T0		R_R8
#define	R_T1		R_R9
#define	R_T2		R_R10
#define	R_T3		R_R11
#define	R_T4		R_R12
#define	R_T5		R_R13
#define	R_T6		R_R14
#define	R_T7		R_R15
#define	R_S0		R_R16
#define	R_S1		R_R17
#define	R_S2		R_R18
#define	R_S3		R_R19
#define	R_S4		R_R20
#define	R_S5		R_R21
#define	R_S6		R_R22
#define	R_S7		R_R23
#define	R_T8		R_R24
#define	R_T9		R_R25
#define	R_K0		R_R26
#define	R_K1		R_R27
#define	R_GP		R_R28
#define	R_SP		R_R29
#define	R_FP		R_R30
#define	R_RA		R_R31

#define	R_R0		0
#define	R_R1		1
#define	R_R2		2
#define	R_R3		3
#define	R_R4		4
#define	R_R5		5
#define	R_R6		6
#define	R_R7		7
#define	R_R8		8
#define	R_R9		9
#define	R_R10		10
#define	R_R11		11
#define	R_R12		12
#define	R_R13		13
#define	R_R14		14
#define	R_R15		15
#define	R_R16		16
#define	R_R17		17
#define	R_R18		18
#define	R_R19		19
#define	R_R20		20
#define	R_R21		21
#define	R_R22		22
#define	R_R23		23
#define	R_R24		24
#define	R_R25		25
#define	R_R26		26
#define	R_R27		27
#define	R_R28		28
#define	R_R29		29
#define	R_R30		30
#define	R_R31		31

#define	R_SR		32
#define	R_MDLO		33
#define	R_MDHI		34
#define	R_BADVADDR	35
#define	R_CAUSE		36
#define	R_EPC		37
#define	R_F0		38
#define R_FCSR		70
#define R_FEIR		71

#define CONTEXT  $4
#define CONFIG   $16
#define STATUS	 $12
#define	C0_SR	 $12		/* status register */
#define	C0_CAUSE $13		/* exception cause */
#define	C0_EPC	 $14		/* exception pc */

#define C1_REVISION     $0
#define C1_STATUS       $31

#if (CPU == MIPS64)
	#define NOP	nop
	#define ADD	dadd
	#define STREG	sd
	#define LDREG	ld
	#define MFCO	dmfc0		/* Only use this op for coprocessor registers that are 64 bit in R4000 architecture */
	#define MTCO	dmtc0		/* Only use this op for coprocessor registers that are 64 bit in R4000 architecture */
	#define ADDU	addu
	#define ADDIU	addiu
	#define R_SZ	8
	#define F_SZ	8
	#define SZ_INT	8
	#define SZ_INT_POW2 3
	#define NREGS	96
	#define STREGC1	sdc1
	#define LDREGC1	ldc1
#elif (CPU == MIPS32) 
	#define NOP	nop
	#define ADD	add
	#define STREG	sw
	#define LDREG	lw
	#define MFCO	mfc0
	#define MTCO	mtc0
	#define ADDU	add
	#define ADDIU	addi
	#define STREGC1	swc1
	#define LDREGC1	lwc1
	#define R_SZ	4
	#define F_SZ	4
	#define SZ_INT	4
	#define SZ_INT_POW2 2
	#define NREGS		81
	#define STREGC1	swc1
	#define LDREGC1	lwc1
#else
	#error "mips assembly: what size registers do I deal with?"
#endif
	
/*
 * Coprocessor 0 register names
 */
#define CP0_INDEX $0
#define CP0_RANDOM $1
#define CP0_ENTRYLO0 $2
#define CP0_ENTRYLO1 $3
#define CP0_CONF $3
#define CP0_CONTEXT $4
#define CP0_PAGEMASK $5
#define CP0_WIRED $6
#define CP0_INFO $7
#define CP0_BADVADDR $8
#define CP0_COUNT $9
#define CP0_ENTRYHI $10
#define CP0_COMPARE $11
#define CP0_STATUS $12
#define CP0_CAUSE $13
#define CP0_EPC $14
#define CP0_PRID $15
#define CP0_EBASE $15 /* 3A中ebase寄存器 */
#define CP0_CONFIG $16
#define CP0_LLADDR $17
#define CP0_WATCHLO $18
#define CP0_WATCHHI $19
#define CP0_XCONTEXT $20
#define CP0_FRAMEMASK $21
#define CP0_DIAGNOSTIC $22
#define CP0_DEBUG $23
#define CP0_DEPC $24
#define CP0_PERFORMANCE $25
#define CP0_ECC $26
#define CP0_CACHEERR $27
#define CP0_TAGLO $28
#define CP0_TAGHI $29
#define CP0_ERROREPC $30
#define CP0_DESAVE $31

#define R_TA_SZ	8
#define TA_REG_ZERO_OFFSET R_ZERO * R_TA_SZ
#define TA_REG_AT_OFFSET R_AT * R_TA_SZ
#define TA_REG_V0_OFFSET R_V0 * R_TA_SZ
#define TA_REG_V1_OFFSET R_V1 * R_TA_SZ
#define TA_REG_A0_OFFSET R_A0 * R_TA_SZ
#define TA_REG_A1_OFFSET R_A1 * R_TA_SZ
#define TA_REG_A2_OFFSET R_A2 * R_TA_SZ
#define TA_REG_A3_OFFSET R_A3 * R_TA_SZ
#define TA_REG_T0_OFFSET R_T0 * R_TA_SZ
#define TA_REG_T1_OFFSET R_T1 * R_TA_SZ
#define TA_REG_T2_OFFSET R_T2 * R_TA_SZ
#define TA_REG_T3_OFFSET R_T3 * R_TA_SZ
#define TA_REG_T4_OFFSET R_T4 * R_TA_SZ
#define TA_REG_T5_OFFSET R_T5 * R_TA_SZ
#define TA_REG_T6_OFFSET R_T6 * R_TA_SZ
#define TA_REG_T7_OFFSET R_T7 * R_TA_SZ

#define TA_REG_S0_OFFSET R_S0 * R_TA_SZ
#define TA_REG_S1_OFFSET R_S1 * R_TA_SZ
#define TA_REG_S2_OFFSET R_S2 * R_TA_SZ
#define TA_REG_S3_OFFSET R_S3 * R_TA_SZ
#define TA_REG_S4_OFFSET R_S4 * R_TA_SZ
#define TA_REG_S5_OFFSET R_S5 * R_TA_SZ
#define TA_REG_S6_OFFSET R_S6 * R_TA_SZ
#define TA_REG_S7_OFFSET R_S7 * R_TA_SZ

#define TA_REG_T8_OFFSET R_T8 * R_TA_SZ
#define TA_REG_T9_OFFSET R_T9 * R_TA_SZ

#define TA_REG_GP_OFFSET R_GP * R_TA_SZ
#define TA_REG_SP_OFFSET R_SP * R_TA_SZ
#define TA_REG_FP_OFFSET R_FP * R_TA_SZ
#define TA_REG_RA_OFFSET R_RA * R_TA_SZ

#define TA_REG_STATUS_OFFSET R_SR * R_TA_SZ
#define TA_REG_EPC_OFFSET R_EPC * R_TA_SZ
#define TA_REG_BAD_OFFSET R_BADVADDR * R_TA_SZ
#define TA_REG_HI_OFFSET R_MDHI * R_TA_SZ
#define TA_REG_LO_OFFSET R_MDLO * R_TA_SZ
#define TA_REG_CAUSE_OFFSET R_CAUSE * R_TA_SZ

 /*  Define macros to handle section beginning and ends.*/
#define BEGIN_CODE .text
#define END_CODE

#define BEGIN_DATA .data
#define END_DATA

#define BEGIN_BSS .bss
#define END_BSS

#define BEGIN_PROGRAM
#define END_PROGRAM  .end

#define CODE32 .code32
#define CODE16 .code16

/**********************************************************************
 *  Following must be tailor for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 **********************************************************************/

#define PUBLIC(sym) .globl SYM (sym)
#define EXTERN(sym) .globl SYM (sym)

#define FRAME(name,frm_reg,offset,ret_reg)      \
        .globl  name;                           \
        .ent    name;                           \
name:;                                          \
        .frame  frm_reg,offset,ret_reg

#define ENDFRAME(name)                          \
        .end name

#define FUNC(func)          func
#define FUNC_LABEL(func)    func:


/* Introduced to abstract assembler idiosyncrasies */
#define FUNC_BEGIN(func)        FUNC_LABEL(func)
#define FUNC_END(func)          .size   FUNC(func), . - FUNC(func)

/*Status Register definitions*/
#define Status	$12
#define Status_CU0	0x10000000
#define Status_CU1  0x20000000
#define Status_CU2  0x40000000
#define Status_CU3  0x80000000
#define Status_RP	0x08000000
#define Status_RE	0x02000000

#define Status_BEV	0x00400000
#define Status_TS	0x00200000
#define Status_SR	0x00100000
#define Status_NMI	0x00080000
#define Status_IM7  0x00008000
#define Status_IM6  0x00004000
#define Status_IM5  0x00002000
#define Status_IM4  0x00001000
#define Status_IM3  0x00000800
#define Status_IM2  0x00000400
#define Status_IM1  0x00000200
#define Status_IM0  0x00000100

#define Status_UM	0x00000010
#define Status_R0	0x00000008
#define Status_ERL	0x00000004
#define Status_EXL	0x00000002
#define Status_IE	0X00000001

#ifdef MIPS64_SUPPORT

#define Status_FR	0x04000000
#define Status_MX   0x01000000
#define Status_PX	0x00800000
#define Status_KX	0x00000080
#define Status_SX	0x00000040
#define Status_UX	0X00000020

#endif

/*Cause Register definitions*/
#define Cause 	$13
#define Cause_BD	0x80000000
#define Cause_CENum	0x70000000

#define Cause_IV	0x00800000
#define Cause_WP	0x00400000
#define Cause_IP0	0x00000100
#define Cause_IP1	0x00000200
#define Cause_IP2	0x00000400
#define Cause_IP3	0x00000800

#define Cause_IP4	0x00001000
#define Cause_IP5	0x00002000
#define Cause_IP6	0x00004000
#define Cause_IP7	0x00008000

#define Cause_RevBits	0x4f1f0083
#define Cause_ExcCode	0x0000007c

/*ExcCode field */
#define ExcCode_Int		0x00
#define ExcCode_Mod 	0x01
#define ExcCode_TLBL 	0x02
#define ExcCode_TLBS 	0x03
#define ExcCode_AdEL 	0x04
#define ExcCode_AdES 	0x05
#define ExcCode_IBE 	0x06
#define ExcCode_DBE 	0x07
#define ExcCode_Sys 	0x08
#define ExcCode_Bp	 	0x09
#define ExcCode_RI	 	0x0a
#define ExcCode_CpU	 	0x0b
#define ExcCode_Ov 		0x0c
#define ExcCode_Tr 		0x0d
#define ExcCode_FPE 	0x0f

#define ExcCode_C2E 	0x12
#define ExcCode_MDMX 	0x16
#define ExcCode_WATCH 	0x17
#define ExcCode_MCheck 	0x18
#define ExcCode_CacheErr 	0x1e

#define EXCEPTION_CODE(exc)	((exc) << 2)

/*Exception Program Counter*/
#define EPC $14
#define Bad $8

#define ErrorEPC $30

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _DB_ASM_H */
