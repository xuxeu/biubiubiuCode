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
 *             <li>ARM调试上下文定义</li>
 */
#ifndef _DB_CONTEXT_H
#define _DB_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"

/************************宏定义********************************/

/* 断点指令 */
#define DB_ARCH_BREAK_INST  0xe120007c  //e120007c  //ef9f0001

/* 断点指令长度 */
#define DB_ARCH_BREAK_INST_LEN  4

 /* PS寄存器的TF位,用于调试单步位使能 */
#define DB_ARM_EFLAGS_TF  0x100

/* 状态寄存器中的单步位 */
#define DB_ARCH_PS_TRAP_BIT DB_ARM_EFLAGS_TF

#ifdef CONFIG_TA_LP64
/* 普通64位寄存器每个元素长度为8个字节 */
#define DB_REGISTER_UNIT_LEN  (8)
#else
/* 普通32位寄存器每个元素长度为4个字节 */
#define DB_REGISTER_UNIT_LEN  (4)

#endif

 /* 将内存数据转换为字符流长度扩展2倍  */
#define DB_MEM_TO_CHAR_LEN    (2)

/* 寄存器长度 */
#define DB_ARM_REGISTER_SUM_LEN (DB_ARM_REGISTER_SUM * DB_REGISTER_UNIT_LEN)

#define DB_ARCH_REGISTER_SUM_SIZE (DB_ARM_REGISTER_SUM_LEN * DB_MEM_TO_CHAR_LEN)

/* 寄存器的长度 */
#define DB_REGISTER_SUM_SIZE DB_ARM_REGISTER_SUM_LEN

#ifdef CONFIG_TA_LP64

#define swap(x) ((((ulong_t)(x) & 0xff00000000000000) >> 56)|\
                (((ulong_t)(x) & 0x00ff000000000000) >>  40)|\
				(((ulong_t)(x) & 0x0000ff0000000000) >> 24) |\
                (((ulong_t)(x) & 0x000000ff00000000) >>  8) |\
                (((ulong_t)(x) & 0x00000000ff000000) <<  8) |\
                (((ulong_t)(x) & 0x0000000000ff0000) << 24) |\
                (((ulong_t)(x) & 0x000000000000ff00) << 40) |\
                (((ulong_t)(x) & 0x00000000000000ff) << 56))
#else

#define swap(x) ((((UINT32)(x) & 0xff000000) >> 24)|\
                (((UINT32)(x) & 0x00ff0000) >>  8) |\
                (((UINT32)(x) & 0x0000ff00)<<8)    |\
                (((UINT32)(x) & 0x000000ff) << 24))
#endif

/************************类型定义******************************/





/* X86的寄存器名字定义 */
typedef enum  T_DB_RegisterName
{
    R0 = 0,
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R4 = 4,
    R5 = 5,
    R6 = 6,
    R7 = 7,
    R8 = 8,
    R9 = 9,
    R10 = 10,
    R11 = 11,
    R12 = 12,
    R13 = 13,
    R14 = 14,
    PC = 15,
    F0 = 16,
    F1 = 17,
    F2 = 18,
    F3 = 19,
    F4 = 20,
    F5 = 21,
    F6 = 22,
    F7 = 23,
    FPS= 24,
    CPSR = 25,
    DB_ARM_REGISTER_SUM = 26,  /*寄存器数量*/
    FP = R11,
    SP = R13,
    LR = R14

} DB_RegisterName;


/* ARM的寄存器上下文定义*/
typedef struct
{
    ulong_t  registers[DB_ARM_REGISTER_SUM];
    ulong_t  vector;
} T_DB_ExceptionContext;



/************************接口声明******************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DB_CONTEXT_H */
