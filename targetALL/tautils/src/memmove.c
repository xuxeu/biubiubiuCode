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
 * @file: memmove.c
 * @brief:
 *	   <li> ʵ���ַ���ģ�����memmove��</li>
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
 *    ��Դ���򿽱�ָ�������ֽڵ�Ŀ������(���ص���Դ�����Ŀ�����򿽱�)��
 * @param[in]: src_void: ����Դ��ʼ��ַ
 * @param[in]: length: ��������
 * @param[out]: dst_void: ����Ŀ����ʼ��ַ
 * @return: 
 *    ����Ŀ����ʼ��ַ��
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes: 
 *    �����в���鿽��Դ��Ŀ����ʼ��ַ���������ȵ���Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.3
 */
void *memmove(void *dst_void, const void *src_void, size_t length)
{
    signed char *dst = dst_void;
    const signed char *src = src_void;
    long *aligned_dst = NULL;
    const long *aligned_src = NULL;
    int   len =  (int)length;
    unsigned int LongTypeSize = 0;

    /* @KEEP_COMMENT: ����*dstΪĿ�ĵ�ַ<dst_void>������*srcΪԴ��ַ<src_void> */

    /*
     * @brief:
     *    �������Դ�����Ŀ�������ص������<src_void>��<length>���ַ���ַ��
     *    <dst_void>��<length>���ַ���ַ�������ֽ�Ϊ��λ���Ե�ַ�ݼ��ķ�ʽ������
     *    ������������<dst_void>��<br>
     * @qualification method: ����
     * @derived requirement: ��
     * @implements: 1
     */
    /* @REPLACE_BRACKET:(src < dst) && (dst < (src+<length>)) */
    if ((src < dst) && (dst < (src+len)))
    {
        /*
         * @KEEP_COMMENT: src��ǰ����λ�������ƶ�<length>�ֽڵ�Դĩβ��
         * dst��ǰ����λ�������ƶ�<length>�ֽڵ�Ŀ��ĩβ
         */
        src += len;
        dst += len;

        /* @REPLACE_BRACKET: <length>������0 */
        while (0 != len)
        {
            /* @KEEP_COMMENT: <length>-- */
            len--;

            /*
             * @KEEP_COMMENT: ���ֽڿ���src��ǰ���ݵ�dst��ǰλ�ã�
             * �����ֽ���ǰ�ƶ�src��dst�Ŀ���λ��
             */
            dst--;
            src--;
            *dst = *src;
        }
    }
    else
    {
        /*
         * @brief:
         *    ������Դ�����Ŀ������û���ص�ʱ������<dst_void>��<src_void>���ֳ�
         *    ���룬����<length>���ڵ����ֳ�ʱ�������²�����в���:<br>
         *	  (1)<length>���ڵ���4���ֳ����ַ���4���ֳ����п�����<br>
         *	  (2)���ڵ����ֳ���С��4���ֳ����ַ����ֳ����п�����<br>
         *	  (3)С���ֳ����ַ����ֽ�Ϊ��λ���п�����<br>
         *	  (4)����<dst_void>��<br>
         * @qualification method: ����
         * @derived requirement: ��
         * @implements: 2 
         */
        /* @REPLACE_BRACKET: <length>���ڵ����ֳ� && src��dst���ֶ��� */
        if ((0 == TOO_SMALL(len)) && (0 == UNALIGNED_2(src, dst)))
        {
            aligned_dst = (long*)dst;
            aligned_src = (long*)src;

            LongTypeSize = (unsigned int)LBLOCKSIZE;

            /* @REPLACE_BRACKET: <length>���ڵ���4���ֳ� */
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

                /* @KEEP_COMMENT: ����<length>Ϊ<length> - 4���ֳ� */
                len -= (LongTypeSize << 2);
            }

            /* @REPLACE_BRACKET: <length>���ڵ����ֳ� */
            while (len >= LITTLEBLOCKSIZE)
            {
                /*
                 * @KEEP_COMMENT: ���ֳ����ȿ���src��ǰ���ݵ�dst��ǰλ�ã�
                 * �����ֳ����������ƶ�src��dst��ǰ����λ��
                 */
                *aligned_dst = *aligned_src;
                aligned_dst++;
                aligned_src++;

                /* @KEEP_COMMENT: ����<length>Ϊ<length> - �ֳ� */
                len -= LITTLEBLOCKSIZE;
            }

            /* @KEEP_COMMENT: ����src��dst�ĵ�ǰ����λ�� */
            dst = (signed char*)aligned_dst;
            src = (signed char*)aligned_src;
        }

        /*
         * @brief:
         *    ������Դ�����Ŀ������û���ص�ʱ������<dst_void>����<src_void>����
         *    �ֳ����룬����<length>С���ֳ�ʱ�����ֽ�Ϊ��λ���п���������
         *    <dst_void>��<br>
         * @qualification method: ����
         * @derived requirement: ��
         * @implements: 3 
         */
        /* @REPLACE_BRACKET: <length>������0 */
        while (0 != len)
        {
            /* @KEEP_COMMENT: <length>-- */
            len--;

            /*
             * @KEEP_COMMENT: ���ֽ�Ϊ��λ������src��ǰ���ݵ�dst��ǰλ�ã�
             * �����ֽ������ƶ�src��dst��ǰ����λ��
             */
            *dst = *src;
            dst++;
            src++;
        }
    }

    /* @REPLACE_BRACKET: <dst_void> */
    return(dst_void);
}

