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
 *@file:dbVector.h
 *@brief:
 *             <li>����vector��غ�</li>
 */
#ifndef _DB_VECTOR_H
#define _DB_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/

/************************�궨��********************************/
#define GENERAL_ZDIV_INT        (0) /*������0�쳣*/
#define GENERAL_DB_INT          (1) /*���Ե����쳣*/
#define GENERAL_NMI_INT         (2) /*�������ж�*/
#define GENERAL_TRAP_INT        (3) /*����ϵ��쳣*/
#define GENERAL_OVERF_INT       (4) /*�������*/
#define GENERAL_BOUND_INT       (5) /*Boundָ�ΧԽ��*/
#define GENERAL_ILLINS_INT 	    (6) /*�Ƿ��������쳣*/
#define GENERAL_DEV_INT         (7) /*Э�������������쳣*/
#define GENERAL_DF_INT          (8) /*˫�����쳣*/
#define GENERAL_CSO_INT         (9) /*Э��������Խ��*/
#define GENERAL_TSS_INT         (10)/*�Ƿ�TSS(Task State Segment)�쳣*/
#define GENERAL_SEGMENT_INT     (11)/*�β������쳣*/
#define GENERAL_SF_INT          (12)/*ջ�����쳣*/
#define GENERAL_GP_INT          (13)/*һ�㱣���쳣*/
#define GENERAL_PAGE_INT        (14)/*ҳ�����쳣*/
#define GENERAL_MF_INT          (16)/*������������쳣*/
#define GENERAL_AE_INT          (17)/*��������쳣*/
#define GENERAL_MC_INT          (18)/*Machine Check�쳣*/
#define GENERAL_SIMD_INT        (19)/*SIMD�����쳣*/

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _DB_VECTOR_H */
