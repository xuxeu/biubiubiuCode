/****************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 *
 * 		Copyright (C) 2000-2008 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * �޸ļ�¼��
 *	20080310	�����ļ���
 *
 */

/**
 * @file 	taLson2euart.h
 * @brief
 *	<li>���ܣ� ���崮�ڵ���غ�ͺ�����������</li>
 * @date 	20080310
 */

#ifndef __SERIAL_TALSON2EUART_H__
#define __SERIAL_TALSON2EUART_H__

#ifdef	__cplusplus
extern	"C" {
#endif

/**************************** ���ò��� *****************************************/
#include <sysTypes.h>	/// ʹ��ϵͳ���Ͷ���
//#include <bspPubInfo.h>

/**************************** ���岿�� *****************************************/
//����ʱ��Ƶ��
#define BSP_8250_FREQENCY 1843200

#define COM_BASEADDR 0x3f8 + 0xbfd00000  //�豸����ַ

#define TA_UART_COM_RX_IRQ (0x4)         //�豸�ж�������

#define TA_UART_COM_MINOR 0              //���ڶ˿ںŶ���


#define COM_DLCR *(volatile unsigned char *)(COM_BASEADDR + 0x03) 	 //���ƼĴ���
#define COM_DLSR *(volatile unsigned char *)(COM_BASEADDR + 0x05)   //״̬�Ĵ���
#define COM_DMCR *(volatile unsigned char *)(COM_BASEADDR + 0x04) 	 //modem���ƼĴ���
#define COM_DMSR *(volatile unsigned char *)(COM_BASEADDR + 0x06) 	 //modem״̬�Ĵ���
#define COM_DIIR *(volatile unsigned char *)(COM_BASEADDR + 0x02) 	 //�жϼĴ���
#define COM_DIER *(volatile unsigned char *)(COM_BASEADDR + 0x01) 	 //�ж�ʹ�ܼĴ���
#define COM_DDLL *(volatile unsigned char *)(COM_BASEADDR + 0x00) 	 //��Ƶ�Ĵ���
#define COM_DDLH *(volatile unsigned char *)(COM_BASEADDR + 0x01) 	 //��Ƶ�Ĵ���
#define COM_DTHR *(volatile unsigned char *)(COM_BASEADDR + 0x00) 	//���ͼĴ���
#define COM_DRBR *(volatile unsigned char *)(COM_BASEADDR + 0x00) 	//���ռĴ���

///���ƼĴ�������
//bit 0-1 : �ֳ�
#define	ULCON_WL5               0x00
#define	ULCON_WL6               0x01
#define	ULCON_WL7               0x02
#define	ULCON_WL8               0x03
#define ULCON_WL_MASK			0X03

//bit 2 : ֹͣλ
#define	ULCON_STOP_2            0x04
#define ULCON_STOP_1            0x00
#define ULCON_STOP_MASK         0x04

//bit 3-5 : У��ģʽ
#define	ULCON_PMD_NO            0x00  //��У��
#define	ULCON_PMD_ODD           0x08  //��Ч��
#define	ULCON_PMD_EVEN          0x18  //żЧ��
#define	ULCON_PMD_CHK1          0x28  //ǿ��Ч����1
#define	ULCON_PMD_CHK0          0x38  //ǿ��Ч����0
#define	ULCON_PMD_MASK          0x38

//�ж�ʹ�ܼĴ�������
#define BSP_IER_RECEIVE_INTERRUPT       0x01  //bit 0: ���������ж�
#define BSP_IER_SEND_OVER_INTERRUPT     0x02  //bit 1�����������ж�
#define BSP_IER_DATA_ERROR_INTERRUPT    0x04  //bit 2���������ݴ�����ж�
#define BSP_IER_MODEM_INTERRUPT         0x08  //bit 3 ��Modem״̬���ı䣩�ж�
#define BSP_IER_DISABLE_ALL				0x00  //��ֹ�����ж�
#define BSP_IER_ENABLE_ALL				0x0f  //���������ж�

//״̬�Ĵ�������
#define BSP_LSR_DATA_RECEIVED  		0x01  //bit 0�����ݽ��վ���
#define BSP_LSR_RECEIVE_BUSY   		0x02  //bit 1�������
#define BSP_LSR_PARITY_ERROR  		0x04  //bit 2����ż��
#define BSP_LSR_DATA_FORMAT_ERROR	0x08  //bit 3��֡��
#define BSP_LSR_STOP_SIGNAL         0x10  //bit 4����ֹ���
#define BSP_LSR_SEND_READY			0x20  //bit 5�����ͱ��ּĴ�����
#define BSP_LSR_DATA_TO_LINE		0x40  //bit 6��������λ�Ĵ�����

//Modem���ƼĴ�������
#define BSP_MCR_TERMINAL_READY		0x01  //bit 0: DTR
#define BSP_MCR_REQUEST_SEND		0x02  //bit 1: RTS
#define BSP_MCR_OUT1_IRQ_LINE		0x04  //bit 2: OUT1
#define BSP_MCR_OUT2_IRQ_LINE		0x08  //bit 3: OUT2����PC COM�����ж�����
#define BSP_MCR_LOOP_TEST			0x10  //bit 4: LOOP���Լ�


#ifdef	__cplusplus
}
#endif

#endif
