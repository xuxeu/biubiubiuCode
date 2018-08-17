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
 * @file: taRtp.h
 * @brief:
 *             <li>�Ự����</li>
 */
#ifndef _TA_RTP_H_
#define _TA_RTP_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ �� ��******************************/
#include <vxWorks.h>
#include <rtpLibP.h>
#include <rtpUtilLibP.h>
#include "taRtIfLib.h"
#include "ta.h"

/************************���Ͷ���*****************************/

/************************�� �� ��******************************/

/* �Ƿ�֧��RTP */
#define TA_RTP_IS_SUPPORED (_func_rtpSpawn != NULL)

/* �Ƿ�֧���ڴ��л� */
#define TA_MEM_SWITCH_IS_SUPPORED() (_func_taskMemCtxSwitch == NULL)

/* ͨ������ID��ȡʵʱ����ID */
#define	taRtpFromTaskGet(taskId) ((UINT32)(taskTcb(taskId)->rtpId))

/************************�ӿ�����*****************************/

/* ����ID */
extern RTP_ID kernelId;

/*
 * @brief:
 *     RTP�����ڴ�����ӿڳ�ʼ��
 * @return:
 *     TA_OK:   �����ɹ�
 */
T_TA_ReturnCode taRtpMemoryInit(void);

/**
 * @brief:
 * 	  RTP�������ʱ�ӿڳ�ʼ��
 * @return:
 *    ��
 */
void taRtpRtIfInit(void);

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
UINT32 taRtpTaskAttach(TA_CTX_TYPE ctxType, UINT32 objId, UINT32 *pSid);

/*
 * @brief:
 *      ���Խ��RTP
 * @param[in]: sid: �ỰID
 * @return:
 *     TA_OK: ���RTP�ɹ�
 */
UINT32 taRtpTaskDetach(UINT32 sid);

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
UINT32 taRtpSystemAttach(TA_CTX_TYPE ctxType, UINT32 objId, UINT32 *pSid);

/*
 * @brief:
 *      ϵͳ�����Խ��RTP
 * @param[in]: sid: �ỰID
 * @return:
 *     TA_OK: ���RTP�ɹ�
 */
UINT32 taRtpSystemDetach(UINT32 sid);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TA_RTP_H_ */
