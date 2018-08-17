/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  resRtpTaskLib.c
 * @brief
 *       ���ܣ�
 *       <li>RTP��ѯ</li>
 */

/************************ͷ �� ��******************************/
#include "rseConfig.h"
#include "rseCmdLib.h"
#include "rseRtpLib.h"
#include "taskLib.h"
#include "rtpLibP.h"
#include "rtpUtilLibP.h"
#include "rtpLibCommon.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/*
 * @brief
 *      �ṩRTP����Ϣ����
 * @param[in] packet: ���յ������ݰ�
 * @return
 *      None
 */
static T_TA_ReturnCode rseRtpHandler(T_TA_Packet* packet);

/*
 * @brief
 * 		��ȡ����RTP��Ϣ
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
static void rseGetRtpList(T_TA_Packet* packet,UINT32 *outSize);

/************************ģ�����*****************************/

/* RTP���� */
static UINT32 rtpIndex = 0;

/* ���RTP ID�б� */
static ulong_t rtpIdList[MAX_RTP_NUM];

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/**
  * @brief
  *     RTP��Ϣ��������ʼ��
  *@return
  *      None
  */
void rseRtpLibInit(void)
{
	/* ��װRTPģ�鴦���� */
	rseRegisterModuleHandler(RSE_TYPE_RTP, rseRtpHandler);
}

/*
 * @brief
 *      �ṩRTP����Ϣ����
 * @param[in] packet: ���յ������ݰ�
 * @return
 *      None
 */
static T_TA_ReturnCode rseRtpHandler(T_TA_Packet *packet)
{
	UINT16 moduleId = 0;
    UINT8 *inbuf = NULL;
    UINT8 *outbuf = NULL;
    UINT32 outSize = 0;

    /* �Ӱ��л�ȡʵ������ */
    outbuf = inbuf = taGetPktUsrData(packet);

	/* ȡģ��ID */
	n2s(inbuf, moduleId);

	switch(moduleId)
	{
		case RSE_TYPE_RTP_LIST:
			rseGetRtpList(packet, &outSize);
			break;
		case RSE_TYPE_RTP_INFO:
			rseGetRtpModuleInfo(packet, &outSize);
			break;
		case RSE_TYPE_RTP_TASK_LIST:
			rseGetRtpTaskList(packet, &outSize);
			break;
		case RSE_TYPE_RTP_SHL_LIST:
			rseGetRtpShlList(packet, &outSize);
			break;
		default:
			outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d", TA_INVALID_OPERATE_ID);
			break;
	}

	/* �������ݰ� */
	taCOMMSendPacket(packet, outSize);

	return TA_OK;
}

/*
 * @brief
 *      ��RTP ID �����б���
 * @param[in] rtpId: RTP ID
 * @param[in] rtpIdList: RTP ID�б�
 * @return
 *      None
 */
void rseGetRtpId(ulong_t rtpId, ulong_t rtpIdList[])
{
	rtpIdList[rtpIndex++] = rtpId;
}

/*
 * @brief
 * 		��ȡϵͳ��RTP�б�
 * @return
 *      RTP����
 */
int rseGetRtpIdList(ulong_t rtpIdList[])
{
	rtpIndex = 0;

	/* ��ȡRTP�����б� */
	objEach(windRtpClass, (void*)rseGetRtpId, (ulong_t)rtpIdList, 0);

    return rtpIndex;
}

/*
 * @brief
 *      ��ȡRTP״̬�ַ���
 * @param[in] rtpId: RTP ID
 * @param[out] rtpStatusStr: ״̬�ַ���
 * @return
 *      None
 */
void rseGetRtpStatusStr(RTP_ID rtpId, char *rtpStatusStr)
{
    char *statusStr = NULL;

    switch (RTP_STATE_GET(rtpId->status))
    {
    	case RTP_STATE_CREATE:
        {
        	if (IS_RTP_STATUS_STOP(rtpId->status))
        	{
                statusStr = "STATE_CREATE+S";
        	}
        	else if (IS_RTP_STATUS_ELECTED_DELETER(rtpId->status))
        	{
                statusStr = "STATE_CREATE+D";
        	}
        	else
        	{
                statusStr = "STATE_CREATE  ";
        	}
            break;
        }
        case RTP_STATE_NORMAL:
        {
        	if (IS_RTP_STATUS_STOP (rtpId->status))
        	{
                statusStr = "STATE_NORMAL+S";
        	}
        	else if (IS_RTP_STATUS_ELECTED_DELETER (rtpId->status))
        	{
                statusStr = "STATE_NORMAL+D";
        	}
        	else
        	{
                statusStr = "STATE_NORMAL  ";
        	}
            break;
        }
        case RTP_STATE_DELETE:
        {
            if (IS_RTP_STATUS_STOP (rtpId->status))
            {
                statusStr = "STATE_DELETE+S";
            }
            else if (IS_RTP_STATUS_ELECTED_DELETER (rtpId->status))
            {
                statusStr = "STATE_DELETE+D";
            }
            else
            {
                statusStr = "STATE_DELETE  ";
            }
            break;
        }
        default:
        {
            statusStr = "STATE_UNKNOWN ";
            break;
        }
    }

    sprintf((INT8 *)rtpStatusStr, "%s", statusStr);
}

/*
 * @brief
 * 		��ȡ����RTP��Ϣ
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
static void rseGetRtpList(T_TA_Packet* packet,UINT32 *outSize)
{
	UINT8 *inbuf = NULL;
	UINT8 *outbuf = NULL;
	UINT8 *ptr = NULL;
	UINT32 len = 0;
	ulong_t rtpId = 0;
	char rtpName[RSE_MAX_NAME_LEN+1] = {0};
	char rtpStatus[RSE_MAX_NAME_LEN+1] = {0};
	static UINT32 count = 0;
	static UINT32 idx = 0;

	/* �Ӱ��л�ȡʵ������ */
	inbuf = taGetPktUsrData(packet);

	/* �����������outbufΪinbuf */
	ptr = outbuf = inbuf;

    /* ��ȡRTP�������б� */
	
    if(count == 0)
    {
    	count = rseGetRtpIdList(rtpIdList);
    }

    /* ������ݰ� */
    memset(ptr,0, TA_PKT_DATA_SIZE);

	 /* ����ѭ���ɼ�����RTP������Ϣ */
	while (idx < count)
	{
    	/* �ȼ��ʣ������Ƿ���Դ洢������Ϣ */
    	RSE_CHECK_OVER_PKTSIZE(ptr, outbuf, RSE_RTP_MAX_INFO_LEN);

		/* ȡ��RTP ID */
		rtpId = rtpIdList[idx++];

    	/* У��RTP ID�Ƿ���Ч */
        if (RTP_ID_VERIFY((WIND_RTP	*)rtpId) != OK)
        {
        	continue;
        }

        /* ���ûظ���� */
        len = sprintf((INT8 *)ptr,"%s","OK");
        ptr += (len+2);

		/* д������ID */
		RSE_PUTPKT_SHORT(RSE_TYPE_RTP_LIST, ptr);

		/* д�����ID */
		RSE_PUTPKT_LONG(rtpId, ptr);

		/* д���־λ */
		*ptr = 1;
		ptr += 1;

		/* ���RTP�� */
		ZeroMemory(rtpName,RSE_MAX_NAME_LEN+1);

		/* д��Ҫ��ʾ��ǩ :RTP��*/
		rsePathTruncate(((RTP_ID)rtpId)->pPathName,rtpName,RSE_MAX_NAME_LEN);
		len = sprintf((INT8 *)ptr,"%s", rtpName);
		ptr += (len+1);

		/* д��������� */
		*ptr = RSE_NULL_OPER;
		ptr += 1;

		/* д�����Ը��� */
		*ptr = 3;
		ptr += 1;

		/* д�����Ա�,RTP ID��RTP����RTP״̬ */
		RSE_PUTPKT_SHORT(RSE_ATTR_ID, ptr);
	#ifdef CONFIG_TA_LP64
		len = sprintf((INT8 *)ptr,"%lx", rtpId);
	#else
		len = sprintf((INT8 *)ptr,"%x", rtpId);
	#endif
		ptr += (len+1);

		/* д������������ */
		RSE_PUTPKT_SHORT(RSE_ATTR_NAME, ptr);
		len = sprintf((INT8 *)ptr,"%s", rtpName);
		ptr += (len+1);

		/* ���״̬�ַ��� */
		ZeroMemory(rtpStatus,RSE_MAX_NAME_LEN+1);

		/* д������״̬���� */
		RSE_PUTPKT_SHORT(RSE_ATTR_STATE, ptr);
		rseGetRtpStatusStr((RTP_ID)rtpId, rtpStatus);
		len = sprintf((INT8 *)ptr,"%s",rtpStatus);
		ptr += (len+1);
	}

    /* �����������Ϊ "$",��ʾRTP��Ϣ������� */
    if(idx == count)
    {
    	*ptr = RSE_END_STRING;
    	ptr += 1;
    	idx = count = 0;
    }
    /* ��������������ݴ�С */
    *outSize = ptr - outbuf;
}
