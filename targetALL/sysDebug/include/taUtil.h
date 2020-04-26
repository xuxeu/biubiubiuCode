/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-29         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:taUtil.h
 *@brief:
 *             <li>公用函数定义</li>
 */
#ifndef _TAUTIL_H
#define _TAUTIL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"

/************************宏定义********************************/

/* 清零定义 */
#define ZeroMemory(addr,size) (memset((void *)(addr),0,(size)))

/************************类型定义******************************/

/************************接口声明******************************/
extern const UINT8 taHexChars[17];

/*
 * @brief:
 *      将32位整数wValue转换成字节流保存在bpDest指定的开始位置，转换后字节流的最大长度为wBytes
 * @param[in]: value: 待转换的整数
 * @param[in]: bytes: 字节流的最大长度
 * @param[out]: dest: 存放字节流的起始地址
 * @return:
 *     FALSE: 转换失败
 *     TRUE: 转换成功
 */
BOOL __store_long(INT32 value, UINT8 *dest, INT32 bytes);

/*
 * @brief:
 *      将bpSource作为起始地址的字节流中wBytes个字节转换成整数保存到bpDest指定的位置
 * @param[in]: source: 存放字节流的起始地址
 * @param[in]: bytes: 字节流的长度
 * @param[out]: dest: 指向转换后的整数的指针
 * @return:
 *      FALSE: 转换失败
 *      TRUE: 转换成功
 */
BOOL __fetch_long(INT32 *dest, const UINT8* source, INT32 bytes);

/*
 * @brief:
 *      将bpSource作为起始地址的字节流中wBytes个字节转换成整数保存到bpDest指定的位置
 * @param[in]: source: 存放字节流的起始地址
 * @param[in]: bytes: 字节流的长度
 * @param[out]: dest: 指向转换后的整数的指针
 * @return:
 *      FALSE: 转换失败
 *      TRUE: 转换成功
 */
BOOL __fetch_short(UINT16 *dest, const UINT8* source, INT32 bytes);

/*
 * @brief:
 *      将单个的十六进制ASCII字符转换成对应的数字
 * @param[in]: ch: 待转换的字符
 * @return:
 *      转换后对应的数字
 *      -1: ch不为十六进制ASCII字符
 */
INT32 hex2num(UINT8 ch);

/*
 * @brief:
 *     将一块内存区中的内容转换成十六进制字符串保存在指定的内存位置中
 * @param[in]: mem: 内存起始地址
 * @param[in]: count: 转换的内存区长度
 * @param[out]: buf: 保存十六进制字符串的缓冲区的地址
 * @return:
 *     无
 */
void mem2hex(const UINT8 *mem, UINT8 *buf, UINT32 count);

/*
 * @brief:
 *      将一块内存区中的内容转换成十六进制字符串。
 * @param: buf: 输入数据和输出字符串存放的起始地址
 * @param[in]: count: 转换的内存区长度
 * @return:
 *      无
 */
void mem2hexEx(UINT8 *buf, UINT32 count);

/*
 * @brief:
 *      将十六进制字符串转换成对应的内容保存在指定的内存位置中
 * @param[in]: buf: 指向十六进制字符串的缓冲区的指针
 * @param[in]: count: 转换的内存区长度
 * @param[out]: mem: 转换后数据的内存起始地址
 * @return:
 *      转换后数据的内存起始地址
 */
UINT8* hex2mem(const UINT8 *buf, UINT8 *mem, UINT32 count);

/*
 * @brief:
 *      将一块内存区中十六进制字符串转换成对应的内容保存在指定的内存位置中
 * @param[in]: buf: 输入字符串和输出数据存放的起始地址
 * @param[in]: count: 转换的字符串长度
 * @param[out]: outbuf: 输入字符串和输出数据存放的起始地址
 * @return:
 *     -1:转换失败
 *      0:转换成功
 */
INT32 hex2memEx(const UINT8 *buf, UINT8 *outbuf, UINT32 count);

/*
 * @brief:
 *      将十六进制字符串转换成对应32位整数
 * @param: ptr: 输入时为指向十六进制字符串首地址的指针，输出时为转换结束时的指针
 * @param[out]: intValue: 指向转换后的32位整数的指针
 * @return:
 *      转换的字符数
 */
UINT32 hex2int(const UINT8 **ptr, UINT32 *intValue);

/*
 * @brief:
 *      将十六进制字符串转换成对应64位整数
 * @param: ptr: 输入时为指向十六进制字符串首地址的指针，输出时为转换结束时的指针
 * @param[out]: intValue: 指向转换后的64位整数的指针
 * @return:
 *      转换的字符数
 */
UINT32 hex2int64(const UINT8 **ptr, UINT64 *intValue);

/*
 * @brief:
 *      将16位整数hValue转换成字节流保存在bpDest指向的缓冲中，转换后字节流的最大长度为wBytes
 * @param[in]: value: 待转换的整数
 * @param[in]: bytes: 字节流的最大长度，即<bpDest>指向的缓冲的最大长度
 * @param[out]: dest: 存放字节流的起始地址
 * @return:
 *      FALSE: 转换失败
 *      TRUE: 转换成功
 */
BOOL __store_short(INT16 value, UINT8 *dest, INT32 bytes);

/*
 * @brief:
 *      获取16进制字符串中'<'和'>'之间字符串，如果字符串首位不为'<'则返回0。
 * @param ubpPtr: 输入时为指向十六进制字符串首地址的指针，输出时为转换结束时的指针
 * @param[in]:len:输入字符串中'<'和'>'之间字符串的最大长度
 * @param[out]:strAddr: 指向转换后输出字符串首地址的指针
 * @return:
 *      输出字符串字符个数
 */
UINT32 hex2str(const UINT8 **ubpPtr, UINT8 **strAddr, UINT32 len);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _TAUTIL_H */
