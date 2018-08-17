/************************************************************************
*				北京科银京成技术有限公司 版权所有
* 	 Copyright (C)  2011 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/*
 * 修改历史:
 * 2011-9-8 胡雪雷，北京科银京成技术有限公司
 *          创建文件        
*/

/*
* @file: taDirectMemory.c
* @brief:
*	    <li>功能:按系统级直接读写内存的接口</li>
* @implements: DTA
*/

/************************头 文 件******************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "dbMemory.h"
#include "monitorBSP.h"
/************************宏 定 义******************************/
/************************类型定义******************************/
/************************外部声明******************************/
T_EXTERN T_VOID  kbspRawCacheInstructUpdate(UINT32 addr,UINT32 len);

/************************前向声明******************************/
/************************模块变量******************************/
/************************全局变量******************************/
/************************实   现*******************************/
/*
 * @brief:
 *      按1字节直接写内存
 * @param[in]:    sessionID:会话ID
 * @param[in]:    ubpSrcAddr: 源内存地址
 * @param[in]:    uwSize:    数据大小
 * @param[out]:    ubpDesAddr: 目的内存地址
 * @return:
 *      无
 */
T_MODULE T_VOID Direct8BitAccessMemory(UINT32 sessionID, UINT32 ubpDesAddr, UINT32 ubpSrcAddr, UINT32 uwSize)
{
	UINT8* desAddr = (UINT8*)ubpDesAddr;
	UINT8* srcAddr = (UINT8*)ubpSrcAddr;
	
	/* 循环执行写内在操作,循环 uwSize次*/
    while(uwSize != 0)
    {
        /*按字节从srcAddr读数据并写到desAddr中*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        uwSize = uwSize - 1;
    }
}

/*
 * @brief:
 *      按2字节直接写内存
 * @param[in]:    sessionID:会话ID
 * @param[in]:    ubpSrcAddr: 源内存地址
 * @param[in]:    uwSize:    数据大小
 * @param[out]:    ubpDesAddr: 目的内存地址
 * @return:
 *      无
 */
T_MODULE T_VOID Direct16BitAccessMemory(UINT32 sessionID, UINT32 ubpDesAddr, UINT32 ubpSrcAddr, UINT32 uwSize)
{
	UINT16* desAddr = (UINT16*)ubpDesAddr;
	UINT16* srcAddr = (UINT16*)ubpSrcAddr;
	
	/* 循环执行写内在操作,循环 uwSize次*/
    while(uwSize != 0)
    {
        /*按字节从srcAddr读数据并写到desAddr中*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        uwSize = uwSize - 2;
    }
}

/*
 * @brief:
 *      按4字节直接写内存
 * @param[in]:    sessionID:会话ID
 * @param[in]:    ubpSrcAddr: 源内存地址
 * @param[in]:    uwSize:    数据大小
 * @param[out]:    ubpDesAddr: 目的内存地址
 * @return:
 *      uwSize
 */
T_MODULE T_VOID Direct32BitAccessMemory(UINT32 sessionID, UINT32 ubpDesAddr, UINT32 ubpSrcAddr, UINT32 uwSize)
{
	UINT32* desAddr = (UINT32*)ubpDesAddr;
	UINT32* srcAddr = (UINT32*)ubpSrcAddr;
	
	/* 循环执行写内在操作,循环 uwSize次*/
    while(uwSize != 0)
    {
        /*按字节从srcAddr读数据并写到desAddr中*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        uwSize = uwSize - 4;
    }
}

/*
 * @brief:
 *      按8字节直接写内存
 * @param[in]:    sessionID:会话ID
 * @param[in]:    ubpSrcAddr: 源内存地址
 * @param[in]:    uwSize:    数据大小
 * @param[out]:    ubpDesAddr: 目的内存地址
 * @return:
 *      uwSize
 */
T_MODULE T_VOID Direct64BitAccessMemory(UINT32 sessionID, UINT32 ubpDesAddr, UINT32 ubpSrcAddr, UINT32 uwSize)
{
	UINT64* desAddr = (UINT64*)ubpDesAddr;
	UINT64* srcAddr = (UINT64*)ubpSrcAddr;
	
	/* 循环执行写内在操作,循环 uwSize次*/
    while(uwSize != 0)
    {
        /*按字节从srcAddr读数据并写到desAddr中*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        uwSize = uwSize - 8;
    }
}

/*
 * @brief:
 *      直接写内存
 * @param[in]:    sessionID:会话ID
 * @param[in]:    ubpSrcAddr: 源内存地址
 * @param[in]:    uwSize:    数据大小
 * @param[in]:    align:    对齐大小
 * @param[out]:    ubpDesAddr: 目的内存地址
 * @return:
 *      DB_OK:    操作成功
 *      DB_INVALID_PARAM:非法的参数
 */
T_TA_ReturnCode DirectWriteMemory(UINT32 sessionID, UINT32 ubpDesAddr, T_CONST UINT8 *ubpSrcAddr, UINT32 uwSize,DB_AccessMemoryAlign align)
{
    /*判空*/
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
 *      直接读内存
 * @param[in]:    sessionID:会话ID
 * @param[in]:    ubpReadAddr:内存地址
 * @param[in]:    align:    对齐大小
 * @param[in]:    uwSize:   要读的内存大小
 * @param[out]:    ubpOutBuf: 读内存缓冲
 * @return:
 *      TA_OK:    操作成功
 *      TA_INVALID_PARAM:非法的参数
 */
UINT32 DirectReadMemory(UINT32 sessionID, UINT32 ubpReadAddr, UINT8 *ubpOutBuf, UINT32 uwSize,DB_AccessMemoryAlign align)
{
    /*判空*/
    if ( (NULL == ubpOutBuf) || (0 == uwSize) )
    {
        /*如果有参数为空,返回TA_INVALID_PARAM*/
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
 *      默认刷Cache
 * @param[in]: sessionID:会话ID
 * @param[in]:    uwAddr:要更新的cache内存地址
 * @param[in]:    uwLen:要更新的长度
 * @return:
 *      无
 */
T_VOID DirectFlushCache(UINT32 sessionID, UINT32 uwAddr, UINT32 uwLen)
{
	monitorCacheMemUpdate(uwAddr, uwLen);
}

