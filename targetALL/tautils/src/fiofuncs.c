/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2011-02-23         李燕，北京科银京成技术有限公司
 *                               创建该文件。
 * 2011-04-15	  张勇，北京科银京成技术有限公司
 * 				   增加PDL描述、整理代码格式、优化代码、修改GBJ5369违反项、。
*/

/*
 * @file: fiofuncs.c
 * @brief:
 *	   <li> 提供浮点数输入、输出相关的函数支持。</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/* @<MOD_INFO */
/*
 * @file: DC_RR_1.pdl
 * @brief:
 *	    <li>本模块向用户提供了一系列的常规C库接口，</li>
 *	    <li>C库主要包含字符串和内存块功能模块。</li>
 *	    <li>字符串功能模块主要提供对字符串进行操作的一系列函数，</li>
 *	    <li>内存块功能模块主要提供对指定内存块区域进行操作的一系列函数。</li>
 * @implements: RR.1
 */
/* @MOD_INFO> */ 

/************************头 文 件******************************/

/* @<MOD_HEAD */
#include <string.h>
#include <stdarg.h>
//#include <tamathBase.h>
#include <floatio.h>
/* @MOD_HEAD> */

/************************宏 定 义******************************/

#define BUF   400
#define PADSIZE   16

/************************类型定义******************************/
/************************外部声明******************************/

/*
 * @brief
 *    将浮点数转换为ASCII字符串，并返回转换出来的字符串长度。
 * @param[in]: pVaList: 参数列表 
 * @param[in]: precision: 精确度
 * @param[in]: doAlt: 替代标志
 * @param[in]: fmtSym: 期望的格式类型
 * @param[in]: pBufEnd: 缓冲区结束地址
 * @param[out]: pDoSign: 符号标志
 * @param[out]: pBufStart: 缓冲区起始地址
 * @return:
 *    转换出来的字符个数。
 * @qualification method: 分析/测试
 * @derived requirement: 否
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
			signed char *pBufEnd);
/*
 * @brief: 
 *    从源缓冲区拷贝指定长度的字节到目的缓冲区。
 * @param[in]: inbuf: 源缓冲区
 * @param[in]: length: 复制的长度
 * @param[out]: outptr: 指向目标缓冲区的指针
 * @return: 
 *    OK:拷贝成功
 * @tracedREQ: RR.1.1
 * @implements: DR.1.15
 */
static STATUS putbuf(signed char *inbuf,
					 int length,
					 signed char **outptr);

/************************前向声明******************************/
/************************模块变量******************************/

/* @<MOD_EXTERN */
/* 包含PADSIZE（16）个空格的字符数组 */
static unsigned char blanks[PADSIZE] =
{
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20
};

/* 包含PADSIZE（16）个'0'的字符数组*/
static unsigned char zeroes[PADSIZE] =
{
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30
};

/* 将浮点数格式化为ASCII字符串的函数的函数指针*/
static FUNCPTR fioFltFormatRtn = fformat;

/* @MOD_EXTERN> */

/************************全局变量******************************/

/* @<MOD_VAR */
/* 表明一个数的精确度中是否应包含符号 */
BOOL signofnum = TRUE;

/* @MOD_VAR> */

/************************实    现******************************/

/* @MODULE> */


/**
 * @req
 * @brief: 
 *    将可选参数按指定的格式转换后输出到指定的缓冲区，并返回输出的字符个数(不包
 *    括结束字符)。<br>
 * @param[in]: fmt: 格式化字符串
 * @param[in]: ...: 可变参数列表
 * @param[out]: buffer: 被写入的缓冲区指针
 * @return: 
 *    输出的字符个数。
 *    ERROR：参数错误或输出字符失败。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes:
 *    函数中不检查格式化字符串、被写入的缓冲区指针和可变参数列表的有效性。<br>
 *    字符串<fmt>包括普通字符（不会被格式转换而直接输出的字符）和格式转换控制字
 *    符串（控制<fmt>后面的参数被转换和输出的格式）。<br>
 * @implements: DR.1.1
 */
int sprintf(char *buffer, const char *fmt, ...)
{
    va_list vaList;
    int nChars = 0;

    /* @KEEP_COMMENT: 开始可变参数读取 */
    va_start(vaList, fmt);

    /*
     * @brief:
     *    针对<fmt>的格式转换按以下情况进行操作:<br>
     *    (1)如果<fmt>为空串("")，则向<buffer>中写入'\0'作为结束符，返回0。<br>
     *    (2)将普通字符按照顺序传递给<buffer>。<br>
     *    (3)将转义字符(以'\'开头的字符)按照顺序转换为指定含义的字符串传递给<buffer>。<br>
     *    (4)将格式控制串(\n : 回车，\r : 换行，\t : Tab空格)转换字符串并将它们传递给<br>
     *       <buffer>。<br>
     *    (5)如果百分号后跟随一个无意义的格式字符，则直接将该字符传递给<buffer>。<br>
     *    (6)如果百分号后紧跟着可以被解释为合法标志(参看注释中的flags描述)、宽度、<br>
     *       精确度、或者大小的格式控制字符，但是当这些字符后没有跟随合法的类型字符<br>
     *       (注解中的任何一个)时，并且如果使用了'0'格式标志，则将<width>-1个空格<br>
     *       及跟随的字符传递给<buffer>，而未使用'0'格式标志，则向<buffer>传递<br>
     *       <width>-1个0字符及跟随的字符。<br>
     *    (7)如果变参<...>中浮点数是一个无穷数，为正数时，将"Inf"传递给<buffer>；<br>
     *       为负数时，将"-Inf"传递给<buffer>。<br>
     *    (8)如果变参<...>中浮点数不是一个数字，将"NaN"传递给<buffer>。<br>
     *    (9)<fmt>的格式转换结束后，向<buffer>中写入'\0'作为结束符，返回<buffer><br>
     *       的字符个数。<br>
     * @qualification method: 测试
     * @notes:
     *    NaN：Not a Number，表示非数字定义<br>
     *    INF：Infinity，无穷大数<br>
     *    -INF：负无穷大数<br>
     *    +INF：正无穷大数<br>
     *<br>
     *    <fmt>中的子串格式为:<br>
     *        "[xxx][%[flags][width][.precision][ 'l' ]type][xxx]"<br>
     *        [xxx]为任意字符串。<br>
     *        []中内容为可选。<br>
     *<br> 
     *    格式说明通常以百分号开始，并从左到右进行被读取出来后对百分号后面的格式
     *    符号进行解析。格式包含一些可选和必须的域，具有如下组成形式：<br>
     *        %[flags][width][.precision][ 'l' ]type<br>
     *<br>
     *    (1) flag（可选）是一个用于调整输出和打印符号位、空格、小数点、八进制和<br>
     *        十六进制前缀的字符。可能会有一个或一个以上的flag出现在格式说明中。<br>
     *        Flag说明：<br>
     *         - 按照width指定的宽度左对齐转换出来的结果字符串<br>
     *         + 如果输出值是有符号的则在前面加上+或-前缀<br>
     *         ' '(空格) 如果输出值是有符号的并且是正的（当和+一起使用时忽略该标<br>
     *             志）<br>
     *         * 按照输入非负整型可变参数指定的宽度右对齐转换出来的结果字符串，<br>
     *           前面添加空格<br>
     *         # 根据type域中指定的输出转换类型，将结果转换为适当的形式：<br>
     *           0: 一个非零的结果将有一个0前缀<br>
     *           x: 一个非零的结果将有一个0x前缀<br>
     *           X: 一个非零的结果将有一个0X前缀<br>
     *           e: 结果总包含一个十进制小数点，即使小数点后没有数字<br>
     *           E: 结果总包含一个十进制小数点，即使小数点后没有数字<br>
     *           f: 结果总包含一个十进制小数点，即使小数点后没有数字<br>
     *           g: 同e和E,不过尾随的零不去掉<br>
     *           G: 同e和E,不过尾随的零不去掉<br>
     *         0 如果在width域中指定了域宽，在width前面有0标志，并且输出转换类型字符<br>
     *           是d，i，o，u，x，X，e，E，f，g，或者G，则用空格来填补域宽，而不是用0。<br>
     *           如果0标志和减号标志同时出现，零标志被忽略。对d，i，o，u，x，和X转换，<br>
     *           如果指定了一个精度precision，零标志被忽略。注意0被解释为一个标志，<br>
     *           不是一个域宽的开始。<br>
     *    (2) width（可选）是一个非负的十进制整数。它说明最小的输出字符个数。如<br>
     *        果输出字符个数小于width，则在输出字符的左边或右边填充0。如果<br>
     *        width被0前缀，则填充空格。<br>
     *    (3) precision（可选）是一个非负的十进制整数。被放于句点(.)后。它表示：<br>
     *        对于字符串：最大的被打印输出的字符个数；对于整数：最小的被打印输出的<br>
     *        数字个数；对于浮点数：有意义数字的个数。precision为'*'时，表示根据输<br>
     *        入整型可变参数表示精度。<br>
     *    (4) size（可选）说明一个必需的参数作为长整形（缺省为整型）的长度。对于<br>
     *        'e'和'f'没有任何意义。<br>
     *    (5) type（必需）决定相关的参数按何种类型被解释。<br>
     *        h 迫使跟在后面的d，I，o，u，x或者X类型的输出转换适用于一个短整数或<br>
     *          者无符号短整数。<br>
     *        l %l迫使后面的d，i，o，u，x或者X转换类型适用于一个长整数或者无符号长<br>
     *          整数。%ll迫使后面的d，i，o，u，x或者X转换类型适用于一个长长整数或者<br>
     *          无符号长长整数。<br>
     *        c 字符<br>
     *        n 统计格式控制字符串中%n之前printf打印出的所有字符的个数。可变参数<br>
     *          列表中与%n对应的可变参数应为一个int型指针变量，用来保存统计到的字<br>
     *          符个数；%ln对应long型指针变量；%lln对应long long型指针变量；%hn对<br>
     *          应short型指针变量。<br>
     *        p 以十六进制的地址形式(例如FFFFFFFF)输出可变参数值。<br>
     *        D 带符号的十进制整数<br>
     *        d 带符号的十进制整数<br>
     *        s 字符串，直至达到指定精度或者遇到一个字符串结束符，可变参数为NULL<br>
     *          则打印(null)<br>
     *        U 无符号十进制整数<br>
     *        u 无符号十进制整数<br>
     *        X 无符号十六进制整数(字母数字大写ABCDEF)<br>
     *        x 无符号十六进制整数(字母数字小写abcdef)<br>
     *        O 八进制整数<br>
     *        o 八进制整数<br>
     *        e 以[-]9.9999e[+|-]999的形式输出一个带符号的值<br>
     *        E 以[-]9.9999E[+|-]999的形式输出一个带符号的值<br>
     *        g 以f或者e的形式输出一个带符号的值，根据给出的值和精度；<br>
     *          仅当需要时才输出跟随的零和十进制小数点<br>
     *        G 同g的方式输出，但是如果需要一个指数时用E来表示<br>
     *        f 形式为[]D.D（D - 一个或多个十进制数）的浮点数<br>
     * @derived requirement: 否
     * @implements: 1 
     */
    /*
     * @KEEP_COMMENT: 调用fioformatstr(DR.1.13)将可变参数根据<fmt>进行格式化后输
     * 出到<buffer>，返回字符数量至变量nChars
     */
    nChars = fioformatstr((const signed char *)fmt, vaList, (int)&buffer);

    /* @KEEP_COMMENT: 结束可变参数读取，设置<buffer>当前数据为'\0' */
    va_end(vaList);
    *buffer = EOS;

    /* @REPLACE_BRACKET: nChars */
    return(nChars);
}
static signed char    FMT[20] = {0x0};
static signed char    buf[BUF] = {0x0};
static signed char    ox[2] = {0x0};
/*
 * @brief: 
 *    将参数列表按给定的格式转换后，输出到指定的设备，返回被输出字符的数目。
 * @param[in]: fmt: 格式控制字符串
 * @param[in]: vaList: 可变参数列表
 * @param[in]: outarg: putbuf的参数
 * @return: 
 *    ERROR(-1): 输出字符失败。
 *    输出字符的数目。
 * @notes: 
 *    ERROR = -1
 * @tracedREQ: RR.1.1
 * @implements: DR.1.14
 */
int fioformatstr(const signed char *fmt, va_list vaList, int outarg)
{
    signed char ch = 0x0;
    int n = 0;
    signed char *cp = NULL;
    const signed char *tmpChrAdd = NULL;
    int width = 0;
    signed char sign = 0x0;
	long long longLongVal = 0;
    unsigned long long ulongLongVal = 0;
    int prec = 0;
    int oldprec = 0;
    int dprec = 0;
    int fpprec = 0;
    int size = 0;
    int fieldsz = 0;
    int realsz = 0;
    
    signed char *  Collect = NULL;
    BOOL    doLongInt = 0;
    BOOL    doLongLongInt = 0;
    BOOL    doShortInt = 0;
    BOOL    doAlt = 0;
    BOOL    doLAdjust = 0;
    BOOL    doZeroPad = 0;
    BOOL    doHexPrefix = 0;
    BOOL    doSign = 0;

    /* @KEEP_COMMENT: 定义一块长度为400个字节的内存缓冲区buf */

    signed char *  xdigs = NULL;

    /* @KEEP_COMMENT: 定义表示返回值的局部变量ret，并初始化为0 */
    int ret = 0;

    /* @KEEP_COMMENT: while(1) */
    FOREVER
    {
        /* @KEEP_COMMENT: 将字符串的首地址给cp，获取当前字符给ch */
        cp = CHAR_FROM_CONST(fmt);
        ch = (*fmt);

        /* @REPLACE_BRACKET: ch不等于'\0' && ch不等于'%' */
        while ((ch != (signed char)EOS) && (ch != (signed char)'%'))
        {
            /* @KEEP_COMMENT: <fmt>++，设置ch为<*fmt> */
            fmt++;
            ch = *fmt;
        }

        /* @KEEP_COMMENT: 设置tmpChrAdd为<fmt>，设置n为tmpChrAdd-cp */
        tmpChrAdd = fmt;
        n = tmpChrAdd - cp;

        /* @REPLACE_BRACKET: n不等于0 */
        if (n != 0)
        {
            /* @KEEP_COMMENT: 将cp拷贝n个字节到<outarg> */
            putbuf(cp, n, (signed char **)outarg);

            /* @KEEP_COMMENT: 设置ret为ret+n */
            ret += n;
        }

        /* @REPLACE_BRACKET: 当前字符ch为'\0' */
        if (ch == (signed char)EOS)
        {
            /* @REPLACE_BRACKET: ret */
            return(ret);
        }

        /* @KEEP_COMMENT: <fmt>++，设置*FMT为'\0'，设置Collect为FMT */
        fmt++;
        *FMT        = EOS;
        Collect     = FMT;

        /* @KEEP_COMMENT: 设置长整型标志doLongInt为FALSE */
        doLongInt   = FALSE;

        /* @KEEP_COMMENT: 设置64位整型标志doLongLongInt为FALSE */
        doLongLongInt = FALSE;

        /* @KEEP_COMMENT: 设置短整型标志doShortInt为FALSE */
        doShortInt  = FALSE;

        /* @KEEP_COMMENT: 设置转换形式标志doAlt为FALSE */
        doAlt       = FALSE;

        /* @KEEP_COMMENT: 设置左对齐标志doLAdjust为FALSE */
        doLAdjust   = FALSE;

        /* @KEEP_COMMENT: 设置零填充标志doZeroPad为FALSE */
        doZeroPad   = FALSE;

        /* @KEEP_COMMENT: 设置十六进制前缀标志doHexPrefix为FALSE */
        doHexPrefix = FALSE;

        /* @KEEP_COMMENT: 设置符号标志doSign为FALSE */
        doSign      = FALSE;

        /* @KEEP_COMMENT: 设置精度标志dprec为0 */
        dprec       = 0;

        /* @KEEP_COMMENT: 设置浮点精度标志fpprec为0 */
        fpprec      = 0;

        /* @KEEP_COMMENT: 设置宽度width为0 */
        width       = 0;

        /* @KEEP_COMMENT: 设置精度prec为-1 */
        prec        = -1;

        /* @KEEP_COMMENT: 设置前精度oldprec为-1 */
        oldprec     = -1;

        /* @KEEP_COMMENT: 设置符号sign为'\0' */
        sign        = EOS;

        /* @KEEP_COMMENT: #define get_CHAR  获取当前字符至ch，并将<fmt>指向下个字符 */
#define get_CHAR  {ch = *fmt; *Collect = *fmt; Collect++; fmt++;}

        /* @KEEP_COMMENT: get_CHAR */
        get_CHAR;

        /* @REPLACE_BRACKET: 1 */
        while (1)
        {
            /* @REPLACE_BRACKET: ch */
            switch (ch)
            {
                case ' ':
                    /* @REPLACE_BRACKET: sign等于0 */
                    if (0 == sign)
                    {
                        /* @KEEP_COMMENT: 设置sign为空格 */
                        sign = ' ';
                    }

                    /* @KEEP_COMMENT: get_CHAR */
                    get_CHAR;
                    break;

                case '#':
                    /* @KEEP_COMMENT: 设置doAlt为TRUE，get_CHAR */
                    doAlt = TRUE;
                    get_CHAR;
                    break;

                case '*':
                    /* @KEEP_COMMENT: 获取可变参数中下一个int型的参数保存至width */
                    width = va_arg(vaList, int);

                    /* @REPLACE_BRACKET: width大于等于0 */
                    if (width >= 0)
                    {
                        /* @KEEP_COMMENT: get_CHAR */
                        get_CHAR;
                        break;  
                    }

                    /* @KEEP_COMMENT: 设置width为-width，设置doLAdjust为TRUE */
                    width = -width;
                    doLAdjust = TRUE;

                    /* @KEEP_COMMENT: get_CHAR */
                    get_CHAR;
                    break;

                case '-':
                    /* @KEEP_COMMENT: 设置doLAdjust为TRUE，get_CHAR */
                    doLAdjust = TRUE;
                    get_CHAR;
                    break;

                case '+':
                    /* @KEEP_COMMENT: 设置sign为'+'，get_CHAR */
                    sign = '+';
                    get_CHAR;
                    break;

                case '.':
                    /* @KEEP_COMMENT: get_CHAR */
                    get_CHAR;

                    /* @REPLACE_BRACKET: ch为'*' */
                    if (ch == (signed char)'*')
                    {
                        /* @KEEP_COMMENT: 获取int型可变参数到n */
                        n = va_arg(vaList, int);

                        /* @KEEP_COMMENT: 如果n小于0，设置prec为-1；否者设置prec为n */
                        prec = (n < 0) ? -1 : n;

                        /* @KEEP_COMMENT: get_CHAR */
                        get_CHAR;
                        break;
                    }

                    /* @KEEP_COMMENT: 设置n为0 */
                    n = 0;

                    /* @REPLACE_BRACKET: ch为0~9范围内的数字 */
                    while (is_digit(ch))
                    {
                        /* @KEEP_COMMENT: 设置n为10*n+(ch转换成的数字)，get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;
                    }

                    /* @KEEP_COMMENT: 如果n小于0，设置prec为-1；否者设置prec为n */
                    prec = n < 0 ? -1 : n;
                    break;

                case '0':
                    /* @KEEP_COMMENT: 设置doZeroPad为TRUE，get_CHAR */
                    doZeroPad = TRUE;
                    get_CHAR;
                    break;

                case '1':
                    /* @KEEP_COMMENT: 设置n为0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: 设置n为10*n+ch转换成的数字，get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: ch为数字 */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: 设置width为n */
                    width = n;
                    break;

                case '2':
                    /* @KEEP_COMMENT: 设置n为0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: 设置n为10*n+ch转换成的数字，get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: ch为数字 */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: 设置宽度width为n */
                    width = n;
                    break;

                case '3':
                    /* @KEEP_COMMENT: 设置n为0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: 设置n为10*n+ch转换成的数字，get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: ch为数字 */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: 设置宽度width为n */
                    width = n;
                    break;

                case '4':
                    /* @KEEP_COMMENT: 设置n为0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: 设置n为10*n+ch转换成的数字，get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: ch为数字 */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: 设置宽度width为n */
                    width = n;
                    break;

                case '5':
                    /* @KEEP_COMMENT: 设置n为0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: 设置n为10*n+ch转换成的数字，get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: ch为数字 */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: 设置宽度width为n */
                    width = n;
                    break;

                case '6':
                    /* @KEEP_COMMENT: 设置n为0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: 设置n为10*n+ch转换成的数字，get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: ch为数字 */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: 设置宽度width为n */
                    width = n;
                    break;

                case '7':
                    /* @KEEP_COMMENT: 设置n为0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: 设置n为10*n+ch转换成的数字，get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: ch为数字 */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: 设置宽度width为n */
                    width = n;
                    break;

                case '8':
                    /* @KEEP_COMMENT: 设置n为0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: 设置n为10*n+ch转换成的数字，get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: ch为数字 */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: 设置宽度width为n */
                    width = n;
                    break;

                case '9':
                    /* @KEEP_COMMENT: 设置n为0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: 设置n为10*n+ch转换成的数字，get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: ch为数字 */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: 设置宽度width为n */
                    width = n;
                    break;

                case 'h':
                    /* @KEEP_COMMENT: 设置doShortInt为TRUE，get_CHAR */
                    doShortInt = TRUE;
                    get_CHAR;
                    break;

                case 'l':
                    /* @KEEP_COMMENT: 从fmt获取下一个字符ch */
                    get_CHAR;

                    /* @REPLACE_BRACKET: ch等于'l' */
                    if (ch == (signed char)'l')
                    {
                        /* @KEEP_COMMENT: 设置64位整型标志doLongLongInt为TRUE */
                        doLongLongInt = TRUE;

                        /* @KEEP_COMMENT: get_CHAR */
                        get_CHAR;
                        break;
                    }
                    else
                    {
                        /* @KEEP_COMMENT: 设置长整型标志doLongInt为TRUE */
                        doLongInt = TRUE;
                        break;
                    }

                case 'c':
                    /*
                     * @KEEP_COMMENT: 设置cp为缓冲区buf的首地址，获取int型可变参
                     * 数到*cp
                     */
                    cp = buf;
                    *cp = va_arg(vaList, int);

                    /* @KEEP_COMMENT: 设置size为1，设置sign为'\0' */
                    size = 1;
                    sign = EOS;

                    goto outwhile;
                    break;

                case 'D':
                    /* @KEEP_COMMENT: 设置长整型标志doLongInt为TRUE */
                    doLongInt = TRUE;

                    /*
                     * @KEEP_COMMENT: 如果doLongLongInt为真，获取long long
                     * 型可变参数到longLongVal；
                     * 如果doLongInt为真，获取long型可变参数到longLongVal；
                     * 如果doShortInt为真，获取int型可变参数到longLongVal；
                     * 否则获取unsigned int型可变参数到longLongVal
                     */
                    longLongVal = SARG();

                    /* @REPLACE_BRACKET: longLongVal小于0 */
                    if (longLongVal < 0)
                    {
                        /* @KEEP_COMMENT: 设置longLongVal为-longLongVal */
                        longLongVal = -longLongVal;

                        /* @KEEP_COMMENT: 设置sign为'-' */
                        sign = '-';
                    }

                    /* @KEEP_COMMENT: 设置精度标志dprec为精度prec */
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec大于等于0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: 设置零填充标志doZeroPad为FALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: cp = buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: longLongVal不等于0 || prec不等于0 */
                    if ((longLongVal != 0) || (prec != 0))
                    {
                        /* @REPLACE_BRACKET: longLongVal大于等于10 */
                        while (longLongVal >= 10)
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: longLongVal除以10的余数转换成ASCII
                             * 字符保存至*cp
                             */
                            *cp = to_char(longLongVal % 10);

                            /* @KEEP_COMMENT: 设置longLongVal为longLongVal/10 */
                            longLongVal /= 10;
                        }

                        /*
                         * @KEEP_COMMENT: cp--，将longLongVal转换成ASCII字符保存
                         * 至*cp
                         */
                        cp--;
                        *cp = to_char(longLongVal);
                    }

                    /* @KEEP_COMMENT: size = buf+BUF-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'd':
                    /*
                     * @KEEP_COMMENT: 如果doLongLongInt为真，获取long long
                     * 型可变参数到longLongVal；
                     * 如果doLongInt为真，获取long型可变参数到longLongVal；
                     * 如果doShortInt为真，获取int型可变参数到longLongVal；
                     * 否则获取unsigned int型可变参数到longLongVal
                     */
                    longLongVal = SARG();

                    /* @REPLACE_BRACKET: longLongVal小于0 */
                    if (longLongVal < 0)
                    {
                        /* @KEEP_COMMENT: 设置longLongVal为-longLongVal */
                        longLongVal = -longLongVal;

                        /* @KEEP_COMMENT: 设置符号sign为'-' */
                        sign = '-';
                    }

                    /* @KEEP_COMMENT: 设置精度标志dprec为精度prec */
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec大于等于0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: 设置零填充标志doZeroPad为FALSE */
                        doZeroPad = FALSE;
                    }
                    /* @KEEP_COMMENT: cp = buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: longLongVal不等于0 || prec不等于0 */
                    if ((longLongVal != 0) || (prec != 0))
                    {
                        /* @REPLACE_BRACKET: longLongVal大于等于10 */
                        while (longLongVal >= 10)
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: 将longLongVal除以10的余数转换成ASCII
                             * 字符保存至*cp
                             */
                            *cp = to_char(longLongVal % 10);

                            /* @KEEP_COMMENT: 设置longLongVal为longLongVal/10 */
                            longLongVal /= 10;
                        }

                        /* @KEEP_COMMENT: cp--，将longLongVal转换成ASCII字符保存至*cp */
                        cp--;
                        *cp = to_char(longLongVal);
                    }

                    /* @KEEP_COMMENT: 设置size为buf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'i':
                    /*
                     * @KEEP_COMMENT: 如果doLongLongInt为真，获取long long
                     * 型可变参数到longLongVal；
                     * 如果doLongInt为真，获取long型可变参数到longLongVal；
                     * 如果doShortInt为真，获取int型可变参数到longLongVal；
                     * 否则获取unsigned int型可变参数到longLongVal
                     */
                    longLongVal = SARG();

                    /* @REPLACE_BRACKET: longLongVal小于0 */
                    if (longLongVal < 0)
                    {
                        /* @KEEP_COMMENT: 设置longLongVal为-longLongVal */
                        longLongVal = -longLongVal;

                        /* @KEEP_COMMENT: 设置符号sign为'-' */
                        sign = '-';
                    }

                    /* @KEEP_COMMENT: 设置精度标志dprec为精度prec */
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec大于等于0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: 设置零填充标志doZeroPad为FALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: cp = buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: longLongVal不等于0 || prec不等于0 */
                    if ((longLongVal != 0) || (prec != 0))
                    {
                        /* @REPLACE_BRACKET: longLongVal大于等于10 */
                        while (longLongVal >= 10)
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: 将longLongVal除以10的余数转换成
                             * ASCII字符保存至*cp
                             */
                            *cp = to_char(longLongVal % 10);

                            /* @KEEP_COMMENT: 设置longLongVal为longLongVal/10 */
                            longLongVal /= 10;
                        }

                        /*
                         * @KEEP_COMMENT: cp--，将longLongVal转换成ASCII字符保存
                         * 至*cp
                         */
                        cp--;
                        *cp = to_char(longLongVal);
                    }

                    /* @KEEP_COMMENT: size = buf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'n':
                    /* @REPLACE_BRACKET: doLongLongInt不等于0 */
                    if (0 != doLongLongInt)
                    {
                        /*
                         * @REPLACE_LINE: 获取一个long long型的指针，并将该指针指
                         * 向的数据置为ret
                         */
                        *va_arg(vaList, long long *) = (long long)ret;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doLongInt不等于0 */
                        if (0 != doLongInt)
                        {
                            /*
                             * @KEEP_COMMENT: 获取一个long型的指针，并将该指针指
                             * 向的数据置为ret
                             */
                            *va_arg(vaList, long *) = ret;
                        }
                        else
                        {
                            /* @REPLACE_BRACKET: doShortInt不等于0 */
                            if (0 != doShortInt)
                            {
                                /*
                                 * @KEEP_COMMENT: 获取一个short型的指针，并将该指
                                 * 针指向的数据置为ret
                                 */
                                *va_arg(vaList, short *) = ret;
                            }
                            else
                            {
                                /*
                                 * @KEEP_COMMENT: 获取一个int型的指针，并将该指针
                                 * 指向的数据置为ret
                                 */
                                *va_arg(vaList, int *) = ret;
                            } 
                        }
                    }

                    /* @KEEP_COMMENT: 将字符串的首地址给cp，获取当前字符给ch */
                    cp = CHAR_FROM_CONST(fmt);
                    ch = (*fmt);

                    /* @REPLACE_BRACKET: ch不等于'\0' && ch不等于'%' */
                    while ((ch != (signed char)EOS) && (ch != (signed char)'%'))
                    {
                        /* @KEEP_COMMENT: <fmt>++，设置ch为<*fmt> */
                        fmt++;
                        ch = (*fmt);
                    }

                    /* @KEEP_COMMENT: 设置tmpChrAdd为<fmt>，设置n为tmpChrAdd-cp */
                    tmpChrAdd = fmt;
                    n = tmpChrAdd-cp;

                    /* @REPLACE_BRACKET: n不等于0 */
                    if (n != 0)
                    {
                        /* @KEEP_COMMENT: 调用putbuf(DR.1.14)从cp拷贝n长度的字节到<outarg> */
                        putbuf(cp, n, (signed char **)outarg);

                        /* @KEEP_COMMENT: 设置ret为ret+n */
                        ret += n;
                    }

                    /* @REPLACE_BRACKET: ch等于'\0' */
                    if (ch == (signed char)EOS)
                    {
                        /* @REPLACE_BRACKET: ret */
                        return(ret);
                    }

                    /* @KEEP_COMMENT: <fmt>++，设置*FMT为'\0'，设置Collect为FMT */
                    fmt++;
                    *FMT        = EOS;
                    Collect     = FMT;

                    /* @KEEP_COMMENT: 设置长整型标志doLongInt为FALSE */
                    doLongInt   = FALSE;

                    /* @KEEP_COMMENT: 设置64位整型标志doLongLongInt为FALSE */
                    doLongLongInt = FALSE;

                    /* @KEEP_COMMENT: 设置短整型标志doShortInt为FALSE */
                    doShortInt  = FALSE;

                    /* @KEEP_COMMENT: 设置转换形式标志doAlt为FALSE */
                    doAlt       = FALSE;

                    /* @KEEP_COMMENT: 设置左对齐标志doLAdjust为FALSE */
                    doLAdjust   = FALSE;

                    /* @KEEP_COMMENT: 设置零填充标志doZeroPad为FALSE */
                    doZeroPad   = FALSE;

                    /* @KEEP_COMMENT: 设置十六进制前缀标志doHexPrefix为FALSE */
                    doHexPrefix = FALSE;

                    /* @KEEP_COMMENT: 设置符号标志设置doSign为FALSE */
                    doSign      = FALSE;

                    /* @KEEP_COMMENT: 设置精度标志dprec为0 */
                    dprec       = 0;

                    /* @KEEP_COMMENT: 设置浮点精度标志fpprec为0 */
                    fpprec      = 0;

                    /* @KEEP_COMMENT: 设置宽度width为0 */
                    width       = 0;

                    /* @KEEP_COMMENT: 设置精度prec为-1 */
                    prec        = -1;

                    /* @KEEP_COMMENT: 设置前精度oldprec为-1 */
                    oldprec     = -1;

                    /* @KEEP_COMMENT: 设置符号sign为'\0' */
                    sign        = EOS;

                    /* @KEEP_COMMENT: get_CHAR */
                    get_CHAR;
                    break;

                case 'O':
                    /* @KEEP_COMMENT: 设置doLongInt为TRUE */
                    doLongInt = TRUE;

                    /*
                     * @KEEP_COMMENT: 如果doLongLongInt为真，获取unsigned long long
                     * 型可变参数到ulongLongVal；
                     * 如果doLongInt为真，获取unsigned long型可变参数到
                     * ulongLongVal；
                     * 如果doShortInt为真，获取int型可变参数到ulongLongVal；
                     * 否则获取unsigned int型可变参数到ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @KEEP_COMMENT: 设置sign为'\0'，设置dprec为prec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec大于等于0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: 设置cp为buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal不等于0 || prec不等于0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        do
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: 将ulongLongVal&7转换成ASCII字符保存
                             * 至*cp
                             */
                            *cp = to_char(ulongLongVal & 7);

                            /* @KEEP_COMMENT: 设置ulongLongVal为ulongLongVal>>3 */
                            ulongLongVal >>= 3;

                            /* @REPLACE_BRACKET: ulongLongVal不等于0 */
                        } while (0 != ulongLongVal);

                        /* @REPLACE_BRACKET: doAlt不等于0 && *cp不等于'0' */
                        if ((0 != doAlt) && (*cp != (signed char)'0'))
                        {
                            /* @KEEP_COMMENT: cp--，设置*cp为'0' */
                            cp--;
                            *cp = '0';
                        }
                    }

                    /* @KEEP_COMMENT: 设置size为buf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'o':
                    /*
                     * @KEEP_COMMENT: 如果doLongLongInt为真，获取unsigned long long
                     * 型可变参数到ulongLongVal；
                     * 如果doLongInt为真，获取unsigned long型可变参数到
                     * ulongLongVal；
                     * 如果doShortInt为真，获取int型可变参数到ulongLongVal；
                     * 否则获取unsigned int型可变参数到ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @KEEP_COMMENT: 设置sign为'\0'，设置dprec为prec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec大于等于0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: 设置cp为buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal不等于0 || prec不等于0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        do
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: 将(ulongLongVal & 7)转换成ASCII字符
                             * 保存至*cp
                             */
                            *cp = to_char(ulongLongVal & 7);

                            /* @KEEP_COMMENT: 设置ulongLongVal为ulongLongVal>>3 */
                            ulongLongVal >>= 3;

                            /* @REPLACE_BRACKET: ulongLongVal不等于0 */
                        } while (0 != ulongLongVal);

                        /* @REPLACE_BRACKET: doAlt不等于0 && *cp不等于'0' */
                        if ((0 != doAlt) && (*cp != (signed char)'0'))
                        {
                            /* @KEEP_COMMENT: cp--，设置*cp为'0' */
                            cp--;
                            *cp = '0';
                        }
                    }

                    /* @KEEP_COMMENT: 设置size为buf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'p':
                    /*
                     * @KEEP_COMMENT: 从可变参数列表获取一个指针，并将该值转换成
                     * unsigned long long类型后保存至ulonglongVal
                     */
                    ulongLongVal = (unsigned long long) (unsigned int) va_arg(vaList, void *);

                    /* @KEEP_COMMENT: 设置局部字符指针变量xdigs为"0123456789abcdef" */
                    xdigs   = "0123456789abcdef";

                    /* @KEEP_COMMENT: 设置doHexPrefix为TRUE，设置ch为'x' */
                    doHexPrefix = TRUE;
                    ch      = 'x';

                    /* @KEEP_COMMENT: 设置sign为'\0'，设置dprec为prec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec大于等于0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: 设置cp为buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal不等于0 || prec不等于0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        do
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /* @KEEP_COMMENT: 设置*cp为*(xdigs+(ulongLongVal&15)) */
                            *cp = (*(xdigs+(ulongLongVal & 15)));

                            /* @KEEP_COMMENT: 设置ulongLongVal为ulongLongVal>>4 */
                            ulongLongVal >>= 4;

                            /* @REPLACE_BRACKET: ulongLongVal不等于0 */
                        } while (0 != ulongLongVal);
                    }

                    /* @KEEP_COMMENT: 设置size为buf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 's':
                    /* @KEEP_COMMENT: 从可变参数列表获取一个char*类型的值保存至cp */
                    cp = va_arg(vaList, char *);

                    /* @REPLACE_BRACKET: NULL == cp */
                    if (NULL == cp)
                    {
                        /* @KEEP_COMMENT: 设置cp为"(null)" */
                        cp = "(null)";
                    }

                    /* @REPLACE_BRACKET: prec大于等于0 */
                    if (prec >= 0)
                    {
                        /*
                         * @KEEP_COMMENT: 在cp指向的字符串的前prec个字符中查找'\0'
                         * 字符，将指向该字符的指针称为p
                         */
                        signed char *p = (signed char *)memchr(cp, 0, prec);

                        /* @REPLACE_BRACKET: p != NULL */
                        if (p != NULL)
                        {
                            /* @KEEP_COMMENT: 设置size为整型(p-cp) */
                            size = (int)(p-cp);
                        }
                        else
                        {
                            /* @KEEP_COMMENT: 设置size为prec */
                            size = prec;
                        }
                    }
                    else
                    {
                        /* @KEEP_COMMENT: 获取字符串cp长度保存至size */
                        size = (int)strlen(cp);
                    }

                    /* @KEEP_COMMENT: 设置sign为'\0' */
                    sign = EOS;
                    goto outwhile;
                    break;

                case 'U':
                    /* @KEEP_COMMENT: 设置doLongInt为TRUE */
                    doLongInt = TRUE;

                    /*
                     * @KEEP_COMMENT: 如果doLongLongInt为真，获取unsigned long long
                     * 型可变参数到ulongLongVal；
                     * 如果doLongInt为真，获取unsigned long型可变参数到
                     * ulongLongVal；
                     * 如果doShortInt为真，获取int型可变参数到ulongLongVal；
                     * 否则获取unsigned int型可变参数到ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @KEEP_COMMENT: 设置sign为'\0'，设置dprec为prec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec大于等于0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: 设置cp为buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal不等于0 || prec不等于0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        /* @REPLACE_BRACKET: ulongLongVal大于等于10 */
                        while (ulongLongVal >= 10)
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: 将ulongLongVal除以10的余数转换成
                             * ASCII字符保存至*cp
                             */
                            *cp = to_char(ulongLongVal % 10);

                            /* @KEEP_COMMENT: 设置ulongLongVal为ulongLongVal/10 */
                            ulongLongVal /= 10;
                        }

                        /*
                         * @KEEP_COMMENT: cp--，并将ulongLongVal转换成ASCII字符保
                         * 存至*cp
                         */
                        cp--;
                        *cp = to_char(ulongLongVal);
                    }

                    /* @KEEP_COMMENT: size = buf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'u':
                    /*
                     * @KEEP_COMMENT: 如果doLongLongInt为真，获取unsigned long long
                     * 型可变参数到ulongLongVal；
                     * 如果doLongInt为真，获取unsigned long型可变参数到
                     * ulongLongVal；
                     * 如果doShortInt为真，获取int型可变参数到ulongLongVal；
                     * 否则获取unsigned int型可变参数到ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @KEEP_COMMENT: 设置sign为'\0'，设置dprec为prec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec大于等于0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: 设置cp为buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal不等于0 || prec不等于0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        /* @REPLACE_BRACKET: ulongLongVal大于等于10 */
                        while (ulongLongVal >= 10)
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: 将ulongLongVal除以10的余数转换成
                             * ASCII字符保存至*cp
                             */
                            *cp = to_char(ulongLongVal % 10);

                            /* @KEEP_COMMENT: 设置ulongLongVal为ulongLongVal/10 */
                            ulongLongVal /= 10;
                        }

                        /*
                         * @KEEP_COMMENT: cp--，并将ulongLongVal转换成ASCII字符保
                         * 存至*cp
                         */
                        cp--;
                        *cp = to_char(ulongLongVal);
                    }

                    /* @KEEP_COMMENT: 设置size为buf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'X':
                    /* @KEEP_COMMENT: 设置局部字符指针变量xdigs为"0123456789ABCDEF" */
                    xdigs = "0123456789ABCDEF";

                    /*
                     * @KEEP_COMMENT: 如果doLongLongInt为真，获取unsigned long long
                     * 型可变参数到ulongLongVal；
                     * 如果doLongInt为真，获取unsigned long型可变参数到
                     * ulongLongVal；
                     * 如果doShortInt为真，获取int型可变参数到ulongLongVal；
                     * 否则获取unsigned int型可变参数到ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @REPLACE_BRACKET: doAlt不等于0 && ulongLongVal不等于0 */
                    if ((0 != doAlt) && (ulongLongVal != 0))
                    {
                        /* @KEEP_COMMENT: 设置doHexPrefix为TRUE */
                        doHexPrefix = TRUE;
                    }

                    /* @KEEP_COMMENT: 设置sign为'\0'，设置dprec为prec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec大于等于0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: 设置cp为buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal不等于0 || prec不等于0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        do
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /* @KEEP_COMMENT: 设置*cp为(*(xdigs+(ulongLongVal&15))) */
                            *cp = (*(xdigs+(ulongLongVal & 15)));

                            /* @KEEP_COMMENT: 设置ulongLongVal为ulongLongVal>>4 */
                            ulongLongVal >>= 4;

                            /* @REPLACE_BRACKET: ulongLongVal不等于0 */
                        } while (0 != ulongLongVal);
                    }

                    /* @KEEP_COMMENT: 设置size为buf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;                  
                    break;

                case 'x':
                    /* @KEEP_COMMENT: 设置局部字符指针变量xdigs为"0123456789abcdef" */
                    xdigs = "0123456789abcdef";

                    /*
                     * @KEEP_COMMENT: 如果doLongLongInt为真，获取unsigned long long
                     * 型可变参数到ulongLongVal；
                     * 如果doLongInt为真，获取unsigned long型可变参数到
                     * ulongLongVal；
                     * 如果doShortInt为真，获取int型可变参数到ulongLongVal；
                     * 否则获取unsigned int型可变参数到ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @REPLACE_BRACKET: doAlt不等于0 && ulongLongVal不等于0 */
                    if ((0 != doAlt) && (ulongLongVal != 0))
                    {
                        /* @KEEP_COMMENT: 设置doHexPrefix为TRUE */
                        doHexPrefix = TRUE;
                    }

                    /* @KEEP_COMMENT: 设置sign为'\0'，设置dprec为prec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec大于等于0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: 设置cp为buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal不等于0 || prec不等于0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        do
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /* @KEEP_COMMENT: 设置*cp为*(xdigs+(ulongLongVal&15))) */
                            *cp = (*(xdigs+(ulongLongVal & 15)));

                            /* @KEEP_COMMENT: 设置ulongLongVal为ulongLongVal>>4 */
                            ulongLongVal >>= 4;

                            /* @REPLACE_BRACKET: ulongLongVal不等于0 */
                        } while (0 != ulongLongVal);
                    }

                    /* @KEEP_COMMENT: 设置size为buf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'e':
                    /* @KEEP_COMMENT: 设置oldprec为prec */
                    oldprec = prec;

                    /* @REPLACE_BRACKET: prec大于39 */
                    if (prec > MAXFRACT)
                    {
                        /* @REPLACE_BRACKET: (ch不为'g' && ch不为'G') || doAlt为TRUE */
                        if (((ch != (signed char)'g') && (ch != (signed char)'G')) || (TRUE == doAlt))
                        {
                            /* @KEEP_COMMENT: 设置fpprec为prec-39 */
                            fpprec = prec-MAXFRACT;
                        }

                        /* @KEEP_COMMENT: 设置prec为39 */
                        prec = MAXFRACT;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: prec等于-1 */
                        if (-1 == prec)
                        {
                            /* @KEEP_COMMENT: 设置prec为6 */
                            prec = 6;
                        }
                    }

                    /* @KEEP_COMMENT: 设置cp为buf，设置*cp为'\0' */
                    cp  = buf;
                    *cp = EOS;

                    /*
                     * @KEEP_COMMENT: 将浮点型可变参数转换为字符串到buf，返回字符
                     * 串长度到size
                     */
                    size = (*fioFltFormatRtn)((va_list *)vaList, prec, doAlt, ch, &doSign,cp,buf+BUF);

                    /* @REPLACE_BRACKET: size小于0 */
                    if ((int)size < 0)
                    {
                        /* @KEEP_COMMENT: 设置size为-size，设置prec为oldprec， */
                        size = -size;
                        prec = oldprec;

                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;

                        /* @REPLACE_BRACKET: doSign为TRUE */
                        if (TRUE == doSign)
                        {
                            /* @KEEP_COMMENT: 设置sign为'-' */
                            sign = '-';
                        }
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doSign不等于0 */
                        if (0 != doSign)
                        {
                            /* @KEEP_COMMENT: 设置sign为'-' */
                            sign = '-';
                        }

                        /* @REPLACE_BRACKET: *cp等于'\0' */
                        if (EOS == *cp)
                        {
                            /* @KEEP_COMMENT: cp++ */
                            cp++;
                        }
                    }

                    goto outwhile;
                    break;

                case 'E':
                    /* @KEEP_COMMENT: 设置oldprec为prec */
                    oldprec = prec;

                    /* @REPLACE_BRACKET: prec大于39 */
                    if (prec > MAXFRACT)
                    {
                        /* @KEEP_COMMENT: 设置fpprec为prec-39，设置prec为39 */
                        fpprec = prec-MAXFRACT;
                        prec = MAXFRACT;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: prec等于-1 */
                        if (-1 == prec)
                        {
                            /* @KEEP_COMMENT: 设置prec为6 */
                            prec = 6;
                        }
                    }

                    /* @KEEP_COMMENT: 设置cp为buf，设置*cp为'\0' */
                    cp  = buf;
                    *cp = EOS;

                    /*
                     * @KEEP_COMMENT: 将浮点型可变参数转换为字符串到buf，返回字符
                     * 串长度到size
                     */
                    size = (*fioFltFormatRtn)((va_list *)vaList, prec, doAlt, ch, &doSign,cp,buf+BUF);

                    /* @REPLACE_BRACKET: size小于0 */
                    if ((int)size < 0)
                    {
                        /* @KEEP_COMMENT: 设置size为-size，设prec为oldprec， */
                        size = -size;
                        prec = oldprec;

                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;

                        /* @REPLACE_BRACKET: doSign等于TRUE */
                        if (TRUE == doSign)
                        {
                            /* @KEEP_COMMENT: 设置sign为'-' */
                            sign = '-';
                        }
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doSign不等于0 */
                        if (0 != doSign)
                        {
                            /* @KEEP_COMMENT: 设置sign为'-' */
                            sign = '-';
                        }

                        /* @REPLACE_BRACKET: *cp等于'\0' */
                        if (EOS == *cp)
                        {
                            /* @KEEP_COMMENT: cp++ */
                            cp++;
                        }
                    }

                    goto outwhile;
                    break;

                case 'f':
                    /* @KEEP_COMMENT: 设置oldprec为prec */
                    oldprec = prec;

                    /* @REPLACE_BRACKET: prec大于39 */
                    if (prec > MAXFRACT)
                    {
                        /* @KEEP_COMMENT: 设置fpprec为prec-39，设置prec为39 */
                        fpprec = prec-MAXFRACT;
                        prec = MAXFRACT;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: prec等于-1 */
                        if (-1 == prec)
                        {
                            /* @KEEP_COMMENT: 设置prec为6 */
                            prec = 6;
                        }
                    }

                    /* @KEEP_COMMENT: 设置cp为buf，设置*cp为'\0' */
                    cp  = buf;
                    *cp = EOS;

                    /*
                     * @KEEP_COMMENT: 将浮点型可变参数转换为字符串到buf，返回字符
                     * 串长度到size
                     */
                    size = (*fioFltFormatRtn)((va_list *)vaList, prec, doAlt, ch, &doSign,cp,buf+BUF);

                    /* @REPLACE_BRACKET: size小于0 */
                    if ((int)size < 0)
                    {
                        /* @KEEP_COMMENT: 设置size为-size，设置prec为oldprec， */
                        size = -size;
                        prec = oldprec;

                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;

                        /* @REPLACE_BRACKET: doSign等于TRUE */
                        if (TRUE == doSign)
                        {
                            /* @KEEP_COMMENT: 设置sign为'-' */
                            sign = '-';
                        }
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doSign不等于0 */
                        if (0 != doSign)
                        {
                            /* @KEEP_COMMENT: 设置sign为'-' */
                            sign = '-';
                        }

                        /* @REPLACE_BRACKET: *cp等于'\0' */
                        if (EOS == *cp)
                        {
                            /* @KEEP_COMMENT: cp++ */
                            cp++;
                        }
                    }

                    goto outwhile;
                    break;

                case 'g':
                    /* @KEEP_COMMENT: 设置oldprec为prec */
                    oldprec = prec;

                    /* @REPLACE_BRACKET: prec大于39 */
                    if (prec > MAXFRACT)
                    {
                        /* @REPLACE_BRACKET: doAlt为TRUE */
                        if (TRUE == doAlt)
                        {
                            /* @KEEP_COMMENT: 设置fpprec为prec-39 */
                            fpprec = prec-MAXFRACT;
                        }

                        /* @KEEP_COMMENT: 设置prec为39 */
                        prec = MAXFRACT;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: prec等于-1 */
                        if (-1 == prec)
                        {
                            /* @KEEP_COMMENT: 设置prec为6 */
                            prec = 6;
                        }
                    }

                    /* @KEEP_COMMENT: 设置cp为buf，设置*cp为'\0' */
                    cp  = buf;
                    *cp = EOS;

                    /*
                     * @KEEP_COMMENT: 将浮点型可变参数转换为字符串到buf，返回字符
                     * 串长度到size
                     */
                    size = (*fioFltFormatRtn)((va_list *)vaList, prec, doAlt, ch, &doSign,cp,buf+BUF);

                    /* @REPLACE_BRACKET: size小于0 */
                    if ((int)size < 0)
                    {
                        /* @KEEP_COMMENT: 设置size为-size，设置prec为oldprec， */
                        size = -size;
                        prec = oldprec;

                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;

                        /* @REPLACE_BRACKET: doSign等于TRUE */
                        if (TRUE == doSign)
                        {
                            /* @KEEP_COMMENT: 设置sign为'-' */
                            sign = '-';
                        }
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doSign不等于0 */
                        if (0 != doSign)
                        {
                            /* @KEEP_COMMENT: 设置sign为'-' */
                            sign = '-';
                        }

                        /* @REPLACE_BRACKET: *cp等于'\0' */
                        if (EOS == *cp)
                        {
                            /* @KEEP_COMMENT: cp++ */
                            cp++;
                        }
                    }

                    goto outwhile;
                    break;

                case 'G':
                    /* @KEEP_COMMENT: 设置oldprec为prec */
                    oldprec = prec;

                    /* @REPLACE_BRACKET: prec大于39 */
                    if (prec > MAXFRACT)
                    {
                        /* @REPLACE_BRACKET: doAlt为TRUE */
                        if (TRUE == doAlt)
                        {
                            /* @KEEP_COMMENT: 设置fpprec为prec-39 */
                            fpprec = prec-MAXFRACT;
                        }

                        /* @KEEP_COMMENT: 设置prec为39 */
                        prec = MAXFRACT;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: prec等于-1 */
                        if (-1 == prec)
                        {
                            /* @KEEP_COMMENT: 设置prec为6 */
                            prec = 6;
                        }
                    }

                    /* @KEEP_COMMENT: 设置cp为buf，设置*cp为'\0' */
                    cp  = buf;
                    *cp = EOS;

                    /*
                     * @KEEP_COMMENT: 将浮点型可变参数转换为字符串到buf，返回字符
                     * 串长度到size
                     */
                    size = (*fioFltFormatRtn)((va_list *)vaList, prec, doAlt, ch, &doSign,cp,buf+BUF);

                    /* @REPLACE_BRACKET: size小于0 */
                    if ((int)size < 0)
                    {
                        /* @KEEP_COMMENT: 设置size为-size，设置prec为oldprec， */
                        size = -size;
                        prec = oldprec;

                        /* @KEEP_COMMENT: 设置doZeroPad为FALSE */
                        doZeroPad = FALSE;

                        /* @REPLACE_BRACKET: doSign为TRUE */
                        if (TRUE == doSign)
                        {
                            /* @KEEP_COMMENT: 设置sign为'-' */
                            sign = '-';
                        }
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doSign不等于0 */
                        if (0 != doSign)
                        {
                            /* @KEEP_COMMENT: 设置sign为'-' */
                            sign = '-';
                        }

                        /* @REPLACE_BRACKET: *cp等于'\0' */
                        if (EOS == *cp)
                        {
                            /* @KEEP_COMMENT: cp++ */
                            cp++;
                        }
                    }

                    goto outwhile;
                    break;

                default:
                    /* @REPLACE_BRACKET: ch等于'\0' */
                    if (ch == (signed char)EOS)
                    {
                        /* @REPLACE_BRACKET: ret */
                        return(ret);
                    }

                    /*
                     * @KEEP_COMMENT: 设置cp为buf，设置*cp为ch，设置size为1，
                     * 设置sign为'\0'
                     */
                    cp   = buf;
                    *cp  = ch;
                    size = 1;
                    sign = EOS;

                    goto outwhile;
                    break;
            }
        }

        /* 
         * 到此所有合理的格式字符都被解释完。此时cp指向
         * 一个长度为width的字符串。如果doZeroPad标志此时为1，
         * 则该字符串的开头应该是符号或者前缀；否则，在前缀
         * 之前应该是空格进行填充。在这些左边填充的前缀之后
         * 根据十进制[diouxX]精度填充0。然后打印转换出来的字符串。
         * 在此之后根据浮点精度的需要填充0。最后，如果doLAdjust为1
         * 则在后面填充空格。
         */
        /* @KEEP_COMMENT: outwhile */
        outwhile:
        /* @KEEP_COMMENT: 设置fieldsz为size+fpprec */
        fieldsz = size+fpprec;

        /* @REPLACE_BRACKET: sign不等于'\0' */
        if (sign != (signed char)EOS)
        {
            /* @KEEP_COMMENT: fieldsz++ */
            fieldsz++;

            /* @REPLACE_BRACKET: signofnum不等于0 */
            if (0 != signofnum)
            {
                /* @KEEP_COMMENT: dprec++ */
                dprec++;
            }
        }
        else
        {
            /* @REPLACE_BRACKET: doHexPrefix不等于0 */
            if (0 != doHexPrefix)
            {
                /* @KEEP_COMMENT: 设置fieldsz为fieldsz+2 */
                fieldsz += 2;
            }
        }

        /*
         * @KEEP_COMMENT: 如果dprec大于fieldsz，设置realsz为dprec；
         * 否则设置realsz为fieldsz
         */
        realsz = (dprec > fieldsz) ? dprec : fieldsz;

        /* @REPLACE_BRACKET: doLAdjust等于0 && doZeroPad等于0 */
        if ((0 == doLAdjust) && (0 == doZeroPad))
        {
            /* @KEEP_COMMENT: 设置n为width-realsz */
            n = width-realsz;

            /* @REPLACE_BRACKET: n大于0 */
            if (n > 0)
            {
                /* @REPLACE_BRACKET: n大于16 */
                while (n > PADSIZE)
                {
                    /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝16个字节的'0'到<outarg> */
                    putbuf((signed char *)zeroes, PADSIZE, (signed char **)outarg);

                    /* @KEEP_COMMENT: 设置n为n-16 */
                    n -= PADSIZE;
                }

                /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝n个字节的'0'到<outarg> */
                putbuf((signed char *)zeroes, n, (signed char **)outarg);
            }
        }

        /* @REPLACE_BRACKET: sign不等于0 */
        if (0 != sign)
        {
            /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝1个字节的负号到<outarg> */
            putbuf(&sign, 1, (signed char **)outarg);
        }
        else
        {
            /* @REPLACE_BRACKET: doHexPrefix不等于0 */
            if (0 != doHexPrefix)
            {
                /* @KEEP_COMMENT: 设置ox[0]为'0'，设置ox[1]为ch */
                ox[0] = '0';
                ox[1] = ch;

                /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝2个字节的"0x"到<outarg> */
                putbuf(ox, 2, (signed char **)outarg);
            }
        }

        /* @REPLACE_BRACKET: doLAdjust为0 && doZeroPad不为0 */
        if ((0 == doLAdjust) && (0 != doZeroPad))
        {
            /* @KEEP_COMMENT: 设置n为width-realsz */
            n = width-realsz;

            /* @REPLACE_BRACKET: n大于0 */
            if (n > 0)
            {
                /* @REPLACE_BRACKET: n大于16 */
                while (n > PADSIZE)
                {
                    /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝16个字节的空格到<outarg> */
                    putbuf((signed char *)blanks, PADSIZE, (signed char **)outarg);

                    /* @KEEP_COMMENT: 设置n为n-16 */
                    n -= PADSIZE;
                }

                /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝n个字节的' '到<outarg> */
                putbuf((signed char *)blanks, n, (signed char **)outarg);
            }
        }

        /* @KEEP_COMMENT: 设置n为dprec-fieldsz */
        n = dprec-fieldsz;

        /* @REPLACE_BRACKET: n大于0 */
        if (n > 0)
        {
            /* @REPLACE_BRACKET: n大于16 */
            while (n > PADSIZE)
            {
                /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝16个字节的'0'到<outarg> */
                putbuf((signed char *)zeroes, PADSIZE, (signed char **)outarg);

                /* @KEEP_COMMENT: 设置n为n-16 */
                n -= PADSIZE;
            }

            /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝n个字节的'0'到<outarg> */
            putbuf((signed char *)zeroes, n, (signed char **)outarg);
        }

        /* @KEEP_COMMENT: 调用putbuf(DR.1.14)从cp拷贝size个字节到<outarg> */
        putbuf(cp, size, (signed char **)outarg);

        /* @KEEP_COMMENT: 设置n为fpprec */
        n = fpprec;

        /* @REPLACE_BRACKET: n大于0 */
        if (n > 0)
        {
            /* @REPLACE_BRACKET: n大于16 */
            while (n > PADSIZE)
            {
                /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝16个字节的'0'到<outarg> */
                putbuf((signed char *)zeroes, PADSIZE, (signed char **)outarg);

                /* @KEEP_COMMENT: 设置n为n-16 */
                n -= PADSIZE;
            }

            /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝n个字节的'0'到<outarg> */
            putbuf((signed char *)zeroes, n, (signed char **)outarg);
        }

        /* @REPLACE_BRACKET: doLAdjust不等于0 */
        if (0 != doLAdjust)
        {
            /* @KEEP_COMMENT: 设置n为width-realsz */
            n = width - realsz;

            /* @REPLACE_BRACKET: n大于0 */
            if (n > 0)
            {
                /* @REPLACE_BRACKET: n大于16 */
                while (n > PADSIZE)
                {
                    /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝16个字节的空格到<outarg> */
                    putbuf((signed char *)blanks, PADSIZE, (signed char **)outarg);

                    /* @KEEP_COMMENT: 设置n为n-16 */
                    n -= PADSIZE;
                }

                /* @KEEP_COMMENT: 调用putbuf(DR.1.14)拷贝n个字节的空格到<outarg> */
                putbuf((signed char *)blanks, n, (signed char **)outarg);
            }
        }

        /*
         * @KEEP_COMMENT: 如果width大于realsz，设置ret为ret+width；
         * 否则设置ret为ret+realsz
         */
        ret += (width > realsz) ? width : realsz;
    }

    /* @REPLACE_BRACKET: ERROR */
    return(ERROR);
}

/*
 * @brief: 
 *    从源缓冲区拷贝指定长度的字节到目的缓冲区。
 * @param[in]: inbuf: 源缓冲区
 * @param[in]: length: 复制的长度
 * @param[out]: outptr: 指向目标缓冲区的指针
 * @return: 
 *    OK:拷贝成功
 * @tracedREQ: RR.1.1
 * @implements: DR.1.15
 */
static int putbuf(signed char *inbuf,
int length,
signed char **outptr)
{
    /*
     * @KEEP_COMMENT: 调用bcopy(DR.1.12)从<inbuf>拷贝<length>长度字节到以<outptr>
     * 为起始地址的目的区域
     */
    bcopy(inbuf, *outptr, length);

    /* @KEEP_COMMENT: 设置*outptr为*outptr+length */
    *outptr += (signed char)length;

    /* @REPLACE_BRACKET: OK */
    return(OK);
}


