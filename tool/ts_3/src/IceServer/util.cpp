/********************************************************************
 * ������:   hex2int
 --------------------------------------------------------------------
 * ����:     ��16�����ַ���ת���ɶ�Ӧ��32λ������
 --------------------------------------------------------------------
 * �������: ��
 --------------------------------------------------------------------
 * �������: intValue ---ת�����32λ����
 --------------------------------------------------------------------
 * �������: ptr ---����ָ��16�����ַ�����ָ�룬ת��������ָ��ͬ��ǰ
 *        �������ת������ʱ��ָ�롣
 --------------------------------------------------------------------
 * ����ֵ:     ����ת����16�����ַ������ȡ�
 --------------------------------------------------------------------
 * ������:   ȫ��
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
    memset(ptr,'\0',size);    //���ô�С,��󻺳����Ĵ�С
    sprintf(ptr,"%x",intValue);
    return strlen(ptr);
}
#if 0
/*���ܣ���16���Ƶ��ַ���,(ÿ���ֽ�������0~F�ַ���ʾ)ת��������
     ���أ�ת�����ֽ���
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
        //ת����char
        char tmpValue = 0;
        if(srcEndian)
        {
            //Դ�����Ǵ�ˣ���ת����x86��С��
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
        //ת����short  
        short tmpValue = 0;
        if(srcEndian)
        {
            //Դ�����Ǵ�ˣ���ת����x86��С��
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
        //ת����int
        int tmpValue = 0;
        if(srcEndian)
        {
            //Դ�����Ǵ�ˣ���ת����x86��С��
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

