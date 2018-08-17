/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ:
 * 2011-02-23  ���£������������ɼ������޹�˾
 *                   �������ļ���
 * 2011-04-15  ���£������������ɼ������޹�˾
 * 				     ����PDL�������Ż����롢�޸�GBJ5369Υ���
 */

/*
 * @file: modf.c
 * @brief:
 *	   <li> modf��ѧ����ʵ���ļ���</li>
 * @implements: DR.2
 */

/* @<MODULE */ 

/************************ͷ �� ��******************************/

/* @<MOD_HEAD */
#include <mathBase.h>
#include <math.h>
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
 *    �зָ�������
 * @param[in]: value: ���зֵĸ�����
 * @param[out]: pIntPart: ����value����������
 * @return: 
 *    ����С�����֣����������<value>��ͬ��
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes: 
 *    ��һ��˫���ȸ��������Ϊ������С�����֡�<br>
 *    ��������������<pIntPart>������С�����֡�<br>
 *    �����ֶ���˫���ȸ�������������<value>��<br>
 *    ZERO��NaN��INF����ѧ����ο�RR.2ע�͡�<br>
 * @implements: DR.2.16
 */
double modf(double value, double *pIntPart)
{
    _DOUBLE     dat = {0.0};
    FAST int    exp_f = 0;
    FAST unsigned int exp_t = 0;
    FAST double fracPart = 0.0;
    FAST double intPart = 0.0;
    int negflag = (int)(value < 0.0);
    unsigned long t = 0;
    unsigned long tt = (unsigned long)-1;

    /* @REPLACE_BRACKET: negflag != 0 */
    if (negflag != 0)
    {
        /* @KEEP_COMMENT: ����<value>�ĸ�ֵ������<value> */
        value = -value;
    }

    /* @KEEP_COMMENT: ת��<value>Ϊ�������ָ�ʽ */
    dat.ddat = value;

    /* @KEEP_COMMENT: ��ȡ<value>ָ����ת��Ϊ2^n��ָ����ʽ������exp_f */
    exp_f = (int)(((unsigned long)(dat.ldat.l1) & (unsigned long)0x7ff00000) >> 20) - 1023;

    /*
     * @brief:
     *    (1)<value>Ϊ��С��(ָ������С��0)������<value>����pIntPart����0��<br>
     *    (2)<value>Ϊ��������(ָ��������(52, +INF))������0����pIntPart����<value>��<br>
     *    (3)<value>ָ��������[0, 20]������sign(value)*INT(|value|)������pIntPart��<br>
     *       ��sign(value)*[|value|-INT(|value|)]��<br>
     *    (4)<value>ָ��������(20, 52]������sign(value)*INT(|value|)������pIntPart��<br>
     *       ��sign(value)*[|value|-INT(|value|)]��<br>
     * @qualification method: ����
     * @derived requirement: ��
     * @notes: 
     *    <value>ָ�������ɹ�ʽ(((value��ǰ�ĸ��ֽ�)&0x7ff00000)>>20)-1023�õ���<br>
     *    sign(value)��ȡ<value>�ķ���λ:value���ڵ���0ʱ��sign(value)=1��<br>
     *                                   valueС��0ʱ��sign(value)=-1��<br>
     *    INT(x)��ȡx���������֡�<br>
     * @implements: 1
     */
    /* @REPLACE_BRACKET: exp_f <= -1 */
    if (exp_f <= -1)
    {
        /* @KEEP_COMMENT: ����fracPart��ֵΪ<value>������intPart��ֵΪ0 */
        fracPart = value;
        intPart = 0.;
    }
    else
    {
        /* ָ��Ϊ���� */
        /* ���value��ָ������ */
        /* @REPLACE_BRACKET: exp_f <= 20 */
        if (exp_f <= 20)
        {
            /* @KEEP_COMMENT: ����exp_tֵΪ�޷��ŵ�����exp_f */
            /* @KEEP_COMMENT: ����tֵΪ�޷��ŵĳ���dat.ldat.l1 */
            exp_t = (unsigned int)exp_f;
            t = (unsigned long)dat.ldat.l1;

            /* @KEEP_COMMENT: ����tֵΪt&(tt<<(20-exp_t)) */
            t &= (tt<<(unsigned long)(20-exp_t));
            /* @KEEP_COMMENT: ����dat.ldat.l1ֵΪ����t������dat.ldat.l2ֵΪ0 */
            dat.ldat.l1 = (long)t;
            dat.ldat.l2 = 0;

            /* @KEEP_COMMENT: ����intPartֵΪdat.ddat */
            intPart = dat.ddat;
            /* @KEEP_COMMENT: ����value - intPart������fracPart */
            fracPart = value - intPart;
        }
        else
        {
            /* @REPLACE_BRACKET: exp_f <= 52 */
            if (exp_f <= 52)
            {
                /* @KEEP_COMMENT: ����exp_tֵΪ(unsigned int)exp_f */
                /* @KEEP_COMMENT: ����tֵΪ(unsigned long)dat.ldat.l2 */
                /* @KEEP_COMMENT: ����tֵΪt&(tt<<(52-exp_t)) */
                exp_t = (unsigned int)exp_f;
                t = (unsigned long)dat.ldat.l2;
                t &= (tt<<(unsigned long)(52-exp_t));

                /* @KEEP_COMMENT: ����dat.ldat.l2ֵΪ(long)t������intPartֵΪdat.ddat */
                dat.ldat.l2 = (long)t;
                intPart = dat.ddat;
                /* @KEEP_COMMENT: ����value-intPart������fracPart */
                fracPart = value-intPart;
            }
            else
            {
                /* @KEEP_COMMENT: ����fracPartֵΪ0.0������intPartֵΪvalue */
                fracPart = 0.;
                intPart = value;
            }
        }
    }

    /*
     * @KEEP_COMMENT: ���negflag != 0������*pIntPart = -intPart��
     * ���negflag = 0������*pIntPart=intPart
     */
    *pIntPart = ((negflag != 0)?-intPart:intPart);

    /* @REPLACE_BRACKET: ���negflag != 0������-fracPart�����򷵻�-fracPart */
    return((negflag != 0)?-fracPart:fracPart);
}

