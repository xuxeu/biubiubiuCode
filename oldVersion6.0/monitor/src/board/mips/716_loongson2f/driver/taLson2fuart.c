/****************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 *
 * 		Copyright (C) 2000-2008 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * �޸ļ�¼��
 *	20080310	�����ļ���BFC0 0000 
 *
 */

/**
 * @file 	taLson2euart.inc
 * @brief
 *	<li>���ܣ� ��о2E������������</li>
 * @date 	20080310
 */

/**************************** ���ò��� *****************************************/
#include <sysAgent.h>
#include "memory.h"
#include "taLson2fuart.h"

/**************************** ���岿�� *****************************************/

/**************************** ʵ�ֲ��� *****************************************/ 
/**
 * @brief
 *	  	���ô������ԡ�
 *
 * @param[in] minor�������豸�š�
 * @param[in] baud�������ʡ�
 *
 * @return �ޡ�
 */
T_VOID monitorRawUartSetAttribute(T_UWORD minor, T_UWORD baud)
{
	unsigned char value;
	unsigned int baudrate;
	value = 0;
	baudrate = 0;
	
	//�ر��豸(�����ж��޹ؼĴ������豸û�������ļĴ������˴�������PC COM�����жϣ�
	value = COM_DMCR;
	value = (value & (~BSP_MCR_OUT2_IRQ_LINE));
	COM_DMCR = value;
	
	//���ô���Ϊÿ�ַ�8λ,1λֹͣλ,��У��
	value = COM_DLCR;
	value  = value & (~ULCON_WL_MASK);
	value  = value | ULCON_WL8 | ULCON_STOP_1 | ULCON_PMD_NO;
	COM_DLCR = value;
   
	//���ô��ڲ�����Ϊָ����ֵ
	baudrate = BSP_8250_FREQENCY / (16 * baud);
	value = COM_DLCR;
	COM_DLCR = (value | 0x80);			//ʹ��·���ƼĴ������λDLAB=1
	COM_DDLL = ((unsigned char)baudrate);		//���ò�����
	COM_DDLH = ((unsigned char)(baudrate>>8));

	COM_DLCR = value;				//�ָ���·���ƼĴ������λDLAB
	
	//����PC COM�����ж�
	COM_DMCR = BSP_MCR_OUT2_IRQ_LINE;

	//����·״̬�Ĵ���
	value = COM_DLSR;
	
	//��Moden״̬�Ĵ���
	value = COM_DMSR;
	
	//�����ݽ��ռĴ���
	value = COM_DRBR;
	
	//�رմ�����ص������ж�
	COM_DIER = BSP_IER_DISABLE_ALL;
  
}

/**
 * @brief
 *	  	�Ӵ��ڶ�ȡһ���ַ���
 *
 * @param[in] minor�������豸�š�
 *
 * @return ���ض�ȡ�����ַ���
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
 *	  	�򴮿���дһ���ַ���
 *
 * @param[in] minor�������豸�š�
 * @param[in] ch����д���ַ���
 *
 * @return �ޡ�
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
 *	  	 �򿪴��ڵĽ����жϡ�
 *
 * @param[in] minor�������豸�š�
 * @param[in] bool���Ƿ�򿪽����жϡ�
 *
 * @return �ޡ�
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
 *	  	 �򿪴��ڵķ����жϡ�
 *
 * @param[in] minor�������豸�š�
 * @param[in] bool���Ƿ�򿪷����жϡ�
 *
 * @return �ޡ�
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
 *	  	 ��ô��ڵ��ж�״̬��
 *
 * @param[in] minor�������豸�š�
 *
 * @return �ж�״̬��
 */
char monitorUartGetIntStatus(int minor)
{
	char status;
	char value;
	
	status = COM_DIIR;

	if (0x0 == (status & (0x2 | 0x4)))
	{    	
		value = COM_DMSR;    	  //����COM��MODEM״̬

	}
	
	return status;

}

/**
 * @brief
 *	  	 ����һ�Ľ����жϷ������
 *
 * @param[in] callback������ָ�룬�жϳ���
 *
 * @return �ޡ�
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
 *	  	 ���ڶ��Ľ����жϷ������
 *
 * @param[in] callback������ָ�룬�жϳ���
 *
 * @return �ޡ�
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
 *	  	 ��ô��ڵ��ж������š�
 *
 * @param[in] minor�������豸�š�
 *
 * @return �ж������š�
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
