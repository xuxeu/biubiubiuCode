/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:dbVector.h
 *@brief:
 *             <li>定义vector相关宏</li>
 */
#ifndef _DB_VECTOR_H
#define _DB_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/

/************************宏定义********************************/

/* vector numbers of specific MIPS exceptions */
#define IV_LOW_VEC              0       /* lowest vector initialized    */
#define IV_TLBMOD_VEC           1       /* tlb mod vector               */
#define IV_TLBL_VEC             2       /* tlb load vector              */
#define IV_TLBS_VEC             3       /* tlb store vector             */
#define IV_ADEL_VEC             4       /* address load vector          */
#define IV_ADES_VEC             5       /* address store vector         */
#define IV_IBUS_VEC             6       /* instr. bus error vector      */
#define IV_DBUS_VEC             7       /* data bus error vector        */
#define IV_SYSCALL_VEC          8       /* system call vector           */
#define IV_BP_VEC               9       /* break point vector           */
#define IV_RESVDINST_VEC        10      /* rsvd instruction vector      */
#define IV_CPU_VEC              11      /* coprocessor unusable vector  */
#define IV_OVF_VEC              12      /* overflow vector              */
#define IV_TRAP_VEC             13      /* trap vector                  */
#define IV_VCEI_VEC             14      /* virtual coherency inst. vec  */
#define IV_FPE_VEC              15      /* floating point vector        */
#define IV_RESVD16_VEC          16      /* reserved entry               */
#define IV_RESVD17_VEC          17      /* reserved entry               */
#define IV_RESVD18_VEC          18      /* reserved entry               */
#define IV_RESVD19_VEC          19      /* reserved entry               */
#define IV_RESVD20_VEC          20      /* reserved entry               */
#define IV_RESVD21_VEC          21      /* reserved entry               */
#define IV_RESVD22_VEC          22      /* reserved entry               */
#define IV_WATCH_VEC            23      /* watchpoint vector            */
#define IV_MCHECK_VEC		    24      /* machine check                */
#define IV_RESVD25_VEC          25      /* reserved entry               */
#define IV_RESVD26_VEC          26      /* reserved entry               */
#define IV_RESVD27_VEC          27      /* reserved entry               */
#define IV_RESVD28_VEC          28      /* reserved entry               */
#define IV_RESVD29_VEC          29      /* reserved entry               */
#define IV_RESVD30_VEC          30      /* reserved entry               */
#define IV_VCED_VEC             31      /* virtual coherency data vec   */
#define IV_SWTRAP0_VEC          32      /* software trap 0              */
#define IV_SWTRAP1_VEC          33      /* software trap 1              */
#define IV_FPA_UNIMP_VEC        54      /* unimplemented FPA oper       */
#define IV_FPA_INV_VEC          55      /* invalid FPA operation        */
#define IV_FPA_DIV0_VEC         56      /* FPA div by zero              */
#define IV_FPA_OVF_VEC          57      /* FPA overflow exception       */
#define IV_FPA_UFL_VEC          58      /* FPA underflow exception      */
#define IV_FPA_PREC_VEC         59      /* FPA inexact operation        */
#define IV_BUS_ERROR_VEC        60      /* bus error vector             */
#define USER_VEC_START          32      /* start of user interrupt vectors */
#define IV_FPA_BASE_VEC         IV_FPA_UNIMP_VEC

/************************类型定义******************************/

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _DB_VECTOR_H */
