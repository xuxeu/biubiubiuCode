/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:dbMemory.h
 *@brief:
 *             <li>调试管理的内存管理定义</li>
 */
#ifndef _DB_MEMORY_H
#define _DB_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头文件********************************/
#include "taTypes.h"
#include "taErrorDefine.h"

/************************宏定义********************************/

/************************类型定义******************************/

/* 内存访问对其方式 */
typedef enum
{
    Align_8Bit = 1,
    Align_16Bit = 2,
    Align_32Bit = 4,
    Align_64Bit = 8,
    Align_Error = 0xFFFFFFFF,
    Align_None = Align_8Bit
} T_DB_AccessMemoryAlign;

/* 内存操作表 */
typedef struct
{
    /* 写内存，该接口具备检查地址有效性，并能返回操作失败的详细原因  */
    ulong_t (*WriteMemory)(ulong_t ctxId, ulong_t desAddr, const UINT8  *srcAddr, UINT32 size, T_DB_AccessMemoryAlign align);

    /* 读内存，该接口具备检查地址有效性，并能返回操作失败的详细原因 */
    ulong_t (*ReadMemory)(ulong_t ctxId, ulong_t readAddr, UINT8  *outBuf, UINT32  size, T_DB_AccessMemoryAlign align);

    /* 刷Cache */
    void (*FlushCache)(ulong_t ctxId, ulong_t addr, UINT32 len);
} T_DB_MemoryOper;

/************************接口声明******************************/

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
T_TA_ReturnCode taDebugReadMemory(ulong_t ctxId, ulong_t readAddr, UINT8 *outBuf, UINT32 size, T_DB_AccessMemoryAlign align);

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
T_TA_ReturnCode taDebugWriteMemory(ulong_t ctxId, ulong_t desAddr, const UINT8 *srcAddr, UINT32 size,T_DB_AccessMemoryAlign align);

/*
 * @brief:
 *   调用全局内存操作表指向的刷新Cache函数进行刷新Cache操作
 * @param[in]:ctxId: 上下文ID
 * @param[in]:addr: 待更新Cache内存地址
 * @param[in]:len: 待更新Cache的内存长度
 * @return:
 *     none
 */
void taDebugFlushCache(ulong_t ctxId, ulong_t addr, UINT32 len);

/*
 * @brief:
 *     初始化内存操作表
 * @param[in]:operTable:包含内存读写、刷新Cache接口的内存操作表
 * @return:
 *        TA_OK: 操作成功
 *        TA_INVALID_PARAM: 参数非法
 */
T_TA_ReturnCode taDebugMemoryInit(const T_DB_MemoryOper* operTable);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_DB_MEMORY_H */
