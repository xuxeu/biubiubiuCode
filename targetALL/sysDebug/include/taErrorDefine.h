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
 *@file: taErrorDefine.h
 *@brief:
 *       <li>错误号定义</li>
 */

#ifndef _TAERRORDEFINE_H
#define _TAERRORDEFINE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/

/************************宏定义********************************/

/************************类型定义******************************/

/* 所有API的返回值类型定义 */
typedef enum
{
    /*GDB错误*/
    GDB_OK = 0,
    GDB_EXEC_COMMAND = 1,             /* 执行命令时出错 */
    GDB_INVALID_PARAM = 2,            /* 认为GDB传的参数有误 */
    GDB_INVALID_VECTOR = 3,           /* 非法的vector */
    GDB_READ_SIZE_ERROR = 4,          /* 读内存大小过长 */

    /* 根据定义,TA的错误号从500开始 */
    TA_OK = 500,                      /* 表示调用成功 */
    TA_FAIL = 501,                    /* 表示调用失败 */
    TA_INVALID_INPUT  = 502,          /* 无效的输入参数 */
    TA_INVALID_NET_CFG = 503,         /* 无效的网络配 */
    TA_COMMANDTYPE_ERROR = 504,       /* 非法的命令类型 */
    TA_OUT_OF_BUFFER = 505,           /* TA一个包的buffer装不下数据 */
    TA_INVALID_ID = 506,              /* 无效的通道ID号 */
    TA_INVALID_PROTOCOL = 507,        /* 协议没有安装 */
    TA_INVALID_ALIGN_SIZE = 508,      /* 无效的访问宽度 */
    TA_COMMAND_NOTFOUND = 509,        /* 命令找不到 */
    TA_GET_DATA_ERROR = 510,          /* 获取数据失败 */
    TA_GET_PACKET_DATA_FAIL = 512,    /* 获取包数据失败 */
    TA_GET_CTRL_PACKET_FAIL = 513,    /* 获取确认包失败 */
    TA_PACKET_CHECK_SUM_FAIL = 514,   /* 验证校验和失败 */
    TA_CTRL_PACKET = 515,             /* 确认包 */
    TA_HANDLED_PACKET = 516,          /* 处理过的包 */
    TA_INVALID_PARAM = 517,           /* 非法的参数 */
    TA_IPI_NO_INIT = 518,             /* IPI 未初始化 */
    TA_IPI_EMIT_FAILED = 519,         /* 投递 IPI 中断失败 */
    TA_ILLEGAL_OPERATION = 520,       /* 非法操作 */
    TA_FUNCTION_IS_NULL = 521,        /* 函数指针为空 */
    TA_AGENT_NO_START = 522,          /* 代理未启动  */
    TA_ERR_MEM_ACCESS = 523,          /* 内存访问失败  */

    /* DB相关错误号定义 */
    TA_DB_BREAKPOINT_FULL = 600,      /* 断点打满了*/
    TA_DB_BREAKPOINT_EMPTY = 601,     /* 没有设置任何断点 */
    TA_DB_BREAKPOINT_NOTFOUND = 602,  /* 没找到断点 */
    TA_DB_BREAKPOINT_DIFFER_ID = 603, /* 找到地址相同,但sessionID,taskID不同的断点 */
    TA_DB_BREAKPOINT_EXIST = 604,     /* 存在相同addr,sessionID,taskID的断点 */
    TA_DB_NEED_IRET = 605,            /* 需要从中断/异常 返回 */
    TA_DB_COMMAND_REGISTERED = 606,   /* 命令已经注册  */

    /* RTP调试相关错误号 */
    TA_INVALID_CONTEXT = 700,         /* 无效上下文 */
    TA_INVALID_RTP_NAME = 701,        /* 无效实时进程名 */
    TA_INVALID_RTP_ID = 702,          /* 无效实时进程ID */
    TA_ALLOCATE_SESSION_FAIL = 703,   /* 分配调试会话失败 */
    TA_SESSION_ALREADY_ATTACH = 704,  /* 调试会话已经绑定 */
    TA_SESSION_FULL = 705,            /* 调试会话已经分配完毕 */
    TA_TASK_NO_ATTACH = 706,          /* 任务未绑定 */
    TA_ALLOC_MEM_FAIL = 707,          /* 分配内存失败 */
    TA_ERR_NO_RT_PROC = 708,          /* 运行时接口未安装 */
    TA_TASK_ALREADY_ATTACH = 709,     /* 任务已经绑定 */
    TA_INVALID_DEBUG_MODE = 710,      /* 无效上下文 */
    TA_INVALID_TASK_NAME = 711,       /* 无效实任务名 */
    TA_ATTACH_TASK_FAIL = 712,        /* 绑定任务失败 */
    TA_INVALID_TASK_ID = 713,         /* 无效实任务ID */
    TA_PAUSE_TASK_FAIL = 714,         /* 暂停任务失败 */

    /* 动态模块加载错误号  */
    TA_UNLOAD_MODULE_FAIL = 900,      /* 卸载模块失败 */
    TA_ALLOC_MODULE_MEM_FAIL = 901,   /* 分配模块内存失败 */
    TA_LOAD_MODULE_FAIL = 902,        /* 加载模块失败 */
    TA_ADD_SYMBOL_FAIL = 903,          /* 加载模块失败 */

    /* RSE错误号 */
    TA_INVALID_OPERATE_ID = 1000,      /* 无效的操作ID */
    TA_INVALID_MODULE_ID = 10001       /* 无效的模块ID */
}T_TA_ReturnCode;

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TAERRORDEFINE_H */
