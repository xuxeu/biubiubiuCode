/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  comfunc.h
 * @Version        :  1.0.0
 * @Brief           :  公共函数定义
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月9日 15:29:34
 * @History        : 
 *   
 */
#ifndef _COMFUNC_H_
#define _COMFUNC_H_

#include "sysType.h"
#include "wcs_interface.h"
/************************引用部分*****************************/

unsigned int hex2int(char **ptr, int *intValue);
unsigned int int2hex(char *ptr, int size,int intValue);

int hex2num(unsigned char vubCh);


int fromhex (int a);
int hex2bin (const char *hex, char *bin, int count);

void Int2Char(int wReg, char *cpBuf, bool bIsBigEndian);
int Char2Int(char *cpBuf, bool bIsBigEndian);




/**
* 功能: 从缓冲区dest中读取bytes字节,并保存到dest中
* @param value  要保存的数据
* @param dest   缓冲区指针
* @param bytes  要保存数据的大小
* @return 0表示成功 
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
* 功能: 将value的值,保存在缓冲区dest中,占用的大小位bytes
* @param value  要保存的数据
* @param dest   缓冲区指针
* @param bytes  要保存数据的大小
* @return 0表示成功 
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

