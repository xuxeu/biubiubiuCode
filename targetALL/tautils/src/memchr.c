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
 * @file: memchr.c
 * @brief:
 *	   <li> ʵ���ַ���ģ�����memchr��</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/************************ͷ �� ��******************************/

/* @<MOD_HEAD */
#include <string.h>
#include <stdarg.h>
#include <floatio.h>
/* @MOD_HEAD> */

/************************�� �� ��******************************/
/************************���Ͷ���******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/
/************************ģ�����******************************/
/************************ȫ�ֱ���******************************/
/************************ʵ    ��******************************/

/* @MODULE> */


/*
 * @brief: 
 *    ��ָ��������ָ�����ַ�������в��ҵ�һ������ָ���ַ���Ԫ�ء�
 * @param[in]: src_void: ����Դ��ʼ��ַ
 * @param[in]: c: �����ַ�
 * @param[in]: length: ���ҳ���
 * @return: 
 *    ��һ������ָ���ַ���Ԫ�ص�ַ��<br>
 *    NULL: û���ҵ�ָ���ַ���<br>
 * @qualification method: ����/����
 * @derived requirement: ��
 * @function location: Internal
 * @implements: DR.1.20
 */
void *memchr(const void *src_void, int c, size_t length)
{
    const unsigned char *src = (const unsigned char *) src_void;
    unsigned long *asrc = NULL;
    unsigned long  buffer = 0;
    unsigned long  mask = 0;
    unsigned int cc = 0;
    int i = 0, j = 0;

    /* @KEEP_COMMENT: ���õ�ǰ����λ��srcΪ<src_void> */

    /* @KEEP_COMMENT: ��<c>ת��Ϊ�ַ�<c> */
    cc = (unsigned int)c;
    cc &= 0xff;
    c = (int)cc;

    /* @REPLACE_BRACKET: <length>���ڵ����ֳ� && src���ֳ����� */
    if ((0 == TOO_SMALL(length)) && (0 == UNALIGNED(src)))
    {
        /* @KEEP_COMMENT: ����asrcΪsrc�������ֳ����ҹؼ���mask��ʼΪ0 */
        asrc = (unsigned long*) src;
        mask = 0;

        /* @REPLACE_BRACKET: ����i = 0; i < �ֳ�; i++ */
        for (i = 0; i < (int)LBLOCKSIZE; i++)
        {
            /* @KEEP_COMMENT: ����maskΪ(mask << 8) + <c> */
            mask = (mask << 8) + c;
        }

        /* @REPLACE_BRACKET: <length>���ڵ����ֳ� */
        while (length >= (size_t)LBLOCKSIZE)
        {
            /* @KEEP_COMMENT: ��ǰ�ֳ��Ƚ����ݺ�mask���бȽ� */
            buffer = *asrc;
            buffer ^=  mask;

            /* @REPLACE_BRACKET: �Ƚ����ݺ�mask���ַ�ƥ�� */
            if (0 != DETECTNULL(buffer))
            {
                /* @KEEP_COMMENT: src = asrc */
                src = (const unsigned char*) asrc;

                /* @REPLACE_BRACKET: ����j = 0; j < �ֳ�; j++ */
                for (j = 0; j < (int)LBLOCKSIZE; j++)
                {
                    /* @KEEP_COMMENT: ��ǰsrc�Ƚ����ݵ���<c> */
                    if (*src == c)
                    {
                        /* @REPLACE_BRACKET: ��ǰ�Ƚ�λ��src */
                        return((signed char*) src);
                    }
                    /* @KEEP_COMMENT: ���ֽ������ƶ��Ƚ�λ�� */
                    src++;
                }
            }

            /* @KEEP_COMMENT: ����<length>Ϊ<length>���ֳ��������ֳ������ƶ��Ƚ�λ�� */
            length -= (size_t)LBLOCKSIZE;
            asrc++;
        }

        /* @KEEP_COMMENT: ���浱ǰ�Ƚ�λ��src */
        src = (const unsigned char*) asrc;
    }

    /* @REPLACE_BRACKET: <length>������0 */
    while (0 != length)
    {
        /* @KEEP_COMMENT: <length>-- */
        length--;

        /* @REPLACE_BRACKET: ��ǰ�ֽڱȽ�����src����<c> */
        if (*src == c)
        {
            /* @REPLACE_BRACKET: ��ǰ�Ƚ�λ��src */
            return((signed char*) src);
        }

        /* @KEEP_COMMENT: ���ֽ������ƶ��Ƚ�λ�� */
        src++;
    }

    /* @REPLACE_BRACKET: NULL */
    return(NULL);
}

