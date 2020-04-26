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
#include <DeltaVer.h>

/************************宏 定 义********************************/
#define FTP_FS_DEVICE_NAME   "host:"
/************************类型定义*******************************/
#ifdef __MIPS__
#define TAESF0 void
#endif
#ifdef __X86__
#define TAESF0 ESF0
#endif
/************************外部声明*******************************/
extern STATUS vxIpiEmit(INT32 ipiId, cpuset_t cpus);
#ifdef __MIPS__
extern STATUS vxIpiEnable(INT32 ipiId);
#endif
extern STATUS wdbConfig(char* targetIP, UINT16 targetPort,
		char *endDeviceName, UINT32 endDeviceUnit, UINT32 timeout);
extern void taDebugInit(void);
extern void taTreatyUDPInit(void);
extern void mdlLibInit(void);
extern void rseLibInit(void);
extern void taManagerInit(void);
/*extern void vxIpiConnect(void);*//*del by chenh 20141015*/
extern T_TA_ReturnCode taUdpRcvfrom(UINT8 *buf, UINT32 bufLen, UINT32 *receSize);
extern T_TA_ReturnCode taUdpSendto (UINT8 *buf, UINT32 bufLen, UINT32 *writeSize);
extern int sysCsExc;
extern T_TA_ReturnCode taTaskInitLib(void);
extern T_TA_ReturnCode taRtpInitLib(void);
extern void sdaIpiHandler(void);
extern void taInstallCpuCtrlHandle(void* pFunction);
#ifdef __MIPS__
extern void printUart(char *fmt, ...);
extern VXDBG_STATUS vxdbgCpuCtrlEnable(void);
extern void taIntVxdbgCpuRegsGet();
#endif
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
/************************函数实现*******************************/

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
	if(*((int*)TA_START_FLAG_ADDR) != TA_START_FLAG_VALUE)
	{
		return;
	}

	taBrigde = (T_TA_BRIDGE*)(*((int*)TA_BRIGDE_ADDR));
 	taBrigde->taskIdSelf = taTaskIdSelf;
 	taBrigde->taskName = taTaskName;
 	taBrigde->taskIdListGet = taTaskIdListGet;
 	taBrigde->taskContextGet = taTaskContextGet;
 	taBrigde->taskContextSet = taTaskContextSet;
 	taBrigde->cpuEnabledGet = taCpuEnabledGet;
 	taBrigde->getCpuID = taGetCpuID;
 	taBrigde->taskIdVerify = taTaskIdVerify;

 	/* 安装调试IPI中断处理函数 */
 	taInstallCpuCtrlHandle((void*)taBrigde->cpuCtrlHandler);

#ifdef __X86__
 	/* 调试IPI初始化 */
	taBrigde->sdaIpiInit(vxIpiConnect,vxIpiEmit,NULL);
#endif

#ifdef __MIPS__
 	/* 调试IPI初始化 */
	taBrigde->sdaIpiInit(vxIpiConnect,vxIpiEmit,vxIpiEnable);
#endif
	/*
	  *change by liy on 2016-7-1:在使能MMU后，内存探测初始化后，访问错误的逻辑地址时才
	  *不影响核心级调试。
	  */	
	  
	/* 内存探测初始化 */
	taBrigde->taMemProbeInit(&_func_excBaseHook);

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
	if(*((int*)TA_START_FLAG_ADDR) != TA_START_FLAG_VALUE)
	{
		return;
	}

	taBrigde = (T_TA_BRIDGE*)(*((int*)TA_BRIGDE_ADDR));
	/* 内存探测初始化 */
	taBrigde->taMemProbeInit(&_func_excBaseHook);
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
	taBrigdeInit();

}

#ifdef CONFIG_USR_DEBUG

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

#ifdef __MIPS__
    intVecSet ((FUNCPTR *)INUM_TO_IVEC(vector), (FUNCPTR) vectorHandle);
#endif
}
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

    /*初始化通信设备*/
	taCOMMIfDevice.commType = TA_COMM_UDP;
    taCOMMInit(&taCOMMIfDevice);

    /* 协议初始化 */
    taTreatyUDPInit();

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
 
void wdbTaskRestart(WIND_TCB *pTcbCurrent, int vecNum, TAESF0 *pEsf)
{
	int tid = taskIdSelf();

	if (tid == wdbTaskId)
	{
#ifdef __MIPS__
        printf("wdb task exception. restarting agent ... vecNum:%x\n", vecNum);
#else
		printf("wdb task exception. restarting agent ... vecNum:%x PC:%x\n",vecNum,pEsf->pc);
#endif
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
		printf("error error error error error error error");
		return status;
	}

	
	if(!isInitWdb){
		/* 注册异常HOOK，WDB任务产生异常后，重新启动WDB任务 */
		excHookAdd((FUNCPTR)wdbTaskRestart);

		/* 创建wdb通信任务  */
		wdbTaskId = usrCreateWdbTask(1, 0x10000);
	
		isInitWdb = true;

		/*
		  *FIXMELIY:由于为了满足用户通过连接应用级通道时设置IP地址，此接口是通过ts发送的
		  *信息调用的。由于是网络任务调用，任务创建完成后，pTcb->taskStd[0/1/2]保存的FD是网络
		  *套接字的FD，导致通过IDE运行RTP时，没有任何打印信息。所以需要重新
		  *设置wdbTaskId的标准输入、输出、错误的FD.
		  */
#ifdef __MIPS__

          taskCpuAffinitySet(wdbTaskId,1);

#endif

		  ioTaskStdSet(wdbTaskId,STD_IN,STD_IN);
		  ioTaskStdSet(wdbTaskId,STD_OUT,STD_OUT);
		  ioTaskStdSet(wdbTaskId,STD_ERR,STD_ERR);
		  printf("DEBUG IP:%s\n", targetIP);
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
   /* 初始化信息查询模块 */
	taManagerInit();

	/* 设备初始化 */
	taDebugDeviceInit();

	/* 动态加载初始化 */
	mdlLibInit();

	/* 系统信息浏览器初始化 */
	rseLibInit();

#if (defined(CONFIG_USR_DEBUG))

	/* 调试初始化 */
	taDebugInit();

    /* IPI初始化 */
#ifdef __MIPS__
	sdaIpiInit(vxIpiConnect,vxIpiEmit,vxIpiEnable);
#endif
#ifdef __X86__
	sdaIpiInit(vxIpiConnect,vxIpiEmit,NULL);
#endif

	/* 初始化内存探测 */
	taMemProbeInit(&_func_excBaseHook);

 	/* 安装调试IPI中断处理函数 */
 	taInstallCpuCtrlHandle(sdaIpiHandler);

	/* 任务级调试初始化 */
	taTaskInitLib();

#if (defined(CONFIG_INCLUDE_RTP))

	/* 任务级调试初始化 */
	/* taTaskInitLib(); */

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



char * versionNumber()
{
	int num = 0;
	int arch_num = 0;
	static char buff[100];

#ifdef CONFIG_TA_LP64
	arch_num = 64;

#else
	arch_num = 32;

#endif

	num=sprintf(buff,"%s%d.%d,%d",RUNTIME_NAME,_DELTAOS_MAJOR,_DELTAOS_MINOR,arch_num);
	return buff;

}

