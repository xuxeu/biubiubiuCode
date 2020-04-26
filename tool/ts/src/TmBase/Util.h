/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  Util.h
* @brief
*       ���ܣ�
*       <li>���������ӿ�</li>
*/


#ifndef UTIL1_INCLUDE
#define UTIL1_INCLUDE

/************************ͷ�ļ�********************************/
#include <vector>
#include <iostream>

using namespace std;

/************************�궨��********************************/
#define LONG_MIN_ (0x80000000)
#define LONG_MAX_ (0x7fffffff)


/************************���Ͷ���******************************/

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
        {
            return ch-'a'+10;
        }

        if (ch >= '0' && ch <= '9')
        {
            return ch-'0';
        }

        if (ch >= 'A' && ch <= 'F')
        {
            return ch-'A'+10;
        }

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
            {
                break;
            }

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
    static int strtoi(char *nptr,  int base);


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
            {
                return 1;
            }
        }

        int iCount = (sizeof (value) < bytes ? sizeof (value) : bytes) ;

        for(i = 0 ; i < iCount ; i++)
        {
            dest[i] = (char)( value & (i == (bytes - 1) ? 127 : 255));
            value = value / 256;
        }

        if (value && value != -1)
        {
            return 1;
        }

        for(; i < bytes ; i++)
        {
            dest[i] = 0;
        }

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
            {
                return 1;
            }

        for (; i >= 0; i--)
        {
            value = value * 256 + (source[i] & ((int)i == (bytes - 1) ? 127 : 255));
        }

        if ((source[bytes - 1] & 128) && (value > 0))
        {
            value = - value;
        }

        *dest = value;
        return 0;
    }
};

/************************�ӿ�����******************************/

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
        {
            return 1;
        }
    }

    int iCount = (sizeof (value) < bytes ? sizeof (value) : bytes) ;

    for(i = 0 ; i < iCount ; i++)
    {
        dest[i] = (char)( value & (i == (bytes - 1) ? 127 : 255));
        value = value / 256;
    }

    if (value && value != -1)
    {
        return 1;
    }

    for(; i < bytes ; i++)
    {
        dest[i] = 0;
    }

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
        {
            return 1;
        }

    for (; i >= 0; i--)
    {
        value = value * 256 + (source[i] & ((int)i == (bytes - 1) ? 127 : 255));
    }

    if ((source[bytes - 1] & 128) && (value > 0))
    {
        value = - value;
    }

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
