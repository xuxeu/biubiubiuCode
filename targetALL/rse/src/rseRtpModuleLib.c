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
 * @file  rseRtpModuleLib.c
 * @brief
 *       功能：
 *       <li>获取RTP模块信息</li>
 */

/************************头 文 件******************************/
#include "rseConfig.h"
#include "rtpLibP.h"
#include "rtpUtilLibP.h"
#include "rtpLibCommon.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief
 * 		根据RTP ID获取RTP模块信息
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
void rseGetRtpModuleInfo(T_TA_Packet *packet,UINT32 *outSize)
{
	UINT8 *inbuf = NULL;
    UINT8 *outbuf = NULL;
	UINT8 *ptr = NULL;
	UINT8 *buf = NULL;
    UINT32 len = 0;
	UINT32 rtpId = 0;
    char rtpName[RSE_MAX_NAME_LEN+1] = {0};
	RTP_DESC rtpInfo;

	 /* 从包中获取实际数据 */
	inbuf = taGetPktUsrData(packet);

	/* 设置输出缓冲outbuf为inbuf */
	ptr = outbuf = inbuf;

	/* 参数起始地址 */
	buf = &inbuf[RSE_ARG_START_OFFSET];

	/* 获取RTP ID */
	n2l(buf,rtpId);

    /* 获取RTP信息 */
    if(OK != rtpInfoGet((RTP_ID)rtpId, &rtpInfo))
    {
    	*outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d", TA_INVALID_INPUT);
    	return;
    }

    /* 清空数据包 */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    /* 设置回复结果 */
	len = sprintf((INT8 *)ptr,"%s","OK");
    ptr += (len+2);

	/* 写入类型ID */
	RSE_PUTPKT_SHORT(RSE_TYPE_RTP_INFO, ptr);

	/* 写入对象ID */
	RSE_PUTPKT_LONG(rtpId, ptr);

	/* 写入标志位 */
	*ptr = 0;
	ptr += 1;

	/* 清空RTP名 */
	ZeroMemory(rtpName,RSE_MAX_NAME_LEN+1);

	/* 写入要显示标签 :RTP名*/
	rsePathTruncate(rtpInfo.pathName, rtpName, RSE_MAX_NAME_LEN);
	len = sprintf((INT8 *)ptr,"%s,%s,%x",rtpName, rtpInfo.pathName,(UINT32)((WIND_RTP *)rtpId)->binaryInfo.textStart);
	ptr += (len+1);

	/* 写入操作个数 */
	*ptr = RSE_NULL_OPER;
	ptr += 1;

	/* 写入属性个数 */
	*ptr = RSE_NULL_ATTR;
	ptr += 1;

	/* 设置输出缓冲数据大小 */
	*outSize = ptr - outbuf;
}
