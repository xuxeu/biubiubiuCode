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
 * @file: memmove.c
 * @brief:
 *	   <li> 实现字符串模块里的memmove。</li>
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
 *    从源区域拷贝指定长度字节到目的区域(带重叠的源区域和目的区域拷贝)。
 * @param[in]: src_void: 拷贝源起始地址
 * @param[in]: length: 拷贝长度
 * @param[out]: dst_void: 拷贝目的起始地址
 * @return: 
 *    拷贝目的起始地址。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes: 
 *    函数中不检查拷贝源和目的起始地址、拷贝长度的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.3
 */
void *memmove(void *dst_void, const void *src_void, size_t length)
{
    signed char *dst = dst_void;
    const signed char *src = src_void;
    long *aligned_dst = NULL;
    const long *aligned_src = NULL;
    int   len =  (int)length;
    unsigned int LongTypeSize = 0;

    /* @KEEP_COMMENT: 设置*dst为目的地址<dst_void>，设置*src为源地址<src_void> */

    /*
     * @brief:
     *    如果拷贝源区域和目的区域重叠，则从<src_void>加<length>的字符地址向
     *    <dst_void>加<length>的字符地址处，以字节为单位并以地址递减的方式进行字
     *    符拷贝，返回<dst_void>。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 1
     */
    /* @REPLACE_BRACKET:(src < dst) && (dst < (src+<length>)) */
    if ((src < dst) && (dst < (src+len)))
    {
        /*
         * @KEEP_COMMENT: src当前拷贝位置往后移动<length>字节到源末尾，
         * dst当前拷贝位置往后移动<length>字节到目的末尾
         */
        src += len;
        dst += len;

        /* @REPLACE_BRACKET: <length>不等于0 */
        while (0 != len)
        {
            /* @KEEP_COMMENT: <length>-- */
            len--;

            /*
             * @KEEP_COMMENT: 按字节拷贝src当前数据到dst当前位置，
             * 并按字节往前移动src和dst的拷贝位置
             */
            dst--;
            src--;
            *dst = *src;
        }
    }
    else
    {
        /*
         * @brief:
         *    当拷贝源区域和目的区域没有重叠时，并且<dst_void>和<src_void>按字长
         *    对齐，并且<length>大于等于字长时，按以下步骤进行操作:<br>
         *	  (1)<length>大于等于4倍字长的字符按4倍字长进行拷贝；<br>
         *	  (2)大于等于字长但小于4倍字长的字符按字长进行拷贝；<br>
         *	  (3)小于字长的字符以字节为单位进行拷贝；<br>
         *	  (4)返回<dst_void>。<br>
         * @qualification method: 测试
         * @derived requirement: 否
         * @implements: 2 
         */
        /* @REPLACE_BRACKET: <length>大于等于字长 && src与dst按字对齐 */
        if ((0 == TOO_SMALL(len)) && (0 == UNALIGNED_2(src, dst)))
        {
            aligned_dst = (long*)dst;
            aligned_src = (long*)src;

            LongTypeSize = (unsigned int)LBLOCKSIZE;

            /* @REPLACE_BRACKET: <length>大于等于4倍字长 */
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

                /* @KEEP_COMMENT: 设置<length>为<length> - 4倍字长 */
                len -= (LongTypeSize << 2);
            }

            /* @REPLACE_BRACKET: <length>大于等于字长 */
            while (len >= LITTLEBLOCKSIZE)
            {
                /*
                 * @KEEP_COMMENT: 按字长长度拷贝src当前数据到dst当前位置，
                 * 并按字长长度往后移动src和dst当前拷贝位置
                 */
                *aligned_dst = *aligned_src;
                aligned_dst++;
                aligned_src++;

                /* @KEEP_COMMENT: 设置<length>为<length> - 字长 */
                len -= LITTLEBLOCKSIZE;
            }

            /* @KEEP_COMMENT: 保存src和dst的当前拷贝位置 */
            dst = (signed char*)aligned_dst;
            src = (signed char*)aligned_src;
        }

        /*
         * @brief:
         *    当拷贝源区域和目的区域没有重叠时，并且<dst_void>或者<src_void>不按
         *    字长对齐，或者<length>小于字长时，以字节为单位进行拷贝，返回
         *    <dst_void>。<br>
         * @qualification method: 测试
         * @derived requirement: 否
         * @implements: 3 
         */
        /* @REPLACE_BRACKET: <length>不等于0 */
        while (0 != len)
        {
            /* @KEEP_COMMENT: <length>-- */
            len--;

            /*
             * @KEEP_COMMENT: 以字节为单位，拷贝src当前数据到dst当前位置，
             * 并按字节往后移动src和dst当前拷贝位置
             */
            *dst = *src;
            dst++;
            src++;
        }
    }

    /* @REPLACE_BRACKET: <dst_void> */
    return(dst_void);
}

