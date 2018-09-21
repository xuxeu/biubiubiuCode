/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbStandardRsp.c
 * @brief:
 *             <li>GDB��RSPЭ��ʵ��</li>
 */

/************************ͷ �� ��*****************************/
#include <string.h>
#include "dbStandardRsp.h"
#include "taErrorDefine.h"
#include "dbAtom.h"
#include "dbBreakpoint.h"
#include "dbContext.h"
#include "sdaCpuLib.h"
#include "dbMemory.h"
#include "taUtil.h"
#include "taCommand.h"
#include "db.h"
#include "taDebugMode.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/* ����trap�쳣 */
extern void taTrap(void);

/* ͨ���߳�ID��ȡCPU ID */
extern UINT32 sdaGetCpuIdByThreadId(UINT32 threadId);

/* ��ʼ������ID */
extern int taInitTaskId;

#ifdef __MIPS__
/*�����ϵ�����ID*/
extern int stepThreadId;
#endif
/************************ǰ������******************************/

/************************ģ�����******************************/

/* ��ֵд�ڴ�ʱ0x7dת���Ļ��� */
static UINT8 m_Content_0x7d[TA_WRITE_M_MAX_DATA_SIZE * 2];

/* ��ͣ�����־ */
static BOOL m_StopCommandTag = FALSE;

/* ���߳���Ϣ�ظ���־ */
static BOOL m_MultiThreadInfoTag = TRUE;

/************************ȫ�ֱ���******************************/

/* NOTE:����GJB5369��׼,����������±�,������ӻ�ɾ�����������ע���޸����� */
T_DB_RspCommandTable taRSPCmdTable[TA_RSP_TABLE_LEN] =
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
    {"!",                RspBreakDebugProc,            TA_NEED_REPLY},
    {"c",                RspContinue,                  TA_NO_REPLY  },
    {"s",                RspStep,                      TA_NO_REPLY  },
    {"qC",    	         RspGetCurThreadID,            TA_NEED_REPLY},
    {"qfThreadInfo",     RspGetThreadIdList,           TA_NEED_REPLY},
    {"qsThreadInfo",     RspGetThreadIdList,           TA_NEED_REPLY},
    {"Hg",    	         RspSetThreadSubsequentOptHg,  TA_NEED_REPLY},
    {"T",    	         RspThreadIsActive,            TA_NEED_REPLY},
    {"qThreadExtraInfo", RspThreadExtraInfo,           TA_NEED_REPLY},
    {"iThread",    	     RspSetMultiThreadInfo,        TA_NEED_REPLY},
    {NULL,               NULL,                         TA_NO_REPLY  }
};

/* ��ѯ�쳣������ */
T_DB_ExceptionContext taGdbGetExpContext;

/************************ʵ   ��*******************************/

/*
 * @brief:
 *     �������Գ�ʼ��
 * @return:
 *     TRUE: ��ʼ���ɹ�
 *     FALSE: ��ʼ��ʧ��
 */
BOOL taDBInit(void)
{
    return (taDebugArchInit());
}

/*
 * @brief:
 *      �����Ƿ�����ͣ�����־
 * @return:
 *     ��
 */
void taDebugSetStopCommandTag(BOOL stopTag)
{
    /* ���õ�ǰ��ͣ��־ */
    m_StopCommandTag = stopTag;
}

/*
 * @brief:
 *      ��ȡ��ͣ�����־
 * @return:
 *     TRUE: ����ͣ����
 *     FALSE: ������ͣ����
 */
BOOL taDebugGetStopCommandTag(void)
{
    /* ���ص�ǰ��ͣ��־ */
    return (m_StopCommandTag);
}

/*
 * @brief:
 *     �����ʼ������
 * @return:
 *     ��
 */
void taDebugTaskActivate(T_DB_Info *debugInfo)
{
	/* ������ʼ������ */
	if (taInitTaskId != 0)
	{
		/* �����ʼ������ */
		if ((debugInfo->ctxType == TA_CTX_TASK_RTP) || (debugInfo->ctxType == TA_CTX_SYSTEM_RTP))
		{
			taTaskCont(taInitTaskId);
		}
		else
		{
			taTaskActivate(taInitTaskId);
		}
		taInitTaskId = 0;
	}
}

/*
 * @brief:
 *      �����ѯ��ǰ�����쳣���¼�
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���,��ʽΪ?
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 */
T_TA_ReturnCode RspQueryEvent(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    UINT32 signal = 0;

    /* ��vectorת��ΪPOSIX�ź� */
    signal = taDebugVector2Signal(debugInfo->context->vector, debugInfo->context);

    /* �����������Ϊ"SXX"��XXΪsignal��ʮ�����Ʊ�ʾ */
    outbuf[0] = 'S';
    outbuf[1] = taHexChars[(signal >> 4) & 0xF];
    outbuf[2] = taHexChars[signal % 16];
    outbuf[3] = '\0';

    /* ��������ַ������� */
    *outSize = strlen((const char *)outbuf);

    return (TA_OK);
}

/*
 * @brief:
 *      ����M����д�ڴ�
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���,������ʽΪaddress,size:xxxxxx
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:bpOutbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspWriteMemory(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    const UINT8* inPtr = NULL;
    UINT8 *outPtr = outbuf;
    UINT32 writeSize = 0;
    UINT32 writeAddr = 0;
    T_TA_ReturnCode ret = TA_FAIL;

    /* �������������е������� */
    inPtr = &(inbuf[1]);

    /* ����ʽ"address,size:xxxxxx"����������� */
    if ( !(( 0 != hex2int(&inPtr,&writeAddr))
        && ( *(inPtr++) == ',' )
        && ( 0 != hex2int(&inPtr, &writeSize))
        && ( *(inPtr++) == ':' ) ))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* д�ڴ��ַ��������ַǷ���ʮ�������� */
    if (-1 == hex2memEx(inPtr, outPtr, writeSize))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d", GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* ��ת������ڴ�����д��writeAddr��ַ�� */
    ret = taDebugWriteMemory(debugInfo->objID, writeAddr, (const UINT8 *)outPtr, writeSize, Align_None);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* ˢ��Cache */
    taDebugFlushCache(debugInfo->objID, writeAddr, writeSize);

    /* ����OK��������� */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *     ������ڴ��������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspReadMemory(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    const UINT8 *ptr = NULL;
    UINT32 readAddr = 0;
    UINT32 readSize = 0;
    T_TA_ReturnCode ret = TA_FAIL;

    /* �������������е������� */
    ptr = &(inbuf[1]);

    /* ����ʽ"m<addr>,<size>"����������� */
    if ( !(( 0 != hex2int(&ptr,&readAddr))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr,&readSize))
        && ( *ptr == '\0' )))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* ��ȡ�ĳ��ȴ���һ�ζ�ȡ�����ȡ����󳤶� */
    if (readSize > (TA_PKT_DATA_SIZE /2))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_READ_SIZE_ERROR);

        return (TA_FAIL);
    }

    /* ��ջ��� */
    ZeroMemory((void *)outbuf, (UINT32)(readSize*2));

    /* ���ڴ浽���������*/
    ret = taDebugReadMemory(debugInfo->objID, readAddr, outbuf, readSize, Align_None);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* ����ȡ���ڴ�����ת��Ϊʮ����ֵ�ַ�������������� */
    mem2hexEx(outbuf, readSize);

    /* ����������ַ������� */
    *outSize = readSize*DB_MEM_TO_CHAR_LEN;

    return (TA_OK);
}

/*
 * @brief:
 *      ����д��������ʽ�ڴ����GDB����0x23(ASCII #),0x24(ASCII $)����չ�ַ��ĸ�ʽΪ(0x7d ԭʼ�ַ����0x20),
 *      ���� 0x7d�������ֽ�(0x7d 0x5d)���͡�����Ŀ����ν���ʱ����н����������ǰ�ַ�Ϊ0x7d,���¸��ַ�Ϊ(*src^ 0x20)
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���,������ʽΪaddr,size:xxxx
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspWriteBinaryMemory(T_DB_Info *debugInfo,
                                            const UINT8 *inbuf,
                                            UINT32 inSize,
                                            UINT8 *outbuf,
                                            UINT32 *outSize)
{
    const UINT8* ptr = NULL;
    const UINT8* src = NULL;
    UINT8* des = NULL;
    UINT32 writeSize = 0;
    UINT32 writeAddr = 0;
    UINT32 cnt = 0;
    UINT8 tempValue = 0;
    T_TA_ReturnCode ret = TA_FAIL;

    /* ������������������� */
    ptr = &(inbuf[1]);

    /* ����ʽ"Xaddr,size:xxxx��"����������� */
    if ( !( ( 0 != hex2int(&ptr,&writeAddr))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr,&writeSize))
        && ( *(ptr++) == ':' )
        && ( writeSize <= sizeof(m_Content_0x7d))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* �������еĶ������ڴ�ֵ�еı�ת����ֽڻ�ԭΪԭֵ */
    src = ptr;
    des = m_Content_0x7d;
    ZeroMemory((void *)m_Content_0x7d, (UINT32)sizeof(m_Content_0x7d));
    cnt = writeSize;

    /*
     * ����ת���ַ��㷨����:
     * ������д���ڴ����ݵ������ֽ�, ��ֵΪ0x7d���ֽ�������ת��:
     * ��ֵΪ0x7d�ֽڵ���һ���ֽ�ֵ�� 0x20���ó�ת��ǰ��ֵ��
     */
    while (0 != (cnt--))
    {
        if (*src == 0x7d)
        {
            src++;
            tempValue = (*src) ^ ((UINT8)0x20);
            *des = tempValue;
            src++;
            des++;
        }
        else
        {
            *des = *src;
            src++;
            des++;
        }
    }

    /* ��ת������ڴ�����д��writeAddr�ڴ��ַ��ʼ�� */
    ret = taDebugWriteMemory(debugInfo->objID, writeAddr, m_Content_0x7d, writeSize, Align_None);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf,"E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL); 
    }

    /* ˢ��Cache */
    taDebugFlushCache(debugInfo->objID, writeAddr, writeSize);

    /* ����"OK"��������� */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      �����������ʽ���ڴ�����
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���,������ʽΪaddr,size
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspReadBinaryMemory(T_DB_Info *debugInfo,
                                           const UINT8 *inbuf,
                                           UINT32 inSize,
                                           UINT8 *outbuf,
                                           UINT32 *outSize)
{
    const UINT8* ptr = NULL;
    UINT32 readSize = 0;
    UINT32 readAddr = 0;
    T_TA_ReturnCode ret = TA_FAIL;
    UINT32 maxOutputSize = *outSize - (UINT32)(1);/* ��������С */

    /* ������������������� */
    ptr = &(inbuf[1]);

    /* ����ʽ"x<addr>,<size>"������������ */
    if ( !(( 0 != hex2int(&ptr,&readAddr))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr,&readSize))
        && ( *(ptr++) == '\0')
        && ( readSize > 0)
        && (readSize <= maxOutputSize)))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* ��ȡ�ڴ� */
    ret = taDebugReadMemory(debugInfo->objID, readAddr, outbuf, readSize,Align_None);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf,"E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* ����������ݴ�С */
    *outSize = readSize;

    return (TA_OK);
}

/*
 * @brief:
 *      ����д�����Ĵ�����������
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���,������ʽΪregNum=value
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspWriteSingleRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    UINT32 regNum = 0;
    T_TA_ReturnCode ret = TA_FAIL;
    const UINT8 *ptr = NULL;

    /* ������������������� */
    ptr = &inbuf[1];

    /* ��"P<regNum>=<value>"��ʽ����������� */
    if (!(( 0 != hex2int(&ptr,&regNum))
        && (*(ptr++) == '=' )))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* д�Ĵ���regNum */
    ret = taDebugWriteSingleRegister(debugInfo->context, regNum, ptr);
    if ( TA_OK != ret )
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* ����������,����RSPЭ�������ȡ,����ᵼ��IDE��ʾ�ļĴ���δ�޸� */
    memcpy((void *)&taGdbGetExpContext, debugInfo->context, sizeof(T_DB_ExceptionContext));

    /* �����������Ϊ "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      ����д���мĴ�����������
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspWriteAllRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    T_TA_ReturnCode ret = TA_FAIL;
    const UINT8 *ptr = NULL;

    /* ������������������� */
    ptr = &inbuf[1];

    /* д���мĴ��� */
    ret = taDebugWriteAllRegisters(debugInfo->context, ptr, strlen((const char *)ptr));
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* ����������,����RSPЭ�������ȡ,����ᵼ��IDE��ʾ�ļĴ���δ�޸� */
    memcpy((void *)&taGdbGetExpContext, debugInfo->context, sizeof(T_DB_ExceptionContext));

    /* ���� "OK"��������� */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      ��������мĴ�����������
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspReadAllRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    T_TA_ReturnCode ret = TA_FAIL;

    /* ��ȡ���мĴ�����ֵ�ŵ�������� */
    ret = taDebugReadAllRegisters(&taGdbGetExpContext, outbuf, outSize);
    if (TA_OK != ret)
    {
    	*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    return (TA_OK);
}

/*
 * @brief:
 *      ����������Ĵ�����������
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL: ִ��ʧ��
 */
T_TA_ReturnCode RspReadSingleRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    T_TA_ReturnCode ret = TA_FAIL;
    const UINT8* ptr = NULL;
    UINT32 regNum = 0;

    /* �������������е������� */
    ptr = &(inbuf[1]);

    /* ����ʽ"p<regnum>"������������ */
    if ( !(( 0 != hex2int(&ptr,&regNum))
        && (*ptr == '\0')))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* ��ȡָ���Ĵ�����ֵ����������� */
    ret = taDebugReadSingleRegister(&taGdbGetExpContext, regNum, outbuf, outSize);
    if (TA_OK != ret)
    {
    	*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    return (TA_OK);
}

/*
 * @brief:
 *      �������öϵ�����,��ָ����ַ�����öϵ�(���/Ӳ���ϵ�)
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���,���������ʽΪtype,address,size
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *     TA_OK��ִ�гɹ�
 *     TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspSetBreakpoint(T_DB_Info *debugInfo,
                                        const UINT8 *inbuf,
                                        UINT32 inSize,
                                        UINT8 *outbuf,
                                        UINT32 *outSize)
{
    UINT32 type = 0;
    UINT32 address = 0;
    UINT32 size = 0;
    T_TA_ReturnCode ret = TA_FAIL;
    const UINT8 *ptr = NULL;
    T_DB_Breakpoint breakPoint;

    /* ��նϵ���Ϣ */
    ZeroMemory((void *)(&breakPoint), sizeof(breakPoint));

    /* �������������е������� */
    ptr = &inbuf[1];

    /* ����ʽ"Z<type>,<addr>,<size>"�������� */
    if ( !( ( 0 != hex2int(&ptr,&type))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr, &address))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr, &size))
        && ( (*ptr == '\0')  || (*ptr == ',')  || (*ptr == ' ') )))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* ���öϵ���Ϣ */
	breakPoint.logicAddr = address;
	breakPoint.bpType= type;
	breakPoint.objID = debugInfo->objID;
	breakPoint.sessionID = debugInfo->sessionID;
	breakPoint.bpClass = BPClass_NORMAL;
	breakPoint.breakSize = size;
#ifdef __MIPS__
	/*���Ϊ��������ϵ��򱣴浱ǰ������ID*/
	if(breakPoint.bpType == DB_BP_SOFT_STEP)
	{
		stepThreadId = taTaskIdSelf();
	}
	else
	{
		stepThreadId = 0;
	}
#endif
    /* ����ϵ� */
    ret = taDebugInsertBP(debugInfo, &breakPoint);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* ����"OK"���������  */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      ����ɾ���ϵ�������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���,������ʽΪtype,address,size
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspRemoveBreakpoint(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    UINT32 type = 0;
    UINT32 address = 0;
    UINT32 size = 0;
    T_TA_ReturnCode ret = TA_FAIL;
    const UINT8 *ptr = NULL;

    /* �������������е������� */
    ptr = &inbuf[1];

    /* ����ʽ"z<type>,<address>,<size>"�������� */
    if ( !( ( 0 != hex2int(&ptr, &type))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr, &address) )
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr, &size))
        && ( *ptr == '\0' )))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* ɾ���ϵ� */
    ret = taDebugRemoveBP(debugInfo, address, type, BPClass_NORMAL, size);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* ����OK�����buffer */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      ���������ͣ����
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *     TA_DB_NEED_IRET:ִ�гɹ�
 */
T_TA_ReturnCode RspBreakDebugProc(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	/* �����ǰ�����쳣״̬����ִ��trap���� */
	if (!TA_DEBUG_IS_CONTINUE_STATUS(debugInfo))
	{
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
	}

	/* ������ͣ�����־ΪTRUE */
	taDebugSetStopCommandTag(TRUE);

	/* trap�����쳣 */
	taTrap();

    return (TA_OK);
}

/*
 * @brief:
 *    ����������в�������,���ص������Գ����������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *     TA_DB_NEED_IRET: ִ�гɹ�
 */
T_TA_ReturnCode RspContinue(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    /* �������λ */
    taDebugContinue(debugInfo->context);

    /* �����ʼ������ */
    taDebugTaskActivate(debugInfo);

    /* ��Ҫ���쳣�˳� */
    return (TA_DB_NEED_IRET);
}

/*
 * @brief:
 *      ��������������,��λ��־�Ĵ����ĵ�����־��ʹ��������ʱ���������쳣
 * @param[in]:tpInfo:���ԻỰ��Ϣ
 * @param[in]:bpInbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]: bpOutbuf:�洢������������ݻ���
 * @param[out]: outSize:�洢����������ݳ���
 * @return:
 *     TA_DB_NEED_IRET: ��Ҫ���ж��쳣����
 */
T_TA_ReturnCode RspStep(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
#ifdef __PPC85XX__
       /* ������������λ��־�Ĵ����ĵ�����־λ */
	int ret =  taDebugStep(debugInfo->context);
	if(ret != TA_OK)
	{
		return (TA_FAIL);
	}
#else
       /* ������������λ��־�Ĵ����ĵ�����־λ */
	taDebugStep(debugInfo->context);
#endif

    /* �����ʼ������ */
	taDebugTaskActivate(debugInfo);

    /* ��Ҫ���쳣�˳� */
    return (TA_DB_NEED_IRET);
}

/*
 * @brief:
 *      ����"qC"����,��ȡ��ǰ�߳�ID
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspGetCurThreadID(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	int threadId = 0;

	/* ��ȡ��ǰ�����߳�ID */
	threadId = TA_GET_CUR_THREAD_ID(debugInfo);
	if (0 == threadId)
	{
	    /* ����"l"��������� */
	    *outSize = (UINT32)sprintf((INT8*)(outbuf), "l");
	    return (TA_FAIL);
	}

    /* ����"QCthreadId"��������� */
    *outSize = (UINT32)sprintf((INT8*)(outbuf), "QC%x",threadId);

	return (TA_OK);
}

/*
 * @brief:
 *      ����"qfThreadInfo"��"qsThreadInfo"����,��ȡ�����߳���Ϣ�����������ȷ���qfThreadInfo�����ȡ��һ���߳���Ϣ
 *      Ȼ����qsThreadInfo�����ȡʣ�µ��߳���Ϣ����Ŀ����˻ظ�'l'��ʾ�߳���Ϣ�ظ����
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspGetThreadIdList(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    static INT32 threadIdList[MAX_TASKS_NUM] = {0};
    static UINT32 count = 0;
    static UINT32 index = 0;
	
    /* �����qfThreadInfo�����ʾ��ȡ��һ���߳�ID��ͨ��taskIdListGet��ȡ�߳�ID�б�  */
    if (!strncmp((const char *)inbuf, "qfThreadInfo", strlen("qfThreadInfo")))
    {
    	if (m_MultiThreadInfoTag == FALSE)
    	{
    		if(TA_GET_CUR_THREAD_ID(debugInfo) != 0)
    		{
    			count = 1;
    			threadIdList[0] = TA_GET_CUR_THREAD_ID(debugInfo);
    		}
    	}
    	else
    	{
			/* ��ȡ�����߳�ID�б� */
			count = taTaskIdListGet(threadIdList,MAX_TASKS_NUM);
    	}
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
	    *outSize = (UINT32)sprintf((INT8*)(outbuf), "m%x",threadIdList[index++]);
	}

    return (TA_OK);
}

/*
 * @brief:
 *      ����"Hg"����,����GDBҪ���̺߳����Ĳ���,Hg��ʾ����������Ҫ����g�����ȡ���мĴ���
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspSetThreadSubsequentOptHg(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	const UINT8 *ptr = &inbuf[2];
    UINT32 threadId = 0;
    UINT32 cpuIndex = 0;
	T_DB_ExceptionContext *context = &taGdbGetExpContext;

    /* ����ʽ"Hg<threadID>"�������� */
    if (!(0 != hex2int(&ptr, &threadId)))
    {
        ptr = &inbuf[2];
    	if('-' == *ptr)
	    {
	  	    ptr++;
		    hex2int(&ptr, &threadId);
		    threadId = 0-threadId;
	    }
    	else
    	{
            *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
            return (TA_FAIL);
    	}
    }

    /* ����߳�IDΪ0����ȡ��ǰ�̼߳Ĵ�����Ϣ */
    if (threadId == 0)
    {
		/* ��ȡ��ǰ�����߳�ID */
		threadId = taTaskIdSelf();
    }

    /* �߳��Ƿ��ڼ���״̬ */
    if (taTaskIdVerify(threadId) != OK)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

	/* ����ǻ�ȡ��ǰ�����쳣������Ĵ����������쳣�����ĵ�taGdbGetExpContext�� */
    if (((cpuIndex = sdaGetCpuIdByThreadId(threadId)) != 0xffffffff) && (DEBUGMODE_SYSTEM == taDebugModeGet()))
	{
        memcpy((void *)context, (void *)taSDAExpContextPointerGet(cpuIndex), sizeof(T_DB_ExceptionContext));
	}
    else if ((TA_GET_CUR_THREAD_ID(debugInfo) == threadId) && (DEBUGMODE_TASK == taDebugModeGet()))
    {
    	memcpy((void *)context, debugInfo->context, sizeof(T_DB_ExceptionContext));
    }
	else
	{
		/* ��������ͨ������ϵͳ�Ľӿڻ�ȡ�����л�ʱ�������� */
		if (OK != taTaskContextGet(threadId, context))
		{
    		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
    		return (TA_OK);
		}
	}

    /* ����"OK"��������� */
    *outSize = (UINT32)sprintf((INT8 *)outbuf, "OK");

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
T_TA_ReturnCode RspThreadIsActive(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	const UINT8 *ptr = &inbuf[1];
	UINT32 threadId = 0;

    /* ����ʽ"T<threadID>"�������� */
    if (!(0 != hex2int(&ptr, &threadId)))
    {
        ptr = &inbuf[1];
    	if('-' == *ptr)
	    {
	 	    ptr++;
	 	    hex2int(&ptr, &threadId);
	 	    threadId = 0-threadId;
	    }
    	else
    	{
            *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
            return (TA_FAIL);
    	}
    }



    /* �����ʾ�����߳���Ϣ����ѯ�����̻߳ظ��߳�δ���� */
    if (m_MultiThreadInfoTag == FALSE)
    {
    	if (threadId != TA_GET_CUR_THREAD_ID(debugInfo))
    	{
    		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
    		return (TA_OK);
    	}
    }

    /* �߳��Ƿ��ڼ���״̬ */
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
 *      ����"qThreadExtraInfo,tid"����,��ȡ�̶߳�����Ϣ,������Ϣ�����߳�ID,�߳����ͺ���Ϣ
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspThreadExtraInfo(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	const UINT8 *ptr = &inbuf[17];
	UINT32 cpuAffinity = 0;
	UINT32 threadId = 0;

    /* ����ʽ"qThreadExtraInfo,threadID"�������� */
    if (!(0 != hex2int(&ptr, &threadId)))
    {
        ptr = &inbuf[17];
    	if('-' == *ptr)
	    {
	 	    ptr++;
	 	    hex2int(&ptr, &threadId);
	 	    threadId = 0-threadId;
	    }
    	else
    	{
    		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        	return (TA_FAIL);
    	}
    }

	/* ��ȡCPU�׺��� */
	cpuAffinity = sdaGetCpuIdByThreadId(threadId);

    /* ����"OK"��������� */
    *outSize = (UINT32)sprintf((INT8 *)outbuf, "%x,%s,%x",threadId,taTaskName(threadId),cpuAffinity);

    /* ת��Ϊʮ�������ַ��� */
    mem2hexEx(outbuf, *outSize);

    /* ����������ַ������� */
    *outSize = (*outSize)*DB_MEM_TO_CHAR_LEN;

	return (TA_OK);
}

/*
 * @brief:
 *      ���ö��߳���Ϣ��������̱߳�־ΪTRUE�ظ����߳���Ϣ������ֻ�ظ���ǰ�����쳣���߳���Ϣ
 * @param[in]:tpInfo:���ԻỰ��Ϣ
 * @param[in]:bpInbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:bpOutbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *     TA_FAIL: ����ʧ��
 *     TA_OK: �����ɹ�
 */
T_TA_ReturnCode RspSetMultiThreadInfo(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    const UINT8* ptr = NULL;
    UINT32 value = 0;

    /* ������������������� */
    ptr = &(inbuf[7]);

    /* ����ʽ������������ */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&value))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* ���ö��߳���Ϣ��־ */
    m_MultiThreadInfoTag = value;

    /* ����"OK"��������� */
    *outSize = (UINT32)sprintf((INT8 *)outbuf, "OK");

	 return (TA_OK);
}

/**********************�����������ͬ�����̱߳�־*****************************/
#ifdef _KERNEL_DEBUG_
/*
 * @brief:
 *      ��ȡ���̱߳�־
 * @return:
 *     ���̱߳�־
 */
BOOL taGetMultiThreadInfoTag()
{
	return m_MultiThreadInfoTag;
}

#else

/*
 * @brief:
 *      ���ö��̱߳�־
 * @param[in]:tag: ���̱߳�־
 * @return:
 *     ��
 */
void taSetMultiThreadInfoTag(BOOL tag)
{
    m_MultiThreadInfoTag = tag;
}
#endif


