/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-08-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:rtpMemLib.c
 * @brief:
 *             <li>rtp �ڴ���ز���</li>
 */

/************************ͷ �� ��*****************************/
#include "taRtp.h"
#include "taRtIfLib.h"
#include "taskSessionLib.h"
#include "ta.h"

/************************�� �� ��******************************/

//UINT32 physicalAddr = 0; /*�ô�����Ϊȫ�ֱ���������ں����в��þֲ�������vmTranslate�����ᱨ�쳣����оƽ̨*/

#define TA_ROUND_DOWN(x, align)	((int)(x) & ~(align - 1))

#define TA_ROUND_UP(x, align)  (((int) (x) + (align - 1)) & ~(align - 1))

/* �ڴ��ַ�Ƿ���Ч */
#define TA_MEM_ADDR_IS_INVALID(virtualAddr,physicalAddr) \
    if (vmTranslate(NULL,(virtualAddr), &(physicalAddr)) != OK)\
    {\
	    return (TA_INVALID_CONTEXT);\
    }

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/
    UINT32 physicalAddr = 0;
/************************ʵ   ��********************************/

/*
 * @brief:
 *     �л��ڴ�������,����Ƿ���ʵʱ���̿ռ䣬��Ҫ�л���ʵʱ���̵Ŀռ�������
 * @param[in]: newCtx: ������ID
 * @return:
 *     NULL:�л�������ʧ��
 *     oldCtx:�л�ǰ��������ID
 */
UINT32 taMemCtxSwitch(UINT32 newCtx)
{
    if (newCtx == 0)
    {
    	newCtx = (UINT32)kernelId;
    }

    /* �Ƿ�֧���ڴ��л� */
    if (TA_MEM_SWITCH_IS_SUPPORED())
    {
    	return (UINT32)kernelId;
    }

    return (TA_RT_MEM_SWITCH(newCtx,1));
}

/*
 * @brief:
 *      ˢ��RTPָ����ַ��Cache��
 * @param[in]: ctxId: ������ID
 * @param[in]: address: �ڴ��ַ
 * @param[in]: size: ��ַ����
 * @return:
 *     ��
 */
static void taRtpFlushCache(UINT32 ctxId, UINT32 address, UINT32 size)
{
    UINT32	savedCtx = 0;

    /* �л���RTP�ڴ������� */
    if ((savedCtx = taMemCtxSwitch(ctxId)) == 0)
    {
    	return;
    }

    /* ˢ��Cache */
    taCacheTextUpdata(address, size);

	/* �ָ��ڴ������� */
    taMemCtxSwitch(savedCtx);
}

/*
 * @brief:
 *     ���ڴ�
 * @param[in]: ctxId: ������ID
 * @param[in]: readAddr: ��ȡ�ڴ���ʼ��ַ
 * @param[out]:outBuf: �����ַ
 * @param[in]: nBytes: ��ȡ���ݴ�С
 * @param[in]: align: �ڴ���뷽ʽ
 * @return:
 *      TA_OK:���ڴ�ɹ�
 *      TA_INVALID_CONTEXT: ��Ч������
 *      TA_FAIL: ���ڴ�ʧ��
 */
static UINT32 taRtpReadMemory(UINT32 ctxId, UINT32 readAddr, UINT8 *outBuf, UINT32 nBytes, T_DB_AccessMemoryAlign align)
{
    UINT32 savedCtx = 0;
    UINT32 ret = TA_OK;
    //UINT32 physicalAddr = 0;

    /* �л���RTP�ڴ������� */
    if ((savedCtx = taMemCtxSwitch(ctxId)) == 0)
    {
    	return (TA_INVALID_CONTEXT);
    }

    /* �ڴ��ַ�Ƿ���Ч */
    TA_MEM_ADDR_IS_INVALID(readAddr,physicalAddr);

    /* ���ڴ� */
    if ((ret = TA_RT_MEM_READ(ctxId, readAddr, outBuf, nBytes, align)) != TA_OK)
    {
    	return (TA_FAIL);
    }

	/* �ָ��ڴ������� */
    taMemCtxSwitch(savedCtx);

	return (TA_OK);
}

/*
 * @brief:
 *     д�ڴ�
 * @param[in]: ctxId: ������ID
 * @param[in]: dest: Ŀ���ڴ��ַ
 * @param[in]: src: Դ�ڴ��ַ
 * @param[in]: nBytes: ���ݴ�С
 * @param[in]: align: �ڴ���뷽ʽ
 * @return:
 *      TA_OK:д�ڴ�ɹ�
 *      TA_INVALID_CONTEXT: ��Ч������
 *      TA_FAIL: д�ڴ�ʧ��
 */
static UINT32 taRtpWriteMemory(UINT32 ctxId, UINT32 dest, const UINT8 *src, UINT32 nBytes, T_DB_AccessMemoryAlign align)
{
    UINT32 savedCtx = 0;
    UINT32 ret = TA_OK;
    //UINT64 physicalAddr = 0;
    UINT32 pageSize = TA_RT_GET_PAGE_SIZE();
    UINT32 addr = 0;
    UINT32 size = 0;
    UINT32 state = 0;


    /* �л���RTP�ڴ������� */
    if ((savedCtx = taMemCtxSwitch(ctxId)) == 0)
    {
    	return (TA_INVALID_CONTEXT);
    }

    /* �ڴ��ַ�Ƿ���Ч */
    TA_MEM_ADDR_IS_INVALID(dest,physicalAddr);


    /* MMU������鿴�ڴ��Ƿ��д */
    if (pageSize != 0)
    {
    	/* ��ַҳ���� */
    	addr = TA_ROUND_DOWN (dest, pageSize);

    	/* ��Сҳ���� */
    	size = TA_ROUND_UP(nBytes, pageSize);

    	/* ��ȡ��ǰ�ڴ��д״̬  */
    	VM_STATE_GET(NULL, (void *)addr, &state);

	    if ((state & VM_STATE_MASK_WRITABLE) != VM_STATE_WRITABLE)
	    {
	    	VM_STATE_SET(NULL, addr, size, VM_STATE_MASK_WRITABLE, VM_STATE_WRITABLE);
	    }
    }

    /* д�ڴ� */
    if ((ret = TA_RT_MEM_WRITE(ctxId, dest, src, nBytes, align)) != TA_OK)
    {
    	return (TA_FAIL);
    }

	/* ����Ϊԭ�����ڴ��д״̬ */
    if (pageSize != 0)
    {
    	if ((state & VM_STATE_MASK_WRITABLE) != VM_STATE_WRITABLE)
    	{
    		VM_STATE_SET (NULL, addr, size, VM_STATE_MASK_WRITABLE, state);
    	}
    }

	/* �ָ��ڴ������� */
    taMemCtxSwitch(savedCtx);

	return (TA_OK);
}

/*
 * @brief:
 *     RTP�����ڴ�����ӿڳ�ʼ��
 * @return:
 *     TA_OK:   �����ɹ�
 */
T_TA_ReturnCode taRtpMemoryInit(void)
{
    T_DB_MemoryOper taRtpMemoryOperTable;

    /* �����ڴ�����ӿ� */
    taRtpMemoryOperTable.FlushCache = taRtpFlushCache;
    taRtpMemoryOperTable.ReadMemory = taRtpReadMemory;
    taRtpMemoryOperTable.WriteMemory = taRtpWriteMemory;

    /* ��װRTP���Ե��ڴ��д�ӿ� ����Cacheˢ�½ӿ�, ��������ֵ */
    return taDebugMemoryInit(&taRtpMemoryOperTable);
}
