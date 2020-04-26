/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-10-23         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:taMemProbeLib.h
 *@brief:
 *             <li>内存探测相关函数声明</li>
 */
#ifndef _TA_MEM_PROBE_LIB_H
#define _TA_MEM_PROBE_LIB_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头 文 件******************************/
#include "ta.h"

/************************宏定义*******************************/

/************************类型定义*****************************/

/************************接口声明*****************************/

/**
 * @brief:
 *     内存探测初始化
 * @param[in]:_func_excBaseHook: 异常HOOK指针，异常产生时操作系统调用该HOOK
 * @return:
 * 	   OK：初始化成功
 * 	   ERROR：初始化失败
 */
STATUS taMemProbeInit(FUNCPTR *_func_excBaseHook);

/**
 * @brief:
 *     内存探测体系结构相关初始化，挂接内存访问非法异常处理函数
 * @param[in]:_func_excBaseHook: 异常HOOK指针，异常产生时操作系统调用该HOOK
 * @return:
 * 	   OK：初始化成功
 */
STATUS taMemArchProbeInit(FUNCPTR *_func_excBaseHook);

/*
 * @brief:
 *     内存探测，探测内存是否可读写，如果不可读写将产生总线错误，如果访问模式为TA_READ，数据将
 *     拷贝到pVal地址处，如果访问模式为TA_WRITE，将把pVal指向的值写入pAdrs地址处
 * @param[in]:pAdrs: 内存地址
 * @param[in]:mode:访问模式，TA_READ或者 TA_WRITE
 * @param[out]:length: 内存访问长度
 * @param[in]:pVal: TA_WRITE模式pVal为源地址，TA_READ模式为 pVal为目的地址
 *     OK：读写内存成功
 *     ERROR：发生非法地址访问异常
 */
STATUS taMemCommonProbe(char *pAdrs, int  mode, int length, char *pVal);

/*
 * @brief:
 *     内存探测，探测内存是否可读写，如果不可读写将产生总线错误，如果访问模式为TA_READ，数据将
 *     拷贝到pVal地址处，如果访问模式为TA_WRITE，将把pVal指向的值写入pAdrs地址处
 * @param[in]:pAdrs: 访问的内存地址
 * @param[in]:mode: 内存访问模式，TA_READ或者 TA_WRITE
 * @param[out]:length: 内存访问长度
 * @param[in]:pVal: TA_WRITE模式pVal为源地址，TA_READ模式为 pVal为目的地址
 *     OK：内存可访问
 *     ERROR：内存不可访问
 */
STATUS taMemProbe(char *pAdrs, int  mode, int length, char *pVal);

/*
 * @brief:
 *      内存探测，探测内存是否可读写，如果不可读写将产生总线错误
 * @param[out]:length: 内存访问长度
 * @param[in]:src: 源地址
 * @param[in]:dest: 目的地址
 *      OK：读写内存成功
 *      ERROR：发生非法地址访问异常
 */
STATUS taMemProbeSup(int length, char *src, char* dest);

/*
 * @brief:
 *     测试一段内存范围地址是否可访问
 * @param[in]:pAdrs: 访问的内存地址
 * @param[out]:size: 内存访问长度
 * @param[in]:mode: 内存访问模式，TA_READ或者 TA_WRITE
 * @param[in]:width: 访问宽度
 *     TA_OK：内存可访问
 *     TA_INVALID_INPUT：内存访问范围溢出
 *     TA_ERR_MEM_ACCESS：内存访问失败
 */
UINT32 taMemTest(char *addr, UINT32 size, int mode, UINT32 width);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _TA_MEM_PROBE_LIB_H */
