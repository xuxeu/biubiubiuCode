/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-07-01         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file: dbMemory.c
 * @brief:
 *             <li>调试管理的内存操作接口管理实现</li>
 */
/************************头 文 件******************************/
#include "dbMemory.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/* 内存操作表 */
static T_DB_MemoryOper taCurrOperTable = {NULL,NULL,NULL};

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *     初始化内存操作表
 * @param[in]:operTable:包含内存读写、刷新Cache接口的内存操作表
 * @return:
 *        TA_OK: 操作成功
 *        TA_INVALID_PARAM: 参数非法
 */
T_TA_ReturnCode taDebugMemoryInit(const T_DB_MemoryOper* operTable)
{
    /* 设置当前内存操作表 */
    taCurrOperTable.WriteMemory = operTable->WriteMemory;
    taCurrOperTable.ReadMemory = operTable->ReadMemory;
    taCurrOperTable.FlushCache = operTable->FlushCache;

    return (TA_OK);
}

/*
 * @brief:
 *     调用全局内存操作表指向的写内存函数进行写内存操作
 * @param[in]:ctxId: 上下文ID
 * @param[in]:desAddr: 待写入数据的内存起始地址
 * @param[in]:srcAddr: 包含写入数据的缓冲地址
 * @param[in]:size: 待写入数据大小
 * @param[in]:align: 内存对齐方式
 * @return:
 *        TA_OK: 操作成功
 *        TA_FAIL: 写内存操作失败
 */
T_TA_ReturnCode taDebugWriteMemory(UINT32 ctxId, UINT32 desAddr, const UINT8 *srcAddr, UINT32 size,T_DB_AccessMemoryAlign align)
{
    T_TA_ReturnCode ret = TA_FAIL;

    /* 内存操作表写内存函数指针不等于NULL */
    if (NULL != taCurrOperTable.WriteMemory)
    {
        /* 调用内存操作表的写内存函数 */
        ret = taCurrOperTable.WriteMemory(ctxId, desAddr, srcAddr, size,align);
		if (ret != TA_OK)
		{
			ret = TA_FAIL;
		}
    }

    return (ret);
}

/*
 * @brief:
 *     调用全局内存操作表指向的读内存函数进行读内存操作
 * @param[in]:ctxId: 上下文ID
 * @param[in]:readAddr:待读取数据的内存起始地址
 * @param[out]:outBuf:用于存储读取数据的缓冲地址
 * @param[in]:size:待读取数据的大小
 * @param[in]:align: 内存对齐方式
 * @return:
 *        TA_OK: 操作成功
 *        TA_FAIL: 读内存失败
 */
T_TA_ReturnCode taDebugReadMemory(UINT32 ctxId, UINT32 readAddr, UINT8 *outBuf, UINT32 size, T_DB_AccessMemoryAlign align)
{
    T_TA_ReturnCode ret = TA_FAIL;

    /* 内存操作表读内存函数指针不等于NULL */
    if (NULL != taCurrOperTable.ReadMemory)
    {
        /* 调用内存操作表读内存函数 */
        ret = taCurrOperTable.ReadMemory(ctxId, readAddr, outBuf, size,align);
		if (ret != TA_OK)
		{
			ret = TA_FAIL;
		}
    }

    return (ret);
}

/*
 * @brief:
 *   调用全局内存操作表指向的刷新Cache函数进行刷新Cache操作
 * @param[in]:ctxId: 上下文ID
 * @param[in]:addr: 待更新Cache内存地址
 * @param[in]:len: 待更新Cache的内存长度
 * @return:
 *     none
 */
void taDebugFlushCache(UINT32 ctxId, UINT32 addr, UINT32 len)
{
    /* 内存操作表刷新Cache函数指针不等于NULL */
    if (NULL != taCurrOperTable.FlushCache)
    {
        /* 调用内存操作表的刷新Cache函数 */
        taCurrOperTable.FlushCache(ctxId, addr, len);
    }
}
