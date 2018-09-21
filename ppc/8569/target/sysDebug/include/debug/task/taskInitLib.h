/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-08-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file: taskInitLib.h
 * @brief:
 *             <li>���񼶵��Գ�ʼ����ؽӿ�����</li>
 */
#ifndef _TASK_INIT_LIB_H_
#define _TASK_INIT_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ �� ��******************************/
#include "taErrorDefine.h"

/************************���Ͷ���*****************************/

/************************�� �� ��******************************/

/************************�ӿ�����*****************************/

/**
 * @brief:
 * 	   ��ʼ������HOOK���������񴴽�HOOK������ɾ��HOOK
 * @return:
 * 	   ��
 */
void taTaskHookInit(void);

/**
 * @brief:
 * 	  ����ʱ�ӿڳ�ʼ��
 * @return:
 * 	   ��
 */
void taRtLibInit(TA_RT_IF *pRtIf);

/**
 * @brief:
 * 	  �����������ʱ�ӿڳ�ʼ��
 * @return:
 * 	   ��
 */
void taTaskRtIfInit(void);

/*
 * @brief:
 *     ��װ�����������
 * @return:
 *     ��
 */
void taCtrlCommandInit(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_INIT_LIB_H_ */
