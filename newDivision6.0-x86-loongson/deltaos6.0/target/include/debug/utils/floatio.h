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
 * @file  floatio.h
 * @brief
 *    <li>提供浮点输入输出相关的宏定义、类型定义和函数声明。<li>
 * @implements: RR.1
 */

#ifndef _FLOATIO_H
#define _FLOATIO_H

/************************头文件********************************/
#include <mathBase.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************宏定义********************************/
/* 最大合理指数 */
#define	MAXEXP		308

/* 最大合理精度 */
#define	MAXFRACT	39

#define	EOF	(-1)
#define EOS '\0'
#define FOREVER while(1)
#ifndef OK
#define OK    	0
#endif
#define ERROR (-1)

#ifndef TRUE
#define TRUE    	1
#endif

#ifndef FALSE
#define FALSE    	0
#endif

#ifndef NULL
#define NULL    	0
#endif

#define to_digit(c)	((c) - '0')
#define is_digit(c)	(((c) >= (signed char)'0') && ((c) <= (signed char)'9'))
#define to_char(n)	(signed char)((n) + '0')

/* 根据转换类型从valist中转换出指定类型的参数，最后转换为long long类型 */
#define SARG()  ((TRUE == doLongLongInt) ? (long long) va_arg(vaList, long long) : \
     (TRUE == doLongInt) ? (long long)(long)va_arg(vaList, long) : \
     (TRUE == doShortInt) ? (long long)(short)va_arg(vaList, int) : \
     (long long)(int) va_arg(vaList, int))

/* 根据转换类型从valist中转换出指定类型的参数，最后转换为unsigned long long类型 */
#define UARG()  ((TRUE == doLongLongInt) ? (unsigned long long) va_arg(vaList, unsigned long long) :  \
   (TRUE == doLongInt) ? (unsigned long long)(ulong_t)va_arg(vaList,ulong_t):\
   (TRUE == doShortInt) ? (unsigned long long)(ushort_t)va_arg(vaList,int):\
   (unsigned long long)(uint_t) va_arg(vaList, uint_t))

/************************类型定义******************************/

typedef int STATUS;
typedef int	BOOL;
typedef int (*FUNCPTR)(va_list *pVaList, int precision, BOOL doAlt, int fmtSym, BOOL *pDoSign, signed char *pBufStart, signed char *pBufEnd);
typedef unsigned int uint_t;
typedef unsigned short ushort_t;
typedef unsigned long ulong_t;
typedef unsigned int size_t;
typedef signed int      INT32;
typedef unsigned char   UINT8;
typedef unsigned int    UINT32;

/************************接口声明******************************/
/*
 * @brief: 
 *    将参数列表按给定的格式转换后，输出到指定的设备，返回被输出字符的数目。
 * @param[in]: fmt: 格式控制字符串
 * @param[in]: vaList: 可变参数列表
 * @param[in]: outarg: putbuf的参数
 * @return: 
 *    ERROR(-1): 输出字符失败。
 *    输出字符的数目。
 * @notes: 
 *    ERROR = -1
 * @tracedREQ: RR.1.1
 * @implements: DR.1.14
 */
int fioformatstr(const signed char *fmt,
va_list vaList,
int outarg);

/*
 * @brief: 
 *    按指定个数在指定的字符数组的中查找第一个等于指定字符的元素。
 * @param[in]: src_void: 查找源起始地址
 * @param[in]: c: 查找字符
 * @param[in]: length: 查找长度
 * @return: 
 *    第一个等于指定字符的元素地址。<br>
 *    NULL: 没有找到指定字符。<br>
 * @qualification method: 分析/测试
 * @derived requirement: 否
 * @function location: Internal
 * @implements: DR.1.20
 */
void *memchr(const void *src_void,
int c,
size_t length);

#ifdef __cplusplus
}
#endif
#endif 



