/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2011-02-23         ���࣬�����������ɼ������޹�˾
 *                               �������ļ���
 * 2011-04-15	  ���£������������ɼ������޹�˾
 * 				  ����PDL��������������ʽ���޸�GBJ5369Υ���
*/

/*
 * @file: strcpy.c
 * @brief:
 *	   <li> ʵ���ַ���ģ�����strcpy��</li>
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
 *    ��ָ����Դ�ַ������������ַ�(����������)��Ŀ���ַ�����
 * @param[in]: src0: ����Դ��ʼ��ַ
 * @param[in]: dst0: ����Ŀ����ʼ��ַ
 * @return: 
 *    ����Ŀ����ʼ��ַ��
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes: 
 *    �����в���鿽��Դ��Ŀ����ʼ��ַ����Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.8
 */
char *strcpy(char *dst0, const char *src0)
{
    signed char *dst = dst0;
    const signed char *src = src0;
    long *aligned_dst = NULL;
    const long *aligned_src = NULL;

    /* @KEEP_COMMENT: ����dstΪ<dst0>������srcΪ<src0> */

    /*
     * @brief:
     *    ���<src0>��<dst0>���ֳ����룬�����ֳ�Ϊ��λ���п���������ĩβ�����ֳ�
     *    ���ַ����ֽ�Ϊ��λ���п���������<dst0>��<br>
     * @qualification method: ����
     * @derived requirement: ��
     * @implements: 1 
     */
    /* @REPLACE_BRACKET: src��dst���ֶ��� */
    if (0 == UNALIGNED_2(src, dst))
    {
        aligned_dst = (long*)dst;
        aligned_src = (long*)src;

        /* @REPLACE_BRACKET: src��ǰ�ֳ�����������û��NULL�ַ� */
        while (0 == DETECTNULL(*aligned_src))
        {
            /*
             * @KEEP_COMMENT: ���ֳ�����src��ǰ���ݵ�dst��ǰλ�ã�
             * �����ֳ������ƶ�src��dst��ǰ����λ��
             */
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;
        }

        /* @KEEP_COMMENT: ����src��dst�ĵ�ǰ����λ�� */
        dst = (signed char*)aligned_dst;
        src = (const signed char*)aligned_src;
    }

    /*
     * @brief:
     *    ���<src0>��<dst0>�����ֳ����룬�����ֽ�Ϊ��λ���п���������<dst0>��
     * @qualification method: ����
     * @derived requirement: ��
     * @implements: 2 
     */
    /* @REPLACE_BRACKET: src��ǰ�ֽڿ������ݲ��ǽ����� */
    while ((*src) != '\0')
    {
        /*
         * @KEEP_COMMENT: ���ֽ�Ϊ��λ������src��ǰ���ݵ�dst��ǰλ�ã�
         * �����ֽ������ƶ�dst��src��ǰ����λ��
         */
        *dst = *src;
        dst++;
        src++;
    }

    /* @KEEP_COMMENT: ����dst��ǰ�ֽڿ�������Ϊ'\0' */
    *dst = '\0';

    /* @REPLACE_BRACKET: <dst0> */
    return(dst0);
}

