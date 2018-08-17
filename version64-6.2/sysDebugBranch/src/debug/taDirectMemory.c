/************************************************************************
*				北京科银京成技术有限公司 版权所有
* 	 Copyright (C)  2013 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/*
 * 修改历史:
 * 2013-06-26   彭元志，北京科银京成技术有限公司
 *                  创建该文件。
*/

/*
* @file: taDirectMemory.c
* @brief:
*	    <li>功能:系统级直接读写内存的接口</li>
*/

/************************头 文 件******************************/
#include "dbMemory.h"
#include "taMemProbeLib.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/* DB结构表，用于传递读写内存和刷cache函数 */
static T_DB_MemoryOper m_OperTable;

/************************全局变量*****************************/

/************************函数实现*****************************/
/*
 * @brief:
 *      按1字节直接写内存
 * @param[in]:ctxId: 上下文ID
 * @param[in]:src: 源内存地址
 * @param[in]:size: 数据大小
 * @param[out]:dest: 目的内存地址
 * @return:
 *      无
 */
static void taDirect8BitAccessMemory(ulong_t ctxId, ulong_t dest, ulong_t src, UINT32 size)
{
	UINT8* desAddr = (UINT8*)dest;
	UINT8* srcAddr = (UINT8*)src;
	
	/* 循环执行写内在操作,循环 size次*/
    while (size != 0)
    {
        /*按字节从srcAddr读数据并写到desAddr中*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        size = size - 1;
    }
}

/*
 * @brief:
 *      按2字节直接写内存
 * @param[in]:ctxId: 上下文ID
 * @param[in]:src: 源内存地址
 * @param[in]:size: 数据大小
 * @param[out]:dest: 目的内存地址
 * @return:
 *      无
 */
static void taDirect16BitAccessMemory(ulong_t ctxId, ulong_t dest, ulong_t src, UINT32 size)
{
	UINT16* desAddr = (UINT16*)dest;
	UINT16* srcAddr = (UINT16*)src;
	
	/* 循环执行写内在操作,循环 size次*/
    while (size != 0)
    {
        /*按字节从srcAddr读数据并写到desAddr中*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        size = size - 2;
    }
}

/*
 * @brief:
 *      按4字节直接写内存
 * @param[in]:ctxId: 上下文ID
 * @param[in]:src: 源内存地址
 * @param[in]:size: 数据大小
 * @param[out]:dest: 目的内存地址
 * @return:
 *      无
 */
static void taDirect32BitAccessMemory(ulong_t ctxId, ulong_t dest, ulong_t src, UINT32 size)
{
	UINT32* desAddr = (UINT32*)dest;
	UINT32* srcAddr = (UINT32*)src;
	
	/* 循环执行写内在操作,循环 size次*/
    while (size != 0)
    {
        /*按字节从srcAddr读数据并写到desAddr中*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        size = size - 4;
    }
}

/*
 * @brief:
 *      按8字节直接写内存
 * @param[in]:ctxId: 上下文ID
 * @param[in]:src: 源内存地址
 * @param[in]:size: 数据大小
 * @param[out]:dest: 目的内存地址
 * @return:
 *      无
 */
static void taDirect64BitAccessMemory(ulong_t ctxId, ulong_t dest, ulong_t src, UINT32 size)
{
	UINT64* desAddr = (UINT64*)dest;
	UINT64* srcAddr = (UINT64*)src;
	
	/* 循环执行写内在操作,循环 size次*/
    while (size != 0)
    {
        /*按字节从srcAddr读数据并写到desAddr中*/
        *desAddr = *srcAddr;
        desAddr++;
        srcAddr++;
        size = size - 8;
    }
}

/*
 * @brief:
 *      直接写内存
 * @param[in]:ctxId: 上下文ID
 * @param[in]:src: 源内存地址
 * @param[in]:size: 数据大小
 * @param[in]:align: 对齐大小
 * @param[out]:dest: 目的内存地址
 * @return:
 *      TA_OK:操作成功
 *      TA_INVALID_PARAM:非法的参数
 */
 
T_TA_ReturnCode taDirectWriteMemory(ulong_t ctxId, ulong_t dest, const UINT8 *src, UINT32 size,T_DB_AccessMemoryAlign align)
{
    /* 判空 */
    if ((NULL == src) || (0 == size))
    {
        return (TA_INVALID_PARAM);
    }

    /* 测试内存是否可访问 */
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
 *      直接读内存
 * @param[in]:ctxId: 上下文ID
 * @param[in]:readAddr:内存地址
 * @param[out]:outBuf: 读内存缓冲
 * @param[in]:size: 要读的内存大小
 * @param[in]:align: 对齐大小
 * @return:
 *      TA_OK:操作成功
 *      TA_INVALID_PARAM:非法的参数
 */
UINT32 taDirectReadMemory(ulong_t ctxId, ulong_t readAddr, UINT8 *outBuf, UINT32 size,T_DB_AccessMemoryAlign align)
{
    /* 判空 */
    if ((NULL == outBuf) || (0 == size))
    {
        return (TA_INVALID_PARAM);
    }

    /* 测试内存是否可访问 */
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
 *      刷新数据Cache
 * @param[in]:ctxId: 上下文ID
 * @param[in]:addr: 要更新的cache内存地址
 * @param[in]:len: 要更新的长度
 * @return:
 *      无
 */
void taDirectFlushCache(ulong_t ctxId, ulong_t addr, UINT32 len)
{
    /* 刷新Cache */
	taCacheTextUpdata(addr, len);
}

/**
 * @brief
 * 		TA直接内存访问初始化
 * @return:
 *      无
 */
void taDebugInitDirectMemory(void)
{
    /* 初始化内存操作表 */
    m_OperTable.FlushCache = taDirectFlushCache;
    m_OperTable.ReadMemory = taDirectReadMemory;
    m_OperTable.WriteMemory = taDirectWriteMemory;

    taDebugMemoryInit(&m_OperTable);
}
