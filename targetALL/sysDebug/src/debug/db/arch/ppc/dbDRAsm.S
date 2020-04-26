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
 * @file:dbDRAsm.S
 * @brief:
 *             <li>设置和读取DABR，IABR寄存器接口</li>
 */

/************************头 文 件*****************************/

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *      设置DABR寄存器
 * @param[in]:uwAddr: 待设地址
 * @return:
 *      无
 */
.global taDebugSetDABR
taDebugSetDABR:
    mtspr 1013, 3
    blr
    
/*
 * @brief:
 *      设置IABR寄存器
 * @param[in]:uwAddr: 待设地址
 * @return:
 *      无
 */
.global taDebugSetIABR
taDebugSetIABR:
    mtspr 1010, 3
    blr
    
/*
 * @brief:
 *      获取DABR寄存器的值
 * @return:
 *      DABR寄存器的值
 */
.global taDebugGetDABR
taDebugGetDABR:
    mfspr 3, 1013
    blr
    
/*
 * @brief:
 *      获取IABR寄存器的值
 * @return:
 *      IABR寄存器的值
 */
.global taDebugGetIABR
taDebugGetIABR:
    mfspr 3, 1010
    blr

/*
 * @brief:
 *      获取DAR寄存器的值
 * @return:
 *      DAR寄存器值
 */
.global taDebugGetDAR
taDebugGetDAR:
    mfspr 3,19
    blr
