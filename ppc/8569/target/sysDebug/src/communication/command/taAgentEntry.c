/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * 修改历史:
 * 2013-06-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。 
 */

/*
 * @file:taAgentEntry.c
 * @brief:
 *             <li>TA逻辑通道消息处理入口实现</li>
 */

/************************头 文 件*****************************/
#include "taCommand.h"
#include "taMsgDispatch.h"
#include "taManager.h"
#include "taPacketComm.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 * 	   TA消息接口处理统一入口，各逻辑通道(二级代理)可以调用统一处理消息接口的命令
 * @param[in]:packet: 要处理的消息包
 * @param[in]:debugInfo: 调试会话信息
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
                             TA_ProcessPrepareCallback prepareCallback)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT32 cmdSize = 0;
    UINT32 bufSize = 0;
    T_TA_ReturnCode ret = TA_COMMAND_NOTFOUND;
    BOOL needReply = TA_NEED_REPLY;

    /* 倒转包头 packet */
    taSwapSenderAndReceiver(&(packet->header));

    /* 从包中获取实际数据 */
    inbuf = taGetPktUsrData(packet);

    /* 设置输出缓冲outbuf为inbuf */
    outbuf = inbuf;

    /* 获取实际数据长度 */
    cmdSize = taGetPktSize(packet);
    
    /* 设置输出缓冲大小为数据包体最大容量 */
    bufSize = taGetPktCapacity(packet);
    
    /* 如果实际数据长度大于0 */
    if (cmdSize > 0)
    {
        /* 如果包长度小于最大允许的包长度 */
        if (cmdSize < (TA_PKT_DATA_SIZE - 1) )
        {
            /* 设置输入buffer的最后一个字符为空字符 */
            inbuf[cmdSize] = (UINT8)'\0';
        }
    
        /* 处理命令前的回调接口<prepareCallback>不等于NULL */
        if (prepareCallback != NULL)
        {
            prepareCallback(inbuf,&(packet->header));
        }
    
        /* 调用taProcessCommand执行命令处理函数 */
        ret = taProcessCommand(debugInfo, inbuf, cmdSize, outbuf, &bufSize, &needReply, commandType,cmdTable,tableLength);
    }
	
    /* 命令不存在 */
    if (TA_COMMAND_NOTFOUND == ret)
    {
        outbuf[0]='\0';
        bufSize = 1;
    }
	
	/* 判断数据包是否需要回复 */
	if (TA_NEED_REPLY == needReply)
	{
		/* 发送数据包 */
		taCOMMSendPacket(packet, bufSize);
	}

    /* 需要异常返回  */
    if (TA_DB_NEED_IRET == ret)
    {
        /* 停止轮循收包,c或s命令执行该分支 */
        taStopLoopDispatch();

        /* 如果处理命令后无需回复时的回调接口不等于NULL */
        if (noReplyCallback != NULL)
        {
            noReplyCallback(&(packet->header));
        }
    }
	
    return (ret);
}
