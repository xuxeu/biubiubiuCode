/*
 * dbDbgALib.S
 *
 *  Created on: 2014-10-11
 *      Author: Administrator
 */

/**
 * @file sdaExceptionAsm.S
 * @brief
 *       功能:
 *       <li>提供异常入口</li>
 */

#ifdef _KERNEL_DEBUG_
/*头文件*/
#define _ASMLANGUAGE

#include "dbAsm.h"  /*为了保证和通用中断的定义统一*/
#include "dbReg.h"

/*定义寄存器在DA上下文缓存中的偏移*/
#define TA_REG_PCR_OFFSET  ((32+32*2)*4)   /*通用寄存器+double浮点寄存器*/
#define TA_REG_MSR_OFFSET  (TA_REG_PCR_OFFSET+4)
#define TA_REG_CR_OFFSET   (TA_REG_MSR_OFFSET+4)
#define TA_REG_LR_OFFSET   (TA_REG_CR_OFFSET+4)
#define TA_REG_CTR_OFFSET  (TA_REG_LR_OFFSET+4)
#define TA_REG_XER_OFFSET  (TA_REG_CTR_OFFSET+4)
#define TA_REG_FPSCR_OFFSET (TA_REG_XER_OFFSET+4)
#define TA_REG_GPR_OFFEST(i) (0+i*4)
#define TA_REG_FGPR_OFFSET(i) (32*4+i*8)

/*保存的内存顺序:32个GPR,32个浮点,PCR,MSR,CR,LR,CTR,XER,FPSCR*/
.macro TA_SAVE_FPGR
    /* 使能浮点 */
    mfmsr  r4
    ori    r4, r4, 0x2000
    mtmsr  r4
    isync
    sync

    /* 保存浮点寄存器到本地 */
    stfd   f0,    TA_REG_FGPR_OFFSET(0)(r5)
    stfd   f1,    TA_REG_FGPR_OFFSET(1)(r5)
    stfd   f2,    TA_REG_FGPR_OFFSET(2)(r5)
    stfd   f3,    TA_REG_FGPR_OFFSET(3)(r5)
    stfd   f4,    TA_REG_FGPR_OFFSET(4)(r5)
    stfd   f5,    TA_REG_FGPR_OFFSET(5)(r5)
    stfd   f6,    TA_REG_FGPR_OFFSET(6)(r5)
    stfd   f7,    TA_REG_FGPR_OFFSET(7)(r5)
    stfd   f8,    TA_REG_FGPR_OFFSET(8)(r5)
    stfd   f9,    TA_REG_FGPR_OFFSET(9)(r5)
    stfd   f10,    TA_REG_FGPR_OFFSET(10)(r5)
    stfd   f11,    TA_REG_FGPR_OFFSET(11)(r5)
    stfd   f12,    TA_REG_FGPR_OFFSET(12)(r5)
    stfd   f13,    TA_REG_FGPR_OFFSET(13)(r5)
    stfd   f14,    TA_REG_FGPR_OFFSET(14)(r5)
    stfd   f15,    TA_REG_FGPR_OFFSET(15)(r5)
    stfd   f16,    TA_REG_FGPR_OFFSET(16)(r5)
    stfd   f17,    TA_REG_FGPR_OFFSET(17)(r5)
    stfd   f18,    TA_REG_FGPR_OFFSET(18)(r5)
    stfd   f19,    TA_REG_FGPR_OFFSET(19)(r5)
    stfd   f20,    TA_REG_FGPR_OFFSET(20)(r5)
    stfd   f21,    TA_REG_FGPR_OFFSET(21)(r5)
    stfd   f22,    TA_REG_FGPR_OFFSET(22)(r5)
    stfd   f23,    TA_REG_FGPR_OFFSET(23)(r5)
    stfd   f24,    TA_REG_FGPR_OFFSET(24)(r5)
    stfd   f25,    TA_REG_FGPR_OFFSET(25)(r5)
    stfd   f26,    TA_REG_FGPR_OFFSET(26)(r5)
    stfd   f27,    TA_REG_FGPR_OFFSET(27)(r5)
    stfd   f28,    TA_REG_FGPR_OFFSET(28)(r5)
    stfd   f29,    TA_REG_FGPR_OFFSET(29)(r5)
    stfd   f30,    TA_REG_FGPR_OFFSET(30)(r5)
    stfd   f31,    TA_REG_FGPR_OFFSET(31)(r5)

    /* 保存浮点状态与控制寄存器 */
    mffs   f0
    /*注意：FPSCR虽然只占4字节，但是stfd指令操作的是8字节，所以需要为FPSCR保留8字节的空间*/
    stfd   f0,     TA_REG_FPSCR_OFFSET(r5)
    lfd    f0,     TA_REG_FGPR_OFFSET(0)(r5)
    isync
    sync
.endm

.macro TA_RESTORE_FGPR
    /* 恢复浮点状态与控制寄存器 */
    lfd    f0,    TA_REG_FPSCR_OFFSET(r1)
    mtfsf  0xff,  f0

    /* 把本地保存的浮点寄存器恢复到CPU */
    lfd    f0,    TA_REG_FGPR_OFFSET(0)(r1)
    lfd    f1,    TA_REG_FGPR_OFFSET(1)(r1)
    lfd    f2,    TA_REG_FGPR_OFFSET(2)(r1)
    lfd    f3,    TA_REG_FGPR_OFFSET(3)(r1)
    lfd    f4,    TA_REG_FGPR_OFFSET(4)(r1)
    lfd    f5,    TA_REG_FGPR_OFFSET(5)(r1)
    lfd    f6,    TA_REG_FGPR_OFFSET(6)(r1)
    lfd    f7,    TA_REG_FGPR_OFFSET(7)(r1)
    lfd    f8,    TA_REG_FGPR_OFFSET(8)(r1)
    lfd    f9,    TA_REG_FGPR_OFFSET(9)(r1)
    lfd    f10,    TA_REG_FGPR_OFFSET(10)(r1)
    lfd    f11,    TA_REG_FGPR_OFFSET(11)(r1)
    lfd    f12,    TA_REG_FGPR_OFFSET(12)(r1)
    lfd    f13,    TA_REG_FGPR_OFFSET(13)(r1)
    lfd    f14,    TA_REG_FGPR_OFFSET(14)(r1)
    lfd    f15,    TA_REG_FGPR_OFFSET(15)(r1)
    lfd    f16,    TA_REG_FGPR_OFFSET(16)(r1)
    lfd    f17,    TA_REG_FGPR_OFFSET(17)(r1)
    lfd    f18,    TA_REG_FGPR_OFFSET(18)(r1)
    lfd    f19,    TA_REG_FGPR_OFFSET(19)(r1)
    lfd    f20,    TA_REG_FGPR_OFFSET(20)(r1)
    lfd    f21,    TA_REG_FGPR_OFFSET(21)(r1)
    lfd    f22,    TA_REG_FGPR_OFFSET(22)(r1)
    lfd    f23,    TA_REG_FGPR_OFFSET(23)(r1)
    lfd    f24,    TA_REG_FGPR_OFFSET(24)(r1)
    lfd    f25,    TA_REG_FGPR_OFFSET(25)(r1)
    lfd    f26,    TA_REG_FGPR_OFFSET(26)(r1)
    lfd    f27,    TA_REG_FGPR_OFFSET(27)(r1)
    lfd    f28,    TA_REG_FGPR_OFFSET(28)(r1)
    lfd    f29,    TA_REG_FGPR_OFFSET(29)(r1)
    lfd    f30,    TA_REG_FGPR_OFFSET(30)(r1)
    lfd    f31,    TA_REG_FGPR_OFFSET(31)(r1)

    isync
    sync
.endm


/* 保存现场 */
.macro SAVE_REGISTERS
    lwi   r5, taSDASmpExpContext

    //TA_SAVE_FPGR

    /* 将通用中断处理保存的寄存器保存 */
    lwz   r4,    SRR0_OFFSET(r1)
    stw   r4,    TA_REG_PCR_OFFSET(r5)         /* pc */
    lwz   r4,    SRR1_OFFSET(r1)
    stw   r4,    TA_REG_MSR_OFFSET(r5)         /* msr */
    lwz   r4,    CR_OFFSET(r1)
    stw   r4,    TA_REG_CR_OFFSET(r5)          /* cr */
    lwz   r4,    CTR_OFFSET(r1)
    stw   r4,    TA_REG_CTR_OFFSET(r5)         /* ctr */
    lwz   r4,    LR_OFFSET(r1)
    stw   r4,    TA_REG_LR_OFFSET(r5)          /* lr */
    lwz   r4,    XER_OFFSET(r1)
    stw   r4,    TA_REG_XER_OFFSET(r5)         /* xer */
    lwz   r4,    GPR0_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(0)(r5)
    lwz   r4,    GPR1_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(1)(r5)
    lwz   r4,    GPR2_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(2)(r5)
    lwz   r4,    GPR3_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(3)(r5)
    lwz   r4,    GPR4_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(4)(r5)
    lwz   r4,    GPR5_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(5)(r5)
    lwz   r4,    GPR6_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(6)(r5)
    lwz   r4,    GPR7_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(7)(r5)
    lwz   r4,    GPR8_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(8)(r5)
    lwz   r4,    GPR9_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(9)(r5)
    lwz   r4,    GPR10_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(10)(r5)
    lwz   r4,    GPR11_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(11)(r5)
    lwz   r4,    GPR12_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(12)(r5)
    lwz   r4,    GPR13_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(13)(r5)
    lwz   r4,    GPR14_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(14)(r5)
    lwz   r4,    GPR15_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(15)(r5)
    lwz   r4,    GPR16_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(16)(r5)
    lwz   r4,    GPR17_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(17)(r5)
    lwz   r4,    GPR18_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(18)(r5)
    lwz   r4,    GPR19_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(19)(r5)
    lwz   r4,    GPR20_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(20)(r5)
    lwz   r4,    GPR21_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(21)(r5)
    lwz   r4,    GPR22_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(22)(r5)
    lwz   r4,    GPR23_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(23)(r5)
    lwz   r4,    GPR24_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(24)(r5)
    lwz   r4,    GPR25_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(25)(r5)
    lwz   r4,    GPR26_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(26)(r5)
    lwz   r4,    GPR27_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(27)(r5)
    lwz   r4,    GPR28_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(28)(r5)
    lwz   r4,    GPR29_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(29)(r5)
    lwz   r4,    GPR30_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(30)(r5)
    lwz   r4,    GPR31_OFFSET(r1)
    stw   r4,    TA_REG_GPR_OFFEST(31)(r5)

    lwz      r28,   EXCEPTION_VECTOR_OFFSET(r1)
    isync
    sync
.endm


/* 恢复上下文 */
.macro RESTORE_REGISTERS

    lwi   r1, taSDASmpExpContext

    /* 恢复浮点上下文 */
    //TA_RESTORE_FGPR

    lwz   r0,    TA_REG_GPR_OFFEST(0)(r1)   /* GPR0 */
    lwz   r2,    TA_REG_GPR_OFFEST(2)(r1)   /* GPR2 */

    lmw  r5,TA_REG_GPR_OFFEST(5)(r1)    /* GPR5~GPR31 */
    isync
    sync


    lwz   r4,TA_REG_CTR_OFFSET(r1)      /*ctr*/
    mtctr r4
    lwz   r4,TA_REG_LR_OFFSET(r1)       /*lr*/
    mtlr r4
    lwz   r4,TA_REG_XER_OFFSET(r1)      /*xer*/
    mtxer r4
    lwz   r4,TA_REG_CR_OFFSET(r1)       /*cr*/
    mtcr r4

    lwz     r4, TA_REG_GPR_OFFEST(4)(r1)    /* GPR4 */

    /*在设置srr0、srr1前，需要关中断、关MMU进行 */
    li      r3, 0x1002
    mtmsr   r3
    sync
    isync

    lwz     r3, TA_REG_PCR_OFFSET(r1)       /* SRR0 */
    mtsrr0  r3

    lwz     r3, TA_REG_MSR_OFFSET(r1)    /* SRR1 */
    mtsrr1  r3

    lwz     r3, TA_REG_GPR_OFFEST(3)(r1)     /* GPR3 */

    lwz     r1, TA_REG_GPR_OFFEST(1)(r1)       /* GPR1 */

    sync
    isync
.endm


/*实现*/

/*
 * @brief:
 *      异常入口
 * @return:
 *      None
 */
.global taBpExceptionStub
taBpExceptionStub:

    /* 关cpu中断 */
    mfmsr   r5
    rlwinm  r5,r5,0,17,15 /* 清除MSR[EE] */
    mtmsr   r5
    isync

    /* 保存上下文到taSDASmpExpContext */
    SAVE_REGISTERS

    /* 处理异常 */
    subi r1,r1,8
    mr   r3,r28
    bl   taAachExceptionHandler
    addi r1,r1,8

    /* 从taSDASmpExpContext恢复上下文 */
    RESTORE_REGISTERS

    rfi
#endif

.globl taTrap
taTrap:
	trap
	blr
	nop
.end taTrap
