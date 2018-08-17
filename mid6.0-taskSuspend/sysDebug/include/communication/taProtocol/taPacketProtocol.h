/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:taPacketProtocol.h
 *@brief:
 *             <li>消息包通信抽象协议定义</li>
 */
#ifndef _TA_PACKET_PROTOCOL_H
#define _TA_PACKET_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/

/************************宏定义********************************/

/* 同步允许范围 */
#define TA_SYNC_RANGE 	((UINT32)(0x1))

/* 发送序列号范围 */
#define TA_TX_PACKET_MAX ((UINT32)(0x3fffffff))
#define TA_TX_PACKET_MIN ((UINT32)(0x00000001))

/* 接收列号范围 */
#define TA_RX_PACKET_MAX TA_TX_PACKET_MAX
#define TA_RX_PACKET_MIN TA_TX_PACKET_MIN

/* 目标机端起始接收, 发送序号 */
#define TA_RX_START ((UINT32)(0x40000002))
#define TA_TX_START ((UINT32)(0x40000001))

/* 主机端起始发送的序号 */
#define TA_TS_TX_START (UINT32)0x40000001

/************************类型定义******************************/

/************************接口声明******************************/

/*
 * @brief
 *   UDP 消息协议的初始化,向协议操作表链中添加消息协议操作表
 * @return:
 *    无
 */
void taTreatyUDPInit(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_TA_PACKET_PROTOCOL_H */
