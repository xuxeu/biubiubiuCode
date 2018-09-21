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
 * @file: strcpy.c
 * @brief:
 *	   <li> 实现字符串模块里的strcpy。</li>
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
 *    从指定的源字符串拷贝所有字符(包括结束符)到目的字符串。
 * @param[in]: src0: 拷贝源起始地址
 * @param[in]: dst0: 拷贝目的起始地址
 * @return: 
 *    拷贝目的起始地址。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes: 
 *    函数中不检查拷贝源和目的起始地址的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.8
 */
char *strcpy(char *dst0, const char *src0)
{
    signed char *dst = dst0;
    const signed char *src = src0;
    long *aligned_dst = NULL;
    const long *aligned_src = NULL;

    /* @KEEP_COMMENT: 设置dst为<dst0>，设置src为<src0> */

    /*
     * @brief:
     *    如果<src0>和<dst0>按字长对齐，则以字长为单位进行拷贝，并将末尾不足字长
     *    的字符以字节为单位进行拷贝，返回<dst0>。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 1 
     */
    /* @REPLACE_BRACKET: src和dst按字对齐 */
    if (0 == UNALIGNED_2(src, dst))
    {
        aligned_dst = (long*)dst;
        aligned_src = (long*)src;

        /* @REPLACE_BRACKET: src当前字长拷贝数据中没有NULL字符 */
        while (0 == DETECTNULL(*aligned_src))
        {
            /*
             * @KEEP_COMMENT: 按字长拷贝src当前数据到dst当前位置，
             * 并按字长往后移动src和dst当前拷贝位置
             */
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;
        }

        /* @KEEP_COMMENT: 保存src和dst的当前拷贝位置 */
        dst = (signed char*)aligned_dst;
        src = (const signed char*)aligned_src;
    }

    /*
     * @brief:
     *    如果<src0>或<dst0>不按字长对齐，则以字节为单位进行拷贝，返回<dst0>。
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 2 
     */
    /* @REPLACE_BRACKET: src当前字节拷贝数据不是结束符 */
    while ((*src) != '\0')
    {
        /*
         * @KEEP_COMMENT: 以字节为单位，拷贝src当前数据到dst当前位置，
         * 并按字节往后移动dst和src当前拷贝位置
         */
        *dst = *src;
        dst++;
        src++;
    }

    /* @KEEP_COMMENT: 设置dst当前字节拷贝数据为'\0' */
    *dst = '\0';

    /* @REPLACE_BRACKET: <dst0> */
    return(dst0);
}

