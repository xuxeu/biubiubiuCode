/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:dbDR.h
 *@brief:
 *             <li>定义X86硬件断点相关的数据结构、宏定义及其相关接口声明</li>
 */
#ifndef _DB_DR_H_
#define _DB_DR_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头文件********************************/
#include "taTypes.h"
#include "taErrorDefine.h"

/************************宏定义********************************/
/* X86断点寄存器定义 */
#define    DR0  0
#define    DR1  1
#define    DR2  2
#define    DR3  3
#define    DR4  4
#define    DR5  5
#define    DR6  6
#define    DR7  7
#define    DR_ERROR 8

/*X86断点类型*/

/* 写硬件观察点(写 */
#define  DR_WRITE     0x01
    
/* 读硬件观察点(读) */
#define  DR_READ      0x03
    
/* 访问硬件观察点(读写) */
#define  DR_ACCESS    0x03
    
/* 执行硬件观察点 */
#define  DR_EXECUTE   0x0

/* X86断点宽度类型 */
#define DR_BYTE  0x00 /* 字节 */
#define DR_WORD  0x01 /* 字 */
#define DR_DWORD 0x03 /* 双字 */

/************************类型定义******************************/

/************************接口声明******************************/

/*
 * @brief
 *   读取DR0的内容
 *@return
 *   DR0的内容
 */
ulong_t taDebugGetDR0(void);

/*
 * @brief
 *   读取DR1的内容
 *@return
 *   DR1的内容
 */
ulong_t taDebugGetDR1(void);

/*
 * @brief
 *   读取DR2的内容
 *@return
 *   DR2的内容
 */
ulong_t taDebugGetDR2(void);

/*
* @brief
 *   读取DR3的内容
 *@return
 *   DR3的内容
 */
ulong_t taDebugGetDR3(void);

/*
 * @brief
 *   读取DR4的内容
 *@return
 *   DR4的内容
 */
ulong_t taDebugGetDR4(void);

/*
 * @brief
 *   读取DR5的内容
 *@return
 *   DR5的内容
 */
ulong_t taDebugGetDR5(void);

/*
 * @brief
 *   读取DR6的内容
 *@return
 *   DR6的内容
 */
ulong_t taDebugGetDR6(void);

/*
 * @brief
 *   读取DR7的内容
 *@return
 *   DR7的内容
 */
ulong_t taDebugGetDR7(void);

/*
 * @brief
 *    设置DR0的内容
 * @param[in] content: 内容
 * @return
 *      none
 */
void taDebugSetDR0(ulong_t content);

/*
 * @brief
 *    设置DR1的内容
 * @param[in] content: 内容
 * @return
 *      none
 */
void taDebugSetDR1(ulong_t content);

/*
 * @brief
 *    设置DR2的内容
 * @param[in] content: 内容
 * @return
 *      none
 */
void taDebugSetDR2(ulong_t content);

/*
 * @brief
 *    设置DR3的内容
 * @param[in] content: 内容
 * @return
 *      none
 */
void taDebugSetDR3(ulong_t content);

/*
 * @brief
 *    设置DR4的内容
 * @param[in] content: 内容
 * @return
 *      none
 */
void taDebugSetDR4(ulong_t content);

/*
 * @brief
 *    设置DR5的内容
 * @param[in] content: 内容
 * @return
 *      none
 */
void taDebugSetDR5(ulong_t content);

/*
 * @brief
 *    设置DR6的内容
 * @param[in] content: 内容
 * @return
 *      none
 */
void taDebugSetDR6(ulong_t content);

/*
 * @brief
 *    设置DR7的内容
 * @param[in] content: 内容
 * @return
 *      none
 */
void taDebugSetDR7(ulong_t content);

/*
 * @brief
 *   读取指定DR的内容
 * @param[in] dr: 调试寄存器的编号(DR0/DR1/DR2/DR3/DR6/DR7)
 * @return
 *    DR的内容,如果dr参数不可识别，返回0
 */
ulong_t taDebugGetDR(UINT32 dr);

/*
 * @brief
 *     设置指定DR的内容
 * @param[in] wIndex: 调试寄存器的编号(DR0/DR1/DR2/DR3/DR6/DR7)
 * @param[in] wContent: 内容
 * @return
 *    none
 */
void taDebugSetDR(UINT32 wIndex, ulong_t wContent);

/*
 * @brief
 *     检查指定的断点DR是否已设置/已使能
 * @param[in] wIndex: 调试寄存器的编号(DR0/DR1/DR2/DR3)
 * @return
 *     TA_OK: 断点已设置
 *     TA_INVALID_INPUT:寄存器编号非法
 *     DB_FAIL: 断点未设置
 */
T_TA_ReturnCode taDebugIsSetDR(UINT32 wIndex);

/*
* @brief
 *    将断点的类型转换成x86类型
 * @param[in]    wType: 数据类型
 *        DB_BP_HWWP_W ---写硬件观察点(写)
 *        DB_BP_HWWP_R ---读硬件观察点(读)
 *        DB_BP_HWWP_A ---访问硬件观察点(读写)
 *        DB_BP_HWBP ---执行硬件观察点
 * @return
 *    x86格的断点类型
 */
UINT32 taDebugTypeConvert(UINT32 wType);

/*
 * @brief
 *   将断点的数据宽度转换成x86格式的宽度
 * @param[in]    wSize: 数据宽度
 *                1 ---一个字节
 *                2 ---两个字节
 *                4 ---四个字节
 * @return
 *   x86格式的数据宽度
 */
UINT32 taDebugSizeConvert(UINT32 wSize);

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
UINT32 taDebugMatchDR(ulong_t vpAddr, UINT32 wType, UINT32 wSize);

/*
 * @brief
 *     使能指定的断点DR
 * @param[in] wIndex: 调试寄存器的编号(DR0/DR1/DR2/DR3)
 * @return
 *     none
 */
void taDebugEnableDR(UINT32 wIndex);

/*
 * @brief
 *     禁止指定的断点DR
 * @param[in] wIndex: 调试寄存器的编号(DR0/DR1/DR2/DR3)
 * @return
 *     none
 */
void taDebugDisableDR(UINT32 wIndex);

/*
* @brief
 *    查找空闲的断点DR
 * @return
 *    如果找到空闲的断点DR则返回idx，否则返回DR_ERROR
 */
UINT32 taDebugLookupDR(void);

/*
 * @brief:
 *     设置的硬件断点是否已经达到目标板支持的上限。
 * @param[in]:sessionID :调试会话ID
 * @param[in]:isAddr:判断指令断点和数据断点标示
 *         TRUE:  仅判断指令断点
 *         FALSE: 仅判断数据断点
 * @return:
 *     TRUE:    已经设置的硬件断点数量达到了上限
 *     FALSE:  有空闲的硬件断点
 */
BOOL taDebugHardBpIsFull(UINT32 sessionID, BOOL isAddr);

/*
 * @brief
 *     在地址addr处设置一个硬件断点指令
 * @param[in] idx: 调试寄存器的编号(DR0/DR1/DR2/DR3)
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
void taDebugInsertDR(UINT32 wIndex, ulong_t vpAddr, UINT32 wType, UINT32 wSize);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _DB_DR_H_ */
