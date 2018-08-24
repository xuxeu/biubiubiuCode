/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file:taExceptionReport.c
 * @brief:
 *             <li>调试异常上报处理</li>
 */

/************************头 文 件******************************/
#include "taExceptionReport.h"
#include "dbAtom.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

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
void taSaveGdbPacketHead(const UINT8 *inbuf, const T_TA_PacketHead *gdbPacketHead, T_TA_PacketHead *packetHead)
{
    /* 输入缓冲的数据为"c","r","s","!"中其中一个 */
    if ((( 'c' == inbuf[0])
        || ( 's' == inbuf[0] )
        || ( 'r' == inbuf[0] )
        || ( '!' == inbuf[0] )))
    {
        /* 调用taSetPktHeadReceiver保存接收者ID，以便产生异常进行主动发包需要用到GDB包头信息 */
        taSetPktHeadReceiver(packetHead,taGetPktHeadSender(gdbPacketHead));
    }
}

/*
 * @brief:
 *     设置异常停止回复信息
 * @param[out]:outbuf:输出数据缓冲，保存异常回复信息
 * @param[in]:signal:信号值
 * @param[in]:taskId:任务ID
 * @return:
 *         无
 */
void taSetExceptionStopReply(UINT8 *outbuf, UINT32 signal, ulong_t taskId)
{
	int idx = 0;

    /* 判断内核是否启动 */
    if(TA_KERNEL_IS_INITIALIZED())
	{
		/* 设置异常回复包信息"T05thread:异常线程ID;" */
    	outbuf[idx++] = 'T';
    	outbuf[idx++] = taHexChars[((signal >> 4) & 0xF)];
    	outbuf[idx++] = taHexChars[signal % 16];
	#ifdef CONFIG_TA_LP64
    	idx += (UINT32)sprintf((INT8*)(&outbuf[idx]), "thread:%lx",taskId);
	#else
		idx += (UINT32)sprintf((INT8*)(&outbuf[idx]), "thread:%x",taskId);
	#endif
    	outbuf[idx++] = ';';
    	outbuf[idx] = '\0';
	}
	else
	{
		outbuf[idx++] = 'S';
		outbuf[idx++] = taHexChars[((signal >> 4) & 0xF)];
		outbuf[idx++] = taHexChars[signal % 16];
		outbuf[idx] = '\0';
	}
}

/*
 * @brief:
 *      上报调试异常给GDB,需要将体系结构相关的调试异常向量号转换为POSIX异常信号上报给GDB
 * @param[in]:session: 调试会话
 * @param[in]:vector: 调试异常向量号
 * @return:
 *      无
 */
void taReportExceptionToDebugger(T_TA_Session *session, UINT32 vector)
{
	ulong_t taskId = 0;
	UINT32 curSignal = 0;

    /* 获取数据包的存放数据的起始地址 */
    UINT8 *pBuf = taGetPktUsrData(&(session->packet));

    /* 将vector转换为POSIX信号 */
    curSignal= taDebugVector2Signal(vector, session->sessInfo.context);

    /* 获取当前异常任务ID */
    taskId = TA_GET_CUR_THREAD_ID(&(session->sessInfo));

    /* 设置异常回复信息 */
    taSetExceptionStopReply(pBuf, curSignal,taskId);

    /* 设置数据包包头 */
    taSetPktHeadReceiver(&(session->packet.header),
            taGetPktHeadReceiver(&(session->gdbPacketHead)));

    /* 发送异常信息数据包给调试器 */
    taCOMMSendPacket(&(session->packet), strlen((const char *)pBuf));
}
