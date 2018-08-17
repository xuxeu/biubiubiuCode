/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2011-09-07         ��ѩ�ף������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:strtoi.c
 * @brief:
 *             <li>MIPS��ϵͳ�ṹ�µ�һЩ����ʵ��</li>
 * @implements:DTA.3.84 DTA.3.85
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
/************************ʵ   ��*******************************/

/* @MODULE> */


/*
 * @brief:
 *     ���ַ���ת����������uwBaseΪ0ʱ��ת���������ַ���������
 * @param[in]:   ubpNptr: �����ַ���
 * @param[in]:   uwBase: ת���Ľ���
 * @return:
 *     ת���������
 *     MIN_INT32: ����������ܱ�ʾ����С������С
 *     MAX_INT32: ����������ܱ�ʾ�������������
 * @tracedREQ: RTA.3
 * @implements: DTA.3.98
 */
INT32 strtoi(const UINT8 *ubpNptr, UINT32 uwBase)
{
	#define MIN_INT32    	(0x80000000)
	#define MAX_INT32    	(0x7fffffff)
    register const UINT8 *s = ubpNptr;
    register INT32 acc = 0;
    register INT32 c = 0;
    register UINT32 cutoff = 0;
    register INT32 neg = 0;
    register INT32 any = 0;
    register INT32 cutlim = 0;

    do
    {
        /* �����ո�� */
        /* @KEEP_COMMENT: ��¼sָ��<ubpNptr>ָ��Ļ��壬��¼cΪsָ����ַ���s�Լ�1ָ����һ���ַ� */
        c = *s;
        s++;
        /* @REPLACE_BRACKET: c����' ' */
    } while (' '== c);

    /* @REPLACE_BRACKET: c����'-' */
    if (c == '-')
    {
        /* @KEEP_COMMENT: ���÷��ű�־��Ϊ1������cֵΪsָ����ַ���s�Լ�1ָ����һ���ַ� */
        neg = 1;
        c = *s;
        s++;
    }

    /* @REPLACE_BRACKET: c����'+' */
    if (c == '+')
    {
        /* @KEEP_COMMENT: ����'+'���ű�־λ���䣬����cֵΪsָ����ַ���s�Լ�1ָ����һ���ַ� */
        c = *s;
        s++;
    }

    /* @KEEP_COMMENT: ȷ��ת���Ľ��� */

    /* @REPLACE_BRACKET: <uwBase>����0����16 && c����'0' && s����'x'����'X' */
    if (( (uwBase == 0) || (uwBase == 16)) && (c == '0') && ((*s == 'x') || (*s == 'X')))
    {
        /* @KEEP_COMMENT: ����cֵΪs+1ָ����ַ���s�Լ�2����<uwBase>ֵΪ16����ʾ��Ϊ16���� */
        c = s[1];
        s += 2;
        uwBase = 16;
    }

    /* @REPLACE_BRACKET: <uwBase>����0 */
    if (uwBase == 0)
    {
        /* @KEEP_COMMENT: ���c����'0'������<uwBase>Ϊ8����������<uwBase>Ϊ10 */
        uwBase = (c == '0' ? 8 : 10);
    }

    /* 
     * @KEEP_COMMENT: ���������cutoff�������ת����Ϊ�����������������cutoffΪ0x7FFFFFFF��
     * �������������cutoffΪ0x80000000������cutoff��С<uwBase>����
     * ��¼�����cutoff����<uwBase>������Ϊcutlim��
     */
    cutoff = (neg != 0) ? -(UINT32)MIN_INT32 : MAX_INT32;
    cutlim = (INT32)(cutoff % uwBase);
    cutoff /= uwBase;
    acc = 0;
    any = 0;

    /* @KEEP_COMMENT: ���ݽ��ƽ��ַ���ת��Ϊ��Ӧ������ */
    /* @REPLACE_BRACKET: ת�������acc��ֵΪ0;;����cֵΪsָ����ַ���s�Լ�1 */
    for (;; )
    {
        /* @REPLACE_BRACKET: c��ֵ��'0'��'9'֮�� */
        if ((c >= '0') && (c <= '9'))
        {
            /* @KEEP_COMMENT: ����cֵΪc��ȥ'0' */
            c -= '0';
        }
        else
        {
            /* @REPLACE_BRACKET: c��ֵ��'a'��'z'֮�� */
            if ((c >= 'a') && (c<= 'z'))
            {
                /* @KEEP_COMMENT: ����cֵΪc��ȥ'a'��10 */
                c -= (INT32)('a' - 10);
            }
            else
            {
                /* @REPLACE_BRACKET: c��ֵ��'A'��'Z'֮�� */
                if ((c >= 'A') && (c<= 'Z'))
                {
                    /* @KEEP_COMMENT: ����cֵΪc��ȥ'A'��10 */
                    c -= (INT32)('A' - 10);
                }
                else
                {
                    break;
                }
            }
        }

        /* ת������������ڽ��ƵĻ��������� */
        
        /* @REPLACE_BRACKET: ת����cֵ���С��0���ߴ��ڽ��ƻ���<uwBase> */
        if ((c < 0) || (c >= (INT32)uwBase))
        {
            break;
        }

        /* ���ת������������������anyΪ-1 */
        /* @REPLACE_BRACKET: �Ѿ�������� || ת�������acc����cutoff || (ת�������acc����cutoff����c����cutlim) */
        if ((any < 0) || (acc > (INT32)cutoff) || ((acc == (INT32)cutoff) && (c > cutlim)))
        {
            /* 
             * �ж�����acc == cutoff && c > cutlim ����Ϊ��ǰ�潫cutoff��С��base����
             * cutlimΪ������Ҫʹ acc * uwBase + c С�������cutoff
             */
            /* @KEEP_COMMENT: ���������־����ʾ�Ѿ���� */
            any = -1;
        }
        else
        {
            /* @KEEP_COMMENT: ���ݽ��ƵĻ����ۼ�ÿ��ת��������֣�acc����<uwBase>�����ټ��ϵ�ǰֵc */
            any = 1;
            acc *= (INT32)uwBase;
            acc += c;
        }
        c = *s;
        s++;
    }

    /* anyС��0��ʾת�����������������ݷ��ŷ�����Ӧ�ĺ�ֵ */
    /* @REPLACE_BRACKET: ת������������ */
    if (any < 0)
    {
        /* 
         * @KEEP_COMMENT: ���������cutoff�������ת����Ϊ������ת������accΪ0x7FFFFFFF��
         * ��������ת������accΪ0x80000000
         */
        acc = ((neg != 0)? MIN_INT32 : MAX_INT32);
    }
    else
    {
        /* @REPLACE_BRACKET: ��ת�����Ǹ��� */
        if (0 != neg)
        {
            /* @KEEP_COMMENT: ����ת����ȡ�� */
            acc = -acc;
        }
    }

    /* @REPLACE_BRACKET: ת���������acc */
    return (acc);
}
