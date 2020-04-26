/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit, including
*    copying it, modifying it, compiling it, and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice, this paragraph, and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* 
*
* @file     Util.h
* @brief     ���������ӿ�
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/

#ifndef UTIL1_INCLUDE
#define UTIL1_INCLUDE
#include <vector>
#include <iostream>

using namespace std;

#define LONG_MIN_ (0x80000000)
#define LONG_MAX_ (0x7fffffff)

class util
{
public:
    /**
    * ����: ��������16����ASCII�ַ�ת���ɶ�Ӧ�����֡�
    * @param ch    ������16����ASCII�ַ�
    * @param intValue[OUT]   ת�����32λ����
    * @return ����ת����16�����ַ������ȡ�
    */
    static int hex2num(unsigned char ch)
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
    static unsigned hex2int(char **ptr, int *intValue)
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
    static unsigned int2hex(char *ptr, int size,int intValue)
    {
        if(NULL == ptr)
        {
            return 0;
        }
        memset(ptr, '\0',size);    //���ô�С,��󻺳����Ĵ�С
        sprintf_s(ptr, size, "%x",intValue);
        return (unsigned)strlen(ptr);
    }

    /**
    * ����: ���ַ���ת����������
    * param nptr ---ָ���ַ�����ָ�롣
    * param base ---ת���Ľ��ơ�
    * return:     ����ת�����������
    */
    static int strtoi(char *nptr,  int base)
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

    /**
    * ����: ��value��ֵ,�����ڻ�����dest��,ռ�õĴ�Сλbytes
    * @param value  Ҫ���������
    * @param dest   ������ָ��
    * @param bytes  Ҫ�������ݵĴ�С
    * @return 0��ʾ�ɹ� 
    */
    template<class T> static int __store(T value,char *dest,int bytes)
    {
        int upper_bit = (value < 0 ? 128 : 0);
        int i;

        if (value < 0)
        {
            int oldvalue = value;
            value = -value;
            if (oldvalue != -value)
                return 1;
        }

        int iCount = (sizeof (value) < bytes ? sizeof (value) : bytes) ;
        for(i = 0 ; i < iCount ; i++) 
        {
            dest[i] = (char)( value & (i == (bytes - 1) ? 127 : 255));
            value = value / 256;
        }

        if (value && value != -1)
            return 1;

        for(; i < bytes ; i++) 
            dest[i] = 0;
        dest[bytes - 1] |= upper_bit;
        return 0;
    }

    /**
    * ����: �ӻ�����dest�ж�ȡbytes�ֽ�,�����浽dest��
    * @param value  Ҫ���������
    * @param dest   ������ָ��
    * @param bytes  Ҫ�������ݵĴ�С
    * @return 0��ʾ�ɹ� 
    */
    template<class T> static int __fetch(T *dest,char *source,int bytes)
    {
        T value = 0;
        int i;

        for (i = bytes - 1; (int) i > (sizeof (*dest) - 1); i--)
            if (source[i] & ((int) i == (bytes - 1) ? 127 : 255 ))
                return 1;

        for (; i >= 0; i--)
            value = value * 256 + (source[i] & ((int)i == (bytes - 1) ? 127 : 255));

        if ((source[bytes - 1] & 128) && (value > 0))
            value = - value;

        *dest = value;
        return 0;
    }
};


/**
* ����: ��value��ֵ,�����ڻ�����dest��,ռ�õĴ�Сλbytes
* @param value  Ҫ���������
* @param dest   ������ָ��
* @param bytes  Ҫ�������ݵĴ�С
* @return 0��ʾ�ɹ� 
*/
template<class T> int __store(T value,char *dest,int bytes) 
{
    int upper_bit = (value < 0 ? 128 : 0);
    int i;

    if (value < 0)
    {
        int oldvalue = value;
        value = -value;
        if (oldvalue != -value)
            return 1;
    }

    int iCount = (sizeof (value) < bytes ? sizeof (value) : bytes) ;
    for(i = 0 ; i < iCount ; i++) 
    {
        dest[i] = (char)( value & (i == (bytes - 1) ? 127 : 255));
        value = value / 256;
    }

    if (value && value != -1)
        return 1;

    for(; i < bytes ; i++) 
        dest[i] = 0;
    dest[bytes - 1] |= upper_bit;
    return 0;
}
/**
* ����: ��������16����ASCII�ַ�ת���ɶ�Ӧ�����֡�
* @param ch    ������16����ASCII�ַ�
* @param intValue[OUT]   ת�����32λ����
* @return ����ת����16�����ַ������ȡ�
*/
extern int hex2num(unsigned char ch);
/**
* ����: ��16�����ַ���ת���ɶ�Ӧ��32λ������
* @param ptr[OUT] ����ָ��16�����ַ�����ָ�룬ת��������ָ��ͬ��ǰ
*        �������ת������ʱ��ָ�롣
* @param intValue[OUT]   ת�����32λ����
* @return ����ת����16�����ַ������ȡ�
*/
extern unsigned int hex2int(char **ptr, int *intValue);
/**
* ����: ��intת����16���Ƶ��ַ���,���뱣֤size��С����ת������ַ�����С
* @param ptr ����16�����ַ����Ļ�����
* @param size   �������Ĵ�С
* @param intValue   Ҫת��������
* @return ת���Ĵ�С
*/
extern unsigned int int2hex(char *ptr, int size,int intValue);

/**
* ����: �ӻ�����dest�ж�ȡbytes�ֽ�,�����浽dest��
* @param value  Ҫ���������
* @param dest   ������ָ��
* @param bytes  Ҫ�������ݵĴ�С
* @return 0��ʾ�ɹ� 
*/
template<class T> int __fetch(T *dest,char *source,int bytes)
{
    T value = 0;
    int i;

    for (i = bytes - 1; (int) i > (sizeof (*dest) - 1); i--)
        if (source[i] & ((int) i == (bytes - 1) ? 127 : 255 ))
            return 1;

    for (; i >= 0; i--)
        value = value * 256 + (source[i] & ((int)i == (bytes - 1) ? 127 : 255));

    if ((source[bytes - 1] & 128) && (value > 0))
        value = - value;

    *dest = value;
    return 0;
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
int strtoi(char *nptr,  int base);


#endif
