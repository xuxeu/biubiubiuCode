/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-23         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file: taEth8139.h
 *@brief:
 *             <li>RTL8139网卡驱动程序头文件</li>
 */
 
#ifndef TA_ETH8139_H
#define TA_ETH8139_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头 文 件******************************/

/************************宏定义*******************************/

/*the registers's offset of rtl8139*/
#define RX_OPTION  0x0a 
#define ONLY_MATCH_PACKET 0x02

#define R8139_IDR0   0x0000     /*ethernet ID(MAC)*/
#define R8139_IDR1   0x0001
#define R8139_IDR2   0x0002
#define R8139_IDR3   0x0003
#define R8139_IDR4   0x0004
#define R8139_IDR5   0x0005

#define R8139_MAR0   0x0008     /*multicast*/
#define R8139_MAR1   0x0009
#define R8139_MAR2   0x000a
#define R8139_MAR3   0x000b
#define R8139_MAR4   0x000c
#define R8139_MAR5   0x000d
#define R8139_MAR6   0x000e
#define R8139_MAR7   0x000f

#define R8139_TSD0   0x0010     /*transmit status of descriptor*/
#define R8139_TSD1   0x0014
#define R8139_TSD2   0x0018
#define R8139_TSD3   0x001c

#define R8139_TSAD0   0x0020    /*transmit start address of descriptor*/
#define R8139_TSAD1   0x0024
#define R8139_TSAD2   0x0028
#define R8139_TSAD3   0x002c

#define R8139_RBSTART   0x0030   /*receive buffer start address*/
#define R8139_ERBCR     0x0034   /*early receive byte count*/
#define R8139_ERSR      0x0036   /*early rx status*/

#define R8139_CR        0x0037   /*command register*/

#define R8139_CAPR      0x0038   /*current address of packet read*/
#define R8139_CBR       0x003a   /*current buffer address*/

#define R8139_IMR       0x003c   /*interrupt mask register*/
#define R8139_ISR       0x003e   /*interrupt status register*/

#define R8139_TCR       0x0040   /*transmit configuration register*/
#define R8139_RCR       0x0044   /*receive  configuration register*/

#define R8139_TCTR      0x0048   /*timer count register*/
#define R8139_MPC       0x004c   /*missed packet counter*/

#define R8139_9346CR    0x0050   /*9346 command register*/
#define R8139_CONFIG0   0x0051   /*configuration register 0*/
#define R8139_CONFIG1   0x0052   /*configuration register 1*/

#define R8139_TIMERINT  0x0054   /*timer interrupt register*/

#define R8139_MSR       0x0058   /*media status register*/
#define R8139_CONFIG3   0x0059   /*configuration register 3*/
#define R8139_CONFIG4   0x005a   /*configuration register 4*/

#define R8139_MULINT    0x005c   /*multiple interrupt select*/
#define R8139_RERID     0x005e   /*pci revision ID(10h)*/

#define R8139_TSAD      0x0060   /*transmit status of all descriptors*/

#define R8139_BMCR      0x0062   /*basic mode control register*/
#define R8139_BMSR      0x0064   /*basic mode status  register*/
#define R8139_ANAR      0x0066   /*auto-negotiation advertisement*/
#define R8139_ANLPAR    0x0068   /*auto-negotiation link partner*/
#define R8139_ANER      0x006a   /*auto-negotiation expansion*/
#define R8139_DIS       0x006c   /*disconnect counter*/
#define R8139_FCSC      0x006e   /*false carrier sense counter*/
#define R8139_NWAYTR    0x0070   /*n-way test register*/
#define R8139_REC       0x0072   /*rx-er counter*/
#define R8139_CSCR      0x0074   /*cs configuration register*/

#define R8139_PHY1PARM  0x0078   /*PHY parameter 1*/
#define R8139_TWPARM    0x007c   /*twister parameter*/
#define R8139_PHY2PARM  0x0080   /*PHY parameter 2*/

#define R8139_CRC0      0x0084   /*power management CRC registers for wakeup frames*/
#define R8139_CRC1      0x0085
#define R8139_CRC2      0x0086
#define R8139_CRC3      0x0087
#define R8139_CRC4      0x0088
#define R8139_CRC5      0x0089
#define R8139_CRC6      0x008a
#define R8139_CRC7      0x008b

#define R8139_WAKEUP0   0x008c    /*power magament wakeup frames(64bit each)*/
#define R8139_WAKEUP1   0x0094
#define R8139_WAKEUP2   0x009c
#define R8139_WAKEUP3   0x00a4
#define R8139_WAKEUP4   0x00ac
#define R8139_WAKEUP5   0x00b4
#define R8139_WAKEUP6   0x00bc
#define R8139_WAKEUP7   0x00c4

#define R8139_LSBCRC0   0x00cc    /*LSB of the mask byte of wakeup frames within offset 12 to 75*/
#define R8139_LSBCRC1   0x00cd
#define R8139_LSBCRC2   0x00ce
#define R8139_LSBCRC3   0x00cf
#define R8139_LSBCRC4   0x00d0
#define R8139_LSBCRC5   0x00d1
#define R8139_LSBCRC6   0x00d2
#define R8139_LSBCRC7   0x00d3

#define R8139_FLASH     0x00d4    /*flash memory read/write register*/

#define R8139_CONFIG5   0x00d8    /*configuration register 5*/

#define R8139_FER       0x00f0    /*function event register(cardbus only)*/
#define R8139_FEMR      0x00f4    /*function event mask register(cardbus only)*/
#define R8139_FPSR      0x00f8    /*function present state register(cardbus only)*/
#define R8139_FFER      0x00fc    /*function force event register(cardbus only)*/

/*the registers's offset of rtl8139 end*/

/*some parameter for 8139*/

#define NUM_TX_DESC	    4			/* Number of Tx descriptor registers. */

#define RX_BUF_LEN_IDX	0			/* 0==8K, 1==16K, 2==32K, 3==64K */
#define RX_BUF_LEN (8192 << RX_BUF_LEN_IDX)

/* Size of the Tx bounce buffers -- must be at least (mtu+14+4). */
#define TX_BUF_SIZE	1536

/* PCI Tuning Parameters
Threshold is bytes transferred to chip before transmission starts. */
#define TX_FIFO_THRESH 256	/* In bytes, rounded down to 32 byte units. */

/* The following settings are log_2(bytes)-4:  0 == 16 bytes .. 6==1024. */
#define RX_FIFO_THRESH	4		/* Rx buffer level before first PCI xfer.  */
#define RX_DMA_BURST	4		/* Maximum PCI burst, '4' is 256 bytes */
#define TX_DMA_BURST	4

/* Operational parameters that usually are not changed. */
/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT  ((4000*ei_status.ticks_per_second)/1000)
#define ETH_ZLEN	60		/* Min. octets in frame sans FCS */
#define MAX_SERVICE     12
#define MAX_ADDR_LEN    6

/* Interrupt register bits, using my own meaningful names. */
enum IntrStatusBits 
{
	PCIErr=0x8000, PCSTimeout=0x4000,
	RxFIFOOver=0x40, RxUnderrun=0x20, RxOverflow=0x10,
	TxErr=0x08, TxOK=0x04, RxErr=0x02, RxOK=0x01,
};

enum TxStatusBits 
{
	TxHostOwns=0x2000, TxUnderrun=0x4000, TxStatOK=0x8000,
	TxOutOfWindow=0x20000000, TxAborted=0x40000000, TxCarrierLost=0x80000000,
};
enum RxStatusBits 
{
	RxMulticast=0x8000, RxPhysical=0x4000, RxBroadcast=0x2000,
	RxBadSymbol=0x0020, RxRunt=0x0010, RxTooLong=0x0008, RxCRCErr=0x0004,
	RxBadAlign=0x0002, RxStatusOK=0x0001,
};

/************************类型定义*****************************/

/************************接口声明*****************************/

/*
 * @brief:
 *    从网络设备读取一个数据包
 * @param: minor: 未使用
 * @param[in]: buf: 存放数据包的缓冲
 * @param[out]: offset: 有些网卡会在以太网帧的前面添加部分状态字，该
 *                      状态字不属于以太网帧，并且依赖于不同的网卡，offset就指明了这
 *                      部分数据的长度，以通知协议栈丢弃该部分数据
 * @return: 
 *    获取的数据包的大小
 */ 
T_WORD taNetGetpkt(T_UBYTE minor,T_CHAR* buf,T_WORD* offset);

/*
 * @brief:
 *    向网络设备发送一个数据包
 * @param: minor: 未使用
 * @param[in]: buf: 存放数据包的缓冲
 * @param[in]: cnt: 数据包的长度
 * @return: 
 *    无
 */
T_VOID taNetPutpkt(T_UBYTE minor,T_CHAR* buf,T_WORD cnt);

/*
 * @brief:
 *    打开网络设备
 * @param[in]: minor: 设备号
 * @param[in]: mac: MAC地址
 * @return: 
 *    TRUE: 成功。
 *    FALSE: 失败。
 */ 
T_BOOL taNetOpen(T_UBYTE minor, UINT8 * mac);

#ifdef __cplusplus 
}
#endif /* __cplusplus */

#endif /* TA_ETH8139_H */
