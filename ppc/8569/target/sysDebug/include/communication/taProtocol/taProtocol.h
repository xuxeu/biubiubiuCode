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
 *@file:taProtocol.h
 *@brief:
 *             <li>消息包通信抽象协议定义</li>
 */
#ifndef _TA_PROTOCOL_H
#define _TA_PROTOCOL_H

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

/* 协议操作表结构体 */
typedef struct T_TA_TREATY_OPS_Struct
{
    /* 下一个操作表 */
    struct T_TA_TREATY_OPS_Struct *next;

    /* 协议类型 */
    T_TA_COMMType type;

    /* 数据包中数据发送的偏移量 */
    UINT32 offset;

    /* 指向发包函数的函数指针 */
    T_TA_ReturnCode (*putpkt)(T_TA_COMMIf *commIf, T_TA_Packet* tpPacket);

    /* 指向收包函数的函数指针 */
    T_TA_ReturnCode (*getpkt)(T_TA_COMMIf *commIf, T_TA_Packet* tpPacket);
} T_TA_TREATY_OPS;

/************************接口声明******************************/

/**
 * @brief:
 *      向协议操作表链中添加一个协议操作表
 * @param[in]:optable: 指向协议操作表的指针
 * @return:
 *      TA_OK: 添加协议成功
 *      TA_INVALID_PARAM:无效的输入参数
 */
T_TA_ReturnCode taTreatyAdd(T_TA_TREATY_OPS *optable);

/**
 * @brief:
 *      通过物理通道发送数据包
 * @param[in]: commIf: 调用者向TA通信协议栈提供的设备读写操作接口
 * @param[in]: packet: 需要发送的数据包内容
 * @return:
 *      TA_OK：接收数据成功
 *      TA_INVALID_PROTOCOL:协议还没有安装 *
 *      TA_INVALID_PARAM: 输入的缓冲指针不合法
 *      TA_FAIL:发送数据失败
 */
T_TA_ReturnCode taPutpkt(T_TA_COMMIf *commIf, T_TA_Packet *packet);

/**
 * @brief:
 *      通过物理通道接收数据包，收到任何有效数据包都立即返回
 * @param[in]:commIf:  通信设备操作表
 * @param[out]:packet: 数据包内容
 * @return:
 *      TA_OK：接收数据成功
 *      TA_INVALID_PROTOCOL:协议还没有安装
 *      TA_INVALID_PARAM: 输出缓冲指针不合法
 *      TA_FAIL:收包失败
 */
T_TA_ReturnCode taGetpkt(T_TA_COMMIf *commIf, T_TA_Packet *packet);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_TA_PROTOCOL_H */
