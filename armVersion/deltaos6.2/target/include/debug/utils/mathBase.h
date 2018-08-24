/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2011-02-23         ���࣬�����������ɼ������޹�˾
 *                               �������ļ���
 * 2011-04-15	  ���£������������ɼ������޹�˾
 * 				  ��������ʽ���Ż����롣
*/

/*
 * @file  mathBase.h
 * @brief:
 *       <li>�ṩ��ѧ�������صĺ궨�塢���Ͷ���͸�������������<li>
 * @implements: DR.2
 */

#ifndef _MATHBASE_H
#define _MATHBASE_H

#ifdef __cplusplus
extern "C" {
#endif

/************************ͷ�ļ�********************************/
#include <mathTypesOld.h>

/************************�궨��********************************/

#define FAST	register

#define isnan(num)      (NAN == (fpTypeGet ((num), 0)))
#define iszero(num)     (ZERO == (fpTypeGet ((num), 0)))
#define isinf(num)      (INF == (fpTypeGet ((num), 0)))

/************************���Ͷ���******************************/
/* ˫����ת������ */
typedef union
{
    double p_double;
    unsigned p_mant[2];
} __cv_type;

/* ˫�������ݽṹ */
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

/************************�ӿ�����******************************/
/*
 * @brief: 
 *    �����ƫ�Ƶ�ָ��ֵ��
 * @param[in]: x: ����
 * @param[in]: c: ����
 * @return: 
 *    ���: c << x  ���fl(x+c)=x.
 *    (c��x��������)
 *    exp__E ����
 *                    / exp(x+c) - 1 - x ,  1E-19 < |x| < 0.3465736
 *    exp__E(x,c) =  |
 *                    \ 0                ,  |x| < 1E-19.
 * @tracedREQ: RR.2
 * @implements: DR.2.23
 */
double exp__E(double x, double c);

/*
 * @brief: 
 *    exp(x)-1�Ŀ�ѧ���ʽ��
 * @param[in]: x: ����
 * @return: 
 *    ˫��������
 *    +INF : xΪ+INF
 *    -1 : xΪ-INF
 *    NaN : xΪNaN
 * @tracedREQ: RR.2
 * @implements: DR.2.24
 */
double expm1(double x);

/*
 * @brief: 
 *    ��Ȼ������̩��չ����
 * @param[in]: z: ����
 * @return: 
 *    ˫�������ݡ�
 * @tracedREQ: RR.2
 * @implements: DR.2.25
 */
double log__L(double z);

/*
 * @brief: 
 *    ����<x>����2^<N>��
 * @param[in]: x: ����
 * @param[in]: N: ��������
 * @return: 
 *    ˫�������ݡ�
 * @tracedREQ: RR.2
 * @implements: DR.2.26
 */
double scaleb(double x, int N);

/*
 * @brief: 
 *    ����<y>�ķ��ŵ�<x>��
 * @param[in]: x: ����
 * @param[in]: y: ����
 * @return: 
 *    ����ȡy���ŵ�x��ֵ
 * @tracedREQ: RR.2
 * @implements: DR.2.27
 */
double copysign(double x, double y);

/*
 * @brief: 
 *    ����x�Ĳ�ƫ(unbias)ָ��(x��[2^n,2^(n+1))ʱ������n)��
 * @param[in]: x: ����
 * @return: 
 *    ˫�������ݡ�
 *    -INF : xΪ0��
 *    INF : xΪINF��
 *    NaN : xΪNaN.
 * @tracedREQ: RR.2
 * @implements: DR.2.28
 */
double logb(double x);

/*
 * @brief: 
 *    �жϲ����Ƿ�Ϊ��������
 * @param[in]: x: ����
 * @return: 
 *    0:xΪINF��NAN��
 *    1:xΪ��������
 * @tracedREQ: RR.2
 * @implements: DR.2.29
 */
int finite(double x);

/*
 * @brief: 
 *    ����x REM y�Ĺ���������
 * @param[in]: x: ����
 * @param[in]: p: ����
 * @return:
 *    ����x REM y  =  x - [x/y]*y ,[x/y]�����x/y�����������[x/y]����������
 *    ��ֵ��ȣ���ȡż������ֵ��Ϊ[-y/2,y/2]��
 * @tracedREQ: RR.2
 * @implements: DR.2.30
 */
double drem(double x, double p);

/*
 * @brief: 
 *    ��ȡ������<v>���������͡�
 * @param[in]: x: ����
 * @param[out]: r: ָ����������ڱ���<v>����������
 * @return: 
 *    0.0:<v>Ϊ0��
 *    INF:<v>ΪINF��
 *    NAN:<v>ΪNAN��
 *    REAL:<v>Ϊ��������
 *    INTEGER:<v>Ϊ������
 * @tracedREQ: RR.2
 * @implements: DR.2.31
 */
int fpTypeGet(double v, double *r);

#ifdef __cplusplus
}
#endif

#endif
