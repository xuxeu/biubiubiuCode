/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file             :  tsmapComm.c
 * @Version        :  1.0.0
 * @brief           :  *   <li> ����</li>tsmap�Ĺ����궨�弰��������ʵ��
                                <li>���˼��</li>
                                <p> n     ע�����n</p> 
 *   
 * @author        :  qingxiaohai
 * @create date:  2007��11��5�� 16:57:08
 * @History        : 
 *   
 */


/************************���ò���*****************************/
#include "stdio.h"
#include "stdlib.h"
#include "tsmapComm.h"
/************************ǰ����������***********************/

/************************���岿��*****************************/

/************************ʵ�ֲ���****************************/

typedef struct 
{
    int      erNo;
    char    *erStr;
}T_ERR_TBL;

T_ERR_TBL errTbl[20] ={
                     {FAIL, ESTR_SYS},
                     {EINVCMD, ESTR_INVCMD},
                     {EISTFAIL, ESTR_ISTFAIL},
                     {ENEXIT, ESTR_NEXIT},
                     {ENOTPMT, ESTR_NOTPMT},
                     {SUCC, "OK;"},
                     {1, ""}
                    };

/**
 * @Funcname:  getAckStr
 * @brief        :  ͨ������ֵ��ȡ��Ӧ�Ļظ��ַ���
 * @para[IN]  :errno, ������ 
 * @return      : ���ش������Ӧ�Ļظ��ַ���
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��11��5�� 17:05:11
 *   
**/

char* getAckStr(int eno)
{
    int i = 0;
    for(i = 0; (errTbl[i].erNo) != 1; i++)
    {
        if((errTbl[i].erNo) == eno)
        {
            return (errTbl[i].erStr);
        }
    }
    
    return NULL;
    
}

/**
* ����: ��������16����ASCII�ַ�ת���ɶ�Ӧ�����֡�
* @param ch    ������16����ASCII�ַ�
* @return ����16�����ַ���Ӧ��ASCII��
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
unsigned int int2hex(char *ptr, int size,int intValue)
{
    if(NULL == ptr)
    {
        return 0;
    }
    memset(ptr, '\0',size);    //���ô�С,��󻺳����Ĵ�С
    sprintf_s(ptr, size, "%x",intValue);
    return strlen(ptr);
}
