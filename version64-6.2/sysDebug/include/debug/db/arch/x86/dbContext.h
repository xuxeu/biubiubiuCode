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

#ifdef CONFIG_TA_LP64
/* ��ͨ64λ�Ĵ���ÿ��Ԫ�س���Ϊ8���ֽ� */
#define DB_REGISTER_UNIT_LEN  (8)
#else
/* ��ͨ32λ�Ĵ���ÿ��Ԫ�س���Ϊ4���ֽ� */
#define DB_REGISTER_UNIT_LEN  (4)

#endif

 /* ���ڴ�����ת��Ϊ�ַ���������չ2��  */
#define DB_MEM_TO_CHAR_LEN    (2)

/* �Ĵ������� */
#define DB_X86_REGISTER_SUM_LEN (DB_X86_REGISTER_SUM * DB_REGISTER_UNIT_LEN)

#define DB_ARCH_REGISTER_SUM_SIZE (DB_X86_REGISTER_SUM_LEN * DB_MEM_TO_CHAR_LEN)

/* �Ĵ����ĳ��� */
#define DB_REGISTER_SUM_SIZE DB_X86_REGISTER_SUM_LEN

#ifdef CONFIG_TA_LP64

#define swap(x) ((((ulong_t)(x) & 0xff00000000000000) >> 56)|\
                (((ulong_t)(x) & 0x00ff000000000000) >>  40)|\
				(((ulong_t)(x) & 0x0000ff0000000000) >> 24) |\
                (((ulong_t)(x) & 0x000000ff00000000) >>  8) |\
                (((ulong_t)(x) & 0x00000000ff000000) <<  8) |\
                (((ulong_t)(x) & 0x0000000000ff0000) << 24) |\
                (((ulong_t)(x) & 0x000000000000ff00) << 40) |\
                (((ulong_t)(x) & 0x00000000000000ff) << 56))
#else

#define swap(x) ((((UINT32)(x) & 0xff000000) >> 24)|\
                (((UINT32)(x) & 0x00ff0000) >>  8) |\
                (((UINT32)(x) & 0x0000ff00)<<8)    |\
                (((UINT32)(x) & 0x000000ff) << 24))
#endif

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


#ifdef CONFIG_TA_LP64

/* X86-64�ļĴ������ֶ��� */
typedef enum  T_DB_RegisterName
{
    RAX = 0,
    RBX = 1,
    RCX = 2,
    RDX = 3,
    RSI = 4,
    RDI = 5,
    RBP = 6,
    RSP = 7,
    R8 = 8,
    R9 = 9,
    R10 = 10,
    R11 = 11,
    R12 = 12,
    R13 = 13,
    R14 = 14,
    R15 = 15,
    RIP = 16,
    RFLAGS = 17,
    CS = 18,
    SS = 19,
    DS = 20,
    ES = 21,
    FS = 22,
    GS = 23,

    DB_X86_REGISTER_SUM = 24,/*�Ĵ�������*/

    SP=RSP,
    PC=RIP,
    PS=RFLAGS
} DB_RegisterName;

#else

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
#endif

#ifdef CONFIG_TA_LP64

/* X86-64�ļĴ��������Ķ���*/
typedef struct
{
    ulong_t  registers[DB_X86_REGISTER_SUM];
    ulong_t  vector;
} T_DB_ExceptionContext;

#else

/* X86�ļĴ��������Ķ���*/
typedef struct
{
    UINT32  registers[DB_X86_REGISTER_SUM];
    UINT32  vector;
} T_DB_ExceptionContext;

#endif


/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DB_CONTEXT_H */
