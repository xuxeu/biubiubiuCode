/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * 修改历史:
 * 2013-11-08         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:taMemProbeLib.c
 * @brief:
 *             <li>该模块提供体系结构无关的内存探测访问接口</li>
 */

/************************头 文 件*****************************/
#include "taMemProbeLib.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/* 内存探测初始化标志 */
static BOOL taMemProbeInitialized = FALSE;

/************************全局变量*****************************/

/************************函数实现*****************************/

/**
 * @brief:
 *     内存探测初始化
 * @param[in]:_func_excBaseHook: 异常HOOK指针，异常产生时操作系统调用该HOOK
 * @return:
 * 	   OK：初始化成功
 * 	   ERROR：初始化失败
 */
STATUS taMemProbeInit(FUNCPTR *_func_excBaseHook)
{
    STATUS status = OK;

    if (taMemProbeInitialized == FALSE)
    {
        status = taMemArchProbeInit(_func_excBaseHook);
        taMemProbeInitialized = (status == OK)?TRUE:FALSE;
    }

    return (status);
}

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
#ifdef __X86__
STATUS taMemCommonProbe(char *pAdrs, int  mode, int length, char *pVal)
{
    /* 验证是否初始化 */
    if (taMemProbeInitialized == FALSE)
    {
        return (OK);
    }

    /* 请求内存访问 */
    if (mode == TA_READ)
    {
        return (taMemProbeSup(length, pAdrs, pVal));
    }
    else
    {
        return (taMemProbeSup(length, pVal, pAdrs));
    }
}
#endif
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
STATUS taMemProbe(char *pAdrs, int  mode, int length, char *pVal)
{
#ifdef __X86__
#ifdef CONFIG_TA_LP64

    return taMemArchProbe((void *)pAdrs, mode, length, (void *)pVal);
#else
    return taMemCommonProbe((void *)pAdrs, mode, length, (void *)pVal);
#endif
#endif
#ifdef __ARM__
    return taMemArchProbe((void *)pAdrs, mode, length, (void *)pVal);
    
    //return (OK);
#endif
}

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
UINT32 taMemTest(char *addr, UINT32 size, int mode, UINT32 width)
{
    UINT32 dummy = 0;
    char *memAddr = 0;
    BOOL preserve = FALSE;


    if (size == 0)
    {
    	return (TA_OK);
    }

    if (width == 0)
    {
    	width = 1;
    }

    /* 检查内存访问范围是否溢出 */
    if (addr > addr + size)
    {
    	return (TA_INVALID_INPUT);
    }

    /* 内存写探测，需要保存原来的内容 */
    preserve = (mode == TA_WRITE)?TRUE:FALSE;

    /* 探测需访问的内存范围是否都可访问 */
	for (memAddr = addr; memAddr < addr + size; memAddr += width)
	{
		if ((preserve && (taMemProbe(memAddr, TA_READ, width,
			    		   (char *)&dummy) != OK)) || 
			(taMemProbe(memAddr, mode, width, (char *)&dummy) != OK))
	    {
			return (TA_ERR_MEM_ACCESS);
	    }
	}

    return (TA_OK);
}
