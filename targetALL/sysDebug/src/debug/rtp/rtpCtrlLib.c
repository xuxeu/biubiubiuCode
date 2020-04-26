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
 * @file:rtpCtrlLib.c
 * @brief:
 *             <li>rtp���Ʋ�������Ҫʵ��RTP���԰󶨺�RTP���Խ��</li>
 */

/************************ͷ �� ��*****************************/
#include "taRtIfLib.h"
#include "taskCtrLib.h"
#include "taRtp.h"
#include "taskSessionLib.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/

/*
 * @brief:
 *      ��RTP��������
 * @param[in]: rtpId: ʵʱ����ID
 * @param[out]: sid: �ỰID
 * @return:
 *     TA_OK: ��RTP��������ɹ�
 *     TA_INVALID_RTP_ID����Чʵʱ����ID
 */
static UINT32 taRtpAllTaskAttach(RTP_ID rtpId, UINT32 sid)
{
    WIND_TCB *tid = 0;

	//int taInitTaskId = 0;
	//taInitTaskId = (((WIND_RTP *)rtpId)->initTaskId);


    if (RTP_VERIFIED_LOCK(rtpId) != OK)
    {
    	return (TA_INVALID_RTP_ID);
    }

    FOR_EACH_TASK_OF_RTP(rtpId, tid);
    taAttachTask(sid, (INT32)tid);
    END_FOR_EACH_TASK_OF_RTP;

    RTP_UNLOCK (rtpId);


    return (TA_OK);
}

/*
 * @brief:
 *      ���񼶵��԰�RTP
 * @param[in]: ctxType: ����������
 * @param[in]: objId: ���Զ���ID
 * @param[out]: pSid: �ỰID
 * @return:
 *     TA_OK: ��RTP�ɹ�
 *     TA_INVALID_RTP_ID����ЧID
 *     TA_ALLOCATE_SESSION_FAIL��������ԻỰʧ��
 */
UINT32 taRtpTaskAttach(TA_CTX_TYPE ctxType, UINT32 objId, UINT32 *pSid)
{
    T_TA_ReturnCode ret = TA_OK;

    /* У��RTP ID�Ƿ���Ч */
	if (OK != TA_RT_CTX_ID_VERIFY(ctxType,objId))
	{
		return (TA_INVALID_RTP_ID);
	}

	/* ������ԻỰ */
	ret = taSessionMalloc(ctxType, objId, pSid);
	if (TA_OK != ret)
	{
		return (TA_ALLOCATE_SESSION_FAIL);
	}

	/* �󶨵�ǰRTP�������� */
	taRtpAllTaskAttach((RTP_ID)objId, *pSid);

	/* ��װ���񼶵���������� */
	taInstallMsgProc(TA_GET_CHANNEL_ID(*pSid), taTaskDebugAgent);

    return (TA_OK);
}

/*
 * @brief:
 *      ���񼶵��Խ��RTP
 * @param[in]: sid: �ỰID
 * @return:
 *     TA_OK: ���RTP�ɹ�
 */
UINT32 taRtpTaskDetach(UINT32 sid)
{
	/* ж�����񼶵���������� */
	taInstallMsgProc(TA_GET_CHANNEL_ID(sid), NULL);

    /* �ͷŵ��ԻỰ */
	taSessionFree(sid);

    return (TA_OK);
}

/*
 * @brief:
 *      ϵͳ�����԰�RTP
 * @param[in]: ctxType: ����������
 * @param[in]: objId: ���Զ���ID
 * @param[out]: pSid: �ỰID
 * @return:
 *     TA_OK: ��RTP�ɹ�
 *     TA_INVALID_RTP_ID����ЧID
 */
UINT32 taRtpSystemAttach(TA_CTX_TYPE ctxType, UINT32 objId, UINT32 *pSid)
{
    /* У��RTP ID�Ƿ���Ч */
	if (OK != TA_RT_CTX_ID_VERIFY(ctxType,objId))
	{
		return (TA_INVALID_RTP_ID);
	}

    /* ���õ��Զ���ID */
	taSDASessInfo.sessInfo.objID = objId;

	/* �������������� */
	taSDASessInfo.sessInfo.ctxType = ctxType;

	/* ����Ϊ��״̬ */
	taSDASessInfo.sessInfo.attachStatus = DEBUG_STATUS_ATTACH;

	/* ���õ�ǰ�߳�IDΪ��ʼ������ID */
	taSDASessInfo.sessInfo.curThreadId = (((WIND_RTP *)objId)->initTaskId);

    return (TA_OK);
}

/*
 * @brief:
 *      ϵͳ�����Խ��RTP
 * @param[in]: sid: �ỰID
 * @return:
 *     TA_OK: ���RTP�ɹ�
 */
UINT32 taRtpSystemDetach(UINT32 sid)
{
	/* ������Զ���ID */
	taSDASessInfo.sessInfo.objID = 0;

	/* �������������� */
	taSDASessInfo.sessInfo.ctxType = TA_CTX_SYSTEM;

	/* ����Ϊ���״̬ */
	taSDASessInfo.sessInfo.attachStatus = DEBUG_STATUS_DETACH;

	/* ���õ�ǰ�߳�Ϊ0 */
	taSDASessInfo.sessInfo.curThreadId = 0;

    return (TA_OK);
}
