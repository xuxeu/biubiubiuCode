/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-11-27         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:taskStandardRsp.c
 * @brief:
 *             <li>任务级相关RSP命令实现，任务级与系统级相关RSP处理方式不同，所以单独剥离出来</li>
 */

/************************头 文 件*****************************/
#include "dbStandardRsp.h"
#include "taExceptionReport.h"
#include "taskStandardRsp.h"
#include "taskCtrLib.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明******************************/

/************************模块变量******************************/

/************************全局变量******************************/

/* NOTE:根据GJB5369标准,数组必须有下标,如果增加或删除了命令表，请注意修改配置 */
T_DB_RspCommandTable taTaskRSPCmdTable[TA_TASK_RSP_TABLE_LEN] =
{
    {"?",                RspQueryEvent,                TA_NEED_REPLY},
    {"M",                RspWriteMemory,               TA_NEED_REPLY},
    {"m",                RspReadMemory,                TA_NEED_REPLY},
    {"X",                RspWriteBinaryMemory,         TA_NEED_REPLY},
    {"x",                RspReadBinaryMemory,          TA_NEED_REPLY},
    {"P",                RspWriteSingleRegister,       TA_NEED_REPLY},
    {"G",                RspWriteAllRegister,          TA_NEED_REPLY},
    {"g",                RspReadAllRegister,           TA_NEED_REPLY},
    {"p",                RspReadSingleRegister,        TA_NEED_REPLY},
    {"Z",                RspSetBreakpoint,             TA_NEED_REPLY},
    {"z",                RspRemoveBreakpoint,          TA_NEED_REPLY},
    {"!",                taTaskRspPauseProc,           TA_NEED_REPLY},
    {"c",                RspContinue,                  TA_NO_REPLY  },
    {"s",                RspStep,                      TA_NO_REPLY  },
    {"qC",    	         RspGetCurThreadID,            TA_NEED_REPLY},
    {"qfThreadInfo",     taTaskRspGetThreadIdList,     TA_NEED_REPLY},
    {"qsThreadInfo",     taTaskRspGetThreadIdList,     TA_NEED_REPLY},
    {"Hg",    	         RspSetThreadSubsequentOptHg,  TA_NEED_REPLY},
    {"T",    	         taTaskRspThreadIsActive,      TA_NEED_REPLY},
    {"qThreadExtraInfo", RspThreadExtraInfo,           TA_NEED_REPLY},
    {"autoAttachTask",   taTaskAutoAttachTask,         TA_NEED_REPLY},
    {NULL,               NULL,                         TA_NO_REPLY  }
};

/************************实   现*******************************/

/*
 * @brief:
 *      处理任务级调试暂停命令
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *     TA_DB_NEED_IRET:执行成功
 */
T_TA_ReturnCode taTaskRspPauseProc(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	/* 如果当前处于异常状态，则不执行暂停操作 */
	if (!TA_DEBUG_IS_CONTINUE_STATUS(debugInfo))
	{
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
	}

	/* 任务级调试暂停 */
	if(OK != taTaskPause(debugInfo->sessionID))
	{
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_PAUSE_TASK_FAIL);
        return (TA_FAIL);
	}

	/* 设置异常回复信息 */
	taSetExceptionStopReply(outbuf, SIGNAL_TRAP, TA_GET_CUR_THREAD_ID(debugInfo));

	/* 设置输出数据大小 */
	*outSize = strlen((const char *)outbuf);

    return (TA_OK);
}

/*
 * @brief:
 *      处理"qfThreadInfo"和"qsThreadInfo"命令,获取调试会话中绑定的调试任务ID列表，
 *      调试器首先发送 qfThreadInfo命令获取第一个线程信息，然后发送qsThreadInfo命令
 *      获取剩下的线程信息，当目标机端回复'l'表示线程信息回复完毕
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode taTaskRspGetThreadIdList(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    static ulong_t threadIdList[MAX_TASKS_NUM] = {0};
    static UINT32 count = 0;
    static UINT32 index = 0;
	
    /* 如果是qfThreadInfo命令，表示获取第一个线程ID，通过taskIdListGet获取线程ID列表  */
    if (!strncmp((const char *)inbuf, "qfThreadInfo", strlen("qfThreadInfo")))
    {
		/* 获取调试会话中绑定的线程ID列表 */
		count = taGetTaskIdList(debugInfo->sessionID, threadIdList, MAX_TASKS_NUM);
    }

    /* 线程ID列表结束或者无线程 */
	if (count == index || count == 0)
	{
		/* 设置"l"到输出缓冲 */
		outbuf[0] = 'l';
		index = 0;
		*outSize = 1;
	}
	else
	{
		/* 输出剩余线程信息，设置"m<threadId>"到输出缓冲 */
	#ifdef CONFIG_TA_LP64
	    *outSize = (UINT32)sprintf((INT8*)(outbuf), "m%lx",threadIdList[index++]);
	#else
		*outSize = (UINT32)sprintf((INT8*)(outbuf), "m%x",threadIdList[index++]);
	#endif
	}

    return (TA_OK);
}

/*
 * @brief:
 *      处理"T"命令,查看线程是否处于激活状态
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode taTaskRspThreadIsActive(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	const UINT8 *ptr = &inbuf[1];
	ulong_t threadId = 0;

    /* 按格式"T<threadID>"解析参数 */

#ifdef CONFIG_TA_LP64
    if (!(0 != hex2int64(&ptr, &threadId)))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }
#else
	if (!(0 != hex2int(&ptr, &threadId)))
	{
		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
		return (TA_FAIL);
	}
#endif

    /* 线程是否处于激活状态 */
	if(threadId !=0)
	{
		threadId = 0xffff800000000000+threadId;
	}

    if (taTaskIdVerify(threadId) != OK)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* 设置"OK"到输出缓冲 */
    *outSize = (UINT32)sprintf((INT8 *)outbuf, "OK");

	return (TA_OK);
}

/*
 * @brief:
 *      处理"autoAttachTask"命令，设置调试会话的自动绑定任务标志，如果设置
 *      为TRUE则由绑定调试任务创建的任务将自动绑定到调试会话
 * @param[in]:tpInfo:调试会话信息
 * @param[in]:bpInbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:bpOutbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *     TA_FAIL: 操作失败
 *     TA_OK: 操作成功
 */
T_TA_ReturnCode taTaskAutoAttachTask(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    const UINT8* ptr = NULL;
    UINT32 value = 0;

    /* 跳过操作命令的命令字 */
    ptr = &(inbuf[14]);

    /* 按格式解析操作参数 */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&value))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 设置任务自动绑定标志 */
    debugInfo->taskIsAutoAttach = value;

    /* 设置"OK"到输出缓冲 */
    *outSize = (UINT32)sprintf((INT8 *)outbuf, "OK");

	 return (TA_OK);
}
