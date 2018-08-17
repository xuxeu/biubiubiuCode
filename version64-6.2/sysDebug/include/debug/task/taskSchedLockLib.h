/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-12-17         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:taskSchedLockLib.h
 * @brief:
 *             <li>任务调度锁宏定义和函数声明</li>
 */
#ifndef _TASK_SCHED_LOCK_LIB_H_
#define _TASK_SCHED_LOCK_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头 文 件******************************/

/************************类型定义*****************************/

/************************宏 定 义******************************/

/************************接口声明*****************************/

/*
 * @brief:
 *     任务调度锁,阻止其它任务运行
 * @return:
 *     无
 */
void taTaskSchedLock(void);

/*
 * @brief:
 *     任务调度解锁,允许其它任务运行
 * @return:
 *     无
 */
void taTaskSchedUnlock(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_SCHED_LOCK_LIB_H_ */
