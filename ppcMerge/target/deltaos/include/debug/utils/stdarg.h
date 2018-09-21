/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2011-02-23         ���࣬�����������ɼ������޹�˾
 *                               �������ļ���
 * 2011-04-15	  ���£������������ɼ������޹�˾
 * 				  ��������ʽ���Ż����롢�޸�GBJ5369Υ���
*/

/*
 * @file�� stdarg.h
 * @brief��
 *	    <li>�ṩ��������������صĺ궨������Ͷ��塣</li>
 * @implements�� DR.1
 */

#ifndef _STDARG_H
#define _STDARG_H

/************************ͷ�ļ�********************************/
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************�궨��********************************/
#if 0
/*4X�еĶ��巽ʽ*/
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

/************************���Ͷ���******************************/


/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif


