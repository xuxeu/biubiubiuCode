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
 * @file:taskSchedLockLib.c
 * @brief:
 *             <li>任务调度锁相关接口</li>
 */

/************************头 文 件*****************************/
#include "taskSchedLockLib.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

#ifdef	CONFIG_CORE_SMP
/* 任务调度锁变量 */
static volatile atomic_t taTaskSchedInvoked = 0;
#endif

/************************全局变量*****************************/

/************************实   现********************************/

#ifdef	CONFIG_CORE_SMP
/*
 * @brief:
 *     CPC中断句柄，该函数用于停止当前CPU
 * @return:
 *     无
 */
void taTaskSchedLockHandler(void)
{
    while (TA_MULTIOS_ATOMIC_GET((atomic_t *)&taTaskSchedInvoked) != 0)
	;
}
#endif

/*
 * @brief:
 *     任务调度锁,阻止其它任务运行
 * @return:
 *     无
 */
void taTaskSchedLock(void)
{
#ifdef	CONFIG_CORE_SMP

	/* 在多核系统中，并发断点异常需要锁调度，直到解锁 */
    while (!(taTas((void *)&taTaskSchedInvoked)))
	;

    /* 投递IPI中断 */
    sdaIpiEmit(taTaskSchedLockHandler, TA_NO_WAIT);
#endif	/* CONFIG_CORE_SMP */
}

/*
 * @brief:
 *     任务调度解锁,允许其它任务运行
 * @return:
 *     无
 */
void taTaskSchedUnlock(void)
{
#ifdef	CONFIG_CORE_SMP

	/* 判断是否已经解锁 */
	if (TA_MULTIOS_ATOMIC_GET ((atomic_t *)&taTaskSchedInvoked) == 0)
	return;

    /* 释放所有CPU，让其它任务运行 */
	TA_MULTIOS_ATOMIC_CLEAR((atomic_t *)&taTaskSchedInvoked);

#endif	/* CONFIG_CORE_SMP */
}
