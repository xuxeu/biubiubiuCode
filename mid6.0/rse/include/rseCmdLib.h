/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-07-26         ��Ԫ־�������������ɼ������޹�˾
 *                           �������ļ���
 */

/**
 * @file  rseCmdLib.h
 * @brief
 *       ���ܣ�
 *       <li>RSE������������</li>
 */
#ifndef RSE_CMDLIB_H_
#define RSE_CMDLIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ �� ��******************************/
#include "rseConfig.h"
#include "ta.h"

/************************�� �� ��******************************/

/* HASH TABLE���� */
#define RSE_MAX_TABLE    256
#define RSE_TYPEID_STAR  0x0300

/************************���Ͷ���*****************************/

/* ������Ϣ������ */
typedef T_TA_ReturnCode (*RseCommandHandler)(T_TA_Packet* tpPacket);

/************************�ӿ�����*****************************/

/*
 * @brief
 *      ע��ģ�鴦����
 *@param[in] moduleId: ģ��ID
 *@param[in] moduleHandler: ģ�鴦����
 *@return
 *      TA_INVALID_ID:��Чģ��ID
 *		TA_OK��ע��ɹ�
 */
T_TA_ReturnCode rseRegisterModuleHandler(UINT16 moduleId, RseCommandHandler moduleCommandHandler);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RSE_CMDLIB_H_ */
