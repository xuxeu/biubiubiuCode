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
 * @file  dbAtomic.h
 * @brief:
 *             <li>原子操作宏定义</li>
 */
#ifdef CONFIG_CORE_SMP

#ifndef _DB_ATOMIC_H_
#define _DB_ATOMIC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"

/************************宏定义********************************/

/* Macros to provide memory barrier */
#ifdef __X86__
#ifdef CONFIG_TA_LP64
	#define MEM_BARRIER_R()	    asm volatile("lock;addq $0,0(%%rsp)":::"memory");
	#define MEM_BARRIER_W()	    asm volatile("lock;addq $0,0(%%rsp)":::"memory");
	#define MEM_BARRIER_RW()	asm volatile("lock;addq $0,0(%%rsp)":::"memory");
	#define INSTR_BARRIER()     asm volatile("lock;addq $0,0(%%rsp)":::"memory");
#else
	#define MEM_BARRIER_R() 	asm volatile("lock;addl $0,0(%%esp)":::"memory");
	#define MEM_BARRIER_W() 	asm volatile("lock;addl $0,0(%%esp)":::"memory");
	#define MEM_BARRIER_RW()	asm volatile("lock;addl $0,0(%%esp)":::"memory");
	#define INSTR_BARRIER() 	asm volatile("lock;addl $0,0(%%esp)":::"memory");
#endif
#endif

/* 将target指向的值与oldValue对比，如果相等，设置target指向的值为newValue */
#ifdef CONFIG_TA_LP64
#define TA_MULTIOS_ATOMIC_CAS(target, oldValue, newValue)  \
	_atomic_64_cas(target, oldValue, newValue)
	
	/* 获取target指向的值 */
#define TA_MULTIOS_ATOMIC_GET(target)  _atomic_64_get(target)
	
	/* 将target指向的值设置为value，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_SET(target, value)  _atomic_64_set(target, value)
	
	/* 清除target指向的值，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_CLEAR(target)  _atomic_64_clear(target)
	
	/* 将target指向的值与value位或，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_OR(target, value)  _atomic_64_or(target, value)
	
	/* 将target指向的值与value位与，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_AND(target, value)  _atomic_64_and(target, value)

#else
#define TA_MULTIOS_ATOMIC_CAS(target, oldValue, newValue)  \
	_atomic_32_cas(target, oldValue, newValue)

/* 将target指向的值增加value，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_ADD(target, value)  _atomic_32_add(target, value)

/* 将target指向的值减去value，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_SUB(target, value)  _atomic_32_sub(target, value)

/* 将target指向的值增加1，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_INC(target)  _atomic_32_inc(target)

/* 将target指向的值减去1，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_DEC(target)  _atomic_32_dec(target)

/* 获取target指向的值 */
#define TA_MULTIOS_ATOMIC_GET(target)  _atomic_32_get(target)

/* 将target指向的值设置为value，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_SET(target, value)  _atomic_32_set(target, value)

/* 清除target指向的值，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_CLEAR(target)  _atomic_32_clear(target)

/* 将target指向的值与value位或，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_OR(target, value)  _atomic_32_or(target, value)

/* 将target指向的值与value位异或，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_XOR(target, value)  _atomic_32_xor(target, value)

/* 将target指向的值与value位与，并返回target原有的值 */
#define TA_MULTIOS_ATOMIC_AND(target, value)  _atomic_32_and(target, value)

#endif


/************************类型定义******************************/

/************************接口声明******************************/

#ifdef CONFIG_TA_LP64

/* 将target指向的值与oldValue对比，如果相等，设置target指向的值为newValue  */
BOOL _atomic_64_cas(long *target,atomic_t oldValue,atomic_t newValue);


/* 将target指向的值与value位或，并返回target原有的值  */
atomic_t _atomic_64_or(atomic_t *target,atomic_t value);

/* 将target指向的值与value位与，并返回target原有的值 */
atomic_t _atomic_64_and(atomic_t *target,atomic_t value);

/* 将target指向的值设置为value，并返回target原有的值 */
atomic_t _atomic_64_set(atomic_t *target, atomic_t value);

/* 获取target指向的值 */
atomic_t _atomic_64_get(atomic_t *target);

/* 清除target指向的值，并返回target原有的值 */
atomic_t _atomic_64_clear(atomic_t *target);

#else
/* 将target指向的值与oldValue对比，如果相等，设置target指向的值为newValue  */
BOOL _atomic_32_cas(atomic_t *target,atomic_t oldValue,atomic_t newValue);


/* 将target指向的值与value位或，并返回target原有的值  */
atomic_t _atomic_32_or(atomic_t *target,atomic_t value);

/* 将target指向的值与value位与，并返回target原有的值 */
atomic_t _atomic_32_and(atomic_t *target,atomic_t value);

/* 将target指向的值设置为value，并返回target原有的值 */
atomic_t _atomic_32_set(atomic_t *target, atomic_t value);

/* 将target指向的值与value位与，并返回target原有的值 */
atomic_t _atomic_32_and(atomic_t *target, atomic_t value);

/* 将target指向的值与value位或，并返回target原有的值 */
atomic_t _atomic_32_or(atomic_t *target, atomic_t value);

/* 将target指向的值与value位与，并返回target原有的值 */
atomic_t _atomic_32_and(atomic_t * target, atomic_t value);

/* 获取target指向的值 */
extern atomic_t _atomic_32_get(atomic_t *target);

/* 清除target指向的值，并返回target原有的值 */
extern atomic_t _atomic_32_clear(atomic_t *target);

/* 测试地址值，如果设置则回复FASLE，否则设置且回复TRUE */
BOOL taTas(void *address);

#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SDA_ATOMIC_H_ */
#endif /* CONFIG_CORE_SMP */
