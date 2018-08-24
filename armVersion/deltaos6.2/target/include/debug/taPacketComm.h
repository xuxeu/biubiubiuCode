/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-26         彭元志，北京科银京成技术有限公司
 *                          创建该文件。 
 */

/*
 *@file:taPacketComm.h
 *@brief:
 *             <li>消息通信接口定义</li>
 */
#ifndef _TA_PACKETCOMM_H
#define _TA_PACKETCOMM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "taPacket.h"
#include "taDevice.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************接口声明******************************/

/**
 * @brief:
 *      设备操作表初始化
 * @param[in]:interface: 设备操作表
 * @return:
 *        TA_OK:操作成功
 *        TA_INVALID_PARAM:无效的输入参数
 */
T_TA_ReturnCode taCOMMInit(T_TA_COMMIf *interface);

/*
 * @brief:
 *      接收消息包函数
 * @param[in]:packet:存储收到的消息包指针
 * @return:
 *      同taGetpkt的返回值
 */
T_TA_ReturnCode taCOMMGetPkt(T_TA_Packet *packet);

/*
 * @brief:
 *      发送消息包函数
 * @param[in]:packet:发送的消息包指针
 * @return:
 *      同taPutpkt的返回值
 */
T_TA_ReturnCode taCOMMPutPkt(T_TA_Packet *packet);

/*
 * @brief:
 *      设置包大小并发送消息包
 * @param[in]:packet :要发送的的消息包指针
 * @param[in]:packetSize :包大小
 * @return:
 *      taCOMMPutPkt的返回值
 */
T_TA_ReturnCode taCOMMSendPacket(T_TA_Packet *packet,UINT32 packetSize);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /*_TA_PACKETCOMM_H */
