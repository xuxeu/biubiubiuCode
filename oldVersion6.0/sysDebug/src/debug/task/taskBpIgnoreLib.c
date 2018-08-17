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
 * @file:taskBpIgnoreLib.c
 * @brief:
 *             <li>�ϵ����ʵ�֣��ǰ����������ϵ���ϵ����������Ըöϵ�</li>
 */

/************************ͷ �� ��*****************************/
#include "taskBpIgnoreLib.h"
#include "ta.h"

/************************�� �� ��******************************/

#ifdef	CONFIG_CORE_SMP
/* �ϵ���Ա�־ */
BOOL taDoIgnore[MAX_CPU_NUM];
#else
BOOL taDoIgnore = FALSE;
#endif

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/

/*
 * @brief:
 *     ͨ���������������Ե�ǰ�ϵ�
 * @return:
 *     TA_OK���ϵ���Գɹ�
 */
T_TA_ReturnCode taTaskBpDoIgnore(T_DB_ExceptionContext *context)
{
	/* ��ֹ�ϵ� */
	taDebugBpAllDisable();

#if	TA_SINGLE_STEP
	/* ���õ���λ */
	taDebugStep(context);
#endif

	/* ���öϵ���Ա�־ */
	TA_DO_IGNORE_SET();

	return TA_OK;
}

/*
 * @brief:
 *     ���"taTaskBpDoIgnore"�����Ķϵ���������
 * @return:
 *     TA_OK����ɶϵ����
 */
T_TA_ReturnCode taTaskBpDoneIgnore(T_DB_ExceptionContext *context)
{
#if	TA_SINGLE_STEP
	/* �������λ */
	taDebugContinue(context);
#endif

	/* ����ϵ���Ա�־ */
	TA_DO_IGNORE_CLR();

	/* ʹ�ܶϵ� */
	taDebugBpAllEnable();

	return TA_OK;
}
