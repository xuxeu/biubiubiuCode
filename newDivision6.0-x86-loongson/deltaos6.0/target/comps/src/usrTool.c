/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-08-4         ��Ԫ־�������������ɼ������޹�˾
 *                         �������ļ�
 */

/*
 * @file:usrTool.c
 * @brief:
 *             <li>������س�ʼ��</li>
 */

#define VX_TYPE_USED
/************************ͷ �� ��******************************/
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

/************************�� �� ��********************************/
#define FTP_FS_DEVICE_NAME   "host:"
/************************���Ͷ���*******************************/
#ifdef __MIPS__
#define TAESF0 void
#endif
#ifdef __X86__
#define TAESF0 ESF0
#endif
/************************�ⲿ����*******************************/
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
/************************ǰ������*******************************/

/************************ģ�����*******************************/
/* ͨ���豸�����ӿ� */
static T_TA_COMMIf taCOMMIfDevice;

/************************ȫ�ֱ���*******************************/

/* �ŵ�ַ */
T_TA_BRIDGE  *taBrigde = NULL;

/*�Ƿ��ʼ��WDB����*/
BOOL isInitWdb = false;
/* WDB����ID */
int	wdbTaskId = 0;
/************************����ʵ��*******************************/

/*
 * @brief:
 *     ��ʼ���Žӿ�
 * @param[in] ��
 * @return:
 *     ��
 */
void taBrigdeInit(void)
{
	/* �ж�TA�Ƿ����� */
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

 	/* ��װ����IPI�жϴ����� */
 	taInstallCpuCtrlHandle((void*)taBrigde->cpuCtrlHandler);

#ifdef __X86__
 	/* ����IPI��ʼ�� */
	taBrigde->sdaIpiInit(vxIpiConnect,vxIpiEmit,NULL);
#endif

#ifdef __MIPS__
 	/* ����IPI��ʼ�� */
	taBrigde->sdaIpiInit(vxIpiConnect,vxIpiEmit,vxIpiEnable);
#endif
	/*
	  *change by liy on 2016-7-1:��ʹ��MMU���ڴ�̽���ʼ���󣬷��ʴ�����߼���ַʱ��
	  *��Ӱ����ļ����ԡ�
	  */	
	  
	/* �ڴ�̽���ʼ�� */
	taBrigde->taMemProbeInit(&_func_excBaseHook);

}

/*
 * @brief:
 *     ��ʼ���ڴ�̽��ӿ�
 * @param[in] ��
 * @return:
 *     ��
 */
void taBrigdeMemProbeInit(void)
{
	/* �ж�TA�Ƿ����� */
	if(*((int*)TA_START_FLAG_ADDR) != TA_START_FLAG_VALUE)
	{
		return;
	}

	taBrigde = (T_TA_BRIDGE*)(*((int*)TA_BRIGDE_ADDR));
	/* �ڴ�̽���ʼ�� */
	taBrigde->taMemProbeInit(&_func_excBaseHook);
}
/*
 * @brief:
 *     ���ļ����Գ�ʼ��
 * @param[in] ��
 * @return:
 *     ��
 */
void kernelDebugInit(void)
{
	taBrigdeInit();

}

#ifdef CONFIG_USR_DEBUG

/*
 * @brief:
 *     ��װ�쳣��������IDT����
 * @param[in] vector :�쳣��
 * @param[in] vectorHandle :�쳣������
 * @return:
 *     ��
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
 *     wdb����ִ����
 * @param[in] ��
 * @return:
 *     ��
 */
void taskWdb(void)
{
	/* �հ��ɷ� */
	while(1)
	{
          
            taMessageDispatch();


	}
}

/*
 * @brief:
 *     ����wdbͨ������
 * @param[in] ��
 * @return:
 *     ��
 */
int usrCreateWdbTask(int priority, int stackSize)
{
	return taskSpawn("wdbTask", priority, VX_FP_TASK | VX_UNBREAKABLE | VX_SUPERVISOR_MODE,
			stackSize, (FUNCPTR)taskWdb,0,0,0,0,0,0,0,0,0,0);
}

/*
 * @brief:
 *      ��ʼ�������豸
 * @return:
 *      ��
 */
void taDebugDeviceInit(void)
{
	/* ��ʼ��ͨ��Э�� */
	taCOMMIfDevice.read = taUdpRcvfrom;
	taCOMMIfDevice.write = taUdpSendto;

    /*��ʼ��ͨ���豸*/
	taCOMMIfDevice.commType = TA_COMM_UDP;
    taCOMMInit(&taCOMMIfDevice);

    /* Э���ʼ�� */
    taTreatyUDPInit();

}

/*
 * @brief:
 *      WDB��������HOOK��WDB��������쳣����������WDB����
 * @param[in]��pTcbCurrent��������ƿ�
 * @param[in]��vecNum���쳣��
 * @param[in]��pEsf���쳣ѹջ��Ϣ
 * @return:
 *     ��
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
 *      ͨ�ų�ʼ��
 * @return:
 *      ��
 */
STATUS usrCommInit (char* targetIP, UINT16 targetPort,
		char *endDeviceName, UINT32 endDeviceUnit, UINT32 timeout)
{

	STATUS status = OK;
	/* wdbͨ�ų�ʼ�� */
	
	status = wdbConfig(targetIP, targetPort, endDeviceName, endDeviceUnit, timeout);
	if(OK != status)
	{
		printf("error error error error error error error");
		return status;
	}

	
	if(!isInitWdb){
		/* ע���쳣HOOK��WDB��������쳣����������WDB���� */
		excHookAdd((FUNCPTR)wdbTaskRestart);

		/* ����wdbͨ������  */
		wdbTaskId = usrCreateWdbTask(1, 0x10000);
	
		isInitWdb = true;

		/*
		  *FIXMELIY:����Ϊ�������û�ͨ������Ӧ�ü�ͨ��ʱ����IP��ַ���˽ӿ���ͨ��ts���͵�
		  *��Ϣ���õġ�����������������ã����񴴽���ɺ�pTcb->taskStd[0/1/2]�����FD������
		  *�׽��ֵ�FD������ͨ��IDE����RTPʱ��û���κδ�ӡ��Ϣ��������Ҫ����
		  *����wdbTaskId�ı�׼���롢����������FD.
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
 *      ���߳�ʼ��
 * @return:
 *      ��
 */
STATUS usrToolsRootInit(void)
{
   /* ��ʼ����Ϣ��ѯģ�� */
	taManagerInit();

	/* �豸��ʼ�� */
	taDebugDeviceInit();

	/* ��̬���س�ʼ�� */
	mdlLibInit();

	/* ϵͳ��Ϣ�������ʼ�� */
	rseLibInit();

#if (defined(CONFIG_USR_DEBUG))

	/* ���Գ�ʼ�� */
	taDebugInit();

    /* IPI��ʼ�� */
#ifdef __MIPS__
	sdaIpiInit(vxIpiConnect,vxIpiEmit,vxIpiEnable);
#endif
#ifdef __X86__
	sdaIpiInit(vxIpiConnect,vxIpiEmit,NULL);
#endif

	/* ��ʼ���ڴ�̽�� */
	taMemProbeInit(&_func_excBaseHook);

 	/* ��װ����IPI�жϴ����� */
 	taInstallCpuCtrlHandle(sdaIpiHandler);

	/* ���񼶵��Գ�ʼ�� */
	taTaskInitLib();

#if (defined(CONFIG_INCLUDE_RTP))

	/* ���񼶵��Գ�ʼ�� */
	/* taTaskInitLib(); */

	/* RTP���Գ�ʼ�� */
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

