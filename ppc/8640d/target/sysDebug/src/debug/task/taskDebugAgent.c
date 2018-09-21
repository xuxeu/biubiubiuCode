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
 * @file:taskDebugAgent.c
 * @brief:
 *             <li>���񼶵��Դ�����ں���</li>
 */

/************************ͷ �� ��*****************************/
#include "ta.h"
#include "taskSessionLib.h"
#include "taskStandardRsp.h"
#include "taRtIfLib.h"
#include "taExceptionReport.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/

/*
 * @brief:
 *     ���񼶵��Դ�����ں���
 * @param[in]: packet:�������Բ���������Ϣ��
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_FAIL: ����ִ��ʧ��
 *     TA_DB_NEED_IRET����Ҫ���쳣�з���
 *     TA_COMMAND_NOTFOUND: ����û�ҵ�
 */
T_TA_ReturnCode taTaskDebugAgent(T_TA_Packet *packet)
{
    T_DB_Info *dbInfo = NULL;
    UINT32 sid = 0;
    T_TA_ReturnCode ret = TA_OK;
    UINT16 channelID = taGetPktHeadRecvChannel(&(packet->header));

    /* ���ݵ���ͨ���Ż�ȡ�����ԻỰID */
    sid = TA_GET_SESSION_ID(channelID);

    /* ��ȡ���ԻỰ��Ϣ */
    dbInfo = TA_GET_SESSION_INFO(sid);

    /* ����GDB��ͷ */
    taSaveGdbPacketHead(taGetPktUsrData(packet),&(packet->header),&(TA_GET_DEBUG_SESSION(sid)->gdbPacketHead));

    /* ����taAgentEntry�����յ��ĵ�����Ϣ�� */
    ret = taAgentEntry(packet,dbInfo,COMMANDTYPE_QUERY,taTaskRSPCmdTable,TA_TASK_RSP_TABLE_LEN,NULL,NULL);

    /* ��Ҫ�쳣����  */
    if (TA_DB_NEED_IRET == ret)
    {
    	/* �����ǰ���ڵ�����̬ͣ�����д��������Ϣ�������õ��ԻỰ״̬Ϊ����̬ */
    	if(TA_DEBUG_IS_PAUSE_STATUS(dbInfo))
    	{
    		/* ��д�����ģ�������ͣ���޷�ִ�е����Ȳ��� */
    		taTaskContextSet(TA_GET_CUR_THREAD_ID(dbInfo),TA_GET_EXP_CONTEXT(sid));

        	/* ���ûỰ��״̬ΪDEBUG_STATUS_CONTINUE����ͣʱ����Ϊ��̬ͣ���ָ�ʱӦ������Ϊ����״̬  */
        	TA_SET_DEBUG_STATUS(TA_GET_SESSION_INFO(sid),DEBUG_STATUS_CONTINUE);
    	}

    	/* �ָ������������� */
    	TA_RT_CTX_RESUME(dbInfo->ctxType,dbInfo->sessionID);
    }

    return ret;
}
