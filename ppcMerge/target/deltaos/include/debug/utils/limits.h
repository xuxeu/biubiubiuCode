/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2011-02-23         ���࣬�����������ɼ������޹�˾
 *                               �������ļ���
 * 2011-04-15	  ���£������������ɼ������޹�˾
 * 				  ��������ʽ��
*/

/*
 * @file�� limits.h
 * @brief��
 *	    <li>���弫��ֵ��</li>
 * @implements��DR 
 */

#ifndef _LIMITS_H
#define _LIMITS_H

/************************ͷ�ļ�********************************/
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************�궨��********************************/
#ifdef __MIPS__
#define __LONG_MAX__ 9223372036854775807L
#endif

#ifdef __X86__ 
#define __LONG_MAX__ 2147483647L
#endif

/* 'char'��λ��  */
#undef CHAR_BIT
#define CHAR_BIT __CHAR_BIT__

/* ���ֽ��ַ�����󳤶�  */
#ifndef MB_LEN_MAX
#define MB_LEN_MAX 1
#endif

/* һ��'signed char'���ܱ�ʾ����Сֵ�����ֵ  */
#undef SCHAR_MIN
#define SCHAR_MIN (-SCHAR_MAX - 1)
#undef SCHAR_MAX
#define SCHAR_MAX __SCHAR_MAX__

/* һ��'signed char'���ܱ�ʾ�����ֵ(��С��Ϊ0).  */
#undef UCHAR_MAX
#if __SCHAR_MAX__ == __INT_MAX__
# define UCHAR_MAX (SCHAR_MAX * 2U + 1U)
#else
# define UCHAR_MAX (SCHAR_MAX * 2 + 1)
#endif

/* һ��'char'���ܱ�ʾ����Сֵ�����ֵ  */
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

/* һ��'signed short int'���ܱ�ʾ����Сֵ�����ֵ  */
#undef SHRT_MIN
#define SHRT_MIN (-SHRT_MAX - 1)
#undef SHRT_MAX
#define SHRT_MAX __SHRT_MAX__

/* һ��'unsigned short int'���ܱ�ʾ�����ֵ(��СֵΪ0).  */
#undef USHRT_MAX
#if __SHRT_MAX__ == __INT_MAX__
# define USHRT_MAX (SHRT_MAX * 2U + 1U)
#else
# define USHRT_MAX (SHRT_MAX * 2 + 1)
#endif

/* һ��'signed int'���ܱ�ʾ����Сֵ�����ֵ  */
#undef INT_MIN
#define INT_MIN (-INT_MAX - 1)
#undef INT_MAX
#define INT_MAX __INT_MAX__

/* һ��'unsigned int'���ܱ�ʾ�����ֵ(��СֵΪ0).  */
#undef UINT_MAX
#define UINT_MAX (INT_MAX * 2U + 1U)

/* һ��'signed long int'���ܱ�ʾ����Сֵ�����ֵ(��'int'һ��).  */
#undef LONG_MIN
#define LONG_MIN (-LONG_MAX - 1L)
#undef LONG_MAX
#define LONG_MAX __LONG_MAX__

/* һ��'unsigned long int'���ܱ�ʾ�����ֵ(��СֵΪ0).  */
#undef ULONG_MAX
#define ULONG_MAX (LONG_MAX * 2UL + 1UL)

/************************���Ͷ���******************************/
/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

