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
 *             <li>MIPS���������Ķ���</li>
 */
#ifndef _DB_CONTEXT_H
#define _DB_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"

/************************�궨��********************************/

/* Ӳ�����ݶϵ��쳣 */
#define GENERAL_HBP_INT       (27)

/* ����ϵ��쳣  */
#define GENERAL_TRAP_INT       (3)

/* �ϵ�ָ�� */
#define DB_ARCH_BREAK_INST     0x0000000d

/* �ϵ�ָ��� */
#define DB_ARCH_BREAK_INST_LEN  4

/* ��ͨ64λ�Ĵ���ÿ��Ԫ�س���Ϊ4���ֽ� */
#define DB_REGISTER_UNIT_LEN   8

/* ���ڴ�����ת��Ϊ�ַ���������չ2��  */
#define DB_MEM_TO_CHAR_LEN     2

/* ��ͨ64λ����Ĵ���ÿ��Ԫ�س���Ϊ8���ֽ� */
#define DB_FLOAT_REGISTER_UNIT_LEN 8

/* �Ĵ������� */
#define DB_MIPS_REGISTER_SIZE (REGISTER_SUM)

#define DB_REGISTER_SIZE  (DB_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

#define DB_FLOAT_REGISTER_SIZE  (DB_FLOAT_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

#define DB_MIPS_REGISTER_SUM_LEN (DB_MIPS_REGISTER_SIZE * DB_REGISTER_UNIT_LEN)

#define DB_ARCH_REGISTER_SUM_SIZE (DB_MIPS_REGISTER_SUM_LEN * DB_MEM_TO_CHAR_LEN)

/* �Ĵ����ĳ��� */
#define DB_REGISTER_SUM_SIZE DB_MIPS_REGISTER_SUM_LEN

/* ״̬�Ĵ����е��ж�λ */
#define DB_ARCH_PS_INT_BIT DB_MIPS_MSR_IE

#define swap64(x) (\
    (((UINT64)(x)  & (UINT64)0xff00000000000000) >> 56)  \
    |(((UINT64)(x) & (UINT64)0x00ff000000000000) >> 40)  \
    |(((UINT64)(x) & (UINT64)0x0000ff0000000000) >> 24)  \
    |(((UINT64)(x) & (UINT64)0x000000ff00000000) >> 8)  \
    |(((UINT64)(x) & (UINT64)0x00000000ff000000) << 8)  \
    |(((UINT64)(x) & (UINT64)0x0000000000ff0000) <<  24)  \
    | (((UINT64)(x) & (UINT64)0x000000000000ff00)<<40) \
    |(((UINT64)(x)  & (UINT64)0x00000000000000ff) << 56))

/************************���Ͷ���******************************/
#ifdef SR
#undef SR
#endif

/* MIPS�ļĴ������ֶ��� */
enum  T_DB_RegisterName
{
	GPR0 = 0,
	GPR1 = 1,
	GPR2 = 2,
	GPR3 = 3,
	GPR4 = 4,
	GPR5 = 5,
	GPR6 = 6,
	GPR7 = 7,
	GPR8 = 8,
	GPR9 = 9,
	GPR10 = 10 ,
	GPR11 = 11,
	GPR12 = 12,
	GPR13 = 13,
	GPR14 = 14,
	GPR15 = 15,
	GPR16 = 16,
	GPR17 = 17,
	GPR18 = 18,
	GPR19 = 19,
	GPR20 = 20,
	GPR21 = 21,
	GPR22 = 22,
	GPR23 = 23,
	GPR24 = 24,
	GPR25 = 25,
	GPR26 = 26,
	GPR27 = 27,
	GPR28 = 28,
	GPR29 =29,
	GPR30 = 30,
	GPR31 = 31,
	GPR_SUM = 32,

	SR = 0,
	LO = 1,
	HI = 2,
	BAD = 3,
	CAUSE = 4,
	EPC = 5,
	SPR_SUM = 6,

	FGPR0 = 0,
	FGPR1 = 1,
	FGPR2 = 2,
	FGPR3 = 3,
	FGPR4 = 4,
	FGPR5 = 5,
	FGPR6 = 6,
	FGPR7 = 7,
	FGPR8 = 8,
	FGPR9 = 9,
	FGPR10 = 10 ,
	FGPR11 = 11,
	FGPR12 = 12,
	FGPR13 = 13,
	FGPR14 = 14,
	FGPR15 = 15,
	FGPR16 = 16,
	FGPR17 = 17,
	FGPR18 = 18,
	FGPR19 = 19,
	FGPR20 = 20,
	FGPR21 = 21,
	FGPR22 = 22,
	FGPR23 = 23,
	FGPR24 = 24,
	FGPR25 = 25,
	FGPR26 = 26,
	FGPR27 = 27,
	FGPR28 = 28,
	FGPR29 = 29,
	FGPR30 = 30,
	FGPR31 = 31,
	FGPR_SUM = 32,
	FSR=0,
	FIR = 1,
	FPSR = 2,
	//REGISTER_SUM = GPR_SUM + FGPR_SUM + SPR_SUM + FPSR,
	REGISTER_SUM = GPR_SUM + SPR_SUM,
	STATUSREG = GPR_SUM  + SR,
	WATCHLOREG = GPR_SUM + LO,
	WATCHHIREG = GPR_SUM + HI,
	BADVADDRREG = GPR_SUM + BAD,
	CAUSEREG = GPR_SUM + CAUSE,
	PC = GPR_SUM + EPC,
	SP = GPR30,
	PLR = GPR31,
}DB_RegisterName;

/* �쳣����MIPS�ļĴ��������Ķ���*/
typedef struct T_DB_ExceptionContext
{
    /*������ڴ�˳��:32��GPR,32������,SR,LO,HI,BAD,CAUSE,EPC,FSR,FIR*/
    UINT64 registers[REGISTER_SUM];

    /* �쳣������ */
    UINT64  vector;
}T_DB_ExceptionContext;

/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DB_CONTEXT_H */
