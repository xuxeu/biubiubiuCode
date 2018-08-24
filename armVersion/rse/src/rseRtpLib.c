/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  resRtpTaskLib.c
 * @brief
 *       功能：
 *       <li>RTP查询</li>
 */

/************************头 文 件******************************/
#include "rseConfig.h"
#include "rseCmdLib.h"
#include "rseRtpLib.h"
#include "taskLib.h"
#include "rtpLibP.h"
#include "rtpUtilLibP.h"
#include "rtpLibCommon.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/*
 * @brief
 *      提供RTP的消息处理
 * @param[in] packet: 接收到的数据包
 * @return
 *      None
 */
static T_TA_ReturnCode rseRtpHandler(T_TA_Packet* packet);

/*
 * @brief
 * 		获取所有RTP信息
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
static void rseGetRtpList(T_TA_Packet* packet,UINT32 *outSize);

/************************模块变量*****************************/

/* RTP个数 */
static UINT32 rtpIndex = 0;

/* 存放RTP ID列表 */
static ulong_t rtpIdList[MAX_RTP_NUM];

/************************全局变量*****************************/

/************************函数实现*****************************/

/**
  * @brief
  *     RTP消息处理函数初始化
  *@return
  *      None
  */
void rseRtpLibInit(void)
{
	/* 安装RTP模块处理函数 */
	rseRegisterModuleHandler(RSE_TYPE_RTP, rseRtpHandler);
}

/*
 * @brief
 *      提供RTP的消息处理
 * @param[in] packet: 接收到的数据包
 * @return
 *      None
 */
static T_TA_ReturnCode rseRtpHandler(T_TA_Packet *packet)
{
	UINT16 moduleId = 0;
    UINT8 *inbuf = NULL;
    UINT8 *outbuf = NULL;
    UINT32 outSize = 0;

    /* 从包中获取实际数据 */
    outbuf = inbuf = taGetPktUsrData(packet);

	/* 取模块ID */
	n2s(inbuf, moduleId);

	switch(moduleId)
	{
		case RSE_TYPE_RTP_LIST:
			rseGetRtpList(packet, &outSize);
			break;
		case RSE_TYPE_RTP_INFO:
			rseGetRtpModuleInfo(packet, &outSize);
			break;
		case RSE_TYPE_RTP_TASK_LIST:
			rseGetRtpTaskList(packet, &outSize);
			break;
		case RSE_TYPE_RTP_SHL_LIST:
			rseGetRtpShlList(packet, &outSize);
			break;
		default:
			outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d", TA_INVALID_OPERATE_ID);
			break;
	}

	/* 发送数据包 */
	taCOMMSendPacket(packet, outSize);

	return TA_OK;
}

/*
 * @brief
 *      将RTP ID 加入列表中
 * @param[in] rtpId: RTP ID
 * @param[in] rtpIdList: RTP ID列表
 * @return
 *      None
 */
void rseGetRtpId(ulong_t rtpId, ulong_t rtpIdList[])
{
	rtpIdList[rtpIndex++] = rtpId;
}

/*
 * @brief
 * 		获取系统中RTP列表
 * @return
 *      RTP个数
 */
int rseGetRtpIdList(ulong_t rtpIdList[])
{
	rtpIndex = 0;

	/* 获取RTP任务列表 */
	objEach(windRtpClass, (void*)rseGetRtpId, (ulong_t)rtpIdList, 0);

    return rtpIndex;
}

/*
 * @brief
 *      获取RTP状态字符串
 * @param[in] rtpId: RTP ID
 * @param[out] rtpStatusStr: 状态字符串
 * @return
 *      None
 */
void rseGetRtpStatusStr(RTP_ID rtpId, char *rtpStatusStr)
{
    char *statusStr = NULL;

    switch (RTP_STATE_GET(rtpId->status))
    {
    	case RTP_STATE_CREATE:
        {
        	if (IS_RTP_STATUS_STOP(rtpId->status))
        	{
                statusStr = "STATE_CREATE+S";
        	}
        	else if (IS_RTP_STATUS_ELECTED_DELETER(rtpId->status))
        	{
                statusStr = "STATE_CREATE+D";
        	}
        	else
        	{
                statusStr = "STATE_CREATE  ";
        	}
            break;
        }
        case RTP_STATE_NORMAL:
        {
        	if (IS_RTP_STATUS_STOP (rtpId->status))
        	{
                statusStr = "STATE_NORMAL+S";
        	}
        	else if (IS_RTP_STATUS_ELECTED_DELETER (rtpId->status))
        	{
                statusStr = "STATE_NORMAL+D";
        	}
        	else
        	{
                statusStr = "STATE_NORMAL  ";
        	}
            break;
        }
        case RTP_STATE_DELETE:
        {
            if (IS_RTP_STATUS_STOP (rtpId->status))
            {
                statusStr = "STATE_DELETE+S";
            }
            else if (IS_RTP_STATUS_ELECTED_DELETER (rtpId->status))
            {
                statusStr = "STATE_DELETE+D";
            }
            else
            {
                statusStr = "STATE_DELETE  ";
            }
            break;
        }
        default:
        {
            statusStr = "STATE_UNKNOWN ";
            break;
        }
    }

    sprintf((INT8 *)rtpStatusStr, "%s", statusStr);
}

/*
 * @brief
 * 		获取所有RTP信息
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
static void rseGetRtpList(T_TA_Packet* packet,UINT32 *outSize)
{
	UINT8 *inbuf = NULL;
	UINT8 *outbuf = NULL;
	UINT8 *ptr = NULL;
	UINT32 len = 0;
	ulong_t rtpId = 0;
	char rtpName[RSE_MAX_NAME_LEN+1] = {0};
	char rtpStatus[RSE_MAX_NAME_LEN+1] = {0};
	static UINT32 count = 0;
	static UINT32 idx = 0;

	/* 从包中获取实际数据 */
	inbuf = taGetPktUsrData(packet);

	/* 设置输出缓冲outbuf为inbuf */
	ptr = outbuf = inbuf;

    /* 获取RTP的任务列表 */
	
    if(count == 0)
    {
    	count = rseGetRtpIdList(rtpIdList);
    }

    /* 清空数据包 */
    memset(ptr,0, TA_PKT_DATA_SIZE);

	 /* 进入循环采集所有RTP任务信息 */
	while (idx < count)
	{
    	/* 先检查剩余包长是否可以存储任务信息 */
    	RSE_CHECK_OVER_PKTSIZE(ptr, outbuf, RSE_RTP_MAX_INFO_LEN);

		/* 取得RTP ID */
		rtpId = rtpIdList[idx++];

    	/* 校验RTP ID是否有效 */
        if (RTP_ID_VERIFY((WIND_RTP	*)rtpId) != OK)
        {
        	continue;
        }

        /* 设置回复结果 */
        len = sprintf((INT8 *)ptr,"%s","OK");
        ptr += (len+2);

		/* 写入类型ID */
		RSE_PUTPKT_SHORT(RSE_TYPE_RTP_LIST, ptr);

		/* 写入对象ID */
		RSE_PUTPKT_LONG(rtpId, ptr);

		/* 写入标志位 */
		*ptr = 1;
		ptr += 1;

		/* 清空RTP名 */
		ZeroMemory(rtpName,RSE_MAX_NAME_LEN+1);

		/* 写入要显示标签 :RTP名*/
		rsePathTruncate(((RTP_ID)rtpId)->pPathName,rtpName,RSE_MAX_NAME_LEN);
		len = sprintf((INT8 *)ptr,"%s", rtpName);
		ptr += (len+1);

		/* 写入操作个数 */
		*ptr = RSE_NULL_OPER;
		ptr += 1;

		/* 写入属性个数 */
		*ptr = 3;
		ptr += 1;

		/* 写入属性表,RTP ID、RTP名、RTP状态 */
		RSE_PUTPKT_SHORT(RSE_ATTR_ID, ptr);
	#ifdef CONFIG_TA_LP64
		len = sprintf((INT8 *)ptr,"%lx", rtpId);
	#else
		len = sprintf((INT8 *)ptr,"%x", rtpId);
	#endif
		ptr += (len+1);

		/* 写入任务名属性 */
		RSE_PUTPKT_SHORT(RSE_ATTR_NAME, ptr);
		len = sprintf((INT8 *)ptr,"%s", rtpName);
		ptr += (len+1);

		/* 清空状态字符串 */
		ZeroMemory(rtpStatus,RSE_MAX_NAME_LEN+1);

		/* 写入任务状态属性 */
		RSE_PUTPKT_SHORT(RSE_ATTR_STATE, ptr);
		rseGetRtpStatusStr((RTP_ID)rtpId, rtpStatus);
		len = sprintf((INT8 *)ptr,"%s",rtpStatus);
		ptr += (len+1);
	}

    /* 设置输出缓冲为 "$",表示RTP信息发送完毕 */
    if(idx == count)
    {
    	*ptr = RSE_END_STRING;
    	ptr += 1;
    	idx = count = 0;
    }
    /* 设置输出缓冲数据大小 */
    *outSize = ptr - outbuf;
}
