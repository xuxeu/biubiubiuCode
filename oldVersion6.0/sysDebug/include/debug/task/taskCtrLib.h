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
 * @file: taskCtrLib.h
 * @brief:
 *             <li>�Ự����</li>
 */
#ifndef _TASK_CTRL_LIB_H_
#define _TASK_CTRL_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ �� ��******************************/
#include "taTypes.h"
#include "taskSllLib.h"
#include "sdaMain.h"
#include "taErrorDefine.h"

/************************���Ͷ���*****************************/

/************************�� �� ��******************************/

/************************�ӿ�����*****************************/

/*
 * @brief:
 *      �ͷ�����ڵ��ڴ�
 * @param[in]: pTaskNode: ����ڵ�
 * @param[in]: arg: ��������
 * @return:
 *     FALSE: ������������
 */
BOOL taTaskNodeFree(SLL_NODE *pTaskNode, INT32 arg);

/*
 * @brief:
 *      ��������ڵ�
 * @param[in]: tid: ����ID
 * @param[out]: pSid: �ỰID
 * @return:
 *     NULL:δ�ҵ���Ӧ������ڵ�
 *     T_TASK_NODE:����ڵ�
 */
SLL_NODE* taTaskNodeFind(INT32 tid, UINT32 *pSid);

/*
 * @brief:
 *     �����Ƿ���Ҫ�󶨣����������������Ϊ�󶨵��Ե�ʵʱ���̴������߰󶨵ĵ������񴴽����Զ��󶨸����񵽵��ԻỰ��
 * @param[in]: tid: ����ID
 * @param[out]: pSid: �ỰID
 * @return:
 *     FALSE: ��������񵽵��ԻỰ
 *     TRUE��������񵽵��ԻỰ
 */
BOOL taTaskIsAttach(int tid, UINT32 *pSid);

/*
 * @brief:
 *      ��ȡ���ԻỰ�а󶨵�����ID�б�
 * @param[in]: sid: �ỰID
 * @param[out]: idList: ����ID�б�
 * @param[in]: maxTasks: ���������������
 * @return:
 *     idx:�������
 */
INT32 taGetTaskIdList(UINT32 sid, INT32 idList[], INT32 maxTasks);

/*
 * @brief:
 *      �󶨵�������
 * @param[in]: sid: �ỰID
 * @param[in]: tid: ����ID
 * @return:
 *     TA_OK:������ɹ�
 *     TA_INVALID_TASK_ID:��Ч����ID
 *     TA_TASK_ALREADY_ATTACH:�����Ѿ���
 *     TA_ALLOC_MEM_FAIL:�����ڴ�ʧ��
 */
T_TA_ReturnCode taAttachTask(UINT32 sid, INT32 tid);

/*
 * @brief:
 *      �������
 * @param[in]: tid: ����ID
 * @return:
 *     TA_OK:�������ɹ�
 *     TA_TASK_NO_ATTACH:����δ��
 */
T_TA_ReturnCode taDetachTask(INT32 tid);

/*
 * @brief:
 *      ���԰�����
 * @param[in]: ctxType: ����������
 * @param[in]: taskId: ����ID
 * @param[out]: pSid: �ỰID
 * @return:
 *     TA_OK: ������ɹ�
 *     TA_INVALID_TASK_ID:��Ч����ID
 *     TA_TASK_ALREADY_ATTACH:�����Ѿ���
 *     TA_ALLOC_MEM_FAIL:�����ڴ�ʧ��
 *     TA_ALLOCATE_SESSION_FAIL��������ԻỰʧ��
 */
UINT32 taTaskAttach(TA_CTX_TYPE ctxType, UINT32 taskId, UINT32 *pSid);

/*
 * @brief:
 *      ���Խ������
 * @param[in]: sid: �ỰID
 * @return:
 *     TA_OK: ���ɹ�
 */
UINT32 taTaskDetach(UINT32 sid);

/*
 * @brief:
 *      ���ݻָ�����ڵ����������
 * @param[in]: pTaskNode: ����ڵ�
 * @param[in]: arg: ��������
 * @return:
 *     FALSE: ��ʾ�ָ�����������������
 */
BOOL taTaskNodeResume(SLL_NODE *pTaskNode, INT32 arg);

/*
 * @brief:
 *      ��������ڵ������
 * @param[in]: pTaskNode: ����ڵ�
 * @param[in]: arg: ��������
 * @return:
 *     FALSE: ��ʾ��������������������
 */
BOOL taTaskNodeSuspend(SLL_NODE *pTaskNode, INT32 arg);

/*
 * @brief:
 *      ���е��ԻỰ���������е���������
 * @param[in]: sid: �ỰID
 * @return:
 *     OK:������������ɹ�
 */
STATUS taTaskAllResume(INT32 sid);

/*
 * @brief:
 *      ��ͣ���ԻỰ���������е���������
 * @param[in]: sid: �ỰID
 * @return:
 *     OK:������������ɹ�
 */
STATUS taTaskAllSuspend(INT32 sid);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_CTRL_LIB_H_ */
