/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-10-23         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file:taExceptionReport.h
 *@brief:
 *             <li>�쳣�ϱ��������</li>
 */
#ifndef _TA_EXCEPTION_REPORT_H
#define _TA_EXCEPTION_REPORT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ �� ��******************************/
#include "ta.h"

/************************�궨��*******************************/

#define	TA_KERNEL_IS_INITIALIZED()	\
	(taTaskIdSelf() == 0 ? FALSE : TRUE)

/************************���Ͷ���*****************************/

/*
 * @brief:
 *     ����GDB��Ϣ��ͷ�������յ�����Ϊ��c��'s' 'r' '!'���¼��
 *     �ݰ���ͷ�����ԻỰ��Ϣ��GDB��ͷ�У��������쳣��������������Ҫ�õ�GDB��ͷ��Ϣ
 * @param[in]:inbuf:�������ݻ���
 * @param[in]:gdbPacketHead:���յ���GDB��ͷ
 * @param[out]:packetHead:����GDB��ͷ
 * @return:
 *         ��
 */
void taSaveGdbPacketHead(const UINT8 *inbuf, const T_TA_PacketHead *gdbPacketHead, T_TA_PacketHead *packetHead);

/*
 * @brief:
 *     �����쳣ֹͣ�ظ���Ϣ
 * @param[out]:outbuf:������ݻ��壬�����쳣�ظ���Ϣ
 * @param[in]:signal:�ź�ֵ
 * @param[in]:taskId:����ID
 * @return:
 *         ��
 */
void taSetExceptionStopReply(UINT8 *outbuf, UINT32 signal, ulong_t taskId);

/*
 * @brief:
 *      �ϱ������쳣��GDB,��Ҫ����ϵ�ṹ��صĵ����쳣������ת��ΪPOSIX�쳣�ź��ϱ���GDB
 * @param[in]:session: ���ԻỰ
 * @param[in]:vector: �����쳣������
 * @return:
 *      ��
 */
void taReportExceptionToDebugger(T_TA_Session *session, UINT32 vector);

/************************�ӿ�����*****************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
