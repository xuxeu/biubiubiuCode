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
 * @file:taProtocol.c
 * @brief:
 *             <li>��Ϣͨ�ų���Э��ʵ��</li>
 */

/************************ͷ �� ��*****************************/
#include "taProtocol.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/* ָ��Э�����������ָ�� */
static T_TA_TREATY_OPS *taTreatyOpsList = NULL;

/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/
/*
 * @brief:
 *      ��Э�����������ָ��Э���Э�������
 * @param[in]: protocolType: Э������
 * @return:
 *      ops: Э��������ָ��
 *      NULL: δ�ҵ�Э�������
 */
static T_TA_TREATY_OPS* taTreatyLookup(T_TA_COMMType protocolType)
{
    T_TA_TREATY_OPS *ops = taTreatyOpsList;

    /* ��Э����������б�������ָ��Э�����͵�Э������� */
    for (ops = taTreatyOpsList; NULL != ops; ops = ops->next)
    {
        if (protocolType == ops->type)
        {
            return (ops);
        }
    }

    return (NULL);
}

/**
 * @brief:
 *      ��Э��������������һ��Э�������
 * @param[in]:optable: ָ��Э��������ָ��
 * @return:
 *      TA_OK: ���Э��ɹ�
 *      TA_INVALID_PARAM:��Ч���������
 */
T_TA_ReturnCode taTreatyAdd(T_TA_TREATY_OPS *optable)
{
    /* �����Ϸ��Լ�� */
    if (( NULL == optable )
	    || ( NULL == optable->getpkt )
	    || ( NULL == optable->putpkt )
	    || (optable->offset > TA_PKT_DATA_SIZE))
    {
        return (TA_INVALID_PARAM);
    }

    /* ��Э�������������� */
    optable->next = taTreatyOpsList;

    taTreatyOpsList = optable;

    return (TA_OK);
}

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
T_TA_ReturnCode taPutpkt(T_TA_COMMIf *commIf, T_TA_Packet *packet)
{
    T_TA_TREATY_OPS *ops = NULL;
	T_TA_ReturnCode ret = TA_OK;
	
    /* �����Ϸ��Լ��δͨ�� */
    if ((NULL == packet) || (NULL == commIf))
    {
         return (TA_INVALID_PARAM);
    }

    /* ����Э�����Ͳ���Э������� */
    ops = taTreatyLookup(commIf->commType);
    if (NULL == ops)
    {
        return (TA_INVALID_PROTOCOL);
    }

    /* ���÷���������������Ϣ�� */
    ret = ops->putpkt(commIf, packet);

    return (ret);
}

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
T_TA_ReturnCode taGetpkt(T_TA_COMMIf *commIf, T_TA_Packet *packet)
{
    T_TA_TREATY_OPS* ops = NULL;
	T_TA_ReturnCode ret = TA_OK;

    /* �����Ϸ��Լ�� */
    if ((NULL == packet) || (NULL == commIf))
    {
        return (TA_INVALID_PARAM);
    }

    /* ����Э�����Ͳ���Э������� */
    ops = taTreatyLookup(commIf->commType);
    if (NULL == ops)
    {
        return (TA_INVALID_PROTOCOL);
    }

    /* �����հ�������������Ϣ�� */
    ret = ops->getpkt(commIf, packet);
    
    return (ret);
}
