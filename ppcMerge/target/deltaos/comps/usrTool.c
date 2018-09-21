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

/************************�� �� ��********************************/
#define FTP_FS_DEVICE_NAME   "host:"
/************************���Ͷ���*******************************/
typedef T_TA_ReturnCode (*readP)(UINT8 *buf, UINT32 bufLen, UINT32 *receSize);
typedef T_TA_ReturnCode (*writeP)(UINT8 *buf, UINT32 bufLen, UINT32 *writeSize);
/************************�ⲿ����*******************************/
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

FUNCPTR _func_excDebugHandle = 0;
/************************����ʵ��*******************************/
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
 *     ��ʼ���Žӿ�
 * @param[in] ��
 * @return:
 *     ��
 */
void taBrigdeInit(void)
{

	/* �ж�TA�Ƿ����� */
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
 *     ��ʼ���ڴ�̽��ӿ�
 * @param[in] ��
 * @return:
 *     ��
 */
void taBrigdeMemProbeInit(void)
{
	/* �ж�TA�Ƿ����� */
	if(*((int*)TA_START_FLAG_ADDR) == TA_START_FLAG_VALUE)
	{
		taBrigde = (T_TA_BRIDGE*)(*((int*)TA_BRIGDE_ADDR));
		
		if(NULL != taBrigde)
		{
			/* �ڴ�̽���ʼ�� */
			taBrigde->taMemProbeInit(&_func_excBaseHook);
		}
		
	}
	
}

/*
 * @brief:
 *      ��ʼ�������豸
 * @return:
 *      ��
 */
void taKernelDebugDeviceInit(void)
{
	/* �ж�TA�Ƿ�����   */ 
	if(*((int*)TA_START_FLAG_ADDR) == TA_START_FLAG_VALUE)
	{
		/*���ļ�����*/
		taBrigde = (T_TA_BRIDGE*)(*((int*)TA_BRIGDE_ADDR));
		
		if(NULL != taBrigde)
		{
			/* ��ʼ��ͨ��Э�� */
			taCOMMIfDevice.read = (readP)taBrigde->taUdpRead;
			taCOMMIfDevice.write = (writeP)taBrigde->taUdpWrite;

			//����ͨѶģʽΪmonitor
			taSetCommunicateMode(0);

			/*��ʼ��ͨ���豸*/
			taCOMMIfDevice.commType = TA_COMM_UDP;
			taCOMMInit(&taCOMMIfDevice);
		}
		
	}
	
    /* Э���ʼ�� */
    taTreatyUDPInit();
}

/*
 * @brief:
 *     ͬ��TA����
 * @param[in] ��
 * @return:
 *     ��
 */
void taSyncData()
{
	UINT32 taTxPacketNum = 0; /* ���Ͱ����к�ֵ */
	UINT32 taRxPacketNum = 0; /* ���հ����к�ֵ */

	/* �ж�TA�Ƿ����� */
	if(*((int*)TA_START_FLAG_ADDR) == TA_START_FLAG_VALUE)
	{
		taBrigde = (T_TA_BRIDGE*)(*((int*)TA_BRIGDE_ADDR));
		
		if(NULL != taBrigde)
		{
			//ͬ�������
			taBrigde->TaPacketNumGet(&taTxPacketNum,&taRxPacketNum);
			TaPacketNumSet(taTxPacketNum,taRxPacketNum);
			//ͬ��said
			taSetSaid(taBrigde->taGetSaid());
			//ͬ���ϵ��б�
			taSyncBreakInfo(taBrigde->taGetBreakInfo());

			//ͬ�����̱߳�־
			taSetMultiThreadInfoTag(taBrigde->taGetMultiThreadInfoTag());
		}
	
	}
	
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
	//�ų�ʼ��
	taBrigdeInit();

	//���ĵ����豸��ʼ��
	taKernelDebugDeviceInit();

    /* ��ʼ����Ϣ��ѯģ�� */
    taManagerInit();

	/* ��̬���س�ʼ�� */
	mdlLibInit();

	/* ϵͳ��Ϣ�������ʼ�� */
	rseLibInit();

    //���Գ�ʼ�����˺���ʹ��monitor���ԣ�ֻ����monitor��ͨ��
	taDebugInit();

	/*ͬ������*/
	taSyncData();

	/* ��ʼ���ڴ�̽�� */
	taMemProbeInit(&_func_excBaseHook);

}

/*
 * @brief:
 *     ��˵��Գ�ʼ��
 * @param[in] ��
 * @return:
 *     ��
 */
void taSMPDebugInit()
{
	/* IPI��ʼ�� */
//	sdaIpiInit(vxIpiConnect,vxIpiEmit,vxIpiEnable);
	sdaIpiInit(intConnect_debug,_ipi_emit_debug,intEnable_debug);

	/* ��װ����IPI�жϴ����� */
	taInstallCpuCtrlHandle(taIntVxdbgCpuRegsGet);
}

#ifdef	CONFIG_CONFIG_WDB
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

	//����ͨѶģʽΪwdb
	taSetCommunicateMode(1);

    /*��ʼ��ͨ���豸*/
	taCOMMIfDevice.commType = TA_COMM_UDP;
    taCOMMInit(&taCOMMIfDevice);
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
		printf("wdbConfig failed!\n");
		return status;
	}
	if(!isInitWdb){
		/* ע���쳣HOOK��WDB��������쳣����������WDB���� */
		excHookAdd((FUNCPTR)wdbTaskRestart);

		/* ����wdbͨ������  */
		wdbTaskId = usrCreateWdbTask(1, 0x10000);
		isInitWdb = true;
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
	/* �豸��ʼ�� */
	taDebugDeviceInit();

	usrCommInit(CONFIG_IP, CONFIG_PORT, CONFIG_END_DEVICE_NAME, CONFIG_END_DEVICE_UNIT, 1000);

#if (defined(CONFIG_USR_DEBUG))
 	/* ���񼶵��Գ�ʼ�� */
 	taTaskInitLib();

#if (defined(CONFIG_INCLUDE_RTP))
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


