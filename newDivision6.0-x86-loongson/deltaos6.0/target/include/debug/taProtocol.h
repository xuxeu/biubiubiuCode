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
 *@file:taProtocol.h
 *@brief:
 *             <li>��Ϣ��ͨ�ų���Э�鶨��</li>
 */
#ifndef _TA_PROTOCOL_H
#define _TA_PROTOCOL_H

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

/* Э�������ṹ�� */
typedef struct T_TA_TREATY_OPS_Struct
{
    /* ��һ�������� */
    struct T_TA_TREATY_OPS_Struct *next;

    /* Э������ */
    T_TA_COMMType type;

    /* ���ݰ������ݷ��͵�ƫ���� */
    UINT32 offset;

    /* ָ�򷢰������ĺ���ָ�� */
    T_TA_ReturnCode (*putpkt)(T_TA_COMMIf *commIf, T_TA_Packet* tpPacket);

    /* ָ���հ������ĺ���ָ�� */
    T_TA_ReturnCode (*getpkt)(T_TA_COMMIf *commIf, T_TA_Packet* tpPacket);
} T_TA_TREATY_OPS;

/************************�ӿ�����******************************/

/**
 * @brief:
 *      ��Э��������������һ��Э�������
 * @param[in]:optable: ָ��Э��������ָ��
 * @return:
 *      TA_OK: ���Э��ɹ�
 *      TA_INVALID_PARAM:��Ч���������
 */
T_TA_ReturnCode taTreatyAdd(T_TA_TREATY_OPS *optable);

/**
 * @brief:
 *      ͨ������ͨ���������ݰ�
 * @param[in]: commIf: ��������TAͨ��Э��ջ�ṩ���豸��д�����ӿ�
 * @param[in]: packet: ��Ҫ���͵����ݰ�����
 * @return:
 *      TA_OK���������ݳɹ�
 *      TA_INVALID_PROTOCOL:Э�黹û�а�װ *
 *      TA_INVALID_PARAM: ����Ļ���ָ�벻�Ϸ�
 *      TA_FAIL:��������ʧ��
 */
T_TA_ReturnCode taPutpkt(T_TA_COMMIf *commIf, T_TA_Packet *packet);

/**
 * @brief:
 *      ͨ������ͨ���������ݰ����յ��κ���Ч���ݰ�����������
 * @param[in]:commIf:  ͨ���豸������
 * @param[out]:packet: ���ݰ�����
 * @return:
 *      TA_OK���������ݳɹ�
 *      TA_INVALID_PROTOCOL:Э�黹û�а�װ
 *      TA_INVALID_PARAM: �������ָ�벻�Ϸ�
 *      TA_FAIL:�հ�ʧ��
 */
T_TA_ReturnCode taGetpkt(T_TA_COMMIf *commIf, T_TA_Packet *packet);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_TA_PROTOCOL_H */
