/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2011-02-23         李燕，北京科银京成技术有限公司
 *                               创建该文件。
 * 2011-04-15	  张勇，北京科银京成技术有限公司
 * 				   增加PDL描述、整理代码格式、修改GBJ5369违反项。
*/

/*
 * @file: strchr.c
 * @brief:
 *	   <li> 实现字符串模块里的strchr。</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/************************头 文 件******************************/

/* @<MOD_HEAD */
#include <string.h>
/* @MOD_HEAD> */

/************************宏 定 义******************************/

/* 如果X包含关键字字符，则返回非0 */
#define DETECTCHAR(X,MASK) (DETECTNULL((X) ^ (MASK)))

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
 *    从指定的字符串中查找第一个等于指定字符的字符。
 * @param[in]: s1: 查找源字符串起始地址
 * @param[in]: i: 查找字符
 * @return: 
 *    第一个等于指定字符的字符的地址。
 *    NULL: 在字符串中没有找到指定字符。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes:
 *    函数中不检查源字符串起始地址和查找字符的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.6 
 */
char *strchr(const char *s1, int i)
{
    const unsigned char *s = NULL;
    unsigned char c = (unsigned char)i;
    unsigned long mask = 0, j = 0;
    unsigned long *aligned_addr = NULL;

    s = (const unsigned char *)s1;

    /*
     * @brief:
     *    如果<s1>按字长对齐，则以字长为单位查找等于<i>(转换为字符)的字符，并将
     *    不足字长的字符以字节为单位进行查找，返回查找到的字符地址。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @notes: 
     *    <s1>和<s2>指定的字符串结尾必须有结束符'\0'。
     * @implements: 1
     */
    /* @REPLACE_BRACKET: <s1>按字对齐 */
    if (0 == UNALIGNED(s))
    {
        /* @KEEP_COMMENT: 构建字长查找关键字mask初始为0 */
        mask = 0;

        /* @REPLACE_BRACKET: 变量j = 0; j < 字长; j++ */
        for (j = 0; j < LBLOCKSIZE; j++)
        {
            /* @KEEP_COMMENT: 构建mask为(mask<<8) | <i> */
            mask = (mask<<8)|c;
        }

        aligned_addr = (unsigned long*)s;

        /*
         * @REPLACE_BRACKET: <s1>当前字长查找数据中没有结束符 && <s1>当前字长查找
         * 数据和mask没有字符匹配
         */
        while ((0 == DETECTNULL(*aligned_addr)) && (0 == DETECTCHAR(*aligned_addr, mask)))
        {
            /* @KEEP_COMMENT: 按字长往后移动<s1>当前查找位置 */
            aligned_addr++;
        }

        /* @KEEP_COMMENT: 保存当前查找位置<s1> */
        s = (unsigned char*)aligned_addr;
    }

    /*
     * @brief:
     *    如果<s1>不按字长对齐，则以字节为单位查找等于<i>(转换为字符)的字符，返
     *    回查找到的字符地址。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 2 
     */
    /* 按字节进行查找 */
    /* @REPLACE_BRACKET: <s1>当前字节查找数据不是'\0' && <s1>当前字节查找数据不为<i> */
    while (((*s) != '\0') && ((*s) != c))
    {
        /* @KEEP_COMMENT: 按字节往后移动<s1>当前查找位置 */
        s++;
    }

    /* @REPLACE_BRACKET: <s1>当前查找数据为<i> */
    if (*s == c)
    {
        /* @REPLACE_BRACKET: s当前查找位置 */
        return((char *)s);
    }

    /*
     * @brief:
     *    如果在字符串中没有找到<i>(转换为字符)，则返回NULL。
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 3 
     */
    /* @REPLACE_BRACKET: NULL */
    return(NULL);
}

