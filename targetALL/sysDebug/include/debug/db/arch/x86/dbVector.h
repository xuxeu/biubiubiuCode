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
#define GENERAL_ZDIV_INT        (0) /*整数除0异常*/
#define GENERAL_DB_INT          (1) /*调试单步异常*/
#define GENERAL_NMI_INT         (2) /*非屏蔽中断*/
#define GENERAL_TRAP_INT        (3) /*软件断点异常*/
#define GENERAL_OVERF_INT       (4) /*运算溢出*/
#define GENERAL_BOUND_INT       (5) /*Bound指令范围越界*/
#define GENERAL_ILLINS_INT 	    (6) /*非法操作码异常*/
#define GENERAL_DEV_INT         (7) /*协处理器不可用异常*/
#define GENERAL_DF_INT          (8) /*双故障异常*/
#define GENERAL_CSO_INT         (9) /*协处理器段越限*/
#define GENERAL_TSS_INT         (10)/*非法TSS(Task State Segment)异常*/
#define GENERAL_SEGMENT_INT     (11)/*段不存在异常*/
#define GENERAL_SF_INT          (12)/*栈故障异常*/
#define GENERAL_GP_INT          (13)/*一般保护异常*/
#define GENERAL_PAGE_INT        (14)/*页故障异常*/
#define GENERAL_MF_INT          (16)/*数字运算错误异常*/
#define GENERAL_AE_INT          (17)/*对齐检验异常*/
#define GENERAL_MC_INT          (18)/*Machine Check异常*/
#define GENERAL_SIMD_INT        (19)/*SIMD浮点异常*/

/************************类型定义******************************/

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _DB_VECTOR_H */
