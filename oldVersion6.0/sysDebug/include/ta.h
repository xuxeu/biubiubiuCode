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
 *@file:ta.h
 *@brief:
 *        <li>向TA各模块提供的头文件</li>
 */
#ifndef _TA_H
#define _TA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "db.h"
#include "taPacket.h"
#include "taDevice.h"
#include "taPacketComm.h"
#include "taDirectMemory.h"
#include "taProtocol.h"
#include "taMsgDispatch.h"
#include "taDebugMode.h"
#include "taUtil.h"
#include "taCommand.h"
#include "sdaMain.h"
#include "sdaCpuset.h"
#include "sdaCpuLib.h"
#include "string.h"

/************************宏定义********************************/

#define TA_xprint printk

/*最值宏定义*/
#ifndef MAX
#define MAX(x,y) (((x) > (y))? (x):(y))
#endif

#ifndef MIN
#define MIN(x,y) (((x) < (y))? (x):(y))
#endif

#undef DEBUG
#ifdef DEBUG
	#define DBG(x) \
            do {(x);} while ((0))
#else
	#define DBG(x)
#endif

#define TA_READ		0
#define TA_WRITE	1
/************************类型定义******************************/

/* 二级代理号 */
typedef enum
{
    TA_MANAGER = 0,        /* MANAGER */
    TA_SYS_DEBUGER = 1,    /* 系统调试代理SDA */
    TA_DYNAMIC_LOADER = 2, /* 动态加载代理 */
    TA_RSE_AGENT = 3,      /* RSE代理 */
    TA_CTRL_COMMAND = 4,   /* 调试控制命令代理 */
} TA_SUBA_ID;

/* TA网络属性 */
typedef struct
{
    INT8 *name;
    UINT8 *ip;
    INT16 port;
    INT8 *mac;
    UINT32 minor;
} T_TA_NET_CONFIG;

/* 指针，回调函数 */
typedef void (*callbackhandler)(void);

/* 字符串格式化 */
extern int sprintf(INT8 *buffer,const char *  fmt,...);

/* 任务级异常处理函数指针 */
extern void (*_func_taTaskExceptionHandler)(UINT32 vector, T_DB_ExceptionContext *context);

/* 系统异常处理函数指针 */
extern void (*_func_taSysExceptionHandler) (UINT32 vector, T_DB_ExceptionContext *context);

/* 内存切换函数指针 */
extern FUNCPTR _func_taskMemCtxSwitch;

/* 查询异常上下文 */
extern T_DB_ExceptionContext taGdbGetExpContext;

/* 系统级调试会话  */
extern T_TA_Session taSDASessInfo;

/* wdb任务ID */
extern int	wdbTaskId;
/************************接口声明******************************/

/*
 * @brief:
 *     安装异常处理函数到IDT表中
 * @param[in] vector :异常号
 * @param[in] vectorHandle :异常处理函数
 * @return:
 *     无
 */
void taInstallVectorHandle(int vector, void *vectorHandle);

/*
 * @brief:
 *     trap异常处理
 * @return:
 *     无
 */
void taTraceExceptionStub(void);

/*
 * @brief:
 *     断点异常处理
 * @return:
 *     无
 */
void taBpExceptionStub(void);

/*
 * @brief:
 *      异常处理功公共入口,根据调试模式和安装的异常处理函数进行异常处理
 * @param[in]: vector: 异常号
 * @param[in]: context: 异常上下文
 * @return:
 *     无
 */
void taExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context);

/*
 * @brief:
 *      任务级异常处理
 * @param[in]: vector: 异常号
 * @param[in]: context: 异常上下文
 * @return:
 *     无
 */
void taTaskExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context);

/*
 * @brief:
 *      系统级调试异常处理程序
 * @param[in]:vector: 调试异常向量号
 * @param[in]: context: 异常上下文
 * @return:
 *      无
 */
void taSDAExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context);

/*
 * @brief:
 *     刷新Cache
 * @param[in]: address: 要更新的cache内存地址
 * @param[in]: size: 要更新的长度
 * @return:
 *     无
 */
void taCacheTextUpdata(UINT32 address, UINT32 size);

/*
 * @brief:
 *     获取当前任务ID
 * @return:
 *     当前任务ID
 */
int taTaskIdSelf(void);

/*
 * @brief:
 *      回显设备输出字符
 * @param[in]: ch:字符
 * @return:
 *      无
 */
void taDisplayDeviceOutputChar(char ch);

/*
 * @brief:
 *    向printk设备打印单个字符
 * @param[in]: ch: 需要输出的字符
 * @return: 
 *    无
 */ 
void taPrintChar(char ch);

/*
 * @brief:
 *      初始化调试设备
 * @return:
 *      无
 */
void taDebugDeviceInit(void);

/*
 * @brief:
 *      显示启动信息
 * @return:
 *      无
 */
void taDisplayStartedInfo(void);

/*
* @brief:
*     桥接口初始化
* @return:
*      无
*/
void taBrigdeInit(void);

/**
 * @brief:
 *     内存探测初始化
 * @param[in]:_func_excBaseHook: 异常HOOK指针，异常产生时操作系统调用该HOOK
 * @return:
 * 	   OK：初始化成功
 * 	   ERROR：初始化失败
 */
STATUS taMemProbeInit(FUNCPTR *_func_excBaseHook);

/*
 * @brief:
 *      任务级调试暂停
 * @param[in]: sid: 会话ID
 * @return:
 *     ERROR: 暂停失败
 *     OK: 暂停成功
 */
STATUS taTaskPause(INT32 sid);

/*
 * @brief:
 *     任务级调试代理入口函数
 * @param[in]: packet:包含调试操作命令消息包
 * @return:
 *     TA_OK: 命令执行成功
 *     TA_FAIL: 命令执行失败
 *     TA_DB_NEED_IRET：需要从异常中返回
 *     TA_COMMAND_NOTFOUND: 命令没找到
 */
T_TA_ReturnCode taTaskDebugAgent(T_TA_Packet *packet);

/*
 * @brief:
 *     安装任务删除HOOK，在每个任务删除时调用该HOOK
 * @param[in]:deleteHook:任务删除HOOK函数指针
 * @return:
 *     ERROR:任务删除HOOK表已满
 *     OK:安装任务删除HOOK成功
 */
STATUS taTaskDeleteHookAdd(FUNCPTR deleteHook);

/*
 * @brief:
 *     安装任务创建HOOK，在每个任务创建时调用该HOOK
 * @param[in]:createHook:任务创建HOOK函数指针
 * @return:
 *     ERROR:任务创建HOOK表已满
 *     OK:安装任务创建HOOK成功
 */
STATUS taTaskCreateHookAdd(FUNCPTR createHook);

/*
 * @brief:
 *     校验任务是否存在
 * @param[in]:taskId:任务ID
 * @return:
 *     OK:任务存在
 *     ERROR:任务不存在
 */
int taTaskIdVerify(int taskId);

/*
 * @brief:
 *     获取任务上下文
 * @param[in]:taskId:任务ID
 * @param[out]:context:任务上下文
 * @return:
 *     OK：获取上下文成功
 *     ERROR：获取上下文失败
 */
int taTaskContextGet(int taskId, T_DB_ExceptionContext *context);

/*
 * @brief:
 *     设置任务上下文
 * @param[in]:taskId:任务ID
 * @param[in]:context:任务上下文
 * @return:
 *     OK：设置上下文成功
 *     ERROR：设置上下文失败
 */
int taTaskContextSet(int taskId, T_DB_ExceptionContext *context);

/*
 * @brief:
 *     获取当前CPU ID
 * @return:
 *     CPU ID
 */
UINT32 taGetCpuID(void);

/*
 * @brief:
 *     获取OS中使能的CPU
 * @return:
 *     OS中使能的CPU
 */
cpuset_t taCpuEnabledGet (void);

/**
* @brief
*    将网卡收发数据的模式变为轮询模式
* @param  无
* @return
*	OK:网卡模式切换成功
*	ERROR:网卡模式切换出错
*/
INT32 taSetEndToPoll(void);

/**
* @brief
*    将网卡收发数据的模式变为中断模式
* @param  无
* @return
*	OK: 网卡模式切换成功
*	ERROR: 网卡模式切换出错
*/
INT32 taSetEndToInt(void);

/*
 * @brief:
 *     挂起任务
 * @param[in]:taskId:任务ID
 * @return:
 *     OK:暂停任务成功
 *     ERROR:无效任务ID
 */
STATUS taTaskSuspend(int tid);

/*
 * @brief:
 *     恢复任务运行
 * @param[in]:tid:任务ID
 * @return:
 *     OK:恢复任务运行成功
 *     ERROR:无效任务ID
 */
STATUS taTaskResume(int tid);

/*
 * @brief:
 *     激活任务
 * @param[in]:tid:任务ID
 * @return:
 *     OK:激活任务成功
 *     ERROR:无效任务ID
 */
STATUS taTaskActivate(int tid);

/*
 * @brief:
 *     运行任务
 * @param[in]:tid:任务ID
 * @return:
 *     OK:运行任务成功
 *     ERROR:无效任务ID
 */
STATUS taTaskCont(int tid);

/*
 * @brief:
 *     根据任务ID获取任务名
 * @param[in]:taskID:任务ID
 * @return:
 *     OK:获取任务名成功
 *     ERROR:获取任务名失败
 */
char *taTaskName(int taskID);

/*
 * @brief:
 *     获取系统中任务列表
 * @param[out]:idList:任务ID列表
 * @param[in]:maxTasks:任务列表能够容纳的最大任务数
 * @return:
 *     任务列表中的任务个数
 */
int taTaskIdListGet(int idList[],int maxTasks);

/*
 * @brief:
 *     禁止本地CPU任务抢占
 * @return:
 *     OK:成功
 *     ERROR:失败
 */
STATUS taTaskCpuLock(void);

/*
 * @brief:
 *     使能本地CPU任务抢占，但不执行调度函数
 * @return:
 *     OK:成功
 *     ERROR:失败
 */
STATUS taTaskCpuUnlockNoResched(void);

/*
 * @brief:
 *     使能本地CPU任务抢占
 * @return:
 *     OK:成功
 *     ERROR:失败
 */
STATUS taTaskCpuUnlock(void);

/*
 * @brief:
 *     分配内存
 * @param[in]:nBytes:分配内存大小
 * @return:
 *     内存地址
 */
void *taMalloc(size_t nBytes);

/*
 * @brief:
 *     释放内存
 * @param[in]: ptr:内存地址
 * @return:
 *     内存地址
 */
void taFree(void *ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TA_H */
