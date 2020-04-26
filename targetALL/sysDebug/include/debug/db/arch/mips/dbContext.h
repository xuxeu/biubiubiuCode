/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:dbContext.h
 *@brief:
 *             <li>MIPS调试上下文定义</li>
 */
#ifndef _DB_CONTEXT_H
#define _DB_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"

/************************宏定义********************************/

/* 硬件数据断点异常 */
#define GENERAL_HBP_INT       (27)

/* 软件断点异常  */
#define GENERAL_TRAP_INT       (3)

/* 断点指令 */
#define DB_ARCH_BREAK_INST     0x0000000d

/* 断点指令长度 */
#define DB_ARCH_BREAK_INST_LEN  4

/* 普通64位寄存器每个元素长度为4个字节 */
#define DB_REGISTER_UNIT_LEN   8

/* 将内存数据转换为字符流长度扩展2倍  */
#define DB_MEM_TO_CHAR_LEN     2

/* 普通64位浮点寄存器每个元素长度为8个字节 */
#define DB_FLOAT_REGISTER_UNIT_LEN 8

/* 寄存器个数 */
#define DB_MIPS_REGISTER_SIZE (REGISTER_SUM)

#define DB_REGISTER_SIZE  (DB_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

#define DB_FLOAT_REGISTER_SIZE  (DB_FLOAT_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

#define DB_MIPS_REGISTER_SUM_LEN (DB_MIPS_REGISTER_SIZE * DB_REGISTER_UNIT_LEN)

#define DB_ARCH_REGISTER_SUM_SIZE (DB_MIPS_REGISTER_SUM_LEN * DB_MEM_TO_CHAR_LEN)

/* 寄存器的长度 */
#define DB_REGISTER_SUM_SIZE DB_MIPS_REGISTER_SUM_LEN

/* 状态寄存器中的中断位 */
#define DB_ARCH_PS_INT_BIT DB_MIPS_MSR_IE

#define swap64(x) (\
    (((UINT64)(x)  & (UINT64)0xff00000000000000) >> 56)  \
    |(((UINT64)(x) & (UINT64)0x00ff000000000000) >> 40)  \
    |(((UINT64)(x) & (UINT64)0x0000ff0000000000) >> 24)  \
    |(((UINT64)(x) & (UINT64)0x000000ff00000000) >> 8)  \
    |(((UINT64)(x) & (UINT64)0x00000000ff000000) << 8)  \
    |(((UINT64)(x) & (UINT64)0x0000000000ff0000) <<  24)  \
    | (((UINT64)(x) & (UINT64)0x000000000000ff00)<<40) \
    |(((UINT64)(x)  & (UINT64)0x00000000000000ff) << 56))

/************************类型定义******************************/
#ifdef SR
#undef SR
#endif

/* MIPS的寄存器名字定义 */
enum  T_DB_RegisterName
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
	GPR10 = 10 ,
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
	GPR29 =29,
	GPR30 = 30,
	GPR31 = 31,
	GPR_SUM = 32,

	SR = 0,
	LO = 1,
	HI = 2,
	BAD = 3,
	CAUSE = 4,
	EPC = 5,
	SPR_SUM = 6,

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
	FGPR10 = 10 ,
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
	FSR=0,
	FIR = 1,
	FPSR = 2,
	//REGISTER_SUM = GPR_SUM + FGPR_SUM + SPR_SUM + FPSR,
	REGISTER_SUM = GPR_SUM + SPR_SUM,
	STATUSREG = GPR_SUM  + SR,
	WATCHLOREG = GPR_SUM + LO,
	WATCHHIREG = GPR_SUM + HI,
	BADVADDRREG = GPR_SUM + BAD,
	CAUSEREG = GPR_SUM + CAUSE,
	PC = GPR_SUM + EPC,
	SP = GPR30,
	PLR = GPR31,
}DB_RegisterName;

/* 异常保存MIPS的寄存器上下文定义*/
typedef struct T_DB_ExceptionContext
{
    /*保存的内存顺序:32个GPR,32个浮点,SR,LO,HI,BAD,CAUSE,EPC,FSR,FIR*/
    UINT64 registers[REGISTER_SUM];

    /* 异常向量号 */
    UINT64  vector;
}T_DB_ExceptionContext;

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DB_CONTEXT_H */
