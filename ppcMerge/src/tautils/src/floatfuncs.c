/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/
 
/*
 * �޸���ʷ��
 * 2011-02-23         ���࣬�����������ɼ������޹�˾
 *                               �������ļ���
 * 2011-04-15	  ���£������������ɼ������޹�˾
 * 				  ����PDL��������������ʽ���Ż����롢�޸�GBJ5369Υ���
  * 				  �޸���exponentCvt���ȴ�ӡ����ȷ�����⡣
*/

/*
 * @file: floatfuncs.c
 * @brief:
 *	   <li> �ṩ��������صĺ���֧�֡�</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/************************ͷ �� ��******************************/

/* @<MOD_HEAD */
#include <string.h>
//#include <mathBase.h>
#include <math.h>
#include <stdarg.h>
#include <floatio.h>
/* @MOD_HEAD> */

/************************�� �� ��******************************/
/************************���Ͷ���******************************/
/************************�ⲿ����******************************/

/*
 * @brief: 
 *    ��������ת��Ϊһ���ַ����顣
 * @param[in]: number: ������
 * @param[in]: prec: ��ȷ��
 * @param[in]: doAlt: �����־
 * @param[in]: fmtch: �����ĸ�ʽ����
 * @param[in]: endp: ������������ַ
 * @param[out]: pDoSign: ���ű�־
 * @param[out]: startp: ��������ʼ��ַ
 * @return:
 *    ת���������ַ�������
 * @qualification method: ����/����
 * @derived requirement: ��
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.17
 */
static int cvt(double number,
			   int prec,
			   BOOL doAlt,
			   int fmtch,
			   BOOL *pDoSign,
			   signed char *startp,
			   signed char *endp);


/*
 * @brief: 
 *    ����ֵת��ΪASCII�ַ����������ָ�����ڴ�����
 * @param[in]: e: �ݵ�ֵ
 * @param[in]: fmtch: ��ʽ�ַ�
 * @param[out]: p: ����ڴ�����ʼ��ַ
 * @return:
 *    ָ��ת���������ַ���β��ָ��
 * @qualification method: ����/����
 * @derived requirement: ��
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.19
 */
static signed char *exponentCvt(signed char *p,
								int e,
								int fmtch);


/*
 * @brief: 
 *    ���ݸ�������ֵ��Ҫ��ĸ�ʽ�Ը���������ת����ȡ��������
 * @param[in]: fract: ������  
 * @param[in]: ch: ������Դ�С�Ļ�׼ֵ
 * @param[in]: end: ������������ַ 
 * @param[out]: e: ��
 * @param[out]: pDoSign: ���ű�־
 * @param[out]: start: ��������ʼ��ַ
 * @return:
 *    ָ��ת���������ַ�����ָ�롣
 * @qualification method: ����/����
 * @derived requirement: ��
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.18
 */
static signed char *roundCvt(double fract,
							 int *e,
							 signed char *start,
							 signed char *end,
							 signed char ch,
							 BOOL *pDoSign);

/************************ǰ������******************************/
/************************ģ�����******************************/
/************************ȫ�ֱ���******************************/
/************************ʵ    ��******************************/

/* @MODULE> */


/*
 * @brief
 *    ��������ת��ΪASCII�ַ�����������ת���������ַ������ȡ�
 * @param[in]: pVaList: �����б� 
 * @param[in]: precision: ��ȷ��
 * @param[in]: doAlt: �����־
 * @param[in]: fmtSym: �����ĸ�ʽ����
 * @param[in]: pBufEnd: ������������ַ
 * @param[out]: pDoSign: ���ű�־
 * @param[out]: pBufStart: ��������ʼ��ַ
 * @return:
 *    ת���������ַ�������
 * @qualification method: ����/����
 * @derived requirement: ��
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.16
 */
int fformat(va_list *pVaList,
int precision,
BOOL doAlt,
int fmtSym,
BOOL *pDoSign,
signed char *pBufStart,
signed char *pBufEnd)
{
    /* @KEEP_COMMENT: �ӱ���б�<*pVaList>��ȡdouble���͵�ֵ�����������dbl */
    double dbl = (double) va_arg(*pVaList, double);

    /* @KEEP_COMMENT: ����<*pDoSign>ΪFALSE */
    *pDoSign = FALSE;

    /* @REPLACE_BRACKET: dblΪINF */
    if (INF == fpTypeGet(dbl, NULL))
    {
        /* @KEEP_COMMENT: ����"Inf"��<pBufStart> */
        strcpy(pBufStart, "Inf");

        /* @REPLACE_BRACKET: dblС��0.0 */
        if (dbl < 0.0)
        {
            /* @KEEP_COMMENT: ����<*pDoSign>ΪTRUE */
            *pDoSign = TRUE;
        }

        /* @REPLACE_BRACKET: -3 */
        return(-3);
    }

    /* @REPLACE_BRACKET: dblΪNAN */
    if (NAN == fpTypeGet(dbl, NULL))
    {
        /* @KEEP_COMMENT: ����"NaN"��<pBufStart> */
        strcpy(pBufStart, "NaN");

        /* @REPLACE_BRACKET: -3 */
        return(-3);
    }

    /* @KEEP_COMMENT: ����cvt(DR.1.16)��db1ת��Ϊ�ַ�������¼����ֵ��ret */
    /* @REPLACE_BRACKET: ret */
    return(cvt(dbl, precision, doAlt, fmtSym, pDoSign, pBufStart, pBufEnd));
}

/*
 * @brief: 
 *    ��������ת��Ϊһ���ַ����顣
 * @param[in]: number: ������
 * @param[in]: prec: ��ȷ��
 * @param[in]: doAlt: �����־
 * @param[in]: fmtch: �����ĸ�ʽ����
 * @param[in]: endp: ������������ַ
 * @param[out]: pDoSign: ���ű�־
 * @param[out]: startp: ��������ʼ��ַ
 * @return:
 *    ת���������ַ�������
 * @qualification method: ����/����
 * @derived requirement: ��
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.17
 */
static int cvt(double number,
int prec,
BOOL doAlt,
int fmtch,
BOOL *pDoSign,
signed char *startp,
signed char *endp)
{
    signed char *p = NULL;
    signed char *t = NULL;
    signed char *tch = NULL;
    signed char *tmpChrAdd = NULL;
    double fract = 0.0;
    int     dotrim = 0;
    int     expcnt = 0;
    int     gformat = 0;
    int     nonZeroInt = 0;
    double  integer = 0.0;
    double  tmp = 0.0;
    BOOL ZeroSign = FALSE;

    dotrim = 0;
    expcnt = 0;
    gformat = 0;

    /* @REPLACE_BRACKET: <number>С��0.0 */
    if (number < 0.0)
    {
        /* @KEEP_COMMENT: ����<number>Ϊ-<number>������<*pDoSign>ΪTRUE */
        number = -number;
        *pDoSign = TRUE;
    }
    else
    {
        /* @KEEP_COMMENT: ����<*pDoSign>ΪFALSE */
        *pDoSign = FALSE;
    }

    /*
     * @KEEP_COMMENT: ��������<number>��ֳ�������С�����֣�integer����������֣�
     * ����С�����ִ����fract
     */
    fract = modf(number, &integer);

    /* @REPLACE_BRACKET: integer������0 */
    if (integer != 0.0)
    {
        /* @KEEP_COMMENT: ���ñ�ʶnumberΪ����ľֲ�����nonZeroIntΪTRUE */
        nonZeroInt = TRUE;
    }

    /* @KEEP_COMMENT: ��<startp>++ָ�뱣������ʱ����t */
    startp++;
    t = startp;

    /* �ж����������Ƿ����0 */
    /* @KEEP_COMMENT: ���integerΪ0������ZeroSignΪTRUE����������ZeroSignΪFALSE */
    ZeroSign = (0 == integer);

    /*
     * @KEEP_COMMENT: ����ֲ�����p��Ϊ��������ָ�룬������ʱ����expcnt��¼number
     * �������ֵ�λ��
     */

    /* @REPLACE_BRACKET: ����pΪ<endp>��1; ZeroSignΪFALSE; ++expcnt */
    for (p = endp - 1; !ZeroSign; ++expcnt)
    {
        /*
         * @KEEP_COMMENT: ���integer/10Ϊ������С�����֣�integer����������֣�С
         * �����ֱ�����tmp
         */
        tmp  = modf(integer / 10, &integer);

        /* @KEEP_COMMENT: ��(tmp+0.01)*10ת����ASCII�ַ�������*p */
        *p = to_char((tmp + .01) * 10);

        /* @KEEP_COMMENT: p-- */
        p--;

        /* @KEEP_COMMENT: ���integer����0������ZeroSignΪTRUE����������ZeroSignΪFALSE */
        ZeroSign = (0 == integer);
    }

    /* @REPLACE_BRACKET: <fmtch> */
    switch (fmtch)
    {
        case 'f':
            /* @REPLACE_BRACKET: expcnt������0 */
            if (0 != expcnt)
            {
                /* @KEEP_COMMENT: p++������tmpChrAddΪ<endp> */
                p++;
                tmpChrAdd = endp;

                /* @REPLACE_BRACKET: pС��tmpChrAdd */
                while (p < tmpChrAdd)
                {
                    /* @KEEP_COMMENT: ����*tΪ*p��t++��p++ */
                    *t = *p;
                    t++;
                    p++;
                }
            }
            else
            {
                /* @KEEP_COMMENT: ����*tΪ'0'��t++ */
                *t = '0';
                t++;
            }

            /* @REPLACE_BRACKET: <prec>������0 || <doAlt>������0 */
            if ((0 != prec) || (0 != doAlt))
            {
                /* @KEEP_COMMENT: ����*tΪ'.'��t++ */
                *t = '.';
                t++;
            }

            /* ���Ҫ����ߵľ��� */
            /* @REPLACE_BRACKET: fract������0.0 */
            if (0.0 != fract)
            {
                /* @REPLACE_BRACKET: <prec>������0.0 */
                if (0 != prec)
                {
                    do 
                    {
                        /*
                         * @KEEP_COMMENT: ���fract*10��������С�����֣�tmp�����
                         * �����֣�����С�����ֱ�����fract
                         */
                        fract = modf(fract * 10, &tmp);

                        /* @KEEP_COMMENT: ��tmpת����ASCII�ַ�������*t */
                        *t = to_char(tmp);

                        /* @KEEP_COMMENT: t++��prec-- */
                        t++;
                        prec--;

                        /* @REPLACE_BRACKET: <prec>������0 && fract������0.0 */
                    } while ((0 != prec) && (0.0 != fract));
                }

                /* @REPLACE_BRACKET: fract������0.0 */
                if (0.0 != fract)
                {
                    /* @KEEP_COMMENT: ������ʱ����tchΪt-1 */
                    tch = t-1;

                    /*
                     * @KEEP_COMMENT: ��������fract����ʽ����ת����ȡ��������
                     * <startp>ָ��ת������ַ���
                     */
                    startp = roundCvt(fract, NULL, startp, tch, 0x0, pDoSign);
                }
            }

            /* @REPLACE_BRACKET: <prec>������0.0 */
            while (prec != 0)
            {
                /* @KEEP_COMMENT: �����ʱû�дﵽ<prec>ָ����λ����ʣ�ಿ�ֲ���0 */
                prec--;
                *t = '0';
                t++;
            }

            break;

        case 'g':
            /* @REPLACE_BRACKET: <prec>����0 */
            if (0 == prec)
            {
                /* @KEEP_COMMENT: ++<prec> */
                ++prec;
            }

            /*
             * ʹ�õķ��ȡ����ת��������ֵ��
             * ���������С��0.0001����ھ�����ʹ�ÿ�ѧ������ʽ
             */
            /*
             * @REPLACE_BRACKET: (expcntΪ0 && fract��Ϊ0.0 && fractС��0.0001)
             *  || expcnt����<prec> 
             */
            if ((expcnt > prec) || ((0 == expcnt) && (0.0 != fract) && (fract < 0.0001)))
            {
                /*
                 * g/G��ʽ��������������ֶ�����ָ��
                 * ��Ӧe/Eg��ʽ����ᵼ��һ��off-by-one���⣬
                 * g/G�ὫС����ǰ������������������֣�
                 * ��e/E��������Ǽ��뾫�ȡ�
                 */
                /* @KEEP_COMMENT: --<prec> */
                --prec;

                /* @KEEP_COMMENT: ����<fmtch>Ϊ<fmtch>��2������gformatΪ1 */
                fmtch  -= 2;
                gformat = 1;
            }
            else
            {
                /* ����������ת������������ǰ�ˡ�ע�⽫���پ��� */
                /* @REPLACE_BRACKET: expcnt������0 */
                if (0 != expcnt)
                {
                    /* @KEEP_COMMENT: p++ */
                    p++;

                    /* @KEEP_COMMENT: ����tmpChrAddΪ<endp> */
                    tmpChrAdd = endp;

                    /* @REPLACE_BRACKET: pС��tmpChrAdd */
                    while (p < tmpChrAdd)
                    {
                        /* @KEEP_COMMENT: ����*tΪ*p��t++ */
                        *t = *p;
                        t++;

                        /* @KEEP_COMMENT: --<prec>��p++ */
                        --prec;
                        p++;
                    }
                }
                else
                {
                    /* ��'0'��� */
                    /* @KEEP_COMMENT: ����*tΪ'0'��t++ */
                    *t = '0';
                    t++;
                }

                /*
                 * ���Ҫ���˾��Ȼ�������ת����־�������һ��С����
                 * �����ʱ��δ�������֣�����ǰ����0
                 */
                /* @REPLACE_BRACKET: <prec>������0 || <doAlt>������0 */
                if ((0 != prec) || (0 != doAlt))
                {
                    /* @KEEP_COMMENT: ����dotrimΪ1 */
                    dotrim = 1;

                    /* ���С���� */
                    /* @KEEP_COMMENT: ����*tΪ'.'��t++ */
                    *t = '.';
                    t++;
                }
                else
                {
                    /* @KEEP_COMMENT: ����dotrimΪ0 */
                    dotrim = 0;
                }

                /* ���Ҫ����ߵľ�������ĩβ���С�� */
                /* @REPLACE_BRACKET: fract������0.0 */
                if (0.0 != fract)
                {
                    /* @REPLACE_BRACKET: <prec>������0 */
                    if (0 != prec)
                    {

                        /* �������������0�����ǲ��ܽ���ͷ��0��Ϊ��������� */
                        /* @REPLACE_BRACKET: nonZeroIntΪFALSE */
                        if (FALSE == nonZeroInt)
                        {
                            do 
                            {
                                /*
                                 * @KEEP_COMMENT: ���fract*10��������С�����֣�
                                 * tmp����������֣�С�����ֱ�����fract
                                 */
                                fract = modf(fract * 10, &tmp);

                                /* @KEEP_COMMENT: ת��tmpΪASCII�ַ�������*t */
                                *t = to_char(tmp);

                                /* @KEEP_COMMENT: t++ */
                                t++;

                                /* @REPLACE_BRACKET: tmp����0.0 */
                            } while (0.0 == tmp);

                            /* @KEEP_COMMENT: <prec>-- */
                            prec--;
                        }

                        /* ��ӵ��ھ������������� */
                        /* @REPLACE_BRACKET: <prec>������0 && fract������0 */
                        while ((0 != prec) && (0 != fract))
                        {
                            /*
                             * @KEEP_COMMENT: ���fract*10��������С�����֣�tmp��
                             * ���������֣�С�����ֱ�����fract
                             */
                            fract = modf(fract * 10, &tmp);

                            /* @KEEP_COMMENT: ת��tmpΪASCII�ַ�������*t */
                            *t = to_char(tmp);

                            /* @KEEP_COMMENT: t++��<prec>-- */
                            t++;
                            prec--;
                        }
                    }

                    /* @REPLACE_BRACKET: fract������0 */
                    if (0.0 != fract)
                    {
                        /* @KEEP_COMMENT: ����tchΪt-1 */
                        tch = t-1;

                        /*
                         * @KEEP_COMMENT: ��������fract����ʽ����ת����ȡ��������
                         * startpָ��ת������ַ���
                         */
                        startp = roundCvt (fract, NULL, startp, tch, 0x0, pDoSign);
                    }
                }

                /* ת����ʽ�����ݾ���Ҫ����ӻ�ɾ��0 */
                /* @REPLACE_BRACKET: <doAlt>������0 */
                if (0 != doAlt)
                {
                    /* @REPLACE_BRACKET: <prec>������0 */
                    while (prec != 0)
                    {
                        /* @KEEP_COMMENT: <prec>-- */
                        prec--;

                        /* ���'0' */
                        /* @KEEP_COMMENT: ����*tΪ'0'��t++ */
                        *t = '0';
                        t++;
                    }
                }
                else
                {
                    /* @REPLACE_BRACKET: dotrim������0 */
                    if (0 != dotrim)
                    {
                        /* @KEEP_COMMENT: t-- */
                        t--;

                        /* @KEEP_COMMENT: ����tmpChrAddΪ<startp> */
                        tmpChrAdd = startp;

                        /* @REPLACE_BRACKET: t����tmpChrAdd && *t����'0' */
                        while ((t > tmpChrAdd ) && ('0' == *t))
                        {
                            /* ɾ��'0' */
                            /* @KEEP_COMMENT: t-- */
                            t--;
                        }

                        /* @REPLACE_BRACKET: *t������'.' */
                        if (*t != (signed char)'.')
                        {
                            /* @KEEP_COMMENT: ++t */
                            ++t;
                        }
                    }
                }

                break;
            }

        case 'G':
            /* ����0������1 */
            /* @REPLACE_BRACKET: <prec>����0 */
            if (0 == prec)
            {
                /* @KEEP_COMMENT: ++<prec> */
                ++prec;
            }

            /*
             * ʹ�õķ��ȡ����ת��������ֵ��
             * ���������С��0.0001����ھ�����ʹ�ÿ�ѧ������ʽ
             */
            /* @REPLACE_BRACKET: (expcnt>prec)||((0==expcnt)&&(0!=fract)&&(fract<0.0001)) */
            if ((expcnt > prec) || ((0 == expcnt) && (0 != fract) && (fract < .0001)))
            {
                /*
                 * g/G��ʽ��������������ֶ�����ָ��
                 * ��Ӧe/Eg��ʽ����ᵼ��һ��off-by-one���⣬
                 * g/G�ὫС����ǰ������������������֣�
                 * ��e/E��������Ǽ��뾫�ȡ�
                 */
                /* @KEEP_COMMENT: --<prec> */
                --prec;

                /* G->E, g->e */
                /* @KEEP_COMMENT: ����<fmtch>Ϊ<fmtch>��2 */
                fmtch  -= 2;

                /* @KEEP_COMMENT: ����gformatΪ1 */
                gformat = 1;
            }
            else
            {
                /* ����������ת������������ǰ�ˡ�ע�⽫���پ���*/
                /* @REPLACE_BRACKET: expcnt������0 */
                if (0 != expcnt)
                {
                    /* @KEEP_COMMENT: p++������tmpChrAddΪ<endp> */
                    p++;
                    tmpChrAdd = endp;

                    /* @REPLACE_BRACKET: pС��tmpChrAdd */
                    while (p < tmpChrAdd)
                    {
                        /* @KEEP_COMMENT: ����*tΪ*p��t++ */
                        *t = *p;
                        t++;

                        /* @KEEP_COMMENT: --<prec>��p++ */
                        --prec;
                        p++;
                    }
                }
                else
                {
                    /* ��'0'��� */
                    /* @KEEP_COMMENT: ����*tΪ'0'��t++ */
                    *t = '0';
                    t++;
                }

                /*
                 * ���Ҫ���˾��Ȼ�������ת����־�������һ��С����
                 * �����ʱ��δ�������֣�����ǰ����0
                 */
                /* @REPLACE_BRACKET: <prec>������0 || <doAlt>������0 */
                if ((0 != prec) || (0 != doAlt))
                {
                    /* ���С���� */
                    /* @KEEP_COMMENT: ����dotrimΪ1������*tΪ'.'��t++ */
                    dotrim = 1;
                    *t = '.';
                    t++;
                }
                else
                {
                    /* @KEEP_COMMENT: ����dotrimΪ0 */
                    dotrim = 0;
                }

                /* ���Ҫ����ߵľ�������ĩβ���С�� */
                /* @REPLACE_BRACKET: fract������0.0 */
                if (0.0 != fract)
                {
                    /* @REPLACE_BRACKET: <prec>������0 */
                    if (0 != prec)
                    {
                        /* �������������0�����ǲ��ܽ���ͷ��0��Ϊ���������*/
                        /* @REPLACE_BRACKET: nonZeroInt����FALSE */
                        if (FALSE == nonZeroInt)
                        {
                            do 
                            {
                                /*
                                 * @KEEP_COMMENT: ���fract*10��������С�����֣�
                                 * tmp����������֣�С�����ֱ�����fract
                                 */
                                fract = modf(fract * 10, &tmp);

                                /* @KEEP_COMMENT: ת��tmpΪASCII�ַ�������*t */
                                *t = to_char(tmp);

                                /* @KEEP_COMMENT: t++ */
                                t++;

                                /* @REPLACE_BRACKET: tmp����0.0 */
                            } while (0.0 == tmp);

                            /* @KEEP_COMMENT: <prec>-- */
                            prec--;
                        }

                        /* ��ӵ��ھ�������������*/
                        /* @REPLACE_BRACKET: <prec>������0 && fract������0 */
                        while ((0 != prec) && (0 != fract))
                        {
                            /*
                             * @KEEP_COMMENT: ���fract*10��������С�����֣�tmp��
                             * ���������֣�С�����ֱ�����fract
                             */
                            fract = modf(fract * 10, &tmp);

                            /* @KEEP_COMMENT: ת��tmpΪASCII�ַ�������*t */
                            *t = to_char(tmp);

                            /* @KEEP_COMMENT: t++��<prec>-- */
                            t++;
                            prec--;
                        }
                    }

                    /* @REPLACE_BRACKET: fract������0 */
                    if (0.0 != fract)
                    {
                        /* @KEEP_COMMENT: ����tchΪt-1 */
                        tch = t-1;

                        /*
                         * @KEEP_COMMENT: ��������fract����ʽ����ת����ȡ��������
                         * startpָ��ת������ַ���
                         */
                        startp = roundCvt (fract, NULL, startp, tch, 0x0, pDoSign);
                    }
                }

                /* ת����ʽ�����ݾ���Ҫ����ӻ�ɾ��0 */
                /* @REPLACE_BRACKET: <doAlt>������0 */
                if (0 != doAlt)
                {
                    /* @REPLACE_BRACKET: <prec>������0 */
                    while (prec != 0)
                    {
                        /* ���'0' */
                        /* @KEEP_COMMENT: <prec>--������*tΪ'0'��t++ */
                        prec--;
                        *t = '0';
                        t++;
                    }
                }
                else
                {
                    /* @REPLACE_BRACKET: dotrim������0 */
                    if (0 != dotrim)
                    {
                        /* @KEEP_COMMENT: t--������tmpChrAddΪ<startp> */
                        t--;
                        tmpChrAdd = startp;

                        /* @REPLACE_BRACKET: t����tmpChrAdd && *t����'0' */
                        while ((t > tmpChrAdd) && ('0' == *t))
                        {
                            /* ɾ��'0' */
                            /* @KEEP_COMMENT: t-- */
                            t--;
                        }

                        /* @REPLACE_BRACKET: *t������'.' */
                        if (*t != (signed char)'.')
                        {
                            /* @KEEP_COMMENT: ++t */
                            ++t;
                        }
                    }
                }

                break;
            }

        case 'e':
            /* @REPLACE_BRACKET: expcnt������0 */
            if (0 != expcnt)
            {
                /* ��ȡ�����������λֵ */
                /* @KEEP_COMMENT: p++������*tΪ*p��t++ */
                p++;
                *t = *p;
                t++;

                /* @REPLACE_BRACKET: <prec>������0 || <doAlt>������0 */
                if ((0 != prec) || (0 != doAlt))
                {
                    /* ���С���� */
                    /* @KEEP_COMMENT: ����*tΪ'.'��t++ */
                    *t = '.';
                    t++;
                }

                /* @KEEP_COMMENT: p++������tmpChrAddΪ<endp> */
                p++;
                tmpChrAdd = endp;       

                /* ��Ӿ��� */
                /* @REPLACE_BRACKET: <prec>������0 && p < tmpChrAdd */
                while ((0 != prec) && (p < tmpChrAdd))
                {
                    /* @KEEP_COMMENT: ����*tΪ*p��t++��p++��<prec>-- */
                    *t= *p;
                    t++;
                    p++;
                    prec--;
                }

                /* @KEEP_COMMENT: p++������tmpChrAddΪ<endp> */
                p++;
                tmpChrAdd = endp;

                /* @REPLACE_BRACKET: <prec>����0 && p < tmpChrAdd */
                if ((0 == prec) && (p < tmpChrAdd))
                {
                    /* @KEEP_COMMENT: ����fractΪ0������tchΪt-1 */ 
                    fract  = 0;
                    tch = t-1;

                    /*
                     * @KEEP_COMMENT: ��0.0���ݸ�����ֵ��Ҫ��������ת����ȡ����
                     * ����<startp>ָ��ת������ַ���
                     */
                    startp = roundCvt(0.0, &expcnt, startp, tch, *p, pDoSign);
                }

                /* @KEEP_COMMENT: --expcnt */
                --expcnt;
            }
            else
            {
                /* ����ָ��ֱ����һ��С������ */
                /* @REPLACE_BRACKET: fract������0 */
                if (0.0 != fract)
                {
                    /* ΪС����ǰ�����ֵ���ָ�� */
                    /* @KEEP_COMMENT: ����expcntΪ-1 */
                    expcnt = -1;

                    /* @REPLACE_BRACKET: 1 */
                    while (1)
                    {
                        /* �ҵ���������һ����Ч���� */
                        /*
                         * @KEEP_COMMENT: ���fract*10Ϊ������С�����֣�tmp�����
                         * �����֣�С�����ֱ�����fract
                         */
                        fract = modf(fract * 10, &tmp);

                        /* @REPLACE_BRACKET: tmp������0.0 */
                        if (0.0 != tmp)
                        {
                            break;
                        }

                        /* @KEEP_COMMENT: expcnt-- */
                        expcnt--;
                    }

                    /* @KEEP_COMMENT: ת��tmpΪASCII�ַ�������*t��t++ */
                    *t = to_char(tmp);
                    t++;

                    /* @REPLACE_BRACKET: <prec>������0 || <doAlt>������0 */
                    if ((0 != prec) || (0 != doAlt))
                    {
                        /* ���С���� */
                        /* @KEEP_COMMENT: ����*tΪ'.'��t++ */
                        *t = '.';
                        t++;
                    }
                }
                else
                {
                    /* ���С�����ֵ���0.0 */
                    /* @KEEP_COMMENT: ����*tΪ'0'��t++ */
                    *t = '0';
                    t++;

                    /* @REPLACE_BRACKET: <prec>������0 || <doAlt>������0 */
                    if ((0 != prec) || (0 != doAlt))
                    {
                        /* ���С���� */
                        /* @KEEP_COMMENT: ����*tΪ'.'��t++ */
                        *t = '.';
                        t++;
                    }
                }
            }

            /* �����Ҫ����ľ���λ��������С��λ*/
            /* @REPLACE_BRACKET: fract������0.0 */
            if (0.0 != fract)
            {
                /* @REPLACE_BRACKET: <prec>������0 */
                if (0 != prec)
                {
                    do 
                    {
                        /*
                         * @KEEP_COMMENT: �Ѹ�����fract*10���Ϊ������С�����֣�
                         * tmp�����������ֵ��fract���С������ֵ
                         */
                        fract = modf(fract * 10, &tmp);

                        /* @KEEP_COMMENT: ת��tmpΪASCII�ַ�������*t */
                        *t = to_char(tmp);

                        /* @KEEP_COMMENT: t++��<prec>-- */
                        t++;
                        prec--;

                        /* @REPLACE_BRACKET: <prec>������0 && fract������0 */
                    } while ((0 != prec) && (0 != fract));
                }

                /* @REPLACE_BRACKET: fract������0.0 */
                if (0.0 != fract)
                {
                    /* @KEEP_COMMENT: ����tchΪt-1 */
                    tch = t-1;

                    /*
                     * @KEEP_COMMENT: ��fract���ݸ�����ֵ��Ҫ��������ת����ȡ��
                     * ������startpָ��ת������ַ���
                     */
                    startp = roundCvt(fract, &expcnt, startp, tch, 0x0, pDoSign);
                }
            }

            /* ��ʱ�������λ�Բ������ں������'0' */
            /* @REPLACE_BRACKET: <prec>������0 */
            while (prec != 0)
            {
                /* @KEEP_COMMENT: <prec>--������*tΪ'0'��t++ */
                prec--;
                *t = '0';
                t++;
            }

            /* ɾ�������'0' */
            /* @REPLACE_BRACKET: gformat������0 && <doAlt>����0 */
            if ((0 != gformat) && (0 == doAlt))
            {
                /* @KEEP_COMMENT: t--������tmpChrAddΪ<startp> */
                t--;
                tmpChrAdd = startp;

                /* @REPLACE_BRACKET: t����tmpChrAdd && *t����'0' */
                while ((t > tmpChrAdd) && ('0' == *t))
                {
                    /* @KEEP_COMMENT: t-- */
                    t--;
                }

                /* @REPLACE_BRACKET: *t����'.' */
                if ('.' == *t)
                {
                    /* @KEEP_COMMENT: --t */
                    --t;
                }
                /* @KEEP_COMMENT: ++t */
                ++t;
            }

            /* ��������ת��Ϊ��ѧ������ */
            /*
             * @KEEP_COMMENT: ������������ֵexpcnt����<fmtch>ָ����ʽת��ΪASCII
             * �ַ����������t
             */
            t = exponentCvt(t, expcnt, fmtch);

            break;

        case 'E':
            /* @REPLACE_BRACKET: expcnt������0 */
            if (0 != expcnt)
            {
                /* ��ȡ�����������λֵ */
                /* @KEEP_COMMENT: p++������*tΪ*p��t++ */
                p++;
                *t = *p;
                t++;

                /* @REPLACE_BRACKET: <prec>������0 || <doAlt>������0 */
                if ((0 != prec) || (0 != doAlt))
                {
                    /* ���С���� */
                    /* @KEEP_COMMENT: ����*tΪ'.'��t++ */
                    *t = '.';
                    t++;
                }

                /* ���Ҫ����ߵľ��� */
                /* @KEEP_COMMENT: p++������tmpChrAddΪ<endp> */
                p++;
                tmpChrAdd = endp;

                /* ��Ӿ��� */
                /* @REPLACE_BRACKET: <prec>������0 && pС��tmpChrAdd */
                while ((0 != prec) && (p < tmpChrAdd))
                {
                    /* @KEEP_COMMENT: ����*tΪ*p��t++��p++��<prec>-- */
                    *t= *p;
                    t++;
                    p++;
                    prec--;
                }

                /* @KEEP_COMMENT: p++������tmpChrAddΪ<endp> */
                p++;
                tmpChrAdd = endp;

                /* @REPLACE_BRACKET: <prec>����0 && pС��tmpChrAdd */
                if ((0 == prec) && (p < tmpChrAdd))
                {
                    /* @KEEP_COMMENT: ����fractΪ0������tchΪt-1 */ 
                    fract  = 0;
                    tch = t-1;

                    /*
                     * @KEEP_COMMENT: ��0.0���ݸ�����ֵ��Ҫ��������ת����ȡ����
                     * ����startpָ��ת������ַ���
                     */
                    startp = roundCvt(0.0, &expcnt, startp, tch, *p, pDoSign);
                }

                /* @KEEP_COMMENT: --expcnt */
                --expcnt;
            }
            else
            {
                /* ����ָ��ֱ����һ��С������ */
                /* @REPLACE_BRACKET: fract������0 */
                if (0.0 != fract)
                {
                    /* ΪС����ǰ�����ֵ���ָ�� */
                    /* @KEEP_COMMENT: ����expcntΪ-1 */
                    expcnt = -1;

                    /* @REPLACE_BRACKET: 1 */
                    while (1)
                    {
                        /* �ҵ���������һ����Ч���� */
                        /*
                         * @KEEP_COMMENT: ���fract*10Ϊ������С�����֣�tmp�����
                         * �����֣�С�����ֱ�����fract
                         */
                        fract = modf(fract * 10, &tmp);

                        /* @REPLACE_BRACKET: tmp������0.0 */
                        if (0.0 != tmp)
                        {
                            break;
                        }

                        /* @KEEP_COMMENT: expcnt-- */
                        expcnt--;
                    }

                    /* @KEEP_COMMENT: ת��tmpΪASCII�ַ�������*t��t++ */
                    *t = to_char(tmp);
                    t++;

                    /* @REPLACE_BRACKET: <prec>������0 || <doAlt>������0 */
                    if ((0 != prec) || (0 != doAlt))
                    {
                        /* ���С���� */
                        /* @KEEP_COMMENT: ����*tΪ'.'��t++ */
                        *t = '.';
                        t++;
                    }
                }
                else
                {
                    /* ���С�����ֵ���0.0 */
                    /* @KEEP_COMMENT: ����*tΪ'0'��t++ */
                    *t = '0';
                    t++;

                    /* @REPLACE_BRACKET: <prec>������0 || <doAlt>������0 */
                    if ((0 != prec) || (0 != doAlt))
                    {
                        /* ���С���� */
                        /* @KEEP_COMMENT: ����*tΪ'.'��t++ */
                        *t = '.';
                        t++;
                    }
                }
            }

            /* �����Ҫ����ľ���λ��������С��λ*/
            /* @REPLACE_BRACKET: fract������0.0 */
            if (0.0 != fract)
            {
                /* @REPLACE_BRACKET: prec������0 */
                if (0 != prec)
                {
                    /* @COMMENT_LINE: ѭ��ת����λ��ΪASCII�ַ�,�����������t�� */
                    do 
                    {
                        /*
                         * @KEEP_COMMENT: ���fract*10Ϊ������С�����֣�tmp�����
                         * �����֣�С�����ֱ�����fract
                         */
                        fract = modf(fract * 10, &tmp);

                        /* @KEEP_COMMENT: ת��tmpΪASCII�ַ�����*t��t++��<prec>-- */
                        *t = to_char(tmp);
                        t++;
                        prec--;

                        /* @REPLACE_BRACKET: <prec>������0 && fract������0 */
                    } while ((0 != prec) && (0 != fract));
                }

                /* @REPLACE_BRACKET: fract������0.0 */
                if (0 != fract)
                {
                    /* @KEEP_COMMENT: tch = t-1 */
                    tch = t-1;

                    /*
                     * @KEEP_COMMENT: ��fract���ݸ�����ֵ��Ҫ��������ת����ȡ��
                     * ������startpָ��ת������ַ���
                     */
                    startp = roundCvt(fract, &expcnt, startp, tch, 0x0, pDoSign);
                }
            }

            /* ��ʱ�������λ�Բ������ں������'0' */
            /* @REPLACE_BRACKET: <prec>������0 */
            while (prec != 0)
            {
                /* @KEEP_COMMENT: <prec>--������*tΪ'0'��t++ */
                prec--;
                *t = '0';
                t++;
            }

            /* ɾ�������'0' */
            /* @REPLACE_BRACKET: gformat������0 && <doAlt>����0 */
            if ((0 != gformat) && (0 == doAlt))
            {
                /* @KEEP_COMMENT: t--������tmpChrAddΪ<startp> */
                t--;
                tmpChrAdd = startp;

                /* @REPLACE_BRACKET: t����tmpChrAdd && *t����'0' */
                while ((t > tmpChrAdd) && ('0' == *t))
                {
                    /* @KEEP_COMMENT: t-- */
                    t--;
                }

                /* @REPLACE_BRACKET: *t����'.' */
                if ('.' == *t)
                {
                    /* @KEEP_COMMENT: --t */
                    --t;
                }

                /* @KEEP_COMMENT: ++t */
                ++t;
            }

            /* ��������ת��Ϊ��ѧ������ */
            /*
             * @KEEP_COMMENT: ����exponentCvt(DR.1.18)������������ֵexpcnt����
             * <fmtch>ָ����ʽת��ΪASCII�ַ����������t
             */
            t = exponentCvt(t, expcnt, fmtch);

            break;

        default :
            break;
    }

    /* @KEEP_COMMENT: ����tmpChrAddΪ<startp> */
    tmpChrAdd = startp;

    /* @REPLACE_BRACKET: t-tmpChrAdd */
    return(t-tmpChrAdd);
}

/*
 * @brief: 
 *    ���ݸ�������ֵ��Ҫ��ĸ�ʽ�Ը���������ת����ȡ��������
 * @param[in]: fract: ������  
 * @param[in]: ch: ������Դ�С�Ļ�׼ֵ
 * @param[in]: end: ������������ַ 
 * @param[out]: e: ��
 * @param[out]: pDoSign: ���ű�־
 * @param[out]: start: ��������ʼ��ַ
 * @return:
 *    ָ��ת���������ַ�����ָ�롣
 * @qualification method: ����/����
 * @derived requirement: ��
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.18
 */
static signed char *roundCvt(double fract,
int *e,
signed char *start,
signed char *end,
signed char ch,
BOOL *pDoSign)
{
    double tmp = 0.0;

    /* @REPLACE_BRACKET: <fract>������0.0 */
    if (0.0 != fract)
    {
        /*
         * @KEEP_COMMENT: ����modf(DR.2.16)�зָ�����<fract>*10������������������
         * ʱ����tmp
         */
        modf(fract*10, &tmp);
    }
    else
    {
        /* @KEEP_COMMENT: ת��<ch>Ϊ���ֱ���������tmp */
        tmp = to_digit(ch);
    }

    /* @REPLACE_BRACKET: tmp����4 */
    if (tmp > 4)
    {
        /* @REPLACE_BRACKET: 1 */
        while (1)
        {
            /* @REPLACE_BRACKET: *end����'.' */
            if (*end == (signed char)'.')
            {
                /* @KEEP_COMMENT: --end */
                --end;
            }

            /* @KEEP_COMMENT: ++*end */
            ++*end;

            /* @REPLACE_BRACKET: *endС�ڵ���'9' */
            if (*end <= (signed char)'9')
            {
                break;
            }

            /* @KEEP_COMMENT: ����*endΪ'0' */
            *end = '0';

            /* @REPLACE_BRACKET: �������׵�ַ���ڻ�������β��ַ */
            if (end == start)
            {
                /* @REPLACE_BRACKET: NULL != e */
                if (NULL != e)
                {
                    /* @KEEP_COMMENT: ����*endΪ'1'��++*e */
                    *end = '1';
                    ++*e;
                }
                else
                {
                    /* @REPLACE_LINE: --end������*endΪ'1'��--start */
                    --end;
                    *end = '1';
                    --start;
                }

                break;
            }

            end--;
        }
    }
    else
    {
        /* @REPLACE_BRACKET: <*pDoSign>������0 */
        if (0 != (*pDoSign))
        {
            /* @REPLACE_BRACKET: 1 */
            while (1)
            {
                /* @REPLACE_BRACKET: <*end>����'.' */
                if (*end == (signed char)'.')
                {
                    /* @KEEP_COMMENT: --end */
                    --end;
                }

                /* @REPLACE_BRACKET: *end������'0' */
                if (*end != (signed char)'0')
                {
                    break;
                }

                /* @REPLACE_BRACKET: �������׵�ַ���ڻ�������β��ַ */
                if (end == start)
                {
                    /* @KEEP_COMMENT: ����<*pDoSign>ΪFALSE */
                    *pDoSign = FALSE;
                }

                /* @KEEP_COMMENT: --end */
                --end;
            }
        }
    }

    /* @REPLACE_BRACKET: start */
    return(start);
}

static signed char expbuf[MAXEXP] = {0x0};
/*
 * @brief: 
 *    ����ֵת��ΪASCII�ַ����������ָ�����ڴ�����
 * @param[in]: e: �ݵ�ֵ
 * @param[in]: fmtch: ��ʽ�ַ�
 * @param[out]: p: ����ڴ�����ʼ��ַ
 * @return:
 *    ָ��ת���������ַ���β��ָ��
 * @qualification method: ����/����
 * @derived requirement: ��
 * @function location: Internal
 * @tracedREQ: RR.1.1
 * @implements: DR.1.19
 */
static signed char *exponentCvt(signed char *p,
int e,
int fmtch)
{
    signed char *t = NULL;
    

    /* @KEEP_COMMENT: ����*pΪ<fmtch>��p++ */
    *p = (signed char)fmtch;
    p++;

    /* @REPLACE_BRACKET: eС��0 */
    if (e < 0)
    {
        /* @KEEP_COMMENT: ����eΪ-e������*pΪ'-'��p++ */
        e = -e;
        *p = '-';
        p++;
    }
    else
    {
        /* @KEEP_COMMENT: ����*pΪ'+'��p++ */
        *p = '+';
        p++;
    }

    /*
     * @KEEP_COMMENT: ����һ������ֵ���ڴ���expbuf��
     * ���СΪMAXEXP(308)���ֽڣ�������ֲ�ָ��tָ��û�������ĩβ
     */
    t = expbuf+MAXEXP;

    /* @REPLACE_BRACKET: e����9 */
    if (e > 9)
    {
        /* @KEEP_COMMENT: ѭ������ */
        do
        {
            /* @KEEP_COMMENT: t-- */
            t--;

            /* @KEEP_COMMENT: ��e����10������ת����ASCII�ַ�������*t */
            *t = to_char(e % 10);

            /* @KEEP_COMMENT: ����eΪe/10 */
            e /= 10;

            /* @REPLACE_BRACKET: e����10����������9 */
        } while (e > 9);

        /* @KEEP_COMMENT: t--����eת��ΪASCII�ַ�������*t */
        t--;
        *t = to_char(e);

        /* @REPLACE_BRACKET: tС��expbuf+308 */
        while (t < (expbuf+MAXEXP))
        {
            /* @KEEP_COMMENT: ����*pΪ*t��p++��t++�� */
			*p = *t;
            p++;
            t++;
        }
    }
    else
    {
        /* @KEEP_COMMENT: ����*pΪ'0'��p++����eת��ΪASCII�ַ�������*p��p++ */
        *p = '0';
        p++;
        *p = to_char(e);
        p++;
    }

    /* @REPLACE_BRACKET: p */
    return(p);
}

