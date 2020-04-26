/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-07-020        彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbHdBp.c
 * @brief:
 *             <li>MIPS体系结构硬件断点设置和删除的实现</li>
 */

/************************头 文 件******************************/
#include "taTypes.h"
#include "dbBreakpoint.h"
#include "dbContext.h"
#include "sdaMain.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *     插入硬件断点，硬件断点分为数据断点和指令断点
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:addr: 断点地址
 * @param[in]:type: 断点类型
 * @param[in]:size: 断点宽度
 * @return:
 *         TA_OK: 插入成功
 *         TA_DB_BREAKPOINT_FULL:插入失败(断点打满了)
 */
T_TA_ReturnCode taDebugInsertHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
    return (TA_OK);
}

/*
 * @brief:
 *     删除硬件断点
 * @param[in]:sessionID :会话ID，
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:addr: 断点逻辑地址
 * @param[in]:type: 断点类型
 * @param[in]:size: 断点宽度
 * @return:
 *         TA_OK:删除成功
 *         TA_DB_BREAKPOINT_NOTFOUND:断点找不到
 */
T_TA_ReturnCode taDebugRemoveHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
    /* 返回操作成功 */
    return (TA_OK);
}
