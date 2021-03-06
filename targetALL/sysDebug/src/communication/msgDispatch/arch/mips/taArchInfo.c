/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                           创建该文件。
 */

/*
 * @file:taArchInfo.c
 * @brief:
 *             <li>体系结构信息查询</li>
 */

/************************头 文 件******************************/
#include "taTypes.h"
#include "string.h"
/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *     查询mips体系结构下的体系结构信息
 * @param[out]:bpOutbuf: 输出数据缓冲，输出数据为"MIPS"
 * @param[out]:outsize: 输出数据长度
 * @return:
 *      无
 */
void taArchInfoGet(UINT8 *bpOutbuf, UINT32 *outSize)
{
    /* 设置目标机体系结构为MIPS */
    strcpy((char *)bpOutbuf, "MIPS");

    /* 设置输出缓冲中的字符串长度 */
    *outSize = strlen((const char *)bpOutbuf);
}

/*
 * @brief:
 *     查询PPC体系结构的大小端
 * @param[out]:outbuf: 输出数据缓冲，输出数据为"big"
 * @param[out]:outsize: 输出数据长度
 * @return:
 *      无
 */
void taEndianInfoGet(UINT8 *outbuf, UINT32 *outSize)
{
    /* 设置为小端 */
    strcpy((char *)outbuf, "little");

    /* 设置输出缓冲中的字符串长度 */
    *outSize = strlen((const char *)outbuf);
}
