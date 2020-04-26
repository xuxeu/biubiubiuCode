/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * 修改历史:
 * 2013-11-08         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbMemProbeLib.c
 * @brief:
 *             <li>该模块提供体系结构相关的内存探测访问接口</li>
 */

/************************头 文 件*****************************/
#include "dbVector.h"
#include "ta.h"
#ifdef CONFIG_DELTAOS
#include <taskLib.h>
#endif

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/
extern void taMemProbeByteRead  (void);
extern void taMemProbeByteWrite (void);
extern void taMemProbeWordRead  (void);
extern void taMemProbeWordWrite (void);
extern void taMemProbeLongRead  (void);
extern void taMemProbeLongWrite (void);
extern void taMemProbeErrorRtn  (void);

/************************前向声明*****************************/

#ifdef CONFIG_DELTAOS
/* 内存探测异常处理函数 */
static BOOL taMemProbeTrapHandle(int vecNum, ESF1 *pEsf, REG_SET *pRegSet,
		EXC_INFO *pExcInfo);
#endif

/************************模块变量*****************************/

/* 保存old异常处理HOOK */
static FUNCPTR oldExcBaseHook;

/************************全局变量*****************************/

/************************函数实现*****************************/

/**
 * @brief:
 *     内存探测体系结构相关初始化，挂接内存访问非法异常处理函数
 * @param[in]:_func_excBaseHook: 异常HOOK指针，异常产生时操作系统调用该HOOK
 * @return:
 * 	   OK：初始化成功
 */
STATUS taMemArchProbeInit(FUNCPTR *_func_excBaseHook)
{
	oldExcBaseHook = *_func_excBaseHook;
#ifdef CONFIG_DELTAOS
    *_func_excBaseHook = &taMemProbeTrapHandle;
#endif
    return (OK);
}

#ifdef CONFIG_DELTAOS
/*
 * @brief:
 *      内存探测异常处理入口函数
 * @param[in]: vecNum: 异常号
 * @param[in]: pEsf: 异常产生时CPU压栈的上下文，包括EFLAGS, CS, EIP等
 * @param[in]: pRegSet: 栈上保存的异常上下文
 * @param[in]: pExcInfo: 额外信息(未使用)
 * @return:
 *     TRUE:系统继续处理异常
 *     FALSE:系统不继续处理异常
 */
static BOOL taMemProbeTrapHandle(int vecNum, ESF1 *pEsf, REG_SET *pRegSet, EXC_INFO *pExcInfo)
{
    /* 检查异常是否发生在内存探测操作 */
    if (((pRegSet->pc == (INSTR *) taMemProbeByteRead)    ||
         (pRegSet->pc == (INSTR *) taMemProbeByteWrite)   ||
         (pRegSet->pc == (INSTR *) taMemProbeWordRead)    ||
         (pRegSet->pc == (INSTR *) taMemProbeWordWrite)   ||
         (pRegSet->pc == (INSTR *) taMemProbeLongRead)    ||
         (pRegSet->pc == (INSTR *) taMemProbeLongWrite))  &&
         ((vecNum == GENERAL_PAGE_INT)||(vecNum == GENERAL_GP_INT)))
    {
       /* 异常由taMemProbe()函数产生，强制使其PC回到taMemProbeErrorRtn处，使taMemProbe()函数返回 ERROR */
       pRegSet->pc = (INSTR *)taMemProbeErrorRtn;
       pEsf->pc = (INSTR *)taMemProbeErrorRtn;
                                                                       
       return (TRUE);
    }

    /* 异常不是内存探测操作引起的异常，调用其它组件安装的异常HOOK */
    if (oldExcBaseHook != NULL)
    {
       return (oldExcBaseHook(vecNum, pEsf, pRegSet, pExcInfo));
    }

    return (FALSE);
}
#endif
