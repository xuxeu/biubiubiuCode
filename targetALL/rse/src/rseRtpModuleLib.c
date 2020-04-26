/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-10-12         ��Ԫ־�������������ɼ������޹�˾
 *                           �������ļ���
 */

/**
 * @file  rseRtpModuleLib.c
 * @brief
 *       ���ܣ�
 *       <li>��ȡRTPģ����Ϣ</li>
 */

/************************ͷ �� ��******************************/
#include "rseConfig.h"
#include "rtpLibP.h"
#include "rtpUtilLibP.h"
#include "rtpLibCommon.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief
 * 		����RTP ID��ȡRTPģ����Ϣ
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
void rseGetRtpModuleInfo(T_TA_Packet *packet,UINT32 *outSize)
{
	UINT8 *inbuf = NULL;
    UINT8 *outbuf = NULL;
	UINT8 *ptr = NULL;
	UINT8 *buf = NULL;
    UINT32 len = 0;
	UINT32 rtpId = 0;
    char rtpName[RSE_MAX_NAME_LEN+1] = {0};
	RTP_DESC rtpInfo;

	 /* �Ӱ��л�ȡʵ������ */
	inbuf = taGetPktUsrData(packet);

	/* �����������outbufΪinbuf */
	ptr = outbuf = inbuf;

	/* ������ʼ��ַ */
	buf = &inbuf[RSE_ARG_START_OFFSET];

	/* ��ȡRTP ID */
	n2l(buf,rtpId);

    /* ��ȡRTP��Ϣ */
    if(OK != rtpInfoGet((RTP_ID)rtpId, &rtpInfo))
    {
    	*outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d", TA_INVALID_INPUT);
    	return;
    }

    /* ������ݰ� */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    /* ���ûظ���� */
	len = sprintf((INT8 *)ptr,"%s","OK");
    ptr += (len+2);

	/* д������ID */
	RSE_PUTPKT_SHORT(RSE_TYPE_RTP_INFO, ptr);

	/* д�����ID */
	RSE_PUTPKT_LONG(rtpId, ptr);

	/* д���־λ */
	*ptr = 0;
	ptr += 1;

	/* ���RTP�� */
	ZeroMemory(rtpName,RSE_MAX_NAME_LEN+1);

	/* д��Ҫ��ʾ��ǩ :RTP��*/
	rsePathTruncate(rtpInfo.pathName, rtpName, RSE_MAX_NAME_LEN);
	len = sprintf((INT8 *)ptr,"%s,%s,%x",rtpName, rtpInfo.pathName,(UINT32)((WIND_RTP *)rtpId)->binaryInfo.textStart);
	ptr += (len+1);

	/* д��������� */
	*ptr = RSE_NULL_OPER;
	ptr += 1;

	/* д�����Ը��� */
	*ptr = RSE_NULL_ATTR;
	ptr += 1;

	/* ��������������ݴ�С */
	*outSize = ptr - outbuf;
}
