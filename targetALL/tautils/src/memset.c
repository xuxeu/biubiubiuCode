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
 * @file: memset.c
 * @brief:
 *	   <li> 实现字符串模块里的memset。</li>
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
 *    按指定长度将指定字符数组的元素填充为指定字符。
 * @param[in]: c: 填充字符
 * @param[in]: n: 填充长度
 * @param[out]: m: 填充目的起始地址
 * @return: 
 *    填充目的起始地址。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes: 
 *    函数中不检查填充目的起始地址、填充字符和填充长度的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.4
 */
void *memset(void *m, int c, size_t n)
{
    signed char *s = NULL;
    int i = 0;
    unsigned int cc = 0;
    unsigned long ct = 0;
    unsigned long buffer = 0;
    unsigned long *aligned_addr = NULL;

    /* @KEEP_COMMENT: 设置当前填充位置s为<m> */
    s = (signed char *) m;

    /*
     * @brief:
     *    如果<n>大于等于字长并且<m>按字长对齐时，按以下步骤进行操作:<br>
     *    (1)<n>大于等于4倍字长的字符按4倍字长将元素填充为字符<c>；<br>
     *    (2)大于等于字长但小于4倍字长的字符按字长进行填充;<br>
     *    (3)小于字长的字符以字节为单位进行填充；<br>
     *    (4)返回<m>。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 1 
     */
    /* @REPLACE_BRACKET: <n>大于等于字长 && <m>按字对齐 */
    if ((0 == TOO_SMALL(n)) && (0 == UNALIGNED(m)))
    {
        aligned_addr = (unsigned long*)m;
        cc = (unsigned int)c;
        cc &= 0xff;
        c = (int)cc;
        ct = (unsigned long)cc;

        /* @REPLACE_BRACKET: 字长等于4 */
        if (LBLOCKSIZE == 4)
        {
            /*
             * @KEEP_COMMENT: 将字长填充数据buffer中高2位和低2位的每个字节都用c填
             * 充并保存
             */
            buffer = (ct << 8)|(ct);
            buffer |= (buffer << 16);
        }
        else
        {
            buffer = 0;

            /* @REPLACE_BRACKET: 变量i = 0; i < 字长; i++ */
            for (i = 0; i < (int)LBLOCKSIZE; i++)
            {
                /* @KEEP_COMMENT: 将字长填充数据buffer中每个字节用c填充并保存 */
                buffer = (buffer << 8)|ct;
            }
        }

        /* @REPLACE_BRACKET: <n>大于等于4倍字长 */
        while (n >= (size_t)LBLOCKSIZE*4)
        {
            /*
             * @KEEP_COMMENT: 使用buffer填充4倍字长数据到s当前位置，
             * 并按4倍字长往后移动填充位置
             */
            *aligned_addr = buffer;
            aligned_addr++;
            *aligned_addr = buffer;
            aligned_addr++;
            *aligned_addr = buffer;
            aligned_addr++;
            *aligned_addr = buffer;
            aligned_addr++;

            /* @KEEP_COMMENT: 设置<n>为<n> - 4倍字长 */
            n -= 4*((size_t)LBLOCKSIZE);
        }

        /* @REPLACE_BRACKET: <n>大于等于字长 */
        while (n >= (size_t)LBLOCKSIZE)
        {
            /*
             * @KEEP_COMMENT: 按字长填充buffer数据到s当前位置，
             * 并按字长往后移动填充位置
             */
            *aligned_addr = buffer;
            aligned_addr++;

            /* @KEEP_COMMENT: 设置<n>为<n>-字长 */
            n -= (size_t)LBLOCKSIZE;
        }

        /* @REPLACE_LINE: 保存当前填充位置s */
        s = (signed char*)aligned_addr;
    }

    /*
     * @brief:
     *    如果<n>小于字长或者<m>不按字长对齐，则以字节为单位将元素填充为字符<c>，
     *    返回<m>。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 2 
     */
    /* @REPLACE_BRACKET: <n>不等于0 */
    while (0 != n)
    {
        /* @KEEP_COMMENT: <n>-- */
        n--;

        /* @KEEP_COMMENT: 按字节填充c到当前位置s，并按字节往后移动填充位置 */
        *s = (signed char)c;
        s++;
    }

    /* @REPLACE_BRACKET: <m> */
    return(m);
}

