/****************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *
 *          Copyright (C) 2000-2007 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * 修改记录：
 * 20090824    创建文件 
 */

/**
 * @file     ta_8641_uart.c
 * @brief
 * 		功能：实现MPC8641串口驱动。串口芯片采用NS16550
 * @date    20090824
 */


/**************************** 引用部分 *****************************************/
//#include "assert.h"
//#include "archMsl.h"
//#include "sysDR.h"
//#include <msl/config_msl.h>
//#include <sysUart.h>
#include <taTypes.h>
#include "taErrorDefine.h"
#include "sysUart.h"
#include <config.h>

#define SYNC __asm__("sync")
#define EIEIO_SYNC __asm__ ("eieio; sync")
#define EIEIO __asm__ ("eieio")
/*************************** 前向声明部分 ****************************************/
/*************************** 外部声明部分 ****************************************/
/**************************** 定义部分 *******************************************/
#define		CONFIG_DRIVERS_SERIAL_TSI109UART		{\
		{("COM1"),(DR_UART_NO_PARITY),(8),(115200),(1),(DR_UART_POLLING)},\
		{("COM2"),(DR_UART_NO_PARITY),(8),(115200),(1),(DR_UART_POLLING)}		}

#ifdef CONFIG_DRIVERS_SERIAL_TSI109UART
	T_DR_UartConfigTable utDR_UartConfigTable[] = CONFIG_DRIVERS_SERIAL_TSI109UART;
#endif

char com1_ier;
char com2_ier;

#define MPC8641_REG_BASE_ADDR	 CCSRBAR
#define MPC8641_UART0_OFFSET	0x4500			/*串口1的块地址*/
#define MPC8641_UART1_OFFSET	0x4600			/*串口2的块地址*/

#define COM1_INT_VECTOR		26
#define COM2_INT_VECTOR		12

/* Register offsets from base address */
/* 0x4500 - URBR1, UTHR1, UDLB1 with the same address offset of 0x04500 */
#define RBR	0x00	/* Receive Holding Register (R/O) */
#define THR	0x00	/* Transmit Holding Register (W/O)*/
#define DLB	0x00	/* Divisor Latch Low */

/* 0x4501 - UIER1, UDMB1 with the same address offset of 0x04501 */
#define IER	0x01	/* Interrupt Enable Register */
#define DMB	0x01	/* Divisor Latch Middle */

/* 0x4502 - UIIR1, UFCR1, UAFR1 with the same address offset of 0x04502 */
#define IIR	0x02	/* Interupt identification Register (R/O) */
#define FCR	0x02	/* FIFO Control register (W/O) */
#define AFR	0x02	/*Alternate Function Registers*/
#define LCR	0x03	/* Line Control Register */
#define MCR	0x04	/* Modem Control Register */
#define LSR	0x05	/* Line Status register */
#define MSR	0x06	/* Modem Status Register */
#define SCR	0x07	/* Scratchpad Register */


/* Line Control Register values */
#define CHAR_LEN_5	0x00
#define CHAR_LEN_6	0x01
#define CHAR_LEN_7	0x02
#define CHAR_LEN_8	0x03

#define ONE_STOP	0x00	/* One stop bit! */
#define TWO_STOP	0x04

#define LCR_PEN		0x08	/* Parity Enable */
#define PARITY_NONE	0x00
#define PARITY_ODD		0x08
#define PARITY_EVEN	0x18

#define LCR_DLAB	0x80	/* Divisor Latch Access Bit */
#define DLAB		LCR_DLAB

/* FIFO Control Register */
#define FCR_EN			0x01		/* FIFO Enable */
#define FIFO_ENABLE	FCR_EN
#define FCR_RXCLR		0x02		/* Rx FIFO Clear */
#define RxCLEAR		FCR_RXCLR
#define FCR_TXCLR		0x04		/* Tx FIFO Clear */
#define TxCLEAR		FCR_TXCLR

/* Interrupt Enable Register */
#define IER_ERDAI	0x01		/* Enable Rx Data Available Int */
#define IER_ETHREI	0x02		/* Enable THR Empty Int */


/* Line Status Register */
#define LSR_DR		0x01	/* Data Ready */
#define LSR_OE		0x02	/* Overrun Error */
#define LSR_PE		0x04	/* Parity Error */
#define LSR_FE		0x08	/* Framing Error */
#define LSR_BI		0x10	/* Received Break Signal */
#define LSR_THRE	0x20	/* Transmit Holding Register Empty */
#define LSR_TEMT	0x40	/* THR and FIFO empty */
#define LSR_FERR	0x80	/* Parity, Framing error or break in FIFO */

/* Interrupt Identification Register */
#define IIR_RDA	0x04		/* Rx Data Available */
#define IIR_THRE	0x02		/* THR Empty */

#define MPX_CLOCK (550000000) //频率为400M

/**************************** 实现部分 ******************************************/
/**
 * @brief
 *    功能:
 *        获取设备数。
 *
 *  实现内容：
 *        获取串口设备个数。
 *
 * @return 串口设备数。
 */
T_WORD BSP_UartGetDeviceNum()
{
	return sizeof(utDR_UartConfigTable)/sizeof(T_DR_UartConfigTable);	
}

/**
 * @brief
 *    功能:
 *        串口初始化操作。
 *
 *  实现内容：
 *        实现对串口模块初始化,配置各个寄存器
 *         设置波特率，设置停止位，数据位，效验位等
 *         设置工作模式中断或者轮询
 *
 * @param[in]  minor：串口设备号
 * @param[in]  config：指针，设备配置信息
 * @param[in]  isr：中断服务程序
 * @return 无。
 */
T_VOID BSP_UartInit(T_WORD minor,T_DR_UartConfigTable *config, Vector_Handler isr)
{
	char data_len;
	char stop_bit;
	char parity;
	unsigned int addr,vector;
    
	if(minor)
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
		vector = COM2_INT_VECTOR;
		com2_ier = IER_ERDAI;
		config = &utDR_UartConfigTable[1];
	}
	else
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
		vector = COM1_INT_VECTOR;
		com1_ier = IER_ERDAI;
		config = &utDR_UartConfigTable[0];
	}
	
	//数据位长
	switch(config->bits_per_char)
	{
	case 5:
	  	data_len = CHAR_LEN_5;
	  	break;
	case 6:
		data_len = CHAR_LEN_6;
		break;
	case 7:
		data_len = CHAR_LEN_7;
		break;
	case 8:
		data_len = CHAR_LEN_8;
		break;
	default:
		data_len = CHAR_LEN_8;
	}
	
	//停止位长
	switch(config->num_of_stop_bits)
	{
	case 1:
		stop_bit = ONE_STOP;
		break;
	case 2:
		stop_bit = TWO_STOP;
		break;
	default:
		stop_bit = ONE_STOP;
	}
	
	//奇偶校验选择
	switch(config->parity_mode)
	{
	case DR_UART_NO_PARITY:
		parity = PARITY_NONE;
		break;
	case DR_UART_EVEN_PARITY:
		parity = PARITY_EVEN;
		break;
	case DR_UART_ODD_PARITY:
		parity = PARITY_ODD;
		break;
	default:
		parity = PARITY_NONE;
	}
	
	(*(volatile unsigned char*)(addr + LCR)) = (LCR_DLAB | data_len | stop_bit | parity);/*使能DMB DLB*/
	EIEIO_SYNC;
	(*(volatile unsigned char*)(addr + FCR)) = (RxCLEAR | TxCLEAR);/*disable FIFO*/
	EIEIO_SYNC;

	(*(volatile unsigned char*)(addr + DLB)) = (MPX_CLOCK/16/(115200))&0xFF;	/*波特率115200*/		//(UART_XTAL/(2*config->baudrate));
	EIEIO_SYNC;
	(*(volatile unsigned char*)(addr + DMB)) = (MPX_CLOCK/16/(115200))>>8;						//((UART_XTAL/(2*config->baudrate)) >> 8);
	EIEIO_SYNC;

	(*(volatile unsigned char*)(addr + LCR)) = (data_len | stop_bit | parity);/*设置停止位、数据位、效验位*/
	EIEIO_SYNC;
	
	(*(volatile unsigned char*)(addr + IER)) = 0;
	EIEIO_SYNC;
	
	if(config->mode == DR_UART_INTERRUPT)/*中断模式*/
	{
		//使能接收中断
		(*(volatile unsigned char*)(addr + IER)) = IER_ERDAI;
		EIEIO_SYNC;
	}

    

}

/**
 * @brief
 *    功能:
 *        关闭串口。
 *
 *  实现内容：
 *        禁止中断，卸掉中断服务程序（中断方式），关闭设备。
 *
 * @param[in]  minor：串口设备号
 *
 * @return 无。
 */
T_VOID  BSP_UartClose(T_WORD minor)
{
	unsigned int addr,vector;
	
	if(minor)
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
		vector = COM2_INT_VECTOR;
	}
	else
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
		vector = COM1_INT_VECTOR;
	}
	//禁止中断
	(*(volatile unsigned char*)(addr + IER)) = 0;
	EIEIO_SYNC;

}

/**
 * @brief
 *    功能:
 *        从串口发送数据（轮询方式）。
 *
 *  实现内容：
 *        等待串口发送空闲
 *         写串口发送寄存器发送数据
 * 
 * @param[in]  minor：串口设备号
 * @param[in]  buf：要发送的数据
 * @param[in]  len：发送数据长度
 * 
 * @return 返回0。
 */
T_WORD BSP_UartPollWrite(T_WORD minor, const T_BYTE *buf, T_WORD len)
{
    unsigned int addr;

    if(minor)
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
   	}
   	else
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
   	}
   	
   while (len--)
	{
		//等待串口准备好
   		while(!((*(volatile unsigned char*)(addr + LSR)) & LSR_THRE));
        EIEIO_SYNC;
		(*(volatile unsigned char*)(addr + THR)) = *buf++;
        EIEIO_SYNC;

	}
	
	return 0;

}

/**
 * @brief
 *    功能:
 *        从串口获取字符（轮询方式）。
 *
 *  实现内容：
 *        等待串口接收到字符
 *         从串口接收寄存器读出字符
 *
 * @param[in]  minor：串口设备号
 *
 * @return 返回读取的字符。
 */
T_WORD BSP_UartPollRead(T_WORD minor)
{
   char ch;
  
    unsigned int addr;

    if(minor)
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
   	}
   	else
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
   	}

   //等待接收到字符
	while(!((*(volatile unsigned char*)(addr + LSR)) & LSR_DR));
    EIEIO_SYNC;
    ch = (*(volatile unsigned char*)(addr + RBR));
    EIEIO_SYNC;

   return (T_WORD)ch;


}

/**
 * @brief
 *    功能:
 *        从串口发送字符。
 *
 *  实现内容：
 *         写串口发送寄存器发送字符
 * 
 * @param[in]  minor：串口设备号
 * @param[in]  ch：要发送的字符
 * 
 * @return 无。
 */
void BSP_UartInterruptWrite(T_WORD minor, T_BYTE ch)
{
	unsigned int addr;

    if(minor)
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
   	}
   	else
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
   	}
   	(*(volatile unsigned char*)(addr + THR)) = ch;
    EIEIO_SYNC;

}


T_WORD BSP_UartReadOnce(T_WORD minor,UINT8 *data)
 {

    char ch;
	unsigned int addr;

    if(minor)
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
   	}
   	else
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
   	}
   	ch = (*(volatile unsigned char*)(addr + RBR));
    EIEIO_SYNC;
	*data = ch;
    return 1;
 }

/**
 * @brief
 *    功能:
 *        禁止接收中断。
 *
 *  实现内容：
 *         禁止串口接收中断
 *
 * @param[in]  minor：串口设备号
 *
 * @return 无。
 */
T_VOID BSP_UartDisableReceiveInterrupt(T_WORD minor)
{
	unsigned int addr;
	
	if(minor)
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
		com2_ier &= (~IER_ERDAI);
		(*(volatile unsigned char*)(addr + IER)) = com2_ier;
		EIEIO_SYNC;
	}
	else
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
		com1_ier &= (~IER_ERDAI);
		(*(volatile unsigned char*)(addr + IER)) = com1_ier;
		EIEIO_SYNC;
	}

}
/**
 * @brief
 *    功能:
 *        从串口获取字符。
 *
 *  实现内容：
 *         从串口接收寄存器读出字符
 *
 * @param[in]  minor：串口设备号
 *
 * @return 返回读取的字符。
 */
 T_UBYTE BSP_UartInterruptRead(T_WORD minor)
 {
	char ch;
	unsigned int addr;

    if(minor)
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
   	}
   	else
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
   	}
   	ch = (*(volatile unsigned char*)(addr + RBR));
    EIEIO_SYNC;

    return ch;
 }

/**
 * @brief
 *    功能:
 *        使能接收中断。
 *
 *  实现内容：
 *         使能串口接收中断
 *
 * @param[in]  minor：串口设备号
 *
 * @return 无。
 */
T_VOID BSP_UartEnableReceiveInterrupt(T_WORD minor)
{
	unsigned int addr;
	
	if(minor)
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
		com2_ier |= IER_ERDAI ;
		(*(volatile unsigned char*)(addr + IER)) = com2_ier;
		EIEIO_SYNC;
	}
	else
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
		com1_ier |= IER_ERDAI ;
		(*(volatile unsigned char*)(addr + IER)) = com1_ier;
		EIEIO_SYNC;
	}

}

/**
 * @brief
 *    功能:
 *        禁止发送中断。
 *
 *  实现内容：
 *         禁止串口发送中断
 *
 * @param[in]  minor：串口设备号
 *
 * @return 无。
 */
T_VOID BSP_UartDisableSendOverInterrupt(T_WORD minor)
{
	unsigned int addr;
	
	if(minor)
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
		com2_ier &= (~IER_ETHREI);
		(*(volatile unsigned char*)(addr + IER)) = com2_ier;
		EIEIO_SYNC;
	}
	else
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
		com1_ier &= (~IER_ETHREI);
		(*(volatile unsigned char*)(addr + IER)) = com1_ier;
		EIEIO_SYNC;
	}

}

/**
 * @brief
 *    功能:
 *        使能发送中断。
 *
 *  实现内容：
 *         使能串口发送中断
 *
 * @param[in]  minor：串口设备号
 *
 * @return 无。
 */
T_VOID BSP_UartEnableSendOverInterrupt(T_WORD minor)
{
	unsigned int addr;
	
	if(minor)
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
		com2_ier |= IER_ETHREI ;
		(*(volatile unsigned char*)(addr + IER)) = com2_ier;
		EIEIO_SYNC;
	}
	else
	{
		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
		com1_ier |= IER_ETHREI ;
		(*(volatile unsigned char*)(addr + IER)) = com1_ier;
		EIEIO_SYNC;
	}

}

/**
 * @brief
 *    功能:
 *        获取中断状态。
 *
 *  实现内容：
 *         获取设备中断状态
 *
 * @param[in]  minor：串口设备号
 *
 * @return 返回中断状态。可能的状态如下：
 *            UART_SENDOVER_INTERRUPT 发送中断
 *            UART_RECEIVE_INTERRUPT  接收中断
 * 
 */
T_UBYTE BSP_UartISRStatus(T_WORD minor)
{
	char isr_status,iir;
	unsigned int addr;
	
	if(minor)
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
   	}
   	else
   	{
   		addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
   	}
   	iir = (*(volatile unsigned char*)(addr + IIR));
	EIEIO_SYNC;

	isr_status = 0;

	if( iir & IIR_RDA )
	  isr_status |= UART_RECEIVE_INTERRUPT ;

	if( iir & IIR_THRE )
      isr_status |= UART_SENDOVER_INTERRUPT ;

     return isr_status ;

}


void BSP_UartGetVector(int minor, int* vector)
{
    if(minor)
    {
        *vector = COM2_INT_VECTOR;
    }
    else
    {
        *vector = COM1_INT_VECTOR;
    }
}

/**
 * @brief
 *    功能:
 *        从串口获取字符（轮询方式）。
 *
 *  实现内容：
 *        等待串口接收到字符
 *         从串口接收寄存器读出字符
 *
 * @param[in]  minor：串口设备号
 *
 * @return 返回读取的字符。
 */
T_UBYTE BSP_UartLSRStatus(T_WORD minor)
{
    char lsr;
    unsigned char lsr_status;
    unsigned int addr;

    if(minor)
    {
        addr = MPC8641_REG_BASE_ADDR + MPC8641_UART1_OFFSET;
    }
    else
    {
        addr = MPC8641_REG_BASE_ADDR + MPC8641_UART0_OFFSET;
    }

    //等待接收到字符
    lsr = *(volatile unsigned char*)(addr + LSR);
    EIEIO_SYNC;

    if( lsr & LSR_DR )
    {
        lsr_status = UART_LINE_STATUS_DATA_READY;
    }
    else
    {
        lsr_status = UART_LINE_STATUS_ERROR;
    }

	return lsr_status;

}

#if 1
/*
 * @brief:
 *      初始化回显设备
 * @return:
 *      无
 */
void taDisplayDeviceOpen(void)
{
	BSP_UartInit(0,utDR_UartConfigTable,NULL);
}

void taDisplayDeviceOpen8640(char *name)
{
	signed int  device_num, n;

		/* 获取串口设备数量 */
		device_num = BSP_UartGetDeviceNum();

		for (n=0; n<device_num; n++)
		{
			/* 查找与printk设备名匹配的串口设备 */
			if (strcmp( name, (const char *)(utDR_UartConfigTable[n].name)) == 0)
			{
				BSP_UartInit( n, &utDR_UartConfigTable[n], NULL );
				/* install printk handler */
			}
		}
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

	BSP_UartPollWrite(0,&ch, 1);
		if (ch == '\n')
		{
			ch = '\r';
			BSP_UartPollWrite(0,&ch, 1);
		}

}
void taDisplayDeviceOutputChar8640(T_CHAR ch)
{

	BSP_UartPollWrite(0,&ch, 1);
		if (ch == '\n')
		{
			ch = '\r';
			BSP_UartPollWrite(0,&ch, 1);
		}

}
#endif

