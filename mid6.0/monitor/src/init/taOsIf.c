/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                         �������ļ���
 */

/*
 * @file:taOsIf.c
 * @brief:
 *             <li>�Žӿڳ�ʼ������OS�ӿڷ�װ</li>
 */
 
/************************ͷ �� ��******************************/
#include "ta.h"
#include "taBridge.h"

/************************�� �� ��********************************/

/************************���Ͷ���*******************************/

/************************�ⲿ����*******************************/

/* IPI�˼��жϴ�����������CPU״̬��ʹ��CPU���ڿ�ת״̬ */
extern void sdaIpiHandler(void);
#ifdef __MIPS__
	extern void CPU_FlushCaches(unsigned int addr, unsigned int len);
	extern T_DB_ExceptionContext* taSDAExpContextPointerGet(UINT32 cpuIndex);
#endif
/************************ǰ������*******************************/
#ifdef __MIPS__
	extern taBpExceptionStub;
	extern T_TA_ReturnCode taUdpRead(UINT8* buf,UINT32 cnt,UINT32* dlen);
	extern T_TA_ReturnCode taUdpWrite(UINT8* buf,UINT32 cnt,UINT32* dlen);
	extern void TaPacketNumGet(UINT32* pTaTxPacketNum, UINT32* pTaRxPacketNum);
	extern int taGetSaid();
	extern unsigned int* taGetBreakInfo();
	extern BOOL taGetMultiThreadInfoTag();
#endif
/************************ģ�����*******************************/

/* �Žӿڱ��� */
static T_TA_BRIDGE  taBrigde;
/************************ȫ�ֱ���*******************************/

/* ��ʼ������ID */
int taInitTaskId = 0;
/************************����ʵ��*******************************/

/**
 * @brief:
 *     �Žӿڳ�ʼ��
 * @return:
 *      ��
*/
void taBrigdeInit(void)
{
    /* IPI��ʼ�� */
    taBrigde.sdaIpiInit = sdaIpiInit;
    
#ifdef __X86__    
    /* �����쳣������������ϵͳ���������˺��ļ������쳣�������������°�װ���ļ����������쳣���� */
    taBrigde.taTraceExceptionStub = taTraceExceptionStub;
#endif
    
    /* �ϵ��쳣������������ϵͳ���������˺��ļ������쳣�������������°�װ���ļ����Զϵ��쳣���� */
    taBrigde.taBpExceptionStub = taBpExceptionStub;
    
    /* ����IPI�жϻص����������ڱ����쳣������ */
    taBrigde.cpuCtrlHandler = sdaIpiHandler;
       
    /* �ڴ�̽���ʼ�� */
    taBrigde.taMemProbeInit = taMemProbeInit;
#ifdef __MIPS__ 
    taBrigde.taUdpRead = taUdpRead;
    taBrigde.taUdpWrite = taUdpWrite;

    taBrigde.TaPacketNumGet = TaPacketNumGet;

    taBrigde.taGetSaid = taGetSaid;
   
    taBrigde.taGetBreakInfo = taGetBreakInfo;
    taBrigde.taGetMultiThreadInfoTag = taGetMultiThreadInfoTag;
#endif
    /* ����ŵ�ַ */
    *((int*)TA_BRIGDE_ADDR) = (int)&taBrigde;
    
    /* ����TA������־ */
    *((int*)TA_START_FLAG_ADDR) = TA_START_FLAG_VALUE;
}

/**
 * @Brief
 *     ��ȡ������
 * @param[in]: taskID:����ID
 * @param[out]: taskNameStr:������
 * @return
 *     ERROR����δ��ʼ�����߻�ȡ������ʧ��
 *     OK����ȡ�������ɹ�
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
 *     ��ȡ��ǰ����ID
 * @return
 *     0����δ��ʼ�����߻�ȡ��ǰ����IDʧ��
 *     �ɹ����ص�ǰ����ID
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
 *     ��ȡϵͳ�������б�
 * @param[out]: idList:�����б�
 * @param[in]: maxTasks:�����б��ܹ����ɵ����������
 * @return
 *     0����δ��ʼ��
 *     �ɹ������������
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
 *     ��ȡ����������
 * @param[in]: taskId:����ID
 * @param[out]: context:�쳣������
 * @return
 *     ERROR����δ��ʼ�����߻�ȡ����������ʧ��
 *     OK����ȡ���������ĳɹ�
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
 *     ��ȡOS��ʹ�ܵ�CPU
 * @return
 *     �ɹ�����CPUʹ��
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
 *     ��ȡCPU ID
 * @return
 *     0����δ��ʼ��
 *     �ɹ�����CPU ID
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
 *      ˢ������Cache
 * @param[in]:addr: Ҫ���µ�cache�ڴ��ַ
 * @param[in]:len: Ҫ���µĳ���
 * @return:
 *      ��
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
 *     У������ID
 * @param[in]: taskId:����ID
 * @return
 *     ERROR����δ��ʼ������У������IDʧ��
 *     OK��У������ID�ɹ�
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
*    �������շ����ݵ�ģʽ��Ϊ��ѯģʽ
* @param  ��
* @return
*	OK:����ģʽ�л��ɹ�
*/
INT32 taSetEndToPoll(void)
{
    return OK;
}

/**
* @brief
*    �������շ����ݵ�ģʽ��Ϊ�ж�ģʽ
* @param  ��
* @return
*	OK:����ģʽ�л��ɹ�
*/
INT32 taSetEndToInt(void)
{
    return OK;
}

/*
 * @brief:
 *      ��ȡ���ԻỰ�а󶨵�����ID�б�
 * @param[in]: tid: ����ID
 * @param[out]: pSid: �ỰID
 * @return:
 *     NULL:δ�ҵ���Ӧ������ڵ�
 *     T_TASK_NODE:����ڵ�
 */
INT32 taGetTaskIdList(UINT32 sid, INT32 idList[], INT32 maxTasks)
{
    return 0;
}

/*
 * @brief:
 *     ��������
 * @param[in]:tid:����ID
 * @return:
 *     OK:��������ɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskActivate(int tid)
{
    return OK;
}

/*
 * @brief:
 *     ��������
 * @param[in]:tid:����ID
 * @return:
 *     OK:��������ɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskCont(int tid)
{
    return OK;
}
/**
 * @Brief
 *     ���ڴ�ӡ
 
void printUart(char *fmt, ...)
{
    if(NULL == taBrigde.printUart)
    {
	    return ERROR;
    }
	
    taBrigde.printUart(fmt);
} */
