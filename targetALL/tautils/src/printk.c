/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2011-02-23         ���࣬�����������ɼ������޹�˾
 *                               �������ļ���
 * 2011-04-15	  ���£������������ɼ������޹�˾
 * 				  ����PDL������ ��������ʽ���Ż����롢�޸�GBJ5369Υ���
*/

/*
 * @file: printk.c
 * @brief:
 *	   <li> ʵ�ִ�ӡ����ӿڡ�</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/************************ͷ �� ��******************************/

/* @<MOD_HEAD */
#include "string.h"
#include <stdarg.h>
#include <stdio.h>

/* @MOD_HEAD> */

/************************�� �� ��******************************/

/************************���Ͷ���******************************/

outputfunc_ptr outputChar;

/************************�ⲿ����******************************/
/************************ǰ������******************************/
/************************ģ�����******************************/
/************************ȫ�ֱ���******************************/
/************************ʵ    ��******************************/

/* @MODULE> */
#if 1

/*
 * @brief:
 *    ��32λ����ʽ����<base>��<sign>ָ���ĸ�ʽ��ӡ����<num>��
 * @param[in] num: ��ӡ���ָ�����
 * @param[in] base: ��ӡ���ֵ����͡�
 * @param[in] sign:  ��ӡ�����Ƿ�Ϊ�з��ţ�1��ʾ�з��ţ�0��ʾ�޷��š�
 * @return:	
 *    ��
 * @tracedREQ: RR.1.11
 * @implements: DR.1.21
 */
static void printNum(unsigned int num, unsigned int base, int sign)
{
	unsigned int n;
	unsigned int count;
	unsigned int toPrint[20];

    /* @REPLACE_BRACKET: <sign>Ϊ1 && <num>Ϊ���� */
    if ((sign == 1) && (INT_MIN == (num & INT_MIN)))
    {
        /* @KEEP_COMMENT: �������'-' */
        outputChar('-');

        /* @KEEP_COMMENT: ����numΪ���� */
        num = ((~num)+1);
    }

    /* @KEEP_COMMENT: ����num/base����������n */
    count = 0;
    n = num / base;

    /* @REPLACE_BRACKET: n����0 */
    while (n > 0)
    {
        /* @KEEP_COMMENT: ����num-(n*base)���򱣴�������toPrint */
        toPrint[count] = (num - (n*base));

        /* @KEEP_COMMENT: ����numΪn������nΪnum/base */
        count++;
        num = n;
        n = num / base;
    }

    /* @KEEP_COMMENT: num����������toPrint */
    toPrint[count] = num;
    count++;

    /* @REPLACE_BRACKET: ����n = 0; n < toPrint����Ч����; n++ */
    for (n = 0; n < count; n++)
    {
        /* @KEEP_COMMENT: ������������е��ַ� */
        outputChar("0123456789ABCDEF"[toPrint[count-(n+1)]]);
    }
}


/**
 * @req
 * @brief:
 *    ���ɱ�����б��еĲ�������<fmt>ָ���ĸ�ʽ���и�ʽ�������Ѹ�ʽ�����������
 *    ����ָ���豸��
 * @param[in]: fmt: ��ʽ�ַ���
 * @param[in]: ...: �ɱ�����б�
 * @return:	
 *    ��  
 * @qualification method: ����/����
 * @derived requirement: ��	
 * @function location: API	
 * @notes:
 *    ��ʽ�ַ������������͵Ķ�����ɣ�<br>
 *        ��ͨ�ַ�����ԭ�ⲻ���������׼����У�<br>
 *        ת��˵����ÿ��ת��˵�����԰ٷֺ��ַ���%����Ϊ��ʼ�����������ת���ַ���<br>
 *    ��ʽ�ַ�����ת���ַ�����˵�����ת�������ͣ�ת���ַ������������������ʾ��<br>
 *        d��D: ���һ���з���ʮ����������<br>
 *        u��U: ���һ���޷���ʮ����������<br>
 *        o��O: ���һ���˽���������<br>
 *        x��X: ���һ��ʮ������������<br>
 *        s: ����ַ�����ֱ�������ַ���������'\0'Ϊֹ��<br>
 *        c: ���һ�����ַ���<br>
 *    ����ٷֺ��ַ���ת���ַ�֮��Ϊ��ĸlʱ���������������Ϊ�����������<br>
 *    ����ٷֺ��ַ�������ַ���������ת���ַ���Χ�ڣ���ת���ַ���Ϊ��ͨ�ַ������<br>
 *    ��������ĸ������ڲ������С�...����������Ĳ���������<fmt>������ָ���ĸ�
 *    ʽ�ĸ��������ñ��ӿڻᵼ�´�������������<br>
 *    ��������ĸ������ڲ������С�...����������Ĳ�����������<fmt>������ָ����
 *    ��ʽ�ĸ����������Ĳ��������ԡ�
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

    /* @KEEP_COMMENT: ��ʼ�ɱ������ȡ */
    va_start(ap, fmt);

    /* @REPLACE_BRACKET: ��ʽ�ַ�����ȡ; <*fmt>��Ϊ'\0'; <fmt>++ */
    for (; '\0' != (*fmt); fmt++)
    {
        /* @KEEP_COMMENT: �������ֵ�����baseΪ0�������Ƿ��з���signΪ0 */
        base  = 0;
        sign = 0;

        /* @REPLACE_BRACKET: <*fmt>Ϊ'%' */
        if ('%' == *fmt)
        {
            /* @KEEP_COMMENT: ++<fmt>������ת���ַ�cΪ<*fmt> */
            ++fmt;
            c = *fmt;

            /* @REPLACE_BRACKET: <*fmt>Ϊ'l' */
            if ('l' == *fmt)
            {
                /* @KEEP_COMMENT: ++<fmt>������ת���ַ�cΪ<*fmt> */
                ++fmt;
                c = *fmt;
            }

            /* @REPLACE_BRACKET: c */
            switch (c)
            {
                case 'o':
                    /* @KEEP_COMMENT: ���ý����ַ�baseΪ8�����÷���signΪ0 */
                    base = 8;
                    sign = 0;
                    break;

                case 'O':
                    /* @KEEP_COMMENT: ���ý����ַ�baseΪ8�����÷���signΪ0 */
                    base = 8;
                    sign = 0;
                    break;

                case 'd':
                    /* @KEEP_COMMENT: ���ý����ַ�baseΪ10�����÷���signΪ1 */
                    base = 10;
                    sign = 1;
                    break;

                case 'D':
                    /* @KEEP_COMMENT: ���ý����ַ�baseΪ10�����÷���signΪ1 */
                    base = 10;
                    sign = 1;
                    break;

                case 'u':
                    /* @KEEP_COMMENT: ���ý����ַ�baseΪ10�����÷���signΪ0 */
                    base = 10;
                    sign = 0;
                    break;

                case 'U':
                    /* @KEEP_COMMENT: ���ý����ַ�baseΪ10�����÷���signΪ0 */
                    base = 10;
                    sign = 0;
                    break;

                case 'x':
                    /* @KEEP_COMMENT: ���ý����ַ�baseΪ16�����÷���signΪ0 */
                    base = 16;
                    sign = 0;
                    break;

                case 'X':
                    /* @KEEP_COMMENT: ���ý����ַ�baseΪ16�����÷���signΪ0 */
                    base = 16;
                    sign = 0;
                    break;

                case 's':
                    /*
                     * @REPLACE_BRACKET: �ɱ�����б���ָ���ַ�����ָ�뵽str; 
                     * *str��Ϊ'\0'; str++
                     */
                    for (str = va_arg(ap, char *); '\0' != *str; str++)
                    {
                        /* @KEEP_COMMENT: ���str�ַ�����ÿһ���ַ� */
                        outputChar(*str);
                    }
                    break;

                case 'c':
                    /* @KEEP_COMMENT: ����ɱ�����б���ָ���޷������ε��ַ� */
                    outputChar(va_arg(ap, unsigned int));
                    break;

                default:
                    /* @KEEP_COMMENT: ���ת���ַ�c��ÿһ���ַ� */
                    outputChar(c);
                    break;

            }

            /* @REPLACE_BRACKET: base��Ϊ0 */
            if (0 != base)
            {
                /*
                 * @KEEP_COMMENT: ����printNum(DR.1.19)��32λ����ʽ����base��sign
                 * ָ���ĸ�ʽ��ӡ�ɱ�����б���ָ�����ε��ַ�
                 */
                vaarg = (unsigned int)(va_arg(ap, int));
                printNum(vaarg,base, sign);
            }
        }
        else
        {
            /* @KEEP_COMMENT: �����ʽ�ַ�����ÿһ���ַ� */
            outputChar(*fmt);
        }
    }

    /* @KEEP_COMMENT: �����ɱ������ȡ */
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
* @fn ����ָ���Ľ��ƴ�ӡһ������
* @param num ����ӡ������
* @param base  ����
* @param sign  ���ֵķ���
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
* @fn ���Դ������õļ򵥴�ӡ����
* @param buf ���ڴ������ַ����Ļ���
* @param cnt ����ĳ���
* @param fmt  ��ӡ��ʽ
* @param arg  �ɱ�����б�
 *@return ����ʵ������������е��ַ�����
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
* @fn ���Դ������õļ򵥴�ӡ����
* @param fmt ��ӡ��ʽ
* @param ...  �ɱ�����б�
*/
T_VOID printk(T_BYTE *fmt, ...)
{
	static char PrintBuf[TA_PKT_BUF_SIZE];
	va_list ap;
	int cnt;
	int i;

	va_start(ap,fmt);

	/*���Ƚ���ӡ��Ϣ��������ػ�����*/
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
 *    ��ʼ����ӡ�����ַ��ĺ���ָ�롣
 * @param[in]: printChar: �����ַ��ĺ���ָ��
 * @return:	
 *    ��	
 * @qualification method: ����
 * @derived requirement: ��	
 * @function location: API
 * @implements: DR.1.12
 */
void printkInit(outputfunc_ptr printChar)
{
    /* @KEEP_COMMENT: outputChar = printChar */
    outputChar = printChar;
}


