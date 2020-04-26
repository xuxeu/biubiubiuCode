/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-07-01         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbDR.c
 * @brief:
 *             <li>PPC体系结构的硬件断点操作</li>
 */
/************************头 文 件******************************/
#include "dbDR.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *     设置数据断点
 * @param[in]:uwAddr:数据断点地址
 * @return:
 *    无
 */
void taDebugSetDataBreakpoint(UINT32 addr)
{
    /* 设置DABR寄存器的值 */
    taDebugSetDABR(addr);
}

/*
 * @brief:
 *     设置指令断点
 * @param[in]:uwAddr:指令断点地址
 * @return:
 *    无
 */
void taDebugSetAddressBreakpoint(UINT32 addr)
{
    /* 设置IABR寄存器的值 */
    taDebugSetIABR(addr);
}

/*
 * @brief:
 *     获取数据断点地址
 * @return:
 *    数据断点地址
 */
UINT32 taDebugGetDataBreakpoint(void)
{
    /* 读取DABR寄存器的值  */
    return (taDebugGetDABR());
}

/*
 * @brief:
 *    获取指令断点地址
 * @return:
 *    指令断点地址
 */
UINT32 taDebugGetAddressBreakpoint(void)
{
    /* 读取IABR寄存器的值 */
    return (taDebugGetIABR());
}

/*
 * @brief:
 *     获取DAR寄存器值
 * @return:
 *    DAR寄存器值
 */
UINT32 taDebugGetDARValue(void)
{
    /* 读取DAR寄存器的值 */
    return (taDebugGetDAR());
}

/*
 * @brief:
 *     设置的硬件断点是否已经达到目标板支持的上限
 * @param[in]:sessionID :调试会话ID
 * @param[in]:isAddr:判断指令断点和数据断点标示
 *         TRUE:  仅判断指令断点
 *         FALSE: 仅判断数据断点
 * @return:
 *     TA_OK:    已经设置的硬件断点数量达到了上限
 *     TA_FAIL:  有空闲的硬件断点
 */
BOOL taDebugHardBpIsFull(UINT32 sessionID, BOOL isAddr)
{
    UINT32 address = 0;

    if (TRUE == isAddr)
    {
        /* 获取地址断点寄存器IABR的值 */
        address = taDebugGetAddressBreakpoint();
    }
    else
    {
        /* 获取数据断点寄存器DABR的值 */
        address = taDebugGetDataBreakpoint();
    }

    return (0 == address) ? FALSE : TRUE;
}
