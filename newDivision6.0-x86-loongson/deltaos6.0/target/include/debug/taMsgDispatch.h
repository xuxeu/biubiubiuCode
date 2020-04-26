/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-07-01         彭元志，北京科银京成技术有限公司
 *                          创建该文件。 
 */

/*
 *@file:taMsgDispatch.h
 *@brief:
 *             <li>通信管理消息派发定义</li>
 */
#ifndef _TA_MSG_DISPATCH_H
#define _TA_MSG_DISPATCH_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taErrorDefine.h"
#include "taTypes.h"
#include "taPacket.h"

/************************宏定义********************************/

/* 定义二级代理个数*/
#define TA_MAX_AGENT_NUM  255

/************************类型定义******************************/

/* 定义消息处理函数*/
typedef T_TA_ReturnCode (*TaMsgProc)(T_TA_Packet* tpPacket);

/************************接口声明******************************/

/*
 * @brief:
 *  	启动循环接收消息
 * @return:
 *      无
 */
void taStartLoopDispatch(void);

/*
 * @brief:
 *      停止循环接收消息
 * @return:
 *      无
 */
void taStopLoopDispatch(void);

/*
 * @brief:
 *      安装通信通道消息处理函数
 * @param[in]:aid: 通道号
 * @param[in]:proc: 消息处理函数指针
 * @return:
 *     TA_OK: 安装成功;
 *     TA_INVALID_ID: 错误的通道号,正确的通道ID号范围应该是0~MAX_AGENT_NUM
 */
T_TA_ReturnCode taInstallMsgProc(UINT32 aid, TaMsgProc proc);

/**
 * @brief:
 *      消息接收派发函数，收到消息包后会根据接收通道ID调用对应安装的消息处理函数，对消息包进行处理
 * @return:
 *      TA_OK:收包成功
 *      TA_FAIL:收包失败
 */
T_TA_ReturnCode taMessageDispatch(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TA_MSG_DISPATCH_H */
