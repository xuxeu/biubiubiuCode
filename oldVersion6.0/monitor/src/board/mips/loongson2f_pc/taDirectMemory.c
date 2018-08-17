/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
* 	 Copyright (C)  2011 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/*
 * �޸���ʷ:
 * 2011-9-8 ��ѩ�ף������������ɼ������޹�˾
 *          �����ļ�        
*/

/*
* @file: taDirectMemory.c
* @brief:
*	    <li>����:��ϵͳ��ֱ�Ӷ�д�ڴ�Ľӿ�</li>
* @implements: DTA
*/

/************************ͷ �� ��******************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "dbMemory.h"
#include "monitorBSP.h"
/************************�� �� ��******************************/
/************************���Ͷ���******************************/
/************************�ⲿ����******************************/
T_EXTERN T_VOID  kbspRawCacheInstructUpdate(UINT32 addr,UINT32 len);

/************************ǰ������******************************/
/************************ģ�����******************************/
/************************ȫ�ֱ���******************************/
/************************ʵ   ��*******************************/
/*
 * @brief:
 *      ��1�ֽ�ֱ��д�ڴ�
 * @param[in]:    sessionID:�ỰID
 * @param[in]:    ubpSrcAddr: Դ�ڴ��ַ
 * @param[in]:    uwSize:    ���ݴ�С
 * @param[out]:    ubpDesAddr: Ŀ���ڴ��ַ
 * @return:
 *      ��
 */
T_MODULE T_VOID Direct8BitAccessMemory(UINT32 sessionID, UINT32 ubpDesAddr, UINT32 ubpSrcAddr, UINT32 uwSize)
{
	UINT8* desAddr = (UINT8*)ubpDesAddr;
	UINT8* srcAddr = (UINT8*)ubpSrcAddr;
	
	/* ѭ��ִ��д���ڲ���,ѭ�� uwSize��*/
    while(uwSize != 0)
    {
        /*���ֽڴ�srcAddr�����ݲ�д��desAddr��*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        uwSize = uwSize - 1;
    }
}

/*
 * @brief:
 *      ��2�ֽ�ֱ��д�ڴ�
 * @param[in]:    sessionID:�ỰID
 * @param[in]:    ubpSrcAddr: Դ�ڴ��ַ
 * @param[in]:    uwSize:    ���ݴ�С
 * @param[out]:    ubpDesAddr: Ŀ���ڴ��ַ
 * @return:
 *      ��
 */
T_MODULE T_VOID Direct16BitAccessMemory(UINT32 sessionID, UINT32 ubpDesAddr, UINT32 ubpSrcAddr, UINT32 uwSize)
{
	UINT16* desAddr = (UINT16*)ubpDesAddr;
	UINT16* srcAddr = (UINT16*)ubpSrcAddr;
	
	/* ѭ��ִ��д���ڲ���,ѭ�� uwSize��*/
    while(uwSize != 0)
    {
        /*���ֽڴ�srcAddr�����ݲ�д��desAddr��*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        uwSize = uwSize - 2;
    }
}

/*
 * @brief:
 *      ��4�ֽ�ֱ��д�ڴ�
 * @param[in]:    sessionID:�ỰID
 * @param[in]:    ubpSrcAddr: Դ�ڴ��ַ
 * @param[in]:    uwSize:    ���ݴ�С
 * @param[out]:    ubpDesAddr: Ŀ���ڴ��ַ
 * @return:
 *      uwSize
 */
T_MODULE T_VOID Direct32BitAccessMemory(UINT32 sessionID, UINT32 ubpDesAddr, UINT32 ubpSrcAddr, UINT32 uwSize)
{
	UINT32* desAddr = (UINT32*)ubpDesAddr;
	UINT32* srcAddr = (UINT32*)ubpSrcAddr;
	
	/* ѭ��ִ��д���ڲ���,ѭ�� uwSize��*/
    while(uwSize != 0)
    {
        /*���ֽڴ�srcAddr�����ݲ�д��desAddr��*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        uwSize = uwSize - 4;
    }
}

/*
 * @brief:
 *      ��8�ֽ�ֱ��д�ڴ�
 * @param[in]:    sessionID:�ỰID
 * @param[in]:    ubpSrcAddr: Դ�ڴ��ַ
 * @param[in]:    uwSize:    ���ݴ�С
 * @param[out]:    ubpDesAddr: Ŀ���ڴ��ַ
 * @return:
 *      uwSize
 */
T_MODULE T_VOID Direct64BitAccessMemory(UINT32 sessionID, UINT32 ubpDesAddr, UINT32 ubpSrcAddr, UINT32 uwSize)
{
	UINT64* desAddr = (UINT64*)ubpDesAddr;
	UINT64* srcAddr = (UINT64*)ubpSrcAddr;
	
	/* ѭ��ִ��д���ڲ���,ѭ�� uwSize��*/
    while(uwSize != 0)
    {
        /*���ֽڴ�srcAddr�����ݲ�д��desAddr��*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        uwSize = uwSize - 8;
    }
}

/*
 * @brief:
 *      ֱ��д�ڴ�
 * @param[in]:    sessionID:�ỰID
 * @param[in]:    ubpSrcAddr: Դ�ڴ��ַ
 * @param[in]:    uwSize:    ���ݴ�С
 * @param[in]:    align:    �����С
 * @param[out]:    ubpDesAddr: Ŀ���ڴ��ַ
 * @return:
 *      DB_OK:    �����ɹ�
 *      DB_INVALID_PARAM:�Ƿ��Ĳ���
 */
T_TA_ReturnCode DirectWriteMemory(UINT32 sessionID, UINT32 ubpDesAddr, T_CONST UINT8 *ubpSrcAddr, UINT32 uwSize,DB_AccessMemoryAlign align)
{
    /*�п�*/
    if ((NULL == ubpSrcAddr) || (0 == uwSize) )
    {
        return (TA_INVALID_PARAM);
    }
    
  	if (((T_UWORD)0xFFFFFFFF - (T_UWORD)ubpDesAddr) < (T_UWORD)uwSize ||
            		(T_UWORD)ubpDesAddr < (T_UWORD)0x80000000)
	{
		return (TA_INVALID_PARAM);
	}
	
	switch(align)
	{
		case Align_8Bit:
			Direct8BitAccessMemory(sessionID,ubpDesAddr,(UINT32)ubpSrcAddr,uwSize);
			break;
		case Align_16Bit:
			{
				if ((uwSize % 2) != 0)
				{
					return (TA_INVALID_PARAM);
				}
				
				Direct16BitAccessMemory(sessionID,ubpDesAddr,(UINT32)ubpSrcAddr,uwSize);
			}
			break;
		case Align_32Bit:
			{
				if ((uwSize % 4) != 0)
				{
					return (TA_INVALID_PARAM);
				}
				
				Direct32BitAccessMemory(sessionID,ubpDesAddr,(UINT32)ubpSrcAddr,uwSize);
			}
			
			break;
		case Align_64Bit:
			{
				if ((uwSize % 8) != 0)
				{
					return (TA_INVALID_PARAM);
				}
				
				Direct64BitAccessMemory(sessionID,ubpDesAddr,(UINT32)ubpSrcAddr,uwSize);
			}
			
			break;
		default:
			Direct8BitAccessMemory(sessionID,ubpDesAddr,(UINT32)ubpSrcAddr,uwSize);
			break;
	}
		
    return TA_OK;
}

/*
 * @brief:
 *      ֱ�Ӷ��ڴ�
 * @param[in]:    sessionID:�ỰID
 * @param[in]:    ubpReadAddr:�ڴ��ַ
 * @param[in]:    align:    �����С
 * @param[in]:    uwSize:   Ҫ�����ڴ��С
 * @param[out]:    ubpOutBuf: ���ڴ滺��
 * @return:
 *      TA_OK:    �����ɹ�
 *      TA_INVALID_PARAM:�Ƿ��Ĳ���
 */
UINT32 DirectReadMemory(UINT32 sessionID, UINT32 ubpReadAddr, UINT8 *ubpOutBuf, UINT32 uwSize,DB_AccessMemoryAlign align)
{
    /*�п�*/
    if ( (NULL == ubpOutBuf) || (0 == uwSize) )
    {
        /*����в���Ϊ��,����TA_INVALID_PARAM*/
        return (TA_INVALID_PARAM);
    }
    
  	if (((T_UWORD)0xFFFFFFFF - (T_UWORD)ubpReadAddr) < (T_UWORD)uwSize ||
            		(T_UWORD)ubpReadAddr < (T_UWORD)0x80000000)
	{
		return (TA_INVALID_PARAM);
	}

	switch(align)
	{
		case Align_8Bit:
			Direct8BitAccessMemory(sessionID,(UINT32)ubpOutBuf,ubpReadAddr,uwSize);
			break;
		case Align_16Bit:
			{
				if ((uwSize % 2) != 0)
				{
					return (TA_INVALID_PARAM);
				}
				
				Direct16BitAccessMemory(sessionID,(UINT32)ubpOutBuf,ubpReadAddr,uwSize);
			}
			break;
		case Align_32Bit:
			{
				if ((uwSize % 4) != 0)
				{
					return (TA_INVALID_PARAM);
				}
				
				Direct32BitAccessMemory(sessionID,(UINT32)ubpOutBuf,ubpReadAddr,uwSize);
			}
			
			break;
		case Align_64Bit:
			{
				if ((uwSize % 8) != 0)
				{
					return (TA_INVALID_PARAM);
				}
				
				Direct64BitAccessMemory(sessionID,(UINT32)ubpOutBuf,ubpReadAddr,uwSize);
			}
			
			break;
		default:
			Direct8BitAccessMemory(sessionID,(UINT32)ubpOutBuf,ubpReadAddr,uwSize);
			break;
	}
	
    return TA_OK;
}



T_VOID  monitorCacheMemUpdate(T_UWORD addr,T_UWORD len)
{
	T_UWORD startAddr = 0;
	T_UWORD endAddr = 0;

	startAddr = (addr  & (~(MONITOR_LOONGSON2F_CACHE_SIZE - 1) ));
	endAddr = ((addr + len + MONITOR_LOONGSON2F_CACHE_SIZE) & (~(MONITOR_LOONGSON2F_CACHE_SIZE -1 )));

	do
	{	
  		monitorFlushCache(startAddr);
		startAddr+= MONITOR_LOONGSON2F_CACHE_SIZE;
	}while (startAddr < endAddr);
}
/*
 * @brief:
 *      Ĭ��ˢCache
 * @param[in]: sessionID:�ỰID
 * @param[in]:    uwAddr:Ҫ���µ�cache�ڴ��ַ
 * @param[in]:    uwLen:Ҫ���µĳ���
 * @return:
 *      ��
 */
T_VOID DirectFlushCache(UINT32 sessionID, UINT32 uwAddr, UINT32 uwLen)
{
	monitorCacheMemUpdate(uwAddr, uwLen);
}

