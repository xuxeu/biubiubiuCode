/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-10-13         彭元志，北京科银京成技术有限公司
 *                           创建该文件。
 */

/**
 * @file  rseRtpLib.h
 * @brief
 *       功能：
 *       <li>RTP命令处理相关声明</li>
 */
#ifndef RSE_RTPLIB_H_
#define RSE_RTPLIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头 文 件******************************/
#include "ta.h"

/************************宏 定 义******************************/

/* RTP显示的最大个数 */
#define MAX_RTP_NUM 1000

/************************类型定义*****************************/

/************************接口声明*****************************/

/*
 * @brief
 * 		提供RTP任务的消息处理
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      TA_OK:操作成功
 */
T_TA_ReturnCode rseRtpTaskHandler(T_TA_Packet *packet,UINT32 *outSize);

/*
 * @brief
 * 		根据RTP ID获取RTP模块信息
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
void rseGetRtpModuleInfo(T_TA_Packet* packet,UINT32* outSize);

/*
 * @brief
 * 		根据RTP ID获取RTP共享库
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
void rseGetRtpShlList(T_TA_Packet *packet,UINT32 *outSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RSE_RTPLIB_H_ */
