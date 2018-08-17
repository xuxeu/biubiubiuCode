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
 * @file: misc.c
 * @brief:
 *	   <li> ��ѧ�����ڲ�����������ʵ���ļ���</li>
 * @implements: DR.2
 */

/* @<MODULE */ 

/************************ͷ �� ��******************************/

/* @<MOD_HEAD */
#include "string.h"
#include <mathBase.h>
#include <math.h>
/* @MOD_HEAD> */

/************************�� �� ��******************************/

#ifdef __LITTLE_ENDIAN__
#define P_INDEX	3
#else
#define P_INDEX	0
#endif

#ifdef __BIG_ENDIAN__
#define LOW		0
#define HIGH	1
#else
#define LOW		1
#define HIGH	0
#endif

/************************���Ͷ���******************************/

typedef union
{
    double          d;
    unsigned short  s[4];
} tdouble;

/************************�ⲿ����******************************/
/************************ǰ������******************************/
/************************ģ�����******************************/

static double
p1     =  1.3887401997267371720E-2    , /*Hex  2^ -7   *  1.C70FF8B3CC2CF */
p2     =  3.3044019718331897649E-5    , /*Hex  2^-15   *  1.15317DF4526C4 */
q1     =  1.1110813732786649355E-1    , /*Hex  2^ -4   *  1.C719538248597 */
q2     =  9.9176615021572857300E-4    ; /*Hex  2^-10   *  1.03FC4CB8C98E8 */

static double
ln2hi  =  6.9314718036912381649E-1    , /*Hex  2^ -1   *  1.62E42FEE00000 */
ln2lo  =  1.9082149292705877000E-10   , /*Hex  2^-33   *  1.A39EF35793C76 */
lnhuge =  7.1602103751842355450E2     , /*Hex  2^  9   *  1.6602B15B7ECF2 */
invln2 =  1.4426950408889633870E0     ; /*Hex  2^  0   *  1.71547652B82FE */

static double
L1     =  6.6666666666667340202E-1    , /*Hex  2^ -1   *  1.5555555555592 */
L2     =  3.9999999999416702146E-1    , /*Hex  2^ -2   *  1.999999997FF24 */
L3     =  2.8571428742008753154E-1    , /*Hex  2^ -2   *  1.24924941E07B4 */
L4     =  2.2222198607186277597E-1    , /*Hex  2^ -3   *  1.C71C52150BEA6 */
L5     =  1.8183562745289935658E-1    , /*Hex  2^ -3   *  1.74663CC94342F */
L6     =  1.5314087275331442206E-1    , /*Hex  2^ -3   *  1.39A1EC014045B */
L7     =  1.4795612545334174692E-1    ; /*Hex  2^ -3   *  1.2F039F0085122 */

static unsigned short msign=0x7fff, mexp =0x7ff0;
static short prep1=54, gap=4, bias=1023;
static double novf=1.7E308, nunf=3.0E-308,zero=0.0;

static unsigned map[32] =
{
    0xffffffff, 0xfffffffe, 0xfffffffc, 0xfffffff8,
    0xfffffff0, 0xffffffe0, 0xffffffc0, 0xffffff80,
    0xffffff00, 0xfffffe00, 0xfffffc00, 0xfffff800,
    0xfffff000, 0xffffe000, 0xffffc000, 0xffff8000,
    0xffff0000, 0xfffe0000, 0xfffc0000, 0xfff80000,
    0xfff00000, 0xffe00000, 0xffc00000, 0xff800000,
    0xff000000, 0xfe000000, 0xfc000000, 0xf8000000,
    0xf0000000, 0xe0000000, 0xc0000000, 0x80000000,
};

/************************ȫ�ֱ���******************************/
/************************ʵ    ��******************************/

/* @MODULE> */ 


/*
 * @brief: 
 *    �����ƫ�Ƶ�ָ��ֵ��
 * @param[in]: x: ����
 * @param[in]: c: ����
 * @return: 
 *    ���: c << x  ���fl(x+c)=x.
 *    (c��x��������)
 *    exp__E ����
 *                    / exp(x+c) - 1 - x ,  1E-19 < |x| < 0.3465736
 *    exp__E(x,c) =  |
 *                    \ 0                ,  |x| < 1E-19.
 * @notes:
 *    ��Ҫ��ϵͳ֧�ֺ���:	<br>
 *        copysign(x,y)		<br>
 *    �ڲ����㷽��:		<br>
 *    1. ����ƽ�. ��r=x+c.		<br>
 *        �õ�		<br>
 *                           2 * sinh(r/2)	<br>
 *        exp(r) - 1 =   ----------------------   ,	<br>
 *                        cosh(r/2) - sinh(r/2)		<br>
 * <br>
 *        exp__E(r) ������		<br>
 * <br>
 *        x*x             (x/2)*W - (Q - (2*P  + x*P))              <br>
 *        --- + (c + x*[---------------------------------- + c ])   <br>
 *         2                        1 - W                           <br>
 * <br>
 *        ����P : = p1*x^2 + p2*x^4,	<br>
 *            Q : = q1*x^2 + q2*x^4 (Ϊ�˾�ȷ��56λ, �ڼ�q3*x^6)	<br>
 *            W : = x/2-(Q-x*P),	<br>
 *        (����p1,p2,q1,q2,q3��ȡֵ. ����ʽ	<br> 
 *        P ��Q ���ܱ���Ϊsinh ��cosh�Ľ���ֵ : 	<br>
 *        sinh(r/2) =  r/2 + r * P  ,  cosh(r/2) =  1 + Q . )	<br>
 *        ϵ����������Remez�㷨ȷ�ϡ�	<br>
 *    ����ֵ����: 	<br>
 * <br>
 *        |  exp(x) - 1                         |    2**(-57),  (IEEE double)	<br>
 *        | ------------  -  (exp__E(x,0)+x)/x  | <=	                            <br>
 *        |      x                              |    2**(-69),  (VAX D)	        <br>
 * <br>
 *    �ýӿڵ������жϸ��ǲο�cosh(RR.2.7.1)   <br>
 * @tracedREQ: RR.2.7-2
 * @implements: DR.2.23
 */
double exp__E(double x, double c)
{
    static double one = 1.0, half = 1.0/2.0, small = 1.0E-19;
    double z, p, q, xp, xh, w;

    /* @REPLACE_BRACKET: |x| > 1.0E-19 */
    if (copysign(x, one) > small)
    {
        /* @KEEP_COMMENT: ����x*x������z */
        /* @KEEP_COMMENT: ����z*(0.013887401997267371720 + z*3.3044019718331897649E-5)�����p */
        z = x*x;
        p = z*(p1 +z*p2);

        /* @KEEP_COMMENT: ����z*(0.11110813732786649355 + z*9.9176615021572857300E-4)������q */
        q = z*(q1 +z*q2);

        /* @KEEP_COMMENT: ����x*p������xp������x/2������xh */
        xp = x*p;
        xh = x*half;
        /* @KEEP_COMMENT: ����xh-(q-xp)������w������p+p������p */
        w = xh-(q-xp);
        p = p+p;

        /* @KEEP_COMMENT: ����c��ֵΪc+x*((xh*w-(q-(p+xp)))/(1.0-w)+c) */
        c += x*((xh*w-(q-(p+xp)))/(one-w)+c);

        /* @REPLACE_BRACKET: z/2+c */
        return(z*half+c);
    }
    else
    {
        /* @REPLACE_BRACKET: ���x < 0������-0.0�����x >= 0������0.0 */
        return(copysign(zero, x));
    }
}

/*
 * @brief: 
 *    exp(x)-1�Ŀ�ѧ���ʽ��
 * @param[in]: x: ����
 * @return: 
 *    ˫��������
 *    +INF : xΪ+INF
 *    -1 : xΪ-INF
 *    NaN : xΪNaN
 * @notes:
 *    ��Ҫ��ϵͳ֧�ֺ���:	<br>
 *        scaleb(x,n)		<br>
 *        copysign(x,y)		<br>
 *        finite(x)		<br>
 *    �ں˺���:			<br>
 *        exp__E(x,c)		<br>
 *    �ڲ����㷽��:			<br>
 *        1. ��������: ��֪�������x, ��r������k ��ϵΪ:	<br> 
 *        x = k*ln2 + r,	|r| <= 0.5*ln2 .	<br>
 *        Ϊ�˸��õ�׼ȷ�ԣ�r����r := z+c  ��ʽ��ʾ.	<br>
 *        2. ����EXPM1(r) = exp(r)-1  : 	<br>
 *        EXPM1(r=z+c) := z + exp__E(z,c)	<br>
 *        3. EXPM1(x) =	2^k * (EXPM1(r) + 1-2^-k).	<br>
 *    ע��:		<br>
 *        1. ��k=1 ��z < -0.25, ���������и��õĹ�ʽ :	<br> 
 *        EXPM1(x) = 2 * ((z+0.5) + exp__E(z,c))	<br>
 *        2. ��k > 56ʱ, Ϊ�˱���1-2^-k �����������������湫ʽ :	<br> 
 *        EXPM1(x) = 2^k * {[z+(exp__E(z,c)-2^-k)] + 1}	<br>
 * @tracedREQ: RR.2.19-2��RR.2.22-2
 * @implements: DR.2.24
 */
double expm1(double x)
{
    static double one = 1.0, half = 1.0/2.0;
    double z, hi, lo, c;
    int k;

    static int prec = 53;

    /* @REPLACE_BRACKET: <x>ΪNaN */
    if (x != x)
    {
        /* @REPLACE_BRACKET: NaN */
        return(x);
    }

    /* @REPLACE_BRACKET: x <= 7.1602103751842355450E2 */
    if (x <= lnhuge)
    {
        /* @REPLACE_BRACKET: x >= -40.0 */
        if (x >= -40.0)
        {
            /*
             * @KEEP_COMMENT: ���x>=0������x/log(2)+0.5��ȡ��������k��
             * ���x<0������x/log(2)-0.5��ȡ��������k
             */
            k = (int)(invln2*x+copysign(0.5, x));

            /* @KEEP_COMMENT: ����x-k*6.9314718036912381649E-1������hi */
            /* @KEEP_COMMENT: ����k*1.9082149292705877000E-10������lo */
            hi = x-k*ln2hi;
            lo = k*ln2lo;

            /* @KEEP_COMMENT: ����hi-lo������z������(hi-z)-lo������c */
            z = hi-lo;
            c = (hi-z)-lo;

            /* @REPLACE_BRACKET: 0 == k */
            if (0 == k)
            {
                /* @REPLACE_BRACKET: z+exp__E(z,c) */
                return(z+exp__E(z,c));
            }

            /* @REPLACE_BRACKET: 1 == k */
            if (1 == k)
            {
                /* @REPLACE_BRACKET: z < -0.25 */
                if (z < -0.25)
                {
                    /* @KEEP_COMMENT: ����z+1/2������x������x��ֵΪx+exp__E(z,c) */
                    x = z+half;
                    x += exp__E(z,c);

                    /* @REPLACE_BRACKET: x+x */
                    return(x+x);
                }
                else
                {
                    /* @KEEP_COMMENT: ����z��ֵΪz+exp__E(z,c)������1/2+z������x */
                    z += exp__E(z,c);
                    x = half+z;

                    /* @REPLACE_BRACKET: x+x */
                    return(x+x);
                }
            }
            else
            {
                /* @REPLACE_BRACKET: k <= 53 */
                if (k <= prec)
                {
                    /* @KEEP_COMMENT: ����1.0-2^(-k)������x������zֵΪz+exp__E(z,c) */
                    x = one-scaleb(one,-k);
                    z += exp__E(z,c);
                }
                else
                {
                    /* @REPLACE_BRACKET: k < 100 */
                    if (k < 100)
                    {
                        /* @KEEP_COMMENT: ����exp__E(z, c)������x������xֵΪx-2^(-k) */
                        /* @KEEP_COMMENT: ����xֵΪx+z������zֵΪ1.0 */
                        x = exp__E(z, c);
                        x -= scaleb(one, -k);
                        x += z;
                        z = one;
                    }
                    else
                    {
                        /* @KEEP_COMMENT: ����exp__E(z,c)+z������x������zֵΪ1.0 */
                        x = exp__E(z,c)+z;
                        z = one;
                    }
                }
                /* @REPLACE_BRACKET: (x+z)*2^k */
                return(scaleb(x+z,k));
            }
        }
        else
        {
            /* @REPLACE_BRACKET: xΪ������ */
            if (finite(x) != 0)
            {
                /* @REPLACE_BRACKET: -1.0 */
                return(-one);
            }
            else
            {
                /* @REPLACE_BRACKET: -1.0 */
                return(-one);
            }
        }
    }
    else
    {
        /* @REPLACE_BRACKET: xΪ������ */
        if (finite(x) != 0)
        {
            /* @REPLACE_BRACKET: 1.0*2^5000 */
            return(scaleb(one,5000));
        }

        /* @REPLACE_BRACKET: +INF */
        return(x);
    }
}

/*
 * @brief: 
 *    ��Ȼ������̩��չ����
 * @param[in]: z: ����
 * @return: 
 *    ˫�������ݡ�
 * @notes:
 *                    LOG(1+X) - 2S                        X                         <br>
 *    log__L(Z)����  ---------------  ���� Z = S*S, S = ------- , 0 <= Z <= .0294...  <br>
 *                         S                             2 + X                        <br>
 * <br>
 *    �ڲ����㷽��:	<br>
 *    1. ����ʽ����ֵ: ��s = x/(2+x).����	<br>
 *        log(1+x) = log(1+s) - log(1-s) = 2s + 2/3 s**3 + 2/5 s**5 + .....,	<br>		  
 *        (log(1+x) - 2s)/s ������ʽ�ӹ���	<br>
 *        z*(L1 + z*(L2 + z*(... (L7 + z*L8)...)))		<br>
 *        ����z=s*s. (����Lk's ��ȡֵ) 	<br>
 *        ϵ����������Remez�㷨ȷ�ϡ�	<br>
 *    ׼ȷ��: 	<br>
 *    ���û���������, ��IEEE double��ʽ����approximation error (����ֵ)		<br> 
 *    ��2**(-58.49), ����VAX D ��ʽ�����ֵ��2**(-63.63)	<br>
 *    for VAX D format.	<br>
 * @tracedREQ: RR.2.14-2
 * @implements: DR.2.25
 */
double log__L(double z)
{
    /* @REPLACE_BRACKET: z*(L1+z*(L2+z*(L3+z*(L4+z*(L5+z*(L6+z*L7)))))) */
    return(z*(L1+z*(L2+z*(L3+z*(L4+z*(L5+z*(L6+z*L7)))))));
}

/*
 * @brief: 
 *    ����<x>����2^<N>��
 * @param[in]: x: ����
 * @param[in]: N: ��������
 * @return: 
 *    ˫�������ݡ�
 * @notes:
 *    ����x * 2^N, N������ֵ��
 * @tracedREQ: RR.2.4-7��RR.2.14-2��
 * @implements: DR.2.26
 */
double scaleb(double x, int N)
{
    int k = 0;
    unsigned short gapt = (unsigned short)gap;
    unsigned int t = 0;
    tdouble y = {0.0};

    /* @KEEP_COMMENT: ����y.d��ֵΪx */
    y.d = x;

    /* @REPLACE_BRACKET: 0.0 == y.d */
    if (zero == y.d)
    {
        /* @REPLACE_BRACKET: 0.0 */
        return(y.d);
    }

    /* @KEEP_COMMENT: ����kΪy.s[P_INDEX]&mexp */
    k = y.s[P_INDEX]&mexp;

    /* @REPLACE_BRACKET: k != 0x7ff0 */
    if (k != mexp)
    {
        /* @REPLACE_BRACKET: N < -2100 */
        if (N < -2100)
        {
            /* @REPLACE_BRACKET: 0.0 */
            return(nunf*nunf);
        }
        else
        {
            /* @REPLACE_BRACKET: N > 2100 */
            if (N > 2100)
            {
                /* @REPLACE_BRACKET: +INF */
                return(novf+novf);
            }
        }

        /* y.sΪ�ǹ�񻯼�С�� ������Ҫ���*/
        /* @REPLACE_BRACKET: 0 == k */
        if (0 == k)
        {
            /* @KEEP_COMMENT: ����y.d��ֵΪy.d * 2^54������N��ֵΪN-54 */
            y.d *= scaleb(1.0,(int)prep1);
            N -= prep1;

            /* @REPLACE_BRACKET: y.d * 2^N */
            return(scaleb(y.d,N));
        }

        /* @KEEP_COMMENT: ����t��ֵΪ(unsigned int)k������t>>4������t */
        t = (unsigned int)k;
        t = (t>>gapt);
        /* @KEEP_COMMENT: ����k��ֵΪ(int)t������k+N������k */
        k = (int)t;
        k = k+N;

        /* @REPLACE_BRACKET: k > 0 */
        if (k > 0)
        {
            /* @KEEP_COMMENT: ����0x7ff0>>4�������޷�������t */
            t = (unsigned int)(mexp>>gapt);

            /* @REPLACE_BRACKET: �޷�������k < t */
            if ((unsigned int)k < t)
            {
                /* @KEEP_COMMENT: ����t��ֵΪ�޷�������k */
                /* @KEEP_COMMENT: ����(y.s[P_INDEX]&~0x7ff0)|(t<<4)������y.s[P_INDEX] */
                t = (unsigned int)k;
                y.s[P_INDEX] = (y.s[P_INDEX]&~mexp)|(t<<gapt);
            }
            else
            {
                /* @KEEP_COMMENT: ����y.dֵΪ+INF */
                y.d = novf+novf; 
            }
        }
        else
        {
            /* @REPLACE_BRACKET: k > -54 */
            if (k > -prep1)
            {
                /* @KEEP_COMMENT: ����(y.s[P_INDEX]&~0x7ff0)|(1<<4)������y.s[P_INDEX] */
                /* @KEEP_COMMENT: ����y.dֵΪy.d*2^(k-1) */
                y.s[P_INDEX] = (y.s[P_INDEX]&~mexp)|(1<<gapt);
                y.d *= scaleb(1.0,k-1);
            }
            else
            {
                /* @REPLACE_BRACKET: 0 */
                return(nunf*nunf);
            }
        }
    }

    /* @REPLACE_BRACKET: y.d */
    return(y.d);
}

/*
 * @brief: 
 *    ����<y>�ķ��ŵ�<x>��
 * @param[in]: x: ����
 * @param[in]: y: ����
 * @return: 
 *    ����ȡy���ŵ�x��ֵ
 * @notes: 
 *    returns x with the sign of y.	
 * @tracedREQ: RR.2.2-1
 * @implements: DR.2.27
 */
double copysign(double x, double y)
{
    tdouble a = {0.0}, b = {0.0};

    /* @KEEP_COMMENT: ����a.dֵΪx������b.dֵΪy */
    a.d = x;
    b.d = y;

    /* @KEEP_COMMENT: ��a.d�ķ���λ�ı�Ϊb.d�ķ���λ��������a.d */
    a.s[P_INDEX] = (a.s[P_INDEX]&msign)|(b.s[P_INDEX]&~msign);

    /* @REPLACE_BRACKET: a.d */
    return(a.d);
}

/*
 * @brief: 
 *    ����x�Ĳ�ƫָ��(x��[2^n,2^(n+1))ʱ������n)��
 * @param[in]: x: ����
 * @return: 
 *    ˫�������ݡ�
 *    -INF : xΪ0��
 *    INF : xΪINF��
 *    NaN : xΪNaN.
 * @notes: 
 *    ����x�Ĳ�ƫ(unbias)ָ��, һ�������ŵ�˫��������.	<br>
 *    ��x�ƽ���2^n������<n>(nΪ����)��<br>
 * @tracedREQ: RR.2.20-2
 * @implements: DR.2.28
 */
double logb(double x)
{
    tdouble a = {0.0};
    unsigned short t = 0;
    unsigned short gapt = (unsigned short)gap;
    short k = 0;
    double f = 0.0;

    /* @REPLACE_LINE: ����a.d��ֵΪx������a.d�Ľ�����t */
    a.d = x;
    t = (a.s[P_INDEX]&mexp);

    /* @REPLACE_BRACKET: t != 0 */
    if (t != 0)
    {
        /* @REPLACE_LINE: ����t>>4������t������kֵΪt������k-1023������f */
        t = (t>>gapt);
        k = (short)t;
        f = (double)(k-bias);

        /* @REPLACE_BRACKET: f */
        return(f);
    }
    else
    {
        /* @REPLACE_BRACKET: -1022.0 */
        return(-1022.0);
    }
}

/*
 * @brief: 
 *    �жϲ����Ƿ�Ϊ��������
 * @param[in]: x: ����
 * @return: 
 *    0:xΪINF��NAN��
 *    1:xΪ��������
 * @notes: 
 *    ��<x>Ϊ����������TRUE(1)�����߷���FALSE(0) ��
 * @tracedREQ: RR.2.4-2��RR.2.6-1
 * @implements: DR.2.29
 */
int finite(double x)
{
    tdouble a = {0.0};

    /* @REPLACE_LINE: ����a.d��ֵΪx */
    a.d = x;

    /* @REPLACE_BRACKET: a.d����ȫΪ1������0�����򷵻�1 */
    return((a.s[P_INDEX]&mexp) != mexp);
}

/*
 * @brief: 
 *    ����x REM y�Ĺ���������
 * @param[in]: x: ����
 * @param[in]: p: ����
 * @return:
 *    ����x REM y  =  x - [x/y]*y ,[x/y]�����x/y�����������[x/y]����������
 *    ��ֵ��ȣ���ȡż������ֵ��Ϊ[-y/2,y/2]��
 * @notes:
 *    drem(x, y)���������ں����ڲ���
 * @tracedREQ: RR.2.6-2��RR.2.18-2
 * @implements: DR.2.30
 */
double drem(double x, double p)
{
    short sign = 0;
    double hp = 0.0;
    tdouble tmp = {0.0}, xd = {0.0};
    tdouble pd = {0.0}, dp = {0.0};
    unsigned short k = 0;
    unsigned short t = 0;

    /* @KEEP_COMMENT: ����xd.dֵΪx������pd.dֵΪp */
    xd.d = x;
    pd.d = p;

    /* @KEEP_COMMENT: ��pd.dת��������������pd.d+pd.d������dp.d������pd.d/2������hp */
    pd.s[P_INDEX] &= msign;
    dp.d = pd.d+pd.d;
    hp = pd.d/2;

    /* @KEEP_COMMENT: ��ȡxd.d�ķ��ű�����sign�У���xd.dת��Ϊ���� */
    t = (xd.s[P_INDEX]&~msign);
    sign = (short)t;
    xd.s[P_INDEX] &= msign;

    /* @REPLACE_BRACKET: xd.d > dp.d */
    while (xd.d > dp.d)
    {
        /* @KEEP_COMMENT: ����xd.s��dp.s�Ľ����ֵ������k������tmp.d��ֵΪdp.d */
        k = (xd.s[P_INDEX]&mexp)-(dp.s[P_INDEX]&mexp);
        tmp.d = dp.d ;

        /* @KEEP_COMMENT: ����tmp.s[P_INDEX]��ֵΪtmp.s[P_INDEX]+k */
        tmp.s[P_INDEX] += k ;

        /* @REPLACE_BRACKET: xd.d < tmp.d */
        if (xd.d < tmp.d)
        {
            /* @KEEP_COMMENT: ����tmp.s[P_INDEX]��ֵΪtmp.s[P_INDEX]-16 */
            tmp.s[P_INDEX] -= 16;
        }

        /* @KEEP_COMMENT: ����xd.d��ֵΪxd.d-tmp.d */
        xd.d -= tmp.d;
    }

    /* @REPLACE_BRACKET: xd.d > hp */
    if (xd.d > hp)
    {
        /* @KEEP_COMMENT: ����xd.d��ֵΪxd.d-pd.d */
        xd.d -= pd.d;

        /* @REPLACE_BRACKET: xd.d >= hp */
        if (xd.d >= hp)
        {
            /* @KEEP_COMMENT: ����xd.d��ֵΪxd.d-pd.d */
            xd.d -= pd.d;
        }
    }

    /* @KEEP_COMMENT: ����xd.s[P_INDEX]��ֵΪxd.s[P_INDEX]^sign */
    xd.s[P_INDEX] ^= (unsigned short)sign;

    /* @REPLACE_BRACKET: xd.d */
    return(xd.d);
}

/*
 * @brief: 
 *    ��ȡ������<v>���������͡�
 * @param[in]: x: ����
 * @param[out]: r: ָ����������ڱ���<v>����������
 * @return: 
 *    0.0:<v>Ϊ0��
 *    INF:<v>ΪINF��
 *    NAN:<v>ΪNAN��
 *    REAL:<v>Ϊ��������
 *    INTEGER:<v>Ϊ������
 * @notes: 
 *    <r>��ȡ<x>���������֡�
 * @tracedREQ: RR.2.5-1��RR.2.9-1
 * @implements: DR.2.31
 */
int fpTypeGet(double v, double *r)
{
    __cv_type cv = {0.0};
    int exp_f = 0;
    BOOL any = FALSE;

    /* @KEEP_COMMENT: ����cv.p_double��ֵΪv */
    cv.p_double = v;

    /* @REPLACE_BRACKET: v�Ľ���Ϊ0����v��β��Ϊ0 */
    if ((0 == (cv.p_mant[LOW] & 0x7fffffff)) && (0 == cv.p_mant[HIGH]))
    {
        /* @REPLACE_BRACKET: r != NULL */
        if (r != NULL)
        {
            /* @KEEP_COMMENT: ����*rֵΪv */
            *r = v;
        }

        /* @REPLACE_BRACKET: 0.0 */
        return(ZERO);
    }

    /* ��ȡ������v�������ͽ���λ��12λ����������ͱ���exp_f�� */
    /* @KEEP_COMMENT: ��ȡ������v�������ͽ���λ��12λ��������exp_f */
    exp_f = ((cv.p_mant[LOW]&0x7ff00000)>>20);

    /* @REPLACE_BRACKET: exp_f == 2047 */
    if (exp_f == 2047)
    {
        /* @REPLACE_BRACKET: r != NULL */
        if (r != NULL)
        {
            /* @KEEP_COMMENT: ����*rֵΪv */
            *r = v;
        }

        /*
         * @REPLACE_BRACKET: (cv.p_mant��32λ����0 && cv.p_mant��32λ����0������INF��
         * ���򷵻�NAN
         */
        return((((cv.p_mant[LOW] & 0x000fffff) == 0) && (cv.p_mant[HIGH] == 0))?INF:NAN);
    }

    /* �����ʶ����Ϊ�����ʾ�������൱����ֵ����2^m-1��mΪλ��������double���ͣ�mΪ10������Ҫ��ȡ��ֵ����Ҫ��ȥ2^m-1 */
    /* @KEEP_COMMENT: ����exp_fֵΪexp_f-1023������anyΪFALSE */
    exp_f -= 1023;
    any = (BOOL)FALSE;

    /* ����С��0����һ���Ǹ���ʵ�����������ֵ���0 */
    /* @REPLACE_BRACKET: exp_f < 0 */
    if (exp_f < 0)
    {
        /* @KEEP_COMMENT: ����anyΪTRUE */
        /* @KEEP_COMMENT: ����cv.p_mant[LOW]Ϊcv.p_mant[LOW]&0x80000000 */
        /* @KEEP_COMMENT: ����cv.p_mant[HIGH]ֵΪ0 */
        any = (BOOL)TRUE;
        cv.p_mant[LOW] &= 0x80000000;
        cv.p_mant[HIGH] = 0;
    }
    else
    {
        /* @REPLACE_BRACKET: exp_f < 20 */
        if (exp_f < 20)
        {
            /*
             * @REPLACE_BRACKET: cv.p_mant��32λ(��С������)������0 || cv.p_mant��
             * 32λβ����С�����ֲ�����0
             */
            if ((cv.p_mant[HIGH] != 0) || ((cv.p_mant[LOW] & ~(map[20-exp_f])) != 0))
            {
                /* @KEEP_COMMENT: ����anyΪTRUE������cv.p_mant[HIGH]��ֵΪ0 */
                /* @KEEP_COMMENT: ����cv.p_mant[LOW]��ֵΪcv.p_mant[LOW]&map[20-exp_f] */
                any = TRUE;
                cv.p_mant[HIGH] = 0;
                cv.p_mant[LOW] &= map[20-exp_f];
            }
        }
        else
        {
            /* @REPLACE_BRACKET: 20 == exp_f */
            if (20 == exp_f)
            {
                /* @REPLACE_BRACKET: cv.p_mantС�����ֲ�����0 */
                if ((cv.p_mant[HIGH] & ~(0x00000000)) != 0)
                {
                    /* @KEEP_COMMENT: ����anyΪTRUE */
                    /* @KEEP_COMMENT: ����cv.p_mant[HIGH]ֵΪcv.p_mant[HIGH]&0x00000000 */
                    any = TRUE;
                    cv.p_mant[HIGH] &= 0x00000000;
                }
            }
            else
            {
                /* @REPLACE_BRACKET: exp_f < 52 */
                if (exp_f < 52)
                {
                    /* @REPLACE_BRACKET: cv.p_mant������С�����ֲ�����0 */
                    if ((cv.p_mant[HIGH] & ~(map [52-exp_f])) != 0)
                    {
                        /*
                         * @KEEP_COMMENT: ����anyΪTRUE������cv.p_mant[HIGH]��ֵ
                         * Ϊcv.p_mant[HIGH]&map[52-exp_f]
                         */
                        any = TRUE;
                        cv.p_mant[HIGH] &= map[52-exp_f];
                    }
                }
            }
        }
    }

    /* �����������ֵ�ֵ */
    /* @REPLACE_BRACKET: r != NULL */
    if (r != NULL)
    {
        /* @KEEP_COMMENT: ����*r��ֵΪcv.p_double */
        *r = cv.p_double;
    }

    /* @REPLACE_BRACKET: any����TRUE������REAL��any����FALSE������INTEGER */
    return((any == TRUE)?REAL:INTEGER);
}

