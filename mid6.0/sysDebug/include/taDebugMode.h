/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-23         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:taDebugMode.h
 *@brief:
 *             <li>调试模式相关函数声明</li>
 */
#ifndef _TA_DEBUG_MODE_H
#define _TA_DEBUG_MODE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头 文 件******************************/
#include "taTypes.h"
#include "taErrorDefine.h"

/************************宏定义*******************************/

/************************类型定义*****************************/

/* 调试模式 */
typedef enum
{
    DEBUGMODE_SYSTEM = 1,
    DEBUGMODE_TASK   = 2,
} T_TA_DebugMode;

/************************接口声明*****************************/

/*
 * @brief:
 *     获取TA当前调试模式
 * @return:
 *     调试模式
 */
T_TA_DebugMode taDebugModeGet(void);

/*
 * @brief:
 *     设置TA当前调试模式
 * @param[in]:mode:调试模式
 * @return:
 *     TA_INVALID_DEBUG_MODE:无效的调试模式
 *     TA_OK:设置调试模式成功
 */
T_TA_ReturnCode taDebugModeSet(T_TA_DebugMode mode);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _TA_DEBUG_MODE_H */
