/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  comfunc.cpp
 * @Version        :  1.0.0
 * @Brief           :  ��������ʵ��
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007��4��9�� 15:29:56
 * @History        : 
 *   
 */




/************************���ò���*****************************/

#include "stdio.h"
#include "string.h"
#include "memory.h"
#include "comfunc.h"
#include "wcs_interface.h"
/************************ǰ����������***********************/

/************************���岿��*****************************/

/************************ʵ�ֲ���****************************/


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
    memset(ptr,'\0',size);    //���ô�С,��󻺳����Ĵ�С
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
*    ��һ�����͸��ݴ�С��תΪCHAR����\n
* @param[in] wReg    �������ṩ�ļĴ�����ֵ
* @param[in] cpBuf   �������ṩ�Ļ���
* @param[in] bIsBigEndian  �������ṩ�Ĵ�С����Ϣ
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
*    ��һ��CHAR������ݴ�С��תΪ����\n
* @param[in] cpBuf   �������ṩ�Ļ���
* @param[in] bIsBigEndian  �������ṩ�Ĵ�С����Ϣ
* @return ������
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
*    ����CPSR�Ĵ�����ֵ��ȡ��ǰĿ�����ģʽ
* @param[in] cpsrValue   CPSR �Ĵ�����ֵ
* @return ������
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

