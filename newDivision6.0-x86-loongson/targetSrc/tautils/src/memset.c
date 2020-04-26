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
 * @file: memset.c
 * @brief:
 *	   <li> ʵ���ַ���ģ�����memset��</li>
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
 *    ��ָ�����Ƚ�ָ���ַ������Ԫ�����Ϊָ���ַ���
 * @param[in]: c: ����ַ�
 * @param[in]: n: ��䳤��
 * @param[out]: m: ���Ŀ����ʼ��ַ
 * @return: 
 *    ���Ŀ����ʼ��ַ��
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes: 
 *    �����в�������Ŀ����ʼ��ַ������ַ�����䳤�ȵ���Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.4
 */
void *memset(void *m, int c, size_t n)
{
    signed char *s = NULL;
    int i = 0;
    unsigned int cc = 0;
    unsigned long ct = 0;
    unsigned long buffer = 0;
    unsigned long *aligned_addr = NULL;

    /* @KEEP_COMMENT: ���õ�ǰ���λ��sΪ<m> */
    s = (signed char *) m;

    /*
     * @brief:
     *    ���<n>���ڵ����ֳ�����<m>���ֳ�����ʱ�������²�����в���:<br>
     *    (1)<n>���ڵ���4���ֳ����ַ���4���ֳ���Ԫ�����Ϊ�ַ�<c>��<br>
     *    (2)���ڵ����ֳ���С��4���ֳ����ַ����ֳ��������;<br>
     *    (3)С���ֳ����ַ����ֽ�Ϊ��λ������䣻<br>
     *    (4)����<m>��<br>
     * @qualification method: ����
     * @derived requirement: ��
     * @implements: 1 
     */
    /* @REPLACE_BRACKET: <n>���ڵ����ֳ� && <m>���ֶ��� */
    if ((0 == TOO_SMALL(n)) && (0 == UNALIGNED(m)))
    {
        aligned_addr = (unsigned long*)m;
        cc = (unsigned int)c;
        cc &= 0xff;
        c = (int)cc;
        ct = (unsigned long)cc;

        /* @REPLACE_BRACKET: �ֳ�����4 */
        if (LBLOCKSIZE == 4)
        {
            /*
             * @KEEP_COMMENT: ���ֳ��������buffer�и�2λ�͵�2λ��ÿ���ֽڶ���c��
             * �䲢����
             */
            buffer = (ct << 8)|(ct);
            buffer |= (buffer << 16);
        }
        else
        {
            buffer = 0;

            /* @REPLACE_BRACKET: ����i = 0; i < �ֳ�; i++ */
            for (i = 0; i < (int)LBLOCKSIZE; i++)
            {
                /* @KEEP_COMMENT: ���ֳ��������buffer��ÿ���ֽ���c��䲢���� */
                buffer = (buffer << 8)|ct;
            }
        }

        /* @REPLACE_BRACKET: <n>���ڵ���4���ֳ� */
        while (n >= (size_t)LBLOCKSIZE*4)
        {
            /*
             * @KEEP_COMMENT: ʹ��buffer���4���ֳ����ݵ�s��ǰλ�ã�
             * ����4���ֳ������ƶ����λ��
             */
            *aligned_addr = buffer;
            aligned_addr++;
            *aligned_addr = buffer;
            aligned_addr++;
            *aligned_addr = buffer;
            aligned_addr++;
            *aligned_addr = buffer;
            aligned_addr++;

            /* @KEEP_COMMENT: ����<n>Ϊ<n> - 4���ֳ� */
            n -= 4*((size_t)LBLOCKSIZE);
        }

        /* @REPLACE_BRACKET: <n>���ڵ����ֳ� */
        while (n >= (size_t)LBLOCKSIZE)
        {
            /*
             * @KEEP_COMMENT: ���ֳ����buffer���ݵ�s��ǰλ�ã�
             * �����ֳ������ƶ����λ��
             */
            *aligned_addr = buffer;
            aligned_addr++;

            /* @KEEP_COMMENT: ����<n>Ϊ<n>-�ֳ� */
            n -= (size_t)LBLOCKSIZE;
        }

        /* @REPLACE_LINE: ���浱ǰ���λ��s */
        s = (signed char*)aligned_addr;
    }

    /*
     * @brief:
     *    ���<n>С���ֳ�����<m>�����ֳ����룬�����ֽ�Ϊ��λ��Ԫ�����Ϊ�ַ�<c>��
     *    ����<m>��<br>
     * @qualification method: ����
     * @derived requirement: ��
     * @implements: 2 
     */
    /* @REPLACE_BRACKET: <n>������0 */
    while (0 != n)
    {
        /* @KEEP_COMMENT: <n>-- */
        n--;

        /* @KEEP_COMMENT: ���ֽ����c����ǰλ��s�������ֽ������ƶ����λ�� */
        *s = (signed char)c;
        s++;
    }

    /* @REPLACE_BRACKET: <m> */
    return(m);
}

