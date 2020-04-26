/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-12-17         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:taskBpIgnoreLib.h
 * @brief:
 *             <li>�ϵ������غ궨��ͺ�������</li>
 */
#ifndef _TASK_BP_IGNORE_LIB_H_
#define _TASK_BP_IGNORE_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ �� ��******************************/
#include "taTypes.h"
#include "ta.h"

/************************���Ͷ���*****************************/

/************************�� �� ��******************************/

#ifdef	CONFIG_CORE_SMP
/* �ϵ���Ա�־��ز����궨�� */
#define TA_DO_IGNORE() \
		taDoIgnore[GET_CPU_INDEX()]
#define TA_DO_IGNORE_SET() \
		(taDoIgnore[GET_CPU_INDEX()] = TRUE)
#define TA_DO_IGNORE_CLR() \
		(taDoIgnore[GET_CPU_INDEX()] = FALSE)
#else
#define TA_DO_IGNORE()		taDoIgnore
#define TA_DO_IGNORE_SET()  (taDoIgnore = TRUE)
#define TA_DO_IGNORE_CLR()	(taDoIgnore = FALSE)
#endif

/************************�ӿ�����*****************************/

#ifdef	CONFIG_CORE_SMP
/* �ϵ���Ա�־ */
extern BOOL taDoIgnore[MAX_CPU_NUM];
#else
extern BOOL taDoIgnore;
#endif

/*
 * @brief:
 *     ͨ���������������Ե�ǰ�ϵ�
 * @return:
 *     TA_OK���ϵ���Գɹ�
 */
T_TA_ReturnCode taTaskBpDoIgnore(T_DB_ExceptionContext *context);

/*
 * @brief:
 *     ���"taTaskBpDoIgnore"�����Ķϵ���������
 * @return:
 *     TA_OK����ɶϵ����
 */
T_TA_ReturnCode taTaskBpDoneIgnore(T_DB_ExceptionContext *context);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_BP_IGNORE_LIB_H_ */
