/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-10-12         彭元志，北京科银京成技术有限公司
 *                           创建该文件。
 */

/**
 * @file  rseRtpTaskLib.c
 * @brief
 *       功能：
 *       <li>获取RTP任务信息</li>
 */

/************************头 文 件******************************/
#include "rseConfig.h"
#include "rtpLibP.h"
#include "rtpUtilLibP.h"
#include "rtpLibCommon.h"
#include "ta.h"

/************************宏 定 义******************************/

/* RTP任务属性个数 */
#define RSE_RTP_TASK_INFO_ATTR_NUM 3

/* 显示RTP的最大任务数 */
#define RSE_MAX_RTP_TASKS	200

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief
 * 		获取RTP任务列表
 * @return
 *      任务个数
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
 * 		截断路径，获取BaseName
 * @return
 *      无
 */
void rsePathTruncate(char *path, char *name, UINT32 maxNameLength)
{
	char *filename = NULL;
	char *pTmpStr;
	char tmpStr[RSE_MAX_PATH_LENGTH + 1];

	/* 拷贝RTP路径 */
	strncpy (tmpStr, path, RSE_MAX_PATH_LENGTH);

	/* 移除路径 */
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

	/* 移除 后缀 */
	pTmpStr = strrchr(filename, '.');
	if (pTmpStr != NULL)
	{
	    *pTmpStr = EOS;
	}

	strncpy(name, filename, maxNameLength);
}

/*
 * @brief
 * 		获取RTP任务列表
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
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

    /* 从包中获取实际数据 */
    inbuf = taGetPktUsrData(packet);

	/* 参数起始地址 */
	buf = &inbuf[RSE_ARG_START_OFFSET];

    /* 设置输出缓冲outbuf为inbuf */
    ptr = outbuf = inbuf;

    /* 获取RTP ID */
	n2l(buf,rtpId);

    /* 获取RTP的任务列表 */
    if(count == 0)
    {
    	/* 校验RTP ID是否有效 */
        if (RTP_VERIFIED_LOCK((WIND_RTP	*)rtpId) != OK)
        {
        	*outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d",TA_INVALID_INPUT);
        	return;
        }

        RTP_UNLOCK ((WIND_RTP*)rtpId);

        /* 获取RTP的任务列表 */
    	count = resGetRtpTaskIdList(rtpId, taskIdList);
    }

    /* 清空数据包 */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    /* 进入循环采集所有RTP子任务信息 */
    while (idx < count)
    {
    	/* 先检查剩余包长是否可以存储任务信息 */
    	RSE_CHECK_OVER_PKTSIZE(ptr, outbuf, RSE_TASK_MAX_INFO_LEN);

        /* 设置回复结果 */
        len = sprintf((INT8 *)ptr,"%s","OK");
        ptr += (len+2);

        /* 取得任务ID */
        taskId = taskIdList[idx++];

		/* 写入类型ID */
		RSE_PUTPKT_SHORT(RSE_TYPE_RTP_TASK_LIST, ptr);

		/* 写入对象ID */
		RSE_PUTPKT_LONG(rtpId, ptr);

		/* 写入标志位 */
		*ptr = 0;
		ptr += 1;

		/* 清空任务名 */
		ZeroMemory(taskName,RSE_MAX_NAME_LEN+1);

		/* 获取任务名 */

		strncpy(taskName, taTaskName(taskId), RSE_MAX_NAME_LEN);

		/* 写入要显示标签 :任务名*/
		len = sprintf((INT8 *)ptr,"%s", taskName);
		ptr += (len+1);

	    /* 写入操作个数 */
	    *ptr = RSE_NULL_OPER;
	    ptr += 1;

		/* 写入属性个数 */
		*ptr = RSE_RTP_TASK_INFO_ATTR_NUM;
		ptr += 1;

		/* 写入属性表,任务ID、任务名、任务状态 */
	    RSE_PUTPKT_SHORT(RSE_ATTR_ID, ptr);
	#ifdef CONFIG_TA_LP64
	    len = sprintf((INT8 *)ptr,"%lx", taskId);
	#else
		len = sprintf((INT8 *)ptr,"%x", taskId);
	#endif
	    ptr += (len+1);

	    /* 写入任务名属性 */
	    RSE_PUTPKT_SHORT(RSE_ATTR_NAME, ptr);
	    len = sprintf((INT8 *)ptr,"%s", taskName);
	    ptr += (len+1);

		/* 清空任务状态字符串 */
		ZeroMemory(taskStatus,RSE_MAX_NAME_LEN+1);

	    /* 写入任务状态属性 */
	    RSE_PUTPKT_SHORT(RSE_ATTR_STATE, ptr);
	    taskStatusString(taskId, taskStatus);
	    len = sprintf((INT8 *)ptr,"%s", taskStatus);
	    ptr += (len+1);
    }

    /* 设置输出缓冲为 "$",表示RTP任务信息发送完毕 */
    if(idx == count)
    {
    	*ptr = RSE_END_STRING;
    	ptr += 1;
    	idx = count = 0;
    }

    /* 设置输出缓冲数据大小 */
    *outSize = ptr - outbuf;
}
