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
 * @file:taskBpIgnoreLib.h
 * @brief:
 *             <li>断点忽略相关宏定义和函数声明</li>
 */
#ifndef _SDA_BP_IGNORE_LIB_H_
#define _SDA_BP_IGNORE_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头 文 件******************************/
#include "taTypes.h"
#include "ta.h"

/************************类型定义*****************************/

/************************宏 定 义******************************/

/************************接口声明*****************************/
/*
 * @brief:
 *     获取忽略标志
 * @return:
 *     忽略标志的值
 */
BOOL taSdaDoIgnoreGet();

/*
 * @brief:
 *     置位忽略标志
 * @return:
 *     无
 */
void taSdaDoIgnoreSet();

/*
 * @brief:
 *     清除忽略标志
 * @return:
 *     无
 */
void taSdaDoIgnoreClr();


/*
 * @brief:
 *     通过单步跳过，忽略当前断点
 * @return:
 *     TA_OK：断点忽略成功
 */
T_TA_ReturnCode taSdaBpDoIgnore(T_DB_ExceptionContext *context);

/*
 * @brief:
 *     完成"taTaskBpDoIgnore"函数的断点跳过功能
 * @return:
 *     TA_OK：完成断点忽略
 */
T_TA_ReturnCode taSdaBpDoneIgnore(T_DB_ExceptionContext *context);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_BP_IGNORE_LIB_H_ */
