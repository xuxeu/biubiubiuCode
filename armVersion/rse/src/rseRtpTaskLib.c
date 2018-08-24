/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-10-12         ��Ԫ־�������������ɼ������޹�˾
 *                           �������ļ���
 */

/**
 * @file  rseRtpTaskLib.c
 * @brief
 *       ���ܣ�
 *       <li>��ȡRTP������Ϣ</li>
 */

/************************ͷ �� ��******************************/
#include "rseConfig.h"
#include "rtpLibP.h"
#include "rtpUtilLibP.h"
#include "rtpLibCommon.h"
#include "ta.h"

/************************�� �� ��******************************/

/* RTP�������Ը��� */
#define RSE_RTP_TASK_INFO_ATTR_NUM 3

/* ��ʾRTP����������� */
#define RSE_MAX_RTP_TASKS	200

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief
 * 		��ȡRTP�����б�
 * @return
 *      �������
 */
static int resGetRtpTaskIdList(ulong_t rtpId, ulong_t rtpTaskIdList[])
{
	WIND_TCB *tid;
	int nTask = 0;

	FOR_EACH_TASK_OF_RTP(rtpId, tid);
		if (nTask+1 <= RSE_MAX_RTP_TASKS)
			rtpTaskIdList[nTask++] = (ulong_t)tid;
	END_FOR_EACH_TASK_OF_RTP;

	return nTask;
}

/*
 * @brief
 * 		�ض�·������ȡBaseName
 * @return
 *      ��
 */
void rsePathTruncate(char *path, char *name, UINT32 maxNameLength)
{
	char *filename = NULL;
	char *pTmpStr;
	char tmpStr[RSE_MAX_PATH_LENGTH + 1];

	/* ����RTP·�� */
	strncpy (tmpStr, path, RSE_MAX_PATH_LENGTH);

	/* �Ƴ�·�� */
	filename = strrchr (tmpStr, '/');
	if (filename == NULL)
	{
	    filename = strrchr(tmpStr, '\\');
	}

	if (filename == NULL)
	{
	    filename = tmpStr;
	}
	else
	{
	    filename[0] = EOS;
	    filename++;
	 }

	/* �Ƴ� ��׺ */
	pTmpStr = strrchr(filename, '.');
	if (pTmpStr != NULL)
	{
	    *pTmpStr = EOS;
	}

	strncpy(name, filename, maxNameLength);
}

/*
 * @brief
 * 		��ȡRTP�����б�
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
void rseGetRtpTaskList(T_TA_Packet *packet,UINT32 *outSize)
{
    UINT8 *inbuf = NULL;
    UINT8 *outbuf = NULL;
    UINT8 *ptr = NULL;
    UINT8 *buf = NULL;
    UINT32 len = 0;
    ulong_t taskId = 0;
    ulong_t rtpId = 0;
    char taskName[RSE_MAX_NAME_LEN+1] = {0};
    char taskStatus[RSE_MAX_NAME_LEN+1] = {0};
    static UINT32 count = 0;
    static UINT32 idx = 0;
    static ulong_t taskIdList[RSE_MAX_RTP_TASKS] = {0};

    /* �Ӱ��л�ȡʵ������ */
    inbuf = taGetPktUsrData(packet);

	/* ������ʼ��ַ */
	buf = &inbuf[RSE_ARG_START_OFFSET];

    /* �����������outbufΪinbuf */
    ptr = outbuf = inbuf;

    /* ��ȡRTP ID */
	n2l(buf,rtpId);

    /* ��ȡRTP�������б� */
    if(count == 0)
    {
    	/* У��RTP ID�Ƿ���Ч */
        if (RTP_VERIFIED_LOCK((WIND_RTP	*)rtpId) != OK)
        {
        	*outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d",TA_INVALID_INPUT);
        	return;
        }

        RTP_UNLOCK ((WIND_RTP*)rtpId);

        /* ��ȡRTP�������б� */
    	count = resGetRtpTaskIdList(rtpId, taskIdList);
    }

    /* ������ݰ� */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    /* ����ѭ���ɼ�����RTP��������Ϣ */
    while (idx < count)
    {
    	/* �ȼ��ʣ������Ƿ���Դ洢������Ϣ */
    	RSE_CHECK_OVER_PKTSIZE(ptr, outbuf, RSE_TASK_MAX_INFO_LEN);

        /* ���ûظ���� */
        len = sprintf((INT8 *)ptr,"%s","OK");
        ptr += (len+2);

        /* ȡ������ID */
        taskId = taskIdList[idx++];

		/* д������ID */
		RSE_PUTPKT_SHORT(RSE_TYPE_RTP_TASK_LIST, ptr);

		/* д�����ID */
		RSE_PUTPKT_LONG(rtpId, ptr);

		/* д���־λ */
		*ptr = 0;
		ptr += 1;

		/* ��������� */
		ZeroMemory(taskName,RSE_MAX_NAME_LEN+1);

		/* ��ȡ������ */

		strncpy(taskName, taTaskName(taskId), RSE_MAX_NAME_LEN);

		/* д��Ҫ��ʾ��ǩ :������*/
		len = sprintf((INT8 *)ptr,"%s", taskName);
		ptr += (len+1);

	    /* д��������� */
	    *ptr = RSE_NULL_OPER;
	    ptr += 1;

		/* д�����Ը��� */
		*ptr = RSE_RTP_TASK_INFO_ATTR_NUM;
		ptr += 1;

		/* д�����Ա�,����ID��������������״̬ */
	    RSE_PUTPKT_SHORT(RSE_ATTR_ID, ptr);
	#ifdef CONFIG_TA_LP64
	    len = sprintf((INT8 *)ptr,"%lx", taskId);
	#else
		len = sprintf((INT8 *)ptr,"%x", taskId);
	#endif
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
    }

    /* �����������Ϊ "$",��ʾRTP������Ϣ������� */
    if(idx == count)
    {
    	*ptr = RSE_END_STRING;
    	ptr += 1;
    	idx = count = 0;
    }

    /* ��������������ݴ�С */
    *outSize = ptr - outbuf;
}
