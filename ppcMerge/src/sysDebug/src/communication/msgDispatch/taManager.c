/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:taManager.c
 * @brief:
 *             <li>信息管理模块实现</li>
 */

/************************头 文 件******************************/
#include "string.h"
#include "taManager.h"
#include "taCommand.h"
#include "taMsgDispatch.h"
#include "taDebugMode.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/
/* 查询大小端信息 */
static T_TA_ReturnCode taQueryEndian(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/* 连接目标机 */
static T_TA_ReturnCode taLinkProcess(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/* 查询目标机体系结构 */
static T_TA_ReturnCode taQueryArch(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/* 查询当前调试模式 */
static T_TA_ReturnCode taQueryMode(T_DB_Info* debugInfo,
                                const UINT8 *inbuf,
                                UINT32 inSize,
                                UINT8 *outbuf,
                                UINT32 *outSize);

/************************模块变量******************************/

/************************全局变量******************************/

/* 查询命令表 */
T_TA_CommandTable taManagerCommandTable[TA_MANAGER_COMMANDNUM] =
{
    {"E",   taQueryEndian, TA_NEED_REPLY},/* 查询大小端 */
    {"e",   taLinkProcess, TA_NEED_REPLY},/* 连接目标机 */
    {"AC",  taQueryArch,   TA_NEED_REPLY},/* 查询体系统结构 */
    {"MD",  taQueryMode,   TA_NEED_REPLY},/* 查询当前调试模式 */
    {NULL,  NULL,          TA_NO_REPLY  }
};

/************************实现******************************/

/*
 * @brief:
 * 		查询通道入口函数
 * @param[in]:packet: 要处理的消息包
 * @return:
 *     TA_OK: 命令执行成功
 *     TA_FAIL: 命令执行失败
 *     TA_COMMAND_NOTFOUND: 命令没找到
 */
static T_TA_ReturnCode taManagerAgent(T_TA_Packet *packet)
{
    /* 调用taAgentEntry处理收到的消息包 */
    return (taAgentEntry(packet,NULL,COMMANDTYPE_QUERY,taManagerCommandTable,TA_MANAGER_COMMANDNUM,NULL,NULL));
}

/*
 * @brief:
 *     初始化TA查询模块
 * @return:
 *     无
 */
void taManagerInit(void)
{
    /* 安装查询通道TA_MANAGER的消息处理函数 */
    taInstallMsgProc(TA_MANAGER, taManagerAgent);
}

/*
 * @brief:
 *      处理查询大小端信息操作命令
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 */
static T_TA_ReturnCode taQueryEndian(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
	/* 获取大小端信息 */
	taEndianInfoGet(outbuf,outSize);

    return(TA_OK);
}

/*
 * @brief:
 *      连接目标机
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 */
static T_TA_ReturnCode taLinkProcess(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    /* 设置输出缓冲为"OK" */
    strcpy((char *)outbuf, "OK");

    /* 设置输出缓冲中的字符串长度 */
    *outSize = strlen((const char *)outbuf);

    return(TA_OK);
}

/*
 * @brief:
 *      查询目标机体系结构命令
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 */
static T_TA_ReturnCode taQueryArch(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    /* 获取目标机体系结构信息 */
    taArchInfoGet(outbuf,outSize);

    return (TA_OK);
}

/*
 * @brief:
 *     查询TA当前调试模式操作命令
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 执行成功
 */
static T_TA_ReturnCode taQueryMode(T_DB_Info* debugInfo,
                                const UINT8 *inbuf,
                                UINT32 inSize,
                                UINT8 *outbuf,
                                UINT32 *outSize)
{
    /* 当前为系统级调试 */
    if (DEBUGMODE_SYSTEM == taDebugModeGet())
    {
        strcpy((char *)outbuf, "OK;system");
    }
    else
    {
        /* 当前为任务级调试 */
        strcpy((char *)outbuf, "OK;task");
    }

    /* 设置输出缓冲中的字符串长度 */
    *outSize = strlen((const char *)outbuf);

    return (TA_OK);
}
