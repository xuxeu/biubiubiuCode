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
 * @file  floatio.h
 * @brief
 *    <li>�ṩ�������������صĺ궨�塢���Ͷ���ͺ���������<li>
 * @implements: RR.1
 */

#ifndef _FLOATIO_H
#define _FLOATIO_H

/************************ͷ�ļ�********************************/
#include <mathBase.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************�궨��********************************/
/* ������ָ�� */
#define	MAXEXP		308

/* �������� */
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

/* ����ת�����ʹ�valist��ת����ָ�����͵Ĳ��������ת��Ϊlong long���� */
#define SARG()  ((TRUE == doLongLongInt) ? (long long) va_arg(vaList, long long) : \
     (TRUE == doLongInt) ? (long long)(long)va_arg(vaList, long) : \
     (TRUE == doShortInt) ? (long long)(short)va_arg(vaList, int) : \
     (long long)(int) va_arg(vaList, int))

/* ����ת�����ʹ�valist��ת����ָ�����͵Ĳ��������ת��Ϊunsigned long long���� */
#define UARG()  ((TRUE == doLongLongInt) ? (unsigned long long) va_arg(vaList, unsigned long long) :  \
   (TRUE == doLongInt) ? (unsigned long long)(ulong_t)va_arg(vaList,ulong_t):\
   (TRUE == doShortInt) ? (unsigned long long)(ushort_t)va_arg(vaList,int):\
   (unsigned long long)(uint_t) va_arg(vaList, uint_t))

/************************���Ͷ���******************************/

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

/************************�ӿ�����******************************/
/*
 * @brief: 
 *    �������б������ĸ�ʽת���������ָ�����豸�����ر�����ַ�����Ŀ��
 * @param[in]: fmt: ��ʽ�����ַ���
 * @param[in]: vaList: �ɱ�����б�
 * @param[in]: outarg: putbuf�Ĳ���
 * @return: 
 *    ERROR(-1): ����ַ�ʧ�ܡ�
 *    ����ַ�����Ŀ��
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
 *    ��ָ��������ָ�����ַ�������в��ҵ�һ������ָ���ַ���Ԫ�ء�
 * @param[in]: src_void: ����Դ��ʼ��ַ
 * @param[in]: c: �����ַ�
 * @param[in]: length: ���ҳ���
 * @return: 
 *    ��һ������ָ���ַ���Ԫ�ص�ַ��<br>
 *    NULL: û���ҵ�ָ���ַ���<br>
 * @qualification method: ����/����
 * @derived requirement: ��
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



