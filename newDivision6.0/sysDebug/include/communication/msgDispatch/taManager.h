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
 *@file:taManager.h
 *@brief:
 *             <li>信息管理模块头文件</li>
 */

#ifndef _TA_MANAGER_H
#define _TA_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"

/************************宏定义********************************/

/* Manager支持的命令个数*/
#define TA_MANAGER_COMMANDNUM  5

/************************类型定义******************************/

/************************接口声明******************************/

/*
 * @brief:
 *     初始化TA查询模块
 * @return:
 *     无
 */
void taManagerInit(void);

/*
 * @brief:
 *     获取体系结构信息
 * @param[out]:outbuf: 输出数据缓冲
 * @param[out]:outsize: 输出数据长度
 * @return:
 *      无
 */
void taArchInfoGet(UINT8 *outbuf, UINT32 *outSize);

/*
 * @brief:
 *     查询体系结构的大小端
 * @param[out]:outbuf: 输出数据缓冲
 * @param[out]:outsize: 输出数据长度
 * @return:
 *      无
 */
void taEndianInfoGet(UINT8 *outbuf, UINT32 *outSize);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_TA_MANAGER_H */
