/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-28         黄斌，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:dbContext.h
 *@brief:
 *             <li>调试上下文定义</li>
 */
#ifndef _DB_CONTEXT_H
#define _DB_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头文件********************************/
#include "taTypes.h"

/************************宏定义********************************/

/* 寄存器大小 */
#define DB_PPC_REGISTER_SIZE (GPR_SUM + FGPR_SUM*2 + SPR_SUM)

/* 注意：FPSCR虽然只占4字节，但是stfd指令操作的是8字节，所以需要为FPSCR保留8字节的空间*/
#define DB_PPC_FPSCR_SIZE	(2)

#define DB_ARCH_BREAK_INST  0x7d821008
#define DB_ARCH_BREAK_INST_LEN 4
#define DB_REGISTER_UNIT_LEN   4     /* 普通32位寄存器每个元素长度为4个字节 */
#define DB_MEM_TO_CHAR_LEN     2     /* 将内存数据转换为字符流长度扩展2倍  */
#define DB_FLOAT_REGISTER_UNIT_LEN 8 /* 普通64位寄存器每个元素长度为8个字节 */

#define DB_PPC_MSR_SE    0x400      /* MSR寄存器的SE位,用于单步位使能 */
#define DB_PPC_MSR_EE    0x8000     /* MSR寄存器的EE位,用于外部中断的使能 */
#define DB_PPC_MSR_IR    0x20       /* PPC MSR寄存器的IR位 */
#define DB_PPC_MSR_DR    0x10       /* PPC MSR寄存器的DR位 */

/* 通用寄存器长度 */
#define DB_REGISTER_SIZE  (DB_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

/* 浮点寄存器长度 */
#define DB_FLOAT_REGISTER_SIZE  (DB_FLOAT_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

/* 状态寄存器中的单步位 */
#define DB_ARCH_PS_TRAP_BIT DB_PPC_MSR_SE

/* 状态寄存器中的中断位 */
#define DB_ARCH_PS_INT_BIT DB_PPC_MSR_EE

/* GDB读取的寄存器长度，DB_PPC_REGISTER_SIZE为异常上下文的长度 */
#define DB_PPC_REGISTER_SUM_LEN (DB_PPC_REGISTER_SIZE * DB_REGISTER_UNIT_LEN)

/* 扩展后寄存器的长度 */
#define DB_PPC_REGISTER_SUM_SIZE (DB_PPC_REGISTER_SUM_LEN * DB_MEM_TO_CHAR_LEN)

#define DB_REGISTER_SUM_SIZE DB_PPC_REGISTER_SUM_LEN

/************************类型定义******************************/
/* PPC的寄存器名字定义 */
typedef enum  T_DB_RegisterName
{
    GPR0 = 0,
    GPR1 = 1,
    GPR2 = 2,
    GPR3 = 3,
    GPR4 = 4,
    GPR5 = 5,
    GPR6 = 6,
    GPR7 = 7,
    GPR8 = 8,
    GPR9 = 9,
    GPR10 = 10,
    GPR11 = 11,
    GPR12 = 12,
    GPR13 = 13,
    GPR14 = 14,
    GPR15 = 15,
    GPR16 = 16,
    GPR17 = 17,
    GPR18 = 18,
    GPR19 = 19,
    GPR20 = 20,
    GPR21 = 21,
    GPR22 = 22,
    GPR23 = 23,
    GPR24 = 24,
    GPR25 = 25,
    GPR26 = 26,
    GPR27 = 27,
    GPR28 = 28,
    GPR29 = 29,
    GPR30 = 30,
    GPR31 = 31,
    GPR_SUM = 32,
    FGPR0 = 0,
    FGPR1 = 1,
    FGPR2 = 2,
    FGPR3 = 3,
    FGPR4 = 4,
    FGPR5 = 5,
    FGPR6 = 6,
    FGPR7 = 7,
    FGPR8 = 8,
    FGPR9 = 9,
    FGPR10 = 10,
    FGPR11 = 11,
    FGPR12 = 12,
    FGPR13 = 13,
    FGPR14 = 14,
    FGPR15 = 15,
    FGPR16 = 16,
    FGPR17 = 17,
    FGPR18 = 18,
    FGPR19 = 19,
    FGPR20 = 20,
    FGPR21 = 21,
    FGPR22 = 22,
    FGPR23 = 23,
    FGPR24 = 24,
    FGPR25 = 25,
    FGPR26 = 26,
    FGPR27 = 27,
    FGPR28 = 28,
    FGPR29 = 29,
    FGPR30 = 30,
    FGPR31 = 31,
    FGPR_SUM = 32,
    PCR = 0,
    MSR = 1,
    CR = 2,
    LR = 3,
    CTR = 4,
    XER = 5,
    SPR_SUM = 6,
    REGISTER_SUM = GPR_SUM + FGPR_SUM + SPR_SUM,
    SP = GPR1,
    PC = GPR_SUM + FGPR_SUM*2 + PCR,
    PS = GPR_SUM + FGPR_SUM*2 + MSR,
    PCR1 = GPR_SUM + FGPR_SUM*2 + CR,
    PLR = GPR_SUM + FGPR_SUM*2 + LR,
    PCTR = GPR_SUM + FGPR_SUM*2 + CTR,
    PXER = GPR_SUM + FGPR_SUM*2 + XER
} DB_RegisterName;


/* 异常保存PPC的寄存器上下文定义*/
typedef struct T_DB_ExceptionContext
{
    /* 保存的内存顺序:32个GPR,32个浮点,PCR,MSR,CR,LR,CTR,XER,FPSCR */
    UINT32 registers[DB_PPC_REGISTER_SIZE + DB_PPC_FPSCR_SIZE];

    /* 异常向量号 */
    UINT32  vector;
} DB_ExceptionContext;

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_DB_CONTEXT_H */
