/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-05-26         彭元志，北京科银京成技术有限公司
 *                           创建该文件。
 */

/**
 * @file  rseCmdLib.c
 * @brief
 *       功能：
 *       <li>RSE命令处理</li>
 */

/************************头 文 件******************************/
#include "rseConfig.h"
#include "rseCmdLib.h"
#include "rseInitLib.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/* 模块函数处理表 */
static RseCommandHandler rseModuleCommandHandlerTable[RSE_MAX_TABLE] = {0};

/************************全局变量*****************************/

/************************函数实现*****************************/
/*
 * @brief
 *      获取对象模块处理函数
 *@param[in] inbuf: 请求包中的模块id
 *@return
 *     获得该模块处理函数
 */
static RseCommandHandler rseGetModuleHandler(UINT8* inbuf)
{
    UINT32 idx = 0;
    UINT16 moduleId = 0;

    /* 取模块ID */
    n2s(inbuf,moduleId);

    /* 根据模块ID计算模块处理函数表索引 */
    idx = moduleId - RSE_TYPEID_STAR;
    idx >>= 8;
    idx &= 0xff;

    /* 获取模块处理函数  */
    return (rseModuleCommandHandlerTable[idx]);
}

/*
 * @brief
 *      注册模块处理函数
 *@param[in] moduleId: 模块ID
 *@param[in] moduleHandler: 模块处理函数
 *@return
 *      TA_INVALID_ID:无效模块ID
 *		TA_OK：注册成功
 */
T_TA_ReturnCode rseRegisterModuleHandler(UINT16 moduleId, RseCommandHandler moduleCommandHandler)
{
    UINT32 idx = 0;

    /* 设置模块处理函数HASH表索引 */
    idx = moduleId - RSE_TYPEID_STAR;
    idx >>= 8;
    idx &= 0xff;

    if ((idx > RSE_MAX_TABLE)||((idx != 0) && (NULL != rseModuleCommandHandlerTable[idx])))
    {
        return (TA_INVALID_ID);
    }

    rseModuleCommandHandlerTable[idx] = moduleCommandHandler;

    return (TA_OK);
}

/*
 * @brief:
 * 		RSE消息包处理函数
 * @param[in]:packet: 要处理的消息包
 * @return:
 *     TA_OK: 命令执行成功
 *     TA_FAIL：命令执行失败
 */
static T_TA_ReturnCode rseCommandHandler(T_TA_Packet *packet)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT32 outSize = 0;
    RseCommandHandler moduleCommandHandler = NULL;

    /* 倒转包头 packet */
    taSwapSenderAndReceiver(&(packet->header));

    /* 从包中获取实际数据 */
    inbuf = taGetPktUsrData(packet);

    /* 设置输出缓冲outbuf为inbuf */
    outbuf = inbuf;

    /* 获取模块处理函数 */
    moduleCommandHandler = rseGetModuleHandler(inbuf);
    if (NULL == moduleCommandHandler)
    {
    	outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d",TA_INVALID_MODULE_ID);
		taCOMMSendPacket(packet, outSize);
        return (TA_FAIL);
    }

    /* 调用处理函数 */
    moduleCommandHandler(packet);

    return TA_OK;
}

/*
 * @brief:
 *     安装RSE消息处理函数
 * @return:
 *     无
 */
void rseCommandInit(void)
{
	/* 安装消息处理函数 */
	taInstallMsgProc(TA_RSE_AGENT, rseCommandHandler);
}
