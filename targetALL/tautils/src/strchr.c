/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2011-02-23         ���࣬�����������ɼ������޹�˾
 *                               �������ļ���
 * 2011-04-15	  ���£������������ɼ������޹�˾
 * 				   ����PDL��������������ʽ���޸�GBJ5369Υ���
*/

/*
 * @file: strchr.c
 * @brief:
 *	   <li> ʵ���ַ���ģ�����strchr��</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/************************ͷ �� ��******************************/

/* @<MOD_HEAD */
#include <string.h>
/* @MOD_HEAD> */

/************************�� �� ��******************************/

/* ���X�����ؼ����ַ����򷵻ط�0 */
#define DETECTCHAR(X,MASK) (DETECTNULL((X) ^ (MASK)))

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
 *    ��ָ�����ַ����в��ҵ�һ������ָ���ַ����ַ���
 * @param[in]: s1: ����Դ�ַ�����ʼ��ַ
 * @param[in]: i: �����ַ�
 * @return: 
 *    ��һ������ָ���ַ����ַ��ĵ�ַ��
 *    NULL: ���ַ�����û���ҵ�ָ���ַ���
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes:
 *    �����в����Դ�ַ�����ʼ��ַ�Ͳ����ַ�����Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.6 
 */
char *strchr(const char *s1, int i)
{
    const unsigned char *s = NULL;
    unsigned char c = (unsigned char)i;
    unsigned long mask = 0, j = 0;
    unsigned long *aligned_addr = NULL;

    s = (const unsigned char *)s1;

    /*
     * @brief:
     *    ���<s1>���ֳ����룬�����ֳ�Ϊ��λ���ҵ���<i>(ת��Ϊ�ַ�)���ַ�������
     *    �����ֳ����ַ����ֽ�Ϊ��λ���в��ң����ز��ҵ����ַ���ַ��<br>
     * @qualification method: ����
     * @derived requirement: ��
     * @notes: 
     *    <s1>��<s2>ָ�����ַ�����β�����н�����'\0'��
     * @implements: 1
     */
    /* @REPLACE_BRACKET: <s1>���ֶ��� */
    if (0 == UNALIGNED(s))
    {
        /* @KEEP_COMMENT: �����ֳ����ҹؼ���mask��ʼΪ0 */
        mask = 0;

        /* @REPLACE_BRACKET: ����j = 0; j < �ֳ�; j++ */
        for (j = 0; j < LBLOCKSIZE; j++)
        {
            /* @KEEP_COMMENT: ����maskΪ(mask<<8) | <i> */
            mask = (mask<<8)|c;
        }

        aligned_addr = (unsigned long*)s;

        /*
         * @REPLACE_BRACKET: <s1>��ǰ�ֳ�����������û�н����� && <s1>��ǰ�ֳ�����
         * ���ݺ�maskû���ַ�ƥ��
         */
        while ((0 == DETECTNULL(*aligned_addr)) && (0 == DETECTCHAR(*aligned_addr, mask)))
        {
            /* @KEEP_COMMENT: ���ֳ������ƶ�<s1>��ǰ����λ�� */
            aligned_addr++;
        }

        /* @KEEP_COMMENT: ���浱ǰ����λ��<s1> */
        s = (unsigned char*)aligned_addr;
    }

    /*
     * @brief:
     *    ���<s1>�����ֳ����룬�����ֽ�Ϊ��λ���ҵ���<i>(ת��Ϊ�ַ�)���ַ�����
     *    �ز��ҵ����ַ���ַ��<br>
     * @qualification method: ����
     * @derived requirement: ��
     * @implements: 2 
     */
    /* ���ֽڽ��в��� */
    /* @REPLACE_BRACKET: <s1>��ǰ�ֽڲ������ݲ���'\0' && <s1>��ǰ�ֽڲ������ݲ�Ϊ<i> */
    while (((*s) != '\0') && ((*s) != c))
    {
        /* @KEEP_COMMENT: ���ֽ������ƶ�<s1>��ǰ����λ�� */
        s++;
    }

    /* @REPLACE_BRACKET: <s1>��ǰ��������Ϊ<i> */
    if (*s == c)
    {
        /* @REPLACE_BRACKET: s��ǰ����λ�� */
        return((char *)s);
    }

    /*
     * @brief:
     *    ������ַ�����û���ҵ�<i>(ת��Ϊ�ַ�)���򷵻�NULL��
     * @qualification method: ����
     * @derived requirement: ��
     * @implements: 3 
     */
    /* @REPLACE_BRACKET: NULL */
    return(NULL);
}

