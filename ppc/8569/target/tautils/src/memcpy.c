/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2011-02-23         李燕，北京科银京成技术有限公司
 *                               创建该文件。
 * 2011-04-15	  张勇，北京科银京成技术有限公司
 * 				   增加PDL描述、整理代码格式。
*/

/*
 * @file: memcpy.c
 * @brief:
 *	   <li> 实现字符串模块里的memcpy。</li>
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
 *    从源区域拷贝指定长度字节到目的区域。
 * @param[in]: src0: 拷贝源起始地址
 * @param[in]: len0: 拷贝长度
 * @param[out]: dst0: 拷贝目的起始地址
 * @return: 
 *    拷贝目的起始地址。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes: 
 *    函数中不检查拷贝源和目的起始地址、拷贝长度的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.2
 */
void *memcpy(void *dst0, const void *src0, size_t len0)
{
    signed char *dst = dst0;
    const signed char *src = src0;
    long *aligned_dst = NULL;
    const long *aligned_src = NULL;
    int   len =  (int)len0;
    unsigned int LongTypeSize = 0;

    /*
     * @brief:
     *    如果<src0>和<dst0>按字长对齐，并且<len0>大于等于字长时，按以下步骤进行
     *    操作:<br>
     *    (1)<len0>大于等于4倍字长的字符按4倍字长进行拷贝；<br>
     *    (2)大于等于字长但小于4倍字长的字符按字长进行拷贝；<br>
     *    (3)小于字长的字符以字节为单位进行拷贝；<br>
     *    (4)返回<dst0>。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 1
     */
    /* @KEEP_COMMENT: 设置拷贝目的地址det为<dst0>，设置拷贝源起始地址src为<src0> */
#ifndef __MIPS__
    /* @REPLACE_BRACKET: <len0>大于等于字长 && src与dst按字对齐 */
    if ((0 == TOO_SMALL(len)) && (0 == UNALIGNED_2(src, dst)))
    {
        aligned_dst = (long*)dst;
        aligned_src = (long*)src;

        LongTypeSize = (unsigned int)LBLOCKSIZE;

        /* @REPLACE_BRACKET: <len0>大于等于4倍字长 */
        while (len >= (LongTypeSize << 2))
        {
            /*
             * @KEEP_COMMENT: 按4倍字长长度拷贝src当前数据到dst当前位置，
             * 并按4倍字长长度往后移动src和dst当前拷贝位置
             */
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;

            /* @KEEP_COMMENT: 设置<len0>为<len0>减去4倍字长 */
            len -= (LongTypeSize<<2);
        }

        /* @REPLACE_BRACKET: <len0>大于等于字长 */
        while (len >= LITTLEBLOCKSIZE)
        {
            /*
             * @KEEP_COMMENT: 按字长长度拷贝src当前数据到dst当前位置，
             * 并按字长长度往后移动src和dst当前拷贝位置
             */
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;

            /* @KEEP_COMMENT: 设置<len0>为<len0>减去字长 */
            len -= LITTLEBLOCKSIZE;
        } 

        /* @KEEP_COMMENT: 保存src和dst的当前拷贝位置 */
        dst = (signed char*)aligned_dst;
        src = (signed char*)aligned_src;
    }
#endif
    /*
     * @brief:
     *    如果<src0>或者<dst0>不按字长对齐，或者<len0>小于字长时，以字节为单位进
     *    行拷贝，返回<dst0>。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 2 
     */
    /* @REPLACE_BRACKET: <len0>不等于0 */
    while (0 != len)
    {
        /* @KEEP_COMMENT: <len0>-- */
        len--;

        /*
         * @KEEP_COMMENT: 以字节为单位，拷贝src当前数据到dst当前位置
         * 并按字节将src和dst从当前拷贝位置往后移动
         */
        *dst = *src;
        dst++;
        src++;
    }

    /* @REPLACE_BRACKET: dst0 */
    return(dst0);
}
