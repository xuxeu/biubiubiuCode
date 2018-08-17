/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-23         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file:sdaCpuset.h
 *@brief:
 *             <li>�ṩ��˵�cpuset_t������ز���</li>
 */
#ifdef CONFIG_CORE_SMP

#ifndef _SDA_CPUSET_H
#define _SDA_CPUSET_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ �� ��******************************/

/************************�궨��*******************************/

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

/************************���Ͷ���*****************************/

/************************�ӿ�����*****************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SDA_CPUSET_H */
#endif
