/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-07-26         彭元志，北京科银京成技术有限公司
 *                           创建该文件。
 */

/**
 * @file  rseCmdLib.h
 * @brief
 *       功能：
 *       <li>RSE命令处理相关声明</li>
 */
#ifndef RSE_CMDLIB_H_
#define RSE_CMDLIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头 文 件******************************/
#include "rseConfig.h"
#include "ta.h"

/************************宏 定 义******************************/

/* HASH TABLE长度 */
#define RSE_MAX_TABLE    256
#define RSE_TYPEID_STAR  0x0300

/************************类型定义*****************************/

/* 定义消息处理函数 */
typedef T_TA_ReturnCode (*RseCommandHandler)(T_TA_Packet* tpPacket);

/************************接口声明*****************************/

/*
 * @brief
 *      注册模块处理函数
 *@param[in] moduleId: 模块ID
 *@param[in] moduleHandler: 模块处理函数
 *@return
 *      TA_INVALID_ID:无效模块ID
 *		TA_OK：注册成功
 */
T_TA_ReturnCode rseRegisterModuleHandler(UINT16 moduleId, RseCommandHandler moduleCommandHandler);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RSE_CMDLIB_H_ */
