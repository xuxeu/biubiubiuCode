/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  comfunc.h
 * @Version        :  1.0.0
 * @Brief           :  ������������
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007��4��9�� 15:29:34
 * @History        : 
 *   
 */
#ifndef _COMFUNC_H_
#define _COMFUNC_H_

#include "sysType.h"
#include "wcs_interface.h"
/************************���ò���*****************************/

unsigned int hex2int(char **ptr, int *intValue);
unsigned int int2hex(char *ptr, int size,int intValue);

int hex2num(unsigned char vubCh);


int fromhex (int a);
int hex2bin (const char *hex, char *bin, int count);

void Int2Char(int wReg, char *cpBuf, bool bIsBigEndian);
int Char2Int(char *cpBuf, bool bIsBigEndian);




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
        short oldvalue = value;
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


WCS_REGMODE_T getCurrentRegMode(UINT32 cpsrValue);


#endif/*_COMFUNC_H_*/

