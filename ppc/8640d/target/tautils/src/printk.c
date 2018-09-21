/************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * 修改历史：
 * 2011-02-23         李燕，北京科银京成技术有限公司
 *                               创建该文件。
 * 2011-04-15	  张勇，北京科银京成技术有限公司
 * 				  增加PDL描述、 整理代码格式、优化代码、修改GBJ5369违反项。
*/

/*
 * @file: printk.c
 * @brief:
 *	   <li> 实现打印输出接口。</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/************************头 文 件******************************/

/* @<MOD_HEAD */
#include "string.h"
#include <stdarg.h>
#include <stdio.h>

/* @MOD_HEAD> */

/************************宏 定 义******************************/

/************************类型定义******************************/

outputfunc_ptr outputChar;

/************************外部声明******************************/
/************************前向声明******************************/
/************************模块变量******************************/
/************************全局变量******************************/
/************************实    现******************************/

/* @MODULE> */
#if 1

/*
 * @brief:
 *    以32位的形式按照<base>和<sign>指定的格式打印数字<num>。
 * @param[in] num: 打印数字个数。
 * @param[in] base: 打印数字的类型。
 * @param[in] sign:  打印数字是否为有符号，1表示有符号，0表示无符号。
 * @return:	
 *    无
 * @tracedREQ: RR.1.11
 * @implements: DR.1.21
 */
static void printNum(unsigned int num, unsigned int base, int sign)
{
	unsigned int n;
	unsigned int count;
	unsigned int toPrint[20];

    /* @REPLACE_BRACKET: <sign>为1 && <num>为负数 */
    if ((sign == 1) && (INT_MIN == (num & INT_MIN)))
    {
        /* @KEEP_COMMENT: 输出负号'-' */
        outputChar('-');

        /* @KEEP_COMMENT: 设置num为正数 */
        num = ((~num)+1);
    }

    /* @KEEP_COMMENT: 计算num/base保存至变量n */
    count = 0;
    n = num / base;

    /* @REPLACE_BRACKET: n大于0 */
    while (n > 0)
    {
        /* @KEEP_COMMENT: 计算num-(n*base)正向保存至数组toPrint */
        toPrint[count] = (num - (n*base));

        /* @KEEP_COMMENT: 设置num为n，设置n为num/base */
        count++;
        num = n;
        n = num / base;
    }

    /* @KEEP_COMMENT: num保存至数组toPrint */
    toPrint[count] = num;
    count++;

    /* @REPLACE_BRACKET: 变量n = 0; n < toPrint的有效个数; n++ */
    for (n = 0; n < count; n++)
    {
        /* @KEEP_COMMENT: 逆向输出数组中的字符 */
        outputChar("0123456789ABCDEF"[toPrint[count-(n+1)]]);
    }
}


/**
 * @req
 * @brief:
 *    将可变参数列表中的参数按照<fmt>指定的格式进行格式化，并把格式化后的数据输
 *    出到指定设备。
 * @param[in]: fmt: 格式字符串
 * @param[in]: ...: 可变参数列表
 * @return:	
 *    无  
 * @qualification method: 分析/测试
 * @derived requirement: 否	
 * @function location: API	
 * @notes:
 *    格式字符串由两种类型的对象组成：<br>
 *        普通字符。被原封不动输出到标准输出中；<br>
 *        转换说明。每个转换说明都以百分号字符‘%’作为起始符，后面紧跟转换字符。<br>
 *    格式字符串中转换字符用来说明输出转换的类型，转换字符及其输出类型如下所示：<br>
 *        d或D: 输出一个有符号十进制整数。<br>
 *        u或U: 输出一个无符号十进制整数。<br>
 *        o或O: 输出一个八进制整数。<br>
 *        x或X: 输出一个十六进制整数。<br>
 *        s: 输出字符串，直到遇到字符串结束符'\0'为止。<br>
 *        c: 输出一个单字符。<br>
 *    如果百分号字符与转换字符之间为字母l时，则将输出的整数作为长整型输出。<br>
 *    如果百分号字符后面的字符不在上述转换字符范围内，则转换字符作为普通字符输出。<br>
 *    如果参数的个数（在参数表中“...”部分填入的参数）少于<fmt>参数中指定的格
 *    式的个数，调用本接口会导致错误的数据输出。<br>
 *    如果参数的个数（在参数表中“...”部分填入的参数）超过了<fmt>参数中指定的
 *    格式的个数，则多余的参数被忽略。
 * @implements: DR.1.11
 */
void printk(char *fmt, ...)
{
    va_list  ap;
    char     c;
    char     *str = NULL;
    int    sign;
    unsigned int      base;
    unsigned int     vaarg;

    /* @KEEP_COMMENT: 开始可变参数读取 */
    va_start(ap, fmt);

    /* @REPLACE_BRACKET: 格式字符串读取; <*fmt>不为'\0'; <fmt>++ */
    for (; '\0' != (*fmt); fmt++)
    {
        /* @KEEP_COMMENT: 设置数字的类型base为0，设置是否有符号sign为0 */
        base  = 0;
        sign = 0;

        /* @REPLACE_BRACKET: <*fmt>为'%' */
        if ('%' == *fmt)
        {
            /* @KEEP_COMMENT: ++<fmt>，设置转换字符c为<*fmt> */
            ++fmt;
            c = *fmt;

            /* @REPLACE_BRACKET: <*fmt>为'l' */
            if ('l' == *fmt)
            {
                /* @KEEP_COMMENT: ++<fmt>，设置转换字符c为<*fmt> */
                ++fmt;
                c = *fmt;
            }

            /* @REPLACE_BRACKET: c */
            switch (c)
            {
                case 'o':
                    /* @KEEP_COMMENT: 设置进制字符base为8，设置符号sign为0 */
                    base = 8;
                    sign = 0;
                    break;

                case 'O':
                    /* @KEEP_COMMENT: 设置进制字符base为8，设置符号sign为0 */
                    base = 8;
                    sign = 0;
                    break;

                case 'd':
                    /* @KEEP_COMMENT: 设置进制字符base为10，设置符号sign为1 */
                    base = 10;
                    sign = 1;
                    break;

                case 'D':
                    /* @KEEP_COMMENT: 设置进制字符base为10，设置符号sign为1 */
                    base = 10;
                    sign = 1;
                    break;

                case 'u':
                    /* @KEEP_COMMENT: 设置进制字符base为10，设置符号sign为0 */
                    base = 10;
                    sign = 0;
                    break;

                case 'U':
                    /* @KEEP_COMMENT: 设置进制字符base为10，设置符号sign为0 */
                    base = 10;
                    sign = 0;
                    break;

                case 'x':
                    /* @KEEP_COMMENT: 设置进制字符base为16，设置符号sign为0 */
                    base = 16;
                    sign = 0;
                    break;

                case 'X':
                    /* @KEEP_COMMENT: 设置进制字符base为16，设置符号sign为0 */
                    base = 16;
                    sign = 0;
                    break;

                case 's':
                    /*
                     * @REPLACE_BRACKET: 可变参数列表中指向字符串的指针到str; 
                     * *str不为'\0'; str++
                     */
                    for (str = va_arg(ap, char *); '\0' != *str; str++)
                    {
                        /* @KEEP_COMMENT: 输出str字符串中每一个字符 */
                        outputChar(*str);
                    }
                    break;

                case 'c':
                    /* @KEEP_COMMENT: 输出可变参数列表中指向无符号整形的字符 */
                    outputChar(va_arg(ap, unsigned int));
                    break;

                default:
                    /* @KEEP_COMMENT: 输出转换字符c的每一个字符 */
                    outputChar(c);
                    break;

            }

            /* @REPLACE_BRACKET: base不为0 */
            if (0 != base)
            {
                /*
                 * @KEEP_COMMENT: 调用printNum(DR.1.19)以32位的形式按照base和sign
                 * 指定的格式打印可变参数列表中指向整形的字符
                 */
                vaarg = (unsigned int)(va_arg(ap, int));
                printNum(vaarg,base, sign);
            }
        }
        else
        {
            /* @KEEP_COMMENT: 输出格式字符串中每一个字符 */
            outputChar(*fmt);
        }
    }

    /* @KEEP_COMMENT: 结束可变参数读取 */
    va_end(ap);
}
#else
T_CHAR* print_buf;
T_WORD   print_cnt;
T_WORD   print_ptr;

#define print_init(_buf,_cnt,_ptr)	\
	do	\
	{	\
		print_buf = _buf;	\
		print_cnt = (_cnt - 1);	\
		print_ptr = _ptr;	\
	}	\
	while(0)

#define print_char(_ch)	\
	do	\
	{	\
		if(print_ptr < print_cnt)	\
			print_buf[print_ptr++] = _ch;	\
	}	\
	while(0)

#define print_end()	\
	do	\
	{	\
		print_buf[print_ptr++] = '\0';	\
	}	\
	while(0)

/**
* @fn 按照指定的进制打印一个数字
* @param num 被打印的数字
* @param base  进制
* @param sign  数字的符号
*/
T_MODULE T_VOID
printNum(T_UDWORD num, T_WORD base, T_WORD sign)
{
  unsigned long long n;
  int count;
  char toPrint[20];

  if ( (sign == 1) && ((long long)num <  0) ) {
    print_char('-');
    num = -num;
  }

  count = 0;
  while ((n = num / base) > 0) {
    toPrint[count++] = (num - (n*base));
    num = n ;
  }
  toPrint[count++] = num;

  for (n = 0; n < count; n++){
    print_char("0123456789ABCDEF"[(int)(toPrint[count-(n+1)])]);
  }
} // printNum


 /**
* @fn 调试代理自用的简单打印函数
* @param buf 用于存放输出字符串的缓冲
* @param cnt 缓冲的长度
* @param fmt  打印格式
* @param arg  可变参数列表
 *@return 返回实际输出到缓冲中的字符个数
*/
T_MODULE T_WORD vsnprint(T_CHAR* buf, T_WORD cnt, const T_CHAR* fmt, va_list arg)
{
	char     c, *str;
	int      lflag, base, sign;

	print_init(buf,cnt,0);

	for (; *fmt != '\0'; fmt++)
	{
	lflag = 0;
	base  = 0;
	sign = 0;
	if (*fmt == '%')
	{
	  if ((c = *++fmt) == 'l')
	  {
	    lflag = 1;
	    c = *++fmt;
	  }
	  switch (c)
	  {
	    case 'o': case 'O': base = 8; sign = 0; break;
	    case 'd': case 'D': base = 10; sign = 1; break;
	    case 'u': case 'U': base = 10; sign = 0; break;
	    case 'x': case 'X': base = 16; sign = 0; break;
	    case 's':
	      for (str = va_arg(arg, char *); *str; str++)
	        print_char(*str);
	      break;
	    case 'c':
#ifdef __PPC__
	      print_char(va_arg(arg, int));
#else
	     print_char(va_arg(arg, int));
#endif
	      break;
	    default:
	      print_char(c);
	      break;
	  } /* switch*/

	  if (base)
	    printNum(lflag ? va_arg(arg, int) : (long long)va_arg(arg, int),
	             base, sign);
	}
	else
	{
	  print_char(*fmt);
	}
	}

	print_end();

	return print_ptr;
} // fnTA_vsnprint

#define TA_PKT_BUF_SIZE 2048
 /**
* @fn 调试代理自用的简单打印函数
* @param fmt 打印格式
* @param ...  可变参数列表
*/
T_VOID printk(T_BYTE *fmt, ...)
{
	static char PrintBuf[TA_PKT_BUF_SIZE];
	va_list ap;
	int cnt;
	int i;

	va_start(ap,fmt);

	/*首先将打印信息输出到本地缓存中*/
	cnt = vsnprint(PrintBuf,TA_PKT_BUF_SIZE,fmt,ap);

	for (i = 0; i < cnt; i++)
	{
		outputChar(PrintBuf[i]);
	}
	


	va_end(ap);
}
#endif

/**
 * @req
 * @brief:
 *    初始化打印单个字符的函数指针。
 * @param[in]: printChar: 单个字符的函数指针
 * @return:	
 *    无	
 * @qualification method: 测试
 * @derived requirement: 否	
 * @function location: API
 * @implements: DR.1.12
 */
void printkInit(outputfunc_ptr printChar)
{
    /* @KEEP_COMMENT: outputChar = printChar */
    outputChar = printChar;
}


