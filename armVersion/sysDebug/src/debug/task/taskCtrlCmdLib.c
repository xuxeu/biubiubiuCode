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
 * @file:rtpCtrlCmdLib.c
 * @brief:
 *             <li>绑定、解绑、模式切换等控制命令实现</li>
 */

/************************头 文 件*****************************/
#include "taRtIfLib.h"
#include "taskSessionLib.h"
#include "taDebugMode.h"
#include "taskCtrLib.h"
#include "ta.h"

/************************宏 定 义******************************/

/* 控制命令个数 */
#define TA_CTRL_COMMAND_NUM 5

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/*
 * @brief:
 * 		调试控制代理消息包处理函数
 * @param[in]:packet: 要处理的消息包
 * @return:
 *     TA_OK: 命令执行成功
 *     TA_COMMAND_NOTFOUND: 命令没找到
 *     TA_INVALID_PARAM: 非法的参数
 */
static T_TA_ReturnCode taCtrlCommandProc(T_TA_Packet *packet);

/*
 * @brief:
 *      绑定命令,命令格式"attach:objId:ctxType","objId"为任务ID或者实时进程ID,"ctxType"为调试上下文类型
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode taCtxAttach(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      解绑命令,命令格式"detach:cid:ctxType","cid"为通道ID,"ctxType"为调试上下文类型
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode taCtxDetach(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      添加调试绑定任务到调试会话中，该命令用于绑定多个任务到调试会话中,
 *      命令格式"taskAttach:cid:tid","cid"为通道ID,"tid"为任务ID
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode taCtxAttachTask(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      模式切换命令,命令格式"SMD:mode","mode"调试模式
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode taModeSwitch(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/************************模块变量*****************************/

/* 查询命令表 */
static T_TA_CommandTable taCtrlCommandTable[TA_CTRL_COMMAND_NUM] =
{
	{"attach",      taCtxAttach,           TA_NEED_REPLY},/* 绑定命令 */
    {"detach",      taCtxDetach,           TA_NEED_REPLY},/* 解绑命令 */
    {"taskAttach",  taCtxAttachTask,       TA_NEED_REPLY},/* 绑定任务命令 */
    {"SMD",         taModeSwitch,          TA_NEED_REPLY},/* 调试模式切换命令 */
    {NULL,          NULL,                  TA_NO_REPLY  }
};
/************************全局变量*****************************/

/************************实   现********************************/

/*
 * @brief:
 *     安装控制命令处理函数
 * @return:
 *     无
 */
void taCtrlCommandInit(void)
{
	/* 安装控制命令处理函数 */
	taInstallMsgProc(TA_CTRL_COMMAND, taCtrlCommandProc);
}

/*
 * @brief:
 * 		调试控制代理消息包处理函数
 * @param[in]:packet: 要处理的消息包
 * @return:
 *     TA_OK: 命令执行成功
 *     TA_COMMAND_NOTFOUND: 命令没找到
 *     TA_INVALID_PARAM: 非法的参数
 */
static T_TA_ReturnCode taCtrlCommandProc(T_TA_Packet *packet)
{
    /* 调用taAgentEntry处理收到的消息包 */
    return (taAgentEntry(packet,NULL,COMMANDTYPE_NONE,taCtrlCommandTable,
    		TA_CTRL_COMMAND_NUM,NULL,NULL));
}

/*
 * @brief:
 *      绑定命令,命令格式"attach:objId:ctxType","objId"为任务ID或者实时进程ID,"ctxType"为调试上下文类型
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode taCtxAttach(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 sid = 0;
    ulong_t objId = 0;
    TA_CTX_TYPE ctxType = 0;
    const UINT8 *ptr = NULL;
    UINT32 ret = TA_OK;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[6];

    /* 按"attach:objId:ctxType"格式解析命令参数 */
#ifdef CONFIG_TA_LP64
	if (!((*(ptr++) == ':') && (0 != hex2int64(&ptr, &objId) && (*(ptr++) == ':') && (0 != hex2int(&ptr,&ctxType)))))
	{
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
	}
#else
	if (!((*(ptr++) == ':') && (0 != hex2int(&ptr, &objId) && (*(ptr++) == ':') && (0 != hex2int(&ptr,&ctxType)))))
	{
		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
		return (TA_FAIL);
	}
#endif

	/* 调试绑定 */
	ret = TA_RT_CTX_ATTACH(ctxType,objId,&sid);
	if (TA_OK != ret)
	{
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",ret);
        return (TA_FAIL);
	}

    /* 设置输出缓冲为 "OK:channel",channel为调试通道号 */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK:%x", TA_GET_CHANNEL_ID(sid));

	return (TA_OK);
}

/*
 * @brief:
 *      解绑命令,命令格式"detach:cid:ctxType","cid"为通道ID,"ctxType"为调试上下文类型
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode taCtxDetach(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 cid = 0;
    UINT32 ret = 0;
    TA_CTX_TYPE ctxType = 0;
    const UINT8 *ptr = NULL;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[6];

    /* 按"detach:cid:ctxType"格式解析命令参数 */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&cid)) && (*(ptr++) == ':') && (0 != hex2int(&ptr,&ctxType))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 调试解绑 */
    ret = TA_RT_CTX_DETACH(ctxType,TA_GET_SESSION_ID(cid));
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",ret);
        return (TA_FAIL);
    }

    /* 设置输出缓冲为 "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      添加调试绑定任务到调试会话中，该命令用于绑定多个任务到调试会话中,命令格式为
 *      "taskAttach:cid:tid","cid"为通道ID,"tid"为任务ID
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode taCtxAttachTask(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 cid = 0;
    UINT32 ret = 0;
    ulong_t tid = 0;
    const UINT8 *ptr = NULL;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[10];


#ifdef CONFIG_TA_LP64
    /* 按"taskAttach:cid:tid"格式解析命令参数 */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&cid)) && (*(ptr++) == ':') && (0 != hex2int64(&ptr,&tid))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#else
    /* 按"taskAttach:cid:tid"格式解析命令参数 */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&cid)) && (*(ptr++) == ':') && (0 != hex2int(&ptr,&tid))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#endif


    /* 绑定任务 */
    ret = taAttachTask(TA_GET_SESSION_ID(cid), tid);
    if (TA_OK != ret)
    {
    	*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",ret);
    	return (TA_FAIL);
    }

    /* 设置输出缓冲为 "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      模式切换命令,命令格式"SMD:mode","mode"调试模式
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode taModeSwitch(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 ret = 0;
    UINT32 debugMode = 0;
    const UINT8 *ptr = NULL;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[3];

    /* 按"SMD:mode"格式解析命令参数 */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&debugMode))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 设置调试模式 */
    ret = taDebugModeSet(debugMode);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",ret);
        return (TA_FAIL);
    }

    /* 设置输出缓冲为 "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OKkkk");

    return (TA_OK);
}
