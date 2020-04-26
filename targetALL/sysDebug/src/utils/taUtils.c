/************************************************************************
*				北京科银京成技术有限公司 版权所有
* 	 Copyright (C)  2013 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/*
 * 修改历史:
 * 2013-06-26   彭元志，北京科银京成技术有限公司
 *                  创建该文件。
*/

/*
* @file: taUtils.c
* @brief:
*	    <li>公用函数库</li>
*/

/************************头 文 件******************************/
#include "taUtil.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/* 单个16进制数字对应的的ASCII字符 */
const UINT8 taHexChars[17] = "0123456789abcdef";

/************************函数实现*****************************/

/*
 * @brief:
 *      将32位整数wValue转换成字节流保存在bpDest指定的开始位置，转换后字节流的最大长度为wBytes
 * @param[in]:value: 待转换的整数
 * @param[in]:bytes: 字节流的最大长度
 * @param[out]:dest: 存放字节流的起始地址
 * @return:
 *     FALSE: 转换失败
 *     TRUE: 转换成功
 */
BOOL __store_long(INT32 value, UINT8 *dest, INT32 bytes)
{
    UINT8 upper_bit = (value < 0 ? 128 : 0);
    UINT32 i;

    if (value < 0)
    {
        INT32 oldvalue = value;
        value = -value;
        
        if (oldvalue != -value)
        {
            return (FALSE);
        }
    }

    for (i = 0 ; i < (sizeof (value) < bytes ? sizeof (value) : bytes) ; i++)
    {
        /* 根据i的大小提取value的低7位或低8位存至<dest>第i字节 */
        dest[i] = (UINT8)( (UINT32)(value) & (UINT32)((i == ((UINT32)bytes - 1) )? 0x7F : 0xFF));

        /* <value>右移8位 */
        value = value / 256;
    }

    if ((value != 0) && (value != -1))
    {
        return (FALSE);
    }

    /* 将未设置大小的位全部置为0 */
    for (; ((INT32)i) < bytes ; i++)
    {
        dest[i] = (UINT8)0;
    }

    /* 保存符号位upper_bit至<dest>中 */
    dest[(UINT32)(bytes - 1)] |= upper_bit;

    return (TRUE);
}

/*
 * @brief:
 *      将16位整数hValue转换成字节流保存在bpDest指向的缓冲中，转换后字节流的最大长度为wBytes
 * @param[in]:value: 待转换的整数
 * @param[in]:bytes: 字节流的最大长度，即<bpDest>指向的缓冲的最大长度
 * @param[out]:dest: 存放字节流的起始地址
 * @return:
 *      FALSE: 转换失败
 *      TRUE: 转换成功
 */
BOOL __store_short(INT16 value, UINT8 *dest, INT32 bytes)
{
    UINT8 upper_bit = (value < 0 ? 128 : 0);
    INT32 i = 0;

    if (value < 0)
    {
        INT32 oldvalue = value;
        value = -value;
        
        if (oldvalue != -value)
        {
            return (FALSE);
        }
    }

    /* 
     * 依次将<value>中的值存入<dest>缓冲中，定义循环总次数count，当前次数i，
     * 如果<value>长度小于<bytes>，设置count值为<value>长度；否则设置count值为<bytes> 
     */
    for (i = 0 ; i < (sizeof (value) < bytes ? sizeof (value) : bytes) ; i++)
    {
        /* 根据i的大小提取value的低7位或低8位存至<dest>第i字节 */
        dest[i] = (UINT8)( ((UINT16)(value)) & (UINT16)((i == (bytes - 1)) ? 127 : 255));

        /* <value>右移8位 */
        value = value / 256;
    }

    /* <value>不等于0，并且<value>不等于-1 */
    if ((value != 0) && (value != -1))
    {
        /* @REPLACE_BRACKET: FALSE */
        return (FALSE);
    }

    /*将未设置大小的位全部置为0 */
    for (; i < bytes ; i++)
    {
        dest[((UINT32)(i))] = (UINT8)0;
    }

    dest[(UINT32)(bytes - 1)] |= upper_bit;

    return (TRUE);
}

/*
 * @brief:
 *      将bpSource作为起始地址的字节流中wBytes个字节转换成整数保存到bpDest指定的位置
 * @param[in]:source: 存放字节流的起始地址
 * @param[in]:bytes: 字节流的长度
 * @param[out]:dest: 指向转换后的整数的指针
 * @return:
 *      FALSE: 转换失败
 *      TRUE: 转换成功
 */
BOOL __fetch_long(INT32 *dest, const UINT8* source, INT32 bytes)
{
    INT32 value = 0;
    INT32 i;

    for (i = bytes - 1; (INT32) i > (sizeof (*dest) - 1); i--)
    {
        /* <source>中超过<*dest>大小部分除符号位外出现不是0的位 */
        if ((source[((UINT32)(i))] & ((UINT32) i == (UINT32)((((UINT32)bytes - 1) !=0 ) ? 127 : 255) )) != 0)
        {
            return (FALSE);
        }
    }

    for (; i >= 0; i--)
    {
        /* 
         * 如果i不等于<bytes-1>，设置value值为value左移8位并加上<source>第i字节和255按位与，
         * 如果i等于<bytes-1>，设置value值为value左移8位并加上<source>第i字节和127按位与结果
         */
        value = value * 256 + (INT32)(source[i] & ((UINT8)(i == (bytes - 1) ? 127 : 255)));
    }

    if (((source[bytes - 1] & 128) != 0) && (value > 0))
    {
        value = - value;
    }

    /* 将value的值存入<dest>指向的空间 */
    *dest = value;

    return (TRUE);
}

/*
 * @brief:
 *      将bpSource作为起始地址的字节流中wBytes个字节转换成整数保存到bpDest指定的位置
 * @param[in]:source: 存放字节流的起始地址
 * @param[in]:bytes: 字节流的长度
 * @param[out]:dest: 指向转换后的整数的指针
 * @return:
 *      FALSE: 转换失败
 *      TRUE: 转换成功
 */
BOOL __fetch_short(UINT16 *dest, const UINT8* source, INT32 bytes)
{
    INT16 value = 0;
    INT32 i;

    for (i = bytes - 1;  i > (sizeof (*dest) - 1); i--)
    {
        /* <source>中超过<*dest>大小部分除符号位外出现不是0的位 */
        if ((source[i] & (UINT8)((i == (bytes - 1)) ? 127 : 255 )) != 0)
        {
            return (FALSE);
        }
    }

    for (; i >= 0; i--)
    {
        /* 
         * 如果i不等于<bytes-1>，设置value值为value左移8位并加上<source>第i字节和255按位与，
         * 如果i等于<bytes-1>，设置value值为value左移8位并加上<source>第i字节和127按位与结果
         */
        value = value * 256 + (INT16)(source[i] & ((UINT32)(i == (bytes - 1) ? 127 : 255)));
    }

    if (((source[(UINT32)(bytes - 1)] & 128) != 0 ) && (value > 0))
    {
        value = - value;
    }

    /* 将value的值存入<dest>指向的空间 */
    *dest = (UINT16)value;

    return (TRUE);
}

/*
 * @brief:
 *      将单个的十六进制ASCII字符转换成对应的数字
 * @param[in]:ch: 待转换的字符
 * @return:
 *      转换后对应的数字
 *      -1: ch不为十六进制ASCII字符
 */
INT32 hex2num(UINT8 ch)
{
    /* ASCII值在'a'到'f'之间 */
    if ((ch >= 'a') && (ch <= 'f'))
    {
        return ((INT32)((INT8)ch - 'a' + 10));
    }

    /* ASCII值在'0'到'9'之间 */
    if ((ch >= '0') && (ch <= '9'))
    {
        return ((INT32)(ch - '0'));
    }

    /* ASCII值在'A'到'F'之间 */
    if ((ch >= 'A') && (ch <= 'F'))
    {
        return ((INT32)((INT8)ch - 'A' + 10));
    }

    return (-1);
}

/*
 * @brief:
 *     将一块内存区中的内容转换成十六进制字符串保存在指定的内存位置中
 * @param[in]:mem: 内存起始地址
 * @param[in]:count: 转换的内存区长度
 * @param[out]:buf: 保存十六进制字符串的缓冲区的地址
 * @return:
 *     无
 */
void mem2hex(const UINT8 *mem, UINT8 *buf, UINT32 count)
{
    UINT8 ch;
    
    while (count > 0)
    {
        count--;

        /* 提取内存<*mem>中的字符存至ch，并让mem指向下一待转内存区域 */
        ch = *mem;

        mem++;

        /* 取ch的高4位并将其转换为对应的16进制数，存放到<buf>指向的缓冲中，并且将<buf>指向下一字节 */
        *buf = taHexChars[(UINT32)(ch >> 4)];
        buf++;

        /* 取ch的低4位并将其转换为对应的16进制数，存放到<buf>指向的缓冲中，并且将<buf>指向下一字节 */
        *buf = taHexChars[(UINT32)(ch & 0xf)];
        buf++;
    }

    *buf = '\0' ;

    return;
}

/*
 * @brief:
 *      将一块内存区中的内容转换成十六进制字符串。
 * @param buf: 输入数据和输出字符串存放的起始地址
 * @param[in]:count: 转换的内存区长度
 * @return:
 *      无
 */
void mem2hexEx(UINT8 *buf, UINT32 count)
{
    UINT8 ch;
    
    /* 
     ×　由于一个内存字节，表示成十六进制字符串需要两个字节，所以需向<buf>的<count>字节后扩充存储，
     ×　定义指针mem，其值为<buf>加上<count> 
     */
    UINT8 *mem = buf + count;

    while (count > 0)
    {
        count--;
        mem--;
        ch = *mem;

        /* 取ch的高4位并将其转换为对应的16进制数 */
        *(mem + count)     = taHexChars[(UINT32)(ch >> 4)];

        /* 取ch的低4位并将其转换为对应的16进制数 */
        *(mem + count + 1) = taHexChars[(UINT32)(ch & 0xf)];
    }
}

/*
 * @brief:
 *      将十六进制字符串转换成对应的内容保存在指定的内存位置中
 * @param[in]:   buf: 指向十六进制字符串的缓冲区的指针
 * @param[in]:   count: 转换的内存区长度
 * @param[out]:   mem: 转换后数据的内存起始地址
 * @return:
 *      转换后数据的内存起始地址
 */
UINT8* hex2mem(const UINT8 *buf, UINT8 *mem, UINT32 count)
{
    UINT8 ch;
    UINT8 *pMem = mem;
    
    while (count >0)
    {
        count--;

        /* 将<buf>的内容转换成对应的数字存入ch的高4位 */
        ch = ((UINT32)(hex2num(*buf)))<< 4;
        buf++;

        /* 将<buf>的内容转换成对应的数字存入ch的低4位 */
        ch |= (UINT32)hex2num(*buf);
        buf++;

        /*设置<mem>指向空间内容为ch，<mem>自加1指向下一字节空间 */
        *mem = ch;
        mem++;
    }

    return (pMem);
}

/*
 * @brief:
 *      将一块内存区中十六进制字符串转换成对应的内容保存在指定的内存位置中
 * @param[in]:buf: 输入字符串和输出数据存放的起始地址
 * @param[in]:count: 转换的字符串长度
 * @param[out]:outbuf: 输入字符串和输出数据存放的起始地址
 * @return:
 *     -1:转换失败
 *      0:转换成功
 */
INT32 hex2memEx(const UINT8 *buf, UINT8 *outbuf, UINT32 count)
{
    UINT8 ch;
    INT32 num;

    while (count > 0)
    {
        count--;

        /* 将<buf>的内容转换成对应的数字存入ch的高4位 */
        num = hex2num(*buf);
        if(-1 == num)
        {
            return (-1);
        }
        ch = (UINT8)(((UINT32)num) << 4);
        buf++;

        /* 将<buf>的内容转换成对应的数字存入ch的低4位 */
        num = hex2num(*buf);
        if(-1 == num)
        {
            return (-1);
        }

        ch |= (UINT8)num;
        buf++;

        /* 设置<outbuf>指向空间内容为ch，<outbuf>自加1指向下一字节空间 */
        *outbuf = ch;
        outbuf++;
    }

    return (0);
}

/*
 * @brief:
 *      将十六进制字符串转换成对应32位整数
 * @param ptr:输入时为指向十六进制字符串首地址的指针，输出时为转换结束时的指针
 * @param[out]:intValue: 指向转换后的32位整数的指针
 * @return:
 *      转换的字符数
 */
UINT32 hex2int(const UINT8 **ptr, UINT32 *intValue)
{
    UINT32 numChars = 0;
    INT32 hexValue;

    *intValue = 0;

    /* <*ptr>指向的的字符不为0 */
    while ((**ptr) != 0)
    {
        /* 将<*ptr>指向的16进制字符转换为对应的数字 */
        hexValue = hex2num(**ptr);

        /* 转换后的数字小于0 */
        if (hexValue < 0)
        {
            break;
        }

        /* 转化次数超过8 */
        if ((numChars++) > 7)
        {
            /* 
             * 传入的待转换成32为整型的字符串不合法，其转换后大小超过32位整数表示范围，
             * 因为4字节整型的十六进制字符串长度最大为8字节，设置<*intValue>值为0
             */
            *intValue = 0;

            return (0);
        }

        /* 将转换后的数依次保存至<*intValue> */
        *intValue = (*intValue << 4) | (UINT32)hexValue;

        (*ptr)++;
    }

    /* 已转换的字符的个数 */
    return (numChars);
}

/*
 * @brief:
 *      将十六进制字符串转换成对应64位整数
 * @param ptr: 输入时为指向十六进制字符串首地址的指针，输出时为转换结束时的指针
 * @param[out]: intValue: 指向转换后的64位整数的指针
 * @return:
 *      转换的字符数
 */
UINT32 hex2int64(const UINT8 **ptr, UINT64 *intValue)
{
    UINT32 numChars = 0;
    INT32 hexValue;

    *intValue = 0;

    /* <*ptr>指向的的字符不为0 */
    while ((**ptr) != 0)
    {
        /* 将<*ptr>指向的16进制字符转换为对应的数字 */
        hexValue = hex2num(**ptr);

        /* 转换后的数字小于0 */
        if (hexValue < 0)
        {
            break;
        }

        /* 转化次数超过16 */
        if ((numChars++) > 15)
        {
            /* 
             * 传入的待转换成32为整型的字符串不合法，其转换后大小超过32位整数表示范围，
             * 因为4字节整型的十六进制字符串长度最大为8字节，设置<*intValue>值为0
             */
            *intValue = 0;
            
            return (0);
        }

        /* 将转换后的数依次保存至<*intValue> */
        *intValue = (*intValue << 4) | (UINT32)hexValue;

        (*ptr)++;
    }

    /* 已转换的字符的个数 */
    return (numChars);
}

/*
 * @brief:
 *      获取16进制字符串中'<'和'>'之间字符串，如果字符串首位不为'<'则返回0。
 * @param ubpPtr: 输入时为指向十六进制字符串首地址的指针，输出时为转换结束时的指针
 * @param[in]:len:输入字符串中'<'和'>'之间字符串的最大长度
 * @param[out]:strAddr: 指向转换后输出字符串首地址的指针
 * @return:
 *      输出字符串字符个数
 */
UINT32 hex2str(const UINT8 **ubpPtr, UINT8 **strAddr, UINT32 len)
{
    UINT32 cnt = 0;

    if ((len > 0) && (**ubpPtr == '<'))
    {
        (*ubpPtr)++;

        /* 使strAddr指向和ptr相同的字符串 */
        while ((**ubpPtr != '>') && (cnt < len))
        {
            cnt++;
            **strAddr = **ubpPtr;
            (*ubpPtr)++;
            (*strAddr)++;
        }

        /* 统计字符个数为0，则设置输出字符串指针为NULL */
        if ((cnt == 0) || (**ubpPtr != '>'))
        {
            return (0);
        }

        **strAddr = '\0';
        (*ubpPtr)++;
    }

    /* 返回字符个数 */
    return (cnt);
}

