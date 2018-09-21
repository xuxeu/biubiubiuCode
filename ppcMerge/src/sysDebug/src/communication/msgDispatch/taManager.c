/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:taManager.c
 * @brief:
 *             <li>��Ϣ����ģ��ʵ��</li>
 */

/************************ͷ �� ��******************************/
#include "string.h"
#include "taManager.h"
#include "taCommand.h"
#include "taMsgDispatch.h"
#include "taDebugMode.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/
/* ��ѯ��С����Ϣ */
static T_TA_ReturnCode taQueryEndian(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/* ����Ŀ��� */
static T_TA_ReturnCode taLinkProcess(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/* ��ѯĿ�����ϵ�ṹ */
static T_TA_ReturnCode taQueryArch(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/* ��ѯ��ǰ����ģʽ */
static T_TA_ReturnCode taQueryMode(T_DB_Info* debugInfo,
                                const UINT8 *inbuf,
                                UINT32 inSize,
                                UINT8 *outbuf,
                                UINT32 *outSize);

/************************ģ�����******************************/

/************************ȫ�ֱ���******************************/

/* ��ѯ����� */
T_TA_CommandTable taManagerCommandTable[TA_MANAGER_COMMANDNUM] =
{
    {"E",   taQueryEndian, TA_NEED_REPLY},/* ��ѯ��С�� */
    {"e",   taLinkProcess, TA_NEED_REPLY},/* ����Ŀ��� */
    {"AC",  taQueryArch,   TA_NEED_REPLY},/* ��ѯ��ϵͳ�ṹ */
    {"MD",  taQueryMode,   TA_NEED_REPLY},/* ��ѯ��ǰ����ģʽ */
    {NULL,  NULL,          TA_NO_REPLY  }
};

/************************ʵ��******************************/

/*
 * @brief:
 * 		��ѯͨ����ں���
 * @param[in]:packet: Ҫ�������Ϣ��
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_FAIL: ����ִ��ʧ��
 *     TA_COMMAND_NOTFOUND: ����û�ҵ�
 */
static T_TA_ReturnCode taManagerAgent(T_TA_Packet *packet)
{
    /* ����taAgentEntry�����յ�����Ϣ�� */
    return (taAgentEntry(packet,NULL,COMMANDTYPE_QUERY,taManagerCommandTable,TA_MANAGER_COMMANDNUM,NULL,NULL));
}

/*
 * @brief:
 *     ��ʼ��TA��ѯģ��
 * @return:
 *     ��
 */
void taManagerInit(void)
{
    /* ��װ��ѯͨ��TA_MANAGER����Ϣ������ */
    taInstallMsgProc(TA_MANAGER, taManagerAgent);
}

/*
 * @brief:
 *      �����ѯ��С����Ϣ��������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 */
static T_TA_ReturnCode taQueryEndian(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
	/* ��ȡ��С����Ϣ */
	taEndianInfoGet(outbuf,outSize);

    return(TA_OK);
}

/*
 * @brief:
 *      ����Ŀ���
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 */
static T_TA_ReturnCode taLinkProcess(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    /* �����������Ϊ"OK" */
    strcpy((char *)outbuf, "OK");

    /* ������������е��ַ������� */
    *outSize = strlen((const char *)outbuf);

    return(TA_OK);
}

/*
 * @brief:
 *      ��ѯĿ�����ϵ�ṹ����
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 */
static T_TA_ReturnCode taQueryArch(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    /* ��ȡĿ�����ϵ�ṹ��Ϣ */
    taArchInfoGet(outbuf,outSize);

    return (TA_OK);
}

/*
 * @brief:
 *     ��ѯTA��ǰ����ģʽ��������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: ִ�гɹ�
 */
static T_TA_ReturnCode taQueryMode(T_DB_Info* debugInfo,
                                const UINT8 *inbuf,
                                UINT32 inSize,
                                UINT8 *outbuf,
                                UINT32 *outSize)
{
    /* ��ǰΪϵͳ������ */
    if (DEBUGMODE_SYSTEM == taDebugModeGet())
    {
        strcpy((char *)outbuf, "OK;system");
    }
    else
    {
        /* ��ǰΪ���񼶵��� */
        strcpy((char *)outbuf, "OK;task");
    }

    /* ������������е��ַ������� */
    *outSize = strlen((const char *)outbuf);

    return (TA_OK);
}
