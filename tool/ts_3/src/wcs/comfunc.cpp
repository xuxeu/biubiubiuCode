/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  comfunc.cpp
 * @Version        :  1.0.0
 * @Brief           :  公共函数实现
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月9日 15:29:56
 * @History        : 
 *   
 */




/************************引用部分*****************************/

#include "stdio.h"
#include "string.h"
#include "memory.h"
#include "comfunc.h"
#include "wcs_interface.h"
/************************前向声明部分***********************/

/************************定义部分*****************************/

/************************实现部分****************************/


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

/*
* @brief
*    将一个整型根据大小端转为CHAR数组\n
* @param[in] wReg    调用者提供的寄存器数值
* @param[in] cpBuf   调用者提供的缓冲
* @param[in] bIsBigEndian  调用者提供的大小端信息
* @return
*/
void Int2Char(int wReg, char *cpBuf, bool bIsBigEndian)
{
    if(bIsBigEndian)
    {
        *cpBuf = ((wReg >> 24) & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 16) & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 8)  & 0xff);
        cpBuf++;
        *cpBuf = (wReg & 0xff);
        }
    else
    {
        *cpBuf = (wReg & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 8)  & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 16) & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 24) & 0xff);
    }
}

/*
* @brief
*    将一个CHAR数组根据大小端转为整型\n
* @param[in] cpBuf   调用者提供的缓冲
* @param[in] bIsBigEndian  调用者提供的大小端信息
* @return 该整型
*/
int Char2Int(char *cpBuf, bool bIsBigEndian)
{
    if(bIsBigEndian)
    {
        return    (*(cpBuf + 3) & 0xff)       |
                (*(cpBuf + 2) & 0xff) <<  8 |
                (*(cpBuf + 1) & 0xff) << 16 |
                (*cpBuf)       << 24;

    }
    else
    {
        return  (*(cpBuf + 3))        << 24 |
                  (*(cpBuf + 2) & 0xff) << 16 |
                  (*(cpBuf + 1) & 0xff) <<  8 |
                  (*cpBuf & 0xff);
    }
}


/*
* @brief
*    根据CPSR寄存器的值获取当前目标机的模式
* @param[in] cpsrValue   CPSR 寄存器的值
* @return 该整型
*/
WCS_REGMODE_T getCurrentRegMode(UINT32 cpsrValue)
{
    WCS_REGMODE_T  mode = WCS_CURR_MODE;
    switch((WCS_REGMODE_T)(cpsrValue & 0x1F))
    {
        case WCS_USR_MODE:
            mode = WCS_USR_MODE;
            break;
            
        case WCS_FIQ_MODE:
            mode = WCS_FIQ_MODE;
            break;
            
        case WCS_IRQ_MODE:
            mode = WCS_IRQ_MODE;
            break;
            
        case WCS_SYS_MODE:
            mode = WCS_SYS_MODE;
            break;
            
        case WCS_SVC_MODE:
            mode = WCS_SVC_MODE;
            break;
            
        case WCS_ABT_MODE:
            mode = WCS_ABT_MODE;
            break;
            
        case WCS_UND_MODE:
            mode = WCS_UND_MODE;
            break;
            
        default:
            mode = WCS_CURR_MODE;
            break;
    }

    return mode;
}

