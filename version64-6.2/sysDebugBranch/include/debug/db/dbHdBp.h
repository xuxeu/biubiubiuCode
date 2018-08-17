/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2011-06-27         彭元志，北京科银京成技术有限公司
 *                          创建该文件。 
 */

/*
 *@file:dbHdBp.h
 *@brief:
 *             <li>硬件断点删除和设置的接口定义</li>
 */

#ifndef _DB_HDBREAKPOINT_H
#define _DB_HDBREAKPOINT_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头文件********************************/
#include "taErrorDefine.h"
#include "dbBreakpoint.h"
#include "ta.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************接口声明******************************/

/*
 * @brief:
 *     插入硬件断点，硬件断点分为数据断点和指令断点
 * @param[in]: debugInfo: 调试会话信息
 * @param[in]: addr: 断点地址
 * @param[in]: type: 断点类型
 * @param[in]: size: 断点宽度
 * @return:
 *    TA_OK: 插入成功
 *    TA_DB_BREAKPOINT_FULL:插入失败(断点打满了)
 */
T_TA_ReturnCode taDebugInsertHardBp(T_DB_Info *debugInfo, ulong_t addr, T_DB_BpType type, UINT32 size);

/*
 * @brief:
 *     删除硬件断点
 * @param[in]: debugInfo: 调试会话信息
 * @param[in]: addr: 断点逻辑地址
 * @param[in]: type: 断点类型
 * @param[in]: size: 断点宽度
 * @return:
 *    TA_OK:删除成功
 *    TA_DB_BREAKPOINT_NOTFOUND:断点找不到
 */
T_TA_ReturnCode taDebugRemoveHardBp(T_DB_Info *debugInfo, ulong_t addr, T_DB_BpType type, UINT32 size);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_DB_HDBREAKPOINT_H */
