/*
* 变更历史：
* 2013-01-15  周英豪      增加浮点寄存器保存到栈上的偏移宏。
* 2012-04-21  邱学强      增加异常处理过程开MMU前使用的临时异常栈相关宏及
*                         异常处理过程中对MSR位进行修改所使用的宏。(BUG6549)
* 2010-08-30  尹立孟      创建该文件。
*/


/*
* @file 	asmPpc.h
* @brief
*    功能：
*      <li>定义PPC汇编相关的宏。</li>
*/

#ifndef _ASMPPC_H
#define _ASMPPC_H


#ifdef __cplusplus
 extern "C" {
#endif

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
#define FUNC_BEGIN(func)        FUNC_LABEL(func)
#define FUNC_END(func)          .size   FUNC(func), . - FUNC(func)

#define MINIMUM_EXCHANDLE_STACK_SIZE 128

#define ISR_VECTOR_OFFSET 0x50
#define ISR_FRAME_SIZE  0x54
#define ISR_FLOAT_FRAME_SIZE 0x108

#define SC_LR_OFFSET  0x8
#define SC_RETVALUE_OFFSET  0xC
#define SC_GPR1_OFFSET  0x10
#define SC_FRAME_SIZE  0x14

/* 处理MMU异常的时候，保护上下文需要的栈大小，使用PPC保留的vector 0xB00，栈地址为0xC00-44 */
#define MMU_STACK_SIZE        (44)
#define __mmu_stack__    (0xBD4)

/* MSR寄存器各种位设置的组合 */
#define MSR_BIT_IR_DR_RI                    0x0032
#define MSR_BIT_FP                          0x2000
#define MSR_BIT_EE_IR_DR_RI                 0x8032
#define MSR_BIT_PR_SE_BE                    0x4600
#define MSR_BIT_EE_PR_SE_BE                 0xc600
#define MSR_BIT_RI                          0x0002

/* fpr0-fpr13 fpscr 共计15个寄存器 每个8字节 */
#define  ISR_VOLATILE_FPRS_SIZE     (8*(14+ 1) + 8)

#define SYM(x) x
#define PUBLIC(sym) .global SYM (sym)

.macro	lwi	reg,val
lis	\reg,\val@H
ori	\reg,\reg,\val@L
.endm

#ifdef __cplusplus
}
#endif

#define SYS_INIT_STACK_SIZE  (8192)

/*宏定义*/
/*寄存器的定义*/
#define r0  0
#define r1  1
#define r2  2
#define r3  3
#define r4  4
#define r5  5
#define r6  6
#define r7  7
#define r8  8
#define r9  9
#define r10  10
#define r11  11
#define r12  12
#define r13  13
#define r14  14
#define r15  15
#define r16  16
#define r17  17
#define r18  18
#define r19  19
#define r20  20
#define r21  21
#define r22  22
#define r23  23
#define r24  24
#define r25  25
#define r26  26
#define r27  27
#define r28  28
#define r29  29
#define r30  30
#define r31  31
#define f0  0
#define f1  1
#define f2  2
#define f3  3
#define f4  4
#define f5  5
#define f6  6
#define f7  7
#define f8  8
#define f9  9
#define f10  10
#define f11  11
#define f12  12
#define f13  13
#define f14  14
#define f15  15
#define f16  16
#define f17  17
#define f18  18
#define f19  19
#define f20  20
#define f21  21
#define f22  22
#define f23  23
#define f24  24
#define f25  25
#define f26  26
#define f27  27
#define f28  28
#define f29  29
#define f30  30
#define f31  31

#define SPRG0    272
#define SPRG1    273
#define SPRG2    274
#define SPRG3    275

#define DAR 	19
#define DSISR 	18

#define srr0 	26
#define srr1 	27

/*上下文偏移宏定义*/
#define SRR0_OFFSET  0x0
#define SRR1_OFFSET  0x4
#define CR_OFFSET  0x8
#define CTR_OFFSET  0xC
#define LR_OFFSET  0x10
#define XER_OFFSET  0x14

#define GPR0_OFFSET  0x18
#define GPR1_OFFSET  0x1C
#define GPR2_OFFSET  0x20
#define GPR3_OFFSET  0x24
#define GPR4_OFFSET  0x28
#define GPR5_OFFSET  0x2C
#define GPR6_OFFSET  0x30
#define GPR7_OFFSET  0x34
#define GPR8_OFFSET  0x38
#define GPR9_OFFSET  0x3C
#define GPR10_OFFSET  0x40
#define GPR11_OFFSET  0x44
#define GPR12_OFFSET  0x48
#define GPR13_OFFSET  0x4C
#define GPR14_OFFSET  0x50
#define GPR15_OFFSET  0x54
#define GPR16_OFFSET  0x58
#define GPR17_OFFSET  0x5C
#define GPR18_OFFSET  0x60
#define GPR19_OFFSET  0x64
#define GPR20_OFFSET  0x68
#define GPR21_OFFSET  0x6C
#define GPR22_OFFSET  0x70
#define GPR23_OFFSET  0x74
#define GPR24_OFFSET  0x78
#define GPR25_OFFSET  0x7C
#define GPR26_OFFSET  0x80
#define GPR27_OFFSET  0x84
#define GPR28_OFFSET  0x88
#define GPR29_OFFSET  0x8C
#define GPR30_OFFSET  0x90
#define GPR31_OFFSET  0x94

#define DAR_OFFSET  0x98
#define DSISR_OFFSET  0x9C
#define FPSCR_OFFSET  0xA0

#define EXCEPTION_VECTOR_OFFSET  0xA8

#define p0	r3	/* argument register, volatile */
#define p1	r4	/* argument register, volatile */
#define p2	r5	/* argument register, volatile */
#define p3	r6	/* argument register, volatile */
#define p4	r7	/* argument register, volatile */
#define p5	r8	/* argument register, volatile */
#define p6	r9	/* argument register, volatile */
#define p7	r10	/* argument register, volatile */
#define glr0	r0	/* prologs(PO,EABI), epilogs, glink routines(EABI) /
			 * language specific purpose(SVR4), volatile */
#define glr1	r11	/* prologs, epilogs, as Pascal environment pointer(EABI)
			 * language specific purpose (SVR4)
			 * calls by pointer, as Pascal environment(PO),
			 * volatile */
#define glr2	r12	/* prologs, epilogs, glink routines, calls by
			 * pointer(EABI), language specific purpose (SVR4),
			 * glue code, exception handling (PO), volatile */
#define retval0	r3	/* return register 0, volatile */
#define retval1	r4	/* return register 1, volatile */


/* non-volatile and dedicated registers saved across subroutine calls */

#define	sp	r1	/* stack pointer, dedicated */

#define t0	r14	/* temporary registers, non-volatile */
#define t1	r15	/* temporary registers, non-volatile */
#define t2	r16	/* temporary registers, non-volatile */
#define t3	r17	/* temporary registers, non-volatile */
#define t4	r18	/* temporary registers, non-volatile */
#define t5	r19	/* temporary registers, non-volatile */
#define t6	r20	/* temporary registers, non-volatile */
#define t7	r21	/* temporary registers, non-volatile */
#define t8	r22	/* temporary registers, non-volatile */
#define t9	r23	/* temporary registers, non-volatile */
#define t10	r24	/* temporary registers, non-volatile */
#define t11	r25	/* temporary registers, non-volatile */
#define t12	r26	/* temporary registers, non-volatile */
#define t13	r27	/* temporary registers, non-volatile */
#define t14	r28	/* temporary registers, non-volatile */
#define t15	r29	/* temporary registers, non-volatile */
#define t16	r30	/* temporary registers, non-volatile */
#define t17	r31	/* temporary registers, non-volatile */

/* 浮点寄存器偏移量，以便于浮点上下文保存的时候计算栈中的偏移*/
#define FP_FPSCR 0
#define FP_0  (FP_FPSCR + 8)
#define FP_1  (FP_0 + 8)
#define FP_2  (FP_1 + 8)
#define FP_3  (FP_2 + 8)
#define FP_4  (FP_3 + 8)
#define FP_5  (FP_4 + 8)
#define FP_6  (FP_5 + 8)
#define FP_7  (FP_6 + 8)
#define FP_8  (FP_7 + 8)
#define FP_9  (FP_8+ 8)
#define FP_10  (FP_9 + 8)
#define FP_11  (FP_10 + 8)
#define FP_12  (FP_11 + 8)
#define FP_13  (FP_12 + 8)
#define FP_14  (FP_13 + 8)
#define FP_15  (FP_14 + 8)
#define FP_16  (FP_15 + 8)
#define FP_17  (FP_16 + 8)
#define FP_18  (FP_17 + 8)
#define FP_19  (FP_18 + 8)
#define FP_20  (FP_19 + 8)
#define FP_21  (FP_20 + 8)
#define FP_22  (FP_21 + 8)
#define FP_23  (FP_22 + 8)
#define FP_24  (FP_23 + 8)
#define FP_25  (FP_24 + 8)
#define FP_26  (FP_25 + 8)
#define FP_27  (FP_26 + 8)
#define FP_28  (FP_27 + 8)
#define FP_29  (FP_28 + 8)
#define FP_30  (FP_29 + 8)
#define FP_31  (FP_30 + 8)

#define MINIMUM_EXCHANDLE_STACK_SIZE 128

#define ISR_VECTOR_OFFSET 0x50
#define ISR_FRAME_SIZE  0x54
#define ISR_FLOAT_FRAME_SIZE 0x108

#define SC_LR_OFFSET  0x8
#define SC_RETVALUE_OFFSET  0xC
#define SC_GPR1_OFFSET  0x10
#define SC_FRAME_SIZE  0x14

/* 处理MMU异常的时候，保护上下文需要的栈大小，使用PPC保留的vector 0xB00，栈地址为0xC00-44 */
#define MMU_STACK_SIZE        (44)
#define __mmu_stack__    (0xBD4)

/* MSR寄存器各种位设置的组合 */
#define MSR_BIT_IR_DR_RI                    0x0032
#define MSR_BIT_FP                          0x2000
#define MSR_BIT_EE_IR_DR_RI                 0x8032
#define MSR_BIT_PR_SE_BE                    0x4600
#define MSR_BIT_EE_PR_SE_BE                 0xc600
#define MSR_BIT_RI                          0x0002

/* fpr0-fpr13 fpscr 共计15个寄存器 每个8字节 */
#define  ISR_VOLATILE_FPRS_SIZE     (8*(14+ 1) + 8)
#endif


