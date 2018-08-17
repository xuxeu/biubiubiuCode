/****************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 *
 * 		Copyright (C) 2000-2008 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * �޸ļ�¼��
 *	20080311	�����ļ���
 *
 */

/**
 * @file 	taLson2eintr.h
 * @brief
 *	<li>���� ��о2E�ж��豸��������ͷ�ļ�</li>
 * @date 	20080311
 */
#ifndef _TA_LSON2EINTR_H
#define _TA_LSON2EINTR_H

#ifdef __cplusplus
extern "C" {
#endif

////�жϿ���������
#define PIC_MASTER_BASE   0x20 + 0xbfd00000
#define PIC_SLAVE_BASE    0xa0 + 0xbfd00000
#define PIC_REG_INTERVAL  0x1

//PIC����Ĵ���
#define PIC_MASTER_COMMAND_IO_PORT *(volatile unsigned char *)(PIC_MASTER_BASE + 0 )	                //Master PIC ����Ĵ���
#define PIC_SLAVE_COMMAND_IO_PORT  *(volatile unsigned char *)(PIC_SLAVE_BASE + 0 )	                //SALVE PIC ����Ĵ���
#define PIC_MASTER_IMR_IO_PORT     *(volatile unsigned char *)(PIC_MASTER_BASE + PIC_REG_INTERVAL )	//Master PIC �ж�Mask�Ĵ���
#define PIC_SLAVE_IMR_IO_PORT      *(volatile unsigned char *)(PIC_SLAVE_BASE + PIC_REG_INTERVAL )	//Slave PIC �ж�Mask�Ĵ���

/////����������ַ
#define PIC_MASTER_ICW1_IO_PORT  *(volatile unsigned char *)(PIC_MASTER_BASE + 0)
#define PIC_SLAVE_ICW1_IO_PORT	  *(volatile unsigned char *)(PIC_SLAVE_BASE + 0 )
#define PIC_MASTER_ICW2_IO_PORT  *(volatile unsigned char *)(PIC_MASTER_BASE + PIC_REG_INTERVAL)
#define PIC_SLAVE_ICW2_IO_PORT	  *(volatile unsigned char *)(PIC_SLAVE_BASE + PIC_REG_INTERVAL )
#define PIC_MASTER_ICW3_IO_PORT  *(volatile unsigned char *)(PIC_MASTER_BASE + PIC_REG_INTERVAL)
#define PIC_SLAVE_ICW3_IO_PORT	  *(volatile unsigned char *)(PIC_SLAVE_BASE + PIC_REG_INTERVAL )
#define PIC_MASTER_ICW4_IO_PORT  *(volatile unsigned char *)(PIC_MASTER_BASE + PIC_REG_INTERVAL)
#define PIC_SLAVE_ICW4_IO_PORT	  *(volatile unsigned char *)(PIC_SLAVE_BASE + PIC_REG_INTERVAL )
#define PIC_MASTER_OCW1_IO_PORT  *(volatile unsigned char *)(PIC_MASTER_BASE + PIC_REG_INTERVAL)
#define PIC_SLAVE_OCW1_IO_PORT	  *(volatile unsigned char *)(PIC_SLAVE_BASE + PIC_REG_INTERVAL )
#define PIC_MASTER_OCW2_IO_PORT  *(volatile unsigned char *)(PIC_MASTER_BASE + 0)
#define PIC_SLAVE_OCW2_IO_PORT	  *(volatile unsigned char *)(PIC_SLAVE_BASE + 0 )
#define PIC_MASTER_OCW3_IO_PORT  *(volatile unsigned char *)(PIC_MASTER_BASE + 0)
#define PIC_SLAVE_OCW3_IO_PORT	  *(volatile unsigned char *)(PIC_SLAVE_BASE + 0 )

///Command for specific EOI (End Of Interrupt): Interrupt acknowledge
#define PIC_EOSI	0x60	// End of Specific Interrupt (EOSI)
#define PIC_EOI		0x20	// Generic End of Interrupt (EOI)


#ifdef __cplusplus
}
#endif

#endif
