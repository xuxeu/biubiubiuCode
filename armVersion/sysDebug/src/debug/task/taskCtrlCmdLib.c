/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-08-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:rtpCtrlCmdLib.c
 * @brief:
 *             <li>�󶨡����ģʽ�л��ȿ�������ʵ��</li>
 */

/************************ͷ �� ��*****************************/
#include "taRtIfLib.h"
#include "taskSessionLib.h"
#include "taDebugMode.h"
#include "taskCtrLib.h"
#include "ta.h"

/************************�� �� ��******************************/

/* ����������� */
#define TA_CTRL_COMMAND_NUM 5

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/*
 * @brief:
 * 		���Կ��ƴ�����Ϣ��������
 * @param[in]:packet: Ҫ�������Ϣ��
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_COMMAND_NOTFOUND: ����û�ҵ�
 *     TA_INVALID_PARAM: �Ƿ��Ĳ���
 */
static T_TA_ReturnCode taCtrlCommandProc(T_TA_Packet *packet);

/*
 * @brief:
 *      ������,�����ʽ"attach:objId:ctxType","objId"Ϊ����ID����ʵʱ����ID,"ctxType"Ϊ��������������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode taCtxAttach(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      �������,�����ʽ"detach:cid:ctxType","cid"Ϊͨ��ID,"ctxType"Ϊ��������������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode taCtxDetach(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      ��ӵ��԰����񵽵��ԻỰ�У����������ڰ󶨶�����񵽵��ԻỰ��,
 *      �����ʽ"taskAttach:cid:tid","cid"Ϊͨ��ID,"tid"Ϊ����ID
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode taCtxAttachTask(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      ģʽ�л�����,�����ʽ"SMD:mode","mode"����ģʽ
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode taModeSwitch(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/************************ģ�����*****************************/

/* ��ѯ����� */
static T_TA_CommandTable taCtrlCommandTable[TA_CTRL_COMMAND_NUM] =
{
	{"attach",      taCtxAttach,           TA_NEED_REPLY},/* ������ */
    {"detach",      taCtxDetach,           TA_NEED_REPLY},/* ������� */
    {"taskAttach",  taCtxAttachTask,       TA_NEED_REPLY},/* ���������� */
    {"SMD",         taModeSwitch,          TA_NEED_REPLY},/* ����ģʽ�л����� */
    {NULL,          NULL,                  TA_NO_REPLY  }
};
/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/

/*
 * @brief:
 *     ��װ�����������
 * @return:
 *     ��
 */
void taCtrlCommandInit(void)
{
	/* ��װ����������� */
	taInstallMsgProc(TA_CTRL_COMMAND, taCtrlCommandProc);
}

/*
 * @brief:
 * 		���Կ��ƴ�����Ϣ��������
 * @param[in]:packet: Ҫ�������Ϣ��
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_COMMAND_NOTFOUND: ����û�ҵ�
 *     TA_INVALID_PARAM: �Ƿ��Ĳ���
 */
static T_TA_ReturnCode taCtrlCommandProc(T_TA_Packet *packet)
{
    /* ����taAgentEntry�����յ�����Ϣ�� */
    return (taAgentEntry(packet,NULL,COMMANDTYPE_NONE,taCtrlCommandTable,
    		TA_CTRL_COMMAND_NUM,NULL,NULL));
}

/*
 * @brief:
 *      ������,�����ʽ"attach:objId:ctxType","objId"Ϊ����ID����ʵʱ����ID,"ctxType"Ϊ��������������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode taCtxAttach(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 sid = 0;
    ulong_t objId = 0;
    TA_CTX_TYPE ctxType = 0;
    const UINT8 *ptr = NULL;
    UINT32 ret = TA_OK;

    /* ������������������� */
    ptr = &inbuf[6];

    /* ��"attach:objId:ctxType"��ʽ����������� */
#ifdef CONFIG_TA_LP64
	if (!((*(ptr++) == ':') && (0 != hex2int64(&ptr, &objId) && (*(ptr++) == ':') && (0 != hex2int(&ptr,&ctxType)))))
	{
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
	}
#else
	if (!((*(ptr++) == ':') && (0 != hex2int(&ptr, &objId) && (*(ptr++) == ':') && (0 != hex2int(&ptr,&ctxType)))))
	{
		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
		return (TA_FAIL);
	}
#endif

	/* ���԰� */
	ret = TA_RT_CTX_ATTACH(ctxType,objId,&sid);
	if (TA_OK != ret)
	{
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",ret);
        return (TA_FAIL);
	}

    /* �����������Ϊ "OK:channel",channelΪ����ͨ���� */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK:%x", TA_GET_CHANNEL_ID(sid));

	return (TA_OK);
}

/*
 * @brief:
 *      �������,�����ʽ"detach:cid:ctxType","cid"Ϊͨ��ID,"ctxType"Ϊ��������������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode taCtxDetach(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 cid = 0;
    UINT32 ret = 0;
    TA_CTX_TYPE ctxType = 0;
    const UINT8 *ptr = NULL;

    /* ������������������� */
    ptr = &inbuf[6];

    /* ��"detach:cid:ctxType"��ʽ����������� */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&cid)) && (*(ptr++) == ':') && (0 != hex2int(&ptr,&ctxType))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* ���Խ�� */
    ret = TA_RT_CTX_DETACH(ctxType,TA_GET_SESSION_ID(cid));
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",ret);
        return (TA_FAIL);
    }

    /* �����������Ϊ "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      ��ӵ��԰����񵽵��ԻỰ�У����������ڰ󶨶�����񵽵��ԻỰ��,�����ʽΪ
 *      "taskAttach:cid:tid","cid"Ϊͨ��ID,"tid"Ϊ����ID
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode taCtxAttachTask(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 cid = 0;
    UINT32 ret = 0;
    ulong_t tid = 0;
    const UINT8 *ptr = NULL;

    /* ������������������� */
    ptr = &inbuf[10];


#ifdef CONFIG_TA_LP64
    /* ��"taskAttach:cid:tid"��ʽ����������� */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&cid)) && (*(ptr++) == ':') && (0 != hex2int64(&ptr,&tid))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#else
    /* ��"taskAttach:cid:tid"��ʽ����������� */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&cid)) && (*(ptr++) == ':') && (0 != hex2int(&ptr,&tid))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#endif


    /* ������ */
    ret = taAttachTask(TA_GET_SESSION_ID(cid), tid);
    if (TA_OK != ret)
    {
    	*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",ret);
    	return (TA_FAIL);
    }

    /* �����������Ϊ "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      ģʽ�л�����,�����ʽ"SMD:mode","mode"����ģʽ
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode taModeSwitch(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 ret = 0;
    UINT32 debugMode = 0;
    const UINT8 *ptr = NULL;

    /* ������������������� */
    ptr = &inbuf[3];

    /* ��"SMD:mode"��ʽ����������� */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&debugMode))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* ���õ���ģʽ */
    ret = taDebugModeSet(debugMode);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",ret);
        return (TA_FAIL);
    }

    /* �����������Ϊ "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OKkkk");

    return (TA_OK);
}
