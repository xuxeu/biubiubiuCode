/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  rseRtpShlLib.c
 * @brief
 *       ���ܣ�
 *       <li>��ѯRTP�����Ĺ����</li>
 */

/************************ͷ �� ��******************************/
#include "rseConfig.h"
#include "taskLib.h"
#include "rseCmdLib.h"
#include "rseInitLib.h"
#include "rtpLibP.h"
#include "rtpUtilLibP.h"
#include "rtpLibCommon.h"
#include "shlLibP.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/* ��ʾ����������� */
#define RSE_MAX_DSP_SHLS 200

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief
 * 		��ȡRTP�����Ĺ����ID�б�
 * @param[in] rtpId: RTP ID
 * @return
 *      ��������
 */
int rseRtpShlIdList(RTP_ID rtpId, RTP_SHL_NODE *shlListId[])
{
	RTP_SHL_NODE *pRtpShlNode;
	int count = 0;

	pRtpShlNode = (RTP_SHL_NODE *) DLL_FIRST(&rtpId->rtpShlNodeList);

    while (pRtpShlNode != NULL)
    {
    	shlListId[count++] = pRtpShlNode;
    	pRtpShlNode = (RTP_SHL_NODE *) DLL_NEXT(pRtpShlNode);
	}

    return count;
}

/*
 * @brief
 * 		����RTP ID��ȡRTP�����
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
void rseGetRtpShlList(T_TA_Packet *packet,UINT32 *outSize)
{
	UINT8 *inbuf = NULL;
    UINT8 *outbuf = NULL;
	UINT8 *ptr = NULL;
	UINT8 *buf = NULL;
    UINT32 len = 0;
	UINT32 rtpId = 0;
	RTP_SHL_NODE *pRtpShlNode = NULL;
    char shlName[RSE_MAX_NAME_LEN+1] = {0};
    static UINT32 count = 0;
    static UINT32 idx = 0;
    static RTP_SHL_NODE *shlIdList[RSE_MAX_DSP_SHLS] = {0};

	 /* �Ӱ��л�ȡʵ������ */
	inbuf = taGetPktUsrData(packet);

	/* ������ʼ��ַ */
	buf = &inbuf[RSE_ARG_START_OFFSET];

	/* �����������outbufΪinbuf */
	ptr = outbuf = inbuf;

	/* ��ȡRTP ID */
	n2l(buf,rtpId);

    /* ��ȡRTP�Ĺ�����б� */
    if(count == 0)
    {
    	/* У��RTP ID�Ƿ���Ч */
        if(RTP_VERIFIED_LOCK((WIND_RTP	*)rtpId) != OK)
        {
        	*outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d",TA_INVALID_INPUT);
        	return;
        }

        RTP_UNLOCK((WIND_RTP*)rtpId);

        /* ��ȡRTP�������Ĺ�����б� */
    	count = rseRtpShlIdList((RTP_ID)rtpId, shlIdList);
    }

    /* ������ݰ� */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    /* ����ѭ���ɼ����й������Ϣ */
    while (idx < count)
    {
    	/* �ȼ��ʣ������Ƿ���Դ洢�������Ϣ */
    	RSE_CHECK_OVER_PKTSIZE(ptr, outbuf, RSE_RTP_SHL_MAX_INFO_LEN);

    	/* ���ûظ���� */
		len = sprintf((INT8 *)ptr,"%s","OK");
		ptr += (len+2);

		pRtpShlNode = shlIdList[idx++];

		/* д������ID */
		RSE_PUTPKT_SHORT(RSE_TYPE_RTP_SHL_LIST, ptr);

		/* д�����ID */
		RSE_PUTPKT_LONG(rtpId, ptr);

		/* д���־λ */
		*ptr = 0;
		ptr += 1;

		/* ��չ������ */
		ZeroMemory(shlName,RSE_MAX_NAME_LEN+1);

		/* д��Ҫ��ʾ��ǩ :������� */
		rsePathTruncate(pRtpShlNode->pShlNode->name, shlName, RSE_MAX_NAME_LEN);
		len = sprintf((INT8 *)ptr,"%s,%s,%x",shlName, pRtpShlNode->pShlNode->name,pRtpShlNode->pShlNode->textBase);
		ptr += (len+1);

		/* д��������� */
		*ptr = RSE_NULL_OPER;
		ptr += 1;

		/* д�����Ը��� */
		*ptr = RSE_NULL_ATTR;
		ptr += 1;
    }

    /* �����������Ϊ "$",��ʾRTP������Ϣ������� */
    if(idx == count)
    {
    	*ptr = RSE_END_STRING;
    	ptr += 1;
    	idx = count = 0;
    }

    /* ��������������ݴ�С */
    *outSize = ptr - outbuf;
}
