/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史:
 * 2011-02-23  张勇，北京科银京成技术有限公司
 *                   创建该文件。
 * 2011-04-15  张勇，北京科银京成技术有限公司
 * 				     增加PDL描述、优化代码、修改GBJ5369违反项。
 */

/*
 * @file: modf.c
 * @brief:
 *	   <li> modf数学函数实现文件。</li>
 * @implements: DR.2
 */

/* @<MODULE */ 

/************************头 文 件******************************/

/* @<MOD_HEAD */
#include <mathBase.h>
#include <math.h>
/* @MOD_HEAD> */

/************************宏 定 义******************************/
/************************类型定义******************************/
/************************外部声明******************************/
/************************前向声明******************************/
/************************模块变量******************************/
/************************全局变量******************************/
/************************实    现******************************/

/* @MODULE> */ 


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
 * @notes: 
 *    把一个双精度浮点数拆分为整数和小数部分。<br>
 *    保存整数部分在<pIntPart>，返回小数部分。<br>
 *    两部分都是双精度浮点数，符号随<value>。<br>
 *    ZERO、NaN、INF的数学用语参考RR.2注释。<br>
 * @implements: DR.2.16
 */
double modf(double value, double *pIntPart)
{
    _DOUBLE     dat = {0.0};
    FAST int    exp_f = 0;
    FAST unsigned int exp_t = 0;
    FAST double fracPart = 0.0;
    FAST double intPart = 0.0;
    int negflag = (int)(value < 0.0);
    unsigned long t = 0;
    unsigned long tt = (unsigned long)-1;

    /* @REPLACE_BRACKET: negflag != 0 */
    if (negflag != 0)
    {
        /* @KEEP_COMMENT: 计算<value>的负值保存至<value> */
        value = -value;
    }

    /* @KEEP_COMMENT: 转换<value>为整数表现格式 */
    dat.ddat = value;

    /* @KEEP_COMMENT: 获取<value>指数并转换为2^n的指数格式保存至exp_f */
    exp_f = (int)(((unsigned long)(dat.ldat.l1) & (unsigned long)0x7ff00000) >> 20) - 1023;

    /*
     * @brief:
     *    (1)<value>为纯小数(指数部分小于0)，返回<value>并且pIntPart等于0。<br>
     *    (2)<value>为极限整数(指数部分在(52, +INF))，返回0并且pIntPart等于<value>。<br>
     *    (3)<value>指数部分在[0, 20]，返回sign(value)*INT(|value|)，并且pIntPart等<br>
     *       于sign(value)*[|value|-INT(|value|)]。<br>
     *    (4)<value>指数部分在(20, 52]，返回sign(value)*INT(|value|)，并且pIntPart等<br>
     *       于sign(value)*[|value|-INT(|value|)]。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @notes: 
     *    <value>指数部分由公式(((value的前四个字节)&0x7ff00000)>>20)-1023得到。<br>
     *    sign(value)获取<value>的符号位:value大于等于0时，sign(value)=1；<br>
     *                                   value小于0时，sign(value)=-1。<br>
     *    INT(x)获取x的整数部分。<br>
     * @implements: 1
     */
    /* @REPLACE_BRACKET: exp_f <= -1 */
    if (exp_f <= -1)
    {
        /* @KEEP_COMMENT: 设置fracPart的值为<value>，设置intPart的值为0 */
        fracPart = value;
        intPart = 0.;
    }
    else
    {
        /* 指数为正数 */
        /* 清除value的指数部分 */
        /* @REPLACE_BRACKET: exp_f <= 20 */
        if (exp_f <= 20)
        {
            /* @KEEP_COMMENT: 设置exp_t值为无符号的整型exp_f */
            /* @KEEP_COMMENT: 设置t值为无符号的长型dat.ldat.l1 */
            exp_t = (unsigned int)exp_f;
            t = (unsigned long)dat.ldat.l1;

            /* @KEEP_COMMENT: 设置t值为t&(tt<<(20-exp_t)) */
            t &= (tt<<(unsigned long)(20-exp_t));
            /* @KEEP_COMMENT: 设置dat.ldat.l1值为长型t，设置dat.ldat.l2值为0 */
            dat.ldat.l1 = (long)t;
            dat.ldat.l2 = 0;

            /* @KEEP_COMMENT: 设置intPart值为dat.ddat */
            intPart = dat.ddat;
            /* @KEEP_COMMENT: 计算value - intPart保存至fracPart */
            fracPart = value - intPart;
        }
        else
        {
            /* @REPLACE_BRACKET: exp_f <= 52 */
            if (exp_f <= 52)
            {
                /* @KEEP_COMMENT: 设置exp_t值为(unsigned int)exp_f */
                /* @KEEP_COMMENT: 设置t值为(unsigned long)dat.ldat.l2 */
                /* @KEEP_COMMENT: 设置t值为t&(tt<<(52-exp_t)) */
                exp_t = (unsigned int)exp_f;
                t = (unsigned long)dat.ldat.l2;
                t &= (tt<<(unsigned long)(52-exp_t));

                /* @KEEP_COMMENT: 设置dat.ldat.l2值为(long)t，设置intPart值为dat.ddat */
                dat.ldat.l2 = (long)t;
                intPart = dat.ddat;
                /* @KEEP_COMMENT: 计算value-intPart保存至fracPart */
                fracPart = value-intPart;
            }
            else
            {
                /* @KEEP_COMMENT: 设置fracPart值为0.0，设置intPart值为value */
                fracPart = 0.;
                intPart = value;
            }
        }
    }

    /*
     * @KEEP_COMMENT: 如果negflag != 0，设置*pIntPart = -intPart；
     * 如果negflag = 0，设置*pIntPart=intPart
     */
    *pIntPart = ((negflag != 0)?-intPart:intPart);

    /* @REPLACE_BRACKET: 如果negflag != 0，返回-fracPart；否则返回-fracPart */
    return((negflag != 0)?-fracPart:fracPart);
}

