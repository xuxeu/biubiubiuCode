/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2011-02-23         李燕，北京科银京成技术有限公司
 *                               创建该文件。
 * 2011-04-15	  张勇，北京科银京成技术有限公司
 * 				  整理代码格式、优化代码。
*/

/*
 * @file  mathBase.h
 * @brief:
 *       <li>提供数学库基本相关的宏定义、类型定义和辅助函数的声明<li>
 * @implements: DR.2
 */

#ifndef _MATHBASE_H
#define _MATHBASE_H

#ifdef __cplusplus
extern "C" {
#endif

/************************头文件********************************/
#include <mathTypesOld.h>

/************************宏定义********************************/

#define FAST	register

#define isnan(num)      (NAN == (fpTypeGet ((num), 0)))
#define iszero(num)     (ZERO == (fpTypeGet ((num), 0)))
#define isinf(num)      (INF == (fpTypeGet ((num), 0)))

/************************类型定义******************************/
/* 双精度转换类型 */
typedef union
{
    double p_double;
    unsigned p_mant[2];
} __cv_type;

/* 双精度数据结构 */
typedef union
{
    double ddat;
#ifdef _BIG_ENDIAN_
    struct
    {
        long l1;
        long l2;
    } ldat;
#else
    struct
    {
        long l2;
        long l1;
    } ldat;
#endif
} _DOUBLE;

/************************接口声明******************************/
/*
 * @brief: 
 *    计算带偏移的指数值。
 * @param[in]: x: 参数
 * @param[in]: c: 参数
 * @return: 
 *    如果: c << x  因此fl(x+c)=x.
 *    (c是x的修正项)
 *    exp__E 返回
 *                    / exp(x+c) - 1 - x ,  1E-19 < |x| < 0.3465736
 *    exp__E(x,c) =  |
 *                    \ 0                ,  |x| < 1E-19.
 * @tracedREQ: RR.2
 * @implements: DR.2.23
 */
double exp__E(double x, double c);

/*
 * @brief: 
 *    exp(x)-1的科学表达式。
 * @param[in]: x: 参数
 * @return: 
 *    双精度数据
 *    +INF : x为+INF
 *    -1 : x为-INF
 *    NaN : x为NaN
 * @tracedREQ: RR.2
 * @implements: DR.2.24
 */
double expm1(double x);

/*
 * @brief: 
 *    自然对数的泰勒展开。
 * @param[in]: z: 参数
 * @return: 
 *    双精度数据。
 * @tracedREQ: RR.2
 * @implements: DR.2.25
 */
double log__L(double z);

/*
 * @brief: 
 *    计算<x>乘以2^<N>。
 * @param[in]: x: 参数
 * @param[in]: N: 整数参数
 * @return: 
 *    双精度数据。
 * @tracedREQ: RR.2
 * @implements: DR.2.26
 */
double scaleb(double x, int N);

/*
 * @brief: 
 *    拷贝<y>的符号到<x>。
 * @param[in]: x: 参数
 * @param[in]: y: 参数
 * @return: 
 *    返回取y符号的x的值
 * @tracedREQ: RR.2
 * @implements: DR.2.27
 */
double copysign(double x, double y);

/*
 * @brief: 
 *    计算x的不偏(unbias)指数(x在[2^n,2^(n+1))时，返回n)。
 * @param[in]: x: 参数
 * @return: 
 *    双精度数据。
 *    -INF : x为0；
 *    INF : x为INF；
 *    NaN : x为NaN.
 * @tracedREQ: RR.2
 * @implements: DR.2.28
 */
double logb(double x);

/*
 * @brief: 
 *    判断参数是否为有限数。
 * @param[in]: x: 参数
 * @return: 
 *    0:x为INF或NAN；
 *    1:x为有限数。
 * @tracedREQ: RR.2
 * @implements: DR.2.29
 */
int finite(double x);

/*
 * @brief: 
 *    计算x REM y的规整余数。
 * @param[in]: x: 参数
 * @param[in]: p: 参数
 * @return:
 *    返回x REM y  =  x - [x/y]*y ,[x/y]是最靠近x/y的整数，如果[x/y]与两整数的
 *    差值相等，则取偶数。其值域为[-y/2,y/2]。
 * @tracedREQ: RR.2
 * @implements: DR.2.30
 */
double drem(double x, double p);

/*
 * @brief: 
 *    获取浮点数<v>的数据类型。
 * @param[in]: x: 参数
 * @param[out]: r: 指针参数，用于保存<v>的整数部分
 * @return: 
 *    0.0:<v>为0；
 *    INF:<v>为INF；
 *    NAN:<v>为NAN；
 *    REAL:<v>为浮点数；
 *    INTEGER:<v>为整数。
 * @tracedREQ: RR.2
 * @implements: DR.2.31
 */
int fpTypeGet(double v, double *r);

#ifdef __cplusplus
}
#endif

#endif
