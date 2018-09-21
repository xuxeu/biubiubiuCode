/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ������  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  shellMain.cpp
* @brief  
*       ���ܣ�
*       <li>��������ʵ��</li>
*/

/************************ͷ�ļ�********************************/
#include "utils.h"
#include "stdafx.h"
#include <stdio.h>

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************ȫ�ֱ���******************************/

/************************ģ�����******************************/

/************************�ⲿ����******************************/

/************************ǰ������******************************/

/************************ʵ��*********************************/

/**
 * @brief       �������в������л�ȡָ��ѡ���Ӧ����������
 * @param[in]    argc  ��������
 * @param[in]    argv  ��������
 * @param[in]    opt   ѡ��
 * @return      �����ɹ���������������Ӧ������������ʧ�ܷ���-1
**/
int ArgGetInt(int argc, char* argv[],char* opt)
{
    int i;

    for(i=1; i<argc; i++)
    {
        if(_stricmp(opt,argv[i]) == 0)
        {
            if(++i <= argc)
            {
                return atoi(argv[i]);
            }
        }
    }
    return -1;
}

/**
 * @brief       �������в������л�ȡָ��ѡ���Ӧ���ַ�������
 * @param[in]	argc  ��������
 * @param[in]	argv  ��������
 * @param[in]	opt   ѡ��
 * @return      �����ɹ�����ָ���ַ���������ָ�룬����ʧ�ܷ���NULL
**/
char* ArgGetStr(int argc, char* argv[],char* opt)
{
	int i;

	for(i=1; i<argc; i++)
	{
		if(_stricmp(opt,argv[i]) == 0)
		{
			if(++i <= argc)
			{
				return argv[i];
			}
		}
	}
	return NULL;
}

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


