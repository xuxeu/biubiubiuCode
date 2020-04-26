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
#ifndef _SDA_BP_IGNORE_LIB_H_
#define _SDA_BP_IGNORE_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ �� ��******************************/
#include "taTypes.h"
#include "ta.h"

/************************���Ͷ���*****************************/

/************************�� �� ��******************************/

/************************�ӿ�����*****************************/
/*
 * @brief:
 *     ��ȡ���Ա�־
 * @return:
 *     ���Ա�־��ֵ
 */
BOOL taSdaDoIgnoreGet();

/*
 * @brief:
 *     ��λ���Ա�־
 * @return:
 *     ��
 */
void taSdaDoIgnoreSet();

/*
 * @brief:
 *     ������Ա�־
 * @return:
 *     ��
 */
void taSdaDoIgnoreClr();


/*
 * @brief:
 *     ͨ���������������Ե�ǰ�ϵ�
 * @return:
 *     TA_OK���ϵ���Գɹ�
 */
T_TA_ReturnCode taSdaBpDoIgnore(T_DB_ExceptionContext *context);

/*
 * @brief:
 *     ���"taTaskBpDoIgnore"�����Ķϵ���������
 * @return:
 *     TA_OK����ɶϵ����
 */
T_TA_ReturnCode taSdaBpDoneIgnore(T_DB_ExceptionContext *context);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_BP_IGNORE_LIB_H_ */
