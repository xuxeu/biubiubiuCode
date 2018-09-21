/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-23         彭元志，北京科银京成技术有限公司
 *                          创建该文件。 
 */

/*
 *@file:taCommand.h
 *@brief:
 *             <li>命令表管理</li>
 */
#ifndef _TACOMMAND_H
#define _TACOMMAND_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************头 文 件******************************/
#include "taTypes.h"
#include "taPacket.h"
#include "taErrorDefine.h"
#include "sdaMain.h"

/************************宏定义********************************/

/* 定义命令是否需要回复 */
#define TA_NEED_REPLY       1
#define TA_NO_REPLY        	0

/************************类型定义******************************/

/* 处理的命令类型  */
typedef enum
{
    COMMANDTYPE_QUERY = 0,/* 处理非调试查询类命令 */
    COMMANDTYPE_DEBUG,    /* 处理调试相关命令 */
    COMMANDTYPE_NONE      /* 未知，其它命令 */
} T_TA_CommandType;

/* TA对外命令处理函数类型 */
typedef T_TA_ReturnCode (*T_TA_CommandProc)
(
   T_DB_Info *debugInfo,  /* 调试会话上下文 */
   const UINT8 *inbuf,    /* 输入Buffer */
   UINT32 inSize,         /* 输入Buffer长度 */
   UINT8 *outbuf,         /* 输出Buffer */
   UINT32 *outSize        /* 输出Buffer长度 */
);

/* 定义TA命令表结构 */
typedef struct
{
    char *ubCmdStr;           /*命令字*/
    T_TA_CommandProc cmdProc; /*命令处理函数*/
    INT32 ubNeedReply;        /* 命令是否需要回复*/
} T_TA_CommandTable;

/* 命令处理前回调函数 */
typedef void (*TA_ProcessNoReplyCallback)(const T_TA_PacketHead *packetHead);

/* 命令处理后回调函数 */
typedef void (*TA_ProcessPrepareCallback)(const UINT8* inbuf,const T_TA_PacketHead *packetHead);

/* RSP命令类型 */
typedef T_TA_CommandProc  T_DB_RspCommandProc;

/* RSP命令表 */
typedef T_TA_CommandTable T_DB_RspCommandTable;

/************************接口声明******************************/

/*
 * @brief:
 *     解析执行调试器命令
 * @param[in]:   debugInfo: 调试代理的基本信息
 * @param[in]:   inbuf:输入的待处理的包体，无包头
 * @param[in]:   inSize: 包的大小
 * @param[in]:   commandType: 命令类型
 * @param[in]:   cmdTable: 在命令表 tpCmdTable 中查询命令
 * @param[out]:  outbuf: 处理完后待输出的包体，无包头
 * @param[out]:  outSize: 处理完后回复数据的字节长度
 * @param[out]:  isNeedReply: 是否需要回复
 * @return:
 *     TA_OK: 命令执行成功
 *     TA_FAIL: 命令执行失败
 *     TA_DB_NEED_IRET：需要从异常中返回
 *     TA_COMMAND_NOTFOUND: 命令没找到
 */
T_TA_ReturnCode taProcessCommand(T_DB_Info *debugInfo,
                                 UINT8 *inbuf,
                                 UINT32 inSize,
                                 UINT8 *outbuf,
                                 UINT32 *outSize,
                                 BOOL *isNeedReply,
                                 T_TA_CommandType commandType,
                                 T_TA_CommandTable *cmdTable,
                                 UINT32 tableLength);
                                    
/*
 * @brief:
 * 	   TA消息接口处理统一入口，各逻辑通道(二级代理)可以调用统一处理消息接口的命令
 * @param[in]:packet: 要处理的消息包
 * @param[in]:debugInfo 调试会话信息
 * @param[in]:commandType: 命令类型，命令为调试命令，查询命令，其它命令
 * @param[in]:cmdTable: 命令表
 * @param[in]:tableLength: 命令表长度
 * @param[in]:noReplyCallback: 处理命令后无需回复时的回调接口
 * @param[in]:prepareCallback: 处理命令前的回调接口
 * @return:
 *     TA_OK: 命令执行成功
 *     TA_FAIL: 命令执行失败
 *     TA_DB_NEED_IRET：需要从异常中返回
 *     TA_COMMAND_NOTFOUND: 命令没找到
 */
T_TA_ReturnCode taAgentEntry(T_TA_Packet *packet,
                             T_DB_Info *debugInfo,
                             T_TA_CommandType commandType,
                             T_TA_CommandTable *cmdTable,
                             UINT32 tableLength,
                             TA_ProcessNoReplyCallback noReplyCallback,
                             TA_ProcessPrepareCallback prepareCallback);

/************************实    现*******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _TACOMMAND_H */
