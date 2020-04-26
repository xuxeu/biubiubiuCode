/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taArchLib.c
 * @brief:
 *             <li>实现对CPU中断和异常通用处理的相关操作</li>
 */

/************************头 文 件******************************/

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/* 所有异常的入口定义 */
.macro IDT_EXCEPTION_WITH_FAULTCODE_ENTRY vector 
.p2align 4       
.global exceptionEntry_\vector
exceptionEntry_\vector :	
	pushl	$\vector	
	jmp  taGeneralExpAsmHandler
.endm

.macro IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY vector 
.p2align 4 
.global exceptionEntry_\vector	
exceptionEntry_\vector :
	pushl	$0	
	pushl	$\vector	
	jmp   taGeneralExpAsmHandler
.endm

.macro IDT_FLOAT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY vector 
.p2align 4 
.global exceptionEntry_\vector	
exceptionEntry_\vector :	
	fnclex
	pushl	$0	
	pushl	$\vector	
	jmp   taGeneralExpAsmHandler
.endm

/* 除零错误 */
IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 0

/* 调试异常（自陷）*/
IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 1

/* 非屏蔽中断 */
IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 2

/* 断点 */
IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 3

/* 运算溢出 */
IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 4

/* Bound指令范围越界 */
IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 5

/* 无效指令 */
IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 6

/* 没有数学协处理器 */
IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 7

/* 二次异常 */
IDT_EXCEPTION_WITH_FAULTCODE_ENTRY    8

/* 协处理器段错误 */
IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 9

/* TSS无效 */
IDT_EXCEPTION_WITH_FAULTCODE_ENTRY 10

/* 段不存在 */
IDT_EXCEPTION_WITH_FAULTCODE_ENTRY 11

/* 栈段异常 */
IDT_EXCEPTION_WITH_FAULTCODE_ENTRY 12

/* 保护异常 */
IDT_EXCEPTION_WITH_FAULTCODE_ENTRY 13

/* 页异常 */
IDT_EXCEPTION_WITH_FAULTCODE_ENTRY 14

/* 浮点异常(15号是Intel保留的，因此略过) */

/* 数字运算错误异常 */
IDT_FLOAT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 16

/* 对齐异常 */
IDT_EXCEPTION_WITH_FAULTCODE_ENTRY 17

/* Machine Check异常 */
IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 18

/* SIMD 浮点异常 */
IDT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY 19
