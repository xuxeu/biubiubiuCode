#include "util.h"
#include <stdio.h>
#include <iostream>


/**
����16����ASCII�ַ���
*/
const char hexchars[]="0123456789abcdef";

/**
* ����: ��������16����ASCII�ַ�ת���ɶ�Ӧ�����֡�
* @param ch    ������16����ASCII�ַ�
* @param intValue[OUT]   ת�����32λ����
* @return ����ת����16�����ַ������ȡ�
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
* ����: ��16�����ַ���ת���ɶ�Ӧ��32λ������
* @param ptr[OUT] ����ָ��16�����ַ�����ָ�룬ת��������ָ��ͬ��ǰ
*        �������ת������ʱ��ָ�롣
* @param intValue[OUT]   ת�����32λ����
* @return ����ת����16�����ַ������ȡ�
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
* ����: ��intת����16���Ƶ��ַ���,���뱣֤size��С����ת������ַ�����С
* @param ptr ����16�����ַ����Ļ�����
* @param size   �������Ĵ�С
* @param intValue   Ҫת��������
* @return ת���Ĵ�С
*/
unsigned int int2hex(char *ptr, int size,int intValue)
{
    if(NULL == ptr)
    {
        return 0;
    }
    memset(ptr, '\0',size);    //���ô�С,��󻺳����Ĵ�С
    sprintf_s(ptr, size, "%x",intValue);
    return strlen(ptr);
}


/********************************************************************
 * ������:   strtoi
 --------------------------------------------------------------------
 * ����:     ���ַ���ת����������
 --------------------------------------------------------------------
 * �������: nptr ---ָ���ַ�����ָ�롣
 *             base ---ת���Ľ��ơ�
 --------------------------------------------------------------------
 * �������: ��
 --------------------------------------------------------------------
 * �������: �ޡ�
 --------------------------------------------------------------------
 * ����ֵ:     ����ת�����������
 --------------------------------------------------------------------
 * ������:   ȫ��
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
