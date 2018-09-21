/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-10-13         ��Ԫ־�������������ɼ������޹�˾
 *                           �������ļ���
 */

/**
 * @file  rseRtpLib.h
 * @brief
 *       ���ܣ�
 *       <li>RTP������������</li>
 */
#ifndef RSE_RTPLIB_H_
#define RSE_RTPLIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ �� ��******************************/
#include "ta.h"

/************************�� �� ��******************************/

/* RTP��ʾ�������� */
#define MAX_RTP_NUM 1000

/************************���Ͷ���*****************************/

/************************�ӿ�����*****************************/

/*
 * @brief
 * 		�ṩRTP�������Ϣ����
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      TA_OK:�����ɹ�
 */
T_TA_ReturnCode rseRtpTaskHandler(T_TA_Packet *packet,UINT32 *outSize);

/*
 * @brief
 * 		����RTP ID��ȡRTPģ����Ϣ
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
void rseGetRtpModuleInfo(T_TA_Packet* packet,UINT32* outSize);

/*
 * @brief
 * 		����RTP ID��ȡRTP�����
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
void rseGetRtpShlList(T_TA_Packet *packet,UINT32 *outSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RSE_RTPLIB_H_ */
