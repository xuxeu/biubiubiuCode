/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2013-06-28         彭元志，北京科银京成技术有限公司
 *                               创建该文件。
*/

/*
 * @file： taTypes.h
 * @brief：
 *	    <li>提供系统类型和普通宏的定义</li>
 */
#ifndef _TATYPE_H
#define _TATYPE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************头文件********************************/
#include "types/vxTypesOld.h"

/************************宏定义********************************/

#ifndef TRUE
#define TRUE    	1
#endif

#ifndef FALSE
#define FALSE    	0
#endif

#ifndef NULL
#define NULL    	0
#endif

#ifndef ERROR
#define ERROR   (-1)
#endif

#ifndef OK
#define OK    	0
#endif
#define big    	    0xffffffff
#define T_VOID      void             /* void类型 */
#define T_VVOID     volatile void    /* 非优化的void类型 */
#define T_MODULE    static           /* static修饰符 */
#define T_INLINE    T_MODULE inline  /* static inline 修饰符 */
#define T_EXTERN    extern           /* extern修饰符 */
#define T_VOLATILE  volatile         /* volatile修饰符 */
#define T_CONST     const            /* const修饰符 */

/************************类型定义******************************/
#ifndef VX_TYPE_USED
typedef signed char                     T_BYTE;     /* 有符号8bit*/
typedef unsigned char                   T_UBYTE;    /* 无符号8bit*/
typedef char                            T_CHAR;     /* 有符号8bit*/

typedef signed short                    T_HWORD;    /* 有符号16bit*/
typedef unsigned short                  T_UHWORD;   /* 无符号16bit*/

typedef signed int                      T_WORD;     /* 有符号32bit*/
typedef unsigned int                    T_UWORD;    /* 无符号32bit*/

typedef signed long long                T_DWORD;    /* 有符号64bit*/
typedef unsigned long long              T_UDWORD;   /* 无符号64bit*/

typedef double                          T_DOUBLE;   /* 双精度浮点型*/
typedef float                           T_FLOAT;    /* 单精度浮点型*/

typedef unsigned int                    T_BOOL;     /* 无符号32bit*/

typedef volatile signed char            T_VBYTE;    /* 非优化的有符号8bit*/
typedef volatile unsigned char          T_VUBYTE;   /* 非优化的无符号8bit*/

typedef volatile signed short           T_VHWORD;   /* 非优化的有符号16bit*/
typedef volatile unsigned short         T_VUHWORD;  /* 非优化的无符号16bit*/

typedef volatile signed int             T_VWORD;    /* 非优化的有符号32bit*/
typedef volatile unsigned int           T_VUWORD;   /* 非优化的无符号32bit*/

typedef volatile signed long long       T_VDWORD;   /* 非优化的有符号64bit*/
typedef volatile unsigned long long     T_VUDWORD;  /* 非优化的无符号64bit*/

typedef volatile double                 T_VDOUBLE;  /* 非优化的双精度浮点型*/
typedef volatile float                  T_VFLOAT;   /* 非优化的单精度浮点型*/

typedef volatile unsigned int           T_VBOOL;    /* 非优化的无符号32bit*/
#endif
/************************接口声明******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _TATYPE_H */
