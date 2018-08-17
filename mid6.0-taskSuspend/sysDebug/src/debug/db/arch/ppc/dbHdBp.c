/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-07-020        彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbHdBp.c
 * @brief:
 *             <li>PPC体系结构硬件断点设置和删除的实现</li>
 */

/************************头 文 件******************************/
#include "taTypes.h"
#include "dbDR.h"
#include "dbBreakpoint.h"
#include "dbContext.h"
#include "sdaMain.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *     插入硬件断点，硬件断点分为数据断点和指令断点
 * @param[in]: debugInfo: 调试会话信息
 * @param[in]: addr: 断点地址
 * @param[in]: type: 断点类型
 * @param[in]: size: 断点宽度
 * @return:
 *    TA_OK: 插入成功
 *    TA_DB_BREAKPOINT_FULL:插入失败(断点打满了)
 */
T_TA_ReturnCode taDebugInsertHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
    UINT32 addrValue = 0;
    T_TA_ReturnCode ret = TA_FAIL;

    /* 断点类型为指令断点 */
    if (DB_BP_HWBP == type)
    {
        /* 检查是否存在空闲的指令断点 */
        ret = taDebugHardBpIsFull(debugInfo->sessionID, TRUE);
        if (TA_OK == ret)
        {
            return (TA_DB_BREAKPOINT_FULL);
        }

        /* 设置断点地址 */
        addrValue = addr;

        /* 检查MSR寄存器的IR位 */
        if ( 0 != (debugInfo->context->registers[PS] & DB_PPC_MSR_IR))
        {
        	/* 设置IABR寄存器的TE位 */
            addrValue |= 1;
        }

        /* 设置IABR寄存器的BE位 */
        addrValue |= 2;

        /* 把addrValue值写入IABR寄存器 */
        taDebugSetAddressBreakpoint(addrValue);
    }
    else
    {
        /* 检查是否存在空闲的数据断点 */
        ret = taDebugHardBpIsFull(debugInfo->sessionID, FALSE);
        if (TA_OK == ret)
        {
            return (TA_DB_BREAKPOINT_FULL);
        }

        /* 设置断点地址,清空低3位 */
        addrValue = ( addr  & (~7) );

        /* 检查MSR寄存器的DR位 */
        if ( 0 != (debugInfo->context->registers[PS] & DB_PPC_MSR_DR))
        {
        	 /* 设置DABR寄存器的BT位 */
            addrValue |= 4;
        }

        /* 断点类型为写观察点 */
        if (DB_BP_HWWP_W == type)
        {
        	/* 设置DABR寄存器的DW位 */
            addrValue |= 2;
        }

        /* 断点类型为读观察点 */
        if (DB_BP_HWWP_R == type)
        {
        	/* 设置DABR寄存器的DR位 */
            addrValue |= 1;
        }

        /* 断点类型为读写观察点 */
        if (DB_BP_HWWP_A == type)
        {
        	/* 设置DABR寄存器的DW位和DR位 */
            addrValue |= 3;
        }

        /* 把addrValue值写入DABR寄存器，设置DABR寄存器 */
	    taDebugSetDataBreakpoint(addrValue);
    }

    return (TA_OK);
}

/*
 * @brief:
 *     删除硬件断点
 * @param[in]: debugInfo: 调试会话信息
 * @param[in]: addr: 断点逻辑地址
 * @param[in]: type: 断点类型
 * @param[in]: size: 断点宽度
 * @return:
 *    TA_OK:删除成功
 *    TA_DB_BREAKPOINT_NOTFOUND:断点找不到
 */
T_TA_ReturnCode taDebugRemoveHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
    UINT32 addrValue = 0;
    UINT32 delAddr = 0;

    /* 断点类型为指令断点 */
    if (DB_BP_HWBP == type)
    {
        /* 获取指令断点地址 */
        addrValue = taDebugGetAddressBreakpoint();

        /* 清除原有指令断点addrValue的低2位 */
        addrValue &= ~0x3;

        /* 获取要删除断点 */
        delAddr = addr & (~0x3);

        /* 比较地址,如果地址不同,则返回TA_DB_BREAKPOINT_NOTFOUND */
        if ( addrValue != delAddr )
        {
            return (TA_DB_BREAKPOINT_NOTFOUND);
        }

        /* 要删除断点与原断点相同,则设置IABR寄存器为0 */
	    taDebugSetAddressBreakpoint(0);
    }
    else
    {
        /* 获取当前存在的数据断点 */
        addrValue = dbGetDataBreakpoint();

        /* 清除原有数据断点addrValue的低3位 */
        addrValue &= ~0x7;

        /* 获取要删除断点地址 */
        delAddr = addr & (~0x7);
        if (addrValue != delAddr )
        {
            return (TA_DB_BREAKPOINT_NOTFOUND);
        }

        /* 要删除断点与原断点相同,则设置DABR寄存器为0 */
	    taDebugSetDataBreakpoint(0);
    }

    return (TA_OK);
}
