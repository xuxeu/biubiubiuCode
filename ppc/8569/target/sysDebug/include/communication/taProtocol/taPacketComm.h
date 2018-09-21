/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-26         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ��� 
 */

/*
 *@file:taPacketComm.h
 *@brief:
 *             <li>��Ϣͨ�Žӿڶ���</li>
 */
#ifndef _TA_PACKETCOMM_H
#define _TA_PACKETCOMM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "taPacket.h"
#include "taDevice.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

/**
 * @brief:
 *      �豸�������ʼ��
 * @param[in]:interface: �豸������
 * @return:
 *        TA_OK:�����ɹ�
 *        TA_INVALID_PARAM:��Ч���������
 */
T_TA_ReturnCode taCOMMInit(T_TA_COMMIf *interface);

/*
 * @brief:
 *      ������Ϣ������
 * @param[in]:packet:�洢�յ�����Ϣ��ָ��
 * @return:
 *      ͬtaGetpkt�ķ���ֵ
 */
T_TA_ReturnCode taCOMMGetPkt(T_TA_Packet *packet);

/*
 * @brief:
 *      ������Ϣ������
 * @param[in]:packet:���͵���Ϣ��ָ��
 * @return:
 *      ͬtaPutpkt�ķ���ֵ
 */
T_TA_ReturnCode taCOMMPutPkt(T_TA_Packet *packet);

/*
 * @brief:
 *      ���ð���С��������Ϣ��
 * @param[in]:packet :Ҫ���͵ĵ���Ϣ��ָ��
 * @param[in]:packetSize :����С
 * @return:
 *      taCOMMPutPkt�ķ���ֵ
 */
T_TA_ReturnCode taCOMMSendPacket(T_TA_Packet *packet,UINT32 packetSize);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /*_TA_PACKETCOMM_H */
