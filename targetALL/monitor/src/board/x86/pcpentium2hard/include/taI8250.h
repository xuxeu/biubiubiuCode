/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taI8250.h
 * @brief:
 *             <li>定义i8250相关的宏、数据结构、函数接口等</li>
 */
#ifndef TA_I8250_H
#define TA_I8250_H

/************************头文件********************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************宏定义********************************/
#define COM_DLCR1 0x03FB /* line control register */
#define COM_DLSR1 0x03FD /* line status register */
#define COM_DMCR1 0x03FC /* modem control register */
#define COM_DMSR1 0x03FE /* modem status register */
#define COM_DIIR1 0x03FA /* interrupt identification register */
#define COM_DIER1 0x03F9 /* interrupt enable register */
#define COM_DDLL1 0x03F8 /* divisor  lock lower register */
#define COM_DDLH1 0x03F9 /* divisor lock higher register */
#define COM_DTHR1 0x03F8 /* transmit hold register */
#define COM_DRBR1 0x03F8 /* receive buffer register */

#define COM_DLCR2 0x02FB /* line control register */
#define COM_DLSR2 0x02FD /* line status register */
#define COM_DMCR2 0x02FC /* modem control register */
#define COM_DMSR2 0x02FE /* modem status register */
#define COM_DIIR2 0x02FA /* interrupt identification register */
#define COM_DIER2 0x02F9 /* interrupt enable register */
#define COM_DDLL2 0x02F8 /* divisor  lock lower register */
#define COM_DDLH2 0x02F9 /* divisor lock higher register */
#define COM_DTHR2 0x02F8 /* transmit register */
#define COM_DRBR2 0x02F8 /* receive register */

/* 禁止所有中断 */
#define IER_DISABLE_ALL			0x00

/* bit 0-1 : Word length */
#define	ULCON_WL5               0x00
#define	ULCON_WL6               0x01
#define	ULCON_WL7               0x02
#define	ULCON_WL8               0x03

/* bit 2 : Number of stop bits */
#define	ULCON_STOP_2            0x04
#define ULCON_STOP_1            0x00

/* bit 3-5 : Parity mode */
#define	ULCON_PMD_NO            0x00  /* no parity */
#define	ULCON_PMD_ODD           0x20  /* odd parity */
#define	ULCON_PMD_EVEN          0x28  /* even parity */
#define	ULCON_PMD_CHK1          0x30  /* parity is forced,checked as a 1 */
#define	ULCON_PMD_CHK0          0x38  /* parity is forced,checked as a 0 */

/* input clock for 8250 */
#define FREQENCY_8250 1843200

/* bit 5：发送保持寄存器空 */
#define LSR_SEND_READY			0x20

/************************类型定义******************************/

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* TA_I8250_H */


