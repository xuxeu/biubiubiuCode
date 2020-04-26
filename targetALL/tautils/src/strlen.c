/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2011-02-23         李燕，北京科银京成技术有限公司
 *                               创建该文件。
 * 2011-04-15	  张勇，北京科银京成技术有限公司
 * 				  增加PDL描述、整理代码格式。
*/

/*
 * @file: strlen.c
 * @brief:
 *	   <li> 实现字符串模块里的strlen。</li>
 * @implements: DR.1
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
/************************实    现******************************/

/* @MODULE> */


/**
 * @req
 * @brief: 
 *    返回指定的字符串所含的字符(不包括结束符)数目。
 * @param[in]: str: 字符串起始地址
 * @return: 
 *    字符数目。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes:
 *    函数中不检查字符串起始地址的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.9
 */
size_t strlen(const char *str)
{
    /* @KEEP_COMMENT: 设置起始位置start为<str>，设置当前查找位置end为<str> */
    const signed char *start = str;
    const signed char *end = str;

    unsigned long *aligned_addr = NULL;

    /*
     * @brief:
     *    如果<str>按字长对齐，则按字长计算<str>的字符数目，并将末尾不足字长的字
     *    符以字节为单位计算字符数目，返回<str>的字符数目总和。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 1
     */
    /* @REPLACE_BRACKET: 当前查找位置end按字对齐 */
    if (0 == UNALIGNED(end))
    {
        aligned_addr = (unsigned long*)end;

        /* @REPLACE_BRACKET: 当前查找位置为NULL */
        if (NULL == aligned_addr)
        {
            /* @REPLACE_BRACKET: NULL */
            return(NULL);
        }

        /* @REPLACE_BRACKET: 当前查找位置的数据中没有结束符 */
        while (0 == DETECTNULL(*aligned_addr))
        {
            /* @KEEP_COMMENT: 按字长往后移动end查找位置 */
            aligned_addr++;
        }

        /* @KEEP_COMMENT: 保存当前查找位置end */
        end = (signed char*)aligned_addr;
    }

    /*
     * @brief:
     *    如果<str>不按字长对齐，则以字节为单位计算<str>的字符数目，返回字符数目。
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 2 
     */
    /* @REPLACE_BRACKET: end当前字节查找数据不是'\0' */
    while ((*end) != '\0')
    {
        /* @KEEP_COMMENT: 按字节往后移动end查找位置 */
        end++;
    }

    /* @REPLACE_BRACKET: end-start */
    return(end-start);
}

