/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2011-02-23         ���࣬�����������ɼ������޹�˾
 *                               �������ļ���
 * 2011-04-15	  ���£������������ɼ������޹�˾
 * 				   ����PDL��������������ʽ���Ż����롢�޸�GBJ5369Υ�����
*/

/*
 * @file: fiofuncs.c
 * @brief:
 *	   <li> �ṩ���������롢�����صĺ���֧�֡�</li>
 * @implements: DR.1
 */

/* @<MODULE */ 

/* @<MOD_INFO */
/*
 * @file: DC_RR_1.pdl
 * @brief:
 *	    <li>��ģ�����û��ṩ��һϵ�еĳ���C��ӿڣ�</li>
 *	    <li>C����Ҫ�����ַ������ڴ�鹦��ģ�顣</li>
 *	    <li>�ַ�������ģ����Ҫ�ṩ���ַ������в�����һϵ�к�����</li>
 *	    <li>�ڴ�鹦��ģ����Ҫ�ṩ��ָ���ڴ��������в�����һϵ�к�����</li>
 * @implements: RR.1
 */
/* @MOD_INFO> */ 

/************************ͷ �� ��******************************/

/* @<MOD_HEAD */
#include <string.h>
#include <stdarg.h>
//#include <tamathBase.h>
#include <floatio.h>
/* @MOD_HEAD> */

/************************�� �� ��******************************/

#define BUF   400
#define PADSIZE   16

/************************���Ͷ���******************************/
/************************�ⲿ����******************************/

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
			signed char *pBufEnd);
/*
 * @brief: 
 *    ��Դ����������ָ�����ȵ��ֽڵ�Ŀ�Ļ�������
 * @param[in]: inbuf: Դ������
 * @param[in]: length: ���Ƶĳ���
 * @param[out]: outptr: ָ��Ŀ�껺������ָ��
 * @return: 
 *    OK:�����ɹ�
 * @tracedREQ: RR.1.1
 * @implements: DR.1.15
 */
static STATUS putbuf(signed char *inbuf,
					 int length,
					 signed char **outptr);

/************************ǰ������******************************/
/************************ģ�����******************************/

/* @<MOD_EXTERN */
/* ����PADSIZE��16�����ո���ַ����� */
static unsigned char blanks[PADSIZE] =
{
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20
};

/* ����PADSIZE��16����'0'���ַ�����*/
static unsigned char zeroes[PADSIZE] =
{
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30
};

/* ����������ʽ��ΪASCII�ַ����ĺ����ĺ���ָ��*/
static FUNCPTR fioFltFormatRtn = fformat;

/* @MOD_EXTERN> */

/************************ȫ�ֱ���******************************/

/* @<MOD_VAR */
/* ����һ�����ľ�ȷ�����Ƿ�Ӧ�������� */
BOOL signofnum = TRUE;

/* @MOD_VAR> */

/************************ʵ    ��******************************/

/* @MODULE> */


/**
 * @req
 * @brief: 
 *    ����ѡ������ָ���ĸ�ʽת���������ָ���Ļ�������������������ַ�����(����
 *    �������ַ�)��<br>
 * @param[in]: fmt: ��ʽ���ַ���
 * @param[in]: ...: �ɱ�����б�
 * @param[out]: buffer: ��д��Ļ�����ָ��
 * @return: 
 *    ������ַ�������
 *    ERROR���������������ַ�ʧ�ܡ�
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes:
 *    �����в�����ʽ���ַ�������д��Ļ�����ָ��Ϳɱ�����б����Ч�ԡ�<br>
 *    �ַ���<fmt>������ͨ�ַ������ᱻ��ʽת����ֱ��������ַ����͸�ʽת��������
 *    ����������<fmt>����Ĳ�����ת��������ĸ�ʽ����<br>
 * @implements: DR.1.1
 */
int sprintf(char *buffer, const char *fmt, ...)
{
    va_list vaList;
    int nChars = 0;

    /* @KEEP_COMMENT: ��ʼ�ɱ������ȡ */
    va_start(vaList, fmt);

    /*
     * @brief:
     *    ���<fmt>�ĸ�ʽת��������������в���:<br>
     *    (1)���<fmt>Ϊ�մ�("")������<buffer>��д��'\0'��Ϊ������������0��<br>
     *    (2)����ͨ�ַ�����˳�򴫵ݸ�<buffer>��<br>
     *    (3)��ת���ַ�(��'\'��ͷ���ַ�)����˳��ת��Ϊָ��������ַ������ݸ�<buffer>��<br>
     *    (4)����ʽ���ƴ�(\n : �س���\r : ���У�\t : Tab�ո�)ת���ַ����������Ǵ��ݸ�<br>
     *       <buffer>��<br>
     *    (5)����ٷֺź����һ��������ĸ�ʽ�ַ�����ֱ�ӽ����ַ����ݸ�<buffer>��<br>
     *    (6)����ٷֺź�����ſ��Ա�����Ϊ�Ϸ���־(�ο�ע���е�flags����)����ȡ�<br>
     *       ��ȷ�ȡ����ߴ�С�ĸ�ʽ�����ַ������ǵ���Щ�ַ���û�и���Ϸ��������ַ�<br>
     *       (ע���е��κ�һ��)ʱ���������ʹ����'0'��ʽ��־����<width>-1���ո�<br>
     *       ��������ַ����ݸ�<buffer>����δʹ��'0'��ʽ��־������<buffer>����<br>
     *       <width>-1��0�ַ���������ַ���<br>
     *    (7)������<...>�и�������һ����������Ϊ����ʱ����"Inf"���ݸ�<buffer>��<br>
     *       Ϊ����ʱ����"-Inf"���ݸ�<buffer>��<br>
     *    (8)������<...>�и���������һ�����֣���"NaN"���ݸ�<buffer>��<br>
     *    (9)<fmt>�ĸ�ʽת����������<buffer>��д��'\0'��Ϊ������������<buffer><br>
     *       ���ַ�������<br>
     * @qualification method: ����
     * @notes:
     *    NaN��Not a Number����ʾ�����ֶ���<br>
     *    INF��Infinity���������<br>
     *    -INF�����������<br>
     *    +INF�����������<br>
     *<br>
     *    <fmt>�е��Ӵ���ʽΪ:<br>
     *        "[xxx][%[flags][width][.precision][ 'l' ]type][xxx]"<br>
     *        [xxx]Ϊ�����ַ�����<br>
     *        []������Ϊ��ѡ��<br>
     *<br> 
     *    ��ʽ˵��ͨ���԰ٷֺſ�ʼ���������ҽ��б���ȡ������԰ٷֺź���ĸ�ʽ
     *    ���Ž��н�������ʽ����һЩ��ѡ�ͱ�����򣬾������������ʽ��<br>
     *        %[flags][width][.precision][ 'l' ]type<br>
     *<br>
     *    (1) flag����ѡ����һ�����ڵ�������ʹ�ӡ����λ���ո�С���㡢�˽��ƺ�<br>
     *        ʮ������ǰ׺���ַ������ܻ���һ����һ�����ϵ�flag�����ڸ�ʽ˵���С�<br>
     *        Flag˵����<br>
     *         - ����widthָ���Ŀ�������ת�������Ľ���ַ���<br>
     *         + ������ֵ���з��ŵ�����ǰ�����+��-ǰ׺<br>
     *         ' '(�ո�) ������ֵ���з��ŵĲ��������ģ�����+һ��ʹ��ʱ���Ըñ�<br>
     *             ־��<br>
     *         * ��������Ǹ����Ϳɱ����ָ���Ŀ���Ҷ���ת�������Ľ���ַ�����<br>
     *           ǰ����ӿո�<br>
     *         # ����type����ָ�������ת�����ͣ������ת��Ϊ�ʵ�����ʽ��<br>
     *           0: һ������Ľ������һ��0ǰ׺<br>
     *           x: һ������Ľ������һ��0xǰ׺<br>
     *           X: һ������Ľ������һ��0Xǰ׺<br>
     *           e: ����ܰ���һ��ʮ����С���㣬��ʹС�����û������<br>
     *           E: ����ܰ���һ��ʮ����С���㣬��ʹС�����û������<br>
     *           f: ����ܰ���һ��ʮ����С���㣬��ʹС�����û������<br>
     *           g: ͬe��E,����β����㲻ȥ��<br>
     *           G: ͬe��E,����β����㲻ȥ��<br>
     *         0 �����width����ָ���������widthǰ����0��־���������ת�������ַ�<br>
     *           ��d��i��o��u��x��X��e��E��f��g������G�����ÿո���������������0��<br>
     *           ���0��־�ͼ��ű�־ͬʱ���֣����־�����ԡ���d��i��o��u��x����Xת����<br>
     *           ���ָ����һ������precision�����־�����ԡ�ע��0������Ϊһ����־��<br>
     *           ����һ�����Ŀ�ʼ��<br>
     *    (2) width����ѡ����һ���Ǹ���ʮ������������˵����С������ַ���������<br>
     *        ������ַ�����С��width����������ַ�����߻��ұ����0�����<br>
     *        width��0ǰ׺�������ո�<br>
     *    (3) precision����ѡ����һ���Ǹ���ʮ���������������ھ��(.)������ʾ��<br>
     *        �����ַ��������ı���ӡ������ַ�������������������С�ı���ӡ�����<br>
     *        ���ָ��������ڸ����������������ֵĸ�����precisionΪ'*'ʱ����ʾ������<br>
     *        �����Ϳɱ������ʾ���ȡ�<br>
     *    (4) size����ѡ��˵��һ������Ĳ�����Ϊ�����Σ�ȱʡΪ���ͣ��ĳ��ȡ�����<br>
     *        'e'��'f'û���κ����塣<br>
     *    (5) type�����裩������صĲ������������ͱ����͡�<br>
     *        h ��ʹ���ں����d��I��o��u��x����X���͵����ת��������һ����������<br>
     *          ���޷��Ŷ�������<br>
     *        l %l��ʹ�����d��i��o��u��x����Xת������������һ�������������޷��ų�<br>
     *          ������%ll��ʹ�����d��i��o��u��x����Xת������������һ��������������<br>
     *          �޷��ų���������<br>
     *        c �ַ�<br>
     *        n ͳ�Ƹ�ʽ�����ַ�����%n֮ǰprintf��ӡ���������ַ��ĸ������ɱ����<br>
     *          �б�����%n��Ӧ�Ŀɱ����ӦΪһ��int��ָ���������������ͳ�Ƶ�����<br>
     *          ��������%ln��Ӧlong��ָ�������%lln��Ӧlong long��ָ�������%hn��<br>
     *          Ӧshort��ָ�������<br>
     *        p ��ʮ�����Ƶĵ�ַ��ʽ(����FFFFFFFF)����ɱ����ֵ��<br>
     *        D �����ŵ�ʮ��������<br>
     *        d �����ŵ�ʮ��������<br>
     *        s �ַ�����ֱ���ﵽָ�����Ȼ�������һ���ַ������������ɱ����ΪNULL<br>
     *          ���ӡ(null)<br>
     *        U �޷���ʮ��������<br>
     *        u �޷���ʮ��������<br>
     *        X �޷���ʮ����������(��ĸ���ִ�дABCDEF)<br>
     *        x �޷���ʮ����������(��ĸ����Сдabcdef)<br>
     *        O �˽�������<br>
     *        o �˽�������<br>
     *        e ��[-]9.9999e[+|-]999����ʽ���һ�������ŵ�ֵ<br>
     *        E ��[-]9.9999E[+|-]999����ʽ���һ�������ŵ�ֵ<br>
     *        g ��f����e����ʽ���һ�������ŵ�ֵ�����ݸ�����ֵ�;��ȣ�<br>
     *          ������Ҫʱ�������������ʮ����С����<br>
     *        G ͬg�ķ�ʽ��������������Ҫһ��ָ��ʱ��E����ʾ<br>
     *        f ��ʽΪ[]D.D��D - һ������ʮ���������ĸ�����<br>
     * @derived requirement: ��
     * @implements: 1 
     */
    /*
     * @KEEP_COMMENT: ����fioformatstr(DR.1.13)���ɱ��������<fmt>���и�ʽ������
     * ����<buffer>�������ַ�����������nChars
     */
    nChars = fioformatstr((const signed char *)fmt, vaList, (int)&buffer);

    /* @KEEP_COMMENT: �����ɱ������ȡ������<buffer>��ǰ����Ϊ'\0' */
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
 *    �������б������ĸ�ʽת���������ָ�����豸�����ر�����ַ�����Ŀ��
 * @param[in]: fmt: ��ʽ�����ַ���
 * @param[in]: vaList: �ɱ�����б�
 * @param[in]: outarg: putbuf�Ĳ���
 * @return: 
 *    ERROR(-1): ����ַ�ʧ�ܡ�
 *    ����ַ�����Ŀ��
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

    /* @KEEP_COMMENT: ����һ�鳤��Ϊ400���ֽڵ��ڴ滺����buf */

    signed char *  xdigs = NULL;

    /* @KEEP_COMMENT: �����ʾ����ֵ�ľֲ�����ret������ʼ��Ϊ0 */
    int ret = 0;

    /* @KEEP_COMMENT: while(1) */
    FOREVER
    {
        /* @KEEP_COMMENT: ���ַ������׵�ַ��cp����ȡ��ǰ�ַ���ch */
        cp = CHAR_FROM_CONST(fmt);
        ch = (*fmt);

        /* @REPLACE_BRACKET: ch������'\0' && ch������'%' */
        while ((ch != (signed char)EOS) && (ch != (signed char)'%'))
        {
            /* @KEEP_COMMENT: <fmt>++������chΪ<*fmt> */
            fmt++;
            ch = *fmt;
        }

        /* @KEEP_COMMENT: ����tmpChrAddΪ<fmt>������nΪtmpChrAdd-cp */
        tmpChrAdd = fmt;
        n = tmpChrAdd - cp;

        /* @REPLACE_BRACKET: n������0 */
        if (n != 0)
        {
            /* @KEEP_COMMENT: ��cp����n���ֽڵ�<outarg> */
            putbuf(cp, n, (signed char **)outarg);

            /* @KEEP_COMMENT: ����retΪret+n */
            ret += n;
        }

        /* @REPLACE_BRACKET: ��ǰ�ַ�chΪ'\0' */
        if (ch == (signed char)EOS)
        {
            /* @REPLACE_BRACKET: ret */
            return(ret);
        }

        /* @KEEP_COMMENT: <fmt>++������*FMTΪ'\0'������CollectΪFMT */
        fmt++;
        *FMT        = EOS;
        Collect     = FMT;

        /* @KEEP_COMMENT: ���ó����ͱ�־doLongIntΪFALSE */
        doLongInt   = FALSE;

        /* @KEEP_COMMENT: ����64λ���ͱ�־doLongLongIntΪFALSE */
        doLongLongInt = FALSE;

        /* @KEEP_COMMENT: ���ö����ͱ�־doShortIntΪFALSE */
        doShortInt  = FALSE;

        /* @KEEP_COMMENT: ����ת����ʽ��־doAltΪFALSE */
        doAlt       = FALSE;

        /* @KEEP_COMMENT: ����������־doLAdjustΪFALSE */
        doLAdjust   = FALSE;

        /* @KEEP_COMMENT: ����������־doZeroPadΪFALSE */
        doZeroPad   = FALSE;

        /* @KEEP_COMMENT: ����ʮ������ǰ׺��־doHexPrefixΪFALSE */
        doHexPrefix = FALSE;

        /* @KEEP_COMMENT: ���÷��ű�־doSignΪFALSE */
        doSign      = FALSE;

        /* @KEEP_COMMENT: ���þ��ȱ�־dprecΪ0 */
        dprec       = 0;

        /* @KEEP_COMMENT: ���ø��㾫�ȱ�־fpprecΪ0 */
        fpprec      = 0;

        /* @KEEP_COMMENT: ���ÿ��widthΪ0 */
        width       = 0;

        /* @KEEP_COMMENT: ���þ���precΪ-1 */
        prec        = -1;

        /* @KEEP_COMMENT: ����ǰ����oldprecΪ-1 */
        oldprec     = -1;

        /* @KEEP_COMMENT: ���÷���signΪ'\0' */
        sign        = EOS;

        /* @KEEP_COMMENT: #define get_CHAR  ��ȡ��ǰ�ַ���ch������<fmt>ָ���¸��ַ� */
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
                    /* @REPLACE_BRACKET: sign����0 */
                    if (0 == sign)
                    {
                        /* @KEEP_COMMENT: ����signΪ�ո� */
                        sign = ' ';
                    }

                    /* @KEEP_COMMENT: get_CHAR */
                    get_CHAR;
                    break;

                case '#':
                    /* @KEEP_COMMENT: ����doAltΪTRUE��get_CHAR */
                    doAlt = TRUE;
                    get_CHAR;
                    break;

                case '*':
                    /* @KEEP_COMMENT: ��ȡ�ɱ��������һ��int�͵Ĳ���������width */
                    width = va_arg(vaList, int);

                    /* @REPLACE_BRACKET: width���ڵ���0 */
                    if (width >= 0)
                    {
                        /* @KEEP_COMMENT: get_CHAR */
                        get_CHAR;
                        break;  
                    }

                    /* @KEEP_COMMENT: ����widthΪ-width������doLAdjustΪTRUE */
                    width = -width;
                    doLAdjust = TRUE;

                    /* @KEEP_COMMENT: get_CHAR */
                    get_CHAR;
                    break;

                case '-':
                    /* @KEEP_COMMENT: ����doLAdjustΪTRUE��get_CHAR */
                    doLAdjust = TRUE;
                    get_CHAR;
                    break;

                case '+':
                    /* @KEEP_COMMENT: ����signΪ'+'��get_CHAR */
                    sign = '+';
                    get_CHAR;
                    break;

                case '.':
                    /* @KEEP_COMMENT: get_CHAR */
                    get_CHAR;

                    /* @REPLACE_BRACKET: chΪ'*' */
                    if (ch == (signed char)'*')
                    {
                        /* @KEEP_COMMENT: ��ȡint�Ϳɱ������n */
                        n = va_arg(vaList, int);

                        /* @KEEP_COMMENT: ���nС��0������precΪ-1����������precΪn */
                        prec = (n < 0) ? -1 : n;

                        /* @KEEP_COMMENT: get_CHAR */
                        get_CHAR;
                        break;
                    }

                    /* @KEEP_COMMENT: ����nΪ0 */
                    n = 0;

                    /* @REPLACE_BRACKET: chΪ0~9��Χ�ڵ����� */
                    while (is_digit(ch))
                    {
                        /* @KEEP_COMMENT: ����nΪ10*n+(chת���ɵ�����)��get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;
                    }

                    /* @KEEP_COMMENT: ���nС��0������precΪ-1����������precΪn */
                    prec = n < 0 ? -1 : n;
                    break;

                case '0':
                    /* @KEEP_COMMENT: ����doZeroPadΪTRUE��get_CHAR */
                    doZeroPad = TRUE;
                    get_CHAR;
                    break;

                case '1':
                    /* @KEEP_COMMENT: ����nΪ0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: ����nΪ10*n+chת���ɵ����֣�get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: chΪ���� */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: ����widthΪn */
                    width = n;
                    break;

                case '2':
                    /* @KEEP_COMMENT: ����nΪ0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: ����nΪ10*n+chת���ɵ����֣�get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: chΪ���� */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: ���ÿ��widthΪn */
                    width = n;
                    break;

                case '3':
                    /* @KEEP_COMMENT: ����nΪ0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: ����nΪ10*n+chת���ɵ����֣�get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: chΪ���� */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: ���ÿ��widthΪn */
                    width = n;
                    break;

                case '4':
                    /* @KEEP_COMMENT: ����nΪ0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: ����nΪ10*n+chת���ɵ����֣�get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: chΪ���� */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: ���ÿ��widthΪn */
                    width = n;
                    break;

                case '5':
                    /* @KEEP_COMMENT: ����nΪ0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: ����nΪ10*n+chת���ɵ����֣�get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: chΪ���� */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: ���ÿ��widthΪn */
                    width = n;
                    break;

                case '6':
                    /* @KEEP_COMMENT: ����nΪ0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: ����nΪ10*n+chת���ɵ����֣�get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: chΪ���� */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: ���ÿ��widthΪn */
                    width = n;
                    break;

                case '7':
                    /* @KEEP_COMMENT: ����nΪ0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: ����nΪ10*n+chת���ɵ����֣�get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: chΪ���� */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: ���ÿ��widthΪn */
                    width = n;
                    break;

                case '8':
                    /* @KEEP_COMMENT: ����nΪ0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: ����nΪ10*n+chת���ɵ����֣�get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: chΪ���� */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: ���ÿ��widthΪn */
                    width = n;
                    break;

                case '9':
                    /* @KEEP_COMMENT: ����nΪ0 */
                    n = 0;

                    do
                    {
                        /* @KEEP_COMMENT: ����nΪ10*n+chת���ɵ����֣�get_CHAR */
                        n = 10*n+to_digit(ch);
                        get_CHAR;

                        /* @REPLACE_BRACKET: chΪ���� */
                    } while (is_digit(ch));

                    /* @KEEP_COMMENT: ���ÿ��widthΪn */
                    width = n;
                    break;

                case 'h':
                    /* @KEEP_COMMENT: ����doShortIntΪTRUE��get_CHAR */
                    doShortInt = TRUE;
                    get_CHAR;
                    break;

                case 'l':
                    /* @KEEP_COMMENT: ��fmt��ȡ��һ���ַ�ch */
                    get_CHAR;

                    /* @REPLACE_BRACKET: ch����'l' */
                    if (ch == (signed char)'l')
                    {
                        /* @KEEP_COMMENT: ����64λ���ͱ�־doLongLongIntΪTRUE */
                        doLongLongInt = TRUE;

                        /* @KEEP_COMMENT: get_CHAR */
                        get_CHAR;
                        break;
                    }
                    else
                    {
                        /* @KEEP_COMMENT: ���ó����ͱ�־doLongIntΪTRUE */
                        doLongInt = TRUE;
                        break;
                    }

                case 'c':
                    /*
                     * @KEEP_COMMENT: ����cpΪ������buf���׵�ַ����ȡint�Ϳɱ��
                     * ����*cp
                     */
                    cp = buf;
                    *cp = va_arg(vaList, int);

                    /* @KEEP_COMMENT: ����sizeΪ1������signΪ'\0' */
                    size = 1;
                    sign = EOS;

                    goto outwhile;
                    break;

                case 'D':
                    /* @KEEP_COMMENT: ���ó����ͱ�־doLongIntΪTRUE */
                    doLongInt = TRUE;

                    /*
                     * @KEEP_COMMENT: ���doLongLongIntΪ�棬��ȡlong long
                     * �Ϳɱ������longLongVal��
                     * ���doLongIntΪ�棬��ȡlong�Ϳɱ������longLongVal��
                     * ���doShortIntΪ�棬��ȡint�Ϳɱ������longLongVal��
                     * �����ȡunsigned int�Ϳɱ������longLongVal
                     */
                    longLongVal = SARG();

                    /* @REPLACE_BRACKET: longLongValС��0 */
                    if (longLongVal < 0)
                    {
                        /* @KEEP_COMMENT: ����longLongValΪ-longLongVal */
                        longLongVal = -longLongVal;

                        /* @KEEP_COMMENT: ����signΪ'-' */
                        sign = '-';
                    }

                    /* @KEEP_COMMENT: ���þ��ȱ�־dprecΪ����prec */
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec���ڵ���0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: ����������־doZeroPadΪFALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: cp = buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: longLongVal������0 || prec������0 */
                    if ((longLongVal != 0) || (prec != 0))
                    {
                        /* @REPLACE_BRACKET: longLongVal���ڵ���10 */
                        while (longLongVal >= 10)
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: longLongVal����10������ת����ASCII
                             * �ַ�������*cp
                             */
                            *cp = to_char(longLongVal % 10);

                            /* @KEEP_COMMENT: ����longLongValΪlongLongVal/10 */
                            longLongVal /= 10;
                        }

                        /*
                         * @KEEP_COMMENT: cp--����longLongValת����ASCII�ַ�����
                         * ��*cp
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
                     * @KEEP_COMMENT: ���doLongLongIntΪ�棬��ȡlong long
                     * �Ϳɱ������longLongVal��
                     * ���doLongIntΪ�棬��ȡlong�Ϳɱ������longLongVal��
                     * ���doShortIntΪ�棬��ȡint�Ϳɱ������longLongVal��
                     * �����ȡunsigned int�Ϳɱ������longLongVal
                     */
                    longLongVal = SARG();

                    /* @REPLACE_BRACKET: longLongValС��0 */
                    if (longLongVal < 0)
                    {
                        /* @KEEP_COMMENT: ����longLongValΪ-longLongVal */
                        longLongVal = -longLongVal;

                        /* @KEEP_COMMENT: ���÷���signΪ'-' */
                        sign = '-';
                    }

                    /* @KEEP_COMMENT: ���þ��ȱ�־dprecΪ����prec */
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec���ڵ���0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: ����������־doZeroPadΪFALSE */
                        doZeroPad = FALSE;
                    }
                    /* @KEEP_COMMENT: cp = buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: longLongVal������0 || prec������0 */
                    if ((longLongVal != 0) || (prec != 0))
                    {
                        /* @REPLACE_BRACKET: longLongVal���ڵ���10 */
                        while (longLongVal >= 10)
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: ��longLongVal����10������ת����ASCII
                             * �ַ�������*cp
                             */
                            *cp = to_char(longLongVal % 10);

                            /* @KEEP_COMMENT: ����longLongValΪlongLongVal/10 */
                            longLongVal /= 10;
                        }

                        /* @KEEP_COMMENT: cp--����longLongValת����ASCII�ַ�������*cp */
                        cp--;
                        *cp = to_char(longLongVal);
                    }

                    /* @KEEP_COMMENT: ����sizeΪbuf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'i':
                    /*
                     * @KEEP_COMMENT: ���doLongLongIntΪ�棬��ȡlong long
                     * �Ϳɱ������longLongVal��
                     * ���doLongIntΪ�棬��ȡlong�Ϳɱ������longLongVal��
                     * ���doShortIntΪ�棬��ȡint�Ϳɱ������longLongVal��
                     * �����ȡunsigned int�Ϳɱ������longLongVal
                     */
                    longLongVal = SARG();

                    /* @REPLACE_BRACKET: longLongValС��0 */
                    if (longLongVal < 0)
                    {
                        /* @KEEP_COMMENT: ����longLongValΪ-longLongVal */
                        longLongVal = -longLongVal;

                        /* @KEEP_COMMENT: ���÷���signΪ'-' */
                        sign = '-';
                    }

                    /* @KEEP_COMMENT: ���þ��ȱ�־dprecΪ����prec */
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec���ڵ���0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: ����������־doZeroPadΪFALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: cp = buf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: longLongVal������0 || prec������0 */
                    if ((longLongVal != 0) || (prec != 0))
                    {
                        /* @REPLACE_BRACKET: longLongVal���ڵ���10 */
                        while (longLongVal >= 10)
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: ��longLongVal����10������ת����
                             * ASCII�ַ�������*cp
                             */
                            *cp = to_char(longLongVal % 10);

                            /* @KEEP_COMMENT: ����longLongValΪlongLongVal/10 */
                            longLongVal /= 10;
                        }

                        /*
                         * @KEEP_COMMENT: cp--����longLongValת����ASCII�ַ�����
                         * ��*cp
                         */
                        cp--;
                        *cp = to_char(longLongVal);
                    }

                    /* @KEEP_COMMENT: size = buf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'n':
                    /* @REPLACE_BRACKET: doLongLongInt������0 */
                    if (0 != doLongLongInt)
                    {
                        /*
                         * @REPLACE_LINE: ��ȡһ��long long�͵�ָ�룬������ָ��ָ
                         * ���������Ϊret
                         */
                        *va_arg(vaList, long long *) = (long long)ret;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doLongInt������0 */
                        if (0 != doLongInt)
                        {
                            /*
                             * @KEEP_COMMENT: ��ȡһ��long�͵�ָ�룬������ָ��ָ
                             * ���������Ϊret
                             */
                            *va_arg(vaList, long *) = ret;
                        }
                        else
                        {
                            /* @REPLACE_BRACKET: doShortInt������0 */
                            if (0 != doShortInt)
                            {
                                /*
                                 * @KEEP_COMMENT: ��ȡһ��short�͵�ָ�룬������ָ
                                 * ��ָ���������Ϊret
                                 */
                                *va_arg(vaList, short *) = ret;
                            }
                            else
                            {
                                /*
                                 * @KEEP_COMMENT: ��ȡһ��int�͵�ָ�룬������ָ��
                                 * ָ���������Ϊret
                                 */
                                *va_arg(vaList, int *) = ret;
                            } 
                        }
                    }

                    /* @KEEP_COMMENT: ���ַ������׵�ַ��cp����ȡ��ǰ�ַ���ch */
                    cp = CHAR_FROM_CONST(fmt);
                    ch = (*fmt);

                    /* @REPLACE_BRACKET: ch������'\0' && ch������'%' */
                    while ((ch != (signed char)EOS) && (ch != (signed char)'%'))
                    {
                        /* @KEEP_COMMENT: <fmt>++������chΪ<*fmt> */
                        fmt++;
                        ch = (*fmt);
                    }

                    /* @KEEP_COMMENT: ����tmpChrAddΪ<fmt>������nΪtmpChrAdd-cp */
                    tmpChrAdd = fmt;
                    n = tmpChrAdd-cp;

                    /* @REPLACE_BRACKET: n������0 */
                    if (n != 0)
                    {
                        /* @KEEP_COMMENT: ����putbuf(DR.1.14)��cp����n���ȵ��ֽڵ�<outarg> */
                        putbuf(cp, n, (signed char **)outarg);

                        /* @KEEP_COMMENT: ����retΪret+n */
                        ret += n;
                    }

                    /* @REPLACE_BRACKET: ch����'\0' */
                    if (ch == (signed char)EOS)
                    {
                        /* @REPLACE_BRACKET: ret */
                        return(ret);
                    }

                    /* @KEEP_COMMENT: <fmt>++������*FMTΪ'\0'������CollectΪFMT */
                    fmt++;
                    *FMT        = EOS;
                    Collect     = FMT;

                    /* @KEEP_COMMENT: ���ó����ͱ�־doLongIntΪFALSE */
                    doLongInt   = FALSE;

                    /* @KEEP_COMMENT: ����64λ���ͱ�־doLongLongIntΪFALSE */
                    doLongLongInt = FALSE;

                    /* @KEEP_COMMENT: ���ö����ͱ�־doShortIntΪFALSE */
                    doShortInt  = FALSE;

                    /* @KEEP_COMMENT: ����ת����ʽ��־doAltΪFALSE */
                    doAlt       = FALSE;

                    /* @KEEP_COMMENT: ����������־doLAdjustΪFALSE */
                    doLAdjust   = FALSE;

                    /* @KEEP_COMMENT: ����������־doZeroPadΪFALSE */
                    doZeroPad   = FALSE;

                    /* @KEEP_COMMENT: ����ʮ������ǰ׺��־doHexPrefixΪFALSE */
                    doHexPrefix = FALSE;

                    /* @KEEP_COMMENT: ���÷��ű�־����doSignΪFALSE */
                    doSign      = FALSE;

                    /* @KEEP_COMMENT: ���þ��ȱ�־dprecΪ0 */
                    dprec       = 0;

                    /* @KEEP_COMMENT: ���ø��㾫�ȱ�־fpprecΪ0 */
                    fpprec      = 0;

                    /* @KEEP_COMMENT: ���ÿ��widthΪ0 */
                    width       = 0;

                    /* @KEEP_COMMENT: ���þ���precΪ-1 */
                    prec        = -1;

                    /* @KEEP_COMMENT: ����ǰ����oldprecΪ-1 */
                    oldprec     = -1;

                    /* @KEEP_COMMENT: ���÷���signΪ'\0' */
                    sign        = EOS;

                    /* @KEEP_COMMENT: get_CHAR */
                    get_CHAR;
                    break;

                case 'O':
                    /* @KEEP_COMMENT: ����doLongIntΪTRUE */
                    doLongInt = TRUE;

                    /*
                     * @KEEP_COMMENT: ���doLongLongIntΪ�棬��ȡunsigned long long
                     * �Ϳɱ������ulongLongVal��
                     * ���doLongIntΪ�棬��ȡunsigned long�Ϳɱ������
                     * ulongLongVal��
                     * ���doShortIntΪ�棬��ȡint�Ϳɱ������ulongLongVal��
                     * �����ȡunsigned int�Ϳɱ������ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @KEEP_COMMENT: ����signΪ'\0'������dprecΪprec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec���ڵ���0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal������0 || prec������0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        do
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: ��ulongLongVal&7ת����ASCII�ַ�����
                             * ��*cp
                             */
                            *cp = to_char(ulongLongVal & 7);

                            /* @KEEP_COMMENT: ����ulongLongValΪulongLongVal>>3 */
                            ulongLongVal >>= 3;

                            /* @REPLACE_BRACKET: ulongLongVal������0 */
                        } while (0 != ulongLongVal);

                        /* @REPLACE_BRACKET: doAlt������0 && *cp������'0' */
                        if ((0 != doAlt) && (*cp != (signed char)'0'))
                        {
                            /* @KEEP_COMMENT: cp--������*cpΪ'0' */
                            cp--;
                            *cp = '0';
                        }
                    }

                    /* @KEEP_COMMENT: ����sizeΪbuf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'o':
                    /*
                     * @KEEP_COMMENT: ���doLongLongIntΪ�棬��ȡunsigned long long
                     * �Ϳɱ������ulongLongVal��
                     * ���doLongIntΪ�棬��ȡunsigned long�Ϳɱ������
                     * ulongLongVal��
                     * ���doShortIntΪ�棬��ȡint�Ϳɱ������ulongLongVal��
                     * �����ȡunsigned int�Ϳɱ������ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @KEEP_COMMENT: ����signΪ'\0'������dprecΪprec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec���ڵ���0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal������0 || prec������0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        do
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: ��(ulongLongVal & 7)ת����ASCII�ַ�
                             * ������*cp
                             */
                            *cp = to_char(ulongLongVal & 7);

                            /* @KEEP_COMMENT: ����ulongLongValΪulongLongVal>>3 */
                            ulongLongVal >>= 3;

                            /* @REPLACE_BRACKET: ulongLongVal������0 */
                        } while (0 != ulongLongVal);

                        /* @REPLACE_BRACKET: doAlt������0 && *cp������'0' */
                        if ((0 != doAlt) && (*cp != (signed char)'0'))
                        {
                            /* @KEEP_COMMENT: cp--������*cpΪ'0' */
                            cp--;
                            *cp = '0';
                        }
                    }

                    /* @KEEP_COMMENT: ����sizeΪbuf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'p':
                    /*
                     * @KEEP_COMMENT: �ӿɱ�����б��ȡһ��ָ�룬������ֵת����
                     * unsigned long long���ͺ󱣴���ulonglongVal
                     */
                    ulongLongVal = (unsigned long long) (unsigned int) va_arg(vaList, void *);

                    /* @KEEP_COMMENT: ���þֲ��ַ�ָ�����xdigsΪ"0123456789abcdef" */
                    xdigs   = "0123456789abcdef";

                    /* @KEEP_COMMENT: ����doHexPrefixΪTRUE������chΪ'x' */
                    doHexPrefix = TRUE;
                    ch      = 'x';

                    /* @KEEP_COMMENT: ����signΪ'\0'������dprecΪprec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec���ڵ���0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal������0 || prec������0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        do
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /* @KEEP_COMMENT: ����*cpΪ*(xdigs+(ulongLongVal&15)) */
                            *cp = (*(xdigs+(ulongLongVal & 15)));

                            /* @KEEP_COMMENT: ����ulongLongValΪulongLongVal>>4 */
                            ulongLongVal >>= 4;

                            /* @REPLACE_BRACKET: ulongLongVal������0 */
                        } while (0 != ulongLongVal);
                    }

                    /* @KEEP_COMMENT: ����sizeΪbuf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 's':
                    /* @KEEP_COMMENT: �ӿɱ�����б��ȡһ��char*���͵�ֵ������cp */
                    cp = va_arg(vaList, char *);

                    /* @REPLACE_BRACKET: NULL == cp */
                    if (NULL == cp)
                    {
                        /* @KEEP_COMMENT: ����cpΪ"(null)" */
                        cp = "(null)";
                    }

                    /* @REPLACE_BRACKET: prec���ڵ���0 */
                    if (prec >= 0)
                    {
                        /*
                         * @KEEP_COMMENT: ��cpָ����ַ�����ǰprec���ַ��в���'\0'
                         * �ַ�����ָ����ַ���ָ���Ϊp
                         */
                        signed char *p = (signed char *)memchr(cp, 0, prec);

                        /* @REPLACE_BRACKET: p != NULL */
                        if (p != NULL)
                        {
                            /* @KEEP_COMMENT: ����sizeΪ����(p-cp) */
                            size = (int)(p-cp);
                        }
                        else
                        {
                            /* @KEEP_COMMENT: ����sizeΪprec */
                            size = prec;
                        }
                    }
                    else
                    {
                        /* @KEEP_COMMENT: ��ȡ�ַ���cp���ȱ�����size */
                        size = (int)strlen(cp);
                    }

                    /* @KEEP_COMMENT: ����signΪ'\0' */
                    sign = EOS;
                    goto outwhile;
                    break;

                case 'U':
                    /* @KEEP_COMMENT: ����doLongIntΪTRUE */
                    doLongInt = TRUE;

                    /*
                     * @KEEP_COMMENT: ���doLongLongIntΪ�棬��ȡunsigned long long
                     * �Ϳɱ������ulongLongVal��
                     * ���doLongIntΪ�棬��ȡunsigned long�Ϳɱ������
                     * ulongLongVal��
                     * ���doShortIntΪ�棬��ȡint�Ϳɱ������ulongLongVal��
                     * �����ȡunsigned int�Ϳɱ������ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @KEEP_COMMENT: ����signΪ'\0'������dprecΪprec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec���ڵ���0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal������0 || prec������0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        /* @REPLACE_BRACKET: ulongLongVal���ڵ���10 */
                        while (ulongLongVal >= 10)
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: ��ulongLongVal����10������ת����
                             * ASCII�ַ�������*cp
                             */
                            *cp = to_char(ulongLongVal % 10);

                            /* @KEEP_COMMENT: ����ulongLongValΪulongLongVal/10 */
                            ulongLongVal /= 10;
                        }

                        /*
                         * @KEEP_COMMENT: cp--������ulongLongValת����ASCII�ַ���
                         * ����*cp
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
                     * @KEEP_COMMENT: ���doLongLongIntΪ�棬��ȡunsigned long long
                     * �Ϳɱ������ulongLongVal��
                     * ���doLongIntΪ�棬��ȡunsigned long�Ϳɱ������
                     * ulongLongVal��
                     * ���doShortIntΪ�棬��ȡint�Ϳɱ������ulongLongVal��
                     * �����ȡunsigned int�Ϳɱ������ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @KEEP_COMMENT: ����signΪ'\0'������dprecΪprec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec���ڵ���0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal������0 || prec������0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        /* @REPLACE_BRACKET: ulongLongVal���ڵ���10 */
                        while (ulongLongVal >= 10)
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /*
                             * @KEEP_COMMENT: ��ulongLongVal����10������ת����
                             * ASCII�ַ�������*cp
                             */
                            *cp = to_char(ulongLongVal % 10);

                            /* @KEEP_COMMENT: ����ulongLongValΪulongLongVal/10 */
                            ulongLongVal /= 10;
                        }

                        /*
                         * @KEEP_COMMENT: cp--������ulongLongValת����ASCII�ַ���
                         * ����*cp
                         */
                        cp--;
                        *cp = to_char(ulongLongVal);
                    }

                    /* @KEEP_COMMENT: ����sizeΪbuf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'X':
                    /* @KEEP_COMMENT: ���þֲ��ַ�ָ�����xdigsΪ"0123456789ABCDEF" */
                    xdigs = "0123456789ABCDEF";

                    /*
                     * @KEEP_COMMENT: ���doLongLongIntΪ�棬��ȡunsigned long long
                     * �Ϳɱ������ulongLongVal��
                     * ���doLongIntΪ�棬��ȡunsigned long�Ϳɱ������
                     * ulongLongVal��
                     * ���doShortIntΪ�棬��ȡint�Ϳɱ������ulongLongVal��
                     * �����ȡunsigned int�Ϳɱ������ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @REPLACE_BRACKET: doAlt������0 && ulongLongVal������0 */
                    if ((0 != doAlt) && (ulongLongVal != 0))
                    {
                        /* @KEEP_COMMENT: ����doHexPrefixΪTRUE */
                        doHexPrefix = TRUE;
                    }

                    /* @KEEP_COMMENT: ����signΪ'\0'������dprecΪprec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec���ڵ���0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal������0 || prec������0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        do
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /* @KEEP_COMMENT: ����*cpΪ(*(xdigs+(ulongLongVal&15))) */
                            *cp = (*(xdigs+(ulongLongVal & 15)));

                            /* @KEEP_COMMENT: ����ulongLongValΪulongLongVal>>4 */
                            ulongLongVal >>= 4;

                            /* @REPLACE_BRACKET: ulongLongVal������0 */
                        } while (0 != ulongLongVal);
                    }

                    /* @KEEP_COMMENT: ����sizeΪbuf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;                  
                    break;

                case 'x':
                    /* @KEEP_COMMENT: ���þֲ��ַ�ָ�����xdigsΪ"0123456789abcdef" */
                    xdigs = "0123456789abcdef";

                    /*
                     * @KEEP_COMMENT: ���doLongLongIntΪ�棬��ȡunsigned long long
                     * �Ϳɱ������ulongLongVal��
                     * ���doLongIntΪ�棬��ȡunsigned long�Ϳɱ������
                     * ulongLongVal��
                     * ���doShortIntΪ�棬��ȡint�Ϳɱ������ulongLongVal��
                     * �����ȡunsigned int�Ϳɱ������ulongLongVal
                     */
                    ulongLongVal = UARG();

                    /* @REPLACE_BRACKET: doAlt������0 && ulongLongVal������0 */
                    if ((0 != doAlt) && (ulongLongVal != 0))
                    {
                        /* @KEEP_COMMENT: ����doHexPrefixΪTRUE */
                        doHexPrefix = TRUE;
                    }

                    /* @KEEP_COMMENT: ����signΪ'\0'������dprecΪprec */
                    sign = EOS;
                    dprec = prec;

                    /* @REPLACE_BRACKET: dprec���ڵ���0 */
                    if (dprec >= 0)
                    {
                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf+400 */
                    cp = buf + BUF;

                    /* @REPLACE_BRACKET: ulongLongVal������0 || prec������0 */
                    if ((ulongLongVal != 0) || (prec != 0))
                    {
                        do
                        {
                            /* @KEEP_COMMENT: cp-- */
                            cp--;

                            /* @KEEP_COMMENT: ����*cpΪ*(xdigs+(ulongLongVal&15))) */
                            *cp = (*(xdigs+(ulongLongVal & 15)));

                            /* @KEEP_COMMENT: ����ulongLongValΪulongLongVal>>4 */
                            ulongLongVal >>= 4;

                            /* @REPLACE_BRACKET: ulongLongVal������0 */
                        } while (0 != ulongLongVal);
                    }

                    /* @KEEP_COMMENT: ����sizeΪbuf+400-cp */
                    size = buf+BUF-cp;
                    goto outwhile;
                    break;

                case 'e':
                    /* @KEEP_COMMENT: ����oldprecΪprec */
                    oldprec = prec;

                    /* @REPLACE_BRACKET: prec����39 */
                    if (prec > MAXFRACT)
                    {
                        /* @REPLACE_BRACKET: (ch��Ϊ'g' && ch��Ϊ'G') || doAltΪTRUE */
                        if (((ch != (signed char)'g') && (ch != (signed char)'G')) || (TRUE == doAlt))
                        {
                            /* @KEEP_COMMENT: ����fpprecΪprec-39 */
                            fpprec = prec-MAXFRACT;
                        }

                        /* @KEEP_COMMENT: ����precΪ39 */
                        prec = MAXFRACT;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: prec����-1 */
                        if (-1 == prec)
                        {
                            /* @KEEP_COMMENT: ����precΪ6 */
                            prec = 6;
                        }
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf������*cpΪ'\0' */
                    cp  = buf;
                    *cp = EOS;

                    /*
                     * @KEEP_COMMENT: �������Ϳɱ����ת��Ϊ�ַ�����buf�������ַ�
                     * �����ȵ�size
                     */
                    size = (*fioFltFormatRtn)((va_list *)vaList, prec, doAlt, ch, &doSign,cp,buf+BUF);

                    /* @REPLACE_BRACKET: sizeС��0 */
                    if ((int)size < 0)
                    {
                        /* @KEEP_COMMENT: ����sizeΪ-size������precΪoldprec�� */
                        size = -size;
                        prec = oldprec;

                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;

                        /* @REPLACE_BRACKET: doSignΪTRUE */
                        if (TRUE == doSign)
                        {
                            /* @KEEP_COMMENT: ����signΪ'-' */
                            sign = '-';
                        }
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doSign������0 */
                        if (0 != doSign)
                        {
                            /* @KEEP_COMMENT: ����signΪ'-' */
                            sign = '-';
                        }

                        /* @REPLACE_BRACKET: *cp����'\0' */
                        if (EOS == *cp)
                        {
                            /* @KEEP_COMMENT: cp++ */
                            cp++;
                        }
                    }

                    goto outwhile;
                    break;

                case 'E':
                    /* @KEEP_COMMENT: ����oldprecΪprec */
                    oldprec = prec;

                    /* @REPLACE_BRACKET: prec����39 */
                    if (prec > MAXFRACT)
                    {
                        /* @KEEP_COMMENT: ����fpprecΪprec-39������precΪ39 */
                        fpprec = prec-MAXFRACT;
                        prec = MAXFRACT;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: prec����-1 */
                        if (-1 == prec)
                        {
                            /* @KEEP_COMMENT: ����precΪ6 */
                            prec = 6;
                        }
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf������*cpΪ'\0' */
                    cp  = buf;
                    *cp = EOS;

                    /*
                     * @KEEP_COMMENT: �������Ϳɱ����ת��Ϊ�ַ�����buf�������ַ�
                     * �����ȵ�size
                     */
                    size = (*fioFltFormatRtn)((va_list *)vaList, prec, doAlt, ch, &doSign,cp,buf+BUF);

                    /* @REPLACE_BRACKET: sizeС��0 */
                    if ((int)size < 0)
                    {
                        /* @KEEP_COMMENT: ����sizeΪ-size����precΪoldprec�� */
                        size = -size;
                        prec = oldprec;

                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;

                        /* @REPLACE_BRACKET: doSign����TRUE */
                        if (TRUE == doSign)
                        {
                            /* @KEEP_COMMENT: ����signΪ'-' */
                            sign = '-';
                        }
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doSign������0 */
                        if (0 != doSign)
                        {
                            /* @KEEP_COMMENT: ����signΪ'-' */
                            sign = '-';
                        }

                        /* @REPLACE_BRACKET: *cp����'\0' */
                        if (EOS == *cp)
                        {
                            /* @KEEP_COMMENT: cp++ */
                            cp++;
                        }
                    }

                    goto outwhile;
                    break;

                case 'f':
                    /* @KEEP_COMMENT: ����oldprecΪprec */
                    oldprec = prec;

                    /* @REPLACE_BRACKET: prec����39 */
                    if (prec > MAXFRACT)
                    {
                        /* @KEEP_COMMENT: ����fpprecΪprec-39������precΪ39 */
                        fpprec = prec-MAXFRACT;
                        prec = MAXFRACT;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: prec����-1 */
                        if (-1 == prec)
                        {
                            /* @KEEP_COMMENT: ����precΪ6 */
                            prec = 6;
                        }
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf������*cpΪ'\0' */
                    cp  = buf;
                    *cp = EOS;

                    /*
                     * @KEEP_COMMENT: �������Ϳɱ����ת��Ϊ�ַ�����buf�������ַ�
                     * �����ȵ�size
                     */
                    size = (*fioFltFormatRtn)((va_list *)vaList, prec, doAlt, ch, &doSign,cp,buf+BUF);

                    /* @REPLACE_BRACKET: sizeС��0 */
                    if ((int)size < 0)
                    {
                        /* @KEEP_COMMENT: ����sizeΪ-size������precΪoldprec�� */
                        size = -size;
                        prec = oldprec;

                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;

                        /* @REPLACE_BRACKET: doSign����TRUE */
                        if (TRUE == doSign)
                        {
                            /* @KEEP_COMMENT: ����signΪ'-' */
                            sign = '-';
                        }
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doSign������0 */
                        if (0 != doSign)
                        {
                            /* @KEEP_COMMENT: ����signΪ'-' */
                            sign = '-';
                        }

                        /* @REPLACE_BRACKET: *cp����'\0' */
                        if (EOS == *cp)
                        {
                            /* @KEEP_COMMENT: cp++ */
                            cp++;
                        }
                    }

                    goto outwhile;
                    break;

                case 'g':
                    /* @KEEP_COMMENT: ����oldprecΪprec */
                    oldprec = prec;

                    /* @REPLACE_BRACKET: prec����39 */
                    if (prec > MAXFRACT)
                    {
                        /* @REPLACE_BRACKET: doAltΪTRUE */
                        if (TRUE == doAlt)
                        {
                            /* @KEEP_COMMENT: ����fpprecΪprec-39 */
                            fpprec = prec-MAXFRACT;
                        }

                        /* @KEEP_COMMENT: ����precΪ39 */
                        prec = MAXFRACT;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: prec����-1 */
                        if (-1 == prec)
                        {
                            /* @KEEP_COMMENT: ����precΪ6 */
                            prec = 6;
                        }
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf������*cpΪ'\0' */
                    cp  = buf;
                    *cp = EOS;

                    /*
                     * @KEEP_COMMENT: �������Ϳɱ����ת��Ϊ�ַ�����buf�������ַ�
                     * �����ȵ�size
                     */
                    size = (*fioFltFormatRtn)((va_list *)vaList, prec, doAlt, ch, &doSign,cp,buf+BUF);

                    /* @REPLACE_BRACKET: sizeС��0 */
                    if ((int)size < 0)
                    {
                        /* @KEEP_COMMENT: ����sizeΪ-size������precΪoldprec�� */
                        size = -size;
                        prec = oldprec;

                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;

                        /* @REPLACE_BRACKET: doSign����TRUE */
                        if (TRUE == doSign)
                        {
                            /* @KEEP_COMMENT: ����signΪ'-' */
                            sign = '-';
                        }
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doSign������0 */
                        if (0 != doSign)
                        {
                            /* @KEEP_COMMENT: ����signΪ'-' */
                            sign = '-';
                        }

                        /* @REPLACE_BRACKET: *cp����'\0' */
                        if (EOS == *cp)
                        {
                            /* @KEEP_COMMENT: cp++ */
                            cp++;
                        }
                    }

                    goto outwhile;
                    break;

                case 'G':
                    /* @KEEP_COMMENT: ����oldprecΪprec */
                    oldprec = prec;

                    /* @REPLACE_BRACKET: prec����39 */
                    if (prec > MAXFRACT)
                    {
                        /* @REPLACE_BRACKET: doAltΪTRUE */
                        if (TRUE == doAlt)
                        {
                            /* @KEEP_COMMENT: ����fpprecΪprec-39 */
                            fpprec = prec-MAXFRACT;
                        }

                        /* @KEEP_COMMENT: ����precΪ39 */
                        prec = MAXFRACT;
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: prec����-1 */
                        if (-1 == prec)
                        {
                            /* @KEEP_COMMENT: ����precΪ6 */
                            prec = 6;
                        }
                    }

                    /* @KEEP_COMMENT: ����cpΪbuf������*cpΪ'\0' */
                    cp  = buf;
                    *cp = EOS;

                    /*
                     * @KEEP_COMMENT: �������Ϳɱ����ת��Ϊ�ַ�����buf�������ַ�
                     * �����ȵ�size
                     */
                    size = (*fioFltFormatRtn)((va_list *)vaList, prec, doAlt, ch, &doSign,cp,buf+BUF);

                    /* @REPLACE_BRACKET: sizeС��0 */
                    if ((int)size < 0)
                    {
                        /* @KEEP_COMMENT: ����sizeΪ-size������precΪoldprec�� */
                        size = -size;
                        prec = oldprec;

                        /* @KEEP_COMMENT: ����doZeroPadΪFALSE */
                        doZeroPad = FALSE;

                        /* @REPLACE_BRACKET: doSignΪTRUE */
                        if (TRUE == doSign)
                        {
                            /* @KEEP_COMMENT: ����signΪ'-' */
                            sign = '-';
                        }
                    }
                    else
                    {
                        /* @REPLACE_BRACKET: doSign������0 */
                        if (0 != doSign)
                        {
                            /* @KEEP_COMMENT: ����signΪ'-' */
                            sign = '-';
                        }

                        /* @REPLACE_BRACKET: *cp����'\0' */
                        if (EOS == *cp)
                        {
                            /* @KEEP_COMMENT: cp++ */
                            cp++;
                        }
                    }

                    goto outwhile;
                    break;

                default:
                    /* @REPLACE_BRACKET: ch����'\0' */
                    if (ch == (signed char)EOS)
                    {
                        /* @REPLACE_BRACKET: ret */
                        return(ret);
                    }

                    /*
                     * @KEEP_COMMENT: ����cpΪbuf������*cpΪch������sizeΪ1��
                     * ����signΪ'\0'
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
         * �������к���ĸ�ʽ�ַ����������ꡣ��ʱcpָ��
         * һ������Ϊwidth���ַ��������doZeroPad��־��ʱΪ1��
         * ����ַ����Ŀ�ͷӦ���Ƿ��Ż���ǰ׺��������ǰ׺
         * ֮ǰӦ���ǿո������䡣����Щ�������ǰ׺֮��
         * ����ʮ����[diouxX]�������0��Ȼ���ӡת���������ַ�����
         * �ڴ�֮����ݸ��㾫�ȵ���Ҫ���0��������doLAdjustΪ1
         * ���ں������ո�
         */
        /* @KEEP_COMMENT: outwhile */
        outwhile:
        /* @KEEP_COMMENT: ����fieldszΪsize+fpprec */
        fieldsz = size+fpprec;

        /* @REPLACE_BRACKET: sign������'\0' */
        if (sign != (signed char)EOS)
        {
            /* @KEEP_COMMENT: fieldsz++ */
            fieldsz++;

            /* @REPLACE_BRACKET: signofnum������0 */
            if (0 != signofnum)
            {
                /* @KEEP_COMMENT: dprec++ */
                dprec++;
            }
        }
        else
        {
            /* @REPLACE_BRACKET: doHexPrefix������0 */
            if (0 != doHexPrefix)
            {
                /* @KEEP_COMMENT: ����fieldszΪfieldsz+2 */
                fieldsz += 2;
            }
        }

        /*
         * @KEEP_COMMENT: ���dprec����fieldsz������realszΪdprec��
         * ��������realszΪfieldsz
         */
        realsz = (dprec > fieldsz) ? dprec : fieldsz;

        /* @REPLACE_BRACKET: doLAdjust����0 && doZeroPad����0 */
        if ((0 == doLAdjust) && (0 == doZeroPad))
        {
            /* @KEEP_COMMENT: ����nΪwidth-realsz */
            n = width-realsz;

            /* @REPLACE_BRACKET: n����0 */
            if (n > 0)
            {
                /* @REPLACE_BRACKET: n����16 */
                while (n > PADSIZE)
                {
                    /* @KEEP_COMMENT: ����putbuf(DR.1.14)����16���ֽڵ�'0'��<outarg> */
                    putbuf((signed char *)zeroes, PADSIZE, (signed char **)outarg);

                    /* @KEEP_COMMENT: ����nΪn-16 */
                    n -= PADSIZE;
                }

                /* @KEEP_COMMENT: ����putbuf(DR.1.14)����n���ֽڵ�'0'��<outarg> */
                putbuf((signed char *)zeroes, n, (signed char **)outarg);
            }
        }

        /* @REPLACE_BRACKET: sign������0 */
        if (0 != sign)
        {
            /* @KEEP_COMMENT: ����putbuf(DR.1.14)����1���ֽڵĸ��ŵ�<outarg> */
            putbuf(&sign, 1, (signed char **)outarg);
        }
        else
        {
            /* @REPLACE_BRACKET: doHexPrefix������0 */
            if (0 != doHexPrefix)
            {
                /* @KEEP_COMMENT: ����ox[0]Ϊ'0'������ox[1]Ϊch */
                ox[0] = '0';
                ox[1] = ch;

                /* @KEEP_COMMENT: ����putbuf(DR.1.14)����2���ֽڵ�"0x"��<outarg> */
                putbuf(ox, 2, (signed char **)outarg);
            }
        }

        /* @REPLACE_BRACKET: doLAdjustΪ0 && doZeroPad��Ϊ0 */
        if ((0 == doLAdjust) && (0 != doZeroPad))
        {
            /* @KEEP_COMMENT: ����nΪwidth-realsz */
            n = width-realsz;

            /* @REPLACE_BRACKET: n����0 */
            if (n > 0)
            {
                /* @REPLACE_BRACKET: n����16 */
                while (n > PADSIZE)
                {
                    /* @KEEP_COMMENT: ����putbuf(DR.1.14)����16���ֽڵĿո�<outarg> */
                    putbuf((signed char *)blanks, PADSIZE, (signed char **)outarg);

                    /* @KEEP_COMMENT: ����nΪn-16 */
                    n -= PADSIZE;
                }

                /* @KEEP_COMMENT: ����putbuf(DR.1.14)����n���ֽڵ�' '��<outarg> */
                putbuf((signed char *)blanks, n, (signed char **)outarg);
            }
        }

        /* @KEEP_COMMENT: ����nΪdprec-fieldsz */
        n = dprec-fieldsz;

        /* @REPLACE_BRACKET: n����0 */
        if (n > 0)
        {
            /* @REPLACE_BRACKET: n����16 */
            while (n > PADSIZE)
            {
                /* @KEEP_COMMENT: ����putbuf(DR.1.14)����16���ֽڵ�'0'��<outarg> */
                putbuf((signed char *)zeroes, PADSIZE, (signed char **)outarg);

                /* @KEEP_COMMENT: ����nΪn-16 */
                n -= PADSIZE;
            }

            /* @KEEP_COMMENT: ����putbuf(DR.1.14)����n���ֽڵ�'0'��<outarg> */
            putbuf((signed char *)zeroes, n, (signed char **)outarg);
        }

        /* @KEEP_COMMENT: ����putbuf(DR.1.14)��cp����size���ֽڵ�<outarg> */
        putbuf(cp, size, (signed char **)outarg);

        /* @KEEP_COMMENT: ����nΪfpprec */
        n = fpprec;

        /* @REPLACE_BRACKET: n����0 */
        if (n > 0)
        {
            /* @REPLACE_BRACKET: n����16 */
            while (n > PADSIZE)
            {
                /* @KEEP_COMMENT: ����putbuf(DR.1.14)����16���ֽڵ�'0'��<outarg> */
                putbuf((signed char *)zeroes, PADSIZE, (signed char **)outarg);

                /* @KEEP_COMMENT: ����nΪn-16 */
                n -= PADSIZE;
            }

            /* @KEEP_COMMENT: ����putbuf(DR.1.14)����n���ֽڵ�'0'��<outarg> */
            putbuf((signed char *)zeroes, n, (signed char **)outarg);
        }

        /* @REPLACE_BRACKET: doLAdjust������0 */
        if (0 != doLAdjust)
        {
            /* @KEEP_COMMENT: ����nΪwidth-realsz */
            n = width - realsz;

            /* @REPLACE_BRACKET: n����0 */
            if (n > 0)
            {
                /* @REPLACE_BRACKET: n����16 */
                while (n > PADSIZE)
                {
                    /* @KEEP_COMMENT: ����putbuf(DR.1.14)����16���ֽڵĿո�<outarg> */
                    putbuf((signed char *)blanks, PADSIZE, (signed char **)outarg);

                    /* @KEEP_COMMENT: ����nΪn-16 */
                    n -= PADSIZE;
                }

                /* @KEEP_COMMENT: ����putbuf(DR.1.14)����n���ֽڵĿո�<outarg> */
                putbuf((signed char *)blanks, n, (signed char **)outarg);
            }
        }

        /*
         * @KEEP_COMMENT: ���width����realsz������retΪret+width��
         * ��������retΪret+realsz
         */
        ret += (width > realsz) ? width : realsz;
    }

    /* @REPLACE_BRACKET: ERROR */
    return(ERROR);
}

/*
 * @brief: 
 *    ��Դ����������ָ�����ȵ��ֽڵ�Ŀ�Ļ�������
 * @param[in]: inbuf: Դ������
 * @param[in]: length: ���Ƶĳ���
 * @param[out]: outptr: ָ��Ŀ�껺������ָ��
 * @return: 
 *    OK:�����ɹ�
 * @tracedREQ: RR.1.1
 * @implements: DR.1.15
 */
static int putbuf(signed char *inbuf,
int length,
signed char **outptr)
{
    /*
     * @KEEP_COMMENT: ����bcopy(DR.1.12)��<inbuf>����<length>�����ֽڵ���<outptr>
     * Ϊ��ʼ��ַ��Ŀ������
     */
    bcopy(inbuf, *outptr, length);

    /* @KEEP_COMMENT: ����*outptrΪ*outptr+length */
    *outptr += (signed char)length;

    /* @REPLACE_BRACKET: OK */
    return(OK);
}


