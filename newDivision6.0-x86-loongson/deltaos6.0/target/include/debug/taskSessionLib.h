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
 * @file: taskSessionLib.h
 * @brief:
 *             <li>�Ự����</li>
 */
#ifndef _TASK_SESSION_LIB_H_
#define _TASK_SESSION_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ �� ��******************************/
#include "taErrorDefine.h"

/************************���Ͷ���*****************************/

/************************�� �� ��******************************/

/* ����ͨ����ʼ�� */
#define TA_DEBUG_CHANNEL_START  6

/* ����ͨ�������� */
#define TA_DEBUG_CHANNEL_END   37

/* ���ݻỰID��ȡ����ͨ�� */
#define TA_GET_CHANNEL_ID(sid) ((sid)+TA_DEBUG_CHANNEL_START)

/* ���ݵ���ͨ��ID��ȡ�ỰID */
#define TA_GET_SESSION_ID(cid) ((cid)-TA_DEBUG_CHANNEL_START)

/* ����ͨ�����Ƿ���Ч */
#define TA_IS_INVALID_CHANNEL(sid) (((sid)<(UINT32)(TA_DEBUG_CHANNEL_START)) || ((sid)>(UINT32)(TA_DEBUG_CHANNEL_END)))

/* ���ĻỰ���� */
#define TA_MAX_SESS_NUM 32

/* ��ȡ�������� */
#define TA_GET_TASK_LIST(sid) (&(taSessionTable[(sid)].sessInfo.taskList))

/* ��ȡ�Ự��Ϣ */
#define TA_GET_SESSION_INFO(sid) (&(taSessionTable[(sid)].sessInfo))

/* ��ȡ�쳣������ */
#define TA_GET_EXP_CONTEXT(sid) ((taSessionTable[(sid)].sessInfo.context))

/* ���ԻỰ�Ƿ�� */
#define TA_SESSION_IS_ATTACH(sid) ((DEBUG_STATUS_ATTACH == taSessionTable[(sid)].sessInfo.attachStatus))

/* ���Զ����Ƿ�� */
#define TA_OBJECT_IS_ATTACH(objId,sid) ((objId) == taSessionTable[(sid)].sessInfo.objID)

/* ���ݻỰID��ȡ������ID */
#define TA_GET_CONTEXT_ID(sid) (taSessionTable[(sid)].sessInfo.objID)

/* ��ȡ���������� */
#define TA_GET_CONTEXT_TYPE(sid) (taSessionTable[(sid)].sessInfo.ctxType)

/* ��ȡ���ԻỰ */
#define TA_GET_DEBUG_SESSION(sid) (&taSessionTable[(sid)])

/* �����Ƿ��Զ���  */
#define TA_TASK_IS_AUTO_ATTACH(sid) (taSessionTable[(sid)].sessInfo.taskIsAutoAttach)

/************************�ӿ�����*****************************/

/* ���ԻỰ�� */
extern T_TA_Session taSessionTable[TA_MAX_SESS_NUM];

/*
 * @brief:
 *     ��ʼ�����ԻỰ
 * @return:
 *     ��
 */
void taSessionInit(void);

/*
 * @brief:
 *      ������ԻỰ
 * @param[in]: objId: ���ԵĶ���ID��ʵʱ���̵���Ϊʵʱ����ID,�����������ΪkernelId
 * @param[out]: pSid: �ỰID
 * @return:
 *     TA_OK:������ԻỰ�ɹ�
 *     TA_SESSION_FULL:���ԻỰ������
 */
T_TA_ReturnCode taSessionMalloc(TA_CTX_TYPE ctxType, UINT32 objId, UINT32 *pSid);

/*
 * @brief:
 *      �ͷŵ��ԻỰ
 * @param[in]: sid: �ỰID
 * @return:
 *     ��
 */
void taSessionFree(UINT32 sid);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_SESSION_LIB_H_ */
