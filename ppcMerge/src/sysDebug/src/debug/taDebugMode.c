/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                         创建该文件。
 */

/**
 * @file:taDebugMode.c
 * @brief:
 *             <li>调试模式管理</li>
 */

/************************头 文 件******************************/
#include "taDebugMode.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/* 有效的调试模式 */
UINT32 taAvailModes = (DEBUGMODE_SYSTEM | DEBUGMODE_TASK);

/* 当前调试模式 */
static T_TA_DebugMode taDebugMode = DEBUGMODE_SYSTEM;

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *     获取TA当前调试模式
 * @return:
 *     调试模式
 */
T_TA_DebugMode taDebugModeGet(void)
{
    return (taDebugMode);
}

/*
 * @brief:
 *     设置TA当前调试模式
 * @param[in]:mode:调试模式
 * @return:
 *     TA_INVALID_DEBUG_MODE:无效的调试模式
 *     TA_OK:设置调试模式成功
 */
T_TA_ReturnCode taDebugModeSet(T_TA_DebugMode newMode)
{
    if (!(newMode & taAvailModes))
    {
    	return (TA_INVALID_DEBUG_MODE);
    }

    /* 设置调试模式 */
    taDebugMode = newMode;

    return (TA_OK);
}
