/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                         创建该文件。
 */

/*
 * @file:taOsIf.c
 * @brief:
 *             <li>桥接口初始化及其OS接口封装</li>
 */
 
/************************头 文 件******************************/
#include "ta.h"
#include "taBridge.h"

/************************宏 定 义********************************/

/************************类型定义*******************************/

/************************外部声明*******************************/

/* IPI核间中断处理函数，控制CPU状态，使其CPU处于空转状态 */
extern void sdaIpiHandler(void);
#ifdef __MIPS__
	extern void CPU_FlushCaches(unsigned int addr, unsigned int len);
	extern T_DB_ExceptionContext* taSDAExpContextPointerGet(UINT32 cpuIndex);
#endif
/************************前向声明*******************************/
#ifdef __MIPS__
	extern taBpExceptionStub;
	extern T_TA_ReturnCode taUdpRead(UINT8* buf,UINT32 cnt,UINT32* dlen);
	extern T_TA_ReturnCode taUdpWrite(UINT8* buf,UINT32 cnt,UINT32* dlen);
	extern void TaPacketNumGet(UINT32* pTaTxPacketNum, UINT32* pTaRxPacketNum);
	extern int taGetSaid();
	extern unsigned int* taGetBreakInfo();
	extern BOOL taGetMultiThreadInfoTag();
#endif
/************************模块变量*******************************/

/* 桥接口变量 */
static T_TA_BRIDGE  taBrigde;
/************************全局变量*******************************/

/* 初始化任务ID */
int taInitTaskId = 0;
/************************函数实现*******************************/

/**
 * @brief:
 *     桥接口初始化
 * @return:
 *      无
*/
void taBrigdeInit(void)
{
    /* IPI初始化 */
    taBrigde.sdaIpiInit = sdaIpiInit;
    
#ifdef __X86__    
    /* 自陷异常处理函数，操作系统处理化覆盖了核心级调试异常处理函数，需重新安装核心级调试自陷异常处理 */
    taBrigde.taTraceExceptionStub = taTraceExceptionStub;
#endif
    
    /* 断点异常处理函数，操作系统处理化覆盖了核心级调试异常处理函数，需重新安装核心级调试断点异常处理 */
    taBrigde.taBpExceptionStub = taBpExceptionStub;
    
    /* 调试IPI中断回调函数，用于保存异常上下文 */
    taBrigde.cpuCtrlHandler = sdaIpiHandler;
       
    /* 内存探测初始化 */
    taBrigde.taMemProbeInit = taMemProbeInit;
#ifdef __MIPS__ 
    taBrigde.taUdpRead = taUdpRead;
    taBrigde.taUdpWrite = taUdpWrite;

    taBrigde.TaPacketNumGet = TaPacketNumGet;

    taBrigde.taGetSaid = taGetSaid;
   
    taBrigde.taGetBreakInfo = taGetBreakInfo;
    taBrigde.taGetMultiThreadInfoTag = taGetMultiThreadInfoTag;
#endif
    /* 存放桥地址 */
    *((int*)TA_BRIGDE_ADDR) = (int)&taBrigde;
    
    /* 设置TA启动标志 */
    *((int*)TA_START_FLAG_ADDR) = TA_START_FLAG_VALUE;
}

/**
 * @Brief
 *     获取任务名
 * @param[in]: taskID:任务ID
 * @param[out]: taskNameStr:任务名
 * @return
 *     ERROR：桥未初始化或者获取任务名失败
 *     OK：获取任务名成功
 */
char* taTaskName(int taskID)
{
    if(NULL == taBrigde.taskName)
    {
	    return NULL;
    }
	
    return taBrigde.taskName(taskID);
}

/**
 * @Brief
 *     获取当前任务ID
 * @return
 *     0：桥未初始化或者获取当前任务ID失败
 *     成功返回当前任务ID
 */
int taTaskIdSelf(void)
{
    if(NULL == taBrigde.taskIdSelf)
    {
	    return 0;
    }
	
    return taBrigde.taskIdSelf();
}

/**
 * @Brief
 *     获取系统中任务列表
 * @param[out]: idList:任务列表
 * @param[in]: maxTasks:任务列表能够容纳的最大任务数
 * @return
 *     0：桥未初始化
 *     成功返回任务个数
 */
int taTaskIdListGet(int idList[],int maxTasks)
{
    if(NULL == taBrigde.taskIdListGet)
    {
	    return 0;
    }
	
    return taBrigde.taskIdListGet(idList,maxTasks);
}
 
/**
 * @Brief
 *     获取任务上下文
 * @param[in]: taskId:任务ID
 * @param[out]: context:异常上下文
 * @return
 *     ERROR：桥未初始化或者获取任务上下文失败
 *     OK：获取任务上下文成功
 */
int taTaskContextGet(int taskId, T_DB_ExceptionContext *context)
{
    if(NULL == taBrigde.taskContextGet)
    {
	    return ERROR;
    }
	
    return taBrigde.taskContextGet(taskId, context);
}

/**
 * @Brief
 *     获取OS中使能的CPU
 * @return
 *     成功返回CPU使能
 */
cpuset_t taCpuEnabledGet (void)
{
    if(NULL == taBrigde.cpuEnabledGet)
    {
	    return 1;
    }
	
    return taBrigde.cpuEnabledGet();
}

/**
 * @Brief
 *     获取CPU ID
 * @return
 *     0：桥未初始化
 *     成功返回CPU ID
 */
unsigned int taGetCpuID(void)
{
    if(NULL == taBrigde.getCpuID)
    {
        return 0;
    }
	
    return taBrigde.getCpuID();
}

/**
 * @brief:
 *      刷新数据Cache
 * @param[in]:addr: 要更新的cache内存地址
 * @param[in]:len: 要更新的长度
 * @return:
 *      无
 */
void taCacheTextUpdata(UINT32 addr, UINT32 len)
{
	#ifdef __MIPS__
		CPU_FlushCaches(addr,len);
	#else
		return;
	#endif

}

/**
 * @Brief
 *     校验任务ID
 * @param[in]: taskId:任务ID
 * @return
 *     ERROR：桥未初始化或者校验任务ID失败
 *     OK：校验任务ID成功
 */
int taTaskIdVerify(int tid)
{
    if(NULL == taBrigde.taskIdVerify)
    {
	    return ERROR;
    }
	
    return taBrigde.taskIdVerify(tid);
}


/**
* @brief
*    将网卡收发数据的模式变为轮询模式
* @param  无
* @return
*	OK:网卡模式切换成功
*/
INT32 taSetEndToPoll(void)
{
    return OK;
}

/**
* @brief
*    将网卡收发数据的模式变为中断模式
* @param  无
* @return
*	OK:网卡模式切换成功
*/
INT32 taSetEndToInt(void)
{
    return OK;
}

/*
 * @brief:
 *      获取调试会话中绑定的任务ID列表
 * @param[in]: tid: 任务ID
 * @param[out]: pSid: 会话ID
 * @return:
 *     NULL:未找到对应的任务节点
 *     T_TASK_NODE:任务节点
 */
INT32 taGetTaskIdList(UINT32 sid, INT32 idList[], INT32 maxTasks)
{
    return 0;
}

/*
 * @brief:
 *     激活任务
 * @param[in]:tid:任务ID
 * @return:
 *     OK:激活任务成功
 *     ERROR:无效任务ID
 */
STATUS taTaskActivate(int tid)
{
    return OK;
}

/*
 * @brief:
 *     运行任务
 * @param[in]:tid:任务ID
 * @return:
 *     OK:运行任务成功
 *     ERROR:无效任务ID
 */
STATUS taTaskCont(int tid)
{
    return OK;
}
/**
 * @Brief
 *     串口打印
 
void printUart(char *fmt, ...)
{
    if(NULL == taBrigde.printUart)
    {
	    return ERROR;
    }
	
    taBrigde.printUart(fmt);
} */
