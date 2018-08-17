/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2013-06-28         ��Ԫ־�������������ɼ������޹�˾
 *                               �������ļ���
*/

/*
 * @file�� taTypes.h
 * @brief��
 *	    <li>�ṩϵͳ���ͺ���ͨ��Ķ���</li>
 */
#ifndef _TATYPE_H
#define _TATYPE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "types/vxTypesOld.h"

/************************�궨��********************************/

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
#define T_VOID      void             /* void���� */
#define T_VVOID     volatile void    /* ���Ż���void���� */
#define T_MODULE    static           /* static���η� */
#define T_INLINE    T_MODULE inline  /* static inline ���η� */
#define T_EXTERN    extern           /* extern���η� */
#define T_VOLATILE  volatile         /* volatile���η� */
#define T_CONST     const            /* const���η� */

/************************���Ͷ���******************************/
#ifndef VX_TYPE_USED
typedef signed char                     T_BYTE;     /* �з���8bit*/
typedef unsigned char                   T_UBYTE;    /* �޷���8bit*/
typedef char                            T_CHAR;     /* �з���8bit*/

typedef signed short                    T_HWORD;    /* �з���16bit*/
typedef unsigned short                  T_UHWORD;   /* �޷���16bit*/

typedef signed int                      T_WORD;     /* �з���32bit*/
typedef unsigned int                    T_UWORD;    /* �޷���32bit*/

typedef signed long long                T_DWORD;    /* �з���64bit*/
typedef unsigned long long              T_UDWORD;   /* �޷���64bit*/

typedef double                          T_DOUBLE;   /* ˫���ȸ�����*/
typedef float                           T_FLOAT;    /* �����ȸ�����*/

typedef unsigned int                    T_BOOL;     /* �޷���32bit*/

typedef volatile signed char            T_VBYTE;    /* ���Ż����з���8bit*/
typedef volatile unsigned char          T_VUBYTE;   /* ���Ż����޷���8bit*/

typedef volatile signed short           T_VHWORD;   /* ���Ż����з���16bit*/
typedef volatile unsigned short         T_VUHWORD;  /* ���Ż����޷���16bit*/

typedef volatile signed int             T_VWORD;    /* ���Ż����з���32bit*/
typedef volatile unsigned int           T_VUWORD;   /* ���Ż����޷���32bit*/

typedef volatile signed long long       T_VDWORD;   /* ���Ż����з���64bit*/
typedef volatile unsigned long long     T_VUDWORD;  /* ���Ż����޷���64bit*/

typedef volatile double                 T_VDOUBLE;  /* ���Ż���˫���ȸ�����*/
typedef volatile float                  T_VFLOAT;   /* ���Ż��ĵ����ȸ�����*/

typedef volatile unsigned int           T_VBOOL;    /* ���Ż����޷���32bit*/
#endif
/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _TATYPE_H */
