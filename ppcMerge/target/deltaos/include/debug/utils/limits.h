/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2011-02-23         李燕，北京科银京成技术有限公司
 *                               创建该文件。
 * 2011-04-15	  张勇，北京科银京成技术有限公司
 * 				  整理代码格式。
*/

/*
 * @file： limits.h
 * @brief：
 *	    <li>定义极限值。</li>
 * @implements：DR 
 */

#ifndef _LIMITS_H
#define _LIMITS_H

/************************头文件********************************/
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************宏定义********************************/
#ifdef __MIPS__
#define __LONG_MAX__ 9223372036854775807L
#endif

#ifdef __X86__ 
#define __LONG_MAX__ 2147483647L
#endif

/* 'char'的位数  */
#undef CHAR_BIT
#define CHAR_BIT __CHAR_BIT__

/* 多字节字符的最大长度  */
#ifndef MB_LEN_MAX
#define MB_LEN_MAX 1
#endif

/* 一个'signed char'数能表示的最小值和最大值  */
#undef SCHAR_MIN
#define SCHAR_MIN (-SCHAR_MAX - 1)
#undef SCHAR_MAX
#define SCHAR_MAX __SCHAR_MAX__

/* 一个'signed char'数能表示的最大值(最小至为0).  */
#undef UCHAR_MAX
#if __SCHAR_MAX__ == __INT_MAX__
# define UCHAR_MAX (SCHAR_MAX * 2U + 1U)
#else
# define UCHAR_MAX (SCHAR_MAX * 2 + 1)
#endif

/* 一个'char'数能表示的最小值和最大值  */
#ifdef __CHAR_UNSIGNED__
# undef CHAR_MIN
# if __SCHAR_MAX__ == __INT_MAX__
#  define CHAR_MIN 0U
# else
#  define CHAR_MIN 0
# endif
# undef CHAR_MAX
# define CHAR_MAX UCHAR_MAX
#else
# undef CHAR_MIN
# define CHAR_MIN SCHAR_MIN
# undef CHAR_MAX
# define CHAR_MAX SCHAR_MAX
#endif

/* 一个'signed short int'数能表示的最小值和最大值  */
#undef SHRT_MIN
#define SHRT_MIN (-SHRT_MAX - 1)
#undef SHRT_MAX
#define SHRT_MAX __SHRT_MAX__

/* 一个'unsigned short int'数能表示的最大值(最小值为0).  */
#undef USHRT_MAX
#if __SHRT_MAX__ == __INT_MAX__
# define USHRT_MAX (SHRT_MAX * 2U + 1U)
#else
# define USHRT_MAX (SHRT_MAX * 2 + 1)
#endif

/* 一个'signed int'数能表示的最小值和最大值  */
#undef INT_MIN
#define INT_MIN (-INT_MAX - 1)
#undef INT_MAX
#define INT_MAX __INT_MAX__

/* 一个'unsigned int'数能表示的最大值(最小值为0).  */
#undef UINT_MAX
#define UINT_MAX (INT_MAX * 2U + 1U)

/* 一个'signed long int'数能表示的最小值和最大值(与'int'一样).  */
#undef LONG_MIN
#define LONG_MIN (-LONG_MAX - 1L)
#undef LONG_MAX
#define LONG_MAX __LONG_MAX__

/* 一个'unsigned long int'数能表示的最大值(最小值为0).  */
#undef ULONG_MAX
#define ULONG_MAX (LONG_MAX * 2UL + 1UL)

/************************类型定义******************************/
/************************接口声明******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

