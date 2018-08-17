/****************************************************************************
 *				北京科银京成技术有限公司 版权所有
 *
 * 		 Copyright (C) 2000-2006 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * 修改记录：
 *
 */

/**
 * @file 	halFuloongminipci.h
 * @brief
 *	<li>功能：定义相关数据和宏等。 </li>
 * @author
 */

#ifndef _HAL_FULOONGMINIPCI
#define _HAL_FULOONGMINIPCI

#ifdef	__cplusplus
extern	"C" {
#endif

/**************************** 引用部分 *****************************************/
#include <taTypes.h>	/// 使用系统类型定义
//#include <bspPubInfo.h>
//#include "godson_pcidata.h"
//#include "delta.h"
//#include "stdio.h"
//#include "stdlib.h"
#include "tastring.h"

/**************************** 声明部分 *****************************************/


/**************************** 定义部分 *****************************************/

#if 0
/// 设备接口函数表宏
#define HAL_FULOONGMINIPCI_DLL_FULOONGMINIPCI_INTERFACE	{}


/// HAL设备数据表结构
typedef struct
{
	/// 第一部分：设备公共信息
	T_BSP_DEV_PUB_DATA pub;

	/// 第二部分：HAL私有的，需要用户配置的信息

	/// 第三部分：HAL私有的，不需要用户配置的信息

} T_HAL_FULOONGMINIPCI_DEV_DATA;
#endif


#define PCIVERBOSE 5
#define _PCIVERBOSE PCIVERBOSE

//#define	MIN(a,b) (((a)<(b))?(a):(b))
//#define	MAX(a,b) (((a)>(b))?(a):(b))
/*
 * Flags given in the bus and device attachment args.
 *
 * OpenBSD doesn't actually use them yet -- csapuntz@cvs.openbsd.org
 */
#define	PCI_FLAGS_IO_ENABLED	0x01		/* I/O space is enabled */
#define	PCI_FLAGS_MEM_ENABLED	0x02		/* memory space is enabled */

#define PCI_INT_0            0

#define PCI_DEVFN(slot,func)	((((slot) & 0x1f) << 3) | ((func) & 0x07))
#define PCI_SLOT(devfn)		(((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)		((devfn) & 0x07)


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

/* 0x35-0x3b are reserved */
#define  PCI_INTERRUPT_LINE    0x3c /* 8位的中断线寄存器 */
#define  PCI_INTERRUPT_PIN    0x3d /* 8位的中断引脚寄存器 */
#define  PCI_MIN_GNT     0x3e /* 8位的Min-Gnt */
#define  PCI_MAX_LAT     0x3f /* 8位的Max-Lat */



#define  PCI_BASE_CLASS_STORAGE   0x01 /* 海量存储控制器基类码 */
#define  PCI_CLASS_STORAGE_SCSI   0x0100 /* SCSI控制器 */
#define  PCI_CLASS_STORAGE_IDE   0x0101 /* IDE控制器 */
#define  PCI_CLASS_STORAGE_FLOPPY  0x0102 /* 软盘控制器 */
#define  PCI_CLASS_STORAGE_IPI   0x0103 /* IPI控制器 */
#define  PCI_CLASS_STORAGE_RAID   0x0104 /*  */
#define  PCI_CLASS_STORAGE_OTHER  0x0180 /* 其他控制器 */

#define PCIBIOS_SUCCESSFUL              0x00    /*成功调用返回*/
#define PCIBIOS_FUNC_NOT_SUPPORTED      0x81    /*不支持的功能调用*/
#define PCIBIOS_BAD_VENDOR_ID           0x83    /*指定了错误的厂商号*/
#define PCIBIOS_DEVICE_NOT_FOUND        0x86    /*未找到指定的设备*/
#define PCIBIOS_BAD_REGISTER_NUMBER     0x87    /*指定了错误的寄存器号*/
#define PCIBIOS_SET_FAILED              0x88    /*设置失败*/
#define PCIBIOS_BUFFER_TOO_SMALL        0x89    /*缓冲太小*/

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

//#define	PCI_CACHE_LINE_SIZE 8		/* expressed in 32 bit words */
 /*
  * PCI功能调用返回值
  */
#define PCI_OP_SUCCESS          	0
#define PCI_OP_FAILED          	1
#define PCI_OP_NOSUPPORT        	2
#define PCI_OP_NOREADY          	3
#define PCI_OP_ERRORPARM			4

#define PCI_NO_INIT 				5
#define PCI_FOUND_DEVICE 			6
#define PCI_NO_FOUND_DEVICE 		7

#define PCI_FOUND_BUS 				8
#define PCI_NO_FOUND_BUS 			9

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
//#define	PCI_CLASS_PROCESSOR			0x0b
#define	PCI_CLASS_SERIALBUS			0x0c
//#define	PCI_CLASS_WIRELESS			0x0d
#define	PCI_CLASS_I2O				0x0e
#define	PCI_CLASS_SATCOM			0x0f
#define	PCI_CLASS_CRYPTO			0x10
#define	PCI_CLASS_DASP				0x11
//#define	PCI_CLASS_UNDEFINED			0xff

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
//#define	UNCACHED_TO_PHYS(x) 	((unsigned)(x) & 0x1fffffff)
#define vtophys(x) (UNCACHED_TO_PHYS(x) + 0x80000000)




#define  PCI_BASE_ADDRESS_0    0x10 /* 32位 */
#define  PCI_BASE_ADDRESS_1    0x14 /* 32位 */
#define  PCI_BASE_ADDRESS_2    0x18 /* 32位 */
#define  PCI_BASE_ADDRESS_3    0x1c /* 32位 */
#define  PCI_BASE_ADDRESS_4    0x20 /* 32位 */
#define  PCI_BASE_ADDRESS_5    0x24 /* 32位 */
#define  PCI_BASE_ADDRESS_SPACE   0x01 /* 对应基址寄存器的位0，决定是何种地址空间：0 = memory, 1 = I/O */
#define  PCI_BASE_ADDRESS_SPACE_IO   0x01 /* 表示I/O地址空间 */
#define  PCI_BASE_ADDRESS_SPACE_MEMORY  0x00 /* 表示memory地址空间 */
#define  PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06 /* 对应基址寄存器的位2－位1，决定memory映射类型 */
#define  PCI_BASE_ADDRESS_MEM_TYPE_32 0x00 /* 映射到32位地址空间 */
//#define  PCI_BASE_ADDRESS_MEM_TYPE_1M 0x02 /* Below 1M */
#define  PCI_BASE_ADDRESS_MEM_TYPE_64 0x04 /* 映射到64位地址空间 */
#define  PCI_BASE_ADDRESS_MEM_PREFETCH 0x08 /* 对应基址寄存器的位3，表示预取性 */
#define  PCI_BASE_ADDRESS_MEM_MASK  (~0x0f) /* memory地址的掩码 */
#define  PCI_BASE_ADDRESS_IO_MASK  (~0x03) /* I/O地址的掩码 */




/*
 * Device identification register; contains a vendor ID and a device ID.
 */
#define	PCI_ID_REG			0x00

//typedef u_int16_t pci_vendor_id_t;
//typedef u_int16_t pci_product_id_t;

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

//typedef u_int8_t pci_intr_pin_t;
//typedef u_int8_t pci_intr_line_t;

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

#define	PCI_INTERRUPT_PIN_NONE			0x00
#define	PCI_INTERRUPT_PIN_A			0x01
#define	PCI_INTERRUPT_PIN_B			0x02
#define	PCI_INTERRUPT_PIN_C			0x03
#define	PCI_INTERRUPT_PIN_D			0x04
#define	PCI_INTERRUPT_PIN_MAX			0x04


/*
 * PCI系统占用的资源类型
 */
#define  PCI_RES_MEM 		0x10
#define PCI_RES_IO 			0x1
#define PCI_RES_PREMEM 		0x2
#define PCI_RES_IRQ 		0x4
#define PCI_RES_ALL			0x8
#define	PCI_RES_NONE		0x10

struct pci_bus;
struct pci_device;

/**
 * Structure that gets filled in by pci_bus_probe
 */
//typedef struct  _tPci_Dev_Godson2e            /* PCI device data */
//{
//    unsigned short ven_id;      	/* vendor ID */
//    unsigned short dev_id;          /* device ID */
//    unsigned char  class;          	/* device Class */
//    unsigned char subclass;			/* device Subclass */
//    unsigned long bar_size[6];		/* Memory size for each base address, */
//									/* 0 for unused BAR's */
//    unsigned long bar_map[6];		/* Physical address mapped - */
//									/* 0 for unused BAR's */
//	unsigned long func;				/* The function number - usually 0 */
//	unsigned long dev;				/* The device number */
//	unsigned long bus;				/* The bus this device resides on. */
//									/* Non-0 means it's across a bridge */
//} T_PCI_DEV_INFO;


struct tgt_bus_space {
	T_UWORD	bus_base;
	T_UWORD	bus_reverse;	/* Reverse bus ops (dummy) */
};
typedef struct tgt_bus_space *bus_space_tag_t;

/*
 * PCI device attach arguments.
 */
struct pci_attach_args {
	bus_space_tag_t pa_iot;		/* pci i/o space tag */
	bus_space_tag_t pa_memt;	/* pci mem space tag */
//	bus_dma_tag_t pa_dmat;		/* DMA tag */
//	pci_chipset_tag_t pa_pc;
	int		pa_flags;	/* flags; see below */
	int		pa_bus;

	T_UWORD	pa_device;
	T_UWORD	pa_function;
	T_UWORD	pa_tag;
	T_UWORD	pa_id, pa_class;

	/*
	 * Interrupt information.
	 *
	 * "Intrline" is used on systems whose firmware puts
	 * the right routing data into the line register in
	 * configuration space.  The rest are used on systems
	 * that do not.
	 */
	T_UWORD	pa_intrswiz;	/* how to swizzle pins if ppb */
	T_UWORD	pa_intrtag;	/* intr. appears to come from here */
	T_UBYTE	pa_intrpin;	/* intr. appears on this pin */
	T_UBYTE	pa_intrline;	/* intr. routing information */
};

/*
 *  Structure describing a PCI BUS. An entire PCI BUS
 *  topology will be described by several structures
 *  linked together on a linked list.
 */
#if 0
struct pci_bus {
	struct pci_bus *next;		/* next bus pointer */
	T_UBYTE	min_gnt;        /* largest min grant */
	T_UBYTE	max_lat;        /* smallest max latency */
	T_UBYTE	devsel;         /* slowest devsel */
	T_UBYTE	fast_b2b;       /* support fast b2b */
	T_UBYTE	prefetch;       /* support prefetch */
	T_UBYTE	freq66;         /* support 66MHz */
	T_UBYTE	width64;        /* 64 bit bus */
	T_UBYTE	bus;
	T_UBYTE	ndev;           /* # devices on bus */
	T_UBYTE	def_ltim;       /* default ltim counter */
	T_UBYTE	max_ltim;       /* maximum ltim counter */
	T_WORD		bandwidth;      /* # of .25us ticks/sec @ 33MHz */
	T_UWORD		minpcimemaddr;	/* PCI allocation min mem for bus */
	T_UWORD		nextpcimemaddr;	/* PCI allocation max mem for bus */
	T_UWORD		minpciioaddr;	/* PCI allocation min i/o for bus */
	T_UWORD		nextpciioaddr;	/* PCI allocation max i/o for bus */
	T_UWORD		pci_mem_base;
	T_UWORD		pci_io_base;
};
#endif


struct pci_intline_routing {
    T_UBYTE	bus;
    T_UBYTE	device;
    T_UBYTE	function;
    T_UBYTE	intline[32][4];	/* interrupt line mapping */
    struct pci_intline_routing *next;
};

/*
 * PCI Memory/IO Space
 */
struct pci_win {
	struct pci_win	*next;
	int		reg;
	int		flags;
	T_UWORD	size;
	T_UWORD	address;
	struct pci_device *device;
};

/*
 * PCI Bridge parameters
 */
struct pci_bridge {
	T_UBYTE		pribus_num;	/* Primary Bus number */
	T_UBYTE		secbus_num;	/* Secondary Bus number */
	T_UBYTE		subbus_num;	/* Sub. Bus number */
	struct pci_bus		*secbus;	/* Secondary PCI bus pointer */
	struct pci_device	*child;
	struct pci_win		*memspace;
	struct pci_win		*iospace;
};
/*
 * PCI Device parameters
 */
struct pci_device {
	struct pci_attach_args	pa;
	unsigned char		min_gnt;
	unsigned char		max_lat;
	unsigned char		int_line;
	T_UWORD		stat;
	T_UBYTE		intr_routing[4];
	T_UBYTE		isr;
	struct pci_bridge  	bridge;
	struct pci_bus		*pcibus;
	struct pci_device	*next;
	struct pci_device	*parent;
};

/*
 * Descriptions of known PCI classes and subclasses.
 *
 * Subclasses are described in the same way as classes, but have a
 * NULL subclass pointer.
 */
struct pci_class {
	char		*name;
	int		val;		/* as wide as pci_{,sub}class_t */
	const struct pci_class *subclasses;
};


///// 设备接口函数表宏
//#define HAL_GODSON2EPCI_DLL_GODSON2EPCI_INTERFACE	{}
//
//
///// HAL设备数据表结构
//typedef struct
//{
//	/// 第一部分：设备公共信息
//	T_BSP_DEV_PUB_DATA pub;
//
//	/// 第二部分：HAL私有的，需要用户配置的信息
//
//	/// 第三部分：HAL私有的，不需要用户配置的信息
//
//} T_HAL_GODSON2EPCI_DEV_DATA;


/**************************** 声明部分 *****************************************/
T_EXTERN T_WORD pci_present(T_VOID);
T_EXTERN T_VOID pci_init(T_VOID);
T_EXTERN T_WORD pci_get_dev(T_UBYTE bus, T_UWORD devfn, struct pci_device **dpDev);
T_EXTERN T_WORD pci_find_dev(T_UHWORD vendor,T_UHWORD device_id,T_UHWORD index, struct pci_device **dpDev);
T_EXTERN T_WORD pci_get_baseaddr(struct pci_device *pDev, T_UBYTE base_num, T_UWORD *type, T_UWORD *base);

/**
 * Find device
 */
T_EXTERN T_WORD pci_find_device(T_UHWORD vendor,T_UHWORD device_id,T_UHWORD index,T_UBYTE *bus,T_UWORD *device_fn);

/**
 * Find class
 */
T_EXTERN T_WORD pci_find_class(T_UWORD class_code,T_UHWORD index,T_UBYTE *bus,T_UWORD *device_fn);


T_EXTERN T_WORD pci_godson2e_init(T_VOID);
T_EXTERN T_WORD GodsonPci_Find_Device( T_UHWORD vendor,T_UHWORD device_id,T_UHWORD index,T_UBYTE *bus,T_UWORD *device_fn );
T_EXTERN T_WORD MpcPci_Find_Class(T_UWORD class_code,T_UHWORD index,T_UBYTE *bus,T_UWORD *device_fn);
T_EXTERN T_WORD GodsonPci_Cfg_Rd_Byte (T_UWORD tag,T_UWORD offset,T_UBYTE *value);
T_EXTERN T_WORD GodsonPci_Cfg_Rd_Word (T_UWORD tag,T_UWORD offset,T_UHWORD*value);
T_EXTERN T_WORD GodsonPci_Cfg_Rd_DWord(T_UWORD tag,T_UBYTE offset,T_UWORD *value);
T_EXTERN T_WORD GodsonPci_Cfg_Wr_Byte (T_UWORD tag,T_UWORD offset,T_UBYTE  value);
T_EXTERN T_WORD GodsonPci_Cfg_Wr_Word (T_UWORD tag,T_UWORD offset,T_UHWORD value);
T_EXTERN T_WORD GodsonPci_Cfg_Wr_DWord(T_UWORD tag,T_UWORD offset,T_UHWORD value);

/**
 * Config word operation about read and write
 */
T_EXTERN T_WORD pci_read_config_byte  (struct pci_device *pDevInfo,T_UBYTE where,T_UBYTE  *value);
T_EXTERN T_WORD pci_read_config_word  (struct pci_device *pDevInfo,T_UBYTE where,T_UHWORD *value);
T_EXTERN T_WORD pci_read_config_dword (struct pci_device *pDevInfo,T_UBYTE where,T_UWORD  *value);
T_EXTERN T_WORD pci_write_config_byte (struct pci_device *pDevInfo,T_UBYTE where,T_UBYTE   value);
T_EXTERN T_WORD pci_write_config_word (struct pci_device *pDevInfo,T_UBYTE where,T_UHWORD  value);
T_EXTERN T_WORD pci_write_config_dword(struct pci_device *pDevInfo,T_UBYTE where,T_UWORD   value);


#ifndef PCI_MAX_DEV
# define PCI_MAX_DEV	32
#endif  /* PCI_MAX_DEV */

#ifndef PCI_MAX_FUNC
# define PCI_MAX_FUNC	8
#endif  /* PCI_MAX_FUNC */

#ifndef PCI_MAX_BUS
#define PCI_MAX_BUS	255	    /* Max number of PCI buses in system */
#endif	/* PCI_MAX_BUS */

#define PCI_HEADER_TYPE_MASK	0x7f	/* mask for header type */
#define PCI_HEADER_PCI_PCI	0x01	/* PCI to PCI bridge */
#define PCI_HEADER_TYPE0        0x00    /* normal device header */
#define PCI_HEADER_MULTI_FUNC	0x80	/* multi function device */

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

#ifdef	__cplusplus
}
#endif

#endif


