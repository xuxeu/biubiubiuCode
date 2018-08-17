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
 * @file: memchr.c
 * @brief:
 *	   <li> 实现字符串模块里的memchr。</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/************************头 文 件******************************/

/* @<MOD_HEAD */
#include <string.h>
#include <stdarg.h>
#include <floatio.h>
/* @MOD_HEAD> */

/************************宏 定 义******************************/
/************************类型定义******************************/
/************************外部声明******************************/
/************************前向声明******************************/
/************************模块变量******************************/
/************************全局变量******************************/
/************************实    现******************************/

/* @MODULE> */


/*
 * @brief: 
 *    按指定个数在指定的字符数组的中查找第一个等于指定字符的元素。
 * @param[in]: src_void: 查找源起始地址
 * @param[in]: c: 查找字符
 * @param[in]: length: 查找长度
 * @return: 
 *    第一个等于指定字符的元素地址。<br>
 *    NULL: 没有找到指定字符。<br>
 * @qualification method: 分析/测试
 * @derived requirement: 否
 * @function location: Internal
 * @implements: DR.1.20
 */
void *memchr(const void *src_void, int c, size_t length)
{
    const unsigned char *src = (const unsigned char *) src_void;
    unsigned long *asrc = NULL;
    unsigned long  buffer = 0;
    unsigned long  mask = 0;
    unsigned int cc = 0;
    int i = 0, j = 0;

    /* @KEEP_COMMENT: 设置当前查找位置src为<src_void> */

    /* @KEEP_COMMENT: 将<c>转换为字符<c> */
    cc = (unsigned int)c;
    cc &= 0xff;
    c = (int)cc;

    /* @REPLACE_BRACKET: <length>大于等于字长 && src按字长对齐 */
    if ((0 == TOO_SMALL(length)) && (0 == UNALIGNED(src)))
    {
        /* @KEEP_COMMENT: 设置asrc为src，构建字长查找关键字mask初始为0 */
        asrc = (unsigned long*) src;
        mask = 0;

        /* @REPLACE_BRACKET: 变量i = 0; i < 字长; i++ */
        for (i = 0; i < (int)LBLOCKSIZE; i++)
        {
            /* @KEEP_COMMENT: 构建mask为(mask << 8) + <c> */
            mask = (mask << 8) + c;
        }

        /* @REPLACE_BRACKET: <length>大于等于字长 */
        while (length >= (size_t)LBLOCKSIZE)
        {
            /* @KEEP_COMMENT: 当前字长比较数据和mask进行比较 */
            buffer = *asrc;
            buffer ^=  mask;

            /* @REPLACE_BRACKET: 比较数据和mask有字符匹配 */
            if (0 != DETECTNULL(buffer))
            {
                /* @KEEP_COMMENT: src = asrc */
                src = (const unsigned char*) asrc;

                /* @REPLACE_BRACKET: 变量j = 0; j < 字长; j++ */
                for (j = 0; j < (int)LBLOCKSIZE; j++)
                {
                    /* @KEEP_COMMENT: 当前src比较数据等于<c> */
                    if (*src == c)
                    {
                        /* @REPLACE_BRACKET: 当前比较位置src */
                        return((signed char*) src);
                    }
                    /* @KEEP_COMMENT: 按字节往后移动比较位置 */
                    src++;
                }
            }

            /* @KEEP_COMMENT: 设置<length>为<length>减字长，并按字长往后移动比较位置 */
            length -= (size_t)LBLOCKSIZE;
            asrc++;
        }

        /* @KEEP_COMMENT: 保存当前比较位置src */
        src = (const unsigned char*) asrc;
    }

    /* @REPLACE_BRACKET: <length>不等于0 */
    while (0 != length)
    {
        /* @KEEP_COMMENT: <length>-- */
        length--;

        /* @REPLACE_BRACKET: 当前字节比较数据src等于<c> */
        if (*src == c)
        {
            /* @REPLACE_BRACKET: 当前比较位置src */
            return((signed char*) src);
        }

        /* @KEEP_COMMENT: 按字节往后移动比较位置 */
        src++;
    }

    /* @REPLACE_BRACKET: NULL */
    return(NULL);
}

