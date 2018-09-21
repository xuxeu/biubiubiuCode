/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file:dbContext.h
 *@brief:
 *             <li>x86���������Ķ���</li>
 */
#ifndef _DB_CONTEXT_H
#define _DB_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"

/************************�궨��********************************/

/* �ϵ�ָ�� */
#define DB_ARCH_BREAK_INST  0xcc

/* �ϵ�ָ��� */
#define DB_ARCH_BREAK_INST_LEN  1

 /* PS�Ĵ�����TFλ,���ڵ��Ե���λʹ�� */
#define DB_X86_EFLAGS_TF  0x100

/* ״̬�Ĵ����еĵ���λ */
#define DB_ARCH_PS_TRAP_BIT DB_X86_EFLAGS_TF

/* ��ͨ32λ�Ĵ���ÿ��Ԫ�س���Ϊ4���ֽ� */
#define DB_REGISTER_UNIT_LEN  (4)

 /* ���ڴ�����ת��Ϊ�ַ���������չ2��  */
#define DB_MEM_TO_CHAR_LEN    (2)

/* �Ĵ������� */
#define DB_X86_REGISTER_SUM_LEN (DB_X86_REGISTER_SUM * DB_REGISTER_UNIT_LEN)

#define DB_ARCH_REGISTER_SUM_SIZE (DB_X86_REGISTER_SUM_LEN * DB_MEM_TO_CHAR_LEN)

/* �Ĵ����ĳ��� */
#define DB_REGISTER_SUM_SIZE DB_X86_REGISTER_SUM_LEN

#define swap(x) ((((UINT32)(x) & 0xff000000) >> 24)|\
                (((UINT32)(x) & 0x00ff0000) >>  8) |\
                (((UINT32)(x) & 0x0000ff00)<<8)    |\
                (((UINT32)(x) & 0x000000ff) << 24))

/************************���Ͷ���******************************/

/* ���÷�ӵ�ͷ�ļ�ʱ���޷�����ͨ�������Ҳ������EAX�Ⱥ꣬�����ظ����壬ֻȡ���˵�ǰ���ӳ�ͻ�Ķ��� */
#ifdef EAX
	#undef EAX
	#undef ECX
	#undef EDX
	#undef EBX
	#undef ESP
	#undef EBP
	#undef ESI
	#undef EDI
	#undef EFLAGS
	#undef CS
	#undef SS
	#undef DS
	#undef ES
	#undef FS
	#undef GS
#endif

/* X86�ļĴ������ֶ��� */
typedef enum  T_DB_RegisterName
{
    EAX = 0,
    ECX = 1,
    EDX = 2,
    EBX = 3,
    ESP = 4,
    EBP = 5,
    ESI = 6,
    EDI = 7,
    EIP = 8,
    EFLAGS = 9,
    CS = 10,
    SS = 11,
    DS = 12,
    ES = 13,
    FS = 14,
    GS = 15,
    DB_X86_REGISTER_SUM = 16,/*�Ĵ�������*/

    SP=ESP,
    PC=EIP,
    PS=EFLAGS
} DB_RegisterName;

/* X86�ļĴ��������Ķ���*/
typedef struct
{
    UINT32  registers[DB_X86_REGISTER_SUM];
    UINT32  vector;
} T_DB_ExceptionContext;

/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DB_CONTEXT_H */
