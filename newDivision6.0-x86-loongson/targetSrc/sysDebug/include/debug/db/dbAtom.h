/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                          创建该文件。 
 */

/*
 *@file:dbAtom.h
 *@brief:
 *             <li>调试原子操作头文件</li>
 */
#ifndef _DBATOM_H
#define _DBATOM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "dbContext.h"
#include "sdaMain.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************接口声明******************************/

/*
 * @brief:
 *     将异常向量号转化成标准POSIX信号表示形式
 * @param[in]:vector:异常向量号
 * @param[in]:context:调试异常上下文
 * @return:
 *     返回向量号对应的POSIX信号
 */
UINT32 taDebugVector2Signal(UINT32 vector, T_DB_ExceptionContext *context);

/*
 * @brief:
 *     读取被调试应用上下文中所有寄存器的值
 * @param[in]:context: 调试异常上下文
 * @param[out]: outBuf:输出数据缓冲，以十六进制字符串保存读取到的所有寄存器值
 * @param[out]:outBufferLen:输出缓冲的长度,输出缓冲长度需大于DB_PPC_REGISTER_SUM_SIZE(将寄存器的值转换成16进制字符串后，长度扩展为原来的2倍)
 * @return:
 *     TA_OK:操作成功
 *     TA_OUT_OF_BUFFER:输出缓冲长度不足
 */
T_TA_ReturnCode taDebugReadAllRegisters(T_DB_ExceptionContext *context, UINT8 *outBuf, UINT32 *outBufferLen);

/*
 * @brief
 *     读单个的用户寄存器
 * @param[in] context:异常上下文
 * @param[in] registerNum:寄存器号
 * @param[out] outBuf:输出buffer
 * @param[out] outBufferLen::输出缓冲的长度
 * @return
 *     TA_OK:操作成功
 *     TA_INVALID_PARAM:非法的参数
 */
T_TA_ReturnCode taDebugReadSingleRegister(T_DB_ExceptionContext *context, UINT32 registerNum, UINT8 *outBuf, UINT32 *outBufferLen);

/*
 * @brief
 *     写单个的用户寄存器
 * @param[in]: context:异常上下文
 * @param[in]: registerNum:寄存器号
 * @param[in]: inBuf:输入buffer
 * @return
 *     TA_OK:操作成功
 *     TA_FAIL:操作失败
 *     TA_INVALID_PARAM:非法的参数
 */
T_TA_ReturnCode taDebugWriteSingleRegister(T_DB_ExceptionContext *context, UINT32 registerNum, const UINT8 *inBuf);

/*
 * @brief:
 *     写被调试应用上下文中所有寄存器的值
 * @param[in]:context: 调试异常上下文
 * @param[in]:inBuf:输入数据缓冲，输入数据为写入所有寄存器的值(十六进值字符串表示)
 * @param[in]:inLen:输入数据长度，输入数据的长度必须为DB_PPC_REGISTER_SUM_SIZE(输入的十六进制字符串为所有寄存器长度总和的2倍)
 * @return:
 *     TA_OK:操作成功
 *     TA_OUT_OF_BUFFER:输入缓冲长度非法
 */
T_TA_ReturnCode taDebugWriteAllRegisters(T_DB_ExceptionContext *context, const UINT8 *inBuf, UINT32 inLen);

/*
 * @brief:
 *     清除被调试应用上下文中PS寄存器的单步标志位，确保返回到被调试应用后可以连续运行
 * @param[in]:context: 调试异常上下文
 * @return:
 *     TA_OK: 操作成功
 */
T_TA_ReturnCode taDebugContinue(T_DB_ExceptionContext *context);

/*
 * @brief:
 *     设置被调试应用上下文中PS寄存器的单步标志位,确保返回到被调试应用运行执行一条指令以后触发单步异常
 * @param[in]:context:调试异常上下文
 * @return:
 *     TA_OK:操作成功
 */
T_TA_ReturnCode taDebugStep(T_DB_ExceptionContext *context);

/*
 * @brief:
 *    获取被调试程序当前断点触发的地址，当被调试程序触发断点异常后,PC寄存器的值将指向当前断点的地址
 * @param[in]:context: 调试异常上下文
 * @param[out]:address:存储触发断点指令的地址
 * @return:
 *     TA_OK:操作成功
 */
T_TA_ReturnCode taDebugGetCurrentBpAddress(T_DB_ExceptionContext *context, UINT32 *address);

/*
 * @brief:
 *     跳过产生异常指令
 * @param[in]: context:调试异常上下文
 * @return:
 *     TA_OK:操作成功
 */
T_TA_ReturnCode taDebugJumpOverTrap(T_DB_ExceptionContext *context);

/*
 * @brief:
 *      体系结构初始化
 * @return:
 *     TRUE: 初始化成功
 */
BOOL taDebugArchInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DBATOM_H */
