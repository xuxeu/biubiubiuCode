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
 *@file:taskStandardRsp.h
 *@brief:
 *             <li>任务标准RSP命令相关函数声明</li>
 */
#ifndef _TASK_STANDARD_RSP_H
#define _TASK_STANDARD_RSP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头 文 件******************************/
#include "ta.h"

/************************宏定义*******************************/

/* 任务级标准RSP命令表长度 */
#define TA_TASK_RSP_TABLE_LEN  22

/************************类型定义*****************************/

/************************接口声明*****************************/

/* 任务级RSP命令表外部声明 */
extern T_DB_RspCommandTable taTaskRSPCmdTable[TA_TASK_RSP_TABLE_LEN];

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
                   UINT32 *outSize);

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
                   UINT32 *outSize);

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
                   UINT32 *outSize);

/*
 * @brief:
 *      设置调试会话的自动绑定任务标志，如果设置为TRUE则由绑定调试任务创建的任务
 *      将自动绑定到调试会话
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
                   UINT32 *outSize);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _TASK_STANDARD_RSP_H */
