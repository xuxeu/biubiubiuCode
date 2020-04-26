#include "util.h"
#include <stdio.h>
#include <iostream>


/**
单个16进制ASCII字符。
*/
const char hexchars[]="0123456789abcdef";

/**
* 功能: 将单个的16进制ASCII字符转换成对应的数字。
* @param ch    单个的16进制ASCII字符
* @param intValue[OUT]   转换后的32位整数
* @return 返回转换的16进制字符串长度。
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


/********************************************************************
 * 函数名:   strtoi
 --------------------------------------------------------------------
 * 功能:     将字符串转换成整数。
 --------------------------------------------------------------------
 * 输入参数: nptr ---指向字符串的指针。
 *             base ---转换的进制。
 --------------------------------------------------------------------
 * 输出参数: 无
 --------------------------------------------------------------------
 * 出入参数: 无。
 --------------------------------------------------------------------
 * 返回值:     返回转换后的整数。
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
int strtoi(char *nptr,  int base)
{
    register const char *s = nptr;
    register unsigned int acc;
    register int c;
    register unsigned int cutoff;
    register int neg = 0,  any,  cutlim;

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0,  allow 0x for hex and 0 for octal,  else
     * assume decimal; if base is already 16,  allow 0x.
     */
     if(NULL == nptr)
     {
         return -1;
     }
     
    do
    {
        c = *s++;
    } while (isspace(c));
    if (c == '-') 
    {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
    if ((base == 0 || base == 16) 
        && (c == '0') && (*s == 'x' || *s == 'X')) 
    {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value,  divided by the
     * base.  An input number that is greater than this value,  if
     * followed by a legal input character,  is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance,  if the range for longs is
     * [-2147483648..2147483647] and the input base is 10, 
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1),  meaning that if we have accumulated
     * a value > 214748364,  or equal but the next digit is > 7 (or 8), 
     * the number is too big,  and we will return a range error.
     *
     * Set any if any `digits' consumed; make it negative to indicate
     * overflow.
     */
    cutoff = neg ? (-(unsigned int)LONG_MIN_) : LONG_MAX_;
    cutlim = cutoff % (unsigned int)base;
    cutoff /= (unsigned int)base;
    for (acc = 0,  any = 0;; c = *s++) 
    {
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else 
        {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    if (any < 0) 
    {
        acc = neg ? LONG_MIN_ : LONG_MAX_;
    } else if (neg)
        acc = -acc;
    return (acc);
}
