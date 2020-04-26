/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史:
 * 2011-02-23  张勇，北京科银京成技术有限公司
 *                   创建该文件。
 * 2011-04-15  张勇，北京科银京成技术有限公司
 * 				     增加PDL描述、优化代码、修改GBJ5369违反项。
 */

/*
 * @file: misc.c
 * @brief:
 *	   <li> 数学函数内部各辅助函数实现文件。</li>
 * @implements: DR.2
 */

/* @<MODULE */ 

/************************头 文 件******************************/

/* @<MOD_HEAD */
#include "string.h"
#include <mathBase.h>
#include <math.h>
/* @MOD_HEAD> */

/************************宏 定 义******************************/

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

/************************类型定义******************************/

typedef union
{
    double          d;
    unsigned short  s[4];
} tdouble;

/************************外部声明******************************/
/************************前向声明******************************/
/************************模块变量******************************/

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

/************************全局变量******************************/
/************************实    现******************************/

/* @MODULE> */ 


/*
 * @brief: 
 *    计算带偏移的指数值。
 * @param[in]: x: 参数
 * @param[in]: c: 参数
 * @return: 
 *    如果: c << x  因此fl(x+c)=x.
 *    (c是x的修正项)
 *    exp__E 返回
 *                    / exp(x+c) - 1 - x ,  1E-19 < |x| < 0.3465736
 *    exp__E(x,c) =  |
 *                    \ 0                ,  |x| < 1E-19.
 * @notes:
 *    需要的系统支持函数:	<br>
 *        copysign(x,y)		<br>
 *    内部计算方法:		<br>
 *    1. 有理逼近. 让r=x+c.		<br>
 *        得到		<br>
 *                           2 * sinh(r/2)	<br>
 *        exp(r) - 1 =   ----------------------   ,	<br>
 *                        cosh(r/2) - sinh(r/2)		<br>
 * <br>
 *        exp__E(r) 被估算		<br>
 * <br>
 *        x*x             (x/2)*W - (Q - (2*P  + x*P))              <br>
 *        --- + (c + x*[---------------------------------- + c ])   <br>
 *         2                        1 - W                           <br>
 * <br>
 *        这里P : = p1*x^2 + p2*x^4,	<br>
 *            Q : = q1*x^2 + q2*x^4 (为了精确到56位, 在加q3*x^6)	<br>
 *            W : = x/2-(Q-x*P),	<br>
 *        (对于p1,p2,q1,q2,q3的取值. 多项式	<br> 
 *        P 和Q 可能被作为sinh 和cosh的近似值 : 	<br>
 *        sinh(r/2) =  r/2 + r * P  ,  cosh(r/2) =  1 + Q . )	<br>
 *        系数会由特殊Remez算法确认。	<br>
 *    近似值错误: 	<br>
 * <br>
 *        |  exp(x) - 1                         |    2**(-57),  (IEEE double)	<br>
 *        | ------------  -  (exp__E(x,0)+x)/x  | <=	                            <br>
 *        |      x                              |    2**(-69),  (VAX D)	        <br>
 * <br>
 *    该接口的条件判断覆盖参看cosh(RR.2.7.1)   <br>
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
        /* @KEEP_COMMENT: 计算x*x保存至z */
        /* @KEEP_COMMENT: 计算z*(0.013887401997267371720 + z*3.3044019718331897649E-5)保存成p */
        z = x*x;
        p = z*(p1 +z*p2);

        /* @KEEP_COMMENT: 计算z*(0.11110813732786649355 + z*9.9176615021572857300E-4)保存至q */
        q = z*(q1 +z*q2);

        /* @KEEP_COMMENT: 计算x*p保存至xp，计算x/2保存至xh */
        xp = x*p;
        xh = x*half;
        /* @KEEP_COMMENT: 计算xh-(q-xp)保存至w，计算p+p保存至p */
        w = xh-(q-xp);
        p = p+p;

        /* @KEEP_COMMENT: 设置c的值为c+x*((xh*w-(q-(p+xp)))/(1.0-w)+c) */
        c += x*((xh*w-(q-(p+xp)))/(one-w)+c);

        /* @REPLACE_BRACKET: z/2+c */
        return(z*half+c);
    }
    else
    {
        /* @REPLACE_BRACKET: 如果x < 0，返回-0.0；如果x >= 0，返回0.0 */
        return(copysign(zero, x));
    }
}

/*
 * @brief: 
 *    exp(x)-1的科学表达式。
 * @param[in]: x: 参数
 * @return: 
 *    双精度数据
 *    +INF : x为+INF
 *    -1 : x为-INF
 *    NaN : x为NaN
 * @notes:
 *    需要的系统支持函数:	<br>
 *        scaleb(x,n)		<br>
 *        copysign(x,y)		<br>
 *        finite(x)		<br>
 *    内核函数:			<br>
 *        exp__E(x,c)		<br>
 *    内部计算方法:			<br>
 *        1. 变量减少: 已知输入参数x, 与r和整数k 关系为:	<br> 
 *        x = k*ln2 + r,	|r| <= 0.5*ln2 .	<br>
 *        为了更好的准确性，r会以r := z+c  方式表示.	<br>
 *        2. 计算EXPM1(r) = exp(r)-1  : 	<br>
 *        EXPM1(r=z+c) := z + exp__E(z,c)	<br>
 *        3. EXPM1(x) =	2^k * (EXPM1(r) + 1-2^-k).	<br>
 *    注意:		<br>
 *        1. 当k=1 和z < -0.25, 我们用下列更好的公式 :	<br> 
 *        EXPM1(x) = 2 * ((z+0.5) + exp__E(z,c))	<br>
 *        2. 当k > 56时, 为了避免1-2^-k 的舍入误差，我们用下面公式 :	<br> 
 *        EXPM1(x) = 2^k * {[z+(exp__E(z,c)-2^-k)] + 1}	<br>
 * @tracedREQ: RR.2.19-2，RR.2.22-2
 * @implements: DR.2.24
 */
double expm1(double x)
{
    static double one = 1.0, half = 1.0/2.0;
    double z, hi, lo, c;
    int k;

    static int prec = 53;

    /* @REPLACE_BRACKET: <x>为NaN */
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
             * @KEEP_COMMENT: 如果x>=0，计算x/log(2)+0.5并取整保存至k；
             * 如果x<0，计算x/log(2)-0.5并取整保存至k
             */
            k = (int)(invln2*x+copysign(0.5, x));

            /* @KEEP_COMMENT: 计算x-k*6.9314718036912381649E-1保存至hi */
            /* @KEEP_COMMENT: 计算k*1.9082149292705877000E-10保存至lo */
            hi = x-k*ln2hi;
            lo = k*ln2lo;

            /* @KEEP_COMMENT: 计算hi-lo保存至z，计算(hi-z)-lo保存至c */
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
                    /* @KEEP_COMMENT: 计算z+1/2保存至x，设置x的值为x+exp__E(z,c) */
                    x = z+half;
                    x += exp__E(z,c);

                    /* @REPLACE_BRACKET: x+x */
                    return(x+x);
                }
                else
                {
                    /* @KEEP_COMMENT: 设置z的值为z+exp__E(z,c)，计算1/2+z保存至x */
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
                    /* @KEEP_COMMENT: 计算1.0-2^(-k)保存至x，设置z值为z+exp__E(z,c) */
                    x = one-scaleb(one,-k);
                    z += exp__E(z,c);
                }
                else
                {
                    /* @REPLACE_BRACKET: k < 100 */
                    if (k < 100)
                    {
                        /* @KEEP_COMMENT: 计算exp__E(z, c)保存至x，设置x值为x-2^(-k) */
                        /* @KEEP_COMMENT: 设置x值为x+z，设置z值为1.0 */
                        x = exp__E(z, c);
                        x -= scaleb(one, -k);
                        x += z;
                        z = one;
                    }
                    else
                    {
                        /* @KEEP_COMMENT: 计算exp__E(z,c)+z保存至x，设置z值为1.0 */
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
            /* @REPLACE_BRACKET: x为有限数 */
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
        /* @REPLACE_BRACKET: x为有限数 */
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
 *    自然对数的泰勒展开。
 * @param[in]: z: 参数
 * @return: 
 *    双精度数据。
 * @notes:
 *                    LOG(1+X) - 2S                        X                         <br>
 *    log__L(Z)返回  ---------------  这里 Z = S*S, S = ------- , 0 <= Z <= .0294...  <br>
 *                         S                             2 + X                        <br>
 * <br>
 *    内部计算方法:	<br>
 *    1. 多项式近似值: 让s = x/(2+x).于是	<br>
 *        log(1+x) = log(1+s) - log(1-s) = 2s + 2/3 s**3 + 2/5 s**5 + .....,	<br>		  
 *        (log(1+x) - 2s)/s 由下面式子估算	<br>
 *        z*(L1 + z*(L2 + z*(... (L7 + z*L8)...)))		<br>
 *        这里z=s*s. (对于Lk's 的取值) 	<br>
 *        系数会由特殊Remez算法确认。	<br>
 *    准确性: 	<br>
 *    如果没有舍入误差, 在IEEE double格式最大的approximation error (绝对值)		<br> 
 *    是2**(-58.49), 而在VAX D 格式的最大值是2**(-63.63)	<br>
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
 *    计算<x>乘以2^<N>。
 * @param[in]: x: 参数
 * @param[in]: N: 整数参数
 * @return: 
 *    双精度数据。
 * @notes:
 *    返回x * 2^N, N是整数值。
 * @tracedREQ: RR.2.4-7，RR.2.14-2。
 * @implements: DR.2.26
 */
double scaleb(double x, int N)
{
    int k = 0;
    unsigned short gapt = (unsigned short)gap;
    unsigned int t = 0;
    tdouble y = {0.0};

    /* @KEEP_COMMENT: 设置y.d的值为x */
    y.d = x;

    /* @REPLACE_BRACKET: 0.0 == y.d */
    if (zero == y.d)
    {
        /* @REPLACE_BRACKET: 0.0 */
        return(y.d);
    }

    /* @KEEP_COMMENT: 设置k为y.s[P_INDEX]&mexp */
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

        /* y.s为非规格化极小数 ，则需要规格化*/
        /* @REPLACE_BRACKET: 0 == k */
        if (0 == k)
        {
            /* @KEEP_COMMENT: 设置y.d的值为y.d * 2^54，设置N的值为N-54 */
            y.d *= scaleb(1.0,(int)prep1);
            N -= prep1;

            /* @REPLACE_BRACKET: y.d * 2^N */
            return(scaleb(y.d,N));
        }

        /* @KEEP_COMMENT: 设置t的值为(unsigned int)k，计算t>>4保存至t */
        t = (unsigned int)k;
        t = (t>>gapt);
        /* @KEEP_COMMENT: 设置k的值为(int)t，计算k+N保存至k */
        k = (int)t;
        k = k+N;

        /* @REPLACE_BRACKET: k > 0 */
        if (k > 0)
        {
            /* @KEEP_COMMENT: 计算0x7ff0>>4保存至无符号整型t */
            t = (unsigned int)(mexp>>gapt);

            /* @REPLACE_BRACKET: 无符号整型k < t */
            if ((unsigned int)k < t)
            {
                /* @KEEP_COMMENT: 设置t的值为无符号整型k */
                /* @KEEP_COMMENT: 计算(y.s[P_INDEX]&~0x7ff0)|(t<<4)保存至y.s[P_INDEX] */
                t = (unsigned int)k;
                y.s[P_INDEX] = (y.s[P_INDEX]&~mexp)|(t<<gapt);
            }
            else
            {
                /* @KEEP_COMMENT: 设置y.d值为+INF */
                y.d = novf+novf; 
            }
        }
        else
        {
            /* @REPLACE_BRACKET: k > -54 */
            if (k > -prep1)
            {
                /* @KEEP_COMMENT: 计算(y.s[P_INDEX]&~0x7ff0)|(1<<4)保存至y.s[P_INDEX] */
                /* @KEEP_COMMENT: 设置y.d值为y.d*2^(k-1) */
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
 *    拷贝<y>的符号到<x>。
 * @param[in]: x: 参数
 * @param[in]: y: 参数
 * @return: 
 *    返回取y符号的x的值
 * @notes: 
 *    returns x with the sign of y.	
 * @tracedREQ: RR.2.2-1
 * @implements: DR.2.27
 */
double copysign(double x, double y)
{
    tdouble a = {0.0}, b = {0.0};

    /* @KEEP_COMMENT: 设置a.d值为x，设置b.d值为y */
    a.d = x;
    b.d = y;

    /* @KEEP_COMMENT: 将a.d的符号位改变为b.d的符号位并保存至a.d */
    a.s[P_INDEX] = (a.s[P_INDEX]&msign)|(b.s[P_INDEX]&~msign);

    /* @REPLACE_BRACKET: a.d */
    return(a.d);
}

/*
 * @brief: 
 *    计算x的不偏指数(x在[2^n,2^(n+1))时，返回n)。
 * @param[in]: x: 参数
 * @return: 
 *    双精度数据。
 *    -INF : x为0；
 *    INF : x为INF；
 *    NaN : x为NaN.
 * @notes: 
 *    返回x的不偏(unbias)指数, 一个带符号的双精度整数.	<br>
 *    当x逼近于2^n，返回<n>(n为整数)。<br>
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

    /* @REPLACE_LINE: 设置a.d的值为x，保存a.d的阶码至t */
    a.d = x;
    t = (a.s[P_INDEX]&mexp);

    /* @REPLACE_BRACKET: t != 0 */
    if (t != 0)
    {
        /* @REPLACE_LINE: 计算t>>4保存至t，设置k值为t，计算k-1023保存至f */
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
 *    判断参数是否为有限数。
 * @param[in]: x: 参数
 * @return: 
 *    0:x为INF或NAN；
 *    1:x为有限数。
 * @notes: 
 *    当<x>为有限数返回TRUE(1)，否者返回FALSE(0) 。
 * @tracedREQ: RR.2.4-2，RR.2.6-1
 * @implements: DR.2.29
 */
int finite(double x)
{
    tdouble a = {0.0};

    /* @REPLACE_LINE: 设置a.d的值为x */
    a.d = x;

    /* @REPLACE_BRACKET: a.d阶码全为1，返回0；否则返回1 */
    return((a.s[P_INDEX]&mexp) != mexp);
}

/*
 * @brief: 
 *    计算x REM y的规整余数。
 * @param[in]: x: 参数
 * @param[in]: p: 参数
 * @return:
 *    返回x REM y  =  x - [x/y]*y ,[x/y]是最靠近x/y的整数，如果[x/y]与两整数的
 *    差值相等，则取偶数。其值域为[-y/2,y/2]。
 * @notes:
 *    drem(x, y)适用于周期函数内部。
 * @tracedREQ: RR.2.6-2，RR.2.18-2
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

    /* @KEEP_COMMENT: 设置xd.d值为x，设置pd.d值为p */
    xd.d = x;
    pd.d = p;

    /* @KEEP_COMMENT: 将pd.d转换成正数，计算pd.d+pd.d保存至dp.d，计算pd.d/2保存至hp */
    pd.s[P_INDEX] &= msign;
    dp.d = pd.d+pd.d;
    hp = pd.d/2;

    /* @KEEP_COMMENT: 获取xd.d的符号保存至sign中，将xd.d转换为正数 */
    t = (xd.s[P_INDEX]&~msign);
    sign = (short)t;
    xd.s[P_INDEX] &= msign;

    /* @REPLACE_BRACKET: xd.d > dp.d */
    while (xd.d > dp.d)
    {
        /* @KEEP_COMMENT: 计算xd.s与dp.s的阶码差值保存至k，设置tmp.d的值为dp.d */
        k = (xd.s[P_INDEX]&mexp)-(dp.s[P_INDEX]&mexp);
        tmp.d = dp.d ;

        /* @KEEP_COMMENT: 设置tmp.s[P_INDEX]的值为tmp.s[P_INDEX]+k */
        tmp.s[P_INDEX] += k ;

        /* @REPLACE_BRACKET: xd.d < tmp.d */
        if (xd.d < tmp.d)
        {
            /* @KEEP_COMMENT: 设置tmp.s[P_INDEX]的值为tmp.s[P_INDEX]-16 */
            tmp.s[P_INDEX] -= 16;
        }

        /* @KEEP_COMMENT: 设置xd.d的值为xd.d-tmp.d */
        xd.d -= tmp.d;
    }

    /* @REPLACE_BRACKET: xd.d > hp */
    if (xd.d > hp)
    {
        /* @KEEP_COMMENT: 设置xd.d的值为xd.d-pd.d */
        xd.d -= pd.d;

        /* @REPLACE_BRACKET: xd.d >= hp */
        if (xd.d >= hp)
        {
            /* @KEEP_COMMENT: 设置xd.d的值为xd.d-pd.d */
            xd.d -= pd.d;
        }
    }

    /* @KEEP_COMMENT: 设置xd.s[P_INDEX]的值为xd.s[P_INDEX]^sign */
    xd.s[P_INDEX] ^= (unsigned short)sign;

    /* @REPLACE_BRACKET: xd.d */
    return(xd.d);
}

/*
 * @brief: 
 *    获取浮点数<v>的数据类型。
 * @param[in]: x: 参数
 * @param[out]: r: 指针参数，用于保存<v>的整数部分
 * @return: 
 *    0.0:<v>为0；
 *    INF:<v>为INF；
 *    NAN:<v>为NAN；
 *    REAL:<v>为浮点数；
 *    INTEGER:<v>为整数。
 * @notes: 
 *    <r>截取<x>的整数部分。
 * @tracedREQ: RR.2.5-1，RR.2.9-1
 * @implements: DR.2.31
 */
int fpTypeGet(double v, double *r)
{
    __cv_type cv = {0.0};
    int exp_f = 0;
    BOOL any = FALSE;

    /* @KEEP_COMMENT: 设置cv.p_double的值为v */
    cv.p_double = v;

    /* @REPLACE_BRACKET: v的阶码为0并且v的尾数为0 */
    if ((0 == (cv.p_mant[LOW] & 0x7fffffff)) && (0 == cv.p_mant[HIGH]))
    {
        /* @REPLACE_BRACKET: r != NULL */
        if (r != NULL)
        {
            /* @KEEP_COMMENT: 设置*r值为v */
            *r = v;
        }

        /* @REPLACE_BRACKET: 0.0 */
        return(ZERO);
    }

    /* 获取浮点数v的数符和阶码位共12位，存放至整型变量exp_f中 */
    /* @KEEP_COMMENT: 获取浮点数v的数符和阶码位共12位，保存至exp_f */
    exp_f = ((cv.p_mant[LOW]&0x7ff00000)>>20);

    /* @REPLACE_BRACKET: exp_f == 2047 */
    if (exp_f == 2047)
    {
        /* @REPLACE_BRACKET: r != NULL */
        if (r != NULL)
        {
            /* @KEEP_COMMENT: 设置*r值为v */
            *r = v;
        }

        /*
         * @REPLACE_BRACKET: (cv.p_mant低32位等于0 && cv.p_mant高32位等于0，返回INF；
         * 否则返回NAN
         */
        return((((cv.p_mant[LOW] & 0x000fffff) == 0) && (cv.p_mant[HIGH] == 0))?INF:NAN);
    }

    /* 阶码标识方法为移码表示，移码相当于真值加上2^m-1，m为位数，对于double类型，m为10，所以要获取真值，需要减去2^m-1 */
    /* @KEEP_COMMENT: 设置exp_f值为exp_f-1023，设置any为FALSE */
    exp_f -= 1023;
    any = (BOOL)FALSE;

    /* 阶码小于0，则一定是浮点实数，整数部分等于0 */
    /* @REPLACE_BRACKET: exp_f < 0 */
    if (exp_f < 0)
    {
        /* @KEEP_COMMENT: 设置any为TRUE */
        /* @KEEP_COMMENT: 设置cv.p_mant[LOW]为cv.p_mant[LOW]&0x80000000 */
        /* @KEEP_COMMENT: 设置cv.p_mant[HIGH]值为0 */
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
             * @REPLACE_BRACKET: cv.p_mant高32位(即小数部分)不等于0 || cv.p_mant低
             * 32位尾数的小数部分不等于0
             */
            if ((cv.p_mant[HIGH] != 0) || ((cv.p_mant[LOW] & ~(map[20-exp_f])) != 0))
            {
                /* @KEEP_COMMENT: 设置any为TRUE，设置cv.p_mant[HIGH]的值为0 */
                /* @KEEP_COMMENT: 设置cv.p_mant[LOW]的值为cv.p_mant[LOW]&map[20-exp_f] */
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
                /* @REPLACE_BRACKET: cv.p_mant小数部分不等于0 */
                if ((cv.p_mant[HIGH] & ~(0x00000000)) != 0)
                {
                    /* @KEEP_COMMENT: 设置any为TRUE */
                    /* @KEEP_COMMENT: 设置cv.p_mant[HIGH]值为cv.p_mant[HIGH]&0x00000000 */
                    any = TRUE;
                    cv.p_mant[HIGH] &= 0x00000000;
                }
            }
            else
            {
                /* @REPLACE_BRACKET: exp_f < 52 */
                if (exp_f < 52)
                {
                    /* @REPLACE_BRACKET: cv.p_mant浮点数小数部分不等于0 */
                    if ((cv.p_mant[HIGH] & ~(map [52-exp_f])) != 0)
                    {
                        /*
                         * @KEEP_COMMENT: 设置any为TRUE，设置cv.p_mant[HIGH]的值
                         * 为cv.p_mant[HIGH]&map[52-exp_f]
                         */
                        any = TRUE;
                        cv.p_mant[HIGH] &= map[52-exp_f];
                    }
                }
            }
        }
    }

    /* 保存整数部分的值 */
    /* @REPLACE_BRACKET: r != NULL */
    if (r != NULL)
    {
        /* @KEEP_COMMENT: 设置*r的值为cv.p_double */
        *r = cv.p_double;
    }

    /* @REPLACE_BRACKET: any等于TRUE，返回REAL；any等于FALSE，返回INTEGER */
    return((any == TRUE)?REAL:INTEGER);
}

