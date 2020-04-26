/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  sdaCpuLib.c
 * @brief
 *       ����:
 *       <li>��˿⣬�ṩ��غ������ƴ�CPU��״̬��ϵͳ��ģʽ�£�BSP�˿��ƴ�CPU������
 *       �ͻָ�</li>
 */
#ifdef CONFIG_CORE_SMP

/************************ͷ �� ��******************************/
#include "taskLibP.h"
#include "dbAtomic.h"
#include "dbContext.h"
#include "sdaCpuLib.h"
#include "string.h"
#include "sdaMain.h"
#include "ta.h"

/************************�� �� ��******************************/

/* �����Ƿ�ʹ�� */
#define CPU_CTRL_IS_ENABLED() (MULTIOS_ATOMIC_GET(&sdaCpuCtrlEnabled) == TRUE)

/* CPUʹ�� */
#define CPU_ENABLED() (taCpuEnabledGet())

/************************���Ͷ���*****************************/

/* ������Ϣ */
typedef struct
{
    UINT32 threadId;
    UINT32 cpuID;
}CPU_THREAD_INFO;

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/* ���ߵ�ǰCPU */
static void sdaCpuLocalSuspend(void);

/************************ģ�����*****************************/
/* ��¼����CPU״̬: 0=Running / 1=Suspended */
static volatile cpuset_t sdaCpuSuspended = 0;

/* ��ʾ�ĸ�CPUΪ��CPU������������CPU��״̬�������ú���sdaCpuAllSuspendʱ�����ã�
 * ����sdaCpuAllResumeʱ����� */
static volatile cpuset_t sdaCpuCtrlRunning = 0;

/* ��������CPU���� */
static UINT32 sdaCpuAllSuspendCnt = 0;

/* IPI��ʼ����־ */
BOOL sdaIpiInitialized = FALSE;

/* CPU����ʹ�ܱ�־ */
static atomic_t sdaCpuCtrlEnabled = FALSE;

/* ���д�CPU��λ��־ */
static atomic_t sdaCpuAllResumeFlag = TRUE;

/* Ͷ�ݺ˼��жϺ��� */
static IPI_EMIT_HANDLER_FUNC sdaIpiEmitFunc;

/* IPI������ */
static IPI_HANDLER_FUNC sdaIpiFunc;

/************************ȫ�ֱ���*****************************/

/* �߳�״̬ */
CPU_THREAD_INFO cpuThreadstatus[MAX_CPU_NUM];

/************************����ʵ��*****************************/

/*
 *@brief
 *     ����:��ȡ������ID
 *@return
 *    �ɹ���������ID
 */
UINT32 GET_CPU_INDEX(void)
{
  return taGetCpuID();
}

/*
 *@brief
 *     ����:ͨ���߳�ID��ȡCPU ID
 *@return
 *    ����ɹ�����CPU ID�����򷵻�0xffffffff
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
 *     ����:ʹ��CPU����
 *@return
 *    ����ɹ�����TA_OK
 */
T_TA_ReturnCode sdaCpuCtrlEnable(void)
{
	TA_MULTIOS_ATOMIC_SET(&sdaCpuCtrlEnabled, TRUE);

    return (TA_OK);
}

/*
 *@brief
 *     ����:��ֹCPU����
 *@return
 *    ����ɹ�����TA_OK
 */
T_TA_ReturnCode sdaCpuCtrlDisable(void)
{
	TA_MULTIOS_ATOMIC_SET(&sdaCpuCtrlEnabled, FALSE);

    return (TA_OK);
}

/*
 *@brief
 *     ����:IPI�˼��жϴ�����
 *@return
 *    ��
 */
void sdaIpiHandler(void)
{
	sdaIpiFunc();
}

/*
 *@brief
 *     ����:IPI��ʼ�����ҽӵ��Դ���˼��ж�
 * @param[in]:func:�˼��жϵ��øú���
 * @param[in]:timeout:��ʱʱ��
 *@return
 *    TA_OK:ִ�гɹ�
 */
T_TA_ReturnCode sdaIpiEmit(IPI_HANDLER_FUNC	func, UINT32 timeout)
{
 	cpuset_t enableCpu = 0;
 	STATUS status = 0;

 	/* �����жϴ����� */
 	sdaIpiFunc = func;

	/* ��ȡʹ�ܵ�CPU */
	enableCpu = CPU_ENABLED();

	/* �ų��Լ� */
	TA_CPUSET_ATOMICCLR(enableCpu, GET_CPU_INDEX());

	/* û��ʹ�ܵ�Զ��CPU */
    if (CPUSET_ISZERO(enableCpu))
	{
    	return (TA_OK);
	}

	/* Ͷ�ݺ˼��жϹ�������CPU */
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
			/* ȷ������CPU�Ѿ�Ӧ��IPI�жϣ������˿�ת״̬ */
			while (!(enableCpu == sdaCpuSuspended))
			{
				/* �ȴ�����CPU�����ת״̬ */
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
 *     ����:ѭ�����ƣ�ʹ��CPU���ڿ�ת״̬
 *@return
 *    ��
 */
void sdaCpuCtrlLoop(void)
{
    int cpuIndex = GET_CPU_INDEX();

    /* ��ǰCPU���ǿ���CPU */
    if (CPUSET_ISSET(sdaCpuCtrlRunning, cpuIndex))
    {
    	TA_CPUSET_ATOMICCLR(sdaCpuSuspended, cpuIndex);
    	return;
	}

    /* �������߱�־����ʾ��ǰCPU�����ת״̬  */
    TA_CPUSET_ATOMICSET (sdaCpuSuspended, cpuIndex);

    /* �ȴ�����CPU�ָ� */
    while (!CPUSET_ISZERO(sdaCpuCtrlRunning))
	{

	}

    /* ������߱�־ */
    TA_CPUSET_ATOMICCLR (sdaCpuSuspended, cpuIndex);
}

/*
 *@brief
 *     ����:���ߵ�ǰCPU
 *@return
 *    ��
 */
static void sdaCpuLocalSuspend(void)
{
    int cpuIndex;

    cpuIndex = GET_CPU_INDEX();

    /* ��ǰCPU���ǿ���CPU�ҿ���CPU�Ѿ����� */
    if (CPUSET_ISZERO (sdaCpuCtrlRunning) ||
	CPUSET_ISSET (sdaCpuCtrlRunning, cpuIndex))
    {
        return;
    }

    /* �����ת״̬���ȴ��˳�  */
    sdaCpuCtrlLoop();
}
#ifdef __MIPS__
void IPI_Handle()
{

}
#endif
/*
 *@brief
 *     ����:IPI��ʼ�����ҽӵ��Դ���˼��ж�
 *@return
 *    TA_OK:ִ�гɹ�
 *    DA_CONNECT_IPI_FAILED:�ҽӺ˼��жϴ�����ʧ��
 *	  DA_ENABLE_IPI_FAILED:ʹ�ܺ˼��ж�ʧ��
 */

T_TA_ReturnCode sdaIpiInit(IPI_INSTALL_HANDLER_FUNC IPI_CONNECT, IPI_EMIT_HANDLER_FUNC IPI_EMIT, IPI_ENABLE_FUNC IPI_ENABLE)
{
	UINT32 i = 0;

#ifdef __MIPS__
	/* ��װ���Ժ˼��жϴ�����  */
	IPI_CONNECT(TA_IPI_INTR_ID_DEBUG, (IPI_HANDLER_FUNC)IPI_Handle, 0);

	/*ʹ��IPI TA_IPI_INTR_ID_DEBUG �ж�*/
	 IPI_ENABLE(TA_IPI_INTR_ID_DEBUG);
#endif

#ifdef __X86__
	 /* ��װ���Ժ˼��жϴ�����  */
	 	IPI_CONNECT(TA_IPI_INTR_ID_DEBUG, (IPI_HANDLER_FUNC)GET_CPU_INDEX, 0);

#endif
	/* ����IPIͶ�ݾ�� */
	sdaIpiEmitFunc = IPI_EMIT;

	/* ��ʼ����¼CPU״̬���� */
	for (i = 0; i < MAX_CPU_NUM; i++)
	{
	   cpuThreadstatus[i].threadId = 0;
	   cpuThreadstatus[i].cpuID = (~0);
	}

	/* ����IPI��ʼ����־Ϊ  TRUE */
	sdaIpiInitialized = TRUE;

	return (TA_OK);
}

/*
 *@brief
 *     ����:���CPU�Ƿ�������״̬
 * @param[in] cpuIndex:CPU������
 *@return
 *    TRUE:CPU��������״̬
 *    FALSE:CPUδ��������״̬
 */
BOOL sdaCpuIsSuspended(int cpuIndex)
{
	if (CPUSET_ISSET(sdaCpuSuspended, cpuIndex))
	{
		/* CPU��������״̬ */
		return (TRUE);
	}

	/* CPUδ��������״̬ */
	return (FALSE);
}

/*
 *@brief
 *     ����:IPI�˼��жϴ�����������CPU״̬��ʹ��CPU���ڿ�ת״̬
 * @param[in] vector:�жϺ�
 *@return
 *    ��
 */
T_TA_ReturnCode sdaCpuCtrlHandler(void)
{
	UINT32 cpuIndex = GET_CPU_INDEX();
	UINT32 lockKey = 0;

	/* IPIδ��ʼ�� */
	if (sdaIpiInitialized == FALSE)
	{
		return (TA_IPI_NO_INIT);
	}

    /* ��CPUδ�ָ�ϵͳ���У���ǰCPU�ǿ���CPU */
    if (CPUSET_ISZERO(sdaCpuCtrlRunning) || CPUSET_ISSET(sdaCpuCtrlRunning, cpuIndex))
   	{
    	return (TA_OK);
   	}

	/* ����Ӻ�CPUID���߳�ID */
    cpuThreadstatus[cpuIndex].threadId = taTaskIdSelf();
	cpuThreadstatus[cpuIndex].cpuID = cpuIndex;

    /* ��CPU�ж� */
    lockKey = INT_CPU_LOCK();

    /* ʹ��CPU�����ת״̬ */
    sdaCpuCtrlLoop();

    /* ���ж�  */
    INT_CPU_UNLOCK(lockKey);

    return (TA_OK);
}
    
 /*
  *@brief
  *     ����:��������CPU
  *@return
  *    TA_OK:ִ�гɹ�
  *    DA_IPI_NO_INIT:IPIδ��ʼ����
  *	  DA_IPI_EMIT_FAILED:Ͷ�����ж�ʧ��
  */
T_TA_ReturnCode sdaCpuAllSuspend(void)
{
	cpuset_t cpuIndex = 0;
 	cpuset_t currentCpuSet = 0;
 	T_TA_ReturnCode ctrlStatus = 0;

 	/* ��ȡCPU���� */
 	cpuIndex = GET_CPU_INDEX();

 	/* �����߳���Ϣ */
	cpuThreadstatus[cpuIndex].threadId = taTaskIdSelf();
	cpuThreadstatus[cpuIndex].cpuID = cpuIndex;

 	/* IPIδ��ʼ�� */
 	if(sdaIpiInitialized == FALSE)
 	{
 		return (TA_IPI_NO_INIT);
 	}

    /* ֪ͨ��ǰCPU����������CPU */
    CPUSET_ZERO(currentCpuSet);
    CPUSET_SET(currentCpuSet, cpuIndex);

    /* �ȴ�����CPU�˳�,������ܵ��������˳�CPU�ٴν���sdaCpuLocalSuspend����
     * sdaCpuSuspended��־������sdaCpuAllResume�����޷��˳� */
    while(!sdaCpuAllResumeFlag);

    /* ��ǰCPU�Ƿ��Ѿ����ڿ���״̬ */
    while (TA_MULTIOS_ATOMIC_CAS((atomic_t*)&sdaCpuCtrlRunning, 0,currentCpuSet) == FALSE)
    {
        /* �����ǰCPU�Ѿ����ڿ���״̬��sdaCpuAllSuspendCntֻ�Ǽ򵥼�1 */
        if (sdaCpuCtrlRunning == currentCpuSet)
        {
            sdaCpuAllSuspendCnt++;

            return (TA_OK);
        }

 		/* ����ǰCPU */
        sdaCpuLocalSuspend();

        /* �ȴ�����CPU�˳�sdaCpuCtrlLoop,�����п���sdaCpuCtrlRunning������
         * �����еĺ˻�δ�˳�sdaCpuCtrlLoopδ���sdaCpuSuspended��־������
         * sdaCpuAllResume�����޷��˳� */
        while(!sdaCpuAllResumeFlag);
    }
#ifdef __MIPS__
#ifndef _KERNEL_DEBUG_
    taDbgCpuCtrlEnable();
#endif
#endif
    /* �ڴ����ϣ��ڴ����ʩ��˳��Լ�� */
    MEM_BARRIER_RW();

    /* Ͷ�ݺ˼��жϹ�������CPU */
    if(TA_OK != sdaIpiEmit((IPI_HANDLER_FUNC)sdaCpuCtrlHandler, TA_WAIT_FOREVER))
    {
        return (TA_IPI_EMIT_FAILED);
    }

    /* ʹ��CPU���� */
    if((ctrlStatus = sdaCpuCtrlEnable()) != TA_OK)
	{
        CPUSET_ZERO (sdaCpuCtrlRunning);

        /* �ڴ����ϣ��ڴ����ʩ��˳��Լ�� */
        MEM_BARRIER_RW();

        return (ctrlStatus);
	}

    sdaCpuAllSuspendCnt++;

    /* ��������CPU�ָ���־ΪFALSE */
    TA_MULTIOS_ATOMIC_SET(&sdaCpuAllResumeFlag, FALSE);

    /* ����CPU�����ת״̬���ô�������Ҫ�Ǵ���ͬʱ�����쳣����� */
    sdaCpuCtrlLoop();

    return (TA_OK);
}

/*
 *@brief
 *     ����:�ָ�����CPU
 *@return
 *    TA_OK:ִ�гɹ�
 *    DA_IPI_NO_INIT:IPIδ��ʼ����
 *	  DA_ILLEGAL_OPERATION:�Ƿ�����
 */
T_TA_ReturnCode sdaCpuAllResume(void)
{    
	INT32 i = 0;
	T_TA_ReturnCode status = TA_OK;

	/* IPIδ��ʼ�� */
    if(sdaIpiInitialized == FALSE)
    {
        return (TA_IPI_NO_INIT);
    }

 	/* �Ƿ�������ֻ����CPU����ִ�иú��� */
    if (!CPUSET_ISSET(sdaCpuCtrlRunning, GET_CPU_INDEX()))
    {
        return (TA_ILLEGAL_OPERATION);
    }

    /* ��CPU��ε���sdaCpuAllSuspend���� */
	if (--sdaCpuAllSuspendCnt > 0)
    {
        return (TA_OK);
    }

	/* ��ֹCPU���� */
    if ((status = sdaCpuCtrlDisable()) != TA_OK)
    {
    	return (status);
    }

    /* ֪ͨ���д�CPU����CPU�����˻ָ����� */
    CPUSET_ZERO(sdaCpuCtrlRunning);
    
    /* �ڴ����ϣ��ڴ����ʩ��˳��Լ�� */
    MEM_BARRIER_RW();

    /* �ȴ����д�CPU�˳���ת״̬ */
    while (!CPUSET_ISZERO (sdaCpuSuspended))
	{
        DEBUG_CPU_DELAY();
	}

	/* ��ռ�¼��CPU״̬���� */
	for (i = 0; i < MAX_CPU_NUM; i++)
	{
	   cpuThreadstatus[i].threadId = 0;
	   cpuThreadstatus[i].cpuID = (~0);
	}

    /* ���д�CPU�Ѿ��ָ����� */
    TA_MULTIOS_ATOMIC_SET(&sdaCpuAllResumeFlag, TRUE);
#ifdef __MIPS__
#ifndef _KERNEL_DEBUG_
    taDbgCpuCtrlDisable();
#endif
#endif
    return (TA_OK);
}
#else



/* ������Ϣ */
typedef struct
{
	unsigned long threadId;
	unsigned int cpuID;
}CPU_THREAD_INFO;



/* �߳�״̬ */
CPU_THREAD_INFO cpuThreadstatus[MAX_CPU_NUM];







unsigned int GET_CPU_INDEX(void)
{
  return 0;//taGetCpuID();
}

/*
 *@brief
 *     ����:ͨ���߳�ID��ȡCPU ID
 *@return
 *    ����ɹ�����CPU ID�����򷵻�0xffffffff
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
