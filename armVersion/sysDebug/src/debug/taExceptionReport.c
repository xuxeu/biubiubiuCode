/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file:taExceptionReport.c
 * @brief:
 *             <li>�����쳣�ϱ�����</li>
 */

/************************ͷ �� ��******************************/
#include "taExceptionReport.h"
#include "dbAtom.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

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
void taSaveGdbPacketHead(const UINT8 *inbuf, const T_TA_PacketHead *gdbPacketHead, T_TA_PacketHead *packetHead)
{
    /* ���뻺�������Ϊ"c","r","s","!"������һ�� */
    if ((( 'c' == inbuf[0])
        || ( 's' == inbuf[0] )
        || ( 'r' == inbuf[0] )
        || ( '!' == inbuf[0] )))
    {
        /* ����taSetPktHeadReceiver���������ID���Ա�����쳣��������������Ҫ�õ�GDB��ͷ��Ϣ */
        taSetPktHeadReceiver(packetHead,taGetPktHeadSender(gdbPacketHead));
    }
}

/*
 * @brief:
 *     �����쳣ֹͣ�ظ���Ϣ
 * @param[out]:outbuf:������ݻ��壬�����쳣�ظ���Ϣ
 * @param[in]:signal:�ź�ֵ
 * @param[in]:taskId:����ID
 * @return:
 *         ��
 */
void taSetExceptionStopReply(UINT8 *outbuf, UINT32 signal, ulong_t taskId)
{
	int idx = 0;

    /* �ж��ں��Ƿ����� */
    if(TA_KERNEL_IS_INITIALIZED())
	{
		/* �����쳣�ظ�����Ϣ"T05thread:�쳣�߳�ID;" */
    	outbuf[idx++] = 'T';
    	outbuf[idx++] = taHexChars[((signal >> 4) & 0xF)];
    	outbuf[idx++] = taHexChars[signal % 16];
	#ifdef CONFIG_TA_LP64
    	idx += (UINT32)sprintf((INT8*)(&outbuf[idx]), "thread:%lx",taskId);
	#else
		idx += (UINT32)sprintf((INT8*)(&outbuf[idx]), "thread:%x",taskId);
	#endif
    	outbuf[idx++] = ';';
    	outbuf[idx] = '\0';
	}
	else
	{
		outbuf[idx++] = 'S';
		outbuf[idx++] = taHexChars[((signal >> 4) & 0xF)];
		outbuf[idx++] = taHexChars[signal % 16];
		outbuf[idx] = '\0';
	}
}

/*
 * @brief:
 *      �ϱ������쳣��GDB,��Ҫ����ϵ�ṹ��صĵ����쳣������ת��ΪPOSIX�쳣�ź��ϱ���GDB
 * @param[in]:session: ���ԻỰ
 * @param[in]:vector: �����쳣������
 * @return:
 *      ��
 */
void taReportExceptionToDebugger(T_TA_Session *session, UINT32 vector)
{
	ulong_t taskId = 0;
	UINT32 curSignal = 0;

    /* ��ȡ���ݰ��Ĵ�����ݵ���ʼ��ַ */
    UINT8 *pBuf = taGetPktUsrData(&(session->packet));

    /* ��vectorת��ΪPOSIX�ź� */
    curSignal= taDebugVector2Signal(vector, session->sessInfo.context);

    /* ��ȡ��ǰ�쳣����ID */
    taskId = TA_GET_CUR_THREAD_ID(&(session->sessInfo));

    /* �����쳣�ظ���Ϣ */
    taSetExceptionStopReply(pBuf, curSignal,taskId);

    /* �������ݰ���ͷ */
    taSetPktHeadReceiver(&(session->packet.header),
            taGetPktHeadReceiver(&(session->gdbPacketHead)));

    /* �����쳣��Ϣ���ݰ��������� */
    taCOMMSendPacket(&(session->packet), strlen((const char *)pBuf));
}
