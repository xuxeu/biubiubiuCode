/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2011-02-23         ���࣬�����������ɼ������޹�˾
 *                               �������ļ���
 * 2011-04-15	  ���£������������ɼ������޹�˾
 * 				   ����PDL��������������ʽ��
*/

/*
 * @file: memcpy.c
 * @brief:
 *	   <li> ʵ���ַ���ģ�����memcpy��</li>
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
 *    ��Դ���򿽱�ָ�������ֽڵ�Ŀ������
 * @param[in]: src0: ����Դ��ʼ��ַ
 * @param[in]: len0: ��������
 * @param[out]: dst0: ����Ŀ����ʼ��ַ
 * @return: 
 *    ����Ŀ����ʼ��ַ��
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes: 
 *    �����в���鿽��Դ��Ŀ����ʼ��ַ���������ȵ���Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.2
 */
void *memcpy(void *dst0, const void *src0, size_t len0)
{
    signed char *dst = dst0;
    const signed char *src = src0;
    long *aligned_dst = NULL;
    const long *aligned_src = NULL;
    int   len =  (int)len0;
    unsigned int LongTypeSize = 0;

    /*
     * @brief:
     *    ���<src0>��<dst0>���ֳ����룬����<len0>���ڵ����ֳ�ʱ�������²������
     *    ����:<br>
     *    (1)<len0>���ڵ���4���ֳ����ַ���4���ֳ����п�����<br>
     *    (2)���ڵ����ֳ���С��4���ֳ����ַ����ֳ����п�����<br>
     *    (3)С���ֳ����ַ����ֽ�Ϊ��λ���п�����<br>
     *    (4)����<dst0>��<br>
     * @qualification method: ����
     * @derived requirement: ��
     * @implements: 1
     */
    /* @KEEP_COMMENT: ���ÿ���Ŀ�ĵ�ַdetΪ<dst0>�����ÿ���Դ��ʼ��ַsrcΪ<src0> */
#ifndef __MIPS__
    /* @REPLACE_BRACKET: <len0>���ڵ����ֳ� && src��dst���ֶ��� */
    if ((0 == TOO_SMALL(len)) && (0 == UNALIGNED_2(src, dst)))
    {
        aligned_dst = (long*)dst;
        aligned_src = (long*)src;

        LongTypeSize = (unsigned int)LBLOCKSIZE;

        /* @REPLACE_BRACKET: <len0>���ڵ���4���ֳ� */
        while (len >= (LongTypeSize << 2))
        {
            /*
             * @KEEP_COMMENT: ��4���ֳ����ȿ���src��ǰ���ݵ�dst��ǰλ�ã�
             * ����4���ֳ����������ƶ�src��dst��ǰ����λ��
             */
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;

            /* @KEEP_COMMENT: ����<len0>Ϊ<len0>��ȥ4���ֳ� */
            len -= (LongTypeSize<<2);
        }

        /* @REPLACE_BRACKET: <len0>���ڵ����ֳ� */
        while (len >= LITTLEBLOCKSIZE)
        {
            /*
             * @KEEP_COMMENT: ���ֳ����ȿ���src��ǰ���ݵ�dst��ǰλ�ã�
             * �����ֳ����������ƶ�src��dst��ǰ����λ��
             */
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;

            /* @KEEP_COMMENT: ����<len0>Ϊ<len0>��ȥ�ֳ� */
            len -= LITTLEBLOCKSIZE;
        } 

        /* @KEEP_COMMENT: ����src��dst�ĵ�ǰ����λ�� */
        dst = (signed char*)aligned_dst;
        src = (signed char*)aligned_src;
    }
#endif
    /*
     * @brief:
     *    ���<src0>����<dst0>�����ֳ����룬����<len0>С���ֳ�ʱ�����ֽ�Ϊ��λ��
     *    �п���������<dst0>��<br>
     * @qualification method: ����
     * @derived requirement: ��
     * @implements: 2 
     */
    /* @REPLACE_BRACKET: <len0>������0 */
    while (0 != len)
    {
        /* @KEEP_COMMENT: <len0>-- */
        len--;

        /*
         * @KEEP_COMMENT: ���ֽ�Ϊ��λ������src��ǰ���ݵ�dst��ǰλ��
         * �����ֽڽ�src��dst�ӵ�ǰ����λ�������ƶ�
         */
        *dst = *src;
        dst++;
        src++;
    }

    /* @REPLACE_BRACKET: dst0 */
    return(dst0);
}
