/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-07-08         彭元志，北京科银京成技术有限公司
 *                           创建该文件。
 */

/**
 * @file  resKernelTaskLib.c
 * @brief
 *       功能：
 *       <li>核心任务信息查询</li>
 */

/************************头 文 件******************************/
#include "rseConfig.h"
#include "taskLib.h"
#include "rseCmdLib.h"
#include "rseInitLib.h"

/************************宏 定 义******************************/

/* 核心任务属性个数 */
#define RSE_TASK_INFO_ATTR_NUM 4

/* 最大任务数 */
#define RSE_MAX_DSP_TASKS 1000

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/*
 * @brief
 *      提供核心任务信息的消息处理
 * @param[in] packet: 接收到的数据包
 * @return
 *      None
 */
static T_TA_ReturnCode rseKernelTaskHandler(T_TA_Packet* packet);

/*
 * @brief
 * 		获取系统核心任务列表
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
static void rseGetKernelTaskList(T_TA_Packet* packet,UINT32* outSize);

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/**
  * @brief
  *     核心任务信息初始化函数
  *@return
  *      None
  */
void rseKernelTaskLibInit(void)
{
	rseRegisterModuleHandler(RSE_TYPE_TASK_MODULE, rseKernelTaskHandler);
}

/*
 * @brief
 *      提供核心任务的消息处理
 * @param[in] packet: 接收到的数据包
 * @return
 *      TA_OK:操作成功
 */
static T_TA_ReturnCode rseKernelTaskHandler(T_TA_Packet* packet)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT32 outSize = 0;
    UINT8 operateId = 0;

    /* 从包中获取实际数据 */
    inbuf = taGetPktUsrData(packet);

    /* 设置输出缓冲outbuf为inbuf */
    outbuf = inbuf;

    /* 获取操作ID */
    operateId = inbuf[2];
    switch (operateId)
    {
        case RSE_OPR_QUERY:
        	rseGetKernelTaskList(packet, &outSize);
            break;
        default:
        	outSize = (UINT32)sprintf((INT8*)outbuf, "E%d", TA_INVALID_OPERATE_ID);
            break;
    }

    /* 发送数据包 */
    taCOMMSendPacket(packet, outSize);

    return TA_OK;
}

/*
 * @brief
 * 		获取系统核心任务列表
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
static void rseGetKernelTaskList(T_TA_Packet* packet,UINT32* outSize)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT8* ptr = NULL;
    UINT32 len = 0;
    INT32 taskId = 0;
    char taskStatus[RSE_MAX_NAME_LEN+1] = {0};
    char taskName[RSE_MAX_NAME_LEN+1] = {0};
    static UINT32 count = 0;
    static UINT32 idx = 0;
    static INT32 taskIdList[RSE_MAX_DSP_TASKS] = {0};

    /* 从包中获取实际数据 */
    inbuf = taGetPktUsrData(packet);

    /* 设置输出缓冲outbuf为inbuf */
    ptr = outbuf = inbuf;

    /* 获取线程ID列表 */
    if(count == 0)
    {
    	count = taTaskIdListGet(taskIdList, RSE_MAX_DSP_TASKS);
    }

    /* 清空数据包 */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    /* 进入循环采集所有核心任务对象信息 */
    while (idx < count)
    {
    	/* 先检查剩余包长是否可以存储任务信息 */
    	RSE_CHECK_OVER_PKTSIZE(ptr,outbuf, RSE_TASK_MAX_INFO_LEN);

        /* 取得任务ID */
        taskId = taskIdList[idx++];

        /* 检查任务是否是核心任务 */
        if(!IS_KERNEL_TASK(taskId))
        {
        	continue;
        }

        /* 设置回复结果 */
        len = sprintf((INT8 *)ptr,"%s","OK");
        ptr += (len+2);

		/* 写入类型ID */
		RSE_PUTPKT_SHORT(RSE_TYPE_KERNEL_TASK, ptr);

		/* 写入对象ID */
		RSE_PUTPKT_LONG(taskId, ptr);

		/* 写入标志位 */
		*ptr = 0;
		ptr += 1;

		/* 清空任务名 */
		ZeroMemory(taskName,RSE_MAX_NAME_LEN+1);

		/* 获取任务名 */
		strncpy(taskName, taTaskName(taskId), RSE_MAX_NAME_LEN);

		/* 写入要显示标签 */
		len = sprintf((INT8 *)ptr,"%s", taskName);
		ptr += (len+1);

	    /* 写入操作个数 */
	    *ptr = RSE_NULL_OPER;
	    ptr += 1;

		/* 写入属性个数 */
		*ptr = RSE_TASK_INFO_ATTR_NUM;
		ptr += 1;

		/* 写入属性表,任务ID、任务名、任务状态、任务类型 */
	    RSE_PUTPKT_SHORT(RSE_ATTR_ID, ptr);
	    len = sprintf((INT8 *)ptr,"%x", taskId);
	    ptr += (len+1);

	    /* 写入任务名属性 */
	    RSE_PUTPKT_SHORT(RSE_ATTR_NAME, ptr);
	    len = sprintf((INT8 *)ptr,"%s", taskName);
	    ptr += (len+1);

		/* 清空任务状态字符串 */
		ZeroMemory(taskStatus,RSE_MAX_NAME_LEN+1);

	    /* 写入任务状态属性 */
	    RSE_PUTPKT_SHORT(RSE_ATTR_STATE, ptr);
	    taskStatusString(taskId, taskStatus);
	    len = sprintf((INT8 *)ptr,"%s", taskStatus);
	    ptr += (len+1);

	    /* 写入任务类型属性 */
	    RSE_PUTPKT_SHORT(RSE_ATTR_TASK_TYPE, ptr);
	    len = sprintf((INT8 *)ptr,"%s", "Object");
	    ptr += (len+1);
    }

    /* 设置输出缓冲为 "$",表示任务信息发送完毕 */
    if(idx == count)
    {
    	*ptr = RSE_END_STRING;
    	ptr += 1;
    	idx = count = 0;
    }

    /* 设置输出缓冲数据大小 */
    *outSize = ptr - outbuf;
}
