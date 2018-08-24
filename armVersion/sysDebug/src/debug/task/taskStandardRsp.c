/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-11-27         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:taskStandardRsp.c
 * @brief:
 *             <li>�������RSP����ʵ�֣�������ϵͳ�����RSP����ʽ��ͬ�����Ե����������</li>
 */

/************************ͷ �� ��*****************************/
#include "dbStandardRsp.h"
#include "taExceptionReport.h"
#include "taskStandardRsp.h"
#include "taskCtrLib.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������******************************/

/************************ģ�����******************************/

/************************ȫ�ֱ���******************************/

/* NOTE:����GJB5369��׼,����������±�,������ӻ�ɾ�����������ע���޸����� */
T_DB_RspCommandTable taTaskRSPCmdTable[TA_TASK_RSP_TABLE_LEN] =
{
    {"?",                RspQueryEvent,                TA_NEED_REPLY},
    {"M",                RspWriteMemory,               TA_NEED_REPLY},
    {"m",                RspReadMemory,                TA_NEED_REPLY},
    {"X",                RspWriteBinaryMemory,         TA_NEED_REPLY},
    {"x",                RspReadBinaryMemory,          TA_NEED_REPLY},
    {"P",                RspWriteSingleRegister,       TA_NEED_REPLY},
    {"G",                RspWriteAllRegister,          TA_NEED_REPLY},
    {"g",                RspReadAllRegister,           TA_NEED_REPLY},
    {"p",                RspReadSingleRegister,        TA_NEED_REPLY},
    {"Z",                RspSetBreakpoint,             TA_NEED_REPLY},
    {"z",                RspRemoveBreakpoint,          TA_NEED_REPLY},
    {"!",                taTaskRspPauseProc,           TA_NEED_REPLY},
    {"c",                RspContinue,                  TA_NO_REPLY  },
    {"s",                RspStep,                      TA_NO_REPLY  },
    {"qC",    	         RspGetCurThreadID,            TA_NEED_REPLY},
    {"qfThreadInfo",     taTaskRspGetThreadIdList,     TA_NEED_REPLY},
    {"qsThreadInfo",     taTaskRspGetThreadIdList,     TA_NEED_REPLY},
    {"Hg",    	         RspSetThreadSubsequentOptHg,  TA_NEED_REPLY},
    {"T",    	         taTaskRspThreadIsActive,      TA_NEED_REPLY},
    {"qThreadExtraInfo", RspThreadExtraInfo,           TA_NEED_REPLY},
    {"autoAttachTask",   taTaskAutoAttachTask,         TA_NEED_REPLY},
    {NULL,               NULL,                         TA_NO_REPLY  }
};

/************************ʵ   ��*******************************/

/*
 * @brief:
 *      �������񼶵�����ͣ����
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *     TA_DB_NEED_IRET:ִ�гɹ�
 */
T_TA_ReturnCode taTaskRspPauseProc(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	/* �����ǰ�����쳣״̬����ִ����ͣ���� */
	if (!TA_DEBUG_IS_CONTINUE_STATUS(debugInfo))
	{
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
	}

	/* ���񼶵�����ͣ */
	if(OK != taTaskPause(debugInfo->sessionID))
	{
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_PAUSE_TASK_FAIL);
        return (TA_FAIL);
	}

	/* �����쳣�ظ���Ϣ */
	taSetExceptionStopReply(outbuf, SIGNAL_TRAP, TA_GET_CUR_THREAD_ID(debugInfo));

	/* ����������ݴ�С */
	*outSize = strlen((const char *)outbuf);

    return (TA_OK);
}

/*
 * @brief:
 *      ����"qfThreadInfo"��"qsThreadInfo"����,��ȡ���ԻỰ�а󶨵ĵ�������ID�б�
 *      ���������ȷ��� qfThreadInfo�����ȡ��һ���߳���Ϣ��Ȼ����qsThreadInfo����
 *      ��ȡʣ�µ��߳���Ϣ����Ŀ����˻ظ�'l'��ʾ�߳���Ϣ�ظ����
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode taTaskRspGetThreadIdList(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    static ulong_t threadIdList[MAX_TASKS_NUM] = {0};
    static UINT32 count = 0;
    static UINT32 index = 0;
	
    /* �����qfThreadInfo�����ʾ��ȡ��һ���߳�ID��ͨ��taskIdListGet��ȡ�߳�ID�б�  */
    if (!strncmp((const char *)inbuf, "qfThreadInfo", strlen("qfThreadInfo")))
    {
		/* ��ȡ���ԻỰ�а󶨵��߳�ID�б� */
		count = taGetTaskIdList(debugInfo->sessionID, threadIdList, MAX_TASKS_NUM);
    }

    /* �߳�ID�б�����������߳� */
	if (count == index || count == 0)
	{
		/* ����"l"��������� */
		outbuf[0] = 'l';
		index = 0;
		*outSize = 1;
	}
	else
	{
		/* ���ʣ���߳���Ϣ������"m<threadId>"��������� */
	#ifdef CONFIG_TA_LP64
	    *outSize = (UINT32)sprintf((INT8*)(outbuf), "m%lx",threadIdList[index++]);
	#else
		*outSize = (UINT32)sprintf((INT8*)(outbuf), "m%x",threadIdList[index++]);
	#endif
	}

    return (TA_OK);
}

/*
 * @brief:
 *      ����"T"����,�鿴�߳��Ƿ��ڼ���״̬
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode taTaskRspThreadIsActive(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	const UINT8 *ptr = &inbuf[1];
	ulong_t threadId = 0;

    /* ����ʽ"T<threadID>"�������� */

#ifdef CONFIG_TA_LP64
    if (!(0 != hex2int64(&ptr, &threadId)))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }
#else
	if (!(0 != hex2int(&ptr, &threadId)))
	{
		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
		return (TA_FAIL);
	}
#endif

    /* �߳��Ƿ��ڼ���״̬ */
	if(threadId !=0)
	{
		threadId = 0xffff800000000000+threadId;
	}

    if (taTaskIdVerify(threadId) != OK)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* ����"OK"��������� */
    *outSize = (UINT32)sprintf((INT8 *)outbuf, "OK");

	return (TA_OK);
}

/*
 * @brief:
 *      ����"autoAttachTask"������õ��ԻỰ���Զ��������־���������
 *      ΪTRUE���ɰ󶨵������񴴽��������Զ��󶨵����ԻỰ
 * @param[in]:tpInfo:���ԻỰ��Ϣ
 * @param[in]:bpInbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:bpOutbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *     TA_FAIL: ����ʧ��
 *     TA_OK: �����ɹ�
 */
T_TA_ReturnCode taTaskAutoAttachTask(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    const UINT8* ptr = NULL;
    UINT32 value = 0;

    /* ������������������� */
    ptr = &(inbuf[14]);

    /* ����ʽ������������ */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&value))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* ���������Զ��󶨱�־ */
    debugInfo->taskIsAutoAttach = value;

    /* ����"OK"��������� */
    *outSize = (UINT32)sprintf((INT8 *)outbuf, "OK");

	 return (TA_OK);
}
