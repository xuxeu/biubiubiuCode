
/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taCpuLib.c
 * @brief:
 *             <li>CPU相关初始化</li>
 */

/************************头 文 件******************************/
#include "taTypes.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/
#define CONFIG_INTERRUPT_NUMBER_OF_VECTORS 255

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/
void* taVectorTable[256];

/************************函数实现*****************************/

/*
 * @brief:
 *      异常处理空函数
 * @param[in]: vector: 异常号
 * @return:
 *     无
 */
void taNullRawHandler(UINT32 vector)
{
	while(1);
}

/*
 * @brief:
 *     安装异常处理函数到IDT表中
 * @param[in] vector :异常号
 * @param[in] vectorHandle :异常处理函数
 * @return:
 *     无
 */
void taInstallVectorHandle(int vector, void *vectorHandle)
{
    if(vector > CONFIG_INTERRUPT_NUMBER_OF_VECTORS)
    {
        return;
    }
    
	/* 设置向量表中<vector>指定的处理程序 */
    taVectorTable[vector] = vectorHandle;
}

/*
 * @brief: 
 *    初始化向量表，使所有的向量的缺省处理函数都是nullRawHandler
 * @return: 
 *    无
 */
void taInitVectorTable(void)
{
    UINT32 vector = 0;
    
    for (vector = 0; vector < 256; vector++)
    {
        /* 设置向量表中<vector>指定的处理程序为nullRawHandler */
        taVectorTable[vector] = taNullRawHandler;
    }
}

/*
 * @brief:
 *      Arch相关初始化
 * @return:
 *      无
 */
void taArchInit(void)
{
	/* 初始化向量表 */
	taInitVectorTable();
}
