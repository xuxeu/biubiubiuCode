/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  tsfsCommon.cpp
* @brief  
*       ���ܣ�
*       <li>����һЩ��������</li>
*/
/************************ͷ�ļ�********************************/
#include "stdafx.h"
#include "tsfsCommon.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************ȫ�ֱ���******************************/

/************************ģ�����******************************/

/************************�ⲿ����******************************/

/************************ǰ������******************************/

/************************ʵ��*********************************/

/**
 * @brief
 *   ��intת����16���Ƶ��ַ���, ���뱣֤size��С����ת������ַ�����С
 * @param ptr[out]: ����16�����ַ����Ļ�����
 * @param size[in]:   �������Ĵ�С
 * @param intValue[in]:   Ҫת��������
 * @return ת���Ĵ�С
 */
unsigned int intToHex(char *ptr,  int size, __int64 intValue)
{
    memset(ptr, '\0', size); //���ô�С, ��󻺳����Ĵ�С
    sprintf_s(ptr, size,  "%x", intValue);
    return (UI)strlen(ptr);
}

/**
 * @brief
 *   ��16�����ַ���ת���ɶ�Ӧ��32λ������
 * @param ptr[out]: ����ָ��16�����ַ�����ָ�룬ת��������ָ��ͬ��ǰ
 *        �������ת������ʱ��ָ�롣
 * @param intValue[out]: ת�����32λ����
 * @return ����ת����16�����ַ������ȡ�
 */
unsigned int hexToInt(char **ptr, int *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;

    while (**ptr)
    {
        hexValue = hexToNum(**ptr);
        if (hexValue < 0)
            break;

        *intValue = (*intValue << 4) | hexValue;
        numChars ++;

        (*ptr)++;
    }

    return (numChars);
}

/**
 * @brief
 *   ��16�����ַ���ת���ɶ�Ӧ��64λ������
 * @param ptr[out]: ����ָ��16�����ַ�����ָ�룬ת��������ָ��ͬ��ǰ
 *      �������ת������ʱ��ָ�롣
 * @param intValue[out]: ת�����64λ����
 * @return ����ת����16�����ַ������ȡ�
 */
unsigned int hexToInt64(char **ptr, __int64 *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;

    while (**ptr)
    {
        hexValue = hexToNum(**ptr);
        if (hexValue < 0)
            break;

        *intValue = (*intValue << 4) | hexValue;
        numChars ++;

        (*ptr)++;
    }

    return (numChars);
}

/**
 * @brief
 *   ��������16����ASCII�ַ�ת���ɶ�Ӧ�����֡�
 * @param ch[in]:    ������16����ASCII�ַ�
 * @param intValue[out]:   ת�����32λ����
 * @return ����ת����16�����ַ������ȡ�
 */
int hexToNum(unsigned char ch)
{
    if (ch >= 'a' && ch <= 'f')
        return ch-'a'+10;
    if (ch >= '0' && ch <= '9')
        return ch-'0';
    if (ch >= 'A' && ch <= 'F')
        return ch-'A'+10;
    return -1;
}
