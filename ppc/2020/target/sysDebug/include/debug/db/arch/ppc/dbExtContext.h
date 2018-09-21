/*
 *�����ʷ: 
 * 2010-11-9  ������       �������ļ���
 */


/**
 * @file  dbExtContext.h
 * @brief 
 *       ����: 
 *       <li>�����Ķ���</li>
 */


#ifndef DB_EXT_CONTEXT_H
#define DB_EXT_CONTEXT_H


/*ͷ�ļ�*/


#include "dbContext.h"

#ifdef __cplusplus
extern "C" {
#endif


/*�궨��*/

/* �ϵ�ָ�� */
#define DB_ARCH_BREAK_INST        0x7d821008

/* �ϵ�ָ��� */
#define DB_ARCH_BREAK_INST_LEN    4

/* ��ͨ32λ�Ĵ���ÿ��Ԫ�س���Ϊ4���ֽ� */
#define DB_REGISTER_UNIT_LEN   4

/* ���ڴ�����ת��Ϊ�ַ���������չ2��  */
#define DB_MEM_TO_CHAR_LEN     2  

 /* ��ͨ64λ�Ĵ���ÿ��Ԫ�س���Ϊ8���ֽ� */
#define DB_FLOAT_REGISTER_UNIT_LEN 8   

 /* MSR�Ĵ�����SEλ,���ڵ���λʹ�� */
#define DB_PPC_MSR_SE    0x400     

/* PPC MSR�Ĵ�����IRλ */
#define DB_PPC_MSR_IR    0x20

 /* PPC MSR�Ĵ�����DRλ */
#define DB_PPC_MSR_DR    0x10

/* ͨ�üĴ������� */
#define DB_REGISTER_SIZE  (DB_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

/* ����Ĵ������� */
#define DB_FLOAT_REGISTER_SIZE  (DB_FLOAT_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

/* ״̬�Ĵ����еĵ���λ */
#define DB_ARCH_PS_TRAP_BIT DB_PPC_MSR_SE

/*���Ͷ���*/


/*�ӿ�����*/


#ifdef __cplusplus
}
#endif

#endif/*DB_EXT_CONTEXT_H*/
