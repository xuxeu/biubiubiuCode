/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-23         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file:taskStandardRsp.h
 *@brief:
 *             <li>�����׼RSP������غ�������</li>
 */
#ifndef _TASK_STANDARD_RSP_H
#define _TASK_STANDARD_RSP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ �� ��******************************/
#include "ta.h"

/************************�궨��*******************************/

/* ���񼶱�׼RSP������� */
#define TA_TASK_RSP_TABLE_LEN  22

/************************���Ͷ���*****************************/

/************************�ӿ�����*****************************/

/* ����RSP������ⲿ���� */
extern T_DB_RspCommandTable taTaskRSPCmdTable[TA_TASK_RSP_TABLE_LEN];

/*
 * @brief:
 *      �������񼶵�����ͣ����
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *     TA_DB_NEED_IRET:ִ�гɹ�
 */
T_TA_ReturnCode taTaskRspPauseProc(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ����"qfThreadInfo"��"qsThreadInfo"����,��ȡ���ԻỰ�а󶨵ĵ�������ID�б�
 *      ���������ȷ��� qfThreadInfo�����ȡ��һ���߳���Ϣ��Ȼ����qsThreadInfo����
 *      ��ȡʣ�µ��߳���Ϣ����Ŀ����˻ظ�'l'��ʾ�߳���Ϣ�ظ����
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode taTaskRspGetThreadIdList(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ����"T"����,�鿴�߳��Ƿ��ڼ���״̬
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode taTaskRspThreadIsActive(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ���õ��ԻỰ���Զ��������־���������ΪTRUE���ɰ󶨵������񴴽�������
 *      ���Զ��󶨵����ԻỰ
 * @param[in]:tpInfo:���ԻỰ��Ϣ
 * @param[in]:bpInbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:bpOutbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *     TA_FAIL: ����ʧ��
 *     TA_OK: �����ɹ�
 */
T_TA_ReturnCode taTaskAutoAttachTask(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _TASK_STANDARD_RSP_H */
