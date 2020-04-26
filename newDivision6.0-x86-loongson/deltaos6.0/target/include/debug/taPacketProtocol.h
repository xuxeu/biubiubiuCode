/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file:taPacketProtocol.h
 *@brief:
 *             <li>��Ϣ��ͨ�ų���Э�鶨��</li>
 */
#ifndef _TA_PACKET_PROTOCOL_H
#define _TA_PACKET_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/

/************************�궨��********************************/

/* ͬ������Χ */
#define TA_SYNC_RANGE 	((UINT32)(0x1))

/* �������кŷ�Χ */
#define TA_TX_PACKET_MAX ((UINT32)(0x3fffffff))
#define TA_TX_PACKET_MIN ((UINT32)(0x00000001))

/* �����кŷ�Χ */
#define TA_RX_PACKET_MAX TA_TX_PACKET_MAX
#define TA_RX_PACKET_MIN TA_TX_PACKET_MIN

/* Ŀ�������ʼ����, ������� */
#define TA_RX_START ((UINT32)(0x40000002))
#define TA_TX_START ((UINT32)(0x40000001))

/* ��������ʼ���͵���� */
#define TA_TS_TX_START (UINT32)0x40000001

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

/*
 * @brief
 *   UDP ��ϢЭ��ĳ�ʼ��,��Э����������������ϢЭ�������
 * @return:
 *    ��
 */
void taTreatyUDPInit(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_TA_PACKET_PROTOCOL_H */
