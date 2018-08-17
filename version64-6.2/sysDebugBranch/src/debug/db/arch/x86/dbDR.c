/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * 修改历史:
 * 2013-06-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbDR.c
 * @brief:
 *             <li>操作硬件断点相关的接口</li>
 */

/************************头 文 件*****************************/
#include "dbDR.h"
#include "dbBreakpoint.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief
 *    读取指定DR的内容
 * @param[in] dr: 调试寄存器的编号(DR0/DR1/DR2/DR3/DR6/DR7)
 * @return        
 *    DR的内容,如果dr参数不可识别，返回0
 */
ulong_t taDebugGetDR(UINT32 dr)
{
    ulong_t drContent = 0;

    /* 根据寄存器编号获取值 */
    switch (dr)
    {
        case DR0:   
        { 
            drContent = taDebugGetDR0();
            break;
        }
        case DR1:    
        {
            drContent = taDebugGetDR1();
            break;
        }
        case DR2:    
        {
            drContent = taDebugGetDR2();
            break;
        }
        case DR3:    
        {
            drContent = taDebugGetDR3();
            break;
        }
        case DR6:    
        {
            drContent = taDebugGetDR6();
            break;
        }
        case DR7:   
        {
            drContent = taDebugGetDR7();
            break;
        }
        default:
        {
            drContent = 0;
            break;
        }
    }

    return (drContent);
}

/*
 * @brief
 *     设置指定DR的内容
 * @param[in] wIndex: 调试寄存器的编号(DR0/DR1/DR2/DR3/DR6/DR7)
 * @param[in] wContent: 内容
 * @return        
 *    none
 */
void taDebugSetDR(UINT32 wIndex, ulong_t wContent)
{
    /* 根据寄存器编号设置寄存器值 */
    switch (wIndex)
    {
        case DR0:    
        {    
            taDebugSetDR0(wContent);    
            break;
        }
        case DR1:
        {
            taDebugSetDR1(wContent);    
            break;
        }
        case DR2:
        {
            taDebugSetDR2(wContent);    
            break;
        }
        case DR3:
        {
            taDebugSetDR3(wContent);    
            break;
        }
        case DR6:
        {
            taDebugSetDR6(wContent);
            break;
        }
        case DR7:
        {
            taDebugSetDR7(wContent);
            break;
        }
        default:
            break;
    }
}

/*
 * @brief
 *    检查指定的断点DR是否已设置/已使能
 * @param[in] wIndex: 调试寄存器的编号(DR0/DR1/DR2/DR3)
 * @return        
 *    TA_OK: 断点已设置
 *    TA_INVALID_INPUT:寄存器编号非法
 *    DB_FAIL: 断点未设置
 */
T_TA_ReturnCode taDebugIsSetDR(UINT32 wIndex)
{
	BOOL isEnable = FALSE;
	UINT32 dr7 = 0;
	
    /* 检查寄存器编号是否合法 */
    if (wIndex > DR3)
    {
        return (TA_INVALID_INPUT);
    }

    /* 首先获得DR7的内容 */
    dr7 = taDebugGetDR(DR7);

    /* 如果该断点被使能则返回假 */
    isEnable = dr7 & ((UINT32)0x3 << (wIndex * 2));
    if (isEnable != FALSE)
    {
        /* 该断点已被使能 */
        return (TA_OK);
    }
    
    return (TA_FAIL);
}

/*
* @brief
 *    将断点的类型转换成x86类型
 * @param[in] wType: 数据类型
 *        DB_BP_HWWP_W ---写硬件观察点(写)
 *        DB_BP_HWWP_R ---读硬件观察点(读)
 *        DB_BP_HWWP_A ---访问硬件观察点(读写)
 *        DB_BP_HWBP ---执行硬件观察点
 * @return        
 *    x86格的断点类型
 */
UINT32 taDebugTypeConvert(UINT32 wType)
{
    UINT32 ret = DR_WRITE;

    /* 根据断点宽度，将断点类型转换成为X86表示格式 */
    switch (wType)
    {
        case DB_BP_HWWP_W:
        {
            ret =  DR_WRITE;
            break;
        }
        case DB_BP_HWWP_R:
        {
            ret =  DR_READ;
            break;
        }
        case DB_BP_HWWP_A:
        {
            ret =  DR_ACCESS;
            break;
        }
        case DB_BP_HWBP:
        {
            ret =  DR_EXECUTE;
            break;
        }
        default:
            ret =  DR_WRITE;
            break;
    }

    return (ret);
}

/*
 * @brief
 *   将断点的数据宽度转换成x86格式的宽度
 * @param[in] wSize: 数据宽度
 *                1 ---一个字节
 *                2 ---两个字节
 *                4 ---四个字节
 * @return
 *   x86格式的数据宽度
 */
UINT32 taDebugSizeConvert(UINT32 wSize)
{
    UINT32 width = 1;

    /* 根据断点宽度，将断点宽度转换成为X86表示格式 */
    switch (wSize)
    {
        case 1:
        {
            width = DR_BYTE;
            break;
        }
        case 2:
        {
            width = DR_WORD;
            break;
        }
        case 4:
        {
            width = DR_DWORD;
            break;
        }
        default:
        {
            width = DR_BYTE;
            break;
        }
    }

    return (width);
}

/*
 * @brief
 *      查找是否存在与指定的地址、访问类型、长度全部一致的断点
 * @param[in] vpAddr: 设置断点指令的地址
 * @param[in] wType: 断点类型
 *                DR_WRITE ---写硬件观察点(写)
 *                DR_READ  ---读硬件观察点(读)
 *                DR_ACCESS---访问硬件观察点(读写)
 *                DR_EXECUTE  执行硬件观察点
 * @param[in] wSize: 断点的宽度
 *                DR_BYTE  ---字节
 *                DR_WORD  ---字
 *                DR_DWORD ---双字
 * @return        
 *      如果成功返回匹配的断点DR，否则返回DR_ERROR
 */
UINT32 taDebugMatchDR(ulong_t vpAddr, UINT32 wType, UINT32 wSize)
{
    UINT32 idx = 0;
    ulong_t dr7 = 0;
    UINT32 flag = 0;
    UINT32 dummy = 0;
    T_TA_ReturnCode ret = TA_OK;
    ulong_t address = 0;

    /* 遍历断点寄存器 */
    for (idx = DR0; idx < DR4; idx++)
    {
        /* 首先检查断点DR是否繁忙以及地址是否匹配 */
    	ret = taDebugIsSetDR(idx);
        address = taDebugGetDR(idx);
        if( (TA_OK == ret) && (address == vpAddr) )
        {
            /* 获得DR7的内容 */
            dr7 = taDebugGetDR(DR7);

            /* 获得指定DR的标记字段 */
            flag = ((dr7 >> (idx * 4 + 16)) & 0x0f);

            /* 制造一个假的标记字段 */
            dummy = 0;
            dummy |= wType << 0;
            dummy |= wSize << 2;

            /* 检查访问类型以及数据宽度是否一致 */
            if(dummy == flag)
            {
                return (idx);
            }
        }
    }
    
    return (DR_ERROR);
}

/*
 * @brief
 *     使能指定的断点DR
 * @param[in] wIndex: 调试寄存器的编号(DR0/DR1/DR2/DR3)
 * @return        
 *     none
 */
void taDebugEnableDR(UINT32 wIndex)
{
    UINT32 dr7 = 0;

	/* 检查寄存器编号是否合法*/
	if (wIndex > DR3)
	{
	   return;
	}

    /* 首先获得DR7的内容 */
    dr7 = taDebugGetDR(DR7);

    /* 允许指定DR的断点 */
    dr7 = dr7 | (0x3U << (wIndex * 2));

    /* 允许所有DR的断点 */
    dr7 |= 0x300;

    /* 重新设置DR7的内容 */
    taDebugSetDR(DR7,dr7);
    taDebugSetDR(DR6,0);
}

/*
 * @brief
 *     禁止指定的断点DR
 * @param[in] wIndex: 调试寄存器的编号(DR0/DR1/DR2/DR3)
 * @return        
 *     none
 */
void taDebugDisableDR(UINT32 wIndex)
{
	UINT32 dr7 = 0;

    /* 检查寄存器编号是否合法*/
    if (wIndex > DR3)
    {
        return;
    }

    /* 首先获得DR7的内容 */
    dr7 = taDebugGetDR(DR7);

    /* 禁止指定DR的局部和全局断点 */
    dr7 = dr7 & (~(0x3U << (wIndex * 2)));

    /* 重新设置DR7的内容 */
    taDebugSetDR(DR7,dr7);
}

/*
* @brief
 *    查找空闲的断点DR
 * @return        
 *    如果找到空闲的断点DR则返回idx，否则返回DR_ERROR
 */
UINT32 taDebugLookupDR(void)
{
    UINT32 idx = DR0;

    for (idx = DR0; idx < DR4; idx++)
    {
        if (taDebugIsSetDR(idx) == TA_FAIL)
        {
            return (idx);
        }
    }
    
    return (DR_ERROR);
}

/*
 * @brief:
 *     设置的硬件断点是否已经达到目标板支持的上限
 * @param[in] sessionID :调试会话ID
 * @param[in] isAddr:判断指令断点和数据断点标识
 *         TRUE:  仅判断指令断点
 *         FALSE: 仅判断数据断点
 * @return:
 *     TRUE:已经设置的硬件断点数量达到了上限
 *     FALSE:有空闲的硬件断点
 */
BOOL taDebugHardBpIsFull(UINT32 sessionID, BOOL isAddr)
{
	UINT32 idx = DR0;

	idx = taDebugLookupDR();
	if(DR_ERROR != idx)
	{
		return (FALSE);
	}
	
	return (TRUE);
}

/*
 * @brief
 *     在地址addr处设置一个硬件断点指令
 * @param[in] wIndex: 调试寄存器的编号(DR0/DR1/DR2/DR3)
 * @param[in] vpAddr: 设置断点指令的地址
 * @param[in] wType: 断点类型
 *                DR_WRITE  ---写硬件观察点(写)
 *                DR_READ   ---读硬件观察点(读)
 *                DR_ACCESS ---访问硬件观察点(读写)
 *                DR_EXECUTE 执行硬件观察点
 * @param[in] wSize: 断点的宽度
 *                DR_BYTE  ---字节
 *                DR_WORD  ---字
 *                DR_DWORD ---双字
 * @return        
 *     none
 */
void taDebugInsertDR(UINT32 wIndex, ulong_t vpAddr, UINT32 wType, UINT32 wSize)
{
    /* 首先获得DR7的内容 */
    UINT32 dr7 = taDebugGetDR(DR7);

    /* 检查寄存器编号是否合法*/
    if (wIndex > DR3)
    {
        return;
    }

    /* 清除DR7中的断点类型和长度 */
    dr7 = (dr7 & (~(0xfU << (wIndex * 4 + 16))));

    /* 设置断点的数据写类型 */
    dr7 = (dr7 | (wType  << (wIndex * 4 + 16 + 0)));

    /* 设置断点的宽度 */
    dr7 = (dr7 | (wSize  << (wIndex * 4 + 16 + 2)));

    /* 重新设置DR7的内容 */
    taDebugSetDR(DR7,dr7);

    /* 设置断点地址到指定的DR */
    taDebugSetDR(wIndex,vpAddr);
}
