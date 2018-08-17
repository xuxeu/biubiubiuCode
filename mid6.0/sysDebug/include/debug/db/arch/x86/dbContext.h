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
 *             <li>x86调试上下文定义</li>
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
#define DB_ARCH_BREAK_INST  0xcc

/* 断点指令长度 */
#define DB_ARCH_BREAK_INST_LEN  1

 /* PS寄存器的TF位,用于调试单步位使能 */
#define DB_X86_EFLAGS_TF  0x100

/* 状态寄存器中的单步位 */
#define DB_ARCH_PS_TRAP_BIT DB_X86_EFLAGS_TF

/* 普通32位寄存器每个元素长度为4个字节 */
#define DB_REGISTER_UNIT_LEN  (4)

 /* 将内存数据转换为字符流长度扩展2倍  */
#define DB_MEM_TO_CHAR_LEN    (2)

/* 寄存器长度 */
#define DB_X86_REGISTER_SUM_LEN (DB_X86_REGISTER_SUM * DB_REGISTER_UNIT_LEN)

#define DB_ARCH_REGISTER_SUM_SIZE (DB_X86_REGISTER_SUM_LEN * DB_MEM_TO_CHAR_LEN)

/* 寄存器的长度 */
#define DB_REGISTER_SUM_SIZE DB_X86_REGISTER_SUM_LEN

#define swap(x) ((((UINT32)(x) & 0xff000000) >> 24)|\
                (((UINT32)(x) & 0x00ff0000) >>  8) |\
                (((UINT32)(x) & 0x0000ff00)<<8)    |\
                (((UINT32)(x) & 0x000000ff) << 24))

/************************类型定义******************************/

/* 引用风河的头文件时，无法编译通过，风河也定义了EAX等宏，导致重复定义，只取消了当前与风河冲突的定义 */
#ifdef EAX
	#undef EAX
	#undef ECX
	#undef EDX
	#undef EBX
	#undef ESP
	#undef EBP
	#undef ESI
	#undef EDI
	#undef EFLAGS
	#undef CS
	#undef SS
	#undef DS
	#undef ES
	#undef FS
	#undef GS
#endif

/* X86的寄存器名字定义 */
typedef enum  T_DB_RegisterName
{
    EAX = 0,
    ECX = 1,
    EDX = 2,
    EBX = 3,
    ESP = 4,
    EBP = 5,
    ESI = 6,
    EDI = 7,
    EIP = 8,
    EFLAGS = 9,
    CS = 10,
    SS = 11,
    DS = 12,
    ES = 13,
    FS = 14,
    GS = 15,
    DB_X86_REGISTER_SUM = 16,/*寄存器数量*/

    SP=ESP,
    PC=EIP,
    PS=EFLAGS
} DB_RegisterName;

/* X86的寄存器上下文定义*/
typedef struct
{
    UINT32  registers[DB_X86_REGISTER_SUM];
    UINT32  vector;
} T_DB_ExceptionContext;

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DB_CONTEXT_H */
