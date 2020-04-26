/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2011-09-07         胡雪雷，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:strtoi.c
 * @brief:
 *             <li>MIPS体系统结构下的一些命令实现</li>
 * @implements:DTA.3.84 DTA.3.85
 */

/* @<MODULE */

/************************头 文 件******************************/

/* @<MOD_HEAD */

#include <string.h>

/* @MOD_HEAD> */

/************************宏 定 义******************************/
/************************类型定义******************************/
/************************外部声明******************************/
/************************前向声明******************************/
/************************模块变量******************************/
/************************全局变量******************************/
/************************实   现*******************************/

/* @MODULE> */


/*
 * @brief:
 *     将字符串转换成整数，uwBase为0时，转换进制由字符串决定。
 * @param[in]:   ubpNptr: 输入字符串
 * @param[in]:   uwBase: 转换的进制
 * @return:
 *     转换后的整数
 *     MIN_INT32: 溢出，比所能表示的最小整数还小
 *     MAX_INT32: 溢出，比所能表示的最大整数还大
 * @tracedREQ: RTA.3
 * @implements: DTA.3.98
 */
INT32 strtoi(const UINT8 *ubpNptr, UINT32 uwBase)
{
	#define MIN_INT32    	(0x80000000)
	#define MAX_INT32    	(0x7fffffff)
    register const UINT8 *s = ubpNptr;
    register INT32 acc = 0;
    register INT32 c = 0;
    register UINT32 cutoff = 0;
    register INT32 neg = 0;
    register INT32 any = 0;
    register INT32 cutlim = 0;

    do
    {
        /* 跳过空格符 */
        /* @KEEP_COMMENT: 记录s指向<ubpNptr>指向的缓冲，记录c为s指向的字符，s自加1指向下一个字符 */
        c = *s;
        s++;
        /* @REPLACE_BRACKET: c等于' ' */
    } while (' '== c);

    /* @REPLACE_BRACKET: c等于'-' */
    if (c == '-')
    {
        /* @KEEP_COMMENT: 设置符号标志符为1，设置c值为s指向的字符，s自加1指向下一个字符 */
        neg = 1;
        c = *s;
        s++;
    }

    /* @REPLACE_BRACKET: c等于'+' */
    if (c == '+')
    {
        /* @KEEP_COMMENT: 遇到'+'符号标志位不变，设置c值为s指向的字符，s自加1指向下一个字符 */
        c = *s;
        s++;
    }

    /* @KEEP_COMMENT: 确定转换的进制 */

    /* @REPLACE_BRACKET: <uwBase>等于0或者16 && c等于'0' && s等于'x'或者'X' */
    if (( (uwBase == 0) || (uwBase == 16)) && (c == '0') && ((*s == 'x') || (*s == 'X')))
    {
        /* @KEEP_COMMENT: 设置c值为s+1指向的字符，s自加2设置<uwBase>值为16，表示其为16进制 */
        c = s[1];
        s += 2;
        uwBase = 16;
    }

    /* @REPLACE_BRACKET: <uwBase>等于0 */
    if (uwBase == 0)
    {
        /* @KEEP_COMMENT: 如果c等于'0'，设置<uwBase>为8，否则设置<uwBase>为10 */
        uwBase = (c == '0' ? 8 : 10);
    }

    /* 
     * @KEEP_COMMENT: 定义溢出数cutoff，如果待转换数为正数，则设置溢出数cutoff为0x7FFFFFFF，
     * 否则设置溢出数cutoff为0x80000000，并将cutoff减小<uwBase>倍，
     * 记录溢出数cutoff除以<uwBase>的余数为cutlim。
     */
    cutoff = (neg != 0) ? -(UINT32)MIN_INT32 : MAX_INT32;
    cutlim = (INT32)(cutoff % uwBase);
    cutoff /= uwBase;
    acc = 0;
    any = 0;

    /* @KEEP_COMMENT: 根据进制将字符串转换为相应的整数 */
    /* @REPLACE_BRACKET: 转换后的数acc赋值为0;;设置c值为s指向的字符，s自加1 */
    for (;; )
    {
        /* @REPLACE_BRACKET: c的值在'0'到'9'之间 */
        if ((c >= '0') && (c <= '9'))
        {
            /* @KEEP_COMMENT: 设置c值为c减去'0' */
            c -= '0';
        }
        else
        {
            /* @REPLACE_BRACKET: c的值在'a'到'z'之间 */
            if ((c >= 'a') && (c<= 'z'))
            {
                /* @KEEP_COMMENT: 设置c值为c减去'a'加10 */
                c -= (INT32)('a' - 10);
            }
            else
            {
                /* @REPLACE_BRACKET: c的值在'A'到'Z'之间 */
                if ((c >= 'A') && (c<= 'Z'))
                {
                    /* @KEEP_COMMENT: 设置c值为c减去'A'加10 */
                    c -= (INT32)('A' - 10);
                }
                else
                {
                    break;
                }
            }
        }

        /* 转换后的整数大于进制的基数则跳出 */
        
        /* @REPLACE_BRACKET: 转换后c值如果小于0或者大于进制基数<uwBase> */
        if ((c < 0) || (c >= (INT32)uwBase))
        {
            break;
        }

        /* 如果转换后的整数溢出则设置any为-1 */
        /* @REPLACE_BRACKET: 已经出现溢出 || 转换后的数acc大于cutoff || (转换后的数acc等于cutoff并且c大于cutlim) */
        if ((any < 0) || (acc > (INT32)cutoff) || ((acc == (INT32)cutoff) && (c > cutlim)))
        {
            /* 
             * 判断条件acc == cutoff && c > cutlim 是因为在前面将cutoff缩小了base倍，
             * cutlim为余数即要使 acc * uwBase + c 小于溢出数cutoff
             */
            /* @KEEP_COMMENT: 设置溢出标志，表示已经溢出 */
            any = -1;
        }
        else
        {
            /* @KEEP_COMMENT: 根据进制的基数累加每次转换后的数字，acc扩大<uwBase>倍，再加上当前值c */
            any = 1;
            acc *= (INT32)uwBase;
            acc += c;
        }
        c = *s;
        s++;
    }

    /* any小于0表示转换后的整数溢出，根据符号返回相应的宏值 */
    /* @REPLACE_BRACKET: 转换后的整数溢出 */
    if (any < 0)
    {
        /* 
         * @KEEP_COMMENT: 定义溢出数cutoff，如果待转换数为正数，转换后数acc为0x7FFFFFFF，
         * 否则设置转换后数acc为0x80000000
         */
        acc = ((neg != 0)? MIN_INT32 : MAX_INT32);
    }
    else
    {
        /* @REPLACE_BRACKET: 待转换数是负数 */
        if (0 != neg)
        {
            /* @KEEP_COMMENT: 将待转换数取反 */
            acc = -acc;
        }
    }

    /* @REPLACE_BRACKET: 转换后的整数acc */
    return (acc);
}
