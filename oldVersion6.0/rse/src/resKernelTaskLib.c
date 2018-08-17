/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-07-08         ��Ԫ־�������������ɼ������޹�˾
 *                           �������ļ���
 */

/**
 * @file  resKernelTaskLib.c
 * @brief
 *       ���ܣ�
 *       <li>����������Ϣ��ѯ</li>
 */

/************************ͷ �� ��******************************/
#include "rseConfig.h"
#include "taskLib.h"
#include "rseCmdLib.h"
#include "rseInitLib.h"

/************************�� �� ��******************************/

/* �����������Ը��� */
#define RSE_TASK_INFO_ATTR_NUM 4

/* ��������� */
#define RSE_MAX_DSP_TASKS 1000

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/*
 * @brief
 *      �ṩ����������Ϣ����Ϣ����
 * @param[in] packet: ���յ������ݰ�
 * @return
 *      None
 */
static T_TA_ReturnCode rseKernelTaskHandler(T_TA_Packet* packet);

/*
 * @brief
 * 		��ȡϵͳ���������б�
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
static void rseGetKernelTaskList(T_TA_Packet* packet,UINT32* outSize);

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/**
  * @brief
  *     ����������Ϣ��ʼ������
  *@return
  *      None
  */
void rseKernelTaskLibInit(void)
{
	rseRegisterModuleHandler(RSE_TYPE_TASK_MODULE, rseKernelTaskHandler);
}

/*
 * @brief
 *      �ṩ�����������Ϣ����
 * @param[in] packet: ���յ������ݰ�
 * @return
 *      TA_OK:�����ɹ�
 */
static T_TA_ReturnCode rseKernelTaskHandler(T_TA_Packet* packet)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT32 outSize = 0;
    UINT8 operateId = 0;

    /* �Ӱ��л�ȡʵ������ */
    inbuf = taGetPktUsrData(packet);

    /* �����������outbufΪinbuf */
    outbuf = inbuf;

    /* ��ȡ����ID */
    operateId = inbuf[2];
    switch (operateId)
    {
        case RSE_OPR_QUERY:
        	rseGetKernelTaskList(packet, &outSize);
            break;
        default:
        	outSize = (UINT32)sprintf((INT8*)outbuf, "E%d", TA_INVALID_OPERATE_ID);
            break;
    }

    /* �������ݰ� */
    taCOMMSendPacket(packet, outSize);

    return TA_OK;
}

/*
 * @brief
 * 		��ȡϵͳ���������б�
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
static void rseGetKernelTaskList(T_TA_Packet* packet,UINT32* outSize)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT8* ptr = NULL;
    UINT32 len = 0;
    INT32 taskId = 0;
    char taskStatus[RSE_MAX_NAME_LEN+1] = {0};
    char taskName[RSE_MAX_NAME_LEN+1] = {0};
    static UINT32 count = 0;
    static UINT32 idx = 0;
    static INT32 taskIdList[RSE_MAX_DSP_TASKS] = {0};

    /* �Ӱ��л�ȡʵ������ */
    inbuf = taGetPktUsrData(packet);

    /* �����������outbufΪinbuf */
    ptr = outbuf = inbuf;

    /* ��ȡ�߳�ID�б� */
    if(count == 0)
    {
    	count = taTaskIdListGet(taskIdList, RSE_MAX_DSP_TASKS);
    }

    /* ������ݰ� */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    /* ����ѭ���ɼ����к������������Ϣ */
    while (idx < count)
    {
    	/* �ȼ��ʣ������Ƿ���Դ洢������Ϣ */
    	RSE_CHECK_OVER_PKTSIZE(ptr,outbuf, RSE_TASK_MAX_INFO_LEN);

        /* ȡ������ID */
        taskId = taskIdList[idx++];

        /* ��������Ƿ��Ǻ������� */
        if(!IS_KERNEL_TASK(taskId))
        {
        	continue;
        }

        /* ���ûظ���� */
        len = sprintf((INT8 *)ptr,"%s","OK");
        ptr += (len+2);

		/* д������ID */
		RSE_PUTPKT_SHORT(RSE_TYPE_KERNEL_TASK, ptr);

		/* д�����ID */
		RSE_PUTPKT_LONG(taskId, ptr);

		/* д���־λ */
		*ptr = 0;
		ptr += 1;

		/* ��������� */
		ZeroMemory(taskName,RSE_MAX_NAME_LEN+1);

		/* ��ȡ������ */
		strncpy(taskName, taTaskName(taskId), RSE_MAX_NAME_LEN);

		/* д��Ҫ��ʾ��ǩ */
		len = sprintf((INT8 *)ptr,"%s", taskName);
		ptr += (len+1);

	    /* д��������� */
	    *ptr = RSE_NULL_OPER;
	    ptr += 1;

		/* д�����Ը��� */
		*ptr = RSE_TASK_INFO_ATTR_NUM;
		ptr += 1;

		/* д�����Ա�,����ID��������������״̬���������� */
	    RSE_PUTPKT_SHORT(RSE_ATTR_ID, ptr);
	    len = sprintf((INT8 *)ptr,"%x", taskId);
	    ptr += (len+1);

	    /* д������������ */
	    RSE_PUTPKT_SHORT(RSE_ATTR_NAME, ptr);
	    len = sprintf((INT8 *)ptr,"%s", taskName);
	    ptr += (len+1);

		/* �������״̬�ַ��� */
		ZeroMemory(taskStatus,RSE_MAX_NAME_LEN+1);

	    /* д������״̬���� */
	    RSE_PUTPKT_SHORT(RSE_ATTR_STATE, ptr);
	    taskStatusString(taskId, taskStatus);
	    len = sprintf((INT8 *)ptr,"%s", taskStatus);
	    ptr += (len+1);

	    /* д�������������� */
	    RSE_PUTPKT_SHORT(RSE_ATTR_TASK_TYPE, ptr);
	    len = sprintf((INT8 *)ptr,"%s", "Object");
	    ptr += (len+1);
    }

    /* �����������Ϊ "$",��ʾ������Ϣ������� */
    if(idx == count)
    {
    	*ptr = RSE_END_STRING;
    	ptr += 1;
    	idx = count = 0;
    }

    /* ��������������ݴ�С */
    *outSize = ptr - outbuf;
}
