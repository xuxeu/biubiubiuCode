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
 *	    <li>提供x86汇编相关的宏定义</li>
 */
#ifndef _DB_ASM_H
#define _DB_ASM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
/************************头文件********************************/

/************************宏定义********************************/

#define CONFIG_INTERRUPT_NUMBER_OF_VECTORS (256)
#define GDT_ENTRYS  80   /*最多可以支持73的CPU*/
#define GDT_SIZE   (GDT_ENTRYS*8)                /* GDT size  (7 entries)*/

#define IDT_ENTRYS   CONFIG_INTERRUPT_NUMBER_OF_VECTORS
#define IDT_SIZE    (CONFIG_INTERRUPT_NUMBER_OF_VECTORS*8)   /*IDT size */

#ifdef ASM_USE

#ifdef __AS_REGISTER_PREFIX__
#undef __AS_REGISTER_PREFIX__
#endif

/*宏定义*/
#define __AS_REGISTER_PREFIX__ %

#ifdef __AS_USER_LABEL_PREFIX__
#undef __AS_USER_LABEL_PREFIX__
#endif

#define __AS_USER_LABEL_PREFIX__

/* ANSI 相关宏定义  */
#define CONCAT1(a, b) CONCAT2(a, b)
#define CONCAT2(a, b) a ## b

/* 给全局符号加适当的前缀  */
#define SYM(x) CONCAT1 (__AS_USER_LABEL_PREFIX__, x)

/* 给寄存器加适当的前缀  */
#define REG(x) CONCAT1 (__AS_REGISTER_PREFIX__, x)

#define FUNC(sym)		sym
#define FUNC_LABEL(sym)	sym:

#define GTEXT(sym) FUNC(sym) ;  .type   FUNC(sym),@function
#define GDATA(sym) FUNC(sym) ;  .type   FUNC(sym),@object

#define PUBLIC(sym) .global SYM (sym)
#define EXTERN(sym) .extern SYM (sym)

#define BEGIN_CODE .text
#define END_CODE

#define BEGIN_DATA .data
#define END_DATA

#define BEGIN_BSS .bss
#define END_BSS

#define BEGIN_PROGRAM
#define END_PROGRAM  .end

#define ENTRY(name) \
   .text;           \
   .code32;         \
   .globl name;     \
   name:

#define eax REG (eax)
#define ebx REG (ebx)
#define ecx REG (ecx)
#define edx REG (edx)
#define esi REG (esi)
#define edi REG (edi)
#define esp REG (esp)
#define ebp REG (ebp)
#define cr0 REG (cr0)
#define cr2 REG (cr2)
#define cr3 REG (cr3)

#define ax REG (ax)
#define bx REG (bx)
#define cx REG (cx)
#define dx REG (dx)
#define si REG (si)
#define di REG (di)
#define sp REG (sp)
#define bp REG (bp)

#define ah REG (ah)
#define bh REG (bh)
#define ch REG (ch)
#define dh REG (dh)

#define al REG (al)
#define bl REG (bl)
#define cl REG (cl)
#define dl REG (dl)

#define cs REG (cs)
#define ds REG (ds)
#define es REG (es)
#define fs REG (fs)
#define gs REG (gs)
#define ss REG (ss)

#define SP_ARG0		0
#define SP_ARG1		4
#define SP_ARG1W	6
#define SP_ARG2		8
#define SP_ARG2W	10
#define SP_ARG3		12
#define SP_ARG3W	14
#define SP_ARG4		16
#define SP_ARG5		20
#define SP_ARG6		24
#define SP_ARG7		28
#define SP_ARG8		32
#define SP_ARG9		36
#define SP_ARG10	40
#define SP_ARG11	44
#define SP_ARG12	48

#define db0 REG (db0)
#define db1 REG (db1)
#define db2 REG (db2)
#define db3 REG (db3)
#define db4 REG (db4)
#define db5 REG (db5)
#define db6 REG (db6)
#define db7 REG (db7)

#endif /* ASM_USE  */

/************************类型定义******************************/

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _DB_ASM_H */
