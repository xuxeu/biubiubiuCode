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
#ifndef _KERNEL_DEBUG_
/************************ͷ �� ��*****************************/
#include "sdaBpIgnoreLib.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

BOOL taSdaDoIgnore = FALSE;

/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/

/*
 * @brief:
 *     ͨ���������������Ե�ǰ�ϵ�
 * @return:
 *     TA_OK���ϵ���Գɹ�
 */
T_TA_ReturnCode taSdaBpDoIgnore(T_DB_ExceptionContext *context)
{
	/* ��ֹ�ϵ� */
	taDebugBpAllDisable();

	//����һ��ָ�����ϵ�
	SetBreakAtNextPc(context);

	/* ���öϵ���Ա�־ */
	taSdaDoIgnoreSet();

	return TA_OK;
}

/*
 * @brief:
 *     ���"taTaskBpDoIgnore"�����Ķϵ���������
 * @return:
 *     TA_OK����ɶϵ����
 */
T_TA_ReturnCode taSdaBpDoneIgnore(T_DB_ExceptionContext *context)
{

	//ɾ����ǰ�ϵ�
	ClearBreakAtCurPc();

	/* ����ϵ���Ա�־ */
	taSdaDoIgnoreClr();

	/* ��ֹ�ϵ� */
	taDebugBpAllDisable(); //ɾ��gdb�ڶϵ������Ĺ����д�Ķϵ�
	
	/* ʹ�ܶϵ� */
	taDebugBpAllEnable();
	
	return TA_OK;
}

/*
 * @brief:
 *     ��ȡ���Ա�־
 * @return:
 *     ���Ա�־��ֵ
 */
BOOL taSdaDoIgnoreGet()
{
	return taSdaDoIgnore;
}

/*
 * @brief:
 *     ��λ���Ա�־
 * @return:
 *     ��
 */
void taSdaDoIgnoreSet()
{
	taSdaDoIgnore = TRUE;
}

/*
 * @brief:
 *     ������Ա�־
 * @return:
 *     ��
 */
void taSdaDoIgnoreClr()
{
	taSdaDoIgnore = FALSE;
}
#endif
