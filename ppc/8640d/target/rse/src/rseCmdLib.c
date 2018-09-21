/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-05-26         ��Ԫ־�������������ɼ������޹�˾
 *                           �������ļ���
 */

/**
 * @file  rseCmdLib.c
 * @brief
 *       ���ܣ�
 *       <li>RSE�����</li>
 */

/************************ͷ �� ��******************************/
#include "rseConfig.h"
#include "rseCmdLib.h"
#include "rseInitLib.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/* ģ�麯������� */
static RseCommandHandler rseModuleCommandHandlerTable[RSE_MAX_TABLE] = {0};

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/
/*
 * @brief
 *      ��ȡ����ģ�鴦����
 *@param[in] inbuf: ������е�ģ��id
 *@return
 *     ��ø�ģ�鴦����
 */
static RseCommandHandler rseGetModuleHandler(UINT8* inbuf)
{
    UINT32 idx = 0;
    UINT16 moduleId = 0;

    /* ȡģ��ID */
    n2s(inbuf,moduleId);

    /* ����ģ��ID����ģ�鴦���������� */
    idx = moduleId - RSE_TYPEID_STAR;
    idx >>= 8;
    idx &= 0xff;

    /* ��ȡģ�鴦����  */
    return (rseModuleCommandHandlerTable[idx]);
}

/*
 * @brief
 *      ע��ģ�鴦����
 *@param[in] moduleId: ģ��ID
 *@param[in] moduleHandler: ģ�鴦����
 *@return
 *      TA_INVALID_ID:��Чģ��ID
 *		TA_OK��ע��ɹ�
 */
T_TA_ReturnCode rseRegisterModuleHandler(UINT16 moduleId, RseCommandHandler moduleCommandHandler)
{
    UINT32 idx = 0;

    /* ����ģ�鴦����HASH������ */
    idx = moduleId - RSE_TYPEID_STAR;
    idx >>= 8;
    idx &= 0xff;

    if ((idx > RSE_MAX_TABLE)||((idx != 0) && (NULL != rseModuleCommandHandlerTable[idx])))
    {
        return (TA_INVALID_ID);
    }

    rseModuleCommandHandlerTable[idx] = moduleCommandHandler;

    return (TA_OK);
}

/*
 * @brief:
 * 		RSE��Ϣ��������
 * @param[in]:packet: Ҫ�������Ϣ��
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_FAIL������ִ��ʧ��
 */
static T_TA_ReturnCode rseCommandHandler(T_TA_Packet *packet)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT32 outSize = 0;
    RseCommandHandler moduleCommandHandler = NULL;

    /* ��ת��ͷ packet */
    taSwapSenderAndReceiver(&(packet->header));

    /* �Ӱ��л�ȡʵ������ */
    inbuf = taGetPktUsrData(packet);

    /* �����������outbufΪinbuf */
    outbuf = inbuf;

    /* ��ȡģ�鴦���� */
    moduleCommandHandler = rseGetModuleHandler(inbuf);
    if (NULL == moduleCommandHandler)
    {
    	outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d",TA_INVALID_MODULE_ID);
		taCOMMSendPacket(packet, outSize);
        return (TA_FAIL);
    }

    /* ���ô����� */
    moduleCommandHandler(packet);

    return TA_OK;
}

/*
 * @brief:
 *     ��װRSE��Ϣ������
 * @return:
 *     ��
 */
void rseCommandInit(void)
{
	/* ��װ��Ϣ������ */
	taInstallMsgProc(TA_RSE_AGENT, rseCommandHandler);
}
