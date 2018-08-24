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
 *             <li>������س�ʼ����</li>
 */

#define VX_TYPE_USED
/************************ͷ �� ��******************************/
#include <stdlib.h>
#include <vxworks.h>
#include <ioLib.h>
#include <config.h>
#include <config_component.h>
#include "config_bsp.h"
#include <cpusetCommon.h>
#include <private/vxCpuLibP.h>
#include "taskLibCommon.h"
#include "taBridge.h"
#include "ta.h"
//#include <DeltaVer.h>
//ȱ�ٻ�ȡ�汾��Ϣ��ͷ�ļ�

/************************�� �� ��********************************/
#define FTP_FS_DEVICE_NAME   "host:"
/************************���Ͷ���*******************************/
int RTPTextProtect=TRUE;

/************************�ⲿ����*******************************/
extern STATUS vxIpiEmit(INT32 ipiId, cpuset_t cpus);
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

#if (defined(CONFIG_INCLUDE_FTP_CLIENT))
extern STATUS 	iam ();
#endif

/************************ǰ������*******************************/

/************************ģ�����*******************************/

/* ͨ���豸�����ӿ� */
static T_TA_COMMIf taCOMMIfDevice;

/************************ȫ�ֱ���*******************************/

/* �ŵ�ַ */
T_TA_BRIDGE  *taBrigde = NULL;

/*�Ƿ��ʼ��WDB����*/
BOOL isInitWdb = 0;

/* WDB����ID */
ulong_t	wdbTaskId = 0;
/************************����ʵ��*******************************/

/*
 * @brief:
 *     ��ʼ���Žӿ�
 * @param[in] ��
 * @return:
 *     ��
 */
//�ýӿ�Ϊ���ļ����������Žӽӿڣ�ϵͳ�������񼶲�����Ҫ�����ڴ���ta����uboot��������ýӿ���Ч������ϵͳҲ�������
void taBrigdeInit(void)
{
	/* �ж�TA�Ƿ����� */
	if(*((ulong_t*)TA_START_FLAG_ADDR) != TA_START_FLAG_VALUE)
	{
		return;
	}

	taBrigde = (T_TA_BRIDGE*)(*((ulong_t*)TA_BRIGDE_ADDR));

	/* change by chenjq on 2016-10-17: ���ļ����Բ���Ҫ�����³�ʼ����*/
#ifdef CONFIG_USR_DEBUG	

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
#endif

 	/* ����IPI��ʼ�� */
	//taBrigde->sdaIpiInit(vxIpiConnect,vxIpiEmit);  //fixme by luosy

	/* �ڴ�̽���ʼ�� */
	taBrigde->vxMemArchProbe = taMemArchProbe;

#if(CONFIG_ENABLE_LP64 == false)	

	taBrigde->taMemProbeInit(&_func_excBaseHook);
#endif

}

#if(CONFIG_ENABLE_LP64 == false)	
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
#endif

/*
 * @brief:
 *     ���ļ����Գ�ʼ��
 * @param[in] ��
 * @return:
 *     ��
 */
//�ýӿ�Ϊ���ļ����������Žӽӿڣ�ϵͳ�������񼶲�����Ҫ�����ڴ���ta����uboot��������ýӿ���Ч������ϵͳҲ�������
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
//ARM���ڵĴ���ʽ�ǣ����˹��Ӻ�������û���쳣����������ȥ��������������ӿ�����ʵ�������Ժ���ܻ��õ����ʱ���
void taInstallVectorHandle(int vector,void *vectorHandle)
{
#ifdef __X86__
#if defined(CONFIG_TA_LP64) && defined(_TA_CONFIG_RTP)

	excConnect(vector, vectorHandle);

#else

	intVecSet2 ((FUNCPTR *)INUM_TO_IVEC(vector), (FUNCPTR) vectorHandle,
			IDT_INT_GATE, sysCsExc);
#endif
#endif

#ifdef __ARM__
    excConnect(vector, vectorHandle);
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
ulong_t usrCreateWdbTask(int priority, int stackSize)
{
	return (ulong_t)taskSpawn("wdbTask", priority, VX_FP_TASK | VX_UNBREAKABLE | VX_SUPERVISOR_MODE,
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
void wdbTaskRestart(WIND_TCB *pTcbCurrent, int vecNum, ESF *pEsf)
{
	ulong_t tid = (ulong_t)taskIdSelf();

	if (tid == wdbTaskId)
	{
		printf("wdb task exception. restarting agent ... vecNum:%x PC:%x\n",vecNum,pEsf->pc);

		if(taskRestart((TASK_ID)wdbTaskId) == ERROR);
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
		excHookAdd((void *)wdbTaskRestart);

		/* ����wdbͨ������  */
		wdbTaskId = usrCreateWdbTask(1, 0x10000);
		isInitWdb = 1;
		 /*
		  *FIXMELIY:����Ϊ�������û�ͨ������Ӧ�ü�ͨ��ʱ����IP��ַ���˽ӿ���ͨ��ts���͵�
		  *��Ϣ���õġ�����������������ã����񴴽���ɺ�pTcb->taskStd[0/1/2]�����FD������
		  *�׽��ֵ�FD������ͨ��IDE����RTPʱ��û���κδ�ӡ��Ϣ��������Ҫ����
		  *����wdbTaskId�ı�׼���롢����������FD.
		  */		  
		  ioTaskStdSet((TASK_ID)wdbTaskId,STD_IN,STD_IN);
		  ioTaskStdSet((TASK_ID)wdbTaskId,STD_OUT,STD_OUT);
		  ioTaskStdSet((TASK_ID)wdbTaskId,STD_ERR,STD_ERR);
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

	/* ���񼶵��Գ�ʼ�� */
	taTaskInitLib();

    /* IPI��ʼ�� */  //shield by luosy
	//sdaIpiInit(vxIpiConnect,vxIpiEmit);

	/* ��ʼ���ڴ�̽�� */
	//�����ʼ���ڴ�̽���Ŀ���Ǵ򿪺�taMemProbeInitialized����ʵ����taMemCommonProbeֻ��x86����Ч����x64�϶��Ѿ�ʧЧ��
	//��ʹ�õ��ǲ���ϵͳ�ṩ��vxMemArchProbe�ӿڣ��������Լ��ظ������ӿڡ�
	//��˲���taMemProbeInitialized��True����False����Ҫ�����ڴ�̽�⡣
	//����ʹ�õ���vxMemProbeInit�ӿ��������ڴ�̽���ʼ�����ýӿ������á�
	//��Ϊ���ڵ��Ե��쳣������ռ����_func_excBaseHook�����ڴ�̽���쳣������Ҳ�ǹ���������ġ�
	//taMemProbeInit(&_func_excBaseHook);
	vxMemProbeInit ();

 	/* ��װ����IPI�жϴ����� */   //shield by luosy
 	//taInstallCpuCtrlHandle(sdaIpiHandler);

#if (defined(CONFIG_INCLUDE_RTP))

	/* RTP���Գ�ʼ�� */
	//taRtpInitLib();
#endif
#endif

/* change by chenjq on 2016-10-19: ����Զ�̷���ftp����������*/
#if (defined(CONFIG_INCLUDE_FTP_CLIENT))
	netDrv ();
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

	//num=sprintf(buff,"%s%d.%d,%d",DELTAOS_RUNTIME_NAME,_DELTAOS_MAJOR,_DELTAOS_MINOR,arch_num);
	num=sprintf(buff,"%s%d.%d,%d","DeltaOS",6,2,arch_num);

	return buff;

}
