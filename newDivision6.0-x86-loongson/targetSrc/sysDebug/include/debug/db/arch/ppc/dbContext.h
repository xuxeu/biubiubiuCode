/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-28         �Ʊ󣬱����������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file:dbContext.h
 *@brief:
 *             <li>���������Ķ���</li>
 */
#ifndef _DB_CONTEXT_H
#define _DB_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"

/************************�궨��********************************/

/* �Ĵ�����С */
#define DB_PPC_REGISTER_SIZE (GPR_SUM + FGPR_SUM*2 + SPR_SUM)

/* ע�⣺FPSCR��Ȼֻռ4�ֽڣ�����stfdָ���������8�ֽڣ�������ҪΪFPSCR����8�ֽڵĿռ�*/
#define DB_PPC_FPSCR_SIZE	(2)

#define DB_ARCH_BREAK_INST  0x7d821008
#define DB_ARCH_BREAK_INST_LEN 4
#define DB_REGISTER_UNIT_LEN   4     /* ��ͨ32λ�Ĵ���ÿ��Ԫ�س���Ϊ4���ֽ� */
#define DB_MEM_TO_CHAR_LEN     2     /* ���ڴ�����ת��Ϊ�ַ���������չ2��  */
#define DB_FLOAT_REGISTER_UNIT_LEN 8 /* ��ͨ64λ�Ĵ���ÿ��Ԫ�س���Ϊ8���ֽ� */

#define DB_PPC_MSR_SE    0x400      /* MSR�Ĵ�����SEλ,���ڵ���λʹ�� */
#define DB_PPC_MSR_EE    0x8000     /* MSR�Ĵ�����EEλ,�����ⲿ�жϵ�ʹ�� */
#define DB_PPC_MSR_IR    0x20       /* PPC MSR�Ĵ�����IRλ */
#define DB_PPC_MSR_DR    0x10       /* PPC MSR�Ĵ�����DRλ */

/* ͨ�üĴ������� */
#define DB_REGISTER_SIZE  (DB_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

/* ����Ĵ������� */
#define DB_FLOAT_REGISTER_SIZE  (DB_FLOAT_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

/* ״̬�Ĵ����еĵ���λ */
#define DB_ARCH_PS_TRAP_BIT DB_PPC_MSR_SE

/* ״̬�Ĵ����е��ж�λ */
#define DB_ARCH_PS_INT_BIT DB_PPC_MSR_EE

/* GDB��ȡ�ļĴ������ȣ�DB_PPC_REGISTER_SIZEΪ�쳣�����ĵĳ��� */
#define DB_PPC_REGISTER_SUM_LEN (DB_PPC_REGISTER_SIZE * DB_REGISTER_UNIT_LEN)

/* ��չ��Ĵ����ĳ��� */
#define DB_PPC_REGISTER_SUM_SIZE (DB_PPC_REGISTER_SUM_LEN * DB_MEM_TO_CHAR_LEN)

#define DB_REGISTER_SUM_SIZE DB_PPC_REGISTER_SUM_LEN

/************************���Ͷ���******************************/
/* PPC�ļĴ������ֶ��� */
typedef enum  T_DB_RegisterName
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
    GPR10 = 10,
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
    GPR29 = 29,
    GPR30 = 30,
    GPR31 = 31,
    GPR_SUM = 32,
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
    FGPR10 = 10,
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
    PCR = 0,
    MSR = 1,
    CR = 2,
    LR = 3,
    CTR = 4,
    XER = 5,
    SPR_SUM = 6,
    REGISTER_SUM = GPR_SUM + FGPR_SUM + SPR_SUM,
    SP = GPR1,
    PC = GPR_SUM + FGPR_SUM*2 + PCR,
    PS = GPR_SUM + FGPR_SUM*2 + MSR,
    PCR1 = GPR_SUM + FGPR_SUM*2 + CR,
    PLR = GPR_SUM + FGPR_SUM*2 + LR,
    PCTR = GPR_SUM + FGPR_SUM*2 + CTR,
    PXER = GPR_SUM + FGPR_SUM*2 + XER
} DB_RegisterName;


/* �쳣����PPC�ļĴ��������Ķ���*/
typedef struct T_DB_ExceptionContext
{
    /* ������ڴ�˳��:32��GPR,32������,PCR,MSR,CR,LR,CTR,XER,FPSCR */
    UINT32 registers[DB_PPC_REGISTER_SIZE + DB_PPC_FPSCR_SIZE];

    /* �쳣������ */
    UINT32  vector;
} DB_ExceptionContext;

/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_DB_CONTEXT_H */
