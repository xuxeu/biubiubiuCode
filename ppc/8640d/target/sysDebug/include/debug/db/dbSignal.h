/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:dbSignal.h
 *@brief:
 *             <li>调试管理信号定义</li>
 */
#ifndef _DB_SIGNAL_H
#define _DB_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************头文件********************************/

/************************宏定义********************************/

/*非法的信号*/
#define SIGNAL_INVAILD     (0xFFFFFFFF)

/* 定义RSP标准信号 */
#define SIGNAL_NON       0       /* nothing*/
#define SIGNAL_HUP       1       /* hangup */
#define SIGNAL_INT       2       /* interrupt */
#define SIGNAL_QUIT      3       /* quit */
#define SIGNAL_ILL       4       /* illegal instruction (not reset when caught) */
#define SIGNAL_TRAP      5       /* trace trap (not reset when caught) */
#define SIGNAL_IOT       6       /* IOT instruction */
#define SIGNAL_ABRT      6       /* used by abort, replace SIGIOT in the future */
#define SIGNAL_EMT       7       /* EMT instruction */
#define SIGNAL_FPE       8       /* floating point exception */
#define SIGNAL_KILL      9       /* kill (cannot be caught or ignored) */
#define SIGNAL_BUS       10      /* bus error */
#define SIGNAL_SEGV      11      /* segmentation violation */
#define SIGNAL_SYS       12      /* bad argument to system call */
#define SIGNAL_PIPE      13      /* write on a pipe with no one to read it */
#define SIGNAL_ALRM      14      /* alarm clock */
#define SIGNAL_TERM      15      /* software termination signal from kill */

/************************类型定义******************************/

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _DB_SIGNAL_H */
