/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  sdaCpuLib.c
 * @brief
 *       功能:
 *       <li>多核库，提供相关函数控制从CPU的状态，系统级模式下，BSP核控制从CPU的休眠
 *       和恢复</li>
 */
#ifdef CONFIG_CORE_SMP

/************************头 文 件******************************/
#include "taskLibP.h"
#include "dbAtomic.h"
#include "dbContext.h"
#include "sdaCpuLib.h"
#include "string.h"
#include "sdaMain.h"
#include "ta.h"

/************************宏 定 义******************************/

/* 控制是否使能 */
#define CPU_CTRL_IS_ENABLED() (MULTIOS_ATOMIC_GET(&sdaCpuCtrlEnabled) == TRUE)

/* CPU使能 */
#define CPU_ENABLED() (taCpuEnabledGet())

/************************类型定义*****************************/

/* 任务信息 */
typedef struct
{
    UINT32 threadId;
    UINT32 cpuID;
}CPU_THREAD_INFO;

/************************外部声明*****************************/

/************************前向声明*****************************/

/* 休眠当前CPU */
static void sdaCpuLocalSuspend(void);

/************************模块变量*****************************/
/* 记录各个CPU状态: 0=Running / 1=Suspended */
static volatile cpuset_t sdaCpuSuspended = 0;

/* 标示哪个CPU为主CPU，控制其他从CPU的状态，当调用函数sdaCpuAllSuspend时被设置，
 * 调用sdaCpuAllResume时被清除 */
static volatile cpuset_t sdaCpuCtrlRunning = 0;

/* 挂起所有CPU次数 */
static UINT32 sdaCpuAllSuspendCnt = 0;

/* IPI初始化标志 */
BOOL sdaIpiInitialized = FALSE;

/* CPU控制使能标志 */
static atomic_t sdaCpuCtrlEnabled = FALSE;

/* 所有从CPU复位标志 */
static atomic_t sdaCpuAllResumeFlag = TRUE;

/* 投递核间中断函数 */
static IPI_EMIT_HANDLER_FUNC sdaIpiEmitFunc;

/* IPI处理函数 */
static IPI_HANDLER_FUNC sdaIpiFunc;

/************************全局变量*****************************/

/* 线程状态 */
CPU_THREAD_INFO cpuThreadstatus[MAX_CPU_NUM];

/************************函数实现*****************************/

/*
 *@brief
 *     功能:获取处理器ID
 *@return
 *    成功：处理器ID
 */
UINT32 GET_CPU_INDEX(void)
{
  return taGetCpuID();
}

/*
 *@brief
 *     功能:通过线程ID获取CPU ID
 *@return
 *    如果成功返回CPU ID，否则返回0xffffffff
 */
UINT32 sdaGetCpuIdByThreadId(UINT32 threadId)
{
    int i = 0;

    for(i = 0;i < MAX_CPU_NUM; i++)
    {
        if(threadId == cpuThreadstatus[i].threadId)
        {
             return cpuThreadstatus[i].cpuID;
        }
    }

    return (~0);
}

/*
 *@brief
 *     功能:使能CPU控制
 *@return
 *    如果成功返回TA_OK
 */
T_TA_ReturnCode sdaCpuCtrlEnable(void)
{
	TA_MULTIOS_ATOMIC_SET(&sdaCpuCtrlEnabled, TRUE);

    return (TA_OK);
}

/*
 *@brief
 *     功能:禁止CPU控制
 *@return
 *    如果成功返回TA_OK
 */
T_TA_ReturnCode sdaCpuCtrlDisable(void)
{
	TA_MULTIOS_ATOMIC_SET(&sdaCpuCtrlEnabled, FALSE);

    return (TA_OK);
}

/*
 *@brief
 *     功能:IPI核间中断处理函数
 *@return
 *    无
 */
void sdaIpiHandler(void)
{
	sdaIpiFunc();
}

/*
 *@brief
 *     功能:IPI初始化，挂接调试处理核间中断
 * @param[in]:func:核间中断调用该函数
 * @param[in]:timeout:超时时间
 *@return
 *    TA_OK:执行成功
 */
T_TA_ReturnCode sdaIpiEmit(IPI_HANDLER_FUNC	func, UINT32 timeout)
{
 	cpuset_t enableCpu = 0;
 	STATUS status = 0;

 	/* 设置中断处理函数 */
 	sdaIpiFunc = func;

	/* 获取使能的CPU */
	enableCpu = CPU_ENABLED();

	/* 排除自己 */
	TA_CPUSET_ATOMICCLR(enableCpu, GET_CPU_INDEX());

	/* 没有使能的远程CPU */
    if (CPUSET_ISZERO(enableCpu))
	{
    	return (TA_OK);
	}

	/* 投递核间中断挂起所有CPU */
	status = sdaIpiEmitFunc(TA_IPI_INTR_ID_DEBUG, enableCpu);
	if(OK != status)
	{
		return (TA_IPI_EMIT_FAILED);
	}

	switch (timeout)
	{
		case TA_NO_WAIT:
			break;

		case TA_WAIT_FOREVER:
			/* 确认所有CPU已经应答IPI中断，进入了空转状态 */
			while (!(enableCpu == sdaCpuSuspended))
			{
				/* 等待所有CPU进入空转状态 */
				DEBUG_CPU_DELAY();
			}
			break;
		default:
			break;
	}

	return TA_OK;
}

/*
 *@brief
 *     功能:循环控制，使其CPU处于空转状态
 *@return
 *    无
 */
void sdaCpuCtrlLoop(void)
{
    int cpuIndex = GET_CPU_INDEX();

    /* 当前CPU不是控制CPU */
    if (CPUSET_ISSET(sdaCpuCtrlRunning, cpuIndex))
    {
    	TA_CPUSET_ATOMICCLR(sdaCpuSuspended, cpuIndex);
    	return;
	}

    /* 设置休眠标志，表示当前CPU进入空转状态  */
    TA_CPUSET_ATOMICSET (sdaCpuSuspended, cpuIndex);

    /* 等待控制CPU恢复 */
    while (!CPUSET_ISZERO(sdaCpuCtrlRunning))
	{

	}

    /* 清除休眠标志 */
    TA_CPUSET_ATOMICCLR (sdaCpuSuspended, cpuIndex);
}

/*
 *@brief
 *     功能:休眠当前CPU
 *@return
 *    无
 */
static void sdaCpuLocalSuspend(void)
{
    int cpuIndex;

    cpuIndex = GET_CPU_INDEX();

    /* 当前CPU不是控制CPU且控制CPU已经设置 */
    if (CPUSET_ISZERO (sdaCpuCtrlRunning) ||
	CPUSET_ISSET (sdaCpuCtrlRunning, cpuIndex))
    {
        return;
    }

    /* 进入空转状态，等待退出  */
    sdaCpuCtrlLoop();
}
#ifdef __MIPS__
void IPI_Handle()
{

}
#endif
/*
 *@brief
 *     功能:IPI初始化，挂接调试处理核间中断
 *@return
 *    TA_OK:执行成功
 *    DA_CONNECT_IPI_FAILED:挂接核间中断处理函数失败
 *	  DA_ENABLE_IPI_FAILED:使能核间中断失败
 */

T_TA_ReturnCode sdaIpiInit(IPI_INSTALL_HANDLER_FUNC IPI_CONNECT, IPI_EMIT_HANDLER_FUNC IPI_EMIT, IPI_ENABLE_FUNC IPI_ENABLE)
{
	UINT32 i = 0;

#ifdef __MIPS__
	/* 安装调试核间中断处理函数  */
	IPI_CONNECT(TA_IPI_INTR_ID_DEBUG, (IPI_HANDLER_FUNC)IPI_Handle, 0);

	/*使能IPI TA_IPI_INTR_ID_DEBUG 中断*/
	 IPI_ENABLE(TA_IPI_INTR_ID_DEBUG);
#endif

#ifdef __X86__
	 /* 安装调试核间中断处理函数  */
	 	IPI_CONNECT(TA_IPI_INTR_ID_DEBUG, (IPI_HANDLER_FUNC)GET_CPU_INDEX, 0);

#endif
	/* 设置IPI投递句柄 */
	sdaIpiEmitFunc = IPI_EMIT;

	/* 初始化记录CPU状态数据 */
	for (i = 0; i < MAX_CPU_NUM; i++)
	{
	   cpuThreadstatus[i].threadId = 0;
	   cpuThreadstatus[i].cpuID = (~0);
	}

	/* 设置IPI初始化标志为  TRUE */
	sdaIpiInitialized = TRUE;

	return (TA_OK);
}

/*
 *@brief
 *     功能:检查CPU是否处于休眠状态
 * @param[in] cpuIndex:CPU索引号
 *@return
 *    TRUE:CPU处于休眠状态
 *    FALSE:CPU未处于休眠状态
 */
BOOL sdaCpuIsSuspended(int cpuIndex)
{
	if (CPUSET_ISSET(sdaCpuSuspended, cpuIndex))
	{
		/* CPU处于休眠状态 */
		return (TRUE);
	}

	/* CPU未处于休眠状态 */
	return (FALSE);
}

/*
 *@brief
 *     功能:IPI核间中断处理函数，控制CPU状态，使其CPU处于空转状态
 * @param[in] vector:中断号
 *@return
 *    无
 */
T_TA_ReturnCode sdaCpuCtrlHandler(void)
{
	UINT32 cpuIndex = GET_CPU_INDEX();
	UINT32 lockKey = 0;

	/* IPI未初始化 */
	if (sdaIpiInitialized == FALSE)
	{
		return (TA_IPI_NO_INIT);
	}

    /* 主CPU未恢复系统运行，或当前CPU是控制CPU */
    if (CPUSET_ISZERO(sdaCpuCtrlRunning) || CPUSET_ISSET(sdaCpuCtrlRunning, cpuIndex))
   	{
    	return (TA_OK);
   	}

	/* 保存从核CPUID和线程ID */
    cpuThreadstatus[cpuIndex].threadId = taTaskIdSelf();
	cpuThreadstatus[cpuIndex].cpuID = cpuIndex;

    /* 关CPU中断 */
    lockKey = INT_CPU_LOCK();

    /* 使其CPU进入空转状态 */
    sdaCpuCtrlLoop();

    /* 开中断  */
    INT_CPU_UNLOCK(lockKey);

    return (TA_OK);
}
    
 /*
  *@brief
  *     功能:挂起所有CPU
  *@return
  *    TA_OK:执行成功
  *    DA_IPI_NO_INIT:IPI未初始化化
  *	  DA_IPI_EMIT_FAILED:投递虚中断失败
  */
T_TA_ReturnCode sdaCpuAllSuspend(void)
{
	cpuset_t cpuIndex = 0;
 	cpuset_t currentCpuSet = 0;
 	T_TA_ReturnCode ctrlStatus = 0;

 	/* 获取CPU索引 */
 	cpuIndex = GET_CPU_INDEX();

 	/* 设置线程信息 */
	cpuThreadstatus[cpuIndex].threadId = taTaskIdSelf();
	cpuThreadstatus[cpuIndex].cpuID = cpuIndex;

 	/* IPI未初始化 */
 	if(sdaIpiInitialized == FALSE)
 	{
 		return (TA_IPI_NO_INIT);
 	}

    /* 通知当前CPU将休眠其它CPU */
    CPUSET_ZERO(currentCpuSet);
    CPUSET_SET(currentCpuSet, cpuIndex);

    /* 等待控制CPU退出,否则可能导致其它退出CPU再次进入sdaCpuLocalSuspend设置
     * sdaCpuSuspended标志，导致sdaCpuAllResume函数无法退出 */
    while(!sdaCpuAllResumeFlag);

    /* 当前CPU是否已经处于控制状态 */
    while (TA_MULTIOS_ATOMIC_CAS((atomic_t*)&sdaCpuCtrlRunning, 0,currentCpuSet) == FALSE)
    {
        /* 如果当前CPU已经处于控制状态，sdaCpuAllSuspendCnt只是简单加1 */
        if (sdaCpuCtrlRunning == currentCpuSet)
        {
            sdaCpuAllSuspendCnt++;

            return (TA_OK);
        }

 		/* 挂起当前CPU */
        sdaCpuLocalSuspend();

        /* 等待所有CPU退出sdaCpuCtrlLoop,否则有可能sdaCpuCtrlRunning设置了
         * 但是有的核还未退出sdaCpuCtrlLoop未清除sdaCpuSuspended标志，导致
         * sdaCpuAllResume函数无法退出 */
        while(!sdaCpuAllResumeFlag);
    }
#ifdef __MIPS__
#ifndef _KERNEL_DEBUG_
    taDbgCpuCtrlEnable();
#endif
#endif
    /* 内存屏障，内存操作施加顺序约束 */
    MEM_BARRIER_RW();

    /* 投递核间中断挂起所有CPU */
    if(TA_OK != sdaIpiEmit((IPI_HANDLER_FUNC)sdaCpuCtrlHandler, TA_WAIT_FOREVER))
    {
        return (TA_IPI_EMIT_FAILED);
    }

    /* 使能CPU控制 */
    if((ctrlStatus = sdaCpuCtrlEnable()) != TA_OK)
	{
        CPUSET_ZERO (sdaCpuCtrlRunning);

        /* 内存屏障，内存操作施加顺序约束 */
        MEM_BARRIER_RW();

        return (ctrlStatus);
	}

    sdaCpuAllSuspendCnt++;

    /* 设置所有CPU恢复标志为FALSE */
    TA_MULTIOS_ATOMIC_SET(&sdaCpuAllResumeFlag, FALSE);

    /* 控制CPU进入空转状态，该处调用主要是处理同时进入异常的情况 */
    sdaCpuCtrlLoop();

    return (TA_OK);
}

/*
 *@brief
 *     功能:恢复所有CPU
 *@return
 *    TA_OK:执行成功
 *    DA_IPI_NO_INIT:IPI未初始化化
 *	  DA_ILLEGAL_OPERATION:非法操作
 */
T_TA_ReturnCode sdaCpuAllResume(void)
{    
	INT32 i = 0;
	T_TA_ReturnCode status = TA_OK;

	/* IPI未初始化 */
    if(sdaIpiInitialized == FALSE)
    {
        return (TA_IPI_NO_INIT);
    }

 	/* 非法操作，只有主CPU才能执行该函数 */
    if (!CPUSET_ISSET(sdaCpuCtrlRunning, GET_CPU_INDEX()))
    {
        return (TA_ILLEGAL_OPERATION);
    }

    /* 主CPU多次调用sdaCpuAllSuspend函数 */
	if (--sdaCpuAllSuspendCnt > 0)
    {
        return (TA_OK);
    }

	/* 禁止CPU控制 */
    if ((status = sdaCpuCtrlDisable()) != TA_OK)
    {
    	return (status);
    }

    /* 通知所有从CPU，主CPU进行了恢复操作 */
    CPUSET_ZERO(sdaCpuCtrlRunning);
    
    /* 内存屏障，内存操作施加顺序约束 */
    MEM_BARRIER_RW();

    /* 等待所有从CPU退出空转状态 */
    while (!CPUSET_ISZERO (sdaCpuSuspended))
	{
        DEBUG_CPU_DELAY();
	}

	/* 清空记录的CPU状态数据 */
	for (i = 0; i < MAX_CPU_NUM; i++)
	{
	   cpuThreadstatus[i].threadId = 0;
	   cpuThreadstatus[i].cpuID = (~0);
	}

    /* 所有从CPU已经恢复运行 */
    TA_MULTIOS_ATOMIC_SET(&sdaCpuAllResumeFlag, TRUE);
#ifdef __MIPS__
#ifndef _KERNEL_DEBUG_
    taDbgCpuCtrlDisable();
#endif
#endif
    return (TA_OK);
}
#else



/* 任务信息 */
typedef struct
{
	unsigned long threadId;
	unsigned int cpuID;
}CPU_THREAD_INFO;



/* 线程状态 */
CPU_THREAD_INFO cpuThreadstatus[MAX_CPU_NUM];







unsigned int GET_CPU_INDEX(void)
{
  return 0;//taGetCpuID();
}

/*
 *@brief
 *     功能:通过线程ID获取CPU ID
 *@return
 *    如果成功返回CPU ID，否则返回0xffffffff
 */
unsigned int sdaGetCpuIdByThreadId(unsigned long threadId)
{
    int i = 0;

    for(i = 0;i < MAX_CPU_NUM; i++)
    {
        if(threadId == cpuThreadstatus[i].threadId)
        {
             return cpuThreadstatus[i].cpuID;
        }
    }

    return (~0);
}


#endif
