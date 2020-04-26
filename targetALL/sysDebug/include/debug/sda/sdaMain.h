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
 * @file  sdaMain.h
 * @brief
 *       功能:
 *       <li>CPU控制相关函数声明</li>
 */
#ifndef _SDA_MAIN_H_
#define _SDA_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "dbContext.h"
#include "taPacket.h"
#include "taskSllLib.h"

/************************宏定义********************************/

/* 系统级调试会话ID */
#define TA_SYS_DEBUGER_SESSION 1

/* 设置当前产生异常的线程ID */
#define TA_SET_CUR_THREAD_ID(debugInfo, threadId) \
do\
{\
	if(ERROR != (threadId))\
    {\
	    ((debugInfo)->curThreadId = (threadId));\
	}\
}\
while(0)

/* 获取当前产生异常的线程ID */
#define TA_GET_CUR_THREAD_ID(debugInfo) ((debugInfo)->curThreadId)

/* 设置调试状态 */
#define TA_SET_DEBUG_STATUS(debugInfo, debugStatus) ((debugInfo)->status = (debugStatus))

/* 设置调试对象ID */
#define TA_SET_DEBUG_OBJ_ID(debugInfo, id) ((debugInfo)->objID = (id))

/* 调试会话是否处于运行态 */
#define TA_DEBUG_IS_CONTINUE_STATUS(debugInfo) ((debugInfo)->status == DEBUG_STATUS_CONTINUE)

/* 调试会话是否处于暂停态 */
#define TA_DEBUG_IS_PAUSE_STATUS(debugInfo) ((debugInfo)->status == DEBUG_STATUS_PAUSE)

/************************类型定义******************************/

/* 调试状态 */
typedef enum
{
    DEBUG_STATUS_NONE = 0,
    DEBUG_STATUS_CONTINUE = 1,
    DEBUG_STATUS_EXCEPTION = 2,
    DEBUG_STATUS_PAUSE = 4
}T_DB_DebugStatus;

/* 程序的运行状态 */
typedef enum
{
	DEBUG_STATUS_DETACH = 0,
	DEBUG_STATUS_ATTACH = 1
}T_TA_AttachStatus;

/* 任务节点 */
typedef struct
{
    SLL_NODE slNode;
    INT32    taskId;
} T_TASK_NODE;

/* 上下文类型 */
typedef enum
{
    TA_CTX_SYSTEM	  = 0,	/* 系统模式 */
    TA_CTX_TASK	      = 1,	/* 核心任务 */
    TA_CTX_TASK_RTP	  = 2,	/* 任务级实时进程 */
    TA_CTX_SYSTEM_RTP = 3,  /* 系统级实时进程 */
    TA_CTX_TYPE_NUM
} TA_CTX_TYPE;

/* 调试信息 */
typedef struct
{
    /* 异常上下文,定义成指针方便在汇编中处理异常上下文 */
    T_DB_ExceptionContext *context;

    /* 任务链表 */
    SLL_LIST taskList;

    /* 任务是否自动绑定 */
    BOOL taskIsAutoAttach;

    /* 会话ID */
    UINT32 sessionID;

    /* 被调试对象 */
    UINT32 objID;

    /* 当前产生异常的线程ID */
    INT32 curThreadId;

    /* 上下文类型 */
    TA_CTX_TYPE ctxType;

    /* 调试状态 */
    T_DB_DebugStatus status;

    /* 记录对象绑定状态 */
    T_TA_AttachStatus attachStatus;
} T_DB_Info;

/* 定义调试会话结构 */
typedef struct
{
    T_DB_Info sessInfo;              /* 被调试对象上下文和会话ID */
    T_TA_Packet packet;              /* 主机端工具交互的消息包 */
    T_TA_PacketHead gdbPacketHead;   /* 保存向调试器发包时的ID */
} T_TA_Session;

/************************接口声明******************************/

/*
 * @brief:
 *         获取调试信息
 * @return:
 *       系统级的调试信息
 */
T_DB_Info* taSDADebugInfoGet(void);

/*
 * @brief:
 *     获取异常上下文指针
 * @return:
 * 		异常上下文指针
 */
T_DB_ExceptionContext* taSDAExpContextPointerGet(UINT32 cpuIndex);
#ifdef __PPC__
#ifdef TA_ALTIVEC_SUPPORT_DEBUG
/*
 * @brief:
 *     获取异常矢量上下文指针
 * @return:
 * 		异常矢量上下文指针
 */
T_DB_ExceptionVectorContext* taSDAExpVectorContextPointerGet(UINT32 cpuIndex);
#endif
#endif

/*
 * @brief:
 *      SDA初始化,负责初始化系统级调试会话，及其消息处理函数的安装
 * @return:
 *        TA_OK: 初始化成功
 */
T_TA_ReturnCode taSDAInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SDA_MAIN_H_ */
