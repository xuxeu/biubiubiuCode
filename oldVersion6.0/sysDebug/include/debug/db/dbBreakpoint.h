/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:dbBreakpoint.h
 *@brief:
 *             <li>调试断点管理定义</li>
 */
#ifndef _DB_BREAKPOINT_H
#define _DB_BREAKPOINT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taErrorDefine.h"
#include "taTypes.h"
#include "dbContext.h"
#include "sdaMain.h"

/************************宏定义********************************/

/* 软件断点个数 */
#define	CONFIG_DB_BREAK_POINT (500)

/************************类型定义******************************/

/* 断点枚举类型定义*/
typedef enum
{
    /*常规软件断点*/
    DB_BP_NORMAL = 0,

    /*硬件断点*/
    DB_BP_HWBP,

    /*硬件写观察点*/
    DB_BP_HWWP_W,

    /*硬件读观察点*/
    DB_BP_HWWP_R,

    /*硬件访问观察点（读写）*/
    DB_BP_HWWP_A,

    /* 不合法类型*/
    DB_BP_INVALID
} T_DB_BpType;

/*断点分类*/
typedef enum
{
    BPClass_NORMAL,  /* 一般断点分类 */
    BPClass_TRACE    /* 跟踪点 */
} T_DB_BpClass;

/* 对于断点本地缓冲的结构体定义 */
typedef struct T_DB_BreakpointStruct
{
    /*当作为链表使用时，表示下一个不同地址的断点*/
    struct T_DB_BreakpointStruct *next_addr;

    /*和当前断点地址相同的下一个断点*/
    struct T_DB_BreakpointStruct *same_addr;

    /* 断点所在的会话ID */
    UINT32 sessionID;

    /* 断点有效对象ID */
    UINT32 objID;

    /* 断点逻辑地址 */
    UINT32 logicAddr;

    /*断点类型*/
    T_DB_BpType bpType;

    /* 断点分类 */
    T_DB_BpClass bpClass;

    /* 断点宽度 */
    INT32 breakSize;

    /*断点处原来内存单元的内容*/
    UINT8  shadow[DB_ARCH_BREAK_INST_LEN];
} T_DB_Breakpoint;

/************************接口声明******************************/

/*
 * @brief:
 *     初始化断点管理数据结构
 * @return:
 *     无
 */
void taDebugBreakPointInit(UINT32 bpNumber);

/*
 * @brief:
 *    查找匹配的断点信息(会话ID + objID + logicAddr全部要匹配)
 * @param[in]:sessionID: 调试会话ID
 * @param[in]:objID: 被调试对象ID
 * @param[in]:logicAddr: 断点逻辑地址
 * @param[out]:breakpointList: 存储查到的符合条件的断点列表,如果逻辑地址相同但sessionID,taskID不同，则返回第一个匹配的断点
 * @return:
 *         TA_OK:找到有匹配的断点
 *         TA_DB_BREAKPOINT_NOTFOUND:没有找到断点,sessionID,taskID,addr全部不同
 *         TA_DB_BREAKPOINT_DIFFER_ID:没有找到完全匹配的断点,addr相同;sessionID,taskID不同
 */
T_TA_ReturnCode taDebugLookUpBP(UINT32 sessionID, UINT32 objID, UINT32 logicAddr, T_DB_Breakpoint **breakpointList);

/*
 * @brief:
 *        插入一个断点(软件断点和硬件断点)，如果已经存在逻辑地址相同的断点则插入相同地址链表否则插入m_breakManageInfo.bp_busy链头
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:breakpoint: 断点信息
 * @return:
 *         TA_OK                   :执行成功
 *         TA_DB_BREAKPOINT_FULL   :断点打满了
 *         TA_INVALID_PARAM        :非法的参数
 *         TA_DB_BREAKPOINT_EXIST  :相同会话,相同任务的相同地址已存在一个断点
 */
T_TA_ReturnCode taDebugInsertBP(T_DB_Info *debugInfo, T_DB_Breakpoint *breakpoint);

/*
 * @brief:
 *        删除一个断点(参数必须完全匹配)
 * @param[in]:debugInfo :调试会话信息
 * @param[in]:logicAddr :断点地址
 * @param[in]:breakpointType :断点类型
 * @param[in]:breakpointClass :断点分类
 * @param[in]:size :断点宽度
 * @return:
 *         TA_OK                    :执行成功
 *         TA_FAIL                    :删除断点失败
 *         TA_DB_BREAKPOINT_EMPTY      :没有打任何断点
 *         TA_INVALID_PARAM         :非法的参数
 *         TA_DB_BREAKPOINT_NOTFOUND   :没找到断点
 */
T_TA_ReturnCode taDebugRemoveBP(T_DB_Info *debugInfo, UINT32 logicAddr, T_DB_BpType breakpointType, T_DB_BpClass breakpointClass, UINT32 size);

/*
 * @brief:
 *      向地址addr处插入一个断点指令，同时将addr处原有的内容保存到shadow中
 * @param[in]:ctxId: 上下文ID
 * @param[in]:addr: 插入断点指令的地址
 * @param[in]:shadow: 用来保存断点处原有内容的缓存
 * @return:
 *        TA_OK: 操作成功
 *        TA_FAIL: 操作失败
 */
T_TA_ReturnCode taDebugInsertSoftBp(UINT32 ctxId,  UINT32 addr, UINT8 *shadow);

/*
 * @brief:
 *        从地址addr处删除一个断点指令，同时恢复addr处原有的内容
 * @param[in]:ctxId: 上下文ID
 * @param[in]:addr: 插入断点指令的地址
 * @param[in]:shadow: 用来保存断点处原有内容的缓存
 * @return:
 *        TA_OK: 操作成功
 *        TA_FAIL:  操作失败
 */
T_TA_ReturnCode taDebugRemoveSoftBp(UINT32 ctxId, UINT32 addr, const UINT8 *shadow);

/*
 * @brief:
 *      删除当前调试会话所有断点
 * @param[in] debugInfo: 调试会话
 * @return:
 *      无
 */
void taDebugRemoveAllBp(T_DB_Info *debugInfo);

/*
 * @brief:
 *      禁止所有软件断点
 * @return:
 *      无
 */
void taDebugBpAllDisable(void);

/*
 * @brief:
 *      使能所有软件断点
 * @return:
 *      无
 */
void taDebugBpAllEnable(void);

/*
 * @brief:
 *     检查内存地址存放的是否是断点指令
 * @return:
 *     FALSE: 非断点指令
 *     TRUE:断点指令
 */
BOOL taDebugBpInstructCheck(UINT8 *addr);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _DB_BREAKPOINT_H */
