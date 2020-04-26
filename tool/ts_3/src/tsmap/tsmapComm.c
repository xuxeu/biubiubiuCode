/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file             :  tsmapComm.c
 * @Version        :  1.0.0
 * @brief           :  *   <li> 功能</li>tsmap的公共宏定义及公共函数实现
                                <li>设计思想</li>
                                <p> n     注意事项：n</p> 
 *   
 * @author        :  qingxiaohai
 * @create date:  2007年11月5日 16:57:08
 * @History        : 
 *   
 */


/************************引用部分*****************************/
#include "stdio.h"
#include "stdlib.h"
#include "tsmapComm.h"
/************************前向声明部分***********************/

/************************定义部分*****************************/

/************************实现部分****************************/

typedef struct 
{
    int      erNo;
    char    *erStr;
}T_ERR_TBL;

T_ERR_TBL errTbl[20] ={
                     {FAIL, ESTR_SYS},
                     {EINVCMD, ESTR_INVCMD},
                     {EISTFAIL, ESTR_ISTFAIL},
                     {ENEXIT, ESTR_NEXIT},
                     {ENOTPMT, ESTR_NOTPMT},
                     {SUCC, "OK;"},
                     {1, ""}
                    };

/**
 * @Funcname:  getAckStr
 * @brief        :  通过返回值获取对应的回复字符串
 * @para[IN]  :errno, 错误码 
 * @return      : 返回错误码对应的回复字符串
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月5日 17:05:11
 *   
**/

char* getAckStr(int eno)
{
    int i = 0;
    for(i = 0; (errTbl[i].erNo) != 1; i++)
    {
        if((errTbl[i].erNo) == eno)
        {
            return (errTbl[i].erStr);
        }
    }
    
    return NULL;
    
}

/**
* 功能: 将单个的16进制ASCII字符转换成对应的数字。
* @param ch    单个的16进制ASCII字符
* @return 返回16进制字符对应的ASCII码
*/
int hex2num(unsigned char ch)
{
    if (ch >= 'a' && ch <= 'f')
        return ch-'a'+10;
    if (ch >= '0' && ch <= '9')
        return ch-'0';
    if (ch >= 'A' && ch <= 'F')
        return ch-'A'+10;
    return -1;
}

/**
* 功能: 将16进制字符串转换成对应的32位整数。
* @param ptr[OUT] 输入指向16进制字符串的指针，转换过程中指针同步前
*        进。输出转换结束时的指针。
* @param intValue[OUT]   转换后的32位整数
* @return 返回转换的16进制字符串长度。
*/
unsigned int hex2int(char **ptr, int *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;
    if((NULL == ptr) || (NULL == intValue))
    {
        return 0;
    }
    while (**ptr)
    {
        hexValue = hex2num(**ptr);
        if (hexValue < 0)
            break;

        *intValue = (*intValue << 4) | hexValue;
        numChars ++;

        (*ptr)++;
    }

    return (numChars);
}

/**
* 功能: 把int转换成16进制的字符串,必须保证size大小大于转换后的字符串大小
* @param ptr 保存16进制字符串的缓冲区
* @param size   缓冲区的大小
* @param intValue   要转换的整形
* @return 转换的大小
*/
unsigned int int2hex(char *ptr, int size,int intValue)
{
    if(NULL == ptr)
    {
        return 0;
    }
    memset(ptr, '\0',size);    //设置大小,最大缓冲区的大小
    sprintf_s(ptr, size, "%x",intValue);
    return strlen(ptr);
}
