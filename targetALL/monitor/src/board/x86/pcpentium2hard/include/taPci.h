
/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taPci.h
 * @brief:
 *             <li>PCI����غ�Ķ���ͺ���������</li>
 */
#ifndef TA_PCI_H_
#define TA_PCI_H_

/************************ͷ�ļ�********************************/
#include "taTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************�궨��********************************/

#define  PCI_VENDOR_ID     			0x00 /* 16λ�ĳ���ID��ƫ�Ƶ�ַ */
#define  PCI_DEVICE_ID     			0x02 /* 16λ���豸ID��ƫ�Ƶ�ַ */

#define  PCI_COMMAND     			0x04 /* 16λ������Ĵ�����ƫ�Ƶ�ַ */
#define  PCI_COMMAND_IO     		0x1  /* ��Ӧ����Ĵ�����λ0��������ӦI/O�ռ�ķ��� */
#define  PCI_COMMAND_MEMORY    		0x2  /* ��Ӧ����Ĵ�����λ1��������ӦMemory�ռ�ķ��� */
#define  PCI_COMMAND_MASTER    		0x4  /* ��Ӧ����Ĵ�����λ2�������豸��Ϊ�������豸 */
#define  PCI_COMMAND_SPECIAL   		0x8  /* ��Ӧ����Ĵ�����λ3�������豸�μ������ϵ��������ڲ��� */
#define  PCI_COMMAND_INVALIDATE   	0x10 /* ��Ӧ����Ĵ�����λ4�������豸�����洢��д����Ч���� */
#define  PCI_COMMAND_VGA_PALETTE   	0x20 /* ��Ӧ����Ĵ�����λ5�������豸��������ĵ�ɫ����� */
#define  PCI_COMMAND_PARITY    		0x40 /* ��Ӧ����Ĵ�����λ6�������豸�ڷ�����żУ���ʱ��ȡ��ʩ */
/* #define  PCI_COMMAND_WAIT    0x80 */ /* ��Ӧ����Ĵ�����λ7�������� */
#define  PCI_COMMAND_SERR    		0x100 /* ��Ӧ����Ĵ�����λ8������SERR#�������� */
#define  PCI_COMMAND_FAST_BACK   	0x200 /* ��Ӧ����Ĵ�����λ9�������豸�Բ�ͬ��Ŀ���豸�������ٱ��Ա����� */
#define  PCI_COMMAND_INTERRUPT_DISABLE 	0x400 /* ��Ӧ����Ĵ�����λ10����ֹʹ��INTX#�ź� */

#define  PCI_STATUS      			0x06 /* 16λ״̬�Ĵ�����ƫ�Ƶ�ַ*/
#define	 PCI_STATUS_INT	 			0x08 /*�ж�״̬λ�������ж�ʱ��λ��1����������Ĵ�����Ӱ��*/
#define  PCI_STATUS_CAP_LIST		0x10	/* Support Capability List */
#define  PCI_STATUS_66MHZ    		0x20 /* ��Ӧ״̬�Ĵ�����λ5���豸��������66MHz */
/*#define  PCI_STATUS_UDF    0x40 */ /* ��Ӧ״̬�Ĵ�����λ6������ */
#define  PCI_STATUS_FAST_BACK   	0x80 /* ��Ӧ״̬�Ĵ�����λ7���豸���Խ��ܿ��ٱ��Ա����� */
#define  PCI_STATUS_PARITY    		0x100 /* ��Ӧ״̬�Ĵ�����λ8��������ż������ */
#define  PCI_STATUS_DEVSEL_MASK   	0x600 /* ��Ӧ״̬�Ĵ�����λ9��λ10���룬��ʾDEVSEL�źŵ�ʱ���ϵ */
#define  PCI_STATUS_DEVSEL_FAST   	0x000 /* ���� */
#define  PCI_STATUS_DEVSEL_MEDIUM   0x200 /* ���� */
#define  PCI_STATUS_DEVSEL_SLOW   	0x400 /* ���� */
#define  PCI_STATUS_SIG_TARGET_ABORT	0x800  /* ��Ӧ״̬�Ĵ�����λ11������Ŀ���ֹ */
#define  PCI_STATUS_REC_TARGET_ABORT	0x1000  /* ��Ӧ״̬�Ĵ�����λ12������Ŀ���ֹ */
#define  PCI_STATUS_REC_MASTER_ABORT	0x2000  /* ��Ӧ״̬�Ĵ�����λ13���������豸��ֹ */
#define  PCI_STATUS_SIG_SYSTEM_ERROR	0x4000  /* ��Ӧ״̬�Ĵ�����λ14��ϵͳ���� */
#define  PCI_STATUS_DETECTED_PARITY		0x8000  /* ��Ӧ״̬�Ĵ�����λ15����żУ����� */

#define  PCI_CLASS_REVISION    		0x08 /* ������޶��汾��ƫ�Ƶ�ַ����24λ�Ƿ�����룬��8λ���޶��汾 */
#define  PCI_REVISION_ID           	0x08    /* �޶��汾�Ĵ�����ƫ�Ƶ�ַ */
#define  PCI_CLASS_PROG            	0x09    /* �Ĵ�����̽ӿڼĴ�����ƫ�Ƶ�ַ */
#define  PCI_CLASS_DEVICE          	0x0a    /* �豸����Ĵ�����ƫ�Ƶ�ַ */

#define  PCI_CACHE_LINE_SIZE   		0x0c /* 8λ�Ļ����д�С�Ĵ�����ƫ�Ƶ�ַ */
#define  PCI_LATENCY_TIMER    		0x0d /* 8λ����ʱ�����Ĵ�����ƫ�Ƶ�ַ*/
#define  PCI_HEADER_TYPE    		0x0e /* 8λ��ͷ�����ͼĴ�����ƫ�Ƶ�ַ */
#define  PCI_HEADER_TYPE_NORMAL		0
#define  PCI_HEADER_TYPE_BRIDGE 	1
#define  PCI_HEADER_TYPE_CARDBUS 	2

/*	added by chengw, refer to Vxworks, PCI_CFG_MODE, SLEEP_MODE_DIS	*/
#define PCI_MODE	0x43
#define PCI_MODE_SLEEP_DIS	0x00


/*************************************************************
*PCI_HEADER_TYPE: pci bridge
****************************************************************/
#define PCI_PCIB_MEMORY_BASE  	0x20
#define PCI_PCIB_MEMORY_LIMIT 	0x22
#define PCI_PCIB_IO_BASE		0x1c
#define PCI_PCIB_IO_LIMIT		0x1d
#define PCI_PCIB_PREMEM_BASE	0x24
#define PCI_PCIB_PREMEM_LIMIT	0x26
#define PCI_PCIB_PREMEM_UP32BASE   0x28
#define PCI_PCIB_PREMEM_UP32LIMIT 0x2c
#define PCI_PCIB_IO_UP16BASE    0x30
#define PCI_PCIB_IO_UP16LIMIT	0x32
/*****************************************************************/
#define  PCI_BIST      0x0f /* 8λ���ں��Բ��ԼĴ�����ƫ�Ƶ�ַ */
#define  PCI_BIST_CODE_MASK    0x0f /* �ں��Բ��ԼĴ�����ɴ��루λ3��λ0�������� */
#define  PCI_BIST_START     0x40 /* ��Ӧ�ں��Բ��ԼĴ�����λ6�������ں��Բ��� */
#define  PCI_BIST_CAPABLE    0x80 /* ��Ӧ�ں��Բ��ԼĴ�����λ7��֧��BIST���� */

/*
 * 32λ�Ļ�ַ�Ĵ���
 */
#define  PCI_BASE_ADDRESS_0    0x10 /* 32λ */
#define  PCI_BASE_ADDRESS_1    0x14 /* 32λ */
#define  PCI_BASE_ADDRESS_2    0x18 /* 32λ */
#define  PCI_BASE_ADDRESS_3    0x1c /* 32λ */
#define  PCI_BASE_ADDRESS_4    0x20 /* 32λ */
#define  PCI_BASE_ADDRESS_5    0x24 /* 32λ */
#define  PCI_BASE_ADDRESS_SPACE   0x01 /* ��Ӧ��ַ�Ĵ�����λ0�������Ǻ��ֵ�ַ�ռ䣺0 = memory, 1 = I/O */
#define  PCI_BASE_ADDRESS_SPACE_IO   0x01 /* ��ʾI/O��ַ�ռ� */
#define  PCI_BASE_ADDRESS_SPACE_MEMORY  0x00 /* ��ʾmemory��ַ�ռ� */
#define  PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06 /* ��Ӧ��ַ�Ĵ�����λ2��λ1������memoryӳ������ */
#define  PCI_BASE_ADDRESS_MEM_TYPE_32 0x00 /* ӳ�䵽32λ��ַ�ռ� */
/*#define  PCI_BASE_ADDRESS_MEM_TYPE_1M 0x02 */ /* Below 1M */
#define  PCI_BASE_ADDRESS_MEM_TYPE_64 0x04 /* ӳ�䵽64λ��ַ�ռ� */
#define  PCI_BASE_ADDRESS_MEM_PREFETCH 0x08 /* ��Ӧ��ַ�Ĵ�����λ3����ʾԤȡ�� */
#define  PCI_BASE_ADDRESS_MEM_MASK  (~0x0f) /* memory��ַ������ */
#define  PCI_BASE_ADDRESS_IO_MASK  (~0x03) /* I/O��ַ������ */

#define  PCI_CARDBUS_CIS    0x28 /* CardBus CISָ��Ĵ�����ƫ�Ƶ�ַ */
#define  PCI_SUBSYSTEM_ID    0x2e /* ��ϵͳʶ��Ĵ�����ƫ�Ƶ�ַ */
#define  PCI_SUBSYSTEM_VENDOR_ID  0x2c /* ��ϵͳ����ʶ��Ĵ���ƫ�Ƶ�ַ */
#define  PCI_ROM_ADDRESS    0x30 /* 32λ��չROM��ַ�Ĵ�����ƫ�Ƶ�ַ */
#define  PCI_ROM_ADDRESS_ENABLE   0x01 /* ��Ӧ��չROM��ַ�Ĵ���λ0:
             * 0 = ��ֹ�����豸����չROM��
             * 1 = ��������豸����չROM��
                * λ31��λ11��ʾ��ַ,
                * λ10��λ2����
                */

#define  PCI_CAPABILITIES_POINTER  0x34 /* 8λ������ָ�� */
/* Header type 2 (CardBus bridges) */
#define PCI_CB_CAPABILITY_LIST	0x14
/* Capability lists */
#define PCI_CAP_LIST_ID		0	/* Capability ID */
#define  PCI_CAP_ID_PM		0x01	/* Power Management */
#define  PCI_CAP_ID_AGP		0x02	/* Accelerated Graphics Port */
#define  PCI_CAP_ID_VPD		0x03	/* Vital Product Data */
#define  PCI_CAP_ID_SLOTID	0x04	/* Slot Identification */
#define  PCI_CAP_ID_MSI		0x05	/* Message Signalled Interrupts */
#define  PCI_CAP_ID_CHSWP	0x06	/* CompactPCI HotSwap */
#define PCI_CAP_LIST_NEXT	1	/* Next capability in the list */
#define PCI_CAP_FLAGS		2	/* Capability defined flags (16 bits) */
#define PCI_CAP_SIZEOF		4



/* 0x35-0x3b are reserved */
#define  PCI_INTERRUPT_LINE    0x3c /* 8λ���ж��߼Ĵ��� */
#define  PCI_INTERRUPT_PIN    0x3d /* 8λ���ж����żĴ��� */
#define  PCI_MIN_GNT     0x3e /* 8λ��Min-Gnt */
#define  PCI_MAX_LAT     0x3f /* 8λ��Max-Lat */


/***********************************************************
 *      PCI�豸�ķ�����루�д����䣩
 ***********************************************************/
 #define PCI_ANY_ID 0xffff
#define  PCI_CLASS_NOT_DEFINED   0x0000 /* δ������豸 */
#define  PCI_CLASS_NOT_DEFINED_VGA  0x0001 /* δ�����VGA�豸 */

#define  PCI_BASE_CLASS_STORAGE   0x01 /* �����洢������������ */
#define  PCI_CLASS_STORAGE_SCSI   0x0100 /* SCSI������ */
#define  PCI_CLASS_STORAGE_IDE   0x0101 /* IDE������ */
#define  PCI_CLASS_STORAGE_FLOPPY  0x0102 /* ���̿����� */
#define  PCI_CLASS_STORAGE_IPI   0x0103 /* IPI������ */
#define  PCI_CLASS_STORAGE_RAID   0x0104 /*  */
#define  PCI_CLASS_STORAGE_OTHER  0x0180 /* ���������� */

#define  PCI_BASE_CLASS_NETWORK   0x02 /* ��������������� */
#define  PCI_CLASS_NETWORK_ETHERNET  0x0200 /* ��̫�������� */
#define  PCI_CLASS_NETWORK_TOKEN_RING 0x0201 /* ������������ */
#define  PCI_CLASS_NETWORK_FDDI   0x0202 /* FDDI������ */
#define  PCI_CLASS_NETWORK_ATM   0x0203 /*  */
#define  PCI_CLASS_NETWORK_OTHER  0x0280 /* ���������� */

#define  PCI_BASE_CLASS_DISPLAY   0x03 /* ��ʾ������������ */
#define  PCI_CLASS_DISPLAY_VGA   0x0300 /* VGA���ݿ����� */
#define  PCI_CLASS_DISPLAY_XGA   0x0301 /* XGA������ */
#define  PCI_CLASS_DISPLAY_OTHER  0x0380 /* ������ʾ������ */

#define  PCI_BASE_CLASS_MULTIMEDIA  0x04 /* ��ý���豸�Ļ����� */
#define  PCI_CLASS_MULTIMEDIA_VIDEO  0x0400 /* ��Ƶ�豸 */
#define  PCI_CLASS_MULTIMEDIA_AUDIO  0x0401 /* ��Ƶ�豸 */
#define  PCI_CLASS_MULTIMEDIA_OTHER  0x0480 /* ������ý���豸 */

#define  PCI_BASE_CLASS_MEMORY   0x05 /* �洢�������Ļ����� */
#define  PCI_CLASS_MEMORY_RAM   0x0500 /* RAM */
#define  PCI_CLASS_MEMORY_FLASH   0x0501 /* FLASH */
#define  PCI_CLASS_MEMORY_OTHER   0x0580 /* ���� */

#define  PCI_BASE_CLASS_BRIDGE   0x06 /* ��·�豸�Ļ����� */
#define  PCI_CLASS_BRIDGE_HOST   0x0600 /* ���� */
#define  PCI_CLASS_BRIDGE_ISA   0x0601 /* ISA�� */
#define  PCI_CLASS_BRIDGE_EISA   0x0602 /* EISA�� */
#define  PCI_CLASS_BRIDGE_MC   0x0603 /* MC�� */
#define  PCI_CLASS_BRIDGE_PCI   0x0604 /* PCI-PCI�� */
#define  PCI_CLASS_BRIDGE_PCMCIA  0x0605 /* PCMCIA�� */
#define  PCI_CLASS_BRIDGE_NUBUS  0x0606 /*  */
#define  PCI_CLASS_BRIDGE_CARDBUS  0x0607 /*  */
#define  PCI_CLASS_BRIDGE_OTHER   0x0680 /* ���� */


#define  PCI_BASE_CLASS_COMMUNICATION  0x07 /* ͨ�ſ����������� */
#define  PCI_CLASS_COMMUNICATION_SERIAL  0x0700 /* ����ͨ�ſ����� */
#define  PCI_CLASS_COMMUNICATION_PARALLEL  0x0701 /* ����ͨ�ſ����� */
#define  PCI_CLASS_COMMUNICATION_OTHER  0x0780 /* ���� */

#define  PCI_BASE_CLASS_SYSTEM   0x08 /* ϵͳ��Χ�豸������ */
#define  PCI_CLASS_SYSTEM_PIC   0x0800 /* PIC */
#define  PCI_CLASS_SYSTEM_DMA   0x0801 /* DMA */
#define  PCI_CLASS_SYSTEM_TIMER   0x0802 /* ��ʱ�� */
#define  PCI_CLASS_SYSTEM_RTC   0x0803 /* RTC */
#define  PCI_CLASS_SYSTEM_OTHER   0x0880 /* ���� */

#define  PCI_BASE_CLASS_INPUT   0x09 /* �����豸������ */
#define  PCI_CLASS_INPUT_KEYBOARD  0x0900 /* ���� */
#define  PCI_CLASS_INPUT_PEN   0x0901 /* ���ֱ� */
#define  PCI_CLASS_INPUT_MOUSE   0x0902 /* ��� */
#define  PCI_CLASS_INPUT_OTHER   0x0980 /* ���� */

#define  PCI_BASE_CLASS_DOCKING   0x0a /* ͣ��״̬������ */
#define  PCI_CLASS_DOCKING_GENERIC  0x0a00 /* һ��ͣ��״̬ */
#define  PCI_CLASS_DOCKING_OTHER  0x0a01 /* ���� */

#define  PCI_BASE_CLASS_PROCESSOR  0x0b /* ������������ */
#define  PCI_CLASS_PROCESSOR_386  0x0b00 /* 386 */
#define  PCI_CLASS_PROCESSOR_486  0x0b01 /* 486 */
#define  PCI_CLASS_PROCESSOR_PENTIUM 0x0b02 /* Pentium */
#define  PCI_CLASS_PROCESSOR_ALPHA  0x0b10 /* Alpaha */
#define  PCI_CLASS_PROCESSOR_POWERPC 0x0b20 /* PowerPC */
#define  PCI_CLASS_PROCESSOR_CO   0x0b40 /* Co-Processor */

#define  PCI_BASE_CLASS_SERIAL   0x0c /* �������߿����������� */
#define  PCI_CLASS_SERIAL_FIREWIRE  0x0c00 /*  */
#define  PCI_CLASS_SERIAL_ACCESS  0x0c01 /* ACCESS���� */
#define  PCI_CLASS_SERIAL_SSA   0x0c02 /* SSA */
#define  PCI_CLASS_SERIAL_USB   0x0c03 /* USB */
#define  PCI_CLASS_SERIAL_FIBER   0x0c04 /* Fibre���� */

#define  PCI_CLASS_OTHERS    0xff /* ���� */


/***********************************************************
 *    �������ִ�С���еĳ���ID���豸ID
 ***********************************************************/

#define PCI_CLASS_NOT_DEFINED       0x0000
#define PCI_CLASS_NOT_DEFINED_VGA   0x0001

#define PCI_BASE_CLASS_STORAGE      0x01
#define PCI_CLASS_STORAGE_SCSI      0x0100
#define PCI_CLASS_STORAGE_IDE       0x0101
#define PCI_CLASS_STORAGE_FLOPPY    0x0102
#define PCI_CLASS_STORAGE_IPI       0x0103
#define PCI_CLASS_STORAGE_RAID      0x0104
#define PCI_CLASS_STORAGE_OTHER     0x0180

#define PCI_BASE_CLASS_NETWORK      0x02
#define PCI_CLASS_NETWORK_ETHERNET  0x0200
#define PCI_CLASS_NETWORK_TOKEN_RING    0x0201
#define PCI_CLASS_NETWORK_FDDI      0x0202
#define PCI_CLASS_NETWORK_ATM       0x0203
#define PCI_CLASS_NETWORK_OTHER     0x0280

#define PCI_BASE_CLASS_DISPLAY      0x03
#define PCI_CLASS_DISPLAY_VGA       0x0300
#define PCI_CLASS_DISPLAY_XGA       0x0301
#define PCI_CLASS_DISPLAY_3D        0x0302
#define PCI_CLASS_DISPLAY_OTHER     0x0380

#define PCI_BASE_CLASS_MULTIMEDIA   0x04
#define PCI_CLASS_MULTIMEDIA_VIDEO  0x0400
#define PCI_CLASS_MULTIMEDIA_AUDIO  0x0401
#define PCI_CLASS_MULTIMEDIA_PHONE  0x0402
#define PCI_CLASS_MULTIMEDIA_OTHER  0x0480

#define PCI_BASE_CLASS_MEMORY       0x05
#define PCI_CLASS_MEMORY_RAM        0x0500
#define PCI_CLASS_MEMORY_FLASH      0x0501
#define PCI_CLASS_MEMORY_OTHER      0x0580

#define PCI_BASE_CLASS_BRIDGE       0x06
#define PCI_CLASS_BRIDGE_HOST       0x0600
#define PCI_CLASS_BRIDGE_ISA        0x0601
#define PCI_CLASS_BRIDGE_EISA       0x0602
#define PCI_CLASS_BRIDGE_MC     0x0603
#define PCI_CLASS_BRIDGE_PCI        0x0604
#define PCI_CLASS_BRIDGE_PCMCIA     0x0605
#define PCI_CLASS_BRIDGE_NUBUS      0x0606
#define PCI_CLASS_BRIDGE_CARDBUS    0x0607
#define PCI_CLASS_BRIDGE_RACEWAY    0x0608
#define PCI_CLASS_BRIDGE_OTHER      0x0680

#define PCI_BASE_CLASS_COMMUNICATION    0x07
#define PCI_CLASS_COMMUNICATION_SERIAL  0x0700
#define PCI_CLASS_COMMUNICATION_PARALLEL 0x0701
#define PCI_CLASS_COMMUNICATION_MULTISERIAL 0x0702
#define PCI_CLASS_COMMUNICATION_MODEM   0x0703
#define PCI_CLASS_COMMUNICATION_OTHER   0x0780

#define PCI_BASE_CLASS_SYSTEM       0x08
#define PCI_CLASS_SYSTEM_PIC        0x0800
#define PCI_CLASS_SYSTEM_DMA        0x0801
#define PCI_CLASS_SYSTEM_TIMER      0x0802
#define PCI_CLASS_SYSTEM_RTC        0x0803
#define PCI_CLASS_SYSTEM_PCI_HOTPLUG    0x0804
#define PCI_CLASS_SYSTEM_OTHER      0x0880

#define PCI_BASE_CLASS_INPUT        0x09
#define PCI_CLASS_INPUT_KEYBOARD    0x0900
#define PCI_CLASS_INPUT_PEN     0x0901
#define PCI_CLASS_INPUT_MOUSE       0x0902
#define PCI_CLASS_INPUT_SCANNER     0x0903
#define PCI_CLASS_INPUT_GAMEPORT    0x0904
#define PCI_CLASS_INPUT_OTHER       0x0980

#define PCI_BASE_CLASS_DOCKING      0x0a
#define PCI_CLASS_DOCKING_GENERIC   0x0a00
/* #define PCI_CLASS_DOCKING_OTHER     0x0a80 */

#define PCI_BASE_CLASS_PROCESSOR    0x0b
#define PCI_CLASS_PROCESSOR_386     0x0b00
#define PCI_CLASS_PROCESSOR_486     0x0b01
#define PCI_CLASS_PROCESSOR_PENTIUM 0x0b02
#define PCI_CLASS_PROCESSOR_ALPHA   0x0b10
#define PCI_CLASS_PROCESSOR_POWERPC 0x0b20
#define PCI_CLASS_PROCESSOR_MIPS    0x0b30
#define PCI_CLASS_PROCESSOR_CO      0x0b40

#define PCI_BASE_CLASS_SERIAL       0x0c
#define PCI_CLASS_SERIAL_FIREWIRE   0x0c00
#define PCI_CLASS_SERIAL_ACCESS     0x0c01
#define PCI_CLASS_SERIAL_SSA        0x0c02
#define PCI_CLASS_SERIAL_USB        0x0c03
#define PCI_CLASS_SERIAL_FIBER      0x0c04
#define PCI_CLASS_SERIAL_SMBUS      0x0c05

#define PCI_BASE_CLASS_INTELLIGENT  0x0e
#define PCI_CLASS_INTELLIGENT_I2O   0x0e00

#define PCI_BASE_CLASS_SATELLITE    0x0f
#define PCI_CLASS_SATELLITE_TV      0x0f00
#define PCI_CLASS_SATELLITE_AUDIO   0x0f01
#define PCI_CLASS_SATELLITE_VOICE   0x0f03
#define PCI_CLASS_SATELLITE_DATA    0x0f04

#define PCI_BASE_CLASS_CRYPT        0x10
#define PCI_CLASS_CRYPT_NETWORK     0x1000
#define PCI_CLASS_CRYPT_ENTERTAINMENT   0x1001
#define PCI_CLASS_CRYPT_OTHER       0x1080

#define PCI_BASE_CLASS_SIGNAL_PROCESSING 0x11
#define PCI_CLASS_SP_DPIO       0x1100
#define PCI_CLASS_SP_OTHER      0x1180

#define PCI_CLASS_OTHERS        0xff

/* Vendors and devices.  Sort key: vendor first, device next. */

#define PCI_VENDOR_ID_COMPAQ        0x0e11
#define PCI_DEVICE_ID_COMPAQ_TOKENRING  0x0508
#define PCI_DEVICE_ID_COMPAQ_1280   0x3033
#define PCI_DEVICE_ID_COMPAQ_TRIFLEX    0x4000
#define PCI_DEVICE_ID_COMPAQ_6010   0x6010
#define PCI_DEVICE_ID_COMPAQ_SMART2P    0xae10
#define PCI_DEVICE_ID_COMPAQ_NETEL100   0xae32
#define PCI_DEVICE_ID_COMPAQ_NETEL10    0xae34
#define PCI_DEVICE_ID_COMPAQ_NETFLEX3I  0xae35
#define PCI_DEVICE_ID_COMPAQ_NETEL100D  0xae40
#define PCI_DEVICE_ID_COMPAQ_NETEL100PI 0xae43
#define PCI_DEVICE_ID_COMPAQ_NETEL100I  0xb011
#define PCI_DEVICE_ID_COMPAQ_THUNDER    0xf130
#define PCI_DEVICE_ID_COMPAQ_NETFLEX3B  0xf150

#define PCI_VENDOR_ID_NCR       0x1000
#define PCI_DEVICE_ID_NCR_53C810    0x0001
#define PCI_DEVICE_ID_NCR_53C820    0x0002
#define PCI_DEVICE_ID_NCR_53C825    0x0003
#define PCI_DEVICE_ID_NCR_53C815    0x0004
#define PCI_DEVICE_ID_NCR_53C860    0x0006
#define PCI_DEVICE_ID_NCR_53C896    0x000b
#define PCI_DEVICE_ID_NCR_53C895    0x000c
#define PCI_DEVICE_ID_NCR_53C885    0x000d
#define PCI_DEVICE_ID_NCR_53C875    0x000f
#define PCI_DEVICE_ID_NCR_53C1510   0x0010
#define PCI_DEVICE_ID_NCR_53C875J   0x008f
#define PCI_DEVICE_ID_NCR_YELLOWFIN 0x0701

#define PCI_VENDOR_ID_ATI       0x1002
#define PCI_DEVICE_ID_ATI_68800     0x4158
#define PCI_DEVICE_ID_ATI_215CT222  0x4354
#define PCI_DEVICE_ID_ATI_210888CX  0x4358
#define PCI_DEVICE_ID_ATI_215GB     0x4742
#define PCI_DEVICE_ID_ATI_215GD     0x4744
#define PCI_DEVICE_ID_ATI_215GI     0x4749
#define PCI_DEVICE_ID_ATI_215GP     0x4750
#define PCI_DEVICE_ID_ATI_215GQ     0x4751
#define PCI_DEVICE_ID_ATI_215GT     0x4754
#define PCI_DEVICE_ID_ATI_215GTB    0x4755
#define PCI_DEVICE_ID_ATI_210888GX  0x4758
#define PCI_DEVICE_ID_ATI_215LG     0x4c47
#define PCI_DEVICE_ID_ATI_264LT     0x4c54
#define PCI_DEVICE_ID_ATI_264VT     0x5654
#define PCI_DEVICE_ID_ATI_RAGE128_RE    0x5245
#define PCI_DEVICE_ID_ATI_RAGE128_RF    0x5246
#define PCI_DEVICE_ID_ATI_RAGE128_RK    0x524b
#define PCI_DEVICE_ID_ATI_RAGE128_RL    0x524c
#define PCI_DEVICE_ID_ATI_RAGE128_PF    0x5046
#define PCI_DEVICE_ID_ATI_RAGE128_PR    0x5052
#define PCI_DEVICE_ID_ATI_RAGE128_LE    0x4c45
#define PCI_DEVICE_ID_ATI_RAGE128_LF    0x4c46

#define PCI_VENDOR_ID_VLSI      0x1004
#define PCI_DEVICE_ID_VLSI_82C592   0x0005
#define PCI_DEVICE_ID_VLSI_82C593   0x0006
#define PCI_DEVICE_ID_VLSI_82C594   0x0007
#define PCI_DEVICE_ID_VLSI_82C597   0x0009
#define PCI_DEVICE_ID_VLSI_82C541   0x000c
#define PCI_DEVICE_ID_VLSI_82C543   0x000d
#define PCI_DEVICE_ID_VLSI_82C532   0x0101
#define PCI_DEVICE_ID_VLSI_82C534   0x0102
#define PCI_DEVICE_ID_VLSI_82C535   0x0104
#define PCI_DEVICE_ID_VLSI_82C147   0x0105
#define PCI_DEVICE_ID_VLSI_VAS96011 0x0702

#define PCI_VENDOR_ID_ADL       0x1005
#define PCI_DEVICE_ID_ADL_2301      0x2301

#define PCI_VENDOR_ID_NS        0x100b
#define PCI_DEVICE_ID_NS_87415      0x0002
#define PCI_DEVICE_ID_NS_87560_LIO      0x000e
#define PCI_DEVICE_ID_NS_87560_USB      0x0012
#define PCI_DEVICE_ID_NS_87410      0xd001

#define PCI_VENDOR_ID_TSENG     0x100c
#define PCI_DEVICE_ID_TSENG_W32P_2  0x3202
#define PCI_DEVICE_ID_TSENG_W32P_b  0x3205
#define PCI_DEVICE_ID_TSENG_W32P_c  0x3206
#define PCI_DEVICE_ID_TSENG_W32P_d  0x3207
#define PCI_DEVICE_ID_TSENG_ET6000  0x3208

#define PCI_VENDOR_ID_WEITEK        0x100e
#define PCI_DEVICE_ID_WEITEK_P9000  0x9001
#define PCI_DEVICE_ID_WEITEK_P9100  0x9100

#define PCI_VENDOR_ID_DEC       0x1011
#define PCI_DEVICE_ID_DEC_BRD       0x0001
#define PCI_DEVICE_ID_DEC_TULIP     0x0002
#define PCI_DEVICE_ID_DEC_TGA       0x0004
#define PCI_DEVICE_ID_DEC_TULIP_FAST    0x0009
#define PCI_DEVICE_ID_DEC_TGA2      0x000D
#define PCI_DEVICE_ID_DEC_FDDI      0x000F
#define PCI_DEVICE_ID_DEC_TULIP_PLUS    0x0014
#define PCI_DEVICE_ID_DEC_21142     0x0019
#define PCI_DEVICE_ID_DEC_21052     0x0021
#define PCI_DEVICE_ID_DEC_21150     0x0022
#define PCI_DEVICE_ID_DEC_21152     0x0024
#define PCI_DEVICE_ID_DEC_21153     0x0025
#define PCI_DEVICE_ID_DEC_21154     0x0026
#define PCI_DEVICE_ID_DEC_21285     0x1065
#define PCI_DEVICE_ID_COMPAQ_42XX   0x0046

#define PCI_VENDOR_ID_CIRRUS        0x1013
#define PCI_DEVICE_ID_CIRRUS_7548   0x0038
#define PCI_DEVICE_ID_CIRRUS_5430   0x00a0
#define PCI_DEVICE_ID_CIRRUS_5434_4 0x00a4
#define PCI_DEVICE_ID_CIRRUS_5434_8 0x00a8
#define PCI_DEVICE_ID_CIRRUS_5436   0x00ac
#define PCI_DEVICE_ID_CIRRUS_5446   0x00b8
#define PCI_DEVICE_ID_CIRRUS_5480   0x00bc
#define PCI_DEVICE_ID_CIRRUS_5462   0x00d0
#define PCI_DEVICE_ID_CIRRUS_5464   0x00d4
#define PCI_DEVICE_ID_CIRRUS_5465   0x00d6
#define PCI_DEVICE_ID_CIRRUS_6729   0x1100
#define PCI_DEVICE_ID_CIRRUS_6832   0x1110
#define PCI_DEVICE_ID_CIRRUS_7542   0x1200
#define PCI_DEVICE_ID_CIRRUS_7543   0x1202
#define PCI_DEVICE_ID_CIRRUS_7541   0x1204

#define PCI_VENDOR_ID_IBM       0x1014
#define PCI_DEVICE_ID_IBM_FIRE_CORAL    0x000a
#define PCI_DEVICE_ID_IBM_TR        0x0018
#define PCI_DEVICE_ID_IBM_82G2675   0x001d
#define PCI_DEVICE_ID_IBM_MCA       0x0020
#define PCI_DEVICE_ID_IBM_82351     0x0022
#define PCI_DEVICE_ID_IBM_PYTHON    0x002d
#define PCI_DEVICE_ID_IBM_SERVERAID 0x002e
#define PCI_DEVICE_ID_IBM_TR_WAKE   0x003e
#define PCI_DEVICE_ID_IBM_MPIC      0x0046
#define PCI_DEVICE_ID_IBM_3780IDSP  0x007d
#define PCI_DEVICE_ID_IBM_405GP     0x0156
#define PCI_DEVICE_ID_IBM_MPIC_2    0xffff

#define PCI_VENDOR_ID_WD        0x101c
#define PCI_DEVICE_ID_WD_7197       0x3296

#define PCI_VENDOR_ID_AMD       0x1022
#define PCI_DEVICE_ID_AMD_LANCE     0x2000
#define PCI_DEVICE_ID_AMD_LANCE_HOME    0x2001
#define PCI_DEVICE_ID_AMD_SCSI      0x2020
#define PCI_DEVICE_ID_AMD_FE_GATE_7006  0x7006
#define PCI_DEVICE_ID_AMD_COBRA_7400    0x7400
#define PCI_DEVICE_ID_AMD_COBRA_7401    0x7401
#define PCI_DEVICE_ID_AMD_COBRA_7403    0x7403
#define PCI_DEVICE_ID_AMD_COBRA_7404    0x7404
#define PCI_DEVICE_ID_AMD_VIPER_7408    0x7408
#define PCI_DEVICE_ID_AMD_VIPER_7409    0x7409
#define PCI_DEVICE_ID_AMD_VIPER_740B    0x740B
#define PCI_DEVICE_ID_AMD_VIPER_740C    0x740C

#define PCI_VENDOR_ID_TRIDENT       0x1023
#define PCI_DEVICE_ID_TRIDENT_9320  0x9320
#define PCI_DEVICE_ID_TRIDENT_9388  0x9388
#define PCI_DEVICE_ID_TRIDENT_9397  0x9397
#define PCI_DEVICE_ID_TRIDENT_939A  0x939A
#define PCI_DEVICE_ID_TRIDENT_9520  0x9520
#define PCI_DEVICE_ID_TRIDENT_9525  0x9525
#define PCI_DEVICE_ID_TRIDENT_9420  0x9420
#define PCI_DEVICE_ID_TRIDENT_9440  0x9440
#define PCI_DEVICE_ID_TRIDENT_9660  0x9660
#define PCI_DEVICE_ID_TRIDENT_9750  0x9750
#define PCI_DEVICE_ID_TRIDENT_9850  0x9850
#define PCI_DEVICE_ID_TRIDENT_9880  0x9880
#define PCI_DEVICE_ID_TRIDENT_8400  0x8400
#define PCI_DEVICE_ID_TRIDENT_8420  0x8420
#define PCI_DEVICE_ID_TRIDENT_8500  0x8500

#define PCI_VENDOR_ID_AI        0x1025
#define PCI_DEVICE_ID_AI_M1435      0x1435

#define PCI_VENDOR_ID_MATROX        0x102B
#define PCI_DEVICE_ID_MATROX_MGA_2  0x0518
#define PCI_DEVICE_ID_MATROX_MIL    0x0519
#define PCI_DEVICE_ID_MATROX_MYS    0x051A
#define PCI_DEVICE_ID_MATROX_MIL_2  0x051b
#define PCI_DEVICE_ID_MATROX_MIL_2_AGP  0x051f
#define PCI_DEVICE_ID_MATROX_MGA_IMP    0x0d10
#define PCI_DEVICE_ID_MATROX_G100_MM    0x1000
#define PCI_DEVICE_ID_MATROX_G100_AGP   0x1001
#define PCI_DEVICE_ID_MATROX_G200_PCI   0x0520
#define PCI_DEVICE_ID_MATROX_G200_AGP   0x0521
#define PCI_DEVICE_ID_MATROX_G400   0x0525
#define PCI_DEVICE_ID_MATROX_VIA    0x4536

#define PCI_VENDOR_ID_CT        0x102c
#define PCI_DEVICE_ID_CT_65545      0x00d8
#define PCI_DEVICE_ID_CT_65548      0x00dc
#define PCI_DEVICE_ID_CT_65550      0x00e0
#define PCI_DEVICE_ID_CT_65554      0x00e4
#define PCI_DEVICE_ID_CT_65555      0x00e5

#define PCI_VENDOR_ID_MIRO      0x1031
#define PCI_DEVICE_ID_MIRO_36050    0x5601

#define PCI_VENDOR_ID_NEC       0x1033
#define PCI_DEVICE_ID_NEC_PCX2      0x0046
#define PCI_DEVICE_ID_NEC_NILE4     0x005a

#define PCI_VENDOR_ID_FD        0x1036
#define PCI_DEVICE_ID_FD_36C70      0x0000

#define PCI_VENDOR_ID_SI        0x1039
#define PCI_DEVICE_ID_SI_5591_AGP   0x0001
#define PCI_DEVICE_ID_SI_6202       0x0002
#define PCI_DEVICE_ID_SI_503        0x0008
#define PCI_DEVICE_ID_SI_ACPI       0x0009
#define PCI_DEVICE_ID_SI_5597_VGA   0x0200
#define PCI_DEVICE_ID_SI_6205       0x0205
#define PCI_DEVICE_ID_SI_501        0x0406
#define PCI_DEVICE_ID_SI_496        0x0496
#define PCI_DEVICE_ID_SI_300        0x0300
#define PCI_DEVICE_ID_SI_530        0x0530
#define PCI_DEVICE_ID_SI_540        0x0540
#define PCI_DEVICE_ID_SI_601        0x0601
#define PCI_DEVICE_ID_SI_620        0x0620
#define PCI_DEVICE_ID_SI_630        0x0630
#define PCI_DEVICE_ID_SI_5107       0x5107
#define PCI_DEVICE_ID_SI_5300       0x5300
#define PCI_DEVICE_ID_SI_5511       0x5511
#define PCI_DEVICE_ID_SI_5513       0x5513
#define PCI_DEVICE_ID_SI_5571       0x5571
#define PCI_DEVICE_ID_SI_5591       0x5591
#define PCI_DEVICE_ID_SI_5597       0x5597
#define PCI_DEVICE_ID_SI_5598       0x5598
#define PCI_DEVICE_ID_SI_5600       0x5600
#define PCI_DEVICE_ID_SI_6300       0x6300
#define PCI_DEVICE_ID_SI_6306       0x6306
#define PCI_DEVICE_ID_SI_6326       0x6326
#define PCI_DEVICE_ID_SI_7001       0x7001

#define PCI_VENDOR_ID_HP        0x103c
#define PCI_DEVICE_ID_HP_J2585A     0x1030
#define PCI_DEVICE_ID_HP_J2585B     0x1031

#define PCI_VENDOR_ID_PCTECH        0x1042
#define PCI_DEVICE_ID_PCTECH_RZ1000 0x1000
#define PCI_DEVICE_ID_PCTECH_RZ1001 0x1001
#define PCI_DEVICE_ID_PCTECH_SAMURAI_0  0x3000
#define PCI_DEVICE_ID_PCTECH_SAMURAI_1  0x3010
#define PCI_DEVICE_ID_PCTECH_SAMURAI_IDE 0x3020

#define PCI_VENDOR_ID_DPT               0x1044
#define PCI_DEVICE_ID_DPT               0xa400

#define PCI_VENDOR_ID_OPTI      0x1045
#define PCI_DEVICE_ID_OPTI_92C178   0xc178
#define PCI_DEVICE_ID_OPTI_82C557   0xc557
#define PCI_DEVICE_ID_OPTI_82C558   0xc558
#define PCI_DEVICE_ID_OPTI_82C621   0xc621
#define PCI_DEVICE_ID_OPTI_82C700   0xc700
#define PCI_DEVICE_ID_OPTI_82C701   0xc701
#define PCI_DEVICE_ID_OPTI_82C814   0xc814
#define PCI_DEVICE_ID_OPTI_82C822   0xc822
#define PCI_DEVICE_ID_OPTI_82C861   0xc861
#define PCI_DEVICE_ID_OPTI_82C825   0xd568

#define PCI_VENDOR_ID_SGS       0x104a
#define PCI_DEVICE_ID_SGS_2000      0x0008
#define PCI_DEVICE_ID_SGS_1764      0x0009

#define PCI_VENDOR_ID_BUSLOGIC            0x104B
#define PCI_DEVICE_ID_BUSLOGIC_MULTIMASTER_NC 0x0140
#define PCI_DEVICE_ID_BUSLOGIC_MULTIMASTER    0x1040
#define PCI_DEVICE_ID_BUSLOGIC_FLASHPOINT     0x8130

#define PCI_VENDOR_ID_TI        0x104c
#define PCI_DEVICE_ID_TI_TVP4010    0x3d04
#define PCI_DEVICE_ID_TI_TVP4020    0x3d07

#define PCI_DEVICE_ID_TI_PCI1130                0xac12
#define PCI_DEVICE_ID_TI_PCI1131                0xac15
#define PCI_DEVICE_ID_TI_PCI1250                0xac16

#define PCI_DEVICE_ID_TI_1130       0xac12
#define PCI_DEVICE_ID_TI_1031       0xac13
#define PCI_DEVICE_ID_TI_1131       0xac15
#define PCI_DEVICE_ID_TI_1250       0xac16
#define PCI_DEVICE_ID_TI_1220       0xac17
#define PCI_DEVICE_ID_TI_1221       0xac19
#define PCI_DEVICE_ID_TI_1210       0xac1a
#define PCI_DEVICE_ID_TI_1450       0xac1b
#define PCI_DEVICE_ID_TI_1225       0xac1c
#define PCI_DEVICE_ID_TI_1251A      0xac1d
#define PCI_DEVICE_ID_TI_1211       0xac1e
#define PCI_DEVICE_ID_TI_1251B      0xac1f
#define PCI_DEVICE_ID_TI_1420       0xac51


#define PCI_VENDOR_ID_OAK       0x104e
#define PCI_DEVICE_ID_OAK_OTI107    0x0107

/* Winbond have two vendor IDs! See 0x10ad as well */
#define PCI_VENDOR_ID_WINBOND2      0x1050
#define PCI_DEVICE_ID_WINBOND2_89C940   0x0940
#define PCI_DEVICE_ID_WINBOND2_89C940F  0x5a5a

#define PCI_VENDOR_ID_MOTOROLA      0x1057
#define PCI_VENDOR_ID_MOTOROLA_OOPS 0x1507
#define PCI_DEVICE_ID_MOTOROLA_MPC105   0x0001
#define PCI_DEVICE_ID_MOTOROLA_MPC106   0x0002
#define PCI_DEVICE_ID_MOTOROLA_RAVEN    0x4801
#define PCI_DEVICE_ID_MOTOROLA_FALCON   0x4802
#define PCI_DEVICE_ID_MOTOROLA_CPX8216  0x4806

#define PCI_VENDOR_ID_PROMISE       0x105a
#define PCI_DEVICE_ID_PROMISE_20267 0x4d30
#define PCI_DEVICE_ID_PROMISE_20246 0x4d33
#define PCI_DEVICE_ID_PROMISE_20262 0x4d38
#define PCI_DEVICE_ID_PROMISE_5300  0x5300

#define PCI_VENDOR_ID_N9        0x105d
#define PCI_DEVICE_ID_N9_I128       0x2309
#define PCI_DEVICE_ID_N9_I128_2     0x2339
#define PCI_DEVICE_ID_N9_I128_T2R   0x493d

#define PCI_VENDOR_ID_UMC       0x1060
#define PCI_DEVICE_ID_UMC_UM8673F   0x0101
#define PCI_DEVICE_ID_UMC_UM8891A   0x0891
#define PCI_DEVICE_ID_UMC_UM8886BF  0x673a
#define PCI_DEVICE_ID_UMC_UM8886A   0x886a
#define PCI_DEVICE_ID_UMC_UM8881F   0x8881
#define PCI_DEVICE_ID_UMC_UM8886F   0x8886
#define PCI_DEVICE_ID_UMC_UM9017F   0x9017
#define PCI_DEVICE_ID_UMC_UM8886N   0xe886
#define PCI_DEVICE_ID_UMC_UM8891N   0xe891

#define PCI_VENDOR_ID_X         0x1061
#define PCI_DEVICE_ID_X_AGX016      0x0001

#define PCI_VENDOR_ID_MYLEX     0x1069
#define PCI_DEVICE_ID_MYLEX_DAC960P_V2  0x0001
#define PCI_DEVICE_ID_MYLEX_DAC960P_V3  0x0002
#define PCI_DEVICE_ID_MYLEX_DAC960P_V4  0x0010
#define PCI_DEVICE_ID_MYLEX_DAC960P_V5  0x0020

#define PCI_VENDOR_ID_MYLEX     0x1069
#define PCI_DEVICE_ID_MYLEX_DAC960P_V2  0x0001
#define PCI_DEVICE_ID_MYLEX_DAC960P_V3  0x0002
#define PCI_DEVICE_ID_MYLEX_DAC960P_V4  0x0010
#define PCI_DEVICE_ID_MYLEX_DAC960P_V5  0x0020

#define PCI_VENDOR_ID_MYLEX     0x1069
#define PCI_DEVICE_ID_MYLEX_DAC960P_V2  0x0001
#define PCI_DEVICE_ID_MYLEX_DAC960P_V3  0x0002
#define PCI_DEVICE_ID_MYLEX_DAC960P_V4  0x0010
#define PCI_DEVICE_ID_MYLEX_DAC960P_V5  0x0020

#define PCI_VENDOR_ID_PICOP     0x1066
#define PCI_DEVICE_ID_PICOP_PT86C52X    0x0001
#define PCI_DEVICE_ID_PICOP_PT80C524    0x8002

#define PCI_VENDOR_ID_APPLE     0x106b
#define PCI_DEVICE_ID_APPLE_BANDIT  0x0001
#define PCI_DEVICE_ID_APPLE_GC      0x0002
#define PCI_DEVICE_ID_APPLE_HYDRA   0x000e

#define PCI_VENDOR_ID_NEXGEN        0x1074
#define PCI_DEVICE_ID_NEXGEN_82C501 0x4e78

#define PCI_VENDOR_ID_QLOGIC        0x1077
#define PCI_DEVICE_ID_QLOGIC_ISP1020    0x1020
#define PCI_DEVICE_ID_QLOGIC_ISP1022    0x1022
#define PCI_DEVICE_ID_QLOGIC_ISP2100    0x2100
#define PCI_DEVICE_ID_QLOGIC_ISP2200    0x2200

#define PCI_VENDOR_ID_CYRIX     0x1078
#define PCI_DEVICE_ID_CYRIX_5510    0x0000
#define PCI_DEVICE_ID_CYRIX_PCI_MASTER  0x0001
#define PCI_DEVICE_ID_CYRIX_5520    0x0002
#define PCI_DEVICE_ID_CYRIX_5530_LEGACY 0x0100
#define PCI_DEVICE_ID_CYRIX_5530_SMI    0x0101
#define PCI_DEVICE_ID_CYRIX_5530_IDE    0x0102
#define PCI_DEVICE_ID_CYRIX_5530_AUDIO  0x0103
#define PCI_DEVICE_ID_CYRIX_5530_VIDEO  0x0104

#define PCI_VENDOR_ID_LEADTEK       0x107d
#define PCI_DEVICE_ID_LEADTEK_805   0x0000

#define PCI_VENDOR_ID_INTERPHASE        0x107e
#define PCI_DEVICE_ID_INTERPHASE_5526   0x0004
#define PCI_DEVICE_ID_INTERPHASE_55x6   0x0005

#define PCI_VENDOR_ID_CONTAQ        0x1080
#define PCI_DEVICE_ID_CONTAQ_82C599 0x0600
#define PCI_DEVICE_ID_CONTAQ_82C693 0xc693

#define PCI_VENDOR_ID_FOREX     0x1083

#define PCI_VENDOR_ID_OLICOM        0x108d
#define PCI_DEVICE_ID_OLICOM_OC3136 0x0001
#define PCI_DEVICE_ID_OLICOM_OC2315 0x0011
#define PCI_DEVICE_ID_OLICOM_OC2325 0x0012
#define PCI_DEVICE_ID_OLICOM_OC2183 0x0013
#define PCI_DEVICE_ID_OLICOM_OC2326 0x0014
#define PCI_DEVICE_ID_OLICOM_OC6151 0x0021

#define PCI_VENDOR_ID_SUN       0x108e
#define PCI_DEVICE_ID_SUN_EBUS      0x1000
#define PCI_DEVICE_ID_SUN_HAPPYMEAL 0x1001
#define PCI_DEVICE_ID_SUN_SIMBA     0x5000
#define PCI_DEVICE_ID_SUN_PBM       0x8000
#define PCI_DEVICE_ID_SUN_SABRE     0xa000

#define PCI_VENDOR_ID_CMD       0x1095
#define PCI_DEVICE_ID_CMD_640       0x0640
#define PCI_DEVICE_ID_CMD_643       0x0643
#define PCI_DEVICE_ID_CMD_646       0x0646
#define PCI_DEVICE_ID_CMD_647       0x0647
#define PCI_DEVICE_ID_CMD_648       0x0648
#define PCI_DEVICE_ID_CMD_649       0x0649
#define PCI_DEVICE_ID_CMD_670       0x0670

#define PCI_VENDOR_ID_VISION        0x1098
#define PCI_DEVICE_ID_VISION_QD8500 0x0001
#define PCI_DEVICE_ID_VISION_QD8580 0x0002

#define PCI_VENDOR_ID_BROOKTREE     0x109e
#define PCI_DEVICE_ID_BROOKTREE_848 0x0350
#define PCI_DEVICE_ID_BROOKTREE_849A    0x0351
#define PCI_DEVICE_ID_BROOKTREE_878_1   0x036e
#define PCI_DEVICE_ID_BROOKTREE_878     0x0878
#define PCI_DEVICE_ID_BROOKTREE_8474    0x8474

#define PCI_VENDOR_ID_SIERRA        0x10a8
#define PCI_DEVICE_ID_SIERRA_STB    0x0000

#define PCI_VENDOR_ID_SGI       0x10a9
#define PCI_DEVICE_ID_SGI_IOC3      0x0003

#define PCI_VENDOR_ID_ACC       0x10aa
#define PCI_DEVICE_ID_ACC_2056      0x0000

#define PCI_VENDOR_ID_WINBOND       0x10ad
#define PCI_DEVICE_ID_WINBOND_83769 0x0001
#define PCI_DEVICE_ID_WINBOND_82C105    0x0105
#define PCI_DEVICE_ID_WINBOND_83C553    0x0565

#define PCI_VENDOR_ID_DATABOOK      0x10b3
#define PCI_DEVICE_ID_DATABOOK_87144    0xb106

#define PCI_VENDOR_ID_PLX       0x10b5
#define PCI_VENDOR_ID_PLX_ROMULUS   0x106a
#define PCI_DEVICE_ID_PLX_SPCOM800  0x1076
#define PCI_DEVICE_ID_PLX_SPCOM200  0x1103
#define PCI_DEVICE_ID_PLX_9050      0x9050
#define PCI_DEVICE_ID_PLX_9060      0x9060
#define PCI_DEVICE_ID_PLX_9060ES    0x906E
#define PCI_DEVICE_ID_PLX_9060SD    0x906D
#define PCI_DEVICE_ID_PLX_9080      0x9080
#define PCI_DEVICE_ID_PLX_GTEK_SERIAL2  0xa001

#define PCI_VENDOR_ID_MADGE     0x10b6
#define PCI_DEVICE_ID_MADGE_MK2     0x0002
#define PCI_DEVICE_ID_MADGE_C155S   0x1001

#define PCI_VENDOR_ID_3COM      0x10b7
#define PCI_DEVICE_ID_3COM_3C985    0x0001
#define PCI_DEVICE_ID_3COM_3C339    0x3390
#define PCI_DEVICE_ID_3COM_3C590    0x5900
#define PCI_DEVICE_ID_3COM_3C595TX  0x5950
#define PCI_DEVICE_ID_3COM_3C595T4  0x5951
#define PCI_DEVICE_ID_3COM_3C595MII 0x5952
#define PCI_DEVICE_ID_3COM_3C900TPO 0x9000
#define PCI_DEVICE_ID_3COM_3C900COMBO   0x9001
#define PCI_DEVICE_ID_3COM_3C905TX  0x9050
#define PCI_DEVICE_ID_3COM_3C905T4  0x9051
#define PCI_DEVICE_ID_3COM_3C905B_TX    0x9055

#define PCI_VENDOR_ID_SMC       0x10b8
#define PCI_DEVICE_ID_SMC_EPIC100   0x0005

#define PCI_VENDOR_ID_AL        0x10b9
#define PCI_DEVICE_ID_AL_M1445      0x1445
#define PCI_DEVICE_ID_AL_M1449      0x1449
#define PCI_DEVICE_ID_AL_M1451      0x1451
#define PCI_DEVICE_ID_AL_M1461      0x1461
#define PCI_DEVICE_ID_AL_M1489      0x1489
#define PCI_DEVICE_ID_AL_M1511      0x1511
#define PCI_DEVICE_ID_AL_M1513      0x1513
#define PCI_DEVICE_ID_AL_M1521      0x1521
#define PCI_DEVICE_ID_AL_M1523      0x1523
#define PCI_DEVICE_ID_AL_M1531      0x1531
#define PCI_DEVICE_ID_AL_M1533      0x1533
#define PCI_DEVICE_ID_AL_M1541      0x1541
#define PCI_DEVICE_ID_AL_M1543      0x1543
#define PCI_DEVICE_ID_AL_M3307      0x3307
#define PCI_DEVICE_ID_AL_M4803      0x5215
#define PCI_DEVICE_ID_AL_M5219      0x5219
#define PCI_DEVICE_ID_AL_M5229      0x5229
#define PCI_DEVICE_ID_AL_M5237      0x5237
#define PCI_DEVICE_ID_AL_M5243      0x5243
#define PCI_DEVICE_ID_AL_M7101      0x7101

#define PCI_VENDOR_ID_MITSUBISHI    0x10ba

#define PCI_VENDOR_ID_SURECOM       0x10bd
#define PCI_DEVICE_ID_SURECOM_NE34  0x0e34

#define PCI_VENDOR_ID_NEOMAGIC          0x10c8
#define PCI_DEVICE_ID_NEOMAGIC_MAGICGRAPH_NM2070 0x0001
#define PCI_DEVICE_ID_NEOMAGIC_MAGICGRAPH_128V 0x0002
#define PCI_DEVICE_ID_NEOMAGIC_MAGICGRAPH_128ZV 0x0003
#define PCI_DEVICE_ID_NEOMAGIC_MAGICGRAPH_NM2160 0x0004
#define PCI_DEVICE_ID_NEOMAGIC_MAGICMEDIA_256AV       0x0005
#define PCI_DEVICE_ID_NEOMAGIC_MAGICGRAPH_128ZVPLUS   0x0083

#define PCI_VENDOR_ID_ASP       0x10cd
#define PCI_DEVICE_ID_ASP_ABP940    0x1200
#define PCI_DEVICE_ID_ASP_ABP940U   0x1300
#define PCI_DEVICE_ID_ASP_ABP940UW  0x2300

#define PCI_VENDOR_ID_MACRONIX      0x10d9
#define PCI_DEVICE_ID_MACRONIX_MX98713  0x0512
#define PCI_DEVICE_ID_MACRONIX_MX987x5  0x0531

#define PCI_VENDOR_ID_TCONRAD       0x10da
#define PCI_DEVICE_ID_TCONRAD_TOKENRING 0x0508

#define PCI_VENDOR_ID_CERN      0x10dc
#define PCI_DEVICE_ID_CERN_SPSB_PMC 0x0001
#define PCI_DEVICE_ID_CERN_SPSB_PCI 0x0002
#define PCI_DEVICE_ID_CERN_HIPPI_DST    0x0021
#define PCI_DEVICE_ID_CERN_HIPPI_SRC    0x0022

#define PCI_VENDOR_ID_NVIDIA        0x10de
#define PCI_DEVICE_ID_NVIDIA_TNT    0x0020
#define PCI_DEVICE_ID_NVIDIA_TNT2   0x0028
#define PCI_DEVICE_ID_NVIDIA_UTNT2  0x0029
#define PCI_DEVICE_ID_NVIDIA_VTNT2  0x002C
#define PCI_DEVICE_ID_NVIDIA_UVTNT2 0x002D
#define PCI_DEVICE_ID_NVIDIA_ITNT2  0x00A0

#define PCI_VENDOR_ID_IMS       0x10e0
#define PCI_DEVICE_ID_IMS_8849      0x8849

#define PCI_VENDOR_ID_TEKRAM2       0x10e1
#define PCI_DEVICE_ID_TEKRAM2_690c  0x690c

#define PCI_VENDOR_ID_TUNDRA        0x10e3
#define PCI_DEVICE_ID_TUNDRA_CA91C042   0x0000

#define PCI_VENDOR_ID_AMCC      0x10e8
#define PCI_DEVICE_ID_AMCC_MYRINET  0x8043
#define PCI_DEVICE_ID_AMCC_PARASTATION  0x8062
#define PCI_DEVICE_ID_AMCC_S5933    0x807d
#define PCI_DEVICE_ID_AMCC_S5933_HEPC3  0x809c

#define PCI_VENDOR_ID_INTERG        0x10ea
#define PCI_DEVICE_ID_INTERG_1680   0x1680
#define PCI_DEVICE_ID_INTERG_1682   0x1682
#define PCI_DEVICE_ID_INTERG_2000   0x2000
#define PCI_DEVICE_ID_INTERG_2010   0x2010
#define PCI_DEVICE_ID_INTERG_5000   0x5000

#define PCI_VENDOR_ID_REALTEK       0x10ec
#define PCI_DEVICE_ID_REALTEK_8029  0x8029
#define PCI_DEVICE_ID_REALTEK_8129  0x8129
#define PCI_DEVICE_ID_REALTEK_8139  0x8139

#define PCI_VENDOR_ID_TRUEVISION    0x10fa
#define PCI_DEVICE_ID_TRUEVISION_T1000  0x000c

#define PCI_VENDOR_ID_INIT      0x1101
#define PCI_DEVICE_ID_INIT_320P     0x9100
#define PCI_DEVICE_ID_INIT_360P     0x9500

#define PCI_VENDOR_ID_TTI       0x1103
#define PCI_DEVICE_ID_TTI_HPT343    0x0003
#define PCI_DEVICE_ID_TTI_HPT366    0x0004

#define PCI_VENDOR_ID_VIA       0x1106
#define PCI_DEVICE_ID_VIA_8371_0    0x0391
#define PCI_DEVICE_ID_VIA_8501_0    0x0501
#define PCI_DEVICE_ID_VIA_8601_0    0x0601
#define PCI_DEVICE_ID_VIA_82C505    0x0505
#define PCI_DEVICE_ID_VIA_82C561    0x0561
#define PCI_DEVICE_ID_VIA_82C586_1  0x0571
#define PCI_DEVICE_ID_VIA_82C576    0x0576
#define PCI_DEVICE_ID_VIA_82C585    0x0585
#define PCI_DEVICE_ID_VIA_82C586_0  0x0586
#define PCI_DEVICE_ID_VIA_82C595    0x0595
#define PCI_DEVICE_ID_VIA_82C596    0x0596
#define PCI_DEVICE_ID_VIA_82C597_0  0x0597
#define PCI_DEVICE_ID_VIA_82C598_0  0x0598
#define PCI_DEVICE_ID_VIA_82C680    0x0680
#define PCI_DEVICE_ID_VIA_82C686    0x0686
#define PCI_DEVICE_ID_VIA_82C691    0x0691
#define PCI_DEVICE_ID_VIA_82C693    0x0693
#define PCI_DEVICE_ID_VIA_82C926    0x0926
#define PCI_DEVICE_ID_VIA_82C416    0x1571
#define PCI_DEVICE_ID_VIA_82C595_97 0x1595
#define PCI_DEVICE_ID_VIA_82C586_2  0x3038
#define PCI_DEVICE_ID_VIA_82C586_3  0x3040
#define PCI_DEVICE_ID_VIA_82C686_4  0x3057
#define PCI_DEVICE_ID_VIA_82C686_5  0x3058
#define PCI_DEVICE_ID_VIA_82C686_6  0x3068
#define PCI_DEVICE_ID_VIA_86C100A   0x6100
#define PCI_DEVICE_ID_VIA_8231      0x8231
#define PCI_DEVICE_ID_VIA_8371_1    0x8391
#define PCI_DEVICE_ID_VIA_8501_1    0x8501
#define PCI_DEVICE_ID_VIA_82C597_1  0x8597
#define PCI_DEVICE_ID_VIA_82C598_1  0x8598
#define PCI_DEVICE_ID_VIA_8601_1    0x8601

#define PCI_VENDOR_ID_SMC2             0x1113
#define PCI_DEVICE_ID_SMC2_1211TX      0x1211

#define PCI_VENDOR_ID_VORTEX        0x1119
#define PCI_DEVICE_ID_VORTEX_GDT60x0    0x0000
#define PCI_DEVICE_ID_VORTEX_GDT6000B   0x0001
#define PCI_DEVICE_ID_VORTEX_GDT6x10    0x0002
#define PCI_DEVICE_ID_VORTEX_GDT6x20    0x0003
#define PCI_DEVICE_ID_VORTEX_GDT6530    0x0004
#define PCI_DEVICE_ID_VORTEX_GDT6550    0x0005
#define PCI_DEVICE_ID_VORTEX_GDT6x17    0x0006
#define PCI_DEVICE_ID_VORTEX_GDT6x27    0x0007
#define PCI_DEVICE_ID_VORTEX_GDT6537    0x0008
#define PCI_DEVICE_ID_VORTEX_GDT6557    0x0009
#define PCI_DEVICE_ID_VORTEX_GDT6x15    0x000a
#define PCI_DEVICE_ID_VORTEX_GDT6x25    0x000b
#define PCI_DEVICE_ID_VORTEX_GDT6535    0x000c
#define PCI_DEVICE_ID_VORTEX_GDT6555    0x000d
#define PCI_DEVICE_ID_VORTEX_GDT6x17RP  0x0100
#define PCI_DEVICE_ID_VORTEX_GDT6x27RP  0x0101
#define PCI_DEVICE_ID_VORTEX_GDT6537RP  0x0102
#define PCI_DEVICE_ID_VORTEX_GDT6557RP  0x0103
#define PCI_DEVICE_ID_VORTEX_GDT6x11RP  0x0104
#define PCI_DEVICE_ID_VORTEX_GDT6x21RP  0x0105
#define PCI_DEVICE_ID_VORTEX_GDT6x17RP1 0x0110
#define PCI_DEVICE_ID_VORTEX_GDT6x27RP1 0x0111
#define PCI_DEVICE_ID_VORTEX_GDT6537RP1 0x0112
#define PCI_DEVICE_ID_VORTEX_GDT6557RP1 0x0113
#define PCI_DEVICE_ID_VORTEX_GDT6x11RP1 0x0114
#define PCI_DEVICE_ID_VORTEX_GDT6x21RP1 0x0115
#define PCI_DEVICE_ID_VORTEX_GDT6x17RP2 0x0120
#define PCI_DEVICE_ID_VORTEX_GDT6x27RP2 0x0121
#define PCI_DEVICE_ID_VORTEX_GDT6537RP2 0x0122
#define PCI_DEVICE_ID_VORTEX_GDT6557RP2 0x0123
#define PCI_DEVICE_ID_VORTEX_GDT6x11RP2 0x0124
#define PCI_DEVICE_ID_VORTEX_GDT6x21RP2 0x0125

#define PCI_VENDOR_ID_EF        0x111a
#define PCI_DEVICE_ID_EF_ATM_FPGA   0x0000
#define PCI_DEVICE_ID_EF_ATM_ASIC   0x0002

#define PCI_VENDOR_ID_FORE      0x1127
#define PCI_DEVICE_ID_FORE_PCA200PC 0x0210
#define PCI_DEVICE_ID_FORE_PCA200E  0x0300

#define PCI_VENDOR_ID_IMAGINGTECH   0x112f
#define PCI_DEVICE_ID_IMAGINGTECH_ICPCI 0x0000

#define PCI_VENDOR_ID_PHILIPS       0x1131
#define PCI_DEVICE_ID_PHILIPS_SAA7145   0x7145
#define PCI_DEVICE_ID_PHILIPS_SAA7146   0x7146

#define PCI_VENDOR_ID_CYCLONE       0x113c
#define PCI_DEVICE_ID_CYCLONE_SDK   0x0001

#define PCI_VENDOR_ID_ALLIANCE      0x1142
#define PCI_DEVICE_ID_ALLIANCE_PROMOTIO 0x3210
#define PCI_DEVICE_ID_ALLIANCE_PROVIDEO 0x6422
#define PCI_DEVICE_ID_ALLIANCE_AT24 0x6424
#define PCI_DEVICE_ID_ALLIANCE_AT3D 0x643d

#define PCI_VENDOR_ID_SYSKONNECT    0x1148
#define PCI_DEVICE_ID_SYSKONNECT_FP 0x4000
#define PCI_DEVICE_ID_SYSKONNECT_TR 0x4200
#define PCI_DEVICE_ID_SYSKONNECT_GE 0x4300

#define PCI_VENDOR_ID_VMIC      0x114a
#define PCI_DEVICE_ID_VMIC_VME      0x7587

#define PCI_VENDOR_ID_DIGI      0x114f
#define PCI_DEVICE_ID_DIGI_EPC      0x0002
#define PCI_DEVICE_ID_DIGI_RIGHTSWITCH  0x0003
#define PCI_DEVICE_ID_DIGI_XEM      0x0004
#define PCI_DEVICE_ID_DIGI_XR       0x0005
#define PCI_DEVICE_ID_DIGI_CX       0x0006
#define PCI_DEVICE_ID_DIGI_XRJ      0x0009
#define PCI_DEVICE_ID_DIGI_EPCJ     0x000a
#define PCI_DEVICE_ID_DIGI_XR_920   0x0027

#define PCI_VENDOR_ID_MUTECH        0x1159
#define PCI_DEVICE_ID_MUTECH_MV1000 0x0001

#define PCI_VENDOR_ID_RENDITION     0x1163
#define PCI_DEVICE_ID_RENDITION_VERITE  0x0001
#define PCI_DEVICE_ID_RENDITION_VERITE2100 0x2000

#define PCI_VENDOR_ID_SERVERWORKS   0x1166
#define PCI_DEVICE_ID_SERVERWORKS_HE    0x0008
#define PCI_DEVICE_ID_SERVERWORKS_LE    0x0009
#define PCI_DEVICE_ID_SERVERWORKS_CIOB30   0x0010
#define PCI_DEVICE_ID_SERVERWORKS_CMIC_HE  0x0011
#define PCI_DEVICE_ID_SERVERWORKS_CSB5      0x0201

#define PCI_VENDOR_ID_TOSHIBA       0x1179
#define PCI_DEVICE_ID_TOSHIBA_601   0x0601
#define PCI_DEVICE_ID_TOSHIBA_TOPIC95   0x060a
#define PCI_DEVICE_ID_TOSHIBA_TOPIC97   0x060f

#define PCI_VENDOR_ID_RICOH     0x1180
#define PCI_DEVICE_ID_RICOH_RL5C465 0x0465
#define PCI_DEVICE_ID_RICOH_RL5C466 0x0466
#define PCI_DEVICE_ID_RICOH_RL5C475 0x0475
#define PCI_DEVICE_ID_RICOH_RL5C476 0x0476
#define PCI_DEVICE_ID_RICOH_RL5C478 0x0478

#define PCI_VENDOR_ID_ARTOP     0x1191
#define PCI_DEVICE_ID_ARTOP_ATP8400 0x0004
#define PCI_DEVICE_ID_ARTOP_ATP850UF    0x0005
#define PCI_DEVICE_ID_ARTOP_ATP860  0x0006
#define PCI_DEVICE_ID_ARTOP_ATP860R 0x0007

#define PCI_VENDOR_ID_ZEITNET       0x1193
#define PCI_DEVICE_ID_ZEITNET_1221  0x0001
#define PCI_DEVICE_ID_ZEITNET_1225  0x0002

#define PCI_VENDOR_ID_OMEGA     0x119b
#define PCI_DEVICE_ID_OMEGA_82C092G 0x1221

#define PCI_SUBVENDOR_ID_KEYSPAN    0x11a9
#define PCI_SUBDEVICE_ID_KEYSPAN_SX2    0x5334

#define PCI_VENDOR_ID_GALILEO       0x11ab
#define PCI_DEVICE_ID_GALILEO_GT64011   0x4146

#define PCI_VENDOR_ID_LITEON        0x11ad
#define PCI_DEVICE_ID_LITEON_LNE100TX   0x0002

#define PCI_VENDOR_ID_V3        0x11b0
#define PCI_DEVICE_ID_V3_V960       0x0001
#define PCI_DEVICE_ID_V3_V350       0x0001
#define PCI_DEVICE_ID_V3_V960V2     0x0002
#define PCI_DEVICE_ID_V3_V350V2     0x0002

#define PCI_VENDOR_ID_NP        0x11bc
#define PCI_DEVICE_ID_NP_PCI_FDDI   0x0001

#define PCI_VENDOR_ID_ATT       0x11c1
#define PCI_DEVICE_ID_ATT_L56XMF    0x0440
#define PCI_DEVICE_ID_ATT_VENUS_MODEM   0x480

#define PCI_VENDOR_ID_SPECIALIX     0x11cb
#define PCI_DEVICE_ID_SPECIALIX_IO8 0x2000
#define PCI_DEVICE_ID_SPECIALIX_XIO 0x4000
#define PCI_DEVICE_ID_SPECIALIX_RIO 0x8000
#define PCI_SUBDEVICE_ID_SPECIALIX_SPEED4 0xa004

#define PCI_VENDOR_ID_AURAVISION    0x11d1
#define PCI_DEVICE_ID_AURAVISION_VXP524 0x01f7

#define PCI_VENDOR_ID_IKON      0x11d5
#define PCI_DEVICE_ID_IKON_10115    0x0115
#define PCI_DEVICE_ID_IKON_10117    0x0117

#define PCI_VENDOR_ID_ZORAN     0x11de
#define PCI_DEVICE_ID_ZORAN_36057   0x6057
#define PCI_DEVICE_ID_ZORAN_36120   0x6120

#define PCI_VENDOR_ID_KINETIC       0x11f4
#define PCI_DEVICE_ID_KINETIC_2915  0x2915

#define PCI_VENDOR_ID_COMPEX        0x11f6
#define PCI_DEVICE_ID_COMPEX_ENET100VG4 0x0112
#define PCI_DEVICE_ID_COMPEX_RL2000 0x1401

#define PCI_VENDOR_ID_RP               0x11fe
#define PCI_DEVICE_ID_RP8OCTA          0x0001
/*#define PCI_DEVICE_ID_RP32INTF         0x0001*/
#define PCI_DEVICE_ID_RP8INTF          0x0002
#define PCI_DEVICE_ID_RP16INTF         0x0003
#define PCI_DEVICE_ID_RP32INTF         0x0004
#define PCI_DEVICE_ID_RP4QUAD          0x0004
/*#define PCI_DEVICE_ID_RP8OCTA          0x0005*/
#define PCI_DEVICE_ID_RP8J         0x0006
#define PCI_DEVICE_ID_RPP4         0x000A
#define PCI_DEVICE_ID_RPP8         0x000B
#define PCI_DEVICE_ID_RP8M         0x000C

#define PCI_VENDOR_ID_CYCLADES      0x120e
#define PCI_DEVICE_ID_CYCLOM_Y_Lo   0x0100
#define PCI_DEVICE_ID_CYCLOM_Y_Hi   0x0101
#define PCI_DEVICE_ID_CYCLOM_4Y_Lo  0x0102
#define PCI_DEVICE_ID_CYCLOM_4Y_Hi  0x0103
#define PCI_DEVICE_ID_CYCLOM_8Y_Lo  0x0104
#define PCI_DEVICE_ID_CYCLOM_8Y_Hi  0x0105
#define PCI_DEVICE_ID_CYCLOM_Z_Lo   0x0200
#define PCI_DEVICE_ID_CYCLOM_Z_Hi   0x0201

#define PCI_VENDOR_ID_ESSENTIAL     0x120f
#define PCI_DEVICE_ID_ESSENTIAL_ROADRUNNER  0x0001

#define PCI_VENDOR_ID_O2        0x1217
#define PCI_DEVICE_ID_O2_6729       0x6729
#define PCI_DEVICE_ID_O2_6730       0x673a
#define PCI_DEVICE_ID_O2_6832       0x6832
#define PCI_DEVICE_ID_O2_6836       0x6836

#define PCI_VENDOR_ID_3DFX      0x121a
#define PCI_DEVICE_ID_3DFX_VOODOO   0x0001
#define PCI_DEVICE_ID_3DFX_VOODOO2  0x0002
#define PCI_DEVICE_ID_3DFX_BANSHEE      0x0003
#define PCI_DEVICE_ID_3DFX_VOODOO3  0x0005

#define PCI_VENDOR_ID_SIGMADES      0x1236
#define PCI_DEVICE_ID_SIGMADES_6425 0x6401

#define PCI_VENDOR_ID_CCUBE     0x123f

#define PCI_VENDOR_ID_AVM       0x1244
#define PCI_DEVICE_ID_AVM_A1        0x0a00

#define PCI_VENDOR_ID_DIPIX     0x1246

#define PCI_VENDOR_ID_STALLION      0x124d
#define PCI_DEVICE_ID_STALLION_ECHPCI832 0x0000
#define PCI_DEVICE_ID_STALLION_ECHPCI864 0x0002
#define PCI_DEVICE_ID_STALLION_EIOPCI   0x0003

#define PCI_VENDOR_ID_OPTIBASE      0x1255
#define PCI_DEVICE_ID_OPTIBASE_FORGE    0x1110
#define PCI_DEVICE_ID_OPTIBASE_FUSION   0x1210
#define PCI_DEVICE_ID_OPTIBASE_VPLEX    0x2110
#define PCI_DEVICE_ID_OPTIBASE_VPLEXCC  0x2120
#define PCI_DEVICE_ID_OPTIBASE_VQUEST   0x2130

#define PCI_VENDOR_ID_ASIX                      0x125b
#define PCI_DEVICE_ID_ASIX_88140                0x1400

#define PCI_VENDOR_ID_ESS       0x125d
#define PCI_DEVICE_ID_ESS_AUDIOPCI  0x1969

#define PCI_VENDOR_ID_SATSAGEM      0x1267
#define PCI_DEVICE_ID_SATSAGEM_PCR2101  0x5352
#define PCI_DEVICE_ID_SATSAGEM_TELSATTURBO 0x5a4b

#define PCI_VENDOR_ID_HUGHES        0x1273
#define PCI_DEVICE_ID_HUGHES_DIRECPC    0x0002

#define PCI_VENDOR_ID_ENSONIQ       0x1274
#define PCI_DEVICE_ID_ENSONIQ_AUDIOPCI  0x5000
#define PCI_DEVICE_ID_ENSONIQ_ES1371    0x1371

#define PCI_VENDOR_ID_ROCKWELL      0x127A

#define PCI_VENDOR_ID_ALTEON        0x12ae
#define PCI_DEVICE_ID_ALTEON_ACENIC 0x0001

#define PCI_VENDOR_ID_USR       0x12B9

#define PCI_SUBVENDOR_ID_CONNECT_TECH           0x12c4
#define PCI_SUBDEVICE_ID_CONNECT_TECH_BH8_232       0x0001
#define PCI_SUBDEVICE_ID_CONNECT_TECH_BH4_232       0x0002
#define PCI_SUBDEVICE_ID_CONNECT_TECH_BH2_232       0x0003
#define PCI_SUBDEVICE_ID_CONNECT_TECH_BH8_485       0x0004
#define PCI_SUBDEVICE_ID_CONNECT_TECH_BH8_485_4_4   0x0005
#define PCI_SUBDEVICE_ID_CONNECT_TECH_BH4_485       0x0006
#define PCI_SUBDEVICE_ID_CONNECT_TECH_BH4_485_2_2   0x0007
#define PCI_SUBDEVICE_ID_CONNECT_TECH_BH2_485       0x0008

#define PCI_VENDOR_ID_PICTUREL      0x12c5
#define PCI_DEVICE_ID_PICTUREL_PCIVST   0x0081

#define PCI_VENDOR_ID_NVIDIA_SGS    0x12d2
#define PCI_DEVICE_ID_NVIDIA_SGS_RIVA128 0x0018

#define PCI_SUBVENDOR_ID_CHASE_PCIFAST      0x12E0
#define PCI_SUBDEVICE_ID_CHASE_PCIFAST4     0x0031
#define PCI_SUBDEVICE_ID_CHASE_PCIFAST8     0x0021
#define PCI_SUBDEVICE_ID_CHASE_PCIFAST16    0x0011
#define PCI_SUBDEVICE_ID_CHASE_PCIFAST16FMC 0x0041
#define PCI_SUBVENDOR_ID_CHASE_PCIRAS       0x124D
#define PCI_SUBDEVICE_ID_CHASE_PCIRAS4      0xF001
#define PCI_SUBDEVICE_ID_CHASE_PCIRAS8      0xF010

#define PCI_VENDOR_ID_CBOARDS       0x1307
#define PCI_DEVICE_ID_CBOARDS_DAS1602_16 0x0001

#define PCI_VENDOR_ID_SIIG      0x131f
#define PCI_DEVICE_ID_SIIG_1S_10x_550   0x1000
#define PCI_DEVICE_ID_SIIG_1S_10x_650   0x1001
#define PCI_DEVICE_ID_SIIG_1S_10x_850   0x1002
#define PCI_DEVICE_ID_SIIG_1S1P_10x_550 0x1010
#define PCI_DEVICE_ID_SIIG_1S1P_10x_650 0x1011
#define PCI_DEVICE_ID_SIIG_1S1P_10x_850 0x1012
#define PCI_DEVICE_ID_SIIG_1P_10x   0x1020
#define PCI_DEVICE_ID_SIIG_2P_10x   0x1021
#define PCI_DEVICE_ID_SIIG_2S_10x_550   0x1030
#define PCI_DEVICE_ID_SIIG_2S_10x_650   0x1031
#define PCI_DEVICE_ID_SIIG_2S_10x_850   0x1032
#define PCI_DEVICE_ID_SIIG_2S1P_10x_550 0x1034
#define PCI_DEVICE_ID_SIIG_2S1P_10x_650 0x1035
#define PCI_DEVICE_ID_SIIG_2S1P_10x_850 0x1036
#define PCI_DEVICE_ID_SIIG_4S_10x_550   0x1050
#define PCI_DEVICE_ID_SIIG_4S_10x_650   0x1051
#define PCI_DEVICE_ID_SIIG_4S_10x_850   0x1052
#define PCI_DEVICE_ID_SIIG_1S_20x_550   0x2000
#define PCI_DEVICE_ID_SIIG_1S_20x_650   0x2001
#define PCI_DEVICE_ID_SIIG_1S_20x_850   0x2002
#define PCI_DEVICE_ID_SIIG_1P_20x   0x2020
#define PCI_DEVICE_ID_SIIG_2P_20x   0x2021
#define PCI_DEVICE_ID_SIIG_2S_20x_550   0x2030
#define PCI_DEVICE_ID_SIIG_2S_20x_650   0x2031
#define PCI_DEVICE_ID_SIIG_2S_20x_850   0x2032
#define PCI_DEVICE_ID_SIIG_2P1S_20x_550 0x2040
#define PCI_DEVICE_ID_SIIG_2P1S_20x_650 0x2041
#define PCI_DEVICE_ID_SIIG_2P1S_20x_850 0x2042
#define PCI_DEVICE_ID_SIIG_1S1P_20x_550 0x2010
#define PCI_DEVICE_ID_SIIG_1S1P_20x_650 0x2011
#define PCI_DEVICE_ID_SIIG_1S1P_20x_850 0x2012
#define PCI_DEVICE_ID_SIIG_4S_20x_550   0x2050
#define PCI_DEVICE_ID_SIIG_4S_20x_650   0x2051
#define PCI_DEVICE_ID_SIIG_4S_20x_850   0x2052
#define PCI_DEVICE_ID_SIIG_2S1P_20x_550 0x2060
#define PCI_DEVICE_ID_SIIG_2S1P_20x_650 0x2061
#define PCI_DEVICE_ID_SIIG_2S1P_20x_850 0x2062

#define PCI_VENDOR_ID_QUATECH       0x135C
#define PCI_DEVICE_ID_QUATECH_QSC100    0x0010
#define PCI_DEVICE_ID_QUATECH_DSC100    0x0020
#define PCI_DEVICE_ID_QUATECH_DSC200    0x0030
#define PCI_DEVICE_ID_QUATECH_QSC200    0x0040
#define PCI_DEVICE_ID_QUATECH_ESC100D   0x0050
#define PCI_DEVICE_ID_QUATECH_ESC100M   0x0060

#define PCI_VENDOR_ID_SEALEVEL      0x135e
#define PCI_DEVICE_ID_SEALEVEL_U530 0x7101
#define PCI_DEVICE_ID_SEALEVEL_UCOMM2   0x7201
#define PCI_DEVICE_ID_SEALEVEL_UCOMM422 0x7402
#define PCI_DEVICE_ID_SEALEVEL_UCOMM232 0x7202
#define PCI_DEVICE_ID_SEALEVEL_COMM4    0x7401
#define PCI_DEVICE_ID_SEALEVEL_COMM8    0x7801

#define PCI_VENDOR_ID_NETGEAR       0x1385
#define PCI_DEVICE_ID_NETGEAR_GA620 0x620a

#define PCI_VENDOR_ID_3WARE     0x13C1
#define PCI_DEVICE_ID_3WARE_1000    0x1000

#define PCI_VENDOR_ID_LAVA      0x1407
#define PCI_DEVICE_ID_LAVA_DSERIAL  0x0100 /* 2x 16550 */
#define PCI_DEVICE_ID_LAVA_QUATRO_A 0x0101 /* 2x 16550, half of 4 port */
#define PCI_DEVICE_ID_LAVA_QUATRO_B 0x0102 /* 2x 16550, half of 4 port */
#define PCI_DEVICE_ID_LAVA_PORT_PLUS    0x0200 /* 2x 16650 */
#define PCI_DEVICE_ID_LAVA_QUAD_A   0x0201 /* 2x 16650, half of 4 port */
#define PCI_DEVICE_ID_LAVA_QUAD_B   0x0202 /* 2x 16650, half of 4 port */
#define PCI_DEVICE_ID_LAVA_SSERIAL  0x0500 /* 1x 16550 */
#define PCI_DEVICE_ID_LAVA_PORT_650 0x0600 /* 1x 16650 */

#define PCI_VENDOR_ID_SYBA      0x1592
#define PCI_DEVICE_ID_SYBA_2P_EPP   0x0782
#define PCI_DEVICE_ID_SYBA_1P_ECP   0x0783

#define PCI_DEVICE_ID_LAVA_PARALLEL 0x8000
#define PCI_DEVICE_ID_LAVA_DUAL_PAR_A   0x8002 /* The Lava Dual Parallel is */
#define PCI_DEVICE_ID_LAVA_DUAL_PAR_B   0x8003 /* two PCI devices on a card */
#define PCI_DEVICE_ID_LAVA_BOCA_IOPPAR  0x8800

#define PCI_VENDOR_ID_TIMEDIA       0x1409
#define PCI_DEVICE_ID_TIMEDIA_1889  0x7168

#define PCI_VENDOR_ID_OXSEMI            0x1415
#define PCI_DEVICE_ID_OXSEMI_16PCI954   0x9501
#define PCI_DEVICE_ID_OXSEMI_16PCI952   0x950A
#define PCI_DEVICE_ID_OXSEMI_16PCI95N   0x9511

#define PCI_VENDOR_ID_TITAN     0x14D2
#define PCI_DEVICE_ID_TITAN_100     0xA001
#define PCI_DEVICE_ID_TITAN_200     0xA005
#define PCI_DEVICE_ID_TITAN_400     0xA003
#define PCI_DEVICE_ID_TITAN_800B    0xA004

#define PCI_VENDOR_ID_PANACOM       0x14d4
#define PCI_DEVICE_ID_PANACOM_QUADMODEM 0x0400
#define PCI_DEVICE_ID_PANACOM_DUALMODEM 0x0402

#define PCI_VENDOR_ID_AFAVLAB       0x14db
#define PCI_DEVICE_ID_AFAVLAB_TK9902    0x2120

#define PCI_VENDOR_ID_MORETON       0x15aa
#define PCI_DEVICE_ID_RASTEL_2PORT  0x2000

#define PCI_VENDOR_ID_SYMPHONY      0x1c1c
#define PCI_DEVICE_ID_SYMPHONY_101  0x0001

#define PCI_VENDOR_ID_TEKRAM        0x1de1
#define PCI_DEVICE_ID_TEKRAM_DC290  0xdc29

#define PCI_VENDOR_ID_3DLABS        0x3d3d
#define PCI_DEVICE_ID_3DLABS_300SX  0x0001
#define PCI_DEVICE_ID_3DLABS_500TX  0x0002
#define PCI_DEVICE_ID_3DLABS_DELTA  0x0003
#define PCI_DEVICE_ID_3DLABS_PERMEDIA   0x0004
#define PCI_DEVICE_ID_3DLABS_MX     0x0006
#define PCI_DEVICE_ID_3DLABS_PERMEDIA2  0x0007
#define PCI_DEVICE_ID_3DLABS_GAMMA  0x0008
#define PCI_DEVICE_ID_3DLABS_PERMEDIA2V 0x0009

#define PCI_VENDOR_ID_AVANCE        0x4005
#define PCI_DEVICE_ID_AVANCE_ALG2064    0x2064
#define PCI_DEVICE_ID_AVANCE_2302   0x2302

#define PCI_VENDOR_ID_NETVIN        0x4a14
#define PCI_DEVICE_ID_NETVIN_NV5000SC   0x5000

#define PCI_VENDOR_ID_S3        0x5333
#define PCI_DEVICE_ID_S3_PLATO_PXS  0x0551
#define PCI_DEVICE_ID_S3_ViRGE      0x5631
#define PCI_DEVICE_ID_S3_TRIO       0x8811
#define PCI_DEVICE_ID_S3_AURORA64VP 0x8812
#define PCI_DEVICE_ID_S3_TRIO64UVP  0x8814
#define PCI_DEVICE_ID_S3_ViRGE_VX   0x883d
#define PCI_DEVICE_ID_S3_868        0x8880
#define PCI_DEVICE_ID_S3_928        0x88b0
#define PCI_DEVICE_ID_S3_864_1      0x88c0
#define PCI_DEVICE_ID_S3_864_2      0x88c1
#define PCI_DEVICE_ID_S3_964_1      0x88d0
#define PCI_DEVICE_ID_S3_964_2      0x88d1
#define PCI_DEVICE_ID_S3_968        0x88f0
#define PCI_DEVICE_ID_S3_TRIO64V2   0x8901
#define PCI_DEVICE_ID_S3_PLATO_PXG  0x8902
#define PCI_DEVICE_ID_S3_ViRGE_DXGX 0x8a01
#define PCI_DEVICE_ID_S3_ViRGE_GX2  0x8a10
#define PCI_DEVICE_ID_S3_ViRGE_MX   0x8c01
#define PCI_DEVICE_ID_S3_ViRGE_MXP  0x8c02
#define PCI_DEVICE_ID_S3_ViRGE_MXPMV    0x8c03
#define PCI_DEVICE_ID_S3_SONICVIBES 0xca00

#define PCI_VENDOR_ID_DCI       0x6666
#define PCI_DEVICE_ID_DCI_PCCOM4    0x0001

#define PCI_VENDOR_ID_GENROCO       0x5555
#define PCI_DEVICE_ID_GENROCO_HFP832    0x0003

#define PCI_VENDOR_ID_INTEL     0x8086
#define PCI_DEVICE_ID_INTEL_21145   0x0039
#define PCI_DEVICE_ID_INTEL_82375   0x0482
#define PCI_DEVICE_ID_INTEL_82424   0x0483
#define PCI_DEVICE_ID_INTEL_82378   0x0484
#define PCI_DEVICE_ID_INTEL_82430   0x0486
#define PCI_DEVICE_ID_INTEL_82434   0x04a3
#define PCI_DEVICE_ID_INTEL_I960    0x0960
#define PCI_DEVICE_ID_INTEL_82559ER 0x1209
#define PCI_DEVICE_ID_INTEL_82092AA_0   0x1221
#define PCI_DEVICE_ID_INTEL_82092AA_1   0x1222
#define PCI_DEVICE_ID_INTEL_7116    0x1223
#define PCI_DEVICE_ID_INTEL_82596   0x1226
#define PCI_DEVICE_ID_INTEL_82865   0x1227
#define PCI_DEVICE_ID_INTEL_82557   0x1229
#define PCI_DEVICE_ID_INTEL_82437   0x122d
#define PCI_DEVICE_ID_INTEL_82371_0 0x122e
#define PCI_DEVICE_ID_INTEL_82371_1 0x1230
#define PCI_DEVICE_ID_INTEL_82371FB_0   0x122e
#define PCI_DEVICE_ID_INTEL_82371FB_1   0x1230
#define PCI_DEVICE_ID_INTEL_82371MX 0x1234
#define PCI_DEVICE_ID_INTEL_82437MX 0x1235
#define PCI_DEVICE_ID_INTEL_82441   0x1237
#define PCI_DEVICE_ID_INTEL_82380FB 0x124b
#define PCI_DEVICE_ID_INTEL_82439   0x1250
#define PCI_DEVICE_ID_INTEL_82371SB_0   0x7000
#define PCI_DEVICE_ID_INTEL_82371SB_1   0x7010
#define PCI_DEVICE_ID_INTEL_82371SB_2   0x7020
#define PCI_DEVICE_ID_INTEL_82437VX 0x7030
#define PCI_DEVICE_ID_INTEL_82439TX 0x7100
#define PCI_DEVICE_ID_INTEL_82371AB_0   0x7110
#define PCI_DEVICE_ID_INTEL_82371AB 0x7111
#define PCI_DEVICE_ID_INTEL_82371AB_2   0x7112
#define PCI_DEVICE_ID_INTEL_82371AB_3   0x7113
#define PCI_DEVICE_ID_INTEL_82801AA_0   0x2410
#define PCI_DEVICE_ID_INTEL_82801AA_1   0x2411
#define PCI_DEVICE_ID_INTEL_82801AA_2   0x2412
#define PCI_DEVICE_ID_INTEL_82801AA_3   0x2413
#define PCI_DEVICE_ID_INTEL_82801AA_5   0x2415
#define PCI_DEVICE_ID_INTEL_82801AA_6   0x2416
#define PCI_DEVICE_ID_INTEL_82801AA_8   0x2418
#define PCI_DEVICE_ID_INTEL_82801AB_0   0x2420
#define PCI_DEVICE_ID_INTEL_82801AB_1   0x2421
#define PCI_DEVICE_ID_INTEL_82801AB_2   0x2422
#define PCI_DEVICE_ID_INTEL_82801AB_3   0x2423
#define PCI_DEVICE_ID_INTEL_82801AB_5   0x2425
#define PCI_DEVICE_ID_INTEL_82801AB_6   0x2426
#define PCI_DEVICE_ID_INTEL_82801AB_8   0x2428
#define PCI_DEVICE_ID_INTEL_82820FW_0   0x2440
#define PCI_DEVICE_ID_INTEL_82820FW_1   0x2442
#define PCI_DEVICE_ID_INTEL_82820FW_2   0x2443
#define PCI_DEVICE_ID_INTEL_82820FW_3   0x2444
#define PCI_DEVICE_ID_INTEL_82820FW_4   0x2449
#define PCI_DEVICE_ID_INTEL_82820FW_5   0x244b
#define PCI_DEVICE_ID_INTEL_82820FW_6   0x244e
#define PCI_DEVICE_ID_INTEL_82810_MC1   0x7120
#define PCI_DEVICE_ID_INTEL_82810_IG1   0x7121
#define PCI_DEVICE_ID_INTEL_82810_MC3   0x7122
#define PCI_DEVICE_ID_INTEL_82810_IG3   0x7123
#define PCI_DEVICE_ID_INTEL_82443LX_0   0x7180
#define PCI_DEVICE_ID_INTEL_82443LX_1   0x7181
#define PCI_DEVICE_ID_INTEL_82443BX_0   0x7190
#define PCI_DEVICE_ID_INTEL_82443BX_1   0x7191
#define PCI_DEVICE_ID_INTEL_82443BX_2   0x7192
#define PCI_DEVICE_ID_INTEL_82443GX_0           0x71A0
#define PCI_DEVICE_ID_INTEL_82443GX_1           0x71A1
#define PCI_DEVICE_ID_INTEL_82443GX_2           0x71A2
#define PCI_DEVICE_ID_INTEL_P6                  0x84c4

#define PCI_DEVICE_ID_INTEL_82443MX_0   0x7198
#define PCI_DEVICE_ID_INTEL_82443MX_1   0x7199
#define PCI_DEVICE_ID_INTEL_82443MX_2   0x719a
#define PCI_DEVICE_ID_INTEL_82443MX_3   0x719b
#define PCI_DEVICE_ID_INTEL_82372FB_0   0x7600
#define PCI_DEVICE_ID_INTEL_82372FB_1   0x7601
#define PCI_DEVICE_ID_INTEL_82372FB_2   0x7602
#define PCI_DEVICE_ID_INTEL_82372FB_3   0x7603
#define PCI_DEVICE_ID_INTEL_82454GX 0x84c4
#define PCI_DEVICE_ID_INTEL_82450GX 0x84c5
#define PCI_DEVICE_ID_INTEL_82451NX 0x84ca
#define PCI_DEVICE_ID_INTEL_82454NX 0x84cb

#define PCI_VENDOR_ID_COMPUTONE     0x8e0e
#define PCI_DEVICE_ID_COMPUTONE_IP2EX   0x0291
#define PCI_DEVICE_ID_COMPUTONE_PG  0x0302
#define PCI_SUBVENDOR_ID_COMPUTONE  0x8e0e
#define PCI_SUBDEVICE_ID_COMPUTONE_PG4  0x0001
#define PCI_SUBDEVICE_ID_COMPUTONE_PG8  0x0002
#define PCI_SUBDEVICE_ID_COMPUTONE_PG6  0x0003

#define PCI_VENDOR_ID_KTI       0x8e2e
#define PCI_DEVICE_ID_KTI_ET32P2    0x3000

#define PCI_VENDOR_ID_ADAPTEC       0x9004
#define PCI_DEVICE_ID_ADAPTEC_7810  0x1078
#define PCI_DEVICE_ID_ADAPTEC_7821  0x2178
#define PCI_DEVICE_ID_ADAPTEC_38602 0x3860
#define PCI_DEVICE_ID_ADAPTEC_7850  0x5078
#define PCI_DEVICE_ID_ADAPTEC_7855  0x5578
#define PCI_DEVICE_ID_ADAPTEC_5800  0x5800
#define PCI_DEVICE_ID_ADAPTEC_3860  0x6038
#define PCI_DEVICE_ID_ADAPTEC_1480A 0x6075
#define PCI_DEVICE_ID_ADAPTEC_7860  0x6078
#define PCI_DEVICE_ID_ADAPTEC_7861  0x6178
#define PCI_DEVICE_ID_ADAPTEC_7870  0x7078
#define PCI_DEVICE_ID_ADAPTEC_7871  0x7178
#define PCI_DEVICE_ID_ADAPTEC_7872  0x7278
#define PCI_DEVICE_ID_ADAPTEC_7873  0x7378
#define PCI_DEVICE_ID_ADAPTEC_7874  0x7478
#define PCI_DEVICE_ID_ADAPTEC_7895  0x7895
#define PCI_DEVICE_ID_ADAPTEC_7880  0x8078
#define PCI_DEVICE_ID_ADAPTEC_7881  0x8178
#define PCI_DEVICE_ID_ADAPTEC_7882  0x8278
#define PCI_DEVICE_ID_ADAPTEC_7883  0x8378
#define PCI_DEVICE_ID_ADAPTEC_7884  0x8478
#define PCI_DEVICE_ID_ADAPTEC_7885  0x8578
#define PCI_DEVICE_ID_ADAPTEC_7886  0x8678
#define PCI_DEVICE_ID_ADAPTEC_7887  0x8778
#define PCI_DEVICE_ID_ADAPTEC_7888  0x8878
#define PCI_DEVICE_ID_ADAPTEC_1030  0x8b78

#define PCI_VENDOR_ID_ADAPTEC2      0x9005
#define PCI_DEVICE_ID_ADAPTEC2_2940U2   0x0010
#define PCI_DEVICE_ID_ADAPTEC2_2930U2   0x0011
#define PCI_DEVICE_ID_ADAPTEC2_7890B    0x0013
#define PCI_DEVICE_ID_ADAPTEC2_7890 0x001f
#define PCI_DEVICE_ID_ADAPTEC2_3940U2   0x0050
#define PCI_DEVICE_ID_ADAPTEC2_3950U2D  0x0051
#define PCI_DEVICE_ID_ADAPTEC2_7896 0x005f
#define PCI_DEVICE_ID_ADAPTEC2_7892A    0x0080
#define PCI_DEVICE_ID_ADAPTEC2_7892B    0x0081
#define PCI_DEVICE_ID_ADAPTEC2_7892D    0x0083
#define PCI_DEVICE_ID_ADAPTEC2_7892P    0x008f
#define PCI_DEVICE_ID_ADAPTEC2_7899A    0x00c0
#define PCI_DEVICE_ID_ADAPTEC2_7899B    0x00c1
#define PCI_DEVICE_ID_ADAPTEC2_7899D    0x00c3
#define PCI_DEVICE_ID_ADAPTEC2_7899P    0x00cf

#define PCI_VENDOR_ID_ATRONICS      0x907f
#define PCI_DEVICE_ID_ATRONICS_2015 0x2015

#define PCI_VENDOR_ID_HOLTEK        0x9412
#define PCI_DEVICE_ID_HOLTEK_6565   0x6565

#define PCI_SUBVENDOR_ID_EXSYS      0xd84d
#define PCI_SUBDEVICE_ID_EXSYS_4014 0x4014

#define PCI_VENDOR_ID_TIGERJET      0xe159
#define PCI_DEVICE_ID_TIGERJET_300  0x0001

#define PCI_VENDOR_ID_ARK       0xedd8
#define PCI_DEVICE_ID_ARK_STING     0xa091
#define PCI_DEVICE_ID_ARK_STINGARK  0xa099
#define PCI_DEVICE_ID_ARK_2000MT    0xa0a1

/*
 * PCI BIOS���ܵ����б�
 */
#define PCIBIOS_PCI_FUNCTION_ID             0xb100
#define PCIBIOS_PCI_BIOS_PRESENT            0xb101  /*���PCI BIOS�Ĵ�����*/
#define PCIBIOS_FIND_PCI_DEVICE             0xb102  /*�����豸�źͳ��̺Ų���PCI�豸*/
#define PCIBIOS_FIND_PCI_CLASS_CODE         0xb103  /*�����������PCI�豸*/
#define PCIBIOS_GENERATE_SPECIAL_CYCLE      0xb106  /*������������*/
#define PCIBIOS_READ_CONFIG_BYTE            0xb108  /*�������ֽ�*/
#define PCIBIOS_READ_CONFIG_WORD            0xb109  /*��������*/
#define PCIBIOS_READ_CONFIG_DWORD           0xb10a  /*������˫��*/
#define PCIBIOS_WRITE_CONFIG_BYTE           0xb10b  /*д�����ֽ�*/
#define PCIBIOS_WRITE_CONFIG_WORD           0xb10c  /*д������*/
#define PCIBIOS_WRITE_CONFIG_DWORD          0xb10d  /*д����˫��*/
#define PCIBIOS_GET_IRQ_ROUTING_OPTIONS     0xb10e  /*����ж�·����Ϣ*/
#define PCIBIOS_SET_PCI_IRQ                 0xb10f  /*����PCIӲ���ж�*/

/*
 * PCI BIOS���÷���ֵ�б�
 */
#define PCIBIOS_SUCCESSFUL              0x00    /*�ɹ����÷���*/
#define PCIBIOS_FUNC_NOT_SUPPORTED      0x81    /*��֧�ֵĹ��ܵ���*/
#define PCIBIOS_BAD_VENDOR_ID           0x83    /*ָ���˴���ĳ��̺�*/
#define PCIBIOS_DEVICE_NOT_FOUND        0x86    /*δ�ҵ�ָ�����豸*/
#define PCIBIOS_BAD_REGISTER_NUMBER     0x87    /*ָ���˴���ļĴ�����*/
#define PCIBIOS_SET_FAILED              0x88    /*����ʧ��*/
#define PCIBIOS_BUFFER_TOO_SMALL        0x89    /*����̫С*/

/*
 * ʹ�õ�ϵͳ��API�ӿڷ�װ
 */
typedef void (*pci_isr_entry)(T_UWORD);
#define pci_delta_isr_entry	delta_handler
#define Pci_CpuDisable(flag)     disableCpuInterrupt((delta_interrupt_level *)flag)  /*�жϽ�ֹ*/
#define Pci_CpuRestore(flag)     enableCpuInterrupt((delta_interrupt_level)flag)     /*�ж�ʹ��*/
#define Pci_InstallIsr(vector,isr) delta_interrupt_set_handler( (delta_vector)vector, (delta_handler)isr )
#define Pci_Get_Int_Isr(vector,isrp)  getCpuIntVector( (delta_vector)vector,(delta_handler *)isrp )

/*
 * ����΢���PCI IRQ routing table �淶1.0,��0xF0000h--0xFFFFF��Χ��
 * ����PCI IRQ ·�ɱ�ı�־ "$PIR",����ȡ�ñ����ڵ�ַ.
 */
#define PIRQ_SIGNATURE (('$' << 0) + ('P' << 8) + ('I' << 16) + ('R' << 24))
#define PIRQ_VERSION   0x0100
/*
 * BIOS32ǩ���ַ�����_32_��
 */
#define BIOS32_SIGNATURE    (('_' << 0) + ('3' << 8) + ('2' << 16) + ('_' << 24))

/*
 * PCI��ʶ��PCI ��
 */
#define PCI_SIGNATURE       (('P' << 0) + ('C' << 8) + ('I' << 16) + (' ' << 24))

/*
 * PCI�����ʶ����$PCI��
 */
#define PCI_SERVICE     (('$' << 0) + ('P' << 8) + ('C' << 16) + ('I' << 24))

/*
 * DELTAOS����Ĵ���κ����ݶεĻ�ַ
 */
#define KERNEL_CS   0x18
#define KERNEL_DS   0x20
/*
 * pci����ģ��
 */
#define PCI_PROBE_BIOS          1
#define PCI_PROBE_CONF1         2
#define PCI_PROBE_CONF2         4
#define PCI_BIOS_IRQ_SCAN       8

/*
 * PCI�ӿڽ��๦���豸��Ϊ�������豸��ÿ���豸�Ĳ��/���ܵ�ַ
 * ���뵽һ���ֽ��У����£�
 * λ7��λ3����ۺ�
 * λ2��λ0�����ܺ�
 */
#define PCI_DEVFN(slot,func) ((((slot) & 0x1f) << 3) | ((func) & 0x07))
#define PCI_SLOT(devfn)  (((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)  ((devfn) & 0x07)

#define PCI_BASE(baseinfo, baseaddr)  do {  if( baseinfo & PCI_BASE_ADDRESS_SPACE_IO )  {\
										baseaddr = baseinfo & 0x0fffffffe;\
										}\
									else {\
										baseaddr = baseinfo & 0x0fffffff0;\
										}}while(0)
#define PCI_BASE_IOM(baseinfo, iom) do {  if( baseinfo & PCI_BASE_ADDRESS_SPACE_IO )  {\
										iom = 1;\
										}\
									else {\
										iom = 0;\
										}}while(0)

#define PCI_BASE_MEMFETCH(baseinfo, fetch) do {  if( baseinfo & PCI_BASE_ADDRESS_MEM_PREFETCH)  {\
										fetch = 1;\
										}\
									else {\
										fetch = 0;\
										}}while(0)
#define PCI_BASE_MEMWHERE(baseinfo, where)  do {\
											   where = baseinfo>>1;\
											  }while(0)

/*
 * PCIϵͳռ�õ���Դ����
 */
#define  PCI_RES_MEM 		0x10
#define PCI_RES_IO 			0x1
#define PCI_RES_PREMEM 		0x2
#define PCI_RES_IRQ 		0x4
#define PCI_RES_ALL			0x8
#define	PCI_RES_NONE		0x10

/*
 * PCI Command �Ĵ�����������
 */
#define PCI_DIS_IO      	0x00000001
#define PCI_EN_IO       	0x00000002
#define PCI_DIS_MEM     	0x00000004
#define PCI_EN_MEM      	0x00000008
#define PCI_DIS_MASTER  	0x00000010
#define PCI_EN_MASTER   	0x00000020
#define PCI_DIS_SPCCON  	0x00000040
#define PCI_EN_SPCCON   	0x00000080
#define PCI_DIS_CACHEW  	0x00000100
#define PCI_EN_CACHEW   	0x00000200
#define PCI_DIS_VGAM    	0x00000400
#define PCI_EN_VGAM    		0x00000800
#define PCI_DIS_PARITY  	0x00001000
#define PCI_EN_PARITY   	0x00002000
#define PCI_DIS_SERR    	0x00004000
#define PCI_EN_SERR     	0x00008000
#define PCI_DIS_BTB     	0x00010000
#define PCI_EN_BTB      	0x00020000
#define PCI_DIS_INTX    	0x00040000
#define PCI_EN_INTX     	0x00080000

/*
 * PCI���ܵ��÷���ֵ
 */
#define PCI_OP_SUCCESS          	0
#define PCI_OP_FAILED          	 	1
#define PCI_OP_NOSUPPORT        	2
#define PCI_OP_NOREADY          	3
#define PCI_OP_ERRORPARM			4

#define PCI_NO_INIT 				5
#define PCI_FOUND_DEVICE 			6
#define PCI_NO_FOUND_DEVICE 		7

#define PCI_FOUND_BUS 				8
#define PCI_NO_FOUND_BUS 			9

/*
 * ��PCIģ����ʹ��PCI_DRV_DBG��������ʾ���.
 */
 #define PCI_DRV_DBG
 
/*
 *��ȡpci�豸����Ϣ�Ļ����С.�ò�����ϵͳ��Pci�豸�ĸ���������
 */
#define PCILIST_BUF_SIZE 3000

/*
 * Flags given in the bus and device attachment args.
 *
 * OpenBSD doesn't actually use them yet -- csapuntz@cvs.openbsd.org
 */
#define	PCI_FLAGS_IO_ENABLED	0x01		/* I/O space is enabled */
#define	PCI_FLAGS_MEM_ENABLED	0x02		/* memory space is enabled */

#define PCI_INT_0            0

#define P6032INT_BONITO_BASE	16
#define P6032INT_MBOX0		16
#define P6032INT_MBOX1		17
#define P6032INT_MBOX2		18
#define P6032INT_MBOX3		19
#define P6032INT_DMARDY		20
#define P6032INT_DMAEMPTY	21
#define P6032INT_COPYRDY	22
#define P6032INT_COPYEMPTYRDY	23
#define P6032INT_COPYERR	24
#define P6032INT_PCIIRQ		25
#define P6032INT_MASTERERR	26
#define P6032INT_SYSTEMERR	27
#define P6032INT_DRAMPERR	28
#define P6032INT_MTIMEOUT	29
#define P6032INT_IRQA		20

/* Index values */
#define PCI_INTLINE_A	0
#define PCI_INTLINE_B	1
#define PCI_INTLINE_C	2
#define PCI_INTLINE_D	3

#define IRQ_ROUTE_REG1 0x51
#define IRQ_ROUTE_REG2 0x52
#define IRQ_ROUTE_REG4 0x55
#define IRQ_ROUTE_REG5 0x56
#define IRQ_ROUTE_REG6 0x57
#define PCI_IRQ_TYPE_REG 0x54

#define VTSB_BUS 0
#define VTSB_DEV 5
#define VTSB_ISA_FUNC 0
#define VTSB_IDE_FUNC 1

/* base classes */
#define	PCI_CLASS_PREHISTORIC			0x00
#define	PCI_CLASS_MASS_STORAGE			0x01
#define	PCI_CLASS_NETWORK			0x02
#define	PCI_CLASS_DISPLAY			0x03
#define	PCI_CLASS_MULTIMEDIA			0x04
#define	PCI_CLASS_MEMORY			0x05
#define	PCI_CLASS_BRIDGE			0x06
#define	PCI_CLASS_COMMUNICATIONS		0x07
#define	PCI_CLASS_SYSTEM			0x08
#define	PCI_CLASS_INPUT				0x09
#define	PCI_CLASS_DOCK				0x0a
#define	PCI_CLASS_PROCESSOR			0x0b
#define	PCI_CLASS_SERIALBUS			0x0c
#define	PCI_CLASS_WIRELESS			0x0d
#define	PCI_CLASS_I2O				0x0e
#define	PCI_CLASS_SATCOM			0x0f
#define	PCI_CLASS_CRYPTO			0x10
#define	PCI_CLASS_DASP				0x11
#define	PCI_CLASS_UNDEFINED			0xff

/*
 * Mapping registers
 */
#define	PCI_MAPREG_START		0x10
#define	PCI_MAPREG_END			0x28
#define	PCI_MAPREG_ROM			0x30
#define	PCI_MAPREG_PPB_END		0x18
#define	PCI_MAPREG_PCB_END		0x14

#define	PCI_MAPREG_TYPE(mr)						\
	    ((mr) & PCI_MAPREG_TYPE_MASK)
#define	PCI_MAPREG_TYPE_MASK			0x00000001

#define	PCI_MAPREG_TYPE_MEM			0x00000000
#define	PCI_MAPREG_TYPE_IO			0x00000001
#define	PCI_MAPREG_TYPE_ROM			0x00000001

#define	PCI_MAPREG_MEM_TYPE(mr)						\
	    ((mr) & PCI_MAPREG_MEM_TYPE_MASK)
#define	PCI_MAPREG_MEM_TYPE_MASK		0x00000006

#define	PCI_MAPREG_MEM_TYPE_32BIT		0x00000000
#define	PCI_MAPREG_MEM_TYPE_32BIT_1M		0x00000002
#define	PCI_MAPREG_MEM_TYPE_64BIT		0x00000004

#define	PCI_MAPREG_MEM_CACHEABLE(mr)					\
	    (((mr) & PCI_MAPREG_MEM_CACHEABLE_MASK) != 0)
#define	PCI_MAPREG_MEM_CACHEABLE_MASK		0x00000008

#define	PCI_MAPREG_MEM_PREFETCHABLE(mr)					\
	    (((mr) & PCI_MAPREG_MEM_PREFETCHABLE_MASK) != 0)
#define	PCI_MAPREG_MEM_PREFETCHABLE_MASK	0x00000008

#define	PCI_MAPREG_MEM_ADDR(mr)						\
	    ((mr) & PCI_MAPREG_MEM_ADDR_MASK)
#define	PCI_MAPREG_MEM_SIZE(mr)						\
	    (PCI_MAPREG_MEM_ADDR(mr) & -PCI_MAPREG_MEM_ADDR(mr))
#define	PCI_MAPREG_MEM_ADDR_MASK		0xfffffff0

#define	PCI_MAPREG_MEM64_ADDR(mr)					\
	    ((mr) & PCI_MAPREG_MEM64_ADDR_MASK)
#define	PCI_MAPREG_MEM64_SIZE(mr)					\
	    (PCI_MAPREG_MEM64_ADDR(mr) & -PCI_MAPREG_MEM64_ADDR(mr))
#define	PCI_MAPREG_MEM64_ADDR_MASK		0xfffffffffffffff0

#define	PCI_MAPREG_IO_ADDR(mr)						\
	    ((mr) & PCI_MAPREG_IO_ADDR_MASK)
#define	PCI_MAPREG_IO_SIZE(mr)						\
	    (PCI_MAPREG_IO_ADDR(mr) & -PCI_MAPREG_IO_ADDR(mr))
#define	PCI_MAPREG_IO_ADDR_MASK			0xfffffffe

#define	PCI_MAPREG_ROM_ADDR(mr)						\
	    ((mr) & PCI_MAPREG_ROM_ADDR_MASK)
#define	PCI_MAPREG_ROM_SIZE(mr)						\
	    (PCI_MAPREG_ROM_ADDR(mr) & -PCI_MAPREG_ROM_ADDR(mr))
#define	PCI_MAPREG_ROM_ADDR_MASK		0xfffff800

#define PCI_ISCLASS(what, class, subclass) \
	(((what) & 0xffff0000) == (class << 24 | subclass << 16))

#define BONITO(x)	*(volatile unsigned long *)(0xbfe00000+(x))

/* config registers for header type 0 devices */

#define PCI_MAPS	0x10
#define PCI_CARDBUSCIS	0x28
#define PCI_SUBVEND_0	0x2c
#define PCI_SUBDEV_0	0x2e
#define PCI_INTLINE	0x3c
#define PCI_INTPIN	      0x3d
#define PCI_MINGNT	0x3e
#define PCI_MAXLAT	0x3f

/* 0x00 prehistoric subclasses */
#define	PCI_SUBCLASS_PREHISTORIC_MISC		0x00
#define	PCI_SUBCLASS_PREHISTORIC_VGA		0x01

/* 0x01 mass storage subclasses */
#define	PCI_SUBCLASS_MASS_STORAGE_SCSI		0x00
#define	PCI_SUBCLASS_MASS_STORAGE_IDE		0x01
#define	PCI_SUBCLASS_MASS_STORAGE_FLOPPY	0x02
#define	PCI_SUBCLASS_MASS_STORAGE_IPI		0x03
#define	PCI_SUBCLASS_MASS_STORAGE_RAID		0x04
#define	PCI_SUBCLASS_MASS_STORAGE_ATA		0x05
#define	PCI_SUBCLASS_MASS_STORAGE_MISC		0x80

/* 0x02 network subclasses */
#define	PCI_SUBCLASS_NETWORK_ETHERNET		0x00
#define	PCI_SUBCLASS_NETWORK_TOKENRING		0x01
#define	PCI_SUBCLASS_NETWORK_FDDI		0x02
#define	PCI_SUBCLASS_NETWORK_ATM		0x03
#define	PCI_SUBCLASS_NETWORK_ISDN		0x04
#define	PCI_SUBCLASS_NETWORK_WORLDFIP		0x05
#define	PCI_SUBCLASS_NETWORK_PCIMGMULTICOMP	0x06
#define	PCI_SUBCLASS_NETWORK_MISC		0x80

/* 0x03 display subclasses */
//#define	PCI_SUBCLASS_DISPLAY_VGA		0x00
//#define	PCI_SUBCLASS_DISPLAY_XGA		0x01
//#define	PCI_SUBCLASS_DISPLAY_3D			0x02
#define	PCI_SUBCLASS_DISPLAY_MISC		0x80

/* 0x04 multimedia subclasses */
#define	PCI_SUBCLASS_MULTIMEDIA_VIDEO		0x00
#define	PCI_SUBCLASS_MULTIMEDIA_AUDIO		0x01
#define	PCI_SUBCLASS_MULTIMEDIA_TELEPHONY	0x02
#define	PCI_SUBCLASS_MULTIMEDIA_MISC		0x80

/* 0x05 memory subclasses */
#define	PCI_SUBCLASS_MEMORY_RAM			0x00
#define	PCI_SUBCLASS_MEMORY_FLASH		0x01
#define	PCI_SUBCLASS_MEMORY_MISC		0x80

/* 0x06 bridge subclasses */
#define	PCI_SUBCLASS_BRIDGE_HOST		0x00
#define	PCI_SUBCLASS_BRIDGE_ISA			0x01
#define	PCI_SUBCLASS_BRIDGE_EISA		0x02
#define	PCI_SUBCLASS_BRIDGE_MC			0x03
#define	PCI_SUBCLASS_BRIDGE_PCI			0x04
#define	PCI_SUBCLASS_BRIDGE_PCMCIA		0x05
#define	PCI_SUBCLASS_BRIDGE_NUBUS		0x06
#define	PCI_SUBCLASS_BRIDGE_CARDBUS		0x07
#define	PCI_SUBCLASS_BRIDGE_RACEWAY		0x08
#define	PCI_SUBCLASS_BRIDGE_STPCI		0x09
#define	PCI_SUBCLASS_BRIDGE_INFINIBAND		0x0a
#define	PCI_SUBCLASS_BRIDGE_MISC		0x80

/* 0x07 communications subclasses */
#define	PCI_SUBCLASS_COMMUNICATIONS_SERIAL	0x00
#define	PCI_SUBCLASS_COMMUNICATIONS_PARALLEL	0x01
#define	PCI_SUBCLASS_COMMUNICATIONS_MPSERIAL	0x02
#define	PCI_SUBCLASS_COMMUNICATIONS_MODEM	0x03
#define	PCI_SUBCLASS_COMMUNICATIONS_MISC	0x80

/* 0x08 system subclasses */
#define	PCI_SUBCLASS_SYSTEM_PIC			0x00
#define	PCI_SUBCLASS_SYSTEM_DMA			0x01
#define	PCI_SUBCLASS_SYSTEM_TIMER		0x02
#define	PCI_SUBCLASS_SYSTEM_RTC			0x03
#define	PCI_SUBCLASS_SYSTEM_PCIHOTPLUG		0x04
#define	PCI_SUBCLASS_SYSTEM_MISC		0x80

/* 0x09 input subclasses */
#define	PCI_SUBCLASS_INPUT_KEYBOARD		0x00
#define	PCI_SUBCLASS_INPUT_DIGITIZER		0x01
//#define	PCI_SUBCLASS_INPUT_MOUSE		0x02
#define	PCI_SUBCLASS_INPUT_SCANNER		0x03
//#define	PCI_SUBCLASS_INPUT_GAMEPORT		0x04
#define	PCI_SUBCLASS_INPUT_MISC			0x80

/* 0x0a dock subclasses */
#define	PCI_SUBCLASS_DOCK_GENERIC		0x00
#define	PCI_SUBCLASS_DOCK_MISC			0x80

/* 0x0b processor subclasses */
//#define	PCI_SUBCLASS_PROCESSOR_386		0x00
//#define	PCI_SUBCLASS_PROCESSOR_486		0x01
//#define	PCI_SUBCLASS_PROCESSOR_PENTIUM		0x02
//#define	PCI_SUBCLASS_PROCESSOR_ALPHA		0x10
//#define	PCI_SUBCLASS_PROCESSOR_POWERPC		0x20
#define	PCI_SUBCLASS_PROCESSOR_MIPS		0x30
//#define	PCI_SUBCLASS_PROCESSOR_COPROC		0x40

/* 0x0c serial bus subclasses */
#define	PCI_SUBCLASS_SERIALBUS_FIREWIRE		0x00
#define	PCI_SUBCLASS_SERIALBUS_ACCESS		0x01
#define	PCI_SUBCLASS_SERIALBUS_SSA		0x02
#define	PCI_SUBCLASS_SERIALBUS_USB		0x03
#define	PCI_SUBCLASS_SERIALBUS_FIBER		0x04
#define	PCI_SUBCLASS_SERIALBUS_SMBUS		0x05
#define	PCI_SUBCLASS_SERIALBUS_INFINIBAND	0x06
#define	PCI_SUBCLASS_SERIALBUS_IPMI		0x07
#define	PCI_SUBCLASS_SERIALBUS_SERCOS		0x08
#define	PCI_SUBCLASS_SERIALBUS_CANBUS		0x09

/* 0x0d wireless subclasses */
//#define	PCI_SUBCLASS_WIRELESS_IRDA		0x00
#define	PCI_SUBCLASS_WIRELESS_CONSUMERIR	0x01
//#define	PCI_SUBCLASS_WIRELESS_RF		0x10
#define	PCI_SUBCLASS_WIRELESS_MISC		0x80

/* 0x0e I2O (Intelligent I/O) subclasses */
#define	PCI_SUBCLASS_I2O_STANDARD		0x00

/* 0x0f satellite communication subclasses */
/*	PCI_SUBCLASS_SATCOM_???			0x00    / * XXX ??? */
//#define	PCI_SUBCLASS_SATCOM_TV			0x01
//#define	PCI_SUBCLASS_SATCOM_AUDIO		0x02
//#define	PCI_SUBCLASS_SATCOM_VOICE		0x03
//#define	PCI_SUBCLASS_SATCOM_DATA		0x04

/* 0x10 encryption/decryption subclasses */
#define	PCI_SUBCLASS_CRYPTO_NETCOMP		0x00
#define	PCI_SUBCLASS_CRYPTO_ENTERTAINMENT	0x10
#define	PCI_SUBCLASS_CRYPTO_MISC		0x80

/* 0x11 data acquisition and signal processing subclasses */
#define	PCI_SUBCLASS_DASP_DPIO			0x00
#define	PCI_SUBCLASS_DASP_TIMEFREQ		0x01
#define	PCI_SUBCLASS_DASP_MISC			0x80

/*********************************************************************/
/*PCI map	                                                     */
/*********************************************************************/
#define PCI_MEM_SPACE_PCI_BASE		0x00000000
#define PCI_LOCAL_MEM_PCI_BASE		0x80000000
#define PCI_LOCAL_MEM_ISA_BASE		0x00800000
#define PCI_LOCAL_REG_PCI_BASE		0x90000000
#define PCI_IO_SPACE_BASE		    0x00000000

#define BONITO_BOOT_BASE		0x1fc00000
#define BONITO_BOOT_SIZE		0x00100000
#define BONITO_BOOT_TOP 		(BONITO_BOOT_BASE+BONITO_BOOT_SIZE-1)
#define BONITO_FLASH_BASE		0x1c000000
#define BONITO_FLASH_SIZE		0x03000000
#define BONITO_FLASH_TOP		(BONITO_FLASH_BASE+BONITO_FLASH_SIZE-1)
#define BONITO_SOCKET_BASE		0x1f800000
#define BONITO_SOCKET_SIZE		0x00400000
#define BONITO_SOCKET_TOP		(BONITO_SOCKET_BASE+BONITO_SOCKET_SIZE-1)
#define BONITO_REG_BASE 		0x1fe00000
#define BONITO_REG_SIZE 		0x00040000
#define BONITO_REG_TOP			(BONITO_REG_BASE+BONITO_REG_SIZE-1)
#define BONITO_DEV_BASE 		0x1ff00000
#define BONITO_DEV_SIZE 		0x00100000
#define BONITO_DEV_TOP			(BONITO_DEV_BASE+BONITO_DEV_SIZE-1)
#define BONITO_PCILO_BASE		0x10000000
#define BONITO_PCILO_BASE_VA    0xb0000000
#define BONITO_PCILO_SIZE		0x0c000000
#define BONITO_PCILO_TOP		(BONITO_PCILO_BASE+BONITO_PCILO_SIZE-1)
#define BONITO_PCILO0_BASE		0x10000000
#define BONITO_PCILO1_BASE		0x14000000
#define BONITO_PCILO2_BASE		0x18000000
#define BONITO_PCIHI_BASE		0x20000000
#define BONITO_PCIHI_SIZE		0x20000000
#define BONITO_PCIHI_TOP		(BONITO_PCIHI_BASE+BONITO_PCIHI_SIZE-1)
#define BONITO_PCIIO_BASE		0x1fd00000
#define BONITO_PCIIO_BASE_VA		0xbfd00000
#define BONITO_PCIIO_SIZE		0x00010000
#define BONITO_PCIIO_TOP		(BONITO_PCIIO_BASE+BONITO_PCIIO_SIZE-1)
#define BONITO_PCICFG_BASE		0x1fe80000
#define BONITO_PCICFG_SIZE		0x00080000
#define BONITO_PCICFG_TOP		(BONITO_PCICFG_BASE+BONITO_PCICFG_SIZE-1)

/* Bonito Register Bases */
#define BONITO_PCICONFIGBASE		0x00
#define BONITO_REGBASE			0x100

/* PCI Configuration  Registers */
#define BONITO_PCI_REG(x)      BONITO(BONITO_PCICONFIGBASE + x)
#define BONITO_PCIDID			BONITO_PCI_REG(0x00)
#define BONITO_PCICMD			BONITO_PCI_REG(0x04)
#define BONITO_PCICLASS 		BONITO_PCI_REG(0x08)
#define BONITO_PCILTIMER		BONITO_PCI_REG(0x0c)
#define BONITO_PCIBASE0 		BONITO_PCI_REG(0x10)
#define BONITO_PCIBASE1 		BONITO_PCI_REG(0x14)
#define BONITO_PCIBASE2 		BONITO_PCI_REG(0x18)
#define BONITO_PCIEXPRBASE		BONITO_PCI_REG(0x30)
#define BONITO_PCIINT			BONITO_PCI_REG(0x3c)

#define BONITO_PCICMD_PERR_CLR		0x80000000
#define BONITO_PCICMD_SERR_CLR		0x40000000
#define BONITO_PCICMD_MABORT_CLR	0x20000000
#define BONITO_PCICMD_MTABORT_CLR	0x10000000
#define BONITO_PCICMD_TABORT_CLR	0x08000000
#define BONITO_PCICMD_MPERR_CLR 	0x01000000
#define BONITO_PCICMD_PERRRESPEN	0x00000040
#define BONITO_PCICMD_ASTEPEN		0x00000080
#define BONITO_PCICMD_SERREN		0x00000100
#define BONITO_PCILTIMER_BUSLATENCY	0x0000ff00
#define BONITO_PCILTIMER_BUSLATENCY_SHIFT	8

/* 4. PCI address map control */
#define BONITO_PCIMAP			BONITO(BONITO_REGBASE + 0x10)
#define BONITO_PCIMEMBASECFG	BONITO(BONITO_REGBASE + 0x14)
#define BONITO_PCIMAP_CFG		BONITO(BONITO_REGBASE + 0x18)

/* pcimap */

#define BONITO_PCIMAP_PCIMAP_LO0	0x0000003f
#define BONITO_PCIMAP_PCIMAP_LO0_SHIFT	0
#define BONITO_PCIMAP_PCIMAP_LO1	0x00000fc0
#define BONITO_PCIMAP_PCIMAP_LO1_SHIFT	6
#define BONITO_PCIMAP_PCIMAP_LO2	0x0003f000
#define BONITO_PCIMAP_PCIMAP_LO2_SHIFT	12
#define BONITO_PCIMAP_PCIMAP_2		0x00040000
#define BONITO_PCIMAP_WIN(WIN,ADDR)	((((ADDR)>>26) & BONITO_PCIMAP_PCIMAP_LO0) << ((WIN)*6))


/*
 * Command and status register.
 */
#define	PCI_COMMAND_STATUS_REG			0x04

#define	PCI_COMMAND_IO_ENABLE			0x00000001
#define	PCI_COMMAND_MEM_ENABLE			0x00000002
#define	PCI_COMMAND_MASTER_ENABLE		0x00000004
#define	PCI_COMMAND_SPECIAL_ENABLE		0x00000008
#define	PCI_COMMAND_INVALIDATE_ENABLE		0x00000010
#define	PCI_COMMAND_PALETTE_ENABLE		0x00000020
#define	PCI_COMMAND_PARITY_ENABLE		0x00000040
#define	PCI_COMMAND_STEPPING_ENABLE		0x00000080
#define	PCI_COMMAND_SERR_ENABLE			0x00000100
#define	PCI_COMMAND_BACKTOBACK_ENABLE		0x00000200

#define	PCI_STATUS_CAPLIST_SUPPORT		0x00100000
#define	PCI_STATUS_66MHZ_SUPPORT		0x00200000
#define	PCI_STATUS_UDF_SUPPORT			0x00400000
#define	PCI_STATUS_BACKTOBACK_SUPPORT		0x00800000
//#define	PCI_STATUS_PARITY_ERROR			0x01000000
//#define	PCI_STATUS_DEVSEL_FAST			0x00000000
//#define	PCI_STATUS_DEVSEL_MEDIUM		0x02000000
//#define	PCI_STATUS_DEVSEL_SLOW			0x04000000
//#define	PCI_STATUS_DEVSEL_MASK			0x06000000
#define	PCI_STATUS_TARGET_TARGET_ABORT		0x08000000
#define	PCI_STATUS_MASTER_TARGET_ABORT		0x10000000
#define	PCI_STATUS_MASTER_ABORT			0x20000000
#define	PCI_STATUS_SPECIAL_ERROR		0x40000000
#define	PCI_STATUS_PARITY_DETECT		0x80000000


#define CACHED_MEMORY_ADDR	0x80000000
#define UNCACHED_MEMORY_ADDR	0xa0000000
#define	CACHED_TO_PHYS(x)	((unsigned)(x) & 0x1fffffff)
#define	PHYS_TO_UNCACHED(x) 	((unsigned)(x) | UNCACHED_MEMORY_ADDR)
#define	CACHED_TO_UNCACHED(x)	(PHYS_TO_UNCACHED(CACHED_TO_PHYS(x)))
#define vtophys(x) (UNCACHED_TO_PHYS(x) + 0x80000000)

#define  PCI_BASE_ADDRESS_0    0x10 /* 32λ */
#define  PCI_BASE_ADDRESS_1    0x14 /* 32λ */
#define  PCI_BASE_ADDRESS_2    0x18 /* 32λ */
#define  PCI_BASE_ADDRESS_3    0x1c /* 32λ */
#define  PCI_BASE_ADDRESS_4    0x20 /* 32λ */
#define  PCI_BASE_ADDRESS_5    0x24 /* 32λ */
#define  PCI_BASE_ADDRESS_SPACE   0x01 /* ��Ӧ��ַ�Ĵ�����λ0�������Ǻ��ֵ�ַ�ռ䣺0 = memory, 1 = I/O */
#define  PCI_BASE_ADDRESS_SPACE_IO   0x01 /* ��ʾI/O��ַ�ռ� */
#define  PCI_BASE_ADDRESS_SPACE_MEMORY  0x00 /* ��ʾmemory��ַ�ռ� */
#define  PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06 /* ��Ӧ��ַ�Ĵ�����λ2��λ1������memoryӳ������ */
#define  PCI_BASE_ADDRESS_MEM_TYPE_32 0x00 /* ӳ�䵽32λ��ַ�ռ� */
//#define  PCI_BASE_ADDRESS_MEM_TYPE_1M 0x02 /* Below 1M */
#define  PCI_BASE_ADDRESS_MEM_TYPE_64 0x04 /* ӳ�䵽64λ��ַ�ռ� */
#define  PCI_BASE_ADDRESS_MEM_PREFETCH 0x08 /* ��Ӧ��ַ�Ĵ�����λ3����ʾԤȡ�� */
#define  PCI_BASE_ADDRESS_MEM_MASK  (~0x0f) /* memory��ַ������ */
#define  PCI_BASE_ADDRESS_IO_MASK  (~0x03) /* I/O��ַ������ */

/*
 * Device identification register; contains a vendor ID and a device ID.
 */
#define	PCI_ID_REG			0x00

#define	PCI_VENDOR_SHIFT			0
#define	PCI_VENDOR_MASK				0xffff
#define	PCI_VENDOR(id) \
	    (((id) >> PCI_VENDOR_SHIFT) & PCI_VENDOR_MASK)

#define	PCI_PRODUCT_SHIFT			16
#define	PCI_PRODUCT_MASK			0xffff
#define	PCI_PRODUCT(id) \
	    (((id) >> PCI_PRODUCT_SHIFT) & PCI_PRODUCT_MASK)


/*
 * PCI Class and Revision Register; defines type and revision of device.
 */
#define	PCI_CLASS_REG			0x08
/*
 * PCI BIST/Header Type/Latency Timer/Cache Line Size Register.
 */
#define	PCI_BHLC_REG			0x0c

#define	PCI_BIST_SHIFT				24
#define	PCI_BIST_MASK				0xff
/* #define	PCI_BIST(bhlcr)
	    (((bhlcr) >> PCI_BIST_SHIFT) & PCI_BIST_MASK) */

#define	PCI_HDRTYPE_SHIFT			16
#define	PCI_HDRTYPE_MASK			0xff
#define	PCI_HDRTYPE(bhlcr) \
	    (((bhlcr) >> PCI_HDRTYPE_SHIFT) & PCI_HDRTYPE_MASK)

#define PCI_HDRTYPE_TYPE(bhlcr) \
	    (PCI_HDRTYPE(bhlcr) & 0x7f)
#define	PCI_HDRTYPE_MULTIFN(bhlcr) \
	    ((PCI_HDRTYPE(bhlcr) & 0x80) != 0)

#define	PCI_LATTIMER_SHIFT			8
#define	PCI_LATTIMER_MASK			0xff
#define	PCI_LATTIMER(bhlcr) \
	    (((bhlcr) >> PCI_LATTIMER_SHIFT) & PCI_LATTIMER_MASK)

#define	PCI_CACHELINE_SHIFT			0
#define	PCI_CACHELINE_MASK			0xff
#define	PCI_CACHELINE(bhlcr) \
	    (((bhlcr) >> PCI_CACHELINE_SHIFT) & PCI_CACHELINE_MASK)


/* config registers for header type 1 devices */

#define PCI_SECSTAT_1	0 /**/

#define PCI_PRIBUS_1	0x18
#define PCI_SECBUS_1	0x19
#define PCI_SUBBUS_1	0x1a
#define PCI_SECLAT_1	0x1b

#define PCI_IOBASEL_1	0x1c
#define PCI_IOLIMITL_1	0x1d
#define PCI_IOBASEH_1	0x30 /**/
#define PCI_IOLIMITH_1	0x32 /**/

#define PCI_MEMBASE_1	0x20
#define PCI_MEMLIMIT_1	0x22

#define PCI_PMBASEL_1	0x24
#define PCI_PMLIMITL_1	0x26
#define PCI_PMBASEH_1	0 /**/
#define PCI_PMLIMITH_1	0 /**/

#define PCI_BRIDGECTL_1 0 /**/

#define PCI_SUBVEND_1	0x34
#define PCI_SUBDEV_1	0x36

#define	PCI_CLASS_SHIFT				24
#define	PCI_CLASS_MASK				0xff
#define	PCI_CLASS(cr) \
	    (((cr) >> PCI_CLASS_SHIFT) & PCI_CLASS_MASK)

#define	PCI_SUBCLASS_SHIFT			16
#define	PCI_SUBCLASS_MASK			0xff
#define	PCI_SUBCLASS(cr) \
	    (((cr) >> PCI_SUBCLASS_SHIFT) & PCI_SUBCLASS_MASK)

#define PCI_ISCLASS(what, class, subclass) \
	(((what) & 0xffff0000) == (class << 24 | subclass << 16))

#define	PCI_INTERFACE_SHIFT			8
#define	PCI_INTERFACE_MASK			0xff
#define	PCI_INTERFACE(cr) \
	    (((cr) >> PCI_INTERFACE_SHIFT) & PCI_INTERFACE_MASK)

#define	PCI_REVISION_SHIFT			0
#define	PCI_REVISION_MASK			0xff
#define	PCI_REVISION(cr) \
	    (((cr) >> PCI_REVISION_SHIFT) & PCI_REVISION_MASK)

/*
 * Interrupt Configuration Register; contains interrupt pin and line.
 */
#define	PCI_INTERRUPT_REG		0x3c

#define	PCI_INTERRUPT_PIN_SHIFT			8
#define	PCI_INTERRUPT_PIN_MASK			0xff
/* #define	PCI_INTERRUPT_PIN(icr) \
	    (((icr) >> PCI_INTERRUPT_PIN_SHIFT) & PCI_INTERRUPT_PIN_MASK) */

#define	PCI_INTERRUPT_LINE_SHIFT		0
#define	PCI_INTERRUPT_LINE_MASK			0xff
/* #define	PCI_INTERRUPT_LINE(icr) \
	    (((icr) >> PCI_INTERRUPT_LINE_SHIFT) & PCI_INTERRUPT_LINE_MASK) */

#define	PCI_MIN_GNT_SHIFT			16
#define	PCI_MIN_GNT_MASK			0xff
/* #define	PCI_MIN_GNT(icr) \
	    (((icr) >> PCI_MIN_GNT_SHIFT) & PCI_MIN_GNT_MASK) */

#define	PCI_MAX_LAT_SHIFT			24
#define	PCI_MAX_LAT_MASK			0xff
/* #define	PCI_MAX_LAT(icr) \
	    (((icr) >> PCI_MAX_LAT_SHIFT) & PCI_MAX_LAT_MASK) */

#define	PCI_INTERRUPT_PIN_NONE		0x00
#define	PCI_INTERRUPT_PIN_A			0x01
#define	PCI_INTERRUPT_PIN_B			0x02
#define	PCI_INTERRUPT_PIN_C			0x03
#define	PCI_INTERRUPT_PIN_D			0x04
#define	PCI_INTERRUPT_PIN_MAX		0x04

#ifndef PCI_MAX_DEV
# define PCI_MAX_DEV	32
#endif  /* PCI_MAX_DEV */

#ifndef PCI_MAX_FUNC
# define PCI_MAX_FUNC	8
#endif  /* PCI_MAX_FUNC */

#ifndef PCI_MAX_BUS
#define PCI_MAX_BUS	255	    /* Max number of PCI buses in system */
#endif	/* PCI_MAX_BUS */

#define PCI_CLASS_NETWORK_ETHERNET	0x0200

#define	PCI_CFG_VENDOR_ID	0x00
#define	PCI_CFG_DEVICE_ID	0x02

#define	PCI_CFG_REVISION	0x08

#define	PCI_CFG_HEADER_TYPE	0x0e

#define	PCI_CFG_SECONDARY_BUS	0x19
#define	PCI_CFG_SUBCLASS	0x0a
#define PCI_CLASS_BRIDGE_CTLR    0x06
#define PCI_SUBCLASS_HOST_PCI_BRIDGE   0x00
#define PCI_SUBCLASS_P2P_BRIDGE  0x04

#define PCI_CLASS_NETWORK_CTLR	0x02
#define PCI_SUBCLASS_00         0x00
#define PCI_SUBCLASS_NET_ETHERNET	(PCI_SUBCLASS_00)
#define PCI_NET_ETHERNET_CLASS \
    ((PCI_CLASS_NETWORK_CTLR << 16) | (PCI_SUBCLASS_NET_ETHERNET << 8))
    
#define PCI_HEADER_MULTI_FUNC	0x80

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* TA_PCI_H_ */
