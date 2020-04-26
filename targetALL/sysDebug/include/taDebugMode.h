/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-23         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file:taDebugMode.h
 *@brief:
 *             <li>����ģʽ��غ�������</li>
 */
#ifndef _TA_DEBUG_MODE_H
#define _TA_DEBUG_MODE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ �� ��******************************/
#include "taTypes.h"
#include "taErrorDefine.h"

/************************�궨��*******************************/

/************************���Ͷ���*****************************/

/* ����ģʽ */
typedef enum
{
    DEBUGMODE_SYSTEM = 1,
    DEBUGMODE_TASK   = 2,
} T_TA_DebugMode;

/************************�ӿ�����*****************************/

/*
 * @brief:
 *     ��ȡTA��ǰ����ģʽ
 * @return:
 *     ����ģʽ
 */
T_TA_DebugMode taDebugModeGet(void);

/*
 * @brief:
 *     ����TA��ǰ����ģʽ
 * @param[in]:mode:����ģʽ
 * @return:
 *     TA_INVALID_DEBUG_MODE:��Ч�ĵ���ģʽ
 *     TA_OK:���õ���ģʽ�ɹ�
 */
T_TA_ReturnCode taDebugModeSet(T_TA_DebugMode mode);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _TA_DEBUG_MODE_H */
