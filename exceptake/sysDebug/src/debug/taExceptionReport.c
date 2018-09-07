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
#include "string.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/
UINT8 *g_sdaExpType[33] =
{
    "Divide Error",        /* 0 */
    "Trap",        /* 1 */
    "NMI Interrupt",        /* 2 */
    "Breakpoint",        /* 3 */
    "Overflow",        /* 4 */
    "BOUND Range Exceeded",        /* 5 */
    "Invalid Opcode(Undefined Opcode)",        /* 6 */
    "Device Not Available (No Math Coprocessor)",        /* 7 */
    "Double Fault",        /* 8 */
    "Coprocessor Segment Overrun",        /* 9 */
    "Invalid TSS(Task State Segment)",        /* 10 */
    "Segment Not Present",        /* 11 */
    "Stack fault exception",        /* 12 */
    "Stack-Segment Fault",        /* 13 */
    "Page Fault",        /* 14 */
    "Reserved",        /* 15 */
    "x87 FPU Floating-Point Error (Math Fault)",        /* 16 */
    "Alignment Check",        /* 17 */
    "Machine Check",        /* 18 */
    "SIMD Floating-Point Exception",        /* 19 */
    "Reserved",        /* 20 */
    "Reserved",        /* 21 */
    "Reserved",        /* 22 */
    "Reserved",        /* 23 */
    "Reserved",        /* 24 */
    "Reserved",        /* 25 */
    "Reserved",        /* 26 */
    "Data address breakpoint",        /* 27 */
    "Reserved",        /* 28 */
    "Reserved",        /* 29 */
    "Reserved",        /* 30 */
    "Instruction address breakpoint",        /* 31 */
    "User defined exception type"         /* 32 */
};

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



/*
 * @brief:
 *      �����ϱ��������˵��쳣��������Ϣ ����Ƿ������쳣����Ҫ������������������������ʱ���з�����
 *      "e:ID:%x;ExceptionInfo"�������˽�����Լ�����������޸ģ�������쳣��Ϣ�ɸ�����Ҫ�������
 * @param[in] pBuf: �쳣�����Ĵ�Ż���
 * @param[in] tid: ����ID
 * @param[in] dbContext: �쳣������
 * @param[in] ctxType: ����������
 * @return:
 *      none
 * @implements  DM.3.153
 */
void taSetReportExceptionInfo(UINT8 *pBuf, UINT32 tid, T_DB_ExceptionContext *dbContext, UINT8 *buf, UINT32 ctxType)
{
	UINT32 vector = dbContext->vector;

	if (vector > 31)
	{
		vector = 32;
	}

	if(NULL == tid)
	{
		/* �����ϱ��������˵��쳣��������Ϣ */
		sprintf(pBuf, "e:ID:%x;ExceptionInfo: Type: %s, tid: %d, tName: %s, PC = 0x%x,  %s, ctxType : %d",
				taGetTargetId(),
				g_sdaExpType[vector],
				tid,
				taTaskName(tid),
				dbContext->registers[PC],
				buf,
				ctxType
				);
	}
	else
	{
		/* �����ϱ��������˵��쳣��������Ϣ */
		sprintf(pBuf, "e:ID:%x;ExceptionInfo: Type: %s, tid: %d, tName: %s, PC = 0x%x, %s, ctxType : %d",
				taGetTargetId(),
				g_sdaExpType[vector],
				tid,
				taTaskName(tid),
				dbContext->registers[PC],
				buf,
				ctxType
				);
	}
}

/*
 * @brief:
 *      �ϱ��쳣��������
 * @param[in] session: ���ԻỰ
 * @param[in] tid: ����ID
 * @param[in] dbContext: �쳣������
 * @param[in] ctxType: ����������
 * @return:
 *      none
 * @implements  DM.3.153
 */
T_TA_ReturnCode taReportExceptionToHost(T_TA_Session *session, UINT32 tid, T_DB_ExceptionContext *dbContext, UINT8 *buf, UINT32 ctxType)
{
	UINT8 *pBuf = NULL;
	UINT32 Offset = 0;

	/* ��ȡ���ݰ��Ĵ�����ݵ���ʼ��ַ */
	pBuf = taGetPktUsrData(&(session->packet));

	/* �������ݰ���ͷ */
	taSetPktHeadReceiver(&(session->packet.header),(UINT16)TSMAP_ID);

	/* �����ϱ����쳣��Ϣ */
	taSetReportExceptionInfo(pBuf, tid, dbContext, buf, ctxType);

	/* �����쳣��Ϣ���ݰ� */
	taCOMMSendPacket(&(session->packet), strlen((const char *)pBuf));

	return (TA_OK);
}

