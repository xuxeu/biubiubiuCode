/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-07-01         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ��� 
 */

/*
 *@file:taMsgDispatch.h
 *@brief:
 *             <li>ͨ�Ź�����Ϣ�ɷ�����</li>
 */
#ifndef _TA_MSG_DISPATCH_H
#define _TA_MSG_DISPATCH_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taErrorDefine.h"
#include "taTypes.h"
#include "taPacket.h"

/************************�궨��********************************/

/* ��������������*/
#define TA_MAX_AGENT_NUM  255

/************************���Ͷ���******************************/

/* ������Ϣ������*/
typedef T_TA_ReturnCode (*TaMsgProc)(T_TA_Packet* tpPacket);

/************************�ӿ�����******************************/

/*
 * @brief:
 *  	����ѭ��������Ϣ
 * @return:
 *      ��
 */
void taStartLoopDispatch(void);

/*
 * @brief:
 *      ֹͣѭ��������Ϣ
 * @return:
 *      ��
 */
void taStopLoopDispatch(void);

/*
 * @brief:
 *      ��װͨ��ͨ����Ϣ������
 * @param[in]:aid: ͨ����
 * @param[in]:proc: ��Ϣ������ָ��
 * @return:
 *     TA_OK: ��װ�ɹ�;
 *     TA_INVALID_ID: �����ͨ����,��ȷ��ͨ��ID�ŷ�ΧӦ����0~MAX_AGENT_NUM
 */
T_TA_ReturnCode taInstallMsgProc(UINT32 aid, TaMsgProc proc);

/**
 * @brief:
 *      ��Ϣ�����ɷ��������յ���Ϣ�������ݽ���ͨ��ID���ö�Ӧ��װ����Ϣ������������Ϣ�����д���
 * @return:
 *      TA_OK:�հ��ɹ�
 *      TA_FAIL:�հ�ʧ��
 */
T_TA_ReturnCode taMessageDispatch(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TA_MSG_DISPATCH_H */
