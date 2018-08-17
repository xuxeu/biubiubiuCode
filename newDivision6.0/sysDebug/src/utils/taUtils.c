/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
* 	 Copyright (C)  2013 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/*
 * �޸���ʷ:
 * 2013-06-26   ��Ԫ־�������������ɼ������޹�˾
 *                  �������ļ���
*/

/*
* @file: taUtils.c
* @brief:
*	    <li>���ú�����</li>
*/

/************************ͷ �� ��******************************/
#include "taUtil.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/* ����16�������ֶ�Ӧ�ĵ�ASCII�ַ� */
const UINT8 taHexChars[17] = "0123456789abcdef";

/************************����ʵ��*****************************/

/*
 * @brief:
 *      ��32λ����wValueת�����ֽ���������bpDestָ���Ŀ�ʼλ�ã�ת�����ֽ�������󳤶�ΪwBytes
 * @param[in]:value: ��ת��������
 * @param[in]:bytes: �ֽ�������󳤶�
 * @param[out]:dest: ����ֽ�������ʼ��ַ
 * @return:
 *     FALSE: ת��ʧ��
 *     TRUE: ת���ɹ�
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
        /* ����i�Ĵ�С��ȡvalue�ĵ�7λ���8λ����<dest>��i�ֽ� */
        dest[i] = (UINT8)( (UINT32)(value) & (UINT32)((i == ((UINT32)bytes - 1) )? 0x7F : 0xFF));

        /* <value>����8λ */
        value = value / 256;
    }

    if ((value != 0) && (value != -1))
    {
        return (FALSE);
    }

    /* ��δ���ô�С��λȫ����Ϊ0 */
    for (; ((INT32)i) < bytes ; i++)
    {
        dest[i] = (UINT8)0;
    }

    /* �������λupper_bit��<dest>�� */
    dest[(UINT32)(bytes - 1)] |= upper_bit;

    return (TRUE);
}

/*
 * @brief:
 *      ��16λ����hValueת�����ֽ���������bpDestָ��Ļ����У�ת�����ֽ�������󳤶�ΪwBytes
 * @param[in]:value: ��ת��������
 * @param[in]:bytes: �ֽ�������󳤶ȣ���<bpDest>ָ��Ļ������󳤶�
 * @param[out]:dest: ����ֽ�������ʼ��ַ
 * @return:
 *      FALSE: ת��ʧ��
 *      TRUE: ת���ɹ�
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
     * ���ν�<value>�е�ֵ����<dest>�����У�����ѭ���ܴ���count����ǰ����i��
     * ���<value>����С��<bytes>������countֵΪ<value>���ȣ���������countֵΪ<bytes> 
     */
    for (i = 0 ; i < (sizeof (value) < bytes ? sizeof (value) : bytes) ; i++)
    {
        /* ����i�Ĵ�С��ȡvalue�ĵ�7λ���8λ����<dest>��i�ֽ� */
        dest[i] = (UINT8)( ((UINT16)(value)) & (UINT16)((i == (bytes - 1)) ? 127 : 255));

        /* <value>����8λ */
        value = value / 256;
    }

    /* <value>������0������<value>������-1 */
    if ((value != 0) && (value != -1))
    {
        /* @REPLACE_BRACKET: FALSE */
        return (FALSE);
    }

    /*��δ���ô�С��λȫ����Ϊ0 */
    for (; i < bytes ; i++)
    {
        dest[((UINT32)(i))] = (UINT8)0;
    }

    dest[(UINT32)(bytes - 1)] |= upper_bit;

    return (TRUE);
}

/*
 * @brief:
 *      ��bpSource��Ϊ��ʼ��ַ���ֽ�����wBytes���ֽ�ת�����������浽bpDestָ����λ��
 * @param[in]:source: ����ֽ�������ʼ��ַ
 * @param[in]:bytes: �ֽ����ĳ���
 * @param[out]:dest: ָ��ת�����������ָ��
 * @return:
 *      FALSE: ת��ʧ��
 *      TRUE: ת���ɹ�
 */
BOOL __fetch_long(INT32 *dest, const UINT8* source, INT32 bytes)
{
    INT32 value = 0;
    INT32 i;

    for (i = bytes - 1; (INT32) i > (sizeof (*dest) - 1); i--)
    {
        /* <source>�г���<*dest>��С���ֳ�����λ����ֲ���0��λ */
        if ((source[((UINT32)(i))] & ((UINT32) i == (UINT32)((((UINT32)bytes - 1) !=0 ) ? 127 : 255) )) != 0)
        {
            return (FALSE);
        }
    }

    for (; i >= 0; i--)
    {
        /* 
         * ���i������<bytes-1>������valueֵΪvalue����8λ������<source>��i�ֽں�255��λ�룬
         * ���i����<bytes-1>������valueֵΪvalue����8λ������<source>��i�ֽں�127��λ����
         */
        value = value * 256 + (INT32)(source[i] & ((UINT8)(i == (bytes - 1) ? 127 : 255)));
    }

    if (((source[bytes - 1] & 128) != 0) && (value > 0))
    {
        value = - value;
    }

    /* ��value��ֵ����<dest>ָ��Ŀռ� */
    *dest = value;

    return (TRUE);
}

/*
 * @brief:
 *      ��bpSource��Ϊ��ʼ��ַ���ֽ�����wBytes���ֽ�ת�����������浽bpDestָ����λ��
 * @param[in]:source: ����ֽ�������ʼ��ַ
 * @param[in]:bytes: �ֽ����ĳ���
 * @param[out]:dest: ָ��ת�����������ָ��
 * @return:
 *      FALSE: ת��ʧ��
 *      TRUE: ת���ɹ�
 */
BOOL __fetch_short(UINT16 *dest, const UINT8* source, INT32 bytes)
{
    INT16 value = 0;
    INT32 i;

    for (i = bytes - 1;  i > (sizeof (*dest) - 1); i--)
    {
        /* <source>�г���<*dest>��С���ֳ�����λ����ֲ���0��λ */
        if ((source[i] & (UINT8)((i == (bytes - 1)) ? 127 : 255 )) != 0)
        {
            return (FALSE);
        }
    }

    for (; i >= 0; i--)
    {
        /* 
         * ���i������<bytes-1>������valueֵΪvalue����8λ������<source>��i�ֽں�255��λ�룬
         * ���i����<bytes-1>������valueֵΪvalue����8λ������<source>��i�ֽں�127��λ����
         */
        value = value * 256 + (INT16)(source[i] & ((UINT32)(i == (bytes - 1) ? 127 : 255)));
    }

    if (((source[(UINT32)(bytes - 1)] & 128) != 0 ) && (value > 0))
    {
        value = - value;
    }

    /* ��value��ֵ����<dest>ָ��Ŀռ� */
    *dest = (UINT16)value;

    return (TRUE);
}

/*
 * @brief:
 *      ��������ʮ������ASCII�ַ�ת���ɶ�Ӧ������
 * @param[in]:ch: ��ת�����ַ�
 * @return:
 *      ת�����Ӧ������
 *      -1: ch��Ϊʮ������ASCII�ַ�
 */
INT32 hex2num(UINT8 ch)
{
    /* ASCIIֵ��'a'��'f'֮�� */
    if ((ch >= 'a') && (ch <= 'f'))
    {
        return ((INT32)((INT8)ch - 'a' + 10));
    }

    /* ASCIIֵ��'0'��'9'֮�� */
    if ((ch >= '0') && (ch <= '9'))
    {
        return ((INT32)(ch - '0'));
    }

    /* ASCIIֵ��'A'��'F'֮�� */
    if ((ch >= 'A') && (ch <= 'F'))
    {
        return ((INT32)((INT8)ch - 'A' + 10));
    }

    return (-1);
}

/*
 * @brief:
 *     ��һ���ڴ����е�����ת����ʮ�������ַ���������ָ�����ڴ�λ����
 * @param[in]:mem: �ڴ���ʼ��ַ
 * @param[in]:count: ת�����ڴ�������
 * @param[out]:buf: ����ʮ�������ַ����Ļ������ĵ�ַ
 * @return:
 *     ��
 */
void mem2hex(const UINT8 *mem, UINT8 *buf, UINT32 count)
{
    UINT8 ch;
    
    while (count > 0)
    {
        count--;

        /* ��ȡ�ڴ�<*mem>�е��ַ�����ch������memָ����һ��ת�ڴ����� */
        ch = *mem;

        mem++;

        /* ȡch�ĸ�4λ������ת��Ϊ��Ӧ��16����������ŵ�<buf>ָ��Ļ����У����ҽ�<buf>ָ����һ�ֽ� */
        *buf = taHexChars[(UINT32)(ch >> 4)];
        buf++;

        /* ȡch�ĵ�4λ������ת��Ϊ��Ӧ��16����������ŵ�<buf>ָ��Ļ����У����ҽ�<buf>ָ����һ�ֽ� */
        *buf = taHexChars[(UINT32)(ch & 0xf)];
        buf++;
    }

    *buf = '\0' ;

    return;
}

/*
 * @brief:
 *      ��һ���ڴ����е�����ת����ʮ�������ַ�����
 * @param buf: �������ݺ�����ַ�����ŵ���ʼ��ַ
 * @param[in]:count: ת�����ڴ�������
 * @return:
 *      ��
 */
void mem2hexEx(UINT8 *buf, UINT32 count)
{
    UINT8 ch;
    
    /* 
     ��������һ���ڴ��ֽڣ���ʾ��ʮ�������ַ�����Ҫ�����ֽڣ���������<buf>��<count>�ֽں�����洢��
     ��������ָ��mem����ֵΪ<buf>����<count> 
     */
    UINT8 *mem = buf + count;

    while (count > 0)
    {
        count--;
        mem--;
        ch = *mem;

        /* ȡch�ĸ�4λ������ת��Ϊ��Ӧ��16������ */
        *(mem + count)     = taHexChars[(UINT32)(ch >> 4)];

        /* ȡch�ĵ�4λ������ת��Ϊ��Ӧ��16������ */
        *(mem + count + 1) = taHexChars[(UINT32)(ch & 0xf)];
    }
}

/*
 * @brief:
 *      ��ʮ�������ַ���ת���ɶ�Ӧ�����ݱ�����ָ�����ڴ�λ����
 * @param[in]:   buf: ָ��ʮ�������ַ����Ļ�������ָ��
 * @param[in]:   count: ת�����ڴ�������
 * @param[out]:   mem: ת�������ݵ��ڴ���ʼ��ַ
 * @return:
 *      ת�������ݵ��ڴ���ʼ��ַ
 */
UINT8* hex2mem(const UINT8 *buf, UINT8 *mem, UINT32 count)
{
    UINT8 ch;
    UINT8 *pMem = mem;
    
    while (count >0)
    {
        count--;

        /* ��<buf>������ת���ɶ�Ӧ�����ִ���ch�ĸ�4λ */
        ch = ((UINT32)(hex2num(*buf)))<< 4;
        buf++;

        /* ��<buf>������ת���ɶ�Ӧ�����ִ���ch�ĵ�4λ */
        ch |= (UINT32)hex2num(*buf);
        buf++;

        /*����<mem>ָ��ռ�����Ϊch��<mem>�Լ�1ָ����һ�ֽڿռ� */
        *mem = ch;
        mem++;
    }

    return (pMem);
}

/*
 * @brief:
 *      ��һ���ڴ�����ʮ�������ַ���ת���ɶ�Ӧ�����ݱ�����ָ�����ڴ�λ����
 * @param[in]:buf: �����ַ�����������ݴ�ŵ���ʼ��ַ
 * @param[in]:count: ת�����ַ�������
 * @param[out]:outbuf: �����ַ�����������ݴ�ŵ���ʼ��ַ
 * @return:
 *     -1:ת��ʧ��
 *      0:ת���ɹ�
 */
INT32 hex2memEx(const UINT8 *buf, UINT8 *outbuf, UINT32 count)
{
    UINT8 ch;
    INT32 num;

    while (count > 0)
    {
        count--;

        /* ��<buf>������ת���ɶ�Ӧ�����ִ���ch�ĸ�4λ */
        num = hex2num(*buf);
        if(-1 == num)
        {
            return (-1);
        }
        ch = (UINT8)(((UINT32)num) << 4);
        buf++;

        /* ��<buf>������ת���ɶ�Ӧ�����ִ���ch�ĵ�4λ */
        num = hex2num(*buf);
        if(-1 == num)
        {
            return (-1);
        }

        ch |= (UINT8)num;
        buf++;

        /* ����<outbuf>ָ��ռ�����Ϊch��<outbuf>�Լ�1ָ����һ�ֽڿռ� */
        *outbuf = ch;
        outbuf++;
    }

    return (0);
}

/*
 * @brief:
 *      ��ʮ�������ַ���ת���ɶ�Ӧ32λ����
 * @param ptr:����ʱΪָ��ʮ�������ַ����׵�ַ��ָ�룬���ʱΪת������ʱ��ָ��
 * @param[out]:intValue: ָ��ת�����32λ������ָ��
 * @return:
 *      ת�����ַ���
 */
UINT32 hex2int(const UINT8 **ptr, UINT32 *intValue)
{
    UINT32 numChars = 0;
    INT32 hexValue;

    *intValue = 0;

    /* <*ptr>ָ��ĵ��ַ���Ϊ0 */
    while ((**ptr) != 0)
    {
        /* ��<*ptr>ָ���16�����ַ�ת��Ϊ��Ӧ������ */
        hexValue = hex2num(**ptr);

        /* ת���������С��0 */
        if (hexValue < 0)
        {
            break;
        }

        /* ת����������8 */
        if ((numChars++) > 7)
        {
            /* 
             * ����Ĵ�ת����32Ϊ���͵��ַ������Ϸ�����ת�����С����32λ������ʾ��Χ��
             * ��Ϊ4�ֽ����͵�ʮ�������ַ����������Ϊ8�ֽڣ�����<*intValue>ֵΪ0
             */
            *intValue = 0;

            return (0);
        }

        /* ��ת����������α�����<*intValue> */
        *intValue = (*intValue << 4) | (UINT32)hexValue;

        (*ptr)++;
    }

    /* ��ת�����ַ��ĸ��� */
    return (numChars);
}

/*
 * @brief:
 *      ��ʮ�������ַ���ת���ɶ�Ӧ64λ����
 * @param ptr: ����ʱΪָ��ʮ�������ַ����׵�ַ��ָ�룬���ʱΪת������ʱ��ָ��
 * @param[out]: intValue: ָ��ת�����64λ������ָ��
 * @return:
 *      ת�����ַ���
 */
UINT32 hex2int64(const UINT8 **ptr, UINT64 *intValue)
{
    UINT32 numChars = 0;
    INT32 hexValue;

    *intValue = 0;

    /* <*ptr>ָ��ĵ��ַ���Ϊ0 */
    while ((**ptr) != 0)
    {
        /* ��<*ptr>ָ���16�����ַ�ת��Ϊ��Ӧ������ */
        hexValue = hex2num(**ptr);

        /* ת���������С��0 */
        if (hexValue < 0)
        {
            break;
        }

        /* ת����������16 */
        if ((numChars++) > 15)
        {
            /* 
             * ����Ĵ�ת����32Ϊ���͵��ַ������Ϸ�����ת�����С����32λ������ʾ��Χ��
             * ��Ϊ4�ֽ����͵�ʮ�������ַ����������Ϊ8�ֽڣ�����<*intValue>ֵΪ0
             */
            *intValue = 0;
            
            return (0);
        }

        /* ��ת����������α�����<*intValue> */
        *intValue = (*intValue << 4) | (UINT32)hexValue;

        (*ptr)++;
    }

    /* ��ת�����ַ��ĸ��� */
    return (numChars);
}

/*
 * @brief:
 *      ��ȡ16�����ַ�����'<'��'>'֮���ַ���������ַ�����λ��Ϊ'<'�򷵻�0��
 * @param ubpPtr: ����ʱΪָ��ʮ�������ַ����׵�ַ��ָ�룬���ʱΪת������ʱ��ָ��
 * @param[in]:len:�����ַ�����'<'��'>'֮���ַ�������󳤶�
 * @param[out]:strAddr: ָ��ת��������ַ����׵�ַ��ָ��
 * @return:
 *      ����ַ����ַ�����
 */
UINT32 hex2str(const UINT8 **ubpPtr, UINT8 **strAddr, UINT32 len)
{
    UINT32 cnt = 0;

    if ((len > 0) && (**ubpPtr == '<'))
    {
        (*ubpPtr)++;

        /* ʹstrAddrָ���ptr��ͬ���ַ��� */
        while ((**ubpPtr != '>') && (cnt < len))
        {
            cnt++;
            **strAddr = **ubpPtr;
            (*ubpPtr)++;
            (*strAddr)++;
        }

        /* ͳ���ַ�����Ϊ0������������ַ���ָ��ΪNULL */
        if ((cnt == 0) || (**ubpPtr != '>'))
        {
            return (0);
        }

        **strAddr = '\0';
        (*ubpPtr)++;
    }

    /* �����ַ����� */
    return (cnt);
}

