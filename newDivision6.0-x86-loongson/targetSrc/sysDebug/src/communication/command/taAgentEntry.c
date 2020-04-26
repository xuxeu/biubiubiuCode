/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * �޸���ʷ:
 * 2013-06-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ��� 
 */

/*
 * @file:taAgentEntry.c
 * @brief:
 *             <li>TA�߼�ͨ����Ϣ�������ʵ��</li>
 */

/************************ͷ �� ��*****************************/
#include "taCommand.h"
#include "taMsgDispatch.h"
#include "taManager.h"
#include "taPacketComm.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 * 	   TA��Ϣ�ӿڴ���ͳһ��ڣ����߼�ͨ��(��������)���Ե���ͳһ������Ϣ�ӿڵ�����
 * @param[in]:packet: Ҫ�������Ϣ��
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:commandType: �������ͣ�����Ϊ���������ѯ�����������
 * @param[in]:cmdTable: �����
 * @param[in]:tableLength: �������
 * @param[in]:noReplyCallback: �������������ظ�ʱ�Ļص��ӿ�
 * @param[in]:prepareCallback: ��������ǰ�Ļص��ӿ�
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_FAIL: ����ִ��ʧ��
 *     TA_DB_NEED_IRET����Ҫ���쳣�з���
 *     TA_COMMAND_NOTFOUND: ����û�ҵ�
 */
T_TA_ReturnCode taAgentEntry(T_TA_Packet *packet,
                             T_DB_Info *debugInfo,
                             T_TA_CommandType commandType,
                             T_TA_CommandTable *cmdTable,
                             UINT32 tableLength,
                             TA_ProcessNoReplyCallback noReplyCallback,
                             TA_ProcessPrepareCallback prepareCallback)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT32 cmdSize = 0;
    UINT32 bufSize = 0;
    T_TA_ReturnCode ret = TA_COMMAND_NOTFOUND;
    BOOL needReply = TA_NEED_REPLY;

    /* ��ת��ͷ packet */
    taSwapSenderAndReceiver(&(packet->header));

    /* �Ӱ��л�ȡʵ������ */
    inbuf = taGetPktUsrData(packet);

    /* �����������outbufΪinbuf */
    outbuf = inbuf;

    /* ��ȡʵ�����ݳ��� */
    cmdSize = taGetPktSize(packet);
    
    /* ������������СΪ���ݰ���������� */
    bufSize = taGetPktCapacity(packet);
    
    /* ���ʵ�����ݳ��ȴ���0 */
    if (cmdSize > 0)
    {
        /* ���������С���������İ����� */
        if (cmdSize < (TA_PKT_DATA_SIZE - 1) )
        {
            /* ��������buffer�����һ���ַ�Ϊ���ַ� */
            inbuf[cmdSize] = (UINT8)'\0';
        }
    
        /* ��������ǰ�Ļص��ӿ�<prepareCallback>������NULL */
        if (prepareCallback != NULL)
        {
            prepareCallback(inbuf,&(packet->header));
        }
    
        /* ����taProcessCommandִ��������� */
        ret = taProcessCommand(debugInfo, inbuf, cmdSize, outbuf, &bufSize, &needReply, commandType,cmdTable,tableLength);
    }
	
    /* ������� */
    if (TA_COMMAND_NOTFOUND == ret)
    {
        outbuf[0]='\0';
        bufSize = 1;
    }
	
	/* �ж����ݰ��Ƿ���Ҫ�ظ� */
	if (TA_NEED_REPLY == needReply)
	{
		/* �������ݰ� */
		taCOMMSendPacket(packet, bufSize);
	}

    /* ��Ҫ�쳣����  */
    if (TA_DB_NEED_IRET == ret)
    {
        /* ֹͣ��ѭ�հ�,c��s����ִ�и÷�֧ */
        taStopLoopDispatch();

        /* ����������������ظ�ʱ�Ļص��ӿڲ�����NULL */
        if (noReplyCallback != NULL)
        {
            noReplyCallback(&(packet->header));
        }
    }
	
    return (ret);
}
