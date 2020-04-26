
/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taI8250.c
 * @brief:
 *             <li>实现i8250的串口驱动</li>
 */

#ifdef CONFIG_TA_DISP_UART

/************************头 文 件******************************/
#include "taIoAccess.h"
#include "taI8250.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *      初始化控制台
 * @return:
 *      无
 */
void taDisplayDeviceOpen(void)
{
	T_CHAR value;
	T_UWORD	baudrate;

	BSP_OutByte(COM_DIER1, IER_DISABLE_ALL);

	/* 清线路状态寄存器 */
	BSP_InByte(COM_DLSR1, value);

	/* 清Moden状态寄存器 */
	BSP_InByte(COM_DMSR1, value);

	/* 清数据接收寄存器 */
	BSP_InByte(COM_DRBR1, value);

	/* 设置数据格式、波特率 */
	BSP_OutByte(COM_DLCR1, ULCON_WL8 | ULCON_STOP_2 | ULCON_PMD_NO);

	/* 使线路控制寄存器最高位DLAB=1 */
	BSP_InByte(COM_DLCR1, value);
	BSP_OutByte(COM_DLCR1, value|0x80);
    
    /* 计算波特率 */
	baudrate = FREQENCY_8250 / (16 * 115200);

	/* 设置波特率 */
	BSP_OutByte(COM_DDLL1, (T_UBYTE)baudrate );
	BSP_OutByte(COM_DDLH1, (T_UBYTE)(baudrate>>8));

	/* 恢复线路控制寄存器最高位DLAB */
	BSP_OutByte(COM_DLCR1, value);
}

/*
 * @brief:
 *      回显设备输出字符
 * @param[in]: ch:字符
 * @return:
 *      无
 */
void taDisplayDeviceOutputChar(T_CHAR ch)
{
	T_CHAR value;

	do
	{
		BSP_InByte(COM_DLSR1, value);

	}while(!(value & LSR_SEND_READY));

	BSP_OutByte(COM_DTHR1, ch);
}
#endif
