/****************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *
 *          Copyright (C) 2000-2007 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * �޸ļ�¼��
 * 20090824    �����ļ� 
 */

/**
 * @file     ta_8641_uart.c
 * @brief
 * 		���ܣ�ʵ��MPC8641��������������оƬ����NS16550
 * @date    20090824
 */


/**************************** ���ò��� *****************************************/
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
/*************************** ǰ���������� ****************************************/
/*************************** �ⲿ�������� ****************************************/
/**************************** ���岿�� *******************************************/
#define		CONFIG_DRIVERS_SERIAL_TSI109UART		{\
		{("COM1"),(DR_UART_NO_PARITY),(8),(115200),(1),(DR_UART_POLLING)},\
		{("COM2"),(DR_UART_NO_PARITY),(8),(115200),(1),(DR_UART_POLLING)}		}

#ifdef CONFIG_DRIVERS_SERIAL_TSI109UART
	T_DR_UartConfigTable utDR_UartConfigTable[] = CONFIG_DRIVERS_SERIAL_TSI109UART;
#endif

char com1_ier;
char com2_ier;

#define MPC8641_REG_BASE_ADDR	 CCSRBAR
#define MPC8641_UART0_OFFSET	0x4500			/*����1�Ŀ��ַ*/
#define MPC8641_UART1_OFFSET	0x4600			/*����2�Ŀ��ַ*/

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

#define MPX_CLOCK (550000000) //Ƶ��Ϊ400M

/**************************** ʵ�ֲ��� ******************************************/
/**
 * @brief
 *    ����:
 *        ��ȡ�豸����
 *
 *  ʵ�����ݣ�
 *        ��ȡ�����豸������
 *
 * @return �����豸����
 */
T_WORD BSP_UartGetDeviceNum()
{
	return sizeof(utDR_UartConfigTable)/sizeof(T_DR_UartConfigTable);	
}

/**
 * @brief
 *    ����:
 *        ���ڳ�ʼ��������
 *
 *  ʵ�����ݣ�
 *        ʵ�ֶԴ���ģ���ʼ��,���ø����Ĵ���
 *         ���ò����ʣ�����ֹͣλ������λ��Ч��λ��
 *         ���ù���ģʽ�жϻ�����ѯ
 *
 * @param[in]  minor�������豸��
 * @param[in]  config��ָ�룬�豸������Ϣ
 * @param[in]  isr���жϷ������
 * @return �ޡ�
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
	
	//����λ��
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
	
	//ֹͣλ��
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
	
	//��żУ��ѡ��
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
	
	(*(volatile unsigned char*)(addr + LCR)) = (LCR_DLAB | data_len | stop_bit | parity);/*ʹ��DMB DLB*/
	EIEIO_SYNC;
	(*(volatile unsigned char*)(addr + FCR)) = (RxCLEAR | TxCLEAR);/*disable FIFO*/
	EIEIO_SYNC;

	(*(volatile unsigned char*)(addr + DLB)) = (MPX_CLOCK/16/(115200))&0xFF;	/*������115200*/		//(UART_XTAL/(2*config->baudrate));
	EIEIO_SYNC;
	(*(volatile unsigned char*)(addr + DMB)) = (MPX_CLOCK/16/(115200))>>8;						//((UART_XTAL/(2*config->baudrate)) >> 8);
	EIEIO_SYNC;

	(*(volatile unsigned char*)(addr + LCR)) = (data_len | stop_bit | parity);/*����ֹͣλ������λ��Ч��λ*/
	EIEIO_SYNC;
	
	(*(volatile unsigned char*)(addr + IER)) = 0;
	EIEIO_SYNC;
	
	if(config->mode == DR_UART_INTERRUPT)/*�ж�ģʽ*/
	{
		//ʹ�ܽ����ж�
		(*(volatile unsigned char*)(addr + IER)) = IER_ERDAI;
		EIEIO_SYNC;
	}

    

}

/**
 * @brief
 *    ����:
 *        �رմ��ڡ�
 *
 *  ʵ�����ݣ�
 *        ��ֹ�жϣ�ж���жϷ�������жϷ�ʽ�����ر��豸��
 *
 * @param[in]  minor�������豸��
 *
 * @return �ޡ�
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
	//��ֹ�ж�
	(*(volatile unsigned char*)(addr + IER)) = 0;
	EIEIO_SYNC;

}

/**
 * @brief
 *    ����:
 *        �Ӵ��ڷ������ݣ���ѯ��ʽ����
 *
 *  ʵ�����ݣ�
 *        �ȴ����ڷ��Ϳ���
 *         д���ڷ��ͼĴ�����������
 * 
 * @param[in]  minor�������豸��
 * @param[in]  buf��Ҫ���͵�����
 * @param[in]  len���������ݳ���
 * 
 * @return ����0��
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
		//�ȴ�����׼����
   		while(!((*(volatile unsigned char*)(addr + LSR)) & LSR_THRE));
        EIEIO_SYNC;
		(*(volatile unsigned char*)(addr + THR)) = *buf++;
        EIEIO_SYNC;

	}
	
	return 0;

}

/**
 * @brief
 *    ����:
 *        �Ӵ��ڻ�ȡ�ַ�����ѯ��ʽ����
 *
 *  ʵ�����ݣ�
 *        �ȴ����ڽ��յ��ַ�
 *         �Ӵ��ڽ��ռĴ��������ַ�
 *
 * @param[in]  minor�������豸��
 *
 * @return ���ض�ȡ���ַ���
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

   //�ȴ����յ��ַ�
	while(!((*(volatile unsigned char*)(addr + LSR)) & LSR_DR));
    EIEIO_SYNC;
    ch = (*(volatile unsigned char*)(addr + RBR));
    EIEIO_SYNC;

   return (T_WORD)ch;


}

/**
 * @brief
 *    ����:
 *        �Ӵ��ڷ����ַ���
 *
 *  ʵ�����ݣ�
 *         д���ڷ��ͼĴ��������ַ�
 * 
 * @param[in]  minor�������豸��
 * @param[in]  ch��Ҫ���͵��ַ�
 * 
 * @return �ޡ�
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
 *    ����:
 *        ��ֹ�����жϡ�
 *
 *  ʵ�����ݣ�
 *         ��ֹ���ڽ����ж�
 *
 * @param[in]  minor�������豸��
 *
 * @return �ޡ�
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
 *    ����:
 *        �Ӵ��ڻ�ȡ�ַ���
 *
 *  ʵ�����ݣ�
 *         �Ӵ��ڽ��ռĴ��������ַ�
 *
 * @param[in]  minor�������豸��
 *
 * @return ���ض�ȡ���ַ���
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
 *    ����:
 *        ʹ�ܽ����жϡ�
 *
 *  ʵ�����ݣ�
 *         ʹ�ܴ��ڽ����ж�
 *
 * @param[in]  minor�������豸��
 *
 * @return �ޡ�
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
 *    ����:
 *        ��ֹ�����жϡ�
 *
 *  ʵ�����ݣ�
 *         ��ֹ���ڷ����ж�
 *
 * @param[in]  minor�������豸��
 *
 * @return �ޡ�
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
 *    ����:
 *        ʹ�ܷ����жϡ�
 *
 *  ʵ�����ݣ�
 *         ʹ�ܴ��ڷ����ж�
 *
 * @param[in]  minor�������豸��
 *
 * @return �ޡ�
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
 *    ����:
 *        ��ȡ�ж�״̬��
 *
 *  ʵ�����ݣ�
 *         ��ȡ�豸�ж�״̬
 *
 * @param[in]  minor�������豸��
 *
 * @return �����ж�״̬�����ܵ�״̬���£�
 *            UART_SENDOVER_INTERRUPT �����ж�
 *            UART_RECEIVE_INTERRUPT  �����ж�
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
 *    ����:
 *        �Ӵ��ڻ�ȡ�ַ�����ѯ��ʽ����
 *
 *  ʵ�����ݣ�
 *        �ȴ����ڽ��յ��ַ�
 *         �Ӵ��ڽ��ռĴ��������ַ�
 *
 * @param[in]  minor�������豸��
 *
 * @return ���ض�ȡ���ַ���
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

    //�ȴ����յ��ַ�
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
 *      ��ʼ�������豸
 * @return:
 *      ��
 */
void taDisplayDeviceOpen(void)
{
	BSP_UartInit(0,utDR_UartConfigTable,NULL);
}

void taDisplayDeviceOpen8640(char *name)
{
	signed int  device_num, n;

		/* ��ȡ�����豸���� */
		device_num = BSP_UartGetDeviceNum();

		for (n=0; n<device_num; n++)
		{
			/* ������printk�豸��ƥ��Ĵ����豸 */
			if (strcmp( name, (const char *)(utDR_UartConfigTable[n].name)) == 0)
			{
				BSP_UartInit( n, &utDR_UartConfigTable[n], NULL );
				/* install printk handler */
			}
		}
}
/*
 * @brief:
 *      �����豸����ַ�
 * @param[in]: ch:�ַ�
 * @return:
 *      ��
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

