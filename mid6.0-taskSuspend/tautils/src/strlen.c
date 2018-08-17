/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2011-02-23         ���࣬�����������ɼ������޹�˾
 *                               �������ļ���
 * 2011-04-15	  ���£������������ɼ������޹�˾
 * 				  ����PDL��������������ʽ��
*/

/*
 * @file: strlen.c
 * @brief:
 *	   <li> ʵ���ַ���ģ�����strlen��</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/************************ͷ �� ��******************************/

/* @<MOD_HEAD */
#include <string.h>
/* @MOD_HEAD> */

/************************�� �� ��******************************/
/************************���Ͷ���******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/
/************************ģ�����******************************/
/************************ȫ�ֱ���******************************/
/************************ʵ    ��******************************/

/* @MODULE> */


/**
 * @req
 * @brief: 
 *    ����ָ�����ַ����������ַ�(������������)��Ŀ��
 * @param[in]: str: �ַ�����ʼ��ַ
 * @return: 
 *    �ַ���Ŀ��
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes:
 *    �����в�����ַ�����ʼ��ַ����Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.9
 */
size_t strlen(const char *str)
{
    /* @KEEP_COMMENT: ������ʼλ��startΪ<str>�����õ�ǰ����λ��endΪ<str> */
    const signed char *start = str;
    const signed char *end = str;

    unsigned long *aligned_addr = NULL;

    /*
     * @brief:
     *    ���<str>���ֳ����룬���ֳ�����<str>���ַ���Ŀ������ĩβ�����ֳ�����
     *    �����ֽ�Ϊ��λ�����ַ���Ŀ������<str>���ַ���Ŀ�ܺ͡�<br>
     * @qualification method: ����
     * @derived requirement: ��
     * @implements: 1
     */
    /* @REPLACE_BRACKET: ��ǰ����λ��end���ֶ��� */
    if (0 == UNALIGNED(end))
    {
        aligned_addr = (unsigned long*)end;

        /* @REPLACE_BRACKET: ��ǰ����λ��ΪNULL */
        if (NULL == aligned_addr)
        {
            /* @REPLACE_BRACKET: NULL */
            return(NULL);
        }

        /* @REPLACE_BRACKET: ��ǰ����λ�õ�������û�н����� */
        while (0 == DETECTNULL(*aligned_addr))
        {
            /* @KEEP_COMMENT: ���ֳ������ƶ�end����λ�� */
            aligned_addr++;
        }

        /* @KEEP_COMMENT: ���浱ǰ����λ��end */
        end = (signed char*)aligned_addr;
    }

    /*
     * @brief:
     *    ���<str>�����ֳ����룬�����ֽ�Ϊ��λ����<str>���ַ���Ŀ�������ַ���Ŀ��
     * @qualification method: ����
     * @derived requirement: ��
     * @implements: 2 
     */
    /* @REPLACE_BRACKET: end��ǰ�ֽڲ������ݲ���'\0' */
    while ((*end) != '\0')
    {
        /* @KEEP_COMMENT: ���ֽ������ƶ�end����λ�� */
        end++;
    }

    /* @REPLACE_BRACKET: end-start */
    return(end-start);
}

