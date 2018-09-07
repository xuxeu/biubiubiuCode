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
#include "string.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/
UINT8 *g_sdaExpType[33] =
{
    "Divide Error",        /* 0 */
    "Trap",        /* 1 */
    "NMI Interrupt",        /* 2 */
    "Breakpoint",        /* 3 */
    "Overflow",        /* 4 */
    "BOUND Range Exceeded",        /* 5 */
    "Invalid Opcode(Undefined Opcode)",        /* 6 */
    "Device Not Available (No Math Coprocessor)",        /* 7 */
    "Double Fault",        /* 8 */
    "Coprocessor Segment Overrun",        /* 9 */
    "Invalid TSS(Task State Segment)",        /* 10 */
    "Segment Not Present",        /* 11 */
    "Stack fault exception",        /* 12 */
    "Stack-Segment Fault",        /* 13 */
    "Page Fault",        /* 14 */
    "Reserved",        /* 15 */
    "x87 FPU Floating-Point Error (Math Fault)",        /* 16 */
    "Alignment Check",        /* 17 */
    "Machine Check",        /* 18 */
    "SIMD Floating-Point Exception",        /* 19 */
    "Reserved",        /* 20 */
    "Reserved",        /* 21 */
    "Reserved",        /* 22 */
    "Reserved",        /* 23 */
    "Reserved",        /* 24 */
    "Reserved",        /* 25 */
    "Reserved",        /* 26 */
    "Data address breakpoint",        /* 27 */
    "Reserved",        /* 28 */
    "Reserved",        /* 29 */
    "Reserved",        /* 30 */
    "Instruction address breakpoint",        /* 31 */
    "User defined exception type"         /* 32 */
};

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
void taSetExceptionStopReply(UINT8 *outbuf, UINT32 signal, UINT32 taskId)
{
	int idx = 0;

    /* 判断内核是否启动 */
    if(TA_KERNEL_IS_INITIALIZED())
	{
		/* 设置异常回复包信息"T05thread:异常线程ID;" */
    	outbuf[idx++] = 'T';
    	outbuf[idx++] = taHexChars[((signal >> 4) & 0xF)];
    	outbuf[idx++] = taHexChars[signal % 16];
    	idx += (UINT32)sprintf((INT8*)(&outbuf[idx]), "thread:%x",taskId);
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
	INT32 taskId = 0;
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



/*
 * @brief:
 *      设置上报给主机端的异常上下文信息 如果是分区级异常则需要分区名用于主机端启动调试时进行分区绑定
 *      "e:ID:%x;ExceptionInfo"与主机端进行了约定不能任意修改，后面的异常信息可根据需要进行添加
 * @param[in] pBuf: 异常上下文存放缓冲
 * @param[in] tid: 任务ID
 * @param[in] dbContext: 异常上下文
 * @param[in] ctxType: 上下文类型
 * @return:
 *      none
 * @implements  DM.3.153
 */
void taSetReportExceptionInfo(UINT8 *pBuf, UINT32 tid, T_DB_ExceptionContext *dbContext, UINT8 *buf, UINT32 ctxType)
{
	UINT32 vector = dbContext->vector;

	if (vector > 31)
	{
		vector = 32;
	}

	if(NULL == tid)
	{
		/* 设置上报给主机端的异常上下文信息 */
		sprintf(pBuf, "e:ID:%x;ExceptionInfo: Type: %s, tid: %d, tName: %s, PC = 0x%x,  %s, ctxType : %d",
				taGetTargetId(),
				g_sdaExpType[vector],
				tid,
				taTaskName(tid),
				dbContext->registers[PC],
				buf,
				ctxType
				);
	}
	else
	{
		/* 设置上报给主机端的异常上下文信息 */
		sprintf(pBuf, "e:ID:%x;ExceptionInfo: Type: %s, tid: %d, tName: %s, PC = 0x%x, %s, ctxType : %d",
				taGetTargetId(),
				g_sdaExpType[vector],
				tid,
				taTaskName(tid),
				dbContext->registers[PC],
				buf,
				ctxType
				);
	}
}

/*
 * @brief:
 *      上报异常给主机端
 * @param[in] session: 调试会话
 * @param[in] tid: 任务ID
 * @param[in] dbContext: 异常上下文
 * @param[in] ctxType: 上下文类型
 * @return:
 *      none
 * @implements  DM.3.153
 */
T_TA_ReturnCode taReportExceptionToHost(T_TA_Session *session, UINT32 tid, T_DB_ExceptionContext *dbContext, UINT8 *buf, UINT32 ctxType)
{
	UINT8 *pBuf = NULL;
	UINT32 Offset = 0;

	/* 获取数据包的存放数据的起始地址 */
	pBuf = taGetPktUsrData(&(session->packet));

	/* 设置数据包包头 */
	taSetPktHeadReceiver(&(session->packet.header),(UINT16)TSMAP_ID);

	/* 设置上报的异常信息 */
	taSetReportExceptionInfo(pBuf, tid, dbContext, buf, ctxType);

	/* 发送异常信息数据包 */
	taCOMMSendPacket(&(session->packet), strlen((const char *)pBuf));

	return (TA_OK);
}

