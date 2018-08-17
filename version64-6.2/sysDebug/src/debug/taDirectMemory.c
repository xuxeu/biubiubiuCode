/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
* 	 Copyright (C)  2013 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/*
 * �޸���ʷ:
 * 2013-06-26   ��Ԫ־�������������ɼ������޹�˾
 *                  �������ļ���
*/

/*
* @file: taDirectMemory.c
* @brief:
*	    <li>����:ϵͳ��ֱ�Ӷ�д�ڴ�Ľӿ�</li>
*/

/************************ͷ �� ��******************************/
#include "dbMemory.h"
#include "taMemProbeLib.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/* DB�ṹ�����ڴ��ݶ�д�ڴ��ˢcache���� */
static T_DB_MemoryOper m_OperTable;

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/
/*
 * @brief:
 *      ��1�ֽ�ֱ��д�ڴ�
 * @param[in]:ctxId: ������ID
 * @param[in]:src: Դ�ڴ��ַ
 * @param[in]:size: ���ݴ�С
 * @param[out]:dest: Ŀ���ڴ��ַ
 * @return:
 *      ��
 */
static void taDirect8BitAccessMemory(ulong_t ctxId, ulong_t dest, ulong_t src, UINT32 size)
{
	UINT8* desAddr = (UINT8*)dest;
	UINT8* srcAddr = (UINT8*)src;
	
	/* ѭ��ִ��д���ڲ���,ѭ�� size��*/
    while (size != 0)
    {
        /*���ֽڴ�srcAddr�����ݲ�д��desAddr��*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        size = size - 1;
    }
}

/*
 * @brief:
 *      ��2�ֽ�ֱ��д�ڴ�
 * @param[in]:ctxId: ������ID
 * @param[in]:src: Դ�ڴ��ַ
 * @param[in]:size: ���ݴ�С
 * @param[out]:dest: Ŀ���ڴ��ַ
 * @return:
 *      ��
 */
static void taDirect16BitAccessMemory(ulong_t ctxId, ulong_t dest, ulong_t src, UINT32 size)
{
	UINT16* desAddr = (UINT16*)dest;
	UINT16* srcAddr = (UINT16*)src;
	
	/* ѭ��ִ��д���ڲ���,ѭ�� size��*/
    while (size != 0)
    {
        /*���ֽڴ�srcAddr�����ݲ�д��desAddr��*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        size = size - 2;
    }
}

/*
 * @brief:
 *      ��4�ֽ�ֱ��д�ڴ�
 * @param[in]:ctxId: ������ID
 * @param[in]:src: Դ�ڴ��ַ
 * @param[in]:size: ���ݴ�С
 * @param[out]:dest: Ŀ���ڴ��ַ
 * @return:
 *      ��
 */
static void taDirect32BitAccessMemory(ulong_t ctxId, ulong_t dest, ulong_t src, UINT32 size)
{
	UINT32* desAddr = (UINT32*)dest;
	UINT32* srcAddr = (UINT32*)src;
	
	/* ѭ��ִ��д���ڲ���,ѭ�� size��*/
    while (size != 0)
    {
        /*���ֽڴ�srcAddr�����ݲ�д��desAddr��*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        size = size - 4;
    }
}

/*
 * @brief:
 *      ��8�ֽ�ֱ��д�ڴ�
 * @param[in]:ctxId: ������ID
 * @param[in]:src: Դ�ڴ��ַ
 * @param[in]:size: ���ݴ�С
 * @param[out]:dest: Ŀ���ڴ��ַ
 * @return:
 *      ��
 */
static void taDirect64BitAccessMemory(ulong_t ctxId, ulong_t dest, ulong_t src, UINT32 size)
{
	UINT64* desAddr = (UINT64*)dest;
	UINT64* srcAddr = (UINT64*)src;
	
	/* ѭ��ִ��д���ڲ���,ѭ�� size��*/
    while (size != 0)
    {
        /*���ֽڴ�srcAddr�����ݲ�д��desAddr��*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        size = size - 8;
    }
}

/*
 * @brief:
 *      ֱ��д�ڴ�
 * @param[in]:ctxId: ������ID
 * @param[in]:src: Դ�ڴ��ַ
 * @param[in]:size: ���ݴ�С
 * @param[in]:align: �����С
 * @param[out]:dest: Ŀ���ڴ��ַ
 * @return:
 *      TA_OK:�����ɹ�
 *      TA_INVALID_PARAM:�Ƿ��Ĳ���
 */
 
T_TA_ReturnCode taDirectWriteMemory(ulong_t ctxId, ulong_t dest, const UINT8 *src, UINT32 size,T_DB_AccessMemoryAlign align)
{
    /* �п� */
    if ((NULL == src) || (0 == size))
    {
        return (TA_INVALID_PARAM);
    }

    /* �����ڴ��Ƿ�ɷ��� */
    if (TA_OK != taMemTest((char*)dest, size, TA_WRITE, align))
    {

    	return (TA_INVALID_PARAM);
    }


	switch (align)
	{
		case Align_8Bit:
			taDirect8BitAccessMemory(ctxId,dest,(ulong_t)src,size);
			break;
		case Align_16Bit:
			{
				if ((size % 2) != 0)
				{
					return (TA_INVALID_PARAM);
				}

				taDirect16BitAccessMemory(ctxId,dest,(ulong_t)src,size);
			}
			break;
		case Align_32Bit:
			{
				if ((size % 4) != 0)
				{
					return (TA_INVALID_PARAM);
				}

				taDirect32BitAccessMemory(ctxId,dest,(ulong_t)src,size);
			}
			break;
		case Align_64Bit:
			{
				if ((size % 8) != 0)
				{
					return (TA_INVALID_PARAM);
				}

				taDirect64BitAccessMemory(ctxId,dest,(ulong_t)src,size);
			}
			break;
		default:
			taDirect8BitAccessMemory(ctxId,dest,(ulong_t)src,size);
			break;
	}
    return TA_OK;
}

/*
 * @brief:
 *      ֱ�Ӷ��ڴ�
 * @param[in]:ctxId: ������ID
 * @param[in]:readAddr:�ڴ��ַ
 * @param[out]:outBuf: ���ڴ滺��
 * @param[in]:size: Ҫ�����ڴ��С
 * @param[in]:align: �����С
 * @return:
 *      TA_OK:�����ɹ�
 *      TA_INVALID_PARAM:�Ƿ��Ĳ���
 */
UINT32 taDirectReadMemory(ulong_t ctxId, ulong_t readAddr, UINT8 *outBuf, UINT32 size,T_DB_AccessMemoryAlign align)
{
    /* �п� */
    if ((NULL == outBuf) || (0 == size))
    {
        return (TA_INVALID_PARAM);
    }

    /* �����ڴ��Ƿ�ɷ��� */
    if (TA_OK != taMemTest((char*)readAddr, size, TA_READ, align))
    {
    	return (TA_INVALID_PARAM);
    }

	switch (align)
	{
		case Align_8Bit:
			taDirect8BitAccessMemory(ctxId,(ulong_t)outBuf,readAddr,size);
			break;
		case Align_16Bit:
			{
				if ((size % 2) != 0)
				{
					return (TA_INVALID_PARAM);
				}
				
				taDirect16BitAccessMemory(ctxId,(ulong_t)outBuf,readAddr,size);
			}
			break;
		case Align_32Bit:
			{
				if ((size % 4) != 0)
				{
					return (TA_INVALID_PARAM);
				}
				
				taDirect32BitAccessMemory(ctxId,(ulong_t)outBuf,readAddr,size);
			}
			
			break;
		case Align_64Bit:
			{
				if ((size % 8) != 0)
				{
					return (TA_INVALID_PARAM);
				}
				
				taDirect64BitAccessMemory(ctxId,(ulong_t)outBuf,readAddr,size);
			}
			
			break;
		default:
			taDirect8BitAccessMemory(ctxId,(ulong_t)outBuf,readAddr,size);
			break;
	}
	
    return TA_OK;
}

/*
 * @brief:
 *      ˢ������Cache
 * @param[in]:ctxId: ������ID
 * @param[in]:addr: Ҫ���µ�cache�ڴ��ַ
 * @param[in]:len: Ҫ���µĳ���
 * @return:
 *      ��
 */
void taDirectFlushCache(ulong_t ctxId, ulong_t addr, UINT32 len)
{
    /* ˢ��Cache */
	taCacheTextUpdata(addr, len);
}

/**
 * @brief
 * 		TAֱ���ڴ���ʳ�ʼ��
 * @return:
 *      ��
 */
void taDebugInitDirectMemory(void)
{
    /* ��ʼ���ڴ������ */
    m_OperTable.FlushCache = taDirectFlushCache;
    m_OperTable.ReadMemory = taDirectReadMemory;
    m_OperTable.WriteMemory = taDirectWriteMemory;

    taDebugMemoryInit(&m_OperTable);
}
