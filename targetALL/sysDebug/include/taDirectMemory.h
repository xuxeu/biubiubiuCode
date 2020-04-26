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
 *@file:taDirectMemory.h
 *@brief:
 *             <li>直接读写内存操作接口声明</li>
 */
#ifndef _TA_DIRECT_MEMORY_H
#define _TA_DIRECT_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头文件********************************/
#include "dbMemory.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************接口声明******************************/

/**
 * @brief
 * 		TA直接内存访问初始化
 * @return:
 *      无
 */
void taDebugInitDirectMemory(void);

/*
 * @brief:
 *      刷新数据Cache
 * @param[in]:sessionID: 会话ID
 * @param[in]:addr: 要更新的cache内存地址
 * @param[in]:len: 要更新的长度
 * @return:
 *      无
 */
void taDirectFlushCache(UINT32 sessionID, UINT32 addr, UINT32 len);

/*
 * @brief:
 *      直接读内存
 * @param[in]:sessionID:会话ID
 * @param[in]:readAddr:内存地址
 * @param[out]:outBuf: 读内存缓冲
 * @param[in]:size: 要读的内存大小
 * @param[in]:align: 对齐大小
 * @return:
 *      TA_OK:操作成功
 *      TA_INVALID_PARAM:非法的参数
 */
UINT32 taDirectReadMemory(UINT32 sessionID, UINT32 readAddr, UINT8 *outBuf, UINT32 size,T_DB_AccessMemoryAlign align);

/*
 * @brief:
 *      直接写内存
 * @param[in]:sessionID: 会话ID
 * @param[in]:src: 源内存地址
 * @param[in]:size: 数据大小
 * @param[in]:align: 对齐大小
 * @param[out]:dest: 目的内存地址
 * @return:
 *      TA_OK:操作成功
 *      TA_INVALID_PARAM:非法的参数
 */
T_TA_ReturnCode taDirectWriteMemory(UINT32 sessionID, UINT32 dest, const UINT8 *src, UINT32 size,T_DB_AccessMemoryAlign align);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_TA_DIRECT_MEMORY_H */
