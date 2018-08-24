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
 *@file:sdaCpuset.h
 *@brief:
 *             <li>提供多核的cpuset_t类型相关操作</li>
 */
#ifdef CONFIG_CORE_SMP

#ifndef _SDA_CPUSET_H
#define _SDA_CPUSET_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头 文 件******************************/

/************************宏定义*******************************/

#define TA_CPUSET_ATOMICCOPY(cpusetDst, cpusetSrc)	\
	(TA_MULTIOS_ATOMIC_SET((atomic_t*)&cpusetDst, (atomic_t)cpusetSrc))

#define TA_CPUSET_ATOMICSET(cpuset, n)   \
	(TA_MULTIOS_ATOMIC_OR((atomic_t *)&cpuset, 1 << n))

#define TA_CPUSET_ATOMICCLR(cpuset, n)	 \
	(TA_MULTIOS_ATOMIC_AND((atomic_t *)&cpuset, ~(1 << n)))

#define CPUSET_ISZERO(cpuset)   (cpuset == 0 ? TRUE : FALSE)
#define CPUSET_SET(cpuset, n)   (cpuset |= (1 << n))
#define CPUSET_ISSET(cpuset, n) (cpuset & (1 << n))
#define CPUSET_CLR(cpuset, n)   (cpuset &= ~(1 << n))
#define CPUSET_ZERO(cpuset)     (cpuset = 0)

/************************类型定义*****************************/

/************************接口声明*****************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SDA_CPUSET_H */
#endif
