/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  tsfsCommon.cpp
* @brief  
*       功能：
*       <li>定义一些公共函数</li>
*/
/************************头文件********************************/
#include "stdafx.h"
#include "tsfsCommon.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************全局变量******************************/

/************************模块变量******************************/

/************************外部声明******************************/

/************************前向声明******************************/

/************************实现*********************************/

/**
 * @brief
 *   把int转换成16进制的字符串, 必须保证size大小大于转换后的字符串大小
 * @param ptr[out]: 保存16进制字符串的缓冲区
 * @param size[in]:   缓冲区的大小
 * @param intValue[in]:   要转换的整形
 * @return 转换的大小
 */
unsigned int intToHex(char *ptr,  int size, __int64 intValue)
{
    memset(ptr, '\0', size); //设置大小, 最大缓冲区的大小
    sprintf_s(ptr, size,  "%x", intValue);
    return (UI)strlen(ptr);
}

/**
 * @brief
 *   将16进制字符串转换成对应的32位整数。
 * @param ptr[out]: 输入指向16进制字符串的指针，转换过程中指针同步前
 *        进。输出转换结束时的指针。
 * @param intValue[out]: 转换后的32位整数
 * @return 返回转换的16进制字符串长度。
 */
unsigned int hexToInt(char **ptr, int *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;

    while (**ptr)
    {
        hexValue = hexToNum(**ptr);
        if (hexValue < 0)
            break;

        *intValue = (*intValue << 4) | hexValue;
        numChars ++;

        (*ptr)++;
    }

    return (numChars);
}

/**
 * @brief
 *   将16进制字符串转换成对应的64位整数。
 * @param ptr[out]: 输入指向16进制字符串的指针，转换过程中指针同步前
 *      进。输出转换结束时的指针。
 * @param intValue[out]: 转换后的64位整数
 * @return 返回转换的16进制字符串长度。
 */
unsigned int hexToInt64(char **ptr, __int64 *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;

    while (**ptr)
    {
        hexValue = hexToNum(**ptr);
        if (hexValue < 0)
            break;

        *intValue = (*intValue << 4) | hexValue;
        numChars ++;

        (*ptr)++;
    }

    return (numChars);
}

/**
 * @brief
 *   将单个的16进制ASCII字符转换成对应的数字。
 * @param ch[in]:    单个的16进制ASCII字符
 * @param intValue[out]:   转换后的32位整数
 * @return 返回转换的16进制字符串长度。
 */
int hexToNum(unsigned char ch)
{
    if (ch >= 'a' && ch <= 'f')
        return ch-'a'+10;
    if (ch >= '0' && ch <= '9')
        return ch-'0';
    if (ch >= 'A' && ch <= 'F')
        return ch-'A'+10;
    return -1;
}
