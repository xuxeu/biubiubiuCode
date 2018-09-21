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
 * @file:rtpRtIfLib.c
 * @brief:
 *             <li>RTP���нӿ�����</li>
 */

/************************ͷ �� ��*****************************/
#include "taRtIfLib.h"
#include "taRtp.h"
#include "taskCtrLib.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/

/**
 * @brief:
 * 	  RTP�������ʱ�ӿڳ�ʼ��
 * @return:
 *    ��
 */
void taRtpRtIfInit(void)
{
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).resume = (STATUS (*) (int))taTaskAllResume;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).suspend = (STATUS (*) (int))taTaskAllSuspend;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).attach = taRtpTaskAttach;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).detach = taRtpTaskDetach;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).idVerify = (STATUS (*) (int))rtpIdVerify;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).contextGet = taTaskContextGet;
    (taRtIf.taCtxIf[TA_CTX_TASK_RTP]).contextSet = taTaskContextSet;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).resume = NULL;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).suspend = NULL;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).attach = taRtpSystemAttach;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).detach = taRtpSystemDetach;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).idVerify = (STATUS (*) (int))rtpIdVerify;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).contextGet = taTaskContextGet;
    (taRtIf.taCtxIf[TA_CTX_SYSTEM_RTP]).contextSet = taTaskContextSet;
}
