/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:taPacketComm.c
 * @brief:
 *             <li>��Ϣͨ���շ��������������ṩ���շ��������ӿ�</li>
 */

/************************ͷ �� ��*****************************/
#include "taPacketComm.h"
#include "taProtocol.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/*��ǰ�豸������*/
static T_TA_COMMIf *taCurCOMMIf = NULL;

/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/

/**
 * @brief:
 *      �豸�������ʼ��
 * @param[in]:interface: �豸������
 * @return:
 *        TA_OK:�����ɹ�
 *        TA_INVALID_PARAM:��Ч���������
 */
T_TA_ReturnCode taCOMMInit(T_TA_COMMIf *interface)
{
    /* �����Ϸ��Լ�� */
    if (( NULL == interface) || (NULL == interface->read) || (NULL == interface->write))
    {
        return (TA_INVALID_PARAM);
    }

    /* ���õ�ǰ�豸������ */
    taCurCOMMIf = interface;

    return (TA_OK);
}

/*
 * @brief:
 *      ������Ϣ������
 * @param[in]:packet:�洢�յ�����Ϣ��ָ��
 * @return:
 *      ͬtaGetpkt�ķ���ֵ
 */
T_TA_ReturnCode taCOMMGetPkt(T_TA_Packet *packet)
{
    /* ����taGetpkt����������Ϣ�� */
    return (taGetpkt(taCurCOMMIf, packet));
}

/*
 * @brief:
 *      ������Ϣ������
 * @param[in]:packet:���͵���Ϣ��ָ��
 * @return:
 *      ͬtaPutpkt�ķ���ֵ
 */
T_TA_ReturnCode taCOMMPutPkt(T_TA_Packet *packet)
{
    /* ����taGetpkt����������Ϣ�� */
    return (taPutpkt(taCurCOMMIf, packet));
}

/*
 * @brief:
 *      ���ð���С��������Ϣ��
 * @param[in]:packet :Ҫ���͵ĵ���Ϣ��ָ��
 * @param[in]:packetSize :����С
 * @return:
 *      taCOMMPutPkt�ķ���ֵ
 */
T_TA_ReturnCode taCOMMSendPacket(T_TA_Packet *packet,UINT32 packetSize)
{
    /* ������Ϣ���Ĵ�СΪ */
    taSetPktSize(packet, packetSize);

    /* ����taCOMMPutPkt����������Ϣ�� */
    return (taCOMMPutPkt(packet));
}
