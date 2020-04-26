/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-08-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:rtpMemLib.c
 * @brief:
 *             <li>rtp 内存相关操作</li>
 */

/************************头 文 件*****************************/
#include "taRtp.h"
#include "taRtIfLib.h"
#include "taskSessionLib.h"
#include "ta.h"

/************************宏 定 义******************************/

//UINT32 physicalAddr = 0; /*该处必须为全局变量，如果在函数中采用局部变量，vmTranslate方法会报异常，龙芯平台*/

#define TA_ROUND_DOWN(x, align)	((int)(x) & ~(align - 1))

#define TA_ROUND_UP(x, align)  (((int) (x) + (align - 1)) & ~(align - 1))

/* 内存地址是否无效 */
#define TA_MEM_ADDR_IS_INVALID(virtualAddr,physicalAddr) \
    if (vmTranslate(NULL,(virtualAddr), &(physicalAddr)) != OK)\
    {\
	    return (TA_INVALID_CONTEXT);\
    }

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/
    UINT32 physicalAddr = 0;
/************************实   现********************************/

/*
 * @brief:
 *     切换内存上下文,如果是访问实时进程空间，需要切换到实时进程的空间上下文
 * @param[in]: newCtx: 上下文ID
 * @return:
 *     NULL:切换上下文失败
 *     oldCtx:切换前的上下文ID
 */
UINT32 taMemCtxSwitch(UINT32 newCtx)
{
    if (newCtx == 0)
    {
    	newCtx = (UINT32)kernelId;
    }

    /* 是否支持内存切换 */
    if (TA_MEM_SWITCH_IS_SUPPORED())
    {
    	return (UINT32)kernelId;
    }

    return (TA_RT_MEM_SWITCH(newCtx,1));
}

/*
 * @brief:
 *      刷新RTP指定地址的Cache块
 * @param[in]: ctxId: 上下文ID
 * @param[in]: address: 内存地址
 * @param[in]: size: 地址长度
 * @return:
 *     无
 */
static void taRtpFlushCache(UINT32 ctxId, UINT32 address, UINT32 size)
{
    UINT32	savedCtx = 0;

    /* 切换到RTP内存上下文 */
    if ((savedCtx = taMemCtxSwitch(ctxId)) == 0)
    {
    	return;
    }

    /* 刷新Cache */
    taCacheTextUpdata(address, size);

	/* 恢复内存上下文 */
    taMemCtxSwitch(savedCtx);
}

/*
 * @brief:
 *     读内存
 * @param[in]: ctxId: 上下文ID
 * @param[in]: readAddr: 读取内存起始地址
 * @param[out]:outBuf: 缓冲地址
 * @param[in]: nBytes: 读取数据大小
 * @param[in]: align: 内存对齐方式
 * @return:
 *      TA_OK:读内存成功
 *      TA_INVALID_CONTEXT: 无效上下文
 *      TA_FAIL: 读内存失败
 */
static UINT32 taRtpReadMemory(UINT32 ctxId, UINT32 readAddr, UINT8 *outBuf, UINT32 nBytes, T_DB_AccessMemoryAlign align)
{
    UINT32 savedCtx = 0;
    UINT32 ret = TA_OK;
    //UINT32 physicalAddr = 0;

    /* 切换到RTP内存上下文 */
    if ((savedCtx = taMemCtxSwitch(ctxId)) == 0)
    {
    	return (TA_INVALID_CONTEXT);
    }

    /* 内存地址是否有效 */
    TA_MEM_ADDR_IS_INVALID(readAddr,physicalAddr);

    /* 读内存 */
    if ((ret = TA_RT_MEM_READ(ctxId, readAddr, outBuf, nBytes, align)) != TA_OK)
    {
    	return (TA_FAIL);
    }

	/* 恢复内存上下文 */
    taMemCtxSwitch(savedCtx);

	return (TA_OK);
}

/*
 * @brief:
 *     写内存
 * @param[in]: ctxId: 上下文ID
 * @param[in]: dest: 目的内存地址
 * @param[in]: src: 源内存地址
 * @param[in]: nBytes: 数据大小
 * @param[in]: align: 内存对齐方式
 * @return:
 *      TA_OK:写内存成功
 *      TA_INVALID_CONTEXT: 无效上下文
 *      TA_FAIL: 写内存失败
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


    /* 切换到RTP内存上下文 */
    if ((savedCtx = taMemCtxSwitch(ctxId)) == 0)
    {
    	return (TA_INVALID_CONTEXT);
    }

    /* 内存地址是否有效 */
    TA_MEM_ADDR_IS_INVALID(dest,physicalAddr);


    /* MMU开启则查看内存是否可写 */
    if (pageSize != 0)
    {
    	/* 地址页对齐 */
    	addr = TA_ROUND_DOWN (dest, pageSize);

    	/* 大小页对齐 */
    	size = TA_ROUND_UP(nBytes, pageSize);

    	/* 获取当前内存读写状态  */
    	VM_STATE_GET(NULL, (void *)addr, &state);

	    if ((state & VM_STATE_MASK_WRITABLE) != VM_STATE_WRITABLE)
	    {
	    	VM_STATE_SET(NULL, addr, size, VM_STATE_MASK_WRITABLE, VM_STATE_WRITABLE);
	    }
    }

    /* 写内存 */
    if ((ret = TA_RT_MEM_WRITE(ctxId, dest, src, nBytes, align)) != TA_OK)
    {
    	return (TA_FAIL);
    }

	/* 设置为原来的内存读写状态 */
    if (pageSize != 0)
    {
    	if ((state & VM_STATE_MASK_WRITABLE) != VM_STATE_WRITABLE)
    	{
    		VM_STATE_SET (NULL, addr, size, VM_STATE_MASK_WRITABLE, state);
    	}
    }

	/* 恢复内存上下文 */
    taMemCtxSwitch(savedCtx);

	return (TA_OK);
}

/*
 * @brief:
 *     RTP调试内存操作接口初始化
 * @return:
 *     TA_OK:   操作成功
 */
T_TA_ReturnCode taRtpMemoryInit(void)
{
    T_DB_MemoryOper taRtpMemoryOperTable;

    /* 设置内存操作接口 */
    taRtpMemoryOperTable.FlushCache = taRtpFlushCache;
    taRtpMemoryOperTable.ReadMemory = taRtpReadMemory;
    taRtpMemoryOperTable.WriteMemory = taRtpWriteMemory;

    /* 安装RTP调试的内存读写接口 ，及Cache刷新接口, 并返回其值 */
    return taDebugMemoryInit(&taRtpMemoryOperTable);
}
