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
 * @file: strncmp.c
 * @brief:
 *	   <li> 实现字符串模块里的strncmp。</li>
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
 *    按指定长度依次比较指定的两个字符串的字符。	
 * @param[in]: s1: 第一个字符串起始地址
 * @param[in]: s2: 第二个字符串起始地址
 * @param[in]: n: 比较长度 
 * @return: 
 *    0: 两个字符串前n个长度的每个字符都相等。
 *    负值: s1前n个长度的字符串中字符的ASCII码小于s2前n个长度的字符串中对应
 *          字符的ASCII码。
 *    正值: s1前n个长度的字符串中字符的ASCII码大于s2前n个长度的字符串中对应
 *          字符的ASCII码。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes: 
 *    函数中不检查第一个字符串和第二个字符串起始地址的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.10 
 */
int strncmp(const char *s1, const char *s2, size_t n)
{
    unsigned long *a1 = NULL;
    unsigned long *a2 = NULL;

    /*
     * @brief:
     *    如果<n>等于0，返回0。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @implements: 1 
     */
    /* @REPLACE_BRACKET: <n>等于0 */
    if (0 == n)
    {
        /* @REPLACE_BRACKET: 0 */
        return(0);
    }

    /*
     * @brief:
     *    如果<s1>和<s2>按字长对齐，则以字长为单位进行比较，并将末尾不足字长的字
     *    符以字节为单位进行比较，依次比较<s1>和<s2>指定的两个字符串的前<n>个字
     *    符，如果<n>大于其中一个字符串的长度，则只比较较短字符串的全部字符。<br>
     *    (1)当<n>小于两个字符串长度且两个字符串的前<n>个字符都相等，或者<n>大于
     *       两个字符串长度且两个字符串每个字符都相等，返回0。<br>
     *    (2)当<n>大于两个字符串长度且两个字符串长度不一致或者两个字符串的前<n>
     *       个字符有不相等字符时，返回c1-c2。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @notes: 
     *    c1为<s1>指定的字符串中第一个不相等的字符。<br>
     *    c2为<s2>指定的字符串中与<s1>中对应不相等的字符。<br>
     * @implements: 1 
     */
    /* 如果s1或s2没有按字对齐，则按字节进行比较 */
    /* @REPLACE_BRACKET: <s1>和<s2>按字对齐 */
    if (0 == UNALIGNED_2(s1, s2))
    {
        a1 = (unsigned long*)s1;
        a2 = (unsigned long*)s2;

        /* @REPLACE_BRACKET: <n>大于等于字长 && <s1>和<s2>当前字长比较数据相等 */       
        while ((n >= LBLOCKSIZE) && ((*a1) == (*a2)))
        {
            /* @KEEP_COMMENT: 设置<n>为<n>-字长 */
            n -= (size_t)LBLOCKSIZE;

            /* @REPLACE_BRACKET: (<n>等于0) || <s1>当前字长比较数据中有结束符 */
            if ((n == 0) || (0 != DETECTNULL(*a1)))
            {
                /* @REPLACE_BRACKET: 0 */
                return(0);
            }

            /* @KEEP_COMMENT: 按字长往后移动<s1>和<s2>比较位置 */
            a1++;
            a2++;
        }

        /* @KEEP_COMMENT: 保存当前比较位置<s1>和<s2> */
        s1 = (const char*)a1;
        s2 = (const char*)a2;
    }

    /*
     * @brief:
     *    如果<s1>或<s2>不按字长对齐，则以字节为单位进行比较，依次比较<s1>和<s2>
     *    指定的两个字符串的前<n>个字符，如果<n>大于其中一个字符串的长度，则只比
     *    较较短字符串的全部字符。<br>
     *    (1)当<n>小于两个字符串长度且两个字符串的前<n>个字符都相等，或者<n>大于
     *       两个字符串长度且两个字符串每个字符都相等，返回0。<br>
     *    (2)当<n>大于两个字符串长度且两个字符串长度不一致或者两个字符串的前<n>
     *       个字符有不相等字符时，返回c1-c2。<br>
     * @qualification method: 测试
     * @derived requirement: 否
     * @notes: 
     *    c1为<s1>指定的字符串中第一个不相等的字符。<br>
     *    c2为<s2>指定的字符串中与<s1>中对应不相等的字符。<br>
     * @implements: 2 
     */
    /* 按字节进行比较 */
    /* @REPLACE_BRACKET: <s1>和<s2>当前字节比较数据相等 */
    while ((*s1) == (*s2))
    {
        /* @KEEP_COMMENT: <n>-- */
        n--;

        /* @REPLACE_BRACKET: <n>等于0 || <s1>当前字节比较数据是结束符 */
        if ((0 == n) || ('\0' == (*s1)))
        {
            /* @REPLACE_BRACKET: 0 */
            return(0);
        }

        /* @KEEP_COMMENT: 按字节往后移动<s1>和<s2>比较位置 */
        s1++;
        s2++;
    }

    /* @REPLACE_BRACKET: <s1>比较数据减去<s2>比较数据 */
    return((*(unsigned char *)s1)-(*(unsigned char *)s2));
}

