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
 *@file:dbStandardRsp.h
 *@brief:
 *             <li>标准RSP命令相关函数声明</li>
 */
#ifndef _DB_STANDARD_RSP_H
#define _DB_STANDARD_RSP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头 文 件******************************/
#include "taTypes.h"
#include "taCommand.h"
#include "taErrorDefine.h"
#include "sdaMain.h"

/************************宏定义*******************************/

/* 标准RSP命令表长度 */
#define TA_RSP_TABLE_LEN  22

/* 最大任务数，之前是1000，luosy改成了200 */
#define MAX_TASKS_NUM 200

/* 写内存的最大的数据长度 */
#define TA_WRITE_M_MAX_DATA_SIZE 590

/************************类型定义*****************************/

/************************接口声明*****************************/

/* RSP命令表 */
extern T_TA_CommandTable taRSPCmdTable[TA_RSP_TABLE_LEN];

/*
 * @brief:
 *      设置是否是暂停命令标志
 * @return:
 *     无
 */
void taDebugSetStopCommandTag(BOOL stopTag);

/*
 * @brief:
 *      获取暂停命令标志
 * @return:
 *     TRUE: 是暂停命令
 *     FALSE: 不是暂停命令
 */
BOOL taDebugGetStopCommandTag(void);

/*
 * @brief:
 *      处理查询当前产生异常的事件
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲,格式为?
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 */
T_TA_ReturnCode RspQueryEvent(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理M命令写内存
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲,参数格式为address,size:xxxxxx
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:bpOutbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspWriteMemory(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *     处理读内存操作命令
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspReadMemory(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理写二进制形式内存命令，GDB传送0x23(ASCII #),0x24(ASCII $)等扩展字符的格式为(0x7d 原始字符异或0x20),
 *      例如 0x7d以两个字节(0x7d 0x5d)传送。所以目标机段接受时需进行解析。如果当前字符为0x7d,则下个字符为(*src^ 0x20)
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲,参数格式为addr,size:xxxx
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspWriteBinaryMemory(T_DB_Info *debugInfo,
                                            const UINT8 *inbuf,
                                            UINT32 inSize,
                                            UINT8 *outbuf,
                                            UINT32 *outSize);

/*
 * @brief:
 *      处理二进制形式读内存命令
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲,参数格式为addr,size
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspReadBinaryMemory(T_DB_Info *debugInfo,
                                           const UINT8 *inbuf,
                                           UINT32 inSize,
                                           UINT8 *outbuf,
                                           UINT32 *outSize);

/*
 * @brief:
 *      处理写单个寄存器操作命令
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲,参数格式为regNum=value
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspWriteSingleRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理写所有寄存器操作命令
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspWriteAllRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理读所有寄存器操作命令
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspReadAllRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理读单个寄存器操作命令
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL: 执行失败
 */
T_TA_ReturnCode RspReadSingleRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理设置断点命令,在指定地址处设置断点(软件/硬件断点)
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲,命令参数格式为type,address,size
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *     TA_OK：执行成功
 *     TA_FAIL:执行失败
 */
T_TA_ReturnCode RspSetBreakpoint(T_DB_Info *debugInfo,
                                        const UINT8 *inbuf,
                                        UINT32 inSize,
                                        UINT8 *outbuf,
                                        UINT32 *outSize);

/*
 * @brief:
 *      处理删除断点操作命令。
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲,参数格式为type,address,size
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspRemoveBreakpoint(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理调试暂停命令
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *     TA_DB_NEED_IRET:执行成功
 */
T_TA_ReturnCode RspBreakDebugProc(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *    处理继续运行操作命令,返回到被调试程序继续运行
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *     TA_DB_NEED_IRET: 执行成功
 */
T_TA_ReturnCode RspContinue(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理单步操作命令,置位标志寄存器的单步标志，使程序运行时产生单步异常
 * @param[in]:tpInfo:调试会话信息
 * @param[in]:bpInbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]: bpOutbuf:存储操作结果的数据缓冲
 * @param[out]: outSize:存储操作结果数据长度
 * @return:
 *     TA_DB_NEED_IRET: 需要从中断异常返回
 */
T_TA_ReturnCode RspStep(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理"qC"命令,获取当前线程ID
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspGetCurThreadID(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理"qfThreadInfo"和"qsThreadInfo"命令,获取所有线程信息，调试器首先发送qfThreadInfo命令获取第一个线程信息
 *      然后发送qsThreadInfo命令获取剩下的线程信息，当目标机端回复'l'表示线程信息回复完毕
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspGetThreadIdList(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理"Hg"命令,设置GDB要对线程后续的操作,Hg表示调试器后续要发送g命令读取所有寄存器
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspSetThreadSubsequentOptHg(T_DB_Info *debugInfo,
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
T_TA_ReturnCode RspThreadIsActive(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      处理"qThreadExtraInfo,tid"命令,获取线程额外信息,额外信息包括线程ID,线程名和核信息
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspThreadExtraInfo(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      设置多线程信息，如果多线程标志为TRUE回复多线程信息，否则只回复当前产生异常的线程信息
 * @param[in]:tpInfo:调试会话信息
 * @param[in]:bpInbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:bpOutbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *     TA_FAIL: 操作失败
 *     TA_OK: 操作成功
 */
T_TA_ReturnCode RspSetMultiThreadInfo(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _DB_STANDARD_RSP_H */
