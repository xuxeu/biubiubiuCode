/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-28         ��Ԫ־�������������ɼ������޹�˾
 *                         �������ļ���
 */

/**
 * @file:taExceptionReport.c
 * @brief:
 *             <li>�����쳣�ϱ�����</li>
 */

/************************ͷ �� ��******************************/
#include "taExceptionReport.h"
#include "dbAtom.h"
#include "string.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/
typedef enum
{
MODE_MONITOR = 0,
MODE_WDB
}COMMUNICATE_MODE;
/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/
static UINT32 taReceiverId = 0;
static UINT32 CommunicateMode = MODE_MONITOR;
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
    //��¼said
    taSetSaid(gdbPacketHead->senderId);
		
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
void taSetExceptionStopReply(UINT8 *outbuf, UINT32 signal, UINT32 taskId)
{
	int idx = 0;

    /* �ж��ں��Ƿ����� */
    if(TA_KERNEL_IS_INITIALIZED())
	{
		/* �����쳣�ظ�����Ϣ"T05thread:�쳣�߳�ID;" */
    	outbuf[idx++] = 'T';
    	outbuf[idx++] = taHexChars[((signal >> 4) & 0xF)];
    	outbuf[idx++] = taHexChars[signal % 16];
    	idx += (UINT32)sprintf((INT8*)(&outbuf[idx]), "thread:%x",taskId);
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
	INT32 taskId = 0;
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


/**********************�����������ͬ��said������ͨ��ģʽ*****************************/
/**
 * @brief:
 *    ��ȡ�����˷������ݵ�said
 * @param[in]:packet:���ݰ�
 * @return:
 *     said
 */
int taGetSaid()
{
    return taReceiverId;
}

/**
 * @brief:
 *    ���������˷������ݵ�said
 * @param[in]:said:said
 * @return:
 *     said
 */
int taSetSaid(int said)
{
    taReceiverId = said;
}


void taSetCommunicateMode(int Mode)
{
	CommunicateMode = Mode;
}

BOOL taIgnoreSetEndToPoll()
{
    return ((CommunicateMode==MODE_MONITOR)?TRUE:FALSE);
}

