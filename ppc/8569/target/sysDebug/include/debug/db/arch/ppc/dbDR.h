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
 *             <li>PPC体系结构的硬件断点操作定义</li>
 */
#ifndef _DB_DR_H
#define _DB_DR_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头文件********************************/
#include "taErrorDefine.h"
#include "taTypes.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************接口声明******************************/

/*
 * @brief:
 *      设置DABR寄存器
 * @param[in]:addr: 待设地址
 * @return:
 *      无
 */
void taDebugSetDABR(UINT32 addr);

/*
 * @brief:
 *      设置IABR寄存器
 * @param[in]:addr: 待设地址
 * @return:
 *      无
 */
void taDebugSetIABR(UINT32 addr);

/*
 * @brief:
 *      获取DABR寄存器的值
 * @return:
 *      DABR寄存器的值
 */
UINT32 taDebugGetDABR(void);

/*
 * @brief:
 *      获取IABR寄存器的值
 * @return:
 *      IABR寄存器的值
 */
UINT32 taDebugGetIABR(void);

/*
 * @brief:
 *      获取DAR寄存器的值
 * @return:
 *      DAR寄存器值
 */
UINT32 taDebugGetDAR(void);

/*
 * @brief:
 *     设置的硬件断点是否已经达到目标板支持的上限。
 * @param[in]:sessionID :调试会话ID
 * @param[in]:isAddr:判断指令断点和数据断点标示
 *         TRUE:  仅判断指令断点
 *         FALSE: 仅判断数据断点
 * @return:
 *     TA_OK:    已经设置的硬件断点数量达到了上限
 *     TA_FAIL:  有空闲的硬件断点
 */
T_TA_ReturnCode taDebugHardBpIsFull(UINT32 sessionID, BOOL isAddr);

/*
 * @brief:
 *     设置数据断点
 * @param[in]:addr:数据断点地址
 * @return:
 *    无
 */
void taDebugSetDataBreakpoint(UINT32 addr);

/*
 * @brief:
 *     设置指令断点
 * @param[in]:addr:指令断点地址
 * @return:
 *    无
 */
void taDebugSetAddressBreakpoint(UINT32 addr);

/*
 * @brief:
 *    获取数据断点地址
 * @return:
 *    数据断点地址
 */
UINT32 taDebugGetDataBreakpoint(void);

/*
 * @brief:
 *    获取指令断点地址
 * @return:
 *    指令断点地址
 */
UINT32 taDebugGetAddressBreakpoint(void);

/*
 * @brief:
 *     获取DAR寄存器值
 * @return:
 *    DAR寄存器值
 */
UINT32 taDebugGetDARValue(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _DB_DR_H */
