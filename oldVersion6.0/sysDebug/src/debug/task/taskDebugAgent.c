/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-08-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:taskDebugAgent.c
 * @brief:
 *             <li>任务级调试代理入口函数</li>
 */

/************************头 文 件*****************************/
#include "ta.h"
#include "taskSessionLib.h"
#include "taskStandardRsp.h"
#include "taRtIfLib.h"
#include "taExceptionReport.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************实   现********************************/

/*
 * @brief:
 *     任务级调试代理入口函数
 * @param[in]: packet:包含调试操作命令消息包
 * @return:
 *     TA_OK: 命令执行成功
 *     TA_FAIL: 命令执行失败
 *     TA_DB_NEED_IRET：需要从异常中返回
 *     TA_COMMAND_NOTFOUND: 命令没找到
 */
T_TA_ReturnCode taTaskDebugAgent(T_TA_Packet *packet)
{
    T_DB_Info *dbInfo = NULL;
    UINT32 sid = 0;
    T_TA_ReturnCode ret = TA_OK;
    UINT16 channelID = taGetPktHeadRecvChannel(&(packet->header));

    /* 根据调试通道号获取被调试会话ID */
    sid = TA_GET_SESSION_ID(channelID);

    /* 获取调试会话信息 */
    dbInfo = TA_GET_SESSION_INFO(sid);

    /* 保存GDB包头 */
    taSaveGdbPacketHead(taGetPktUsrData(packet),&(packet->header),&(TA_GET_DEBUG_SESSION(sid)->gdbPacketHead));

    /* 调用taAgentEntry处理收到的调试消息包 */
    ret = taAgentEntry(packet,dbInfo,COMMANDTYPE_QUERY,taTaskRSPCmdTable,TA_TASK_RSP_TABLE_LEN,NULL,NULL);

    /* 需要异常返回  */
    if (TA_DB_NEED_IRET == ret)
    {
    	/* 如果当前处于调试暂停态，则回写上下文信息，并设置调试会话状态为运行态 */
    	if(TA_DEBUG_IS_PAUSE_STATUS(dbInfo))
    	{
    		/* 回写上下文，否则暂停后无法执行单步等操作 */
    		taTaskContextSet(TA_GET_CUR_THREAD_ID(dbInfo),TA_GET_EXP_CONTEXT(sid));

        	/* 设置会话的状态为DEBUG_STATUS_CONTINUE，暂停时设置为暂停态，恢复时应该设置为运行状态  */
        	TA_SET_DEBUG_STATUS(TA_GET_SESSION_INFO(sid),DEBUG_STATUS_CONTINUE);
    	}

    	/* 恢复所有任务运行 */
    	TA_RT_CTX_RESUME(dbInfo->ctxType,dbInfo->sessionID);
    }

    return ret;
}
