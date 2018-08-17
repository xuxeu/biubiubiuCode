/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  dbHdBp.c
 * @brief:
 *             <li>硬件断点插入、删除操作接口实现</li>
 */
/************************头文件******************************/
#include "ta.h"
#include "dbDR.h"
#include "taErrorDefine.h"
#include "dbBreakpoint.h"

/************************宏定义******************************/

/************************类型定义****************************/

/************************全局变量****************************/

/************************前向声明****************************/

/************************模块变量****************************/

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
    UINT32 idx = DR_ERROR;
    UINT32 drsize = 0;
    UINT32 drtype = DR_WRITE;

    /* 查询硬件断点是否打满 */
    if ((idx = taDebugLookupDR()) == DR_ERROR)
    {
        /* 硬断点已经打满 */
        return (TA_DB_BREAKPOINT_FULL);
    }

    /* 转换类型为X86断点标准 */
    drtype = taDebugTypeConvert(type);

    /* 转换宽度为X86标准 */
    drsize = taDebugSizeConvert(size);

    /* 根据输入的断点类型插入一个硬件断点 */
    taDebugInsertDR(idx, addr, drtype, drsize);

    /* 使能指定的断点 */
    taDebugEnableDR(idx);

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
    UINT32 drsize = 0;
    UINT32 drtype = DR_WRITE;
    UINT32 idx = DR_ERROR;

    /* 转换类型为X86断点标准 */
    drtype = taDebugTypeConvert(type);

    /* 转换宽度为X86标准 */
    drsize = taDebugSizeConvert(size);

    /* 查看是否有匹配的断点 */
    idx = taDebugMatchDR(addr, drtype, drsize);
    if (DR_ERROR != idx)
    {
        /* 存在匹配的断点,则调用taDebugDisableDR禁止断点 */
        taDebugDisableDR(idx);

        /* 返回操作成功 */
        return (TA_OK);
    }

    /* 不存在匹配的断点,则返回DB_BREAKPOINT_NOTFOUND */
    return (TA_DB_BREAKPOINT_NOTFOUND);
}
