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
 * @file: taskInitLib.h
 * @brief:
 *             <li>任务级调试初始化相关接口声明</li>
 */
#ifndef _TASK_INIT_LIB_H_
#define _TASK_INIT_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头 文 件******************************/
#include "taErrorDefine.h"

/************************类型定义*****************************/

/************************宏 定 义******************************/

/************************接口声明*****************************/

/**
 * @brief:
 * 	   初始化任务HOOK，挂载任务创建HOOK和任务删除HOOK
 * @return:
 * 	   无
 */
void taTaskHookInit(void);

/**
 * @brief:
 * 	  运行时接口初始化
 * @return:
 * 	   无
 */
void taRtLibInit(TA_RT_IF *pRtIf);

/**
 * @brief:
 * 	  任务相关运行时接口初始化
 * @return:
 * 	   无
 */
void taTaskRtIfInit(void);

/*
 * @brief:
 *     安装控制命令处理函数
 * @return:
 *     无
 */
void taCtrlCommandInit(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_INIT_LIB_H_ */
