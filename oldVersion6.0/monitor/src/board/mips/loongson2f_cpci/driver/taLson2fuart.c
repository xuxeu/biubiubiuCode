/****************************************************************************
 *				北京科银京成技术有限公司 版权所有
 *
 * 		Copyright (C) 2000-2008 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * 修改记录：
 *	20080310	创建文件。BFC0 0000 
 *
 */

/**
 * @file 	taLson2euart.inc
 * @brief
 *	<li>功能： 龙芯2E串口驱动程序。</li>
 * @date 	20080310
 */

/**************************** 引用部分 *****************************************/
#include <sysAgent.h>
#include "memory.h"
#include "taLson2fuart.h"

/**************************** 定义部分 *****************************************/

/**************************** 实现部分 *****************************************/ 
/**
 * @brief
 *	  	设置串口属性。
 *
 * @param[in] minor：串口设备号。
 * @param[in] baud：波特率。
 *
 * @return 无。
 */
T_VOID monitorRawUartSetAttribute(T_UWORD minor, T_UWORD baud)
{
	unsigned char value;
	unsigned int baudrate;
	value = 0;
	baudrate = 0;
	
	//关闭设备(关与中断无关寄存器，设备没有这样的寄存器，此处是屏蔽PC COM卡的中断）
	value = COM_DMCR;
	value = (value & (~BSP_MCR_OUT2_IRQ_LINE));
	COM_DMCR = value;
	
	//设置串口为每字符8位,1位停止位,无校验
	value = COM_DLCR;
	value  = value & (~ULCON_WL_MASK);
	value  = value | ULCON_WL8 | ULCON_STOP_1 | ULCON_PMD_NO;
	COM_DLCR = value;
   
	//设置串口波特率为指定的值
	baudrate = BSP_8250_FREQENCY / (16 * baud);
	value = COM_DLCR;
	COM_DLCR = (value | 0x80);			//使线路控制寄存器最高位DLAB=1
	COM_DDLL = ((unsigned char)baudrate);		//设置波特率
	COM_DDLH = ((unsigned char)(baudrate>>8));

	COM_DLCR = value;				//恢复线路控制寄存器最高位DLAB
	
	//启动PC COM卡的中断
	COM_DMCR = BSP_MCR_OUT2_IRQ_LINE;

	//清线路状态寄存器
	value = COM_DLSR;
	
	//清Moden状态寄存器
	value = COM_DMSR;
	
	//清数据接收寄存器
	value = COM_DRBR;
	
	//关闭串口相关的所有中断
	COM_DIER = BSP_IER_DISABLE_ALL;
  
}

/**
 * @brief
 *	  	从串口读取一个字符。
 *
 * @param[in] minor：串口设备号。
 *
 * @return 返回读取到的字符。
 */
T_CHAR monitorRawUartGetchar(T_UWORD minor, T_WORD *result)
{
	char value;
	value = 0;

	while(!(COM_DLSR & BSP_LSR_DATA_RECEIVED))
	{
		;
	}
	
	value = COM_DRBR;
	
	return value;
}

/**
 * @brief
 *	  	向串口填写一个字符。
 *
 * @param[in] minor：串口设备号。
 * @param[in] ch：填写的字符。
 *
 * @return 无。
 */
T_VOID monitorRawUartPutchar(T_UWORD minor, T_CHAR ch)
{	
	char value;
	value = 0;
	
	while(!(COM_DLSR & BSP_LSR_SEND_READY))
	{
		;
	}
	
	COM_DTHR = ch;


}

/**
 * @brief
 *	  	 打开串口的接收中断。
 *
 * @param[in] minor：串口设备号。
 * @param[in] bool：是否打开接收中断。
 *
 * @return 无。
 */
void monitorUartEnableRxInt(int minor, int bool)
{
	unsigned char value;
	
   	if(bool)
   	{
   		COM_DMCR = BSP_MCR_OUT2_IRQ_LINE;
   		value = COM_DIER;
		value = value | BSP_IER_RECEIVE_INTERRUPT;
		COM_DIER = value;
	}
	else
	{
		value = COM_DIER;
		value = value & (~BSP_IER_RECEIVE_INTERRUPT);
		COM_DIER = value;
        } 

}

/**
 * @brief
 *	  	 打开串口的发送中断。
 *
 * @param[in] minor：串口设备号。
 * @param[in] bool：是否打开发送中断。
 *
 * @return 无。
 */
void monitorUartEnableTxInt(int minor, int bool)
{
	unsigned char value;
	
   	if(bool)
   	{		
   		value = COM_DIER;
   		value = value | BSP_IER_SEND_OVER_INTERRUPT;
		COM_DIER = value;
	}
	else
	{   		
		value = COM_DIER;
    		value = value & (~BSP_IER_SEND_OVER_INTERRUPT);
		COM_DIER = value;
	}     

}

/**
 * @brief
 *	  	 获得串口的中断状态。
 *
 * @param[in] minor：串口设备号。
 *
 * @return 中断状态。
 */
char monitorUartGetIntStatus(int minor)
{
	char status;
	char value;
	
	status = COM_DIIR;

	if (0x0 == (status & (0x2 | 0x4)))
	{    	
		value = COM_DMSR;    	  //消除COM的MODEM状态

	}
	
	return status;

}

/**
 * @brief
 *	  	 串口一的接收中断服务程序。
 *
 * @param[in] callback：函数指针，中断程序。
 *
 * @return 无。
 */
void monitorUartCom1Isr(void(*callback)(void))
{
	char status;
	char cnt;
	cnt = 0;

	status = COM_DIIR;

	callback();
	
	return;    
}

/**
 * @brief
 *	  	 串口二的接收中断服务程序。
 *
 * @param[in] callback：函数指针，中断程序。
 *
 * @return 无。
 */
void monitorUartCom2Isr(void(*callback)(void))
{
   /*char status,cnt = 0;
    status = fnTA_uart_get_int_status(TA_UART_COM2_MINOR);
	callback();
	return;
	*/
}

/**
 * @brief
 *	  	 获得串口的中断向量号。
 *
 * @param[in] minor：串口设备号。
 *
 * @return 中断向量号。
 */
int  monitorUartGetRxVector(int minor)
{
	if (TA_UART_COM_MINOR  == minor)
	{
		return TA_UART_COM_RX_IRQ;
	}
	else
	{
		return FAIL;
	}
}
