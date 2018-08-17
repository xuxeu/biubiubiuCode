/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-10-23         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:taExceptionReport.h
 *@brief:
 *             <li>异常上报相关声明</li>
 */
#ifndef _TA_EXCEPTION_REPORT_H
#define _TA_EXCEPTION_REPORT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头 文 件******************************/
#include "ta.h"

/************************宏定义*******************************/

#define	TA_KERNEL_IS_INITIALIZED()	\
	(taTaskIdSelf() == 0 ? FALSE : TRUE)

/************************类型定义*****************************/

/*
 * @brief:
 *     保存GDB信息包头，当接收的命令为‘c’'s' 'r' '!'则记录数
 *     据包包头到调试会话信息的GDB包头中，当产生异常进行主动发包需要用到GDB包头信息
 * @param[in]:inbuf:输入数据缓冲
 * @param[in]:gdbPacketHead:接收到的GDB包头
 * @param[out]:packetHead:保存GDB包头
 * @return:
 *         无
 */
void taSaveGdbPacketHead(const UINT8 *inbuf, const T_TA_PacketHead *gdbPacketHead, T_TA_PacketHead *packetHead);

/*
 * @brief:
 *     设置异常停止回复信息
 * @param[out]:outbuf:输出数据缓冲，保存异常回复信息
 * @param[in]:signal:信号值
 * @param[in]:taskId:任务ID
 * @return:
 *         无
 */
void taSetExceptionStopReply(UINT8 *outbuf, UINT32 signal, ulong_t taskId);

/*
 * @brief:
 *      上报调试异常给GDB,需要将体系结构相关的调试异常向量号转换为POSIX异常信号上报给GDB
 * @param[in]:session: 调试会话
 * @param[in]:vector: 调试异常向量号
 * @return:
 *      无
 */
void taReportExceptionToDebugger(T_TA_Session *session, UINT32 vector);

/************************接口声明*****************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
