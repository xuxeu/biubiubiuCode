/****************************************************************************
 *				北京科银京成技术有限公司 版权所有
 *
 * 		Copyright (C) 2000-2008 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * 修改记录：
 *	20080310	创建文件。
 *
 */

/**
 * @file 	taLson2euart.h
 * @brief
 *	<li>功能： 定义串口的相关宏和函数的声明。</li>
 * @date 	20080310
 */

#ifndef __SERIAL_TALSON2EUART_H__
#define __SERIAL_TALSON2EUART_H__

#ifdef	__cplusplus
extern	"C" {
#endif

/**************************** 引用部分 *****************************************/
#include <sysTypes.h>	/// 使用系统类型定义
//#include <bspPubInfo.h>

/**************************** 定义部分 *****************************************/
//输入时钟频率
#define BSP_8250_FREQENCY 1843200

#define COM_BASEADDR 0x3f8 + 0xbfd00000  //设备基地址

#define TA_UART_COM_RX_IRQ (0x4)         //设备中断向量号

#define TA_UART_COM_MINOR 0              //串口端口号定义


#define COM_DLCR *(volatile unsigned char *)(COM_BASEADDR + 0x03) 	 //控制寄存器
#define COM_DLSR *(volatile unsigned char *)(COM_BASEADDR + 0x05)   //状态寄存器
#define COM_DMCR *(volatile unsigned char *)(COM_BASEADDR + 0x04) 	 //modem控制寄存器
#define COM_DMSR *(volatile unsigned char *)(COM_BASEADDR + 0x06) 	 //modem状态寄存器
#define COM_DIIR *(volatile unsigned char *)(COM_BASEADDR + 0x02) 	 //中断寄存器
#define COM_DIER *(volatile unsigned char *)(COM_BASEADDR + 0x01) 	 //中断使能寄存器
#define COM_DDLL *(volatile unsigned char *)(COM_BASEADDR + 0x00) 	 //分频寄存器
#define COM_DDLH *(volatile unsigned char *)(COM_BASEADDR + 0x01) 	 //分频寄存器
#define COM_DTHR *(volatile unsigned char *)(COM_BASEADDR + 0x00) 	//发送寄存器
#define COM_DRBR *(volatile unsigned char *)(COM_BASEADDR + 0x00) 	//接收寄存器

///控制寄存器设置
//bit 0-1 : 字长
#define	ULCON_WL5               0x00
#define	ULCON_WL6               0x01
#define	ULCON_WL7               0x02
#define	ULCON_WL8               0x03
#define ULCON_WL_MASK			0X03

//bit 2 : 停止位
#define	ULCON_STOP_2            0x04
#define ULCON_STOP_1            0x00
#define ULCON_STOP_MASK         0x04

//bit 3-5 : 校验模式
#define	ULCON_PMD_NO            0x00  //无校验
#define	ULCON_PMD_ODD           0x08  //奇效验
#define	ULCON_PMD_EVEN          0x18  //偶效验
#define	ULCON_PMD_CHK1          0x28  //强制效验检查1
#define	ULCON_PMD_CHK0          0x38  //强制效验检查0
#define	ULCON_PMD_MASK          0x38

//中断使能寄存器设置
#define BSP_IER_RECEIVE_INTERRUPT       0x01  //bit 0: 接收数据中断
#define BSP_IER_SEND_OVER_INTERRUPT     0x02  //bit 1：发送数据中断
#define BSP_IER_DATA_ERROR_INTERRUPT    0x04  //bit 2：接收数据代码错中断
#define BSP_IER_MODEM_INTERRUPT         0x08  //bit 3 ：Modem状态（改变）中断
#define BSP_IER_DISABLE_ALL				0x00  //禁止所有中断
#define BSP_IER_ENABLE_ALL				0x0f  //允许所有中断

//状态寄存器设置
#define BSP_LSR_DATA_RECEIVED  		0x01  //bit 0：数据接收就绪
#define BSP_LSR_RECEIVE_BUSY   		0x02  //bit 1：溢出错
#define BSP_LSR_PARITY_ERROR  		0x04  //bit 2：奇偶错
#define BSP_LSR_DATA_FORMAT_ERROR	0x08  //bit 3：帧错
#define BSP_LSR_STOP_SIGNAL         0x10  //bit 4：中止检测
#define BSP_LSR_SEND_READY			0x20  //bit 5：发送保持寄存器空
#define BSP_LSR_DATA_TO_LINE		0x40  //bit 6：发送移位寄存器空

//Modem控制寄存器设置
#define BSP_MCR_TERMINAL_READY		0x01  //bit 0: DTR
#define BSP_MCR_REQUEST_SEND		0x02  //bit 1: RTS
#define BSP_MCR_OUT1_IRQ_LINE		0x04  //bit 2: OUT1
#define BSP_MCR_OUT2_IRQ_LINE		0x08  //bit 3: OUT2，将PC COM卡的中断启用
#define BSP_MCR_LOOP_TEST			0x10  //bit 4: LOOP，自检


#ifdef	__cplusplus
}
#endif

#endif
