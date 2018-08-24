/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2011-02-23         李燕，北京科银京成技术有限公司
 *                               创建该文件。
 * 2011-04-15	  张勇，北京科银京成技术有限公司
 * 				  整理代码格式、优化代码、修改GBJ5369违反项。
*/

/*
 * @file： math.h
 * @brief：
 *	    <li>定义数学库相关的宏定义和接口声明。</li>
 * @implements: DR.2
 */

#ifndef _MATH_H
#define _MATH_H

/************************头文件********************************/
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************宏定义********************************/
#define ZERO 	0
#define REAL 	1
#define INTEGER 2
#define INF 	3
#define NAN 	4


/************************类型定义******************************/
/************************接口声明******************************/


/**
 * @req
 * @brief: 
 *    切分浮点数。
 * @param[in]: value: 被切分的浮点数
 * @param[out]: pIntPart: 保存value的整数部分
 * @return: 
 *    返回小数部分，符号与参数<value>相同。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @implements: DR.2.16
 */
double tamodf(double value, double *pIntPart);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

