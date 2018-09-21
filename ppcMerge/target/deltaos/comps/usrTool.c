/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-08-4         彭元志，北京科银京成技术有限公司
 *                         创建该文件
 */

/*
 * @file:usrTool.c
 * @brief:
 *             <li>工具相关初始化</li>
 */

#define VX_TYPE_USED
/************************头 文 件******************************/
#include <stdlib.h>
#include <delta.h>
#include <ioLib.h>
#include <config.h>
#include <config_component.h>
#include "config_bsp.h"
#include <cpusetCommon.h>
#include <private/vxCpuLibP.h>
#include "taskLibCommon.h"
#include "taBridge.h"
#include "ta.h"

/************************宏 定 义********************************/
#define FTP_FS_DEVICE_NAME   "host:"
/************************类型定义*******************************/
typedef T_TA_ReturnCode (*readP)(UINT8 *buf, UINT32 bufLen, UINT32 *receSize);
typedef T_TA_ReturnCode (*writeP)(UINT8 *buf, UINT32 bufLen, UINT32 *writeSize);
/************************外部声明*******************************/
extern void _ipi_emit_debug(unsigned int ipiNum,unsigned int cpus);
extern _Delta_Status   intConnect_debug(VOIDFUNCPTR *vector,VOIDFUNCPTR routine,int parameter);
extern int intEnable_debug(int intLevel);
extern STATUS wdbConfig(char* targetIP, UINT16 targetPort,
		char *endDeviceName, UINT32 endDeviceUnit, UINT32 timeout);
extern void taDebugInit(void);
extern void taTreatyUDPInit(void);
extern void mdlLibInit(void);
extern void rseLibInit(void);
extern void taManagerInit(void);
extern T_TA_ReturnCode taUdpRcvfrom(UINT8 *buf, UINT32 bufLen, UINT32 *receSize);
extern T_TA_ReturnCode taUdpSendto (UINT8 *buf, UINT32 bufLen, UINT32 *writeSize);
extern int sysCsExc;
extern T_TA_ReturnCode taTaskInitLib(void);
extern T_TA_ReturnCode taRtpInitLib(void);
extern void sdaIpiHandler(void);
extern void taInstallCpuCtrlHandle(void* pFunction);
extern void printUart(char *fmt, ...);
extern VXDBG_STATUS vxdbgCpuCtrlEnable(void);
extern void taIntVxdbgCpuRegsGet();
extern void taSetCommunicateMode();
extern void TaPacketNumSet();
extern void taSetSaid();
extern void taSyncBreakInfo();
extern void taSetMultiThreadInfoTag();

/************************前向声明*******************************/

/************************模块变量*******************************/

/* 通信设备操作接口 */
static T_TA_COMMIf taCOMMIfDevice;

/************************全局变量*******************************/

/* 桥地址 */
T_TA_BRIDGE  *taBrigde = NULL;

/*是否初始化WDB任务*/
BOOL isInitWdb = false;
/* WDB任务ID */
int	wdbTaskId = 0;

FUNCPTR _func_excDebugHandle = 0;
/************************函数实现*******************************/
/*
 * @brief:
 *     安装异常处理函数到IDT表中
 * @param[in] vector :异常号
 * @param[in] vectorHandle :异常处理函数
 * @return:
 *     无
 */
void taInstallVectorHandle(int vector,void *vectorHandle)
{
#ifdef __X86__
    intVecSet2 ((FUNCPTR *)INUM_TO_IVEC(vector), (FUNCPTR) vectorHandle,
    		IDT_INT_GATE, sysCsExc);
#endif

#ifdef __mips__
    intVecSet ((FUNCPTR *)INUM_TO_IVEC(vector), (FUNCPTR) vectorHandle);
#endif

#ifdef __PPC__
    //intVecSet ((FUNCPTR *)INUM_TO_IVEC(vector), (FUNCPTR) vectorHandle);

    if(vector == GENERAL_DB_INT)
    {
     	excVecSet((FUNCPTR *)EXC_TRACE,vectorHandle);
    }
    else if(vector == GENERAL_TRAP_INT)
    {
    	excVecSet((FUNCPTR *)EXC_PROGRAM,vectorHandle);
    }
    else if(vector == GENERAL_HBP_INT)
    {
    	excVecSet((FUNCPTR *)EXC_DSI,vectorHandle);
    }
    else if(vector == GENERAL_IAB_INT)
    {
    	excVecSet((FUNCPTR *)EXC_INSTRUCTION_ADDRESS_BREAKPOINT,vectorHandle);
    }

#endif
}

/*
 * @brief:
 *     初始化桥接口
 * @param[in] 无
 * @return:
 *     无
 */
void taBrigdeInit(void)
{

	/* 判断TA是否运行 */
	if(*((int*)TA_START_FLAG_ADDR) == TA_START_FLAG_VALUE)
	{
		taBrigde = (T_TA_BRIDGE*)(*((int*)TA_BRIGDE_ADDR));
		if(NULL != taBrigde)
		{
			taBrigde->taskIdSelf = taTaskIdSelf;
			taBrigde->taskName = taTaskName;
			taBrigde->taskIdListGet = taTaskIdListGet;
			taBrigde->taskContextGet = taTaskContextGet;
			taBrigde->taskContextSet = taTaskContextSet;
			taBrigde->cpuEnabledGet = taCpuEnabledGet;
			taBrigde->getCpuID = taGetCpuID;
			taBrigde->taskIdVerify = taTaskIdVerify;
			//taBrigde->printUart = printUart;
		}
		
	}
}

/*
 * @brief:
 *     初始化内存探测接口
 * @param[in] 无
 * @return:
 *     无
 */
void taBrigdeMemProbeInit(void)
{
	/* 判断TA是否运行 */
	if(*((int*)TA_START_FLAG_ADDR) == TA_START_FLAG_VALUE)
	{
		taBrigde = (T_TA_BRIDGE*)(*((int*)TA_BRIGDE_ADDR));
		
		if(NULL != taBrigde)
		{
			/* 内存探测初始化 */
			taBrigde->taMemProbeInit(&_func_excBaseHook);
		}
		
	}
	
}

/*
 * @brief:
 *      初始化调试设备
 * @return:
 *      无
 */
void taKernelDebugDeviceInit(void)
{
	/* 判断TA是否运行   */ 
	if(*((int*)TA_START_FLAG_ADDR) == TA_START_FLAG_VALUE)
	{
		/*核心级调试*/
		taBrigde = (T_TA_BRIDGE*)(*((int*)TA_BRIGDE_ADDR));
		
		if(NULL != taBrigde)
		{
			/* 初始化通信协议 */
			taCOMMIfDevice.read = (readP)taBrigde->taUdpRead;
			taCOMMIfDevice.write = (writeP)taBrigde->taUdpWrite;

			//设置通讯模式为monitor
			taSetCommunicateMode(0);

			/*初始化通信设备*/
			taCOMMIfDevice.commType = TA_COMM_UDP;
			taCOMMInit(&taCOMMIfDevice);
		}
		
	}
	
    /* 协议初始化 */
    taTreatyUDPInit();
}

/*
 * @brief:
 *     同步TA数据
 * @param[in] 无
 * @return:
 *     无
 */
void taSyncData()
{
	UINT32 taTxPacketNum = 0; /* 发送包序列号值 */
	UINT32 taRxPacketNum = 0; /* 接收包序列号值 */

	/* 判断TA是否运行 */
	if(*((int*)TA_START_FLAG_ADDR) == TA_START_FLAG_VALUE)
	{
		taBrigde = (T_TA_BRIDGE*)(*((int*)TA_BRIGDE_ADDR));
		
		if(NULL != taBrigde)
		{
			//同步包序号
			taBrigde->TaPacketNumGet(&taTxPacketNum,&taRxPacketNum);
			TaPacketNumSet(taTxPacketNum,taRxPacketNum);
			//同步said
			taSetSaid(taBrigde->taGetSaid());
			//同步断点列表
			taSyncBreakInfo(taBrigde->taGetBreakInfo());

			//同步多线程标志
			taSetMultiThreadInfoTag(taBrigde->taGetMultiThreadInfoTag());
		}
	
	}
	
}

/*
 * @brief:
 *     核心级调试初始化
 * @param[in] 无
 * @return:
 *     无
 */
void kernelDebugInit(void)
{
	//桥初始化
	taBrigdeInit();

	//核心调试设备初始化
	taKernelDebugDeviceInit();

    /* 初始化信息查询模块 */
    taManagerInit();

	/* 动态加载初始化 */
	mdlLibInit();

	/* 系统信息浏览器初始化 */
	rseLibInit();

    //调试初始化，此后不再使用monitor调试，只是用monitor的通信
	taDebugInit();

	/*同步数据*/
	taSyncData();

	/* 初始化内存探测 */
	taMemProbeInit(&_func_excBaseHook);

}

/*
 * @brief:
 *     多核调试初始化
 * @param[in] 无
 * @return:
 *     无
 */
void taSMPDebugInit()
{
	/* IPI初始化 */
//	sdaIpiInit(vxIpiConnect,vxIpiEmit,vxIpiEnable);
	sdaIpiInit(intConnect_debug,_ipi_emit_debug,intEnable_debug);

	/* 安装调试IPI中断处理函数 */
	taInstallCpuCtrlHandle(taIntVxdbgCpuRegsGet);
}

#ifdef	CONFIG_CONFIG_WDB
/*
 * @brief:
 *     wdb任务执行体
 * @param[in] 无
 * @return:
 *     无
 */
void taskWdb(void)
{
	/* 收包派发 */
	while(1)
	{
		taMessageDispatch();
	}
}

/*
 * @brief:
 *     创建wdb通信任务
 * @param[in] 无
 * @return:
 *     无
 */
int usrCreateWdbTask(int priority, int stackSize)
{
	return taskSpawn("wdbTask", priority, VX_FP_TASK | VX_UNBREAKABLE | VX_SUPERVISOR_MODE,
			stackSize, (FUNCPTR)taskWdb,0,0,0,0,0,0,0,0,0,0);
}

/*
 * @brief:
 *      初始化调试设备
 * @return:
 *      无
 */
void taDebugDeviceInit(void)
{
	/* 初始化通信协议 */
	taCOMMIfDevice.read = taUdpRcvfrom;
	taCOMMIfDevice.write = taUdpSendto;

	//设置通讯模式为wdb
	taSetCommunicateMode(1);

    /*初始化通信设备*/
	taCOMMIfDevice.commType = TA_COMM_UDP;
    taCOMMInit(&taCOMMIfDevice);
}

/*
 * @brief:
 *      WDB任务重启HOOK，WDB任务产生异常后，重新启动WDB任务
 * @param[in]：pTcbCurrent：任务控制块
 * @param[in]：vecNum：异常号
 * @param[in]：pEsf：异常压栈信息
 * @return:
 *     无
 */
void wdbTaskRestart(WIND_TCB *pTcbCurrent, int vecNum, void *pEsf)
{
	int tid = taskIdSelf();

	if (tid == wdbTaskId)
	{
		printf("wdb task exception. restarting agent ... vecNum:%x\n", vecNum);

		if(taskRestart(wdbTaskId) == ERROR);
			printf("Unable to restart tWdbTask.\n");
	}
}

/*
 * @brief:
 *      通信初始化
 * @return:
 *      无
 */
STATUS usrCommInit (char* targetIP, UINT16 targetPort,
		char *endDeviceName, UINT32 endDeviceUnit, UINT32 timeout)
{
	STATUS status = OK;
	/* wdb通信初始化 */
	status = wdbConfig(targetIP, targetPort, endDeviceName, endDeviceUnit, timeout);
	if(OK != status)
	{
		printf("wdbConfig failed!\n");
		return status;
	}
	if(!isInitWdb){
		/* 注册异常HOOK，WDB任务产生异常后，重新启动WDB任务 */
		excHookAdd((FUNCPTR)wdbTaskRestart);

		/* 创建wdb通信任务  */
		wdbTaskId = usrCreateWdbTask(1, 0x10000);
		isInitWdb = true;
	}
	return OK;
}

/*
 * @brief:
 *      工具初始化
 * @return:
 *      无
 */
STATUS usrToolsRootInit(void)
{
	/* 设备初始化 */
	taDebugDeviceInit();

	usrCommInit(CONFIG_IP, CONFIG_PORT, CONFIG_END_DEVICE_NAME, CONFIG_END_DEVICE_UNIT, 1000);

#if (defined(CONFIG_USR_DEBUG))
 	/* 任务级调试初始化 */
 	taTaskInitLib();

#if (defined(CONFIG_INCLUDE_RTP))
	/* RTP调试初始化 */
	taRtpInitLib();
#endif
#endif

#if (defined(CONFIG_INCLUDE_FTP_CLIENT))
	netDevCreate (FTP_FS_DEVICE_NAME, CONFIG_END_FTP_SEVER_IP, 1);
	iam(CONFIG_END_FTP_USER_NAME, CONFIG_END_FTP_USER_PASS);
	ioDefPathSet (FTP_FS_DEVICE_NAME);
#endif

	return OK;
}
#endif


