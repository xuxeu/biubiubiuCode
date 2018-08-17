/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * @file:sdaMain.c
 * @brief:
 *             <li>调试管理系统级调试初始化、消息处理</li>
 */

/************************头 文 件******************************/
#include <string.h>
#include "taCommand.h"
#include "taPacketComm.h"
#include "taMsgDispatch.h"
#include "taDebugMode.h"
#include "sdaCpuLib.h"
#include "taExceptionReport.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/* 异常上下文地址 */
ulong_t taSDAExpContext[MAX_CPU_NUM];

/* 异常上下文 */
T_DB_ExceptionContext taSDASmpExpContext[MAX_CPU_NUM];

/* 系统级调试会话  */
T_TA_Session  taSDASessInfo;
/************************函数实现******************************/

/*
 * @brief:
 *      SDA消息处理接口
 * @param[in]:packet: 收到的消息包
 * @return:
 *     TA_OK: 命令执行成功
 *     TA_FAIL: 命令执行失败 
 */
static T_TA_ReturnCode taSDAMessageProc(T_TA_Packet *packet)
{
    T_DB_Info* debugInfo = NULL;
    UINT32 desSAid = (UINT32)(TA_SYS_DEBUGER);
    T_TA_ReturnCode ret = TA_OK;

    /* 获取通道号 */
    desSAid = (UINT32)taGetPktHeadRecvChannel(&(packet->header));

    /* 获取通道号为desSAid的调试信息 */
    debugInfo = taSDADebugInfoGet();

    /* 保存GDB包头 */
    taSaveGdbPacketHead(taGetPktUsrData(packet),&(packet->header),&(taSDASessInfo.gdbPacketHead));

    /* 调用taAgentEntry通用消息处理接口,处理系统级调试消息包 */
    ret = taAgentEntry(packet,debugInfo,COMMANDTYPE_DEBUG,NULL,0,NULL,NULL);

    return (ret);
}

/*
 * @brief:
 *         获取调试信息
 * @return:
 *       系统级的调试信息
 */
T_DB_Info* taSDADebugInfoGet(void)
{
    return (&(taSDASessInfo.sessInfo));
}

/*
 * @brief:
 *     获取异常上下文指针
 * @return:
 * 		异常上下文指针
 */
T_DB_ExceptionContext* taSDAExpContextPointerGet(UINT32 cpuIndex)
{
    return (&taSDASmpExpContext[cpuIndex]);
}

/*
 * @brief:
 *      SDA初始化,负责初始化系统级调试会话，及其消息处理函数的安装
 * @return:
 *        TA_OK: 初始化成功
 */
T_TA_ReturnCode taSDAInit(void)
{
    UINT32 i = 0;

    /* 初始化数据包头 */
    taZeroPktHead(&(taSDASessInfo.gdbPacketHead));

    /* 安装消息处理函数  */
    taInstallMsgProc(TA_SYS_DEBUGER, taSDAMessageProc);

    /* 清空异常上下文  */
    ZeroMemory((void *)taSDAExpContextPointerGet(0), sizeof(T_DB_ExceptionContext) * MAX_CPU_NUM);

    /* 设置异常上下文地址  */
    for (i = 0; i < (INT32)(MAX_CPU_NUM); i++)
    {
        /* 设置异常上下文地址 */
    	taSDAExpContext[i] = (ulong_t)&taSDASmpExpContext[i];
    }

    /* 设置调试会话 */
    taSDASessInfo.sessInfo.sessionID = TA_SYS_DEBUGER_SESSION;
    taSDASessInfo.sessInfo.objID = 0;
    taSDASessInfo.sessInfo.curThreadId = 0;
    taSDASessInfo.sessInfo.status = DEBUG_STATUS_CONTINUE;
    taSDASessInfo.sessInfo.attachStatus = DEBUG_STATUS_DETACH;
    taSDASessInfo.sessInfo.ctxType = TA_CTX_SYSTEM;

    /* 设置当前调试会话上下文指针 */
    taSDASessInfo.sessInfo.context = taSDAExpContextPointerGet(GET_CPU_INDEX());

    /* 设置当前调试模式为系统级调试模式 */
    taDebugModeSet(DEBUGMODE_SYSTEM);

    /* 设置系统级异常处理函数 */
    _func_taSysExceptionHandler = taSDAExceptionHandler;

    /* 设置数据包中用户数据起始位置 */
    taSetPktOffset(&(taSDASessInfo.packet),TA_PROTOCOL_HEAD_SIZE+TA_PROTOCOL_CHECKSUM_SIZE);

    return (TA_OK);
}
