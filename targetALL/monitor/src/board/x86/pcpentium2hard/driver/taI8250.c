
/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taI8250.c
 * @brief:
 *             <li>ʵ��i8250�Ĵ�������</li>
 */

#ifdef CONFIG_TA_DISP_UART

/************************ͷ �� ��******************************/
#include "taIoAccess.h"
#include "taI8250.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *      ��ʼ������̨
 * @return:
 *      ��
 */
void taDisplayDeviceOpen(void)
{
	T_CHAR value;
	T_UWORD	baudrate;

	BSP_OutByte(COM_DIER1, IER_DISABLE_ALL);

	/* ����·״̬�Ĵ��� */
	BSP_InByte(COM_DLSR1, value);

	/* ��Moden״̬�Ĵ��� */
	BSP_InByte(COM_DMSR1, value);

	/* �����ݽ��ռĴ��� */
	BSP_InByte(COM_DRBR1, value);

	/* �������ݸ�ʽ�������� */
	BSP_OutByte(COM_DLCR1, ULCON_WL8 | ULCON_STOP_2 | ULCON_PMD_NO);

	/* ʹ��·���ƼĴ������λDLAB=1 */
	BSP_InByte(COM_DLCR1, value);
	BSP_OutByte(COM_DLCR1, value|0x80);
    
    /* ���㲨���� */
	baudrate = FREQENCY_8250 / (16 * 115200);

	/* ���ò����� */
	BSP_OutByte(COM_DDLL1, (T_UBYTE)baudrate );
	BSP_OutByte(COM_DDLH1, (T_UBYTE)(baudrate>>8));

	/* �ָ���·���ƼĴ������λDLAB */
	BSP_OutByte(COM_DLCR1, value);
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
	T_CHAR value;

	do
	{
		BSP_InByte(COM_DLSR1, value);

	}while(!(value & LSR_SEND_READY));

	BSP_OutByte(COM_DTHR1, ch);
}
#endif
