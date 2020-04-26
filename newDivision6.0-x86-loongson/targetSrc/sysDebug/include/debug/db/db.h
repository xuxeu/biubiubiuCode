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
 *@file:db.h
 *@brief:
 *             <li>基本调试模块向外提供的头文件</li>
 */
#ifndef _DB_H
#define _DB_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"
#include "dbStandardRsp.h"
#include "dbBreakpoint.h"
#include "dbSignal.h"
#include "dbContext.h"
#include "dbVector.h"
#include "dbAtom.h"
#include "dbAtomic.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************接口声明******************************/

/*
 * @brief:
 *     基本调试初始化
 * @return:
 *     TRUE: 初始化成功
 *     FALSE: 初始化失败
 */
BOOL taDBInit(void);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif  /*_DB_H*/
