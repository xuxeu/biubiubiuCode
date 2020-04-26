/********************************************************************
 * 函数名:   hex2int
 --------------------------------------------------------------------
 * 功能:     将16进制字符串转换成对应的32位整数。
 --------------------------------------------------------------------
 * 输入参数: 无
 --------------------------------------------------------------------
 * 输出参数: intValue ---转换后的32位整数
 --------------------------------------------------------------------
 * 出入参数: ptr ---输入指向16进制字符串的指针，转换过程中指针同步前
 *        进。输出转换结束时的指针。
 --------------------------------------------------------------------
 * 返回值:     返回转换的16进制字符串长度。
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
#include "util.h"
#include "stdio.h"
#include "string.h"
#include "memory.h"

unsigned int hex2int(char **ptr, int *intValue)
{
  int numChars = 0;
  int hexValue;

  *intValue = 0;

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

int hex2num(unsigned char vubCh)
{
  if (vubCh >= 'a' && vubCh <= 'f')
    return vubCh-'a'+10;
  if (vubCh >= '0' && vubCh <= '9')
    return vubCh-'0';
  if (vubCh >= 'A' && vubCh <= 'F')
    return vubCh-'A'+10;
  return -1;
}

unsigned int int2hex(char *ptr, int size,int intValue)
{
    memset(ptr,'\0',size);    //设置大小,最大缓冲区的大小
    sprintf(ptr,"%x",intValue);
    return strlen(ptr);
}
#if 0
/*功能：将16进制的字符串,(每个字节由两个0~F字符表示)转换成整形
     返回：转换的字节数
*/
int hexStringToInt(char *pbuf, US buflen, int value, bool srcEndian)
{
    if(NULL == pbuf)
    {
        return 0;
    }
    int iValue = 0;
    if(buflen == (sizeof(char)*2))
    {
        //转换成char
        char tmpValue = 0;
        if(srcEndian)
        {
            //源数据是大端，则转换成x86的小端
            tmpValue = ((*(pbuf + 1) & 0xff) << 4) | (*pbuf & 0xff) ;
        }
        else
        {
            tmpValue = ((*pbuf & 0xff) << 4) | (*(pbuf + 1) & 0xff) ;
        }
        iValue = tmpValue;
    }
    else if(buflen == (sizeof(short)*2))
    {
        //转换成short  
        short tmpValue = 0;
        if(srcEndian)
        {
            //源数据是大端，则转换成x86的小端
            tmpValue = (((*(pbuf + 3) & 0xff) << 24) | 
                                 ((*(pbuf + 2) & 0xff) << 16) |
                                 ((*(pbuf + 1) & 0xff) << 8) |
                                 ((*(pbuf) & 0xff))
                                 );
        }
        else
        {
            tmpValue = ((*pbuf & 0xff) << 4) | (*(pbuf + 1) & 0xff) ;
        }
        iValue = tmpValue;
    }
    else if(buflen == (sizeof(short)*2))
    {
        //转换成int
        int tmpValue = 0;
        if(srcEndian)
        {
            //源数据是大端，则转换成x86的小端
            tmpValue = ((*(pbuf + 1) & 0xff) << 4) | (*pbuf & 0xff) ;
        }
        else
        {
            tmpValue = ((*pbuf & 0xff) << 4) | (*(pbuf + 1) & 0xff) ;
        }
        iValue = tmpValue;
    }
    return iValue;
}
#endif

int fromhex (int a)
{

  if (a >= '0' && a <= '9')

    return a - '0';

  else if (a >= 'a' && a <= 'f')

    return a - 'a' + 10;

  else if (a >= 'A' && a <= 'F')

    return a - 'A' + 10;

  else

    return -1;

}
int hex2bin (const char *hex, char *bin, int count)
{

  int i;

  for (i = 0; i < count; i++)

    {

      if (hex[0] == 0 || hex[1] == 0)

         {

           /* Hex string is short, or of uneven length.

              Return the count that has been converted so far. */

           return i;

         }

      *bin++ = fromhex (hex[0]) * 16 + fromhex (hex[1]);

      hex += 2;

    }

  return i;

}

