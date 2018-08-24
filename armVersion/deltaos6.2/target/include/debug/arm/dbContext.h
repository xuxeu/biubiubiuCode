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
 *             <li>ARM���������Ķ���</li>
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
#define DB_ARCH_BREAK_INST  0xe120007c  //e120007c  //ef9f0001

/* �ϵ�ָ��� */
#define DB_ARCH_BREAK_INST_LEN  4

 /* PS�Ĵ�����TFλ,���ڵ��Ե���λʹ�� */
#define DB_ARM_EFLAGS_TF  0x100

/* ״̬�Ĵ����еĵ���λ */
#define DB_ARCH_PS_TRAP_BIT DB_ARM_EFLAGS_TF

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
#define DB_ARM_REGISTER_SUM_LEN (DB_ARM_REGISTER_SUM * DB_REGISTER_UNIT_LEN)

#define DB_ARCH_REGISTER_SUM_SIZE (DB_ARM_REGISTER_SUM_LEN * DB_MEM_TO_CHAR_LEN)

/* �Ĵ����ĳ��� */
#define DB_REGISTER_SUM_SIZE DB_ARM_REGISTER_SUM_LEN

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





/* X86�ļĴ������ֶ��� */
typedef enum  T_DB_RegisterName
{
    R0 = 0,
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R4 = 4,
    R5 = 5,
    R6 = 6,
    R7 = 7,
    R8 = 8,
    R9 = 9,
    R10 = 10,
    R11 = 11,
    R12 = 12,
    R13 = 13,
    R14 = 14,
    PC = 15,
    F0 = 16,
    F1 = 17,
    F2 = 18,
    F3 = 19,
    F4 = 20,
    F5 = 21,
    F6 = 22,
    F7 = 23,
    FPS= 24,
    CPSR = 25,
    DB_ARM_REGISTER_SUM = 26,  /*�Ĵ�������*/
    FP = R11,
    SP = R13,
    LR = R14

} DB_RegisterName;


/* ARM�ļĴ��������Ķ���*/
typedef struct
{
    ulong_t  registers[DB_ARM_REGISTER_SUM];
    ulong_t  vector;
} T_DB_ExceptionContext;



/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DB_CONTEXT_H */
