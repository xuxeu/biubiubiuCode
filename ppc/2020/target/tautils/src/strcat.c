/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2011-02-23         李燕，北京科银京成技术有限公司
 *                               创建该文件。
 * 2011-04-15	  张勇，北京科银京成技术有限公司
 * 				  增加PDL描述、整理代码格式、修改GBJ5369违反项。
*/

/*
 * @file: strcat.c
 * @brief:
 *	   <li> 实现字符串模块里的strcat。</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/************************头 文 件******************************/

/* @<MOD_HEAD */
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
/* @MOD_HEAD> */

/************************宏 定 义******************************/
/************************类型定义******************************/
/************************外部声明******************************/
/************************前向声明******************************/
/************************模块变量******************************/

static signed char *s = NULL;

/************************全局变量******************************/
/************************实    现******************************/

/* @MODULE> */


/**
 * @req
 * @brief: 
 *    拷贝源字符串的所有字符(包括结束符)到目的字符串尾(从结束符开始覆盖)。
 * @param[in]: s2: 拷贝目的字符串
 * @param[out]: s1: 拷贝源字符串
 * @return: 
 *    拷贝目的字符串起始地址。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes: 
 *    函数中不检查源和目的起始地址的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.5
 */
char *strcat(char *s1, const char *s2)
{
    /* @KEEP_COMMENT: 设置当前拷贝位置s为<s1> */
    s = s1;

    /*
     * @brief:
     *    如果<s1>按字长对齐，则以字长为单位查找<s1>的串尾结束符所在地址，并对末
     *    尾不足字长的字符以字节为单位进行查找。从<s1>的串尾结束符所在地址，将
     *    <s2>指定的字符串拷贝到<s1>中，返回<s1>。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @notes: 
     *    <s1>和<s2>指定的字符串结尾必须有结束符'\0'。
     * @implements: 1
     */
    /* @REPLACE_BRACKET: <s1>按字对齐 */
    if (0 != ALIGNED(s1))
    {
        unsigned long *aligned_s1 = (unsigned long *)s1;

        /* @REPLACE_BRACKET: <s1>字符串中没有NULL字符 */
        while (0 == DETECTNULL(*aligned_s1))
        {
            /* @KEEP_COMMENT: 按字长往后移动<s1>当前拷贝位置 */
            aligned_s1++;
        }

        /* @KEEP_COMMENT: 保存当前拷贝位置<s1> */
        s1 = (char *)aligned_s1;
    }

    /*
     * @brief:
     *    如果<s1>不按字长对齐，则以字节为单位查找<s1>的串尾结束符所在地址。从
     *    <s1>的串尾结束符所在地址，将<s2>指定的字符串拷贝到<s1>中，返回<s1>。
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 2 
     */
    /* @REPLACE_BRACKET: <s1>当前字节拷贝数据不是'\0' */
    while ('\0' != (*s1))
    {
        /* @KEEP_COMMENT: 按字节往后移动s1当前拷贝位置 */
        s1++;
    }

    /*
     * @KEEP_COMMENT: 调用strcpy(DR.1.8)拷贝<s2>的所有字符(包括结束符)到<s1>当前
     * 拷贝位置
     */
    strcpy(s1, s2);

    /* @REPLACE_BRACKET: s */
    return(s);
}
