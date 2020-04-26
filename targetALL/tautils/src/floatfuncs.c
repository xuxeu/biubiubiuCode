/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/
 
/*
 * 修改历史：
 * 2011-02-23         李燕，北京科银京成技术有限公司
 *                               创建该文件。
 * 2011-04-15	  张勇，北京科银京成技术有限公司
 * 				  增加PDL描述、整理代码格式、优化代码、修改GBJ5369违反项、
  * 				  修改了exponentCvt精度打印不正确的问题。
*/

/*
 * @file: floatfuncs.c
 * @brief:
 *	   <li> 提供浮点数相关的函数支持。</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/************************头 文 件******************************/

/* @<MOD_HEAD */
#include <string.h>
//#include <mathBase.h>
#include <math.h>
#include <stdarg.h>
#include <floatio.h>
/* @MOD_HEAD> */

/************************宏 定 义******************************/
/************************类型定义******************************/
/************************外部声明******************************/

/*
 * @brief: 
 *    将浮点数转换为一个字符数组。
 * @param[in]: number: 浮点数
 * @param[in]: prec: 精确度
 * @param[in]: doAlt: 替代标志
 * @param[in]: fmtch: 期望的格式类型
 * @param[in]: endp: 缓冲区结束地址
 * @param[out]: pDoSign: 符号标志
 * @param[out]: startp: 缓冲区起始地址
 * @return:
 *    转换出来的字符个数。
 * @qualification method: 分析/测试
 * @derived requirement: 否
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.17
 */
static int cvt(double number,
			   int prec,
			   BOOL doAlt,
			   int fmtch,
			   BOOL *pDoSign,
			   signed char *startp,
			   signed char *endp);


/*
 * @brief: 
 *    将幂值转换为ASCII字符串并输出到指定的内存区。
 * @param[in]: e: 幂的值
 * @param[in]: fmtch: 格式字符
 * @param[out]: p: 输出内存区起始地址
 * @return:
 *    指向转换出来的字符串尾的指针
 * @qualification method: 分析/测试
 * @derived requirement: 否
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.19
 */
static signed char *exponentCvt(signed char *p,
								int e,
								int fmtch);


/*
 * @brief: 
 *    根据浮点数的值和要求的格式对浮点数进行转换和取整操作。
 * @param[in]: fract: 浮点数  
 * @param[in]: ch: 数字相对大小的基准值
 * @param[in]: end: 缓冲区结束地址 
 * @param[out]: e: 幂
 * @param[out]: pDoSign: 符号标志
 * @param[out]: start: 缓冲区起始地址
 * @return:
 *    指向转换出来的字符串的指针。
 * @qualification method: 分析/测试
 * @derived requirement: 否
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.18
 */
static signed char *roundCvt(double fract,
							 int *e,
							 signed char *start,
							 signed char *end,
							 signed char ch,
							 BOOL *pDoSign);

/************************前向声明******************************/
/************************模块变量******************************/
/************************全局变量******************************/
/************************实    现******************************/

/* @MODULE> */


/*
 * @brief
 *    将浮点数转换为ASCII字符串，并返回转换出来的字符串长度。
 * @param[in]: pVaList: 参数列表 
 * @param[in]: precision: 精确度
 * @param[in]: doAlt: 替代标志
 * @param[in]: fmtSym: 期望的格式类型
 * @param[in]: pBufEnd: 缓冲区结束地址
 * @param[out]: pDoSign: 符号标志
 * @param[out]: pBufStart: 缓冲区起始地址
 * @return:
 *    转换出来的字符个数。
 * @qualification method: 分析/测试
 * @derived requirement: 否
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.16
 */
int fformat(va_list *pVaList,
int precision,
BOOL doAlt,
int fmtSym,
BOOL *pDoSign,
signed char *pBufStart,
signed char *pBufEnd)
{
    /* @KEEP_COMMENT: 从变参列表<*pVaList>获取double类型的值，存放至变量dbl */
    double dbl = (double) va_arg(*pVaList, double);

    /* @KEEP_COMMENT: 设置<*pDoSign>为FALSE */
    *pDoSign = FALSE;

    /* @REPLACE_BRACKET: dbl为INF */
    if (INF == fpTypeGet(dbl, NULL))
    {
        /* @KEEP_COMMENT: 拷贝"Inf"到<pBufStart> */
        strcpy(pBufStart, "Inf");

        /* @REPLACE_BRACKET: dbl小于0.0 */
        if (dbl < 0.0)
        {
            /* @KEEP_COMMENT: 设置<*pDoSign>为TRUE */
            *pDoSign = TRUE;
        }

        /* @REPLACE_BRACKET: -3 */
        return(-3);
    }

    /* @REPLACE_BRACKET: dbl为NAN */
    if (NAN == fpTypeGet(dbl, NULL))
    {
        /* @KEEP_COMMENT: 拷贝"NaN"到<pBufStart> */
        strcpy(pBufStart, "NaN");

        /* @REPLACE_BRACKET: -3 */
        return(-3);
    }

    /* @KEEP_COMMENT: 调用cvt(DR.1.16)将db1转换为字符串，记录返回值是ret */
    /* @REPLACE_BRACKET: ret */
    return(cvt(dbl, precision, doAlt, fmtSym, pDoSign, pBufStart, pBufEnd));
}

/*
 * @brief: 
 *    将浮点数转换为一个字符数组。
 * @param[in]: number: 浮点数
 * @param[in]: prec: 精确度
 * @param[in]: doAlt: 替代标志
 * @param[in]: fmtch: 期望的格式类型
 * @param[in]: endp: 缓冲区结束地址
 * @param[out]: pDoSign: 符号标志
 * @param[out]: startp: 缓冲区起始地址
 * @return:
 *    转换出来的字符个数。
 * @qualification method: 分析/测试
 * @derived requirement: 否
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.17
 */
static int cvt(double number,
int prec,
BOOL doAlt,
int fmtch,
BOOL *pDoSign,
signed char *startp,
signed char *endp)
{
    signed char *p = NULL;
    signed char *t = NULL;
    signed char *tch = NULL;
    signed char *tmpChrAdd = NULL;
    double fract = 0.0;
    int     dotrim = 0;
    int     expcnt = 0;
    int     gformat = 0;
    int     nonZeroInt = 0;
    double  integer = 0.0;
    double  tmp = 0.0;
    BOOL ZeroSign = FALSE;

    dotrim = 0;
    expcnt = 0;
    gformat = 0;

    /* @REPLACE_BRACKET: <number>小于0.0 */
    if (number < 0.0)
    {
        /* @KEEP_COMMENT: 设置<number>为-<number>，设置<*pDoSign>为TRUE */
        number = -number;
        *pDoSign = TRUE;
    }
    else
    {
        /* @KEEP_COMMENT: 设置<*pDoSign>为FALSE */
        *pDoSign = FALSE;
    }

    /*
     * @KEEP_COMMENT: 将浮点数<number>拆分成整数和小数部分，integer存放整数部分，
     * 返回小数部分存放于fract
     */
    fract = modf(number, &integer);

    /* @REPLACE_BRACKET: integer不等于0 */
    if (integer != 0.0)
    {
        /* @KEEP_COMMENT: 设置标识number为非零的局部变量nonZeroInt为TRUE */
        nonZeroInt = TRUE;
    }

    /* @KEEP_COMMENT: 将<startp>++指针保存至临时变量t */
    startp++;
    t = startp;

    /* 判断整数部分是否等于0 */
    /* @KEEP_COMMENT: 如果integer为0，设置ZeroSign为TRUE；否则设置ZeroSign为FALSE */
    ZeroSign = (0 == integer);

    /*
     * @KEEP_COMMENT: 定义局部变量p作为缓冲区的指针，定义临时变量expcnt记录number
     * 整数部分的位数
     */

    /* @REPLACE_BRACKET: 变量p为<endp>减1; ZeroSign为FALSE; ++expcnt */
    for (p = endp - 1; !ZeroSign; ++expcnt)
    {
        /*
         * @KEEP_COMMENT: 拆分integer/10为整数和小数部分，integer存放整数部分，小
         * 数部分保存至tmp
         */
        tmp  = modf(integer / 10, &integer);

        /* @KEEP_COMMENT: 将(tmp+0.01)*10转换成ASCII字符保存至*p */
        *p = to_char((tmp + .01) * 10);

        /* @KEEP_COMMENT: p-- */
        p--;

        /* @KEEP_COMMENT: 如果integer等于0，设置ZeroSign为TRUE；否则设置ZeroSign为FALSE */
        ZeroSign = (0 == integer);
    }

    /* @REPLACE_BRACKET: <fmtch> */
    switch (fmtch)
    {
        case 'f':
            /* @REPLACE_BRACKET: expcnt不等于0 */
            if (0 != expcnt)
            {
                /* @KEEP_COMMENT: p++，设置tmpChrAdd为<endp> */
                p++;
                tmpChrAdd = endp;

                /* @REPLACE_BRACKET: p小于tmpChrAdd */
                while (p < tmpChrAdd)
                {
                    /* @KEEP_COMMENT: 设置*t为*p，t++，p++ */
                    *t = *p;
                    t++;
                    p++;
                }
            }
            else
            {
                /* @KEEP_COMMENT: 设置*t为'0'，t++ */
                *t = '0';
                t++;
            }

            /* @REPLACE_BRACKET: <prec>不等于0 || <doAlt>不等于0 */
            if ((0 != prec) || (0 != doAlt))
            {
                /* @KEEP_COMMENT: 设置*t为'.'，t++ */
                *t = '.';
                t++;
            }

            /* 如果要求更高的精度 */
            /* @REPLACE_BRACKET: fract不等于0.0 */
            if (0.0 != fract)
            {
                /* @REPLACE_BRACKET: <prec>不等于0.0 */
                if (0 != prec)
                {
                    do 
                    {
                        /*
                         * @KEEP_COMMENT: 拆分fract*10成整数和小数部分，tmp存放整
                         * 数部分，返回小数部分保存至fract
                         */
                        fract = modf(fract * 10, &tmp);

                        /* @KEEP_COMMENT: 将tmp转换成ASCII字符保存至*t */
                        *t = to_char(tmp);

                        /* @KEEP_COMMENT: t++，prec-- */
                        t++;
                        prec--;

                        /* @REPLACE_BRACKET: <prec>不等于0 && fract不等于0.0 */
                    } while ((0 != prec) && (0.0 != fract));
                }

                /* @REPLACE_BRACKET: fract不等于0.0 */
                if (0.0 != fract)
                {
                    /* @KEEP_COMMENT: 设置临时变量tch为t-1 */
                    tch = t-1;

                    /*
                     * @KEEP_COMMENT: 将浮点数fract按格式进行转换和取整操作，
                     * <startp>指向转换后的字符串
                     */
                    startp = roundCvt(fract, NULL, startp, tch, 0x0, pDoSign);
                }
            }

            /* @REPLACE_BRACKET: <prec>不等于0.0 */
            while (prec != 0)
            {
                /* @KEEP_COMMENT: 如果此时没有达到<prec>指定的位数则剩余部分补充0 */
                prec--;
                *t = '0';
                t++;
            }

            break;

        case 'g':
            /* @REPLACE_BRACKET: <prec>等于0 */
            if (0 == prec)
            {
                /* @KEEP_COMMENT: ++<prec> */
                ++prec;
            }

            /*
             * 使用的风格取决于转换出来的值。
             * 如果浮点数小于0.0001或大于精度则使用科学计数方式
             */
            /*
             * @REPLACE_BRACKET: (expcnt为0 && fract不为0.0 && fract小于0.0001)
             *  || expcnt大于<prec> 
             */
            if ((expcnt > prec) || ((0 == expcnt) && (0.0 != fract) && (fract < 0.0001)))
            {
                /*
                 * g/G格式计算有意义的数字而不是指数
                 * 对应e/Eg格式，这会导致一个off-by-one问题，
                 * g/G会将小数点前的数看作有意义的数字，
                 * 而e/E不会把它们计入精度。
                 */
                /* @KEEP_COMMENT: --<prec> */
                --prec;

                /* @KEEP_COMMENT: 设置<fmtch>为<fmtch>减2，设置gformat为1 */
                fmtch  -= 2;
                gformat = 1;
            }
            else
            {
                /* 将整数部分转储到缓冲区的前端。注意将减少精度 */
                /* @REPLACE_BRACKET: expcnt不等于0 */
                if (0 != expcnt)
                {
                    /* @KEEP_COMMENT: p++ */
                    p++;

                    /* @KEEP_COMMENT: 设置tmpChrAdd为<endp> */
                    tmpChrAdd = endp;

                    /* @REPLACE_BRACKET: p小于tmpChrAdd */
                    while (p < tmpChrAdd)
                    {
                        /* @KEEP_COMMENT: 设置*t为*p，t++ */
                        *t = *p;
                        t++;

                        /* @KEEP_COMMENT: --<prec>，p++ */
                        --prec;
                        p++;
                    }
                }
                else
                {
                    /* 用'0'填充 */
                    /* @KEEP_COMMENT: 设置*t为'0'，t++ */
                    *t = '0';
                    t++;
                }

                /*
                 * 如果要求了精度或设置了转换标志，则加入一个小数点
                 * 如果此时尚未整数部分，则在前面添0
                 */
                /* @REPLACE_BRACKET: <prec>不等于0 || <doAlt>不等于0 */
                if ((0 != prec) || (0 != doAlt))
                {
                    /* @KEEP_COMMENT: 设置dotrim为1 */
                    dotrim = 1;

                    /* 添加小数点 */
                    /* @KEEP_COMMENT: 设置*t为'.'，t++ */
                    *t = '.';
                    t++;
                }
                else
                {
                    /* @KEEP_COMMENT: 设置dotrim为0 */
                    dotrim = 0;
                }

                /* 如果要求更高的精度则在末尾添加小数 */
                /* @REPLACE_BRACKET: fract不等于0.0 */
                if (0.0 != fract)
                {
                    /* @REPLACE_BRACKET: <prec>不等于0 */
                    if (0 != prec)
                    {

                        /* 如果整数部分是0，我们不能将开头的0计为有意义的数 */
                        /* @REPLACE_BRACKET: nonZeroInt为FALSE */
                        if (FALSE == nonZeroInt)
                        {
                            do 
                            {
                                /*
                                 * @KEEP_COMMENT: 拆分fract*10成整数和小数部分，
                                 * tmp存放整数部分，小数部分保存至fract
                                 */
                                fract = modf(fract * 10, &tmp);

                                /* @KEEP_COMMENT: 转换tmp为ASCII字符保存至*t */
                                *t = to_char(tmp);

                                /* @KEEP_COMMENT: t++ */
                                t++;

                                /* @REPLACE_BRACKET: tmp等于0.0 */
                            } while (0.0 == tmp);

                            /* @KEEP_COMMENT: <prec>-- */
                            prec--;
                        }

                        /* 添加等于精度数量的数字 */
                        /* @REPLACE_BRACKET: <prec>不等于0 && fract不等于0 */
                        while ((0 != prec) && (0 != fract))
                        {
                            /*
                             * @KEEP_COMMENT: 拆分fract*10成整数和小数部分，tmp存
                             * 放整数部分，小数部分保存至fract
                             */
                            fract = modf(fract * 10, &tmp);

                            /* @KEEP_COMMENT: 转换tmp为ASCII字符保存至*t */
                            *t = to_char(tmp);

                            /* @KEEP_COMMENT: t++，<prec>-- */
                            t++;
                            prec--;
                        }
                    }

                    /* @REPLACE_BRACKET: fract不等于0 */
                    if (0.0 != fract)
                    {
                        /* @KEEP_COMMENT: 设置tch为t-1 */
                        tch = t-1;

                        /*
                         * @KEEP_COMMENT: 将浮点数fract按格式进行转换和取整操作，
                         * startp指向转换后的字符串
                         */
                        startp = roundCvt (fract, NULL, startp, tch, 0x0, pDoSign);
                    }
                }

                /* 转换格式，根据精度要求添加或删除0 */
                /* @REPLACE_BRACKET: <doAlt>不等于0 */
                if (0 != doAlt)
                {
                    /* @REPLACE_BRACKET: <prec>不等于0 */
                    while (prec != 0)
                    {
                        /* @KEEP_COMMENT: <prec>-- */
                        prec--;

                        /* 添加'0' */
                        /* @KEEP_COMMENT: 设置*t为'0'，t++ */
                        *t = '0';
                        t++;
                    }
                }
                else
                {
                    /* @REPLACE_BRACKET: dotrim不等于0 */
                    if (0 != dotrim)
                    {
                        /* @KEEP_COMMENT: t-- */
                        t--;

                        /* @KEEP_COMMENT: 设置tmpChrAdd为<startp> */
                        tmpChrAdd = startp;

                        /* @REPLACE_BRACKET: t大于tmpChrAdd && *t等于'0' */
                        while ((t > tmpChrAdd ) && ('0' == *t))
                        {
                            /* 删除'0' */
                            /* @KEEP_COMMENT: t-- */
                            t--;
                        }

                        /* @REPLACE_BRACKET: *t不等于'.' */
                        if (*t != (signed char)'.')
                        {
                            /* @KEEP_COMMENT: ++t */
                            ++t;
                        }
                    }
                }

                break;
            }

        case 'G':
            /* 精度0被当作1 */
            /* @REPLACE_BRACKET: <prec>等于0 */
            if (0 == prec)
            {
                /* @KEEP_COMMENT: ++<prec> */
                ++prec;
            }

            /*
             * 使用的风格取决于转换出来的值。
             * 如果浮点数小于0.0001或大于精度则使用科学计数方式
             */
            /* @REPLACE_BRACKET: (expcnt>prec)||((0==expcnt)&&(0!=fract)&&(fract<0.0001)) */
            if ((expcnt > prec) || ((0 == expcnt) && (0 != fract) && (fract < .0001)))
            {
                /*
                 * g/G格式计算有意义的数字而不是指数
                 * 对应e/Eg格式，这会导致一个off-by-one问题，
                 * g/G会将小数点前的数看作有意义的数字，
                 * 而e/E不会把它们计入精度。
                 */
                /* @KEEP_COMMENT: --<prec> */
                --prec;

                /* G->E, g->e */
                /* @KEEP_COMMENT: 设置<fmtch>为<fmtch>减2 */
                fmtch  -= 2;

                /* @KEEP_COMMENT: 设置gformat为1 */
                gformat = 1;
            }
            else
            {
                /* 将整数部分转储到缓冲区的前端。注意将减少精度*/
                /* @REPLACE_BRACKET: expcnt不等于0 */
                if (0 != expcnt)
                {
                    /* @KEEP_COMMENT: p++，设置tmpChrAdd为<endp> */
                    p++;
                    tmpChrAdd = endp;

                    /* @REPLACE_BRACKET: p小于tmpChrAdd */
                    while (p < tmpChrAdd)
                    {
                        /* @KEEP_COMMENT: 设置*t为*p，t++ */
                        *t = *p;
                        t++;

                        /* @KEEP_COMMENT: --<prec>，p++ */
                        --prec;
                        p++;
                    }
                }
                else
                {
                    /* 用'0'填充 */
                    /* @KEEP_COMMENT: 设置*t为'0'，t++ */
                    *t = '0';
                    t++;
                }

                /*
                 * 如果要求了精度或设置了转换标志，则加入一个小数点
                 * 如果此时尚未整数部分，则在前面添0
                 */
                /* @REPLACE_BRACKET: <prec>不等于0 || <doAlt>不等于0 */
                if ((0 != prec) || (0 != doAlt))
                {
                    /* 添加小数点 */
                    /* @KEEP_COMMENT: 设置dotrim为1，设置*t为'.'，t++ */
                    dotrim = 1;
                    *t = '.';
                    t++;
                }
                else
                {
                    /* @KEEP_COMMENT: 设置dotrim为0 */
                    dotrim = 0;
                }

                /* 如果要求更高的精度则在末尾添加小数 */
                /* @REPLACE_BRACKET: fract不等于0.0 */
                if (0.0 != fract)
                {
                    /* @REPLACE_BRACKET: <prec>不等于0 */
                    if (0 != prec)
                    {
                        /* 如果整数部分是0，我们不能将开头的0计为有意义的数*/
                        /* @REPLACE_BRACKET: nonZeroInt等于FALSE */
                        if (FALSE == nonZeroInt)
                        {
                            do 
                            {
                                /*
                                 * @KEEP_COMMENT: 拆分fract*10成整数和小数部分，
                                 * tmp存放整数部分，小数部分保存至fract
                                 */
                                fract = modf(fract * 10, &tmp);

                                /* @KEEP_COMMENT: 转换tmp为ASCII字符保存至*t */
                                *t = to_char(tmp);

                                /* @KEEP_COMMENT: t++ */
                                t++;

                                /* @REPLACE_BRACKET: tmp等于0.0 */
                            } while (0.0 == tmp);

                            /* @KEEP_COMMENT: <prec>-- */
                            prec--;
                        }

                        /* 添加等于精度数量的数字*/
                        /* @REPLACE_BRACKET: <prec>不等于0 && fract不等于0 */
                        while ((0 != prec) && (0 != fract))
                        {
                            /*
                             * @KEEP_COMMENT: 拆分fract*10成整数和小数部分，tmp存
                             * 放整数部分，小数部分保存至fract
                             */
                            fract = modf(fract * 10, &tmp);

                            /* @KEEP_COMMENT: 转换tmp为ASCII字符保存至*t */
                            *t = to_char(tmp);

                            /* @KEEP_COMMENT: t++，<prec>-- */
                            t++;
                            prec--;
                        }
                    }

                    /* @REPLACE_BRACKET: fract不等于0 */
                    if (0.0 != fract)
                    {
                        /* @KEEP_COMMENT: 设置tch为t-1 */
                        tch = t-1;

                        /*
                         * @KEEP_COMMENT: 将浮点数fract按格式进行转换和取整操作，
                         * startp指向转换后的字符串
                         */
                        startp = roundCvt (fract, NULL, startp, tch, 0x0, pDoSign);
                    }
                }

                /* 转换格式，根据精度要求添加或删除0 */
                /* @REPLACE_BRACKET: <doAlt>不等于0 */
                if (0 != doAlt)
                {
                    /* @REPLACE_BRACKET: <prec>不等于0 */
                    while (prec != 0)
                    {
                        /* 添加'0' */
                        /* @KEEP_COMMENT: <prec>--，设置*t为'0'，t++ */
                        prec--;
                        *t = '0';
                        t++;
                    }
                }
                else
                {
                    /* @REPLACE_BRACKET: dotrim不等于0 */
                    if (0 != dotrim)
                    {
                        /* @KEEP_COMMENT: t--，设置tmpChrAdd为<startp> */
                        t--;
                        tmpChrAdd = startp;

                        /* @REPLACE_BRACKET: t大于tmpChrAdd && *t等于'0' */
                        while ((t > tmpChrAdd) && ('0' == *t))
                        {
                            /* 删除'0' */
                            /* @KEEP_COMMENT: t-- */
                            t--;
                        }

                        /* @REPLACE_BRACKET: *t不等于'.' */
                        if (*t != (signed char)'.')
                        {
                            /* @KEEP_COMMENT: ++t */
                            ++t;
                        }
                    }
                }

                break;
            }

        case 'e':
            /* @REPLACE_BRACKET: expcnt不等于0 */
            if (0 != expcnt)
            {
                /* 获取整数部分最高位值 */
                /* @KEEP_COMMENT: p++，设置*t为*p，t++ */
                p++;
                *t = *p;
                t++;

                /* @REPLACE_BRACKET: <prec>不等于0 || <doAlt>不等于0 */
                if ((0 != prec) || (0 != doAlt))
                {
                    /* 添加小数点 */
                    /* @KEEP_COMMENT: 设置*t为'.'，t++ */
                    *t = '.';
                    t++;
                }

                /* @KEEP_COMMENT: p++，设置tmpChrAdd为<endp> */
                p++;
                tmpChrAdd = endp;       

                /* 添加精度 */
                /* @REPLACE_BRACKET: <prec>不等于0 && p < tmpChrAdd */
                while ((0 != prec) && (p < tmpChrAdd))
                {
                    /* @KEEP_COMMENT: 设置*t为*p，t++，p++，<prec>-- */
                    *t= *p;
                    t++;
                    p++;
                    prec--;
                }

                /* @KEEP_COMMENT: p++，设置tmpChrAdd为<endp> */
                p++;
                tmpChrAdd = endp;

                /* @REPLACE_BRACKET: <prec>等于0 && p < tmpChrAdd */
                if ((0 == prec) && (p < tmpChrAdd))
                {
                    /* @KEEP_COMMENT: 设置fract为0，设置tch为t-1 */ 
                    fract  = 0;
                    tch = t-1;

                    /*
                     * @KEEP_COMMENT: 将0.0根据浮点数值和要求对其进行转换和取整操
                     * 作，<startp>指向转换后的字符串
                     */
                    startp = roundCvt(0.0, &expcnt, startp, tch, *p, pDoSign);
                }

                /* @KEEP_COMMENT: --expcnt */
                --expcnt;
            }
            else
            {
                /* 增加指数直到第一个小数数字 */
                /* @REPLACE_BRACKET: fract不等于0 */
                if (0.0 != fract)
                {
                    /* 为小数点前的数字调整指数 */
                    /* @KEEP_COMMENT: 设置expcnt为-1 */
                    expcnt = -1;

                    /* @REPLACE_BRACKET: 1 */
                    while (1)
                    {
                        /* 找到浮点数第一个有效数字 */
                        /*
                         * @KEEP_COMMENT: 拆分fract*10为整数和小数部分，tmp存放整
                         * 数部分，小数部分保存至fract
                         */
                        fract = modf(fract * 10, &tmp);

                        /* @REPLACE_BRACKET: tmp不等于0.0 */
                        if (0.0 != tmp)
                        {
                            break;
                        }

                        /* @KEEP_COMMENT: expcnt-- */
                        expcnt--;
                    }

                    /* @KEEP_COMMENT: 转换tmp为ASCII字符保存至*t，t++ */
                    *t = to_char(tmp);
                    t++;

                    /* @REPLACE_BRACKET: <prec>不等于0 || <doAlt>不等于0 */
                    if ((0 != prec) || (0 != doAlt))
                    {
                        /* 添加小数点 */
                        /* @KEEP_COMMENT: 设置*t为'.'，t++ */
                        *t = '.';
                        t++;
                    }
                }
                else
                {
                    /* 如果小数部分等于0.0 */
                    /* @KEEP_COMMENT: 设置*t为'0'，t++ */
                    *t = '0';
                    t++;

                    /* @REPLACE_BRACKET: <prec>不等于0 || <doAlt>不等于0 */
                    if ((0 != prec) || (0 != doAlt))
                    {
                        /* 添加小数点 */
                        /* @KEEP_COMMENT: 设置*t为'.'，t++ */
                        *t = '.';
                        t++;
                    }
                }
            }

            /* 如果需要更多的精度位则向后添加小数位*/
            /* @REPLACE_BRACKET: fract不等于0.0 */
            if (0.0 != fract)
            {
                /* @REPLACE_BRACKET: <prec>不等于0 */
                if (0 != prec)
                {
                    do 
                    {
                        /*
                         * @KEEP_COMMENT: 把浮点数fract*10拆分为整数和小数部分，
                         * tmp存放整数部分值，fract存放小数部分值
                         */
                        fract = modf(fract * 10, &tmp);

                        /* @KEEP_COMMENT: 转换tmp为ASCII字符保存至*t */
                        *t = to_char(tmp);

                        /* @KEEP_COMMENT: t++，<prec>-- */
                        t++;
                        prec--;

                        /* @REPLACE_BRACKET: <prec>不等于0 && fract不等于0 */
                    } while ((0 != prec) && (0 != fract));
                }

                /* @REPLACE_BRACKET: fract不等于0.0 */
                if (0.0 != fract)
                {
                    /* @KEEP_COMMENT: 设置tch为t-1 */
                    tch = t-1;

                    /*
                     * @KEEP_COMMENT: 将fract根据浮点数值和要求对其进行转换和取整
                     * 操作，startp指向转换后的字符串
                     */
                    startp = roundCvt(fract, &expcnt, startp, tch, 0x0, pDoSign);
                }
            }

            /* 此时如果精度位仍不够则在后面填充'0' */
            /* @REPLACE_BRACKET: <prec>不等于0 */
            while (prec != 0)
            {
                /* @KEEP_COMMENT: <prec>--，设置*t为'0'，t++ */
                prec--;
                *t = '0';
                t++;
            }

            /* 删除多余的'0' */
            /* @REPLACE_BRACKET: gformat不等于0 && <doAlt>等于0 */
            if ((0 != gformat) && (0 == doAlt))
            {
                /* @KEEP_COMMENT: t--，设置tmpChrAdd为<startp> */
                t--;
                tmpChrAdd = startp;

                /* @REPLACE_BRACKET: t大于tmpChrAdd && *t等于'0' */
                while ((t > tmpChrAdd) && ('0' == *t))
                {
                    /* @KEEP_COMMENT: t-- */
                    t--;
                }

                /* @REPLACE_BRACKET: *t等于'.' */
                if ('.' == *t)
                {
                    /* @KEEP_COMMENT: --t */
                    --t;
                }
                /* @KEEP_COMMENT: ++t */
                ++t;
            }

            /* 将浮点数转换为科学计数法 */
            /*
             * @KEEP_COMMENT: 将浮点数的幂值expcnt按照<fmtch>指定格式转换为ASCII
             * 字符串并输出到t
             */
            t = exponentCvt(t, expcnt, fmtch);

            break;

        case 'E':
            /* @REPLACE_BRACKET: expcnt不等于0 */
            if (0 != expcnt)
            {
                /* 获取整数部分最高位值 */
                /* @KEEP_COMMENT: p++，设置*t为*p，t++ */
                p++;
                *t = *p;
                t++;

                /* @REPLACE_BRACKET: <prec>不等于0 || <doAlt>不等于0 */
                if ((0 != prec) || (0 != doAlt))
                {
                    /* 添加小数点 */
                    /* @KEEP_COMMENT: 设置*t为'.'，t++ */
                    *t = '.';
                    t++;
                }

                /* 如果要求更高的精度 */
                /* @KEEP_COMMENT: p++，设置tmpChrAdd为<endp> */
                p++;
                tmpChrAdd = endp;

                /* 添加精度 */
                /* @REPLACE_BRACKET: <prec>不等于0 && p小于tmpChrAdd */
                while ((0 != prec) && (p < tmpChrAdd))
                {
                    /* @KEEP_COMMENT: 设置*t为*p，t++，p++，<prec>-- */
                    *t= *p;
                    t++;
                    p++;
                    prec--;
                }

                /* @KEEP_COMMENT: p++，设置tmpChrAdd为<endp> */
                p++;
                tmpChrAdd = endp;

                /* @REPLACE_BRACKET: <prec>等于0 && p小于tmpChrAdd */
                if ((0 == prec) && (p < tmpChrAdd))
                {
                    /* @KEEP_COMMENT: 设置fract为0，设置tch为t-1 */ 
                    fract  = 0;
                    tch = t-1;

                    /*
                     * @KEEP_COMMENT: 将0.0根据浮点数值和要求对其进行转换和取整操
                     * 作，startp指向转换后的字符串
                     */
                    startp = roundCvt(0.0, &expcnt, startp, tch, *p, pDoSign);
                }

                /* @KEEP_COMMENT: --expcnt */
                --expcnt;
            }
            else
            {
                /* 增加指数直到第一个小数数字 */
                /* @REPLACE_BRACKET: fract不等于0 */
                if (0.0 != fract)
                {
                    /* 为小数点前的数字调整指数 */
                    /* @KEEP_COMMENT: 设置expcnt为-1 */
                    expcnt = -1;

                    /* @REPLACE_BRACKET: 1 */
                    while (1)
                    {
                        /* 找到浮点数第一个有效数字 */
                        /*
                         * @KEEP_COMMENT: 拆分fract*10为整数和小数部分，tmp存放整
                         * 数部分，小数部分保存至fract
                         */
                        fract = modf(fract * 10, &tmp);

                        /* @REPLACE_BRACKET: tmp不等于0.0 */
                        if (0.0 != tmp)
                        {
                            break;
                        }

                        /* @KEEP_COMMENT: expcnt-- */
                        expcnt--;
                    }

                    /* @KEEP_COMMENT: 转换tmp为ASCII字符保存至*t，t++ */
                    *t = to_char(tmp);
                    t++;

                    /* @REPLACE_BRACKET: <prec>不等于0 || <doAlt>不等于0 */
                    if ((0 != prec) || (0 != doAlt))
                    {
                        /* 添加小数点 */
                        /* @KEEP_COMMENT: 设置*t为'.'，t++ */
                        *t = '.';
                        t++;
                    }
                }
                else
                {
                    /* 如果小数部分等于0.0 */
                    /* @KEEP_COMMENT: 设置*t为'0'，t++ */
                    *t = '0';
                    t++;

                    /* @REPLACE_BRACKET: <prec>不等于0 || <doAlt>不等于0 */
                    if ((0 != prec) || (0 != doAlt))
                    {
                        /* 添加小数点 */
                        /* @KEEP_COMMENT: 设置*t为'.'，t++ */
                        *t = '.';
                        t++;
                    }
                }
            }

            /* 如果需要更多的精度位则向后添加小数位*/
            /* @REPLACE_BRACKET: fract不等于0.0 */
            if (0.0 != fract)
            {
                /* @REPLACE_BRACKET: prec不等于0 */
                if (0 != prec)
                {
                    /* @COMMENT_LINE: 循环转换各位数为ASCII字符,存放至缓冲区t中 */
                    do 
                    {
                        /*
                         * @KEEP_COMMENT: 拆分fract*10为整数和小数部分，tmp存放整
                         * 数部分，小数部分保存至fract
                         */
                        fract = modf(fract * 10, &tmp);

                        /* @KEEP_COMMENT: 转换tmp为ASCII字符存至*t，t++，<prec>-- */
                        *t = to_char(tmp);
                        t++;
                        prec--;

                        /* @REPLACE_BRACKET: <prec>不等于0 && fract不等于0 */
                    } while ((0 != prec) && (0 != fract));
                }

                /* @REPLACE_BRACKET: fract不等于0.0 */
                if (0 != fract)
                {
                    /* @KEEP_COMMENT: tch = t-1 */
                    tch = t-1;

                    /*
                     * @KEEP_COMMENT: 将fract根据浮点数值和要求对其进行转换和取整
                     * 操作，startp指向转换后的字符串
                     */
                    startp = roundCvt(fract, &expcnt, startp, tch, 0x0, pDoSign);
                }
            }

            /* 此时如果精度位仍不够则在后面填充'0' */
            /* @REPLACE_BRACKET: <prec>不等于0 */
            while (prec != 0)
            {
                /* @KEEP_COMMENT: <prec>--，设置*t为'0'，t++ */
                prec--;
                *t = '0';
                t++;
            }

            /* 删除多余的'0' */
            /* @REPLACE_BRACKET: gformat不等于0 && <doAlt>等于0 */
            if ((0 != gformat) && (0 == doAlt))
            {
                /* @KEEP_COMMENT: t--，设置tmpChrAdd为<startp> */
                t--;
                tmpChrAdd = startp;

                /* @REPLACE_BRACKET: t大于tmpChrAdd && *t等于'0' */
                while ((t > tmpChrAdd) && ('0' == *t))
                {
                    /* @KEEP_COMMENT: t-- */
                    t--;
                }

                /* @REPLACE_BRACKET: *t等于'.' */
                if ('.' == *t)
                {
                    /* @KEEP_COMMENT: --t */
                    --t;
                }

                /* @KEEP_COMMENT: ++t */
                ++t;
            }

            /* 将浮点数转换为科学计数法 */
            /*
             * @KEEP_COMMENT: 调用exponentCvt(DR.1.18)将浮点数的幂值expcnt按照
             * <fmtch>指定格式转换为ASCII字符串并输出到t
             */
            t = exponentCvt(t, expcnt, fmtch);

            break;

        default :
            break;
    }

    /* @KEEP_COMMENT: 设置tmpChrAdd为<startp> */
    tmpChrAdd = startp;

    /* @REPLACE_BRACKET: t-tmpChrAdd */
    return(t-tmpChrAdd);
}

/*
 * @brief: 
 *    根据浮点数的值和要求的格式对浮点数进行转换和取整操作。
 * @param[in]: fract: 浮点数  
 * @param[in]: ch: 数字相对大小的基准值
 * @param[in]: end: 缓冲区结束地址 
 * @param[out]: e: 幂
 * @param[out]: pDoSign: 符号标志
 * @param[out]: start: 缓冲区起始地址
 * @return:
 *    指向转换出来的字符串的指针。
 * @qualification method: 分析/测试
 * @derived requirement: 否
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.18
 */
static signed char *roundCvt(double fract,
int *e,
signed char *start,
signed char *end,
signed char ch,
BOOL *pDoSign)
{
    double tmp = 0.0;

    /* @REPLACE_BRACKET: <fract>不等于0.0 */
    if (0.0 != fract)
    {
        /*
         * @KEEP_COMMENT: 调用modf(DR.2.16)切分浮点数<fract>*10，保存整数部分至临
         * 时变量tmp
         */
        modf(fract*10, &tmp);
    }
    else
    {
        /* @KEEP_COMMENT: 转换<ch>为数字保存至变量tmp */
        tmp = to_digit(ch);
    }

    /* @REPLACE_BRACKET: tmp大于4 */
    if (tmp > 4)
    {
        /* @REPLACE_BRACKET: 1 */
        while (1)
        {
            /* @REPLACE_BRACKET: *end等于'.' */
            if (*end == (signed char)'.')
            {
                /* @KEEP_COMMENT: --end */
                --end;
            }

            /* @KEEP_COMMENT: ++*end */
            ++*end;

            /* @REPLACE_BRACKET: *end小于等于'9' */
            if (*end <= (signed char)'9')
            {
                break;
            }

            /* @KEEP_COMMENT: 设置*end为'0' */
            *end = '0';

            /* @REPLACE_BRACKET: 缓冲区首地址等于缓冲区结尾地址 */
            if (end == start)
            {
                /* @REPLACE_BRACKET: NULL != e */
                if (NULL != e)
                {
                    /* @KEEP_COMMENT: 设置*end为'1'，++*e */
                    *end = '1';
                    ++*e;
                }
                else
                {
                    /* @REPLACE_LINE: --end，设置*end为'1'，--start */
                    --end;
                    *end = '1';
                    --start;
                }

                break;
            }

            end--;
        }
    }
    else
    {
        /* @REPLACE_BRACKET: <*pDoSign>不等于0 */
        if (0 != (*pDoSign))
        {
            /* @REPLACE_BRACKET: 1 */
            while (1)
            {
                /* @REPLACE_BRACKET: <*end>等于'.' */
                if (*end == (signed char)'.')
                {
                    /* @KEEP_COMMENT: --end */
                    --end;
                }

                /* @REPLACE_BRACKET: *end不等于'0' */
                if (*end != (signed char)'0')
                {
                    break;
                }

                /* @REPLACE_BRACKET: 缓冲区首地址等于缓冲区结尾地址 */
                if (end == start)
                {
                    /* @KEEP_COMMENT: 设置<*pDoSign>为FALSE */
                    *pDoSign = FALSE;
                }

                /* @KEEP_COMMENT: --end */
                --end;
            }
        }
    }

    /* @REPLACE_BRACKET: start */
    return(start);
}

static signed char expbuf[MAXEXP] = {0x0};
/*
 * @brief: 
 *    将幂值转换为ASCII字符串并输出到指定的内存区。
 * @param[in]: e: 幂的值
 * @param[in]: fmtch: 格式字符
 * @param[out]: p: 输出内存区起始地址
 * @return:
 *    指向转换出来的字符串尾的指针
 * @qualification method: 分析/测试
 * @derived requirement: 否
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.19
 */
static signed char *exponentCvt(signed char *p,
int e,
int fmtch)
{
    signed char *t = NULL;
    

    /* @KEEP_COMMENT: 设置*p为<fmtch>，p++ */
    *p = (signed char)fmtch;
    p++;

    /* @REPLACE_BRACKET: e小于0 */
    if (e < 0)
    {
        /* @KEEP_COMMENT: 设置e为-e，设置*p为'-'，p++ */
        e = -e;
        *p = '-';
        p++;
    }
    else
    {
        /* @KEEP_COMMENT: 设置*p为'+'，p++ */
        *p = '+';
        p++;
    }

    /*
     * @KEEP_COMMENT: 定义一块存放幂值的内存区expbuf，
     * 其大小为MAXEXP(308)个字节，并定义局部指针t指向该缓冲区的末尾
     */
    t = expbuf+MAXEXP;

    /* @REPLACE_BRACKET: e大于9 */
    if (e > 9)
    {
        /* @KEEP_COMMENT: 循环降幂 */
        do
        {
            /* @KEEP_COMMENT: t-- */
            t--;

            /* @KEEP_COMMENT: 将e除以10的余数转换成ASCII字符保存至*t */
            *t = to_char(e % 10);

            /* @KEEP_COMMENT: 设置e为e/10 */
            e /= 10;

            /* @REPLACE_BRACKET: e除以10的整数大于9 */
        } while (e > 9);

        /* @KEEP_COMMENT: t--，将e转换为ASCII字符保存至*t */
        t--;
        *t = to_char(e);

        /* @REPLACE_BRACKET: t小于expbuf+308 */
        while (t < (expbuf+MAXEXP))
        {
            /* @KEEP_COMMENT: 设置*p为*t，p++，t++， */
			*p = *t;
            p++;
            t++;
        }
    }
    else
    {
        /* @KEEP_COMMENT: 设置*p为'0'，p++，将e转换为ASCII字符保存至*p，p++ */
        *p = '0';
        p++;
        *p = to_char(e);
        p++;
    }

    /* @REPLACE_BRACKET: p */
    return(p);
}

