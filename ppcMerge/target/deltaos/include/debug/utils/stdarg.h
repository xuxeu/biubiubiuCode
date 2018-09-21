/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2011-02-23         李燕，北京科银京成技术有限公司
 *                               创建该文件。
 * 2011-04-15	  张勇，北京科银京成技术有限公司
 * 				  整理代码格式、优化代码、修改GBJ5369违反项。
*/

/*
 * @file： stdarg.h
 * @brief：
 *	    <li>提供不定函数参数相关的宏定义和类型定义。</li>
 * @implements： DR.1
 */

#ifndef _STDARG_H
#define _STDARG_H

/************************头文件********************************/
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************宏定义********************************/
#if 0
/*4X中的定义方式*/
#define va_start(v,l) (__builtin_va_start(v,l))
#define va_end(v) (__builtin_va_end(v))
#define va_arg(v,l) (__builtin_va_arg(v,l))
#endif


typedef __builtin_va_list va_list;


#ifdef __MIPS__


#define va_start(v,l)	__builtin_stdarg_start((v),l)

#endif

#ifdef __PPC__
#define va_start(v,l)	__builtin_stdarg_start((v),l)
#endif

#ifdef __X86__
#define va_start(ap, pN)	\
	((ap) = ((va_list) __builtin_next_arg(pN)))

#endif
#define va_end		__builtin_va_end
#define va_arg		__builtin_va_arg

/************************类型定义******************************/


/************************接口声明******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif


