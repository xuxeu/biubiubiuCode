/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  rseRtpShlLib.c
 * @brief
 *       功能：
 *       <li>查询RTP依赖的共享库</li>
 */

/************************头 文 件******************************/
#include "rseConfig.h"
#include "taskLib.h"
#include "rseCmdLib.h"
#include "rseInitLib.h"
#include "rtpLibP.h"
#include "rtpUtilLibP.h"
#include "rtpLibCommon.h"
#include "shlLibP.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/* 显示的最大共享库个数 */
#define RSE_MAX_DSP_SHLS 200

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief
 * 		获取RTP依赖的共享库ID列表
 * @param[in] rtpId: RTP ID
 * @return
 *      共享库个数
 */
int rseRtpShlIdList(RTP_ID rtpId, RTP_SHL_NODE *shlListId[])
{
	RTP_SHL_NODE *pRtpShlNode;
	int count = 0;

	pRtpShlNode = (RTP_SHL_NODE *) DLL_FIRST(&rtpId->rtpShlNodeList);

    while (pRtpShlNode != NULL)
    {
    	shlListId[count++] = pRtpShlNode;
    	pRtpShlNode = (RTP_SHL_NODE *) DLL_NEXT(pRtpShlNode);
	}

    return count;
}

/*
 * @brief
 * 		根据RTP ID获取RTP共享库
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
void rseGetRtpShlList(T_TA_Packet *packet,UINT32 *outSize)
{
	UINT8 *inbuf = NULL;
    UINT8 *outbuf = NULL;
	UINT8 *ptr = NULL;
	UINT8 *buf = NULL;
    UINT32 len = 0;
	UINT32 rtpId = 0;
	RTP_SHL_NODE *pRtpShlNode = NULL;
    char shlName[RSE_MAX_NAME_LEN+1] = {0};
    static UINT32 count = 0;
    static UINT32 idx = 0;
    static RTP_SHL_NODE *shlIdList[RSE_MAX_DSP_SHLS] = {0};

	 /* 从包中获取实际数据 */
	inbuf = taGetPktUsrData(packet);

	/* 参数起始地址 */
	buf = &inbuf[RSE_ARG_START_OFFSET];

	/* 设置输出缓冲outbuf为inbuf */
	ptr = outbuf = inbuf;

	/* 获取RTP ID */
	n2l(buf,rtpId);

    /* 获取RTP的共享库列表 */
    if(count == 0)
    {
    	/* 校验RTP ID是否有效 */
        if(RTP_VERIFIED_LOCK((WIND_RTP	*)rtpId) != OK)
        {
        	*outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d",TA_INVALID_INPUT);
        	return;
        }

        RTP_UNLOCK((WIND_RTP*)rtpId);

        /* 获取RTP的依赖的共享库列表 */
    	count = rseRtpShlIdList((RTP_ID)rtpId, shlIdList);
    }

    /* 清空数据包 */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    /* 进入循环采集所有共享库信息 */
    while (idx < count)
    {
    	/* 先检查剩余包长是否可以存储共享库信息 */
    	RSE_CHECK_OVER_PKTSIZE(ptr, outbuf, RSE_RTP_SHL_MAX_INFO_LEN);

    	/* 设置回复结果 */
		len = sprintf((INT8 *)ptr,"%s","OK");
		ptr += (len+2);

		pRtpShlNode = shlIdList[idx++];

		/* 写入类型ID */
		RSE_PUTPKT_SHORT(RSE_TYPE_RTP_SHL_LIST, ptr);

		/* 写入对象ID */
		RSE_PUTPKT_LONG(rtpId, ptr);

		/* 写入标志位 */
		*ptr = 0;
		ptr += 1;

		/* 清空共享库名 */
		ZeroMemory(shlName,RSE_MAX_NAME_LEN+1);

		/* 写入要显示标签 :共享库名 */
		rsePathTruncate(pRtpShlNode->pShlNode->name, shlName, RSE_MAX_NAME_LEN);
		len = sprintf((INT8 *)ptr,"%s,%s,%x",shlName, pRtpShlNode->pShlNode->name,pRtpShlNode->pShlNode->textBase);
		ptr += (len+1);

		/* 写入操作个数 */
		*ptr = RSE_NULL_OPER;
		ptr += 1;

		/* 写入属性个数 */
		*ptr = RSE_NULL_ATTR;
		ptr += 1;
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
