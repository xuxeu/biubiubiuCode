/********************************************************************
 * 文件名:		taIntel8254x.inc
 --------------------------------------------------------------------
 * 模块:			系统总代理
 --------------------------------------------------------------------
 * 功能:			taIntel8254x网卡驱动程序
 --------------------------------------------------------------------
 * 作者:			卢希
 --------------------------------------------------------------------
 * 单位:			科银-系统部
 --------------------------------------------------------------------
 * 创建日期：	2007-11-12
 --------------------------------------------------------------------
 * 修改记录:		日期				修改者			描述
 * 				2007-11-12    	卢希        		创建该文件
 ********************************************************************/

//#include <sysAgent.h>
#include "memory.h"
#include "tastdio.h"
#include "tastring.h"
#include "taIntel8254x.h"
//#include <setjmp.h>
#include "pci.h"
#include "bios32.h"
#include "halFuloongminipci.h"

/*************************** 前向声明部分 ****************************************/

T_MODULE T_BOOL 	INTEL8254x_Reset_HW(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_BOOL 	INTEL8254x_Read_Mac_Addr(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_BOOL 	INTEL8254x_Read_Eeprom(T_HAL_INTEL8254x_DEV_DATA *pDevData,T_UHWORD offset,T_UHWORD *data);
T_MODULE T_BOOL 	INTEL8254x_Reset(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_BOOL 	INTEL8254x_Init_HW(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_BOOL 	INTEL8254x_Setup_Link(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_BOOL 	INTEL8254x_Read_PHY_Reg(T_HAL_INTEL8254x_DEV_DATA *pDevData,T_UWORD reg_addr,T_UHWORD *phy_data);
T_MODULE T_BOOL 	INTEL8254x_Write_PHY_Reg(T_HAL_INTEL8254x_DEV_DATA *pDevData,T_UWORD reg_addr,T_UHWORD phy_data);
T_MODULE T_BOOL 	INTEL8254x_PHY_Reset(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_BOOL 	INTEL8254x_PHY_Setup_AutoNeg(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_BOOL 	INTEL8254x_Setup_TX_Resources(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_BOOL 	INTEL8254x_Setup_RX_Resources(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_BOOL 	INTEL8254x_Up(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_BOOL 	INTEL8254x_AddMultiAddr( T_HAL_INTEL8254x_DEV_DATA *vpHalDevData, T_CHAR *bMacAddr, T_VOID *vMultiMacChain);
T_MODULE T_BOOL 	INTEL8254x_DelMultiAddr( T_HAL_INTEL8254x_DEV_DATA *vpHalDevData, T_CHAR *bMacAddr, T_VOID *vMultiMacChain);
T_MODULE T_VOID 	INTEL8254x_Set_Multi(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Configure_TX(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Setup_Rctl(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Configure_RX(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Alloc_RX_Buffers(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Alloc_TX_Buffers(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Free_TX_Resources(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Free_RX_Resources(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Free_RX_Buffers(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Rar_Set(T_HAL_INTEL8254x_DEV_DATA *pDevData,T_UBYTE *addr,T_UWORD index);
T_MODULE T_VOID		INTEL8254x_Write_Reg_IO(T_HAL_INTEL8254x_DEV_DATA *pDevData, T_UWORD offset, T_UWORD value);
T_MODULE T_UWORD	INTEL8254x_Read_Reg_IO(T_HAL_INTEL8254x_DEV_DATA *pDevData, T_UWORD offset);
T_MODULE T_VOID		INTEL8254x_Write_Reg_Array_IO(T_HAL_INTEL8254x_DEV_DATA *pDevData, T_UWORD reg, T_UWORD offset, T_UWORD value);
T_MODULE T_UWORD	INTEL8254x_Read_Reg_Array_IO(T_HAL_INTEL8254x_DEV_DATA *pDevData, T_UWORD reg, T_UWORD offset);
T_MODULE T_VOID 	INTEL8254x_Setup_Eeprom(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Shift_Out_EE_Bits(T_HAL_INTEL8254x_DEV_DATA *pDevData,T_UHWORD data,T_UHWORD count);
T_MODULE T_VOID 	INTEL8254x_Standby_Eeprom(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Raise_EE_Clk(T_HAL_INTEL8254x_DEV_DATA *pDevData,T_UWORD *eecd);
T_MODULE T_VOID 	INTEL8254x_Lower_EE_Clk(T_HAL_INTEL8254x_DEV_DATA *pDevData,T_UWORD *eecd);
T_MODULE T_VOID 	INTEL8254x_Clear_Vfta(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Init_RX_Addrs(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Config_Collision_Dist(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID		INTEL8254x_Force_Mac_FC(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_VOID 	INTEL8254x_Clear_HW_Cntrs(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_WORD 	INTEL8254x_Setup_Copper_Link(T_HAL_INTEL8254x_DEV_DATA *pDevData);
T_MODULE T_UHWORD 	INTEL8254x_Shift_In_EE_Bits(T_HAL_INTEL8254x_DEV_DATA *pDevData);



T_EXTERN T_WORD cacheLsn2ePipeFlush (T_VOID);

/***********************************************************************/
/*         PCI initialization                                          */
/***********************************************************************/

union bios32 {
	struct {
		unsigned long signature;	/* _32_ */
		unsigned long entry;		/* 32 bit physical address */
		unsigned char revision;		/* Revision level, 0 */
		unsigned char length;		/* Length in paragraphs should be 01 */
		unsigned char checksum;		/* All bytes must add up to zero */
		unsigned char reserved[5]; 	/* Must be zero */
	} fields;
	char chars[16];
};

#define PCIBIOS_PCI_FUNCTION_ID 	0xb1XX
#define PCIBIOS_PCI_BIOS_PRESENT 	0xb101
#define PCIBIOS_FIND_PCI_DEVICE		0xb102
#define PCIBIOS_FIND_PCI_CLASS_CODE	0xb103
#define PCIBIOS_GENERATE_SPECIAL_CYCLE	0xb106
#define PCIBIOS_READ_CONFIG_BYTE	0xb108
#define PCIBIOS_READ_CONFIG_WORD	0xb109
#define PCIBIOS_READ_CONFIG_DWORD	0xb10a
#define PCIBIOS_WRITE_CONFIG_BYTE	0xb10b
#define PCIBIOS_WRITE_CONFIG_WORD	0xb10c
#define PCIBIOS_WRITE_CONFIG_DWORD	0xb10d

#define KERNEL_CS	0x18

/* BIOS32 signature: "_32_" */
#define BIOS32_SIGNATURE	(('_' << 0) + ('3' << 8) + ('2' << 16) + ('_' << 24))

/* PCI signature: "PCI " */
#define PCI_SIGNATURE		(('P' << 0) + ('C' << 8) + ('I' << 16) + (' ' << 24))

/* PCI service signature: "$PCI" */
#define PCI_SERVICE		(('$' << 0) + ('P' << 8) + ('C' << 16) + ('I' << 24))


#define CONFIG_CMD(bus, device_fn, where)   (0x80000000 | (bus << 16) | (device_fn << 8) | (where & ~3))

/*
 * This is the standard structure used to identify the entry point
 * to the BIOS32 Service Directory, as documented in
 * 	Standard BIOS 32-bit Service Directory Proposal
 * 	Revision 0.4 May 24, 1993
 *  the PCI BIOS specification.
 */

/*
 * Physical address of the service directory.  I don't know if we're
 * allowed to have more than one of these or not, so just in case
 * we'll make pcibios_present() take a memory start parameter and store
 * the array there.
 */


static unsigned long bios32_entry = 0;
static struct {
	unsigned long address;
	unsigned short segment;
} bios32_indirect = { 0, KERNEL_CS };


#define ERROR -1

  int pciConfigOutByte
    (
    int	busNo,    /* bus number */
    int	deviceNo, /* device number */
    int	funcNo,   /* function number */
    int	offset,   /* offset into the configuration space */
    UINT8 data    /* data written to the offset */
    )
    {
    

	UINT32 addr;
    	UINT32 type;
	int width = 1;//表示读取一字节

	if (busNo == 0)
    {
		/* Type 0 configuration on onboard PCI bus */
		if (deviceNo > 20 || funcNo > 7)
		{
		    return ERROR;		/* device out of range */
		}
		addr = (1 << (deviceNo+11)) | (funcNo << 8) | offset;
		type = 0x00000;
    }
    else
    {
		/* Type 1 configuration on offboard PCI bus */
		if (busNo > 255 || deviceNo > 31 || funcNo > 7)
		{
		    return ERROR;	/* device out of range */
		}
		addr = (busNo << 16) | (deviceNo << 11) | (funcNo << 8) | offset;
		type = 0x10000;
    }

    /* clear aborts */
    BONITO_PCICMD |= PCI_STATUS_MASTER_ABORT | PCI_STATUS_MASTER_TARGET_ABORT;

   BONITO_PCIMAP_CFG = (addr >> 16)|type;


    {
		T_UWORD ori = *(volatile T_UWORD *)PHYS_TO_UNCACHED(BONITO_PCICFG_BASE | (addr & 0xfffc));
		T_UWORD mask = 0x0;

		if (width == 2)
		{
			if (addr & 3)
			{
				mask = 0xffff;
			}
			else
			{
				mask = 0xffff0000;
			}
		}
		else if (width == 1)
		{
			if ((addr & 3) == 1)
			{
				mask = 0xffff00ff;
			}
			else if ((addr & 3) == 2)
			{
				mask = 0xff00ffff;
			}
			else if ((addr & 3) == 3)
			{
				mask = 0x00ffffff;
			}
			else
			{
				mask = 0xffffff00;
			}
		}

		data = data << ((addr & 3) << 3);
		data = (ori & mask) | data;
		*(volatile T_UWORD *)PHYS_TO_UNCACHED(BONITO_PCICFG_BASE | (addr & 0xfffc)) = data;
    }


    if (BONITO_PCICMD & PCI_STATUS_MASTER_ABORT)
    {
		BONITO_PCICMD |= PCI_STATUS_MASTER_ABORT;
		return ERROR;
    }

    if (BONITO_PCICMD & PCI_STATUS_MASTER_TARGET_ABORT)
    {
		BONITO_PCICMD |= PCI_STATUS_MASTER_TARGET_ABORT;
		return ERROR;
    }
    
    return 0;
}


int pcibios_read_config_dword (int busNo,
	int deviceNo, int funcNo,int where, unsigned int *value)
{
	unsigned int addr;
   	 unsigned int type;
   	 unsigned int data;

	if (busNo == 0)
		{
			/* Type 0 configuration on onboard PCI bus */
			if (deviceNo > 20 || funcNo > 7)
			{
				return -1;		/* device out of range */
			}
			addr = (1 << (deviceNo+11)) | (funcNo << 8) | where;
			type = 0x00000;
		}
		else
		{
			/* Type 1 configuration on offboard PCI bus */
			if (busNo > 255 || deviceNo > 31 || funcNo > 7)
			{
				return -1;	/* device out of range */
			}
			addr = (busNo << 16) | (deviceNo << 11) | (funcNo << 8) | where;
			type = 0x10000;
		}

		/* clear aborts */
		BONITO_PCICMD |= PCI_STATUS_MASTER_ABORT | PCI_STATUS_MASTER_TARGET_ABORT;

		BONITO_PCIMAP_CFG = (addr >> 16) | type;

		data = *(volatile T_UWORD *)PHYS_TO_UNCACHED(BONITO_PCICFG_BASE | (addr & 0xfffc));

		/* move data to correct position */
		data = data >> ((addr & 3) << 3);

		if (BONITO_PCICMD & PCI_STATUS_MASTER_ABORT)
		{
			BONITO_PCICMD |= PCI_STATUS_MASTER_ABORT;
			return -1;
		}

		if (BONITO_PCICMD & PCI_STATUS_MASTER_TARGET_ABORT)
		{
			BONITO_PCICMD |= PCI_STATUS_MASTER_TARGET_ABORT;
			return -1;
		}
		*value = data;
		return 0;

}


 int pcibios_read_config_byte(int busNo,
	int deviceNo, int funcNo,int where, unsigned char *value)
{

	unsigned int addr;
   	 unsigned int type;
   	 unsigned int data;

	if (busNo == 0)
		{
			/* Type 0 configuration on onboard PCI bus */
			if (deviceNo > 20 || funcNo > 7)
			{
				return -1;		/* device out of range */
			}
			addr = (1 << (deviceNo+11)) | (funcNo << 8) | where;
			type = 0x00000;
		}
		else
		{
			/* Type 1 configuration on offboard PCI bus */
			if (busNo > 255 || deviceNo > 31 || funcNo > 7)
			{
				return -1;	/* device out of range */
			}
			addr = (busNo << 16) | (deviceNo << 11) | (funcNo << 8) | where;
			type = 0x10000;
		}

		/* clear aborts */
		BONITO_PCICMD |= PCI_STATUS_MASTER_ABORT | PCI_STATUS_MASTER_TARGET_ABORT;

		BONITO_PCIMAP_CFG = (addr >> 16) | type;

		data = *(volatile T_UWORD *)PHYS_TO_UNCACHED(BONITO_PCICFG_BASE | (addr & 0xfffc));

		/* move data to correct position */
		data = data >> ((addr & 3) << 3);

		if (BONITO_PCICMD & PCI_STATUS_MASTER_ABORT)
		{
			BONITO_PCICMD |= PCI_STATUS_MASTER_ABORT;
			return -1;
		}

		if (BONITO_PCICMD & PCI_STATUS_MASTER_TARGET_ABORT)
		{
			BONITO_PCICMD |= PCI_STATUS_MASTER_TARGET_ABORT;
			return -1;
		}
		*value = data;
		return 0;
}

 int pcibios_read_config_word (int busNo,
	int deviceNo, int funcNo,int where, unsigned short *value)
{

	unsigned int  addr;
   	unsigned int  type;
   	unsigned int  data;

	if (busNo == 0)
		{
			/* Type 0 configuration on onboard PCI bus */
			if (deviceNo > 20 || funcNo > 7)
			{
				return -1;		/* device out of range */
			}
			addr = (1 << (deviceNo+11)) | (funcNo << 8) | where;
			type = 0x00000;
		}
		else
		{
			/* Type 1 configuration on offboard PCI bus */
			if (busNo > 255 || deviceNo > 31 || funcNo > 7)
			{
				return -1;	/* device out of range */
			}
			addr = (busNo << 16) | (deviceNo << 11) | (funcNo << 8) | where;
			type = 0x10000;
		}

		/* clear aborts */
		BONITO_PCICMD |= PCI_STATUS_MASTER_ABORT | PCI_STATUS_MASTER_TARGET_ABORT;

		BONITO_PCIMAP_CFG = (addr >> 16) | type;

		data = *(volatile unsigned int *)PHYS_TO_UNCACHED(BONITO_PCICFG_BASE | (addr & 0xfffc));

		/* move data to correct position */
		data = data >> ((addr & 3) << 3);

		if (BONITO_PCICMD & PCI_STATUS_MASTER_ABORT)
		{
			BONITO_PCICMD |= PCI_STATUS_MASTER_ABORT;
			return -1;
		}

		if (BONITO_PCICMD & PCI_STATUS_MASTER_TARGET_ABORT)
		{
			BONITO_PCICMD |= PCI_STATUS_MASTER_TARGET_ABORT;
			return -1;
		}
		*value = data;
		return 0;

}


 int pcibios_find_class (unsigned int class_code, unsigned short index,
	int *pBusNo, int *pDeviceNo,int *pFuncNo)
{

   int status = -1;
   int	 cont	= TRUE;
   int	  busNo;
   int	  deviceNo;
   int	  funcNo;
   unsigned int classCodeReg;
   unsigned int vendor;
   unsigned char  header;


   for (busNo = 0; cont == TRUE && busNo <= PCI_MAX_BUS; busNo++)
	   for (deviceNo = 0; ((cont == TRUE) && (deviceNo < PCI_MAX_DEV)); ++deviceNo)
		   for (funcNo = 0; cont == TRUE && funcNo < PCI_MAX_FUNC; funcNo++)
	   {

	   /* avoid a special bus cycle */
	   //printf("deviceNo :%x, funcNo:%x\n",deviceNo, funcNo);

	   if ((deviceNo == 0x1f) && (funcNo == 0x07))
		   continue;

	   pcibios_read_config_dword (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID, &vendor);


	   /*
		* If nonexistent device, skip to next, only look at
		* vendor ID field for existence check
		*/

	   if (((vendor & 0x0000ffff) == 0x0000FFFF) && (funcNo == 0))
	    {
		   break;
	    }

	   pcibios_read_config_dword (busNo, deviceNo, funcNo, PCI_CFG_REVISION, &classCodeReg);
	   //printf("((classCodeReg >> 8) & 0x00ffffff) : %x\n", ((classCodeReg >> 8) & 0x00ffffff) );

	   if ((((classCodeReg >> 8) & 0x00ffffff) == class_code) && (index-- == 0))
		   {
		   *pBusNo = busNo;
		   *pDeviceNo  = deviceNo;
		   *pFuncNo    = funcNo;
		   status  = 0;
		   cont    = FALSE;    /* terminate all loops */
		   continue;
		   }

	   /* goto next if current device is single function */

	   pcibios_read_config_byte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE, &header);
	   if ((header & PCI_HEADER_MULTI_FUNC) != PCI_HEADER_MULTI_FUNC && funcNo == 0)
		   break;
	   }

   return (status);

}



 int check_pcibios(void)
{
	return 0;
}


int pciConfigOutWord
    (
    int	busNo,    /* bus number */
    int	deviceNo, /* device number */
    int	funcNo,   /* function number */
    int	offset,   /* offset into the configuration space */
    UINT16 data   /* data written to the offset */
    )
    {

		UINT32 addr;
			UINT32 type;
		int width = 2;//表示读取一字节

		if (busNo == 0)
		{
			/* Type 0 configuration on onboard PCI bus */
			if (deviceNo > 20 || funcNo > 7)
			{
				return -1;		/* device out of range */
			}
			addr = (1 << (deviceNo+11)) | (funcNo << 8) | offset;
			type = 0x00000;
		}
		else
		{
			/* Type 1 configuration on offboard PCI bus */
			if (busNo > 255 || deviceNo > 31 || funcNo > 7)
			{
				return -1;	/* device out of range */
			}
			addr = (busNo << 16) | (deviceNo << 11) | (funcNo << 8) | offset;
			type = 0x10000;
		}

		/* clear aborts */
		BONITO_PCICMD |= PCI_STATUS_MASTER_ABORT | PCI_STATUS_MASTER_TARGET_ABORT;

	   BONITO_PCIMAP_CFG = (addr >> 16)|type;


		{
			T_UWORD ori = *(volatile T_UWORD *)PHYS_TO_UNCACHED(BONITO_PCICFG_BASE | (addr & 0xfffc));
			T_UWORD mask = 0x0;

			if (width == 2)
			{
				if (addr & 3)
				{
					mask = 0xffff;
				}
				else
				{
					mask = 0xffff0000;
				}
			}
			else if (width == 1)
			{
				if ((addr & 3) == 1)
				{
					mask = 0xffff00ff;
				}
				else if ((addr & 3) == 2)
				{
					mask = 0xff00ffff;
				}
				else if ((addr & 3) == 3)
				{
					mask = 0x00ffffff;
				}
				else
				{
					mask = 0xffffff00;
				}
			}

			data = data << ((addr & 3) << 3);
			data = (ori & mask) | data;
			*(volatile T_UWORD *)PHYS_TO_UNCACHED(BONITO_PCICFG_BASE | (addr & 0xfffc)) = data;
		}


		if (BONITO_PCICMD & PCI_STATUS_MASTER_ABORT)
		{
			BONITO_PCICMD |= PCI_STATUS_MASTER_ABORT;
			return -1;
		}

		if (BONITO_PCICMD & PCI_STATUS_MASTER_TARGET_ABORT)
		{
			BONITO_PCICMD |= PCI_STATUS_MASTER_TARGET_ABORT;
			return -1;
		}
		return 0;
    }

void pcibios_init()
{
	union bios32 *check;
	unsigned char sum;
	int i, length;
	int faint;

	/*
	 * Follow the standard procedure for locating the BIOS32 Service
	 * directory by scanning the permissible address range from
	 * 0xe0000 through 0xfffff for a valid BIOS32 structure.
	 *
	 */

	for (check = (union bios32 *) 0xe0000;
	     check <= (union bios32 *) 0xffff0;
	     ++check) {
		if (check->fields.signature != BIOS32_SIGNATURE)
			continue;
		length = check->fields.length * 16;
		if (!length)
	 		continue;
		sum = 0;
		for (i = 0; i < length ; ++i)
			sum += check->chars[i];
		if (sum != 0)
			continue;
		if (check->fields.revision != 0) {
					continue;
		}
		if (!bios32_entry) {
			if (check->fields.entry >= 0x100000) {
				faint = 1;
			} else {
				bios32_entry = check->fields.entry;
				bios32_indirect.address = bios32_entry;
			}
		}
	}
check_pcibios();


}

/********************************************************/
/*   Intel8254x   TA			                   		*/
/********************************************************/
T_UWORD gTaNetCardNum = 0;
static T_WORD TA_Intel8254x_cardindex = 0;

T_UDWORD TA_Intel8254x_iobase  = 0;
T_UDWORD TA_Intel8254x_membase = 0;	
T_UHWORD TA_tra_port_vector    = 0;	
T_UHWORD TA_Intel8254x_vendor  = 0x8086;
#if(TA_ETHER_TYPE == TA_INTEL82541)
T_UHWORD TA_Intel82541_device  = 0x1076;
#endif

#if(TA_ETHER_TYPE == TA_INTEL82546)
T_UHWORD TA_Intel82546EB_device  = 0x1010;
T_UHWORD TA_Intel82546GB_device  = 0x1079;

#endif

#if(TA_ETHER_TYPE == TA_INTEL82573)
T_UHWORD TA_Intel82573_device  = 0x109a; // add luxi 09-05-11
#endif

T_HAL_INTEL8254x_DEV_DATA intel8254x_DevData;
T_HAL_INTEL8254x_DEV_DATA *pDevData=&intel8254x_DevData;
T_UWORD  Flag82573 = 0;

struct e1000_rx_desc rcv_desc_ring_buf[RCV_BUFF_NUM] __attribute__((__section__(".netSection"))) __attribute__((__aligned__(128)));
struct e1000_tx_desc tx_desc_ring_buf[TX_BUFF_NUM]  __attribute__((__section__(".netSection"))) __attribute__((__aligned__(128)));

struct e1000_buffer tx_buf[TX_BUFF_NUM]  __attribute__((__section__(".netSection"))) __attribute__((__aligned__(128)));
struct e1000_buffer rx_buf[RCV_BUFF_NUM]  __attribute__((__section__(".netSection"))) __attribute__((__aligned__(128)));

T_VOID CPU_FlushCaches(T_UWORD addr, T_UWORD len);



int pci_find_devs( 
    int bus,
    int recurse,
    int *  pBusNo,	/* bus number */
    int *  pDeviceNo,	/* device number */
    int *  pFuncNo	,/* function number */
    int *  find_card
    )
{
    int    busNo = bus;
    int    deviceNo = 0;
    int    funcNo = 0;
    int   cont = 1;
    unsigned int vendor = 0;
    int device = 0;

    UINT16 hostBridge = (PCI_CLASS_BRIDGE_CTLR<<8)|PCI_SUBCLASS_HOST_PCI_BRIDGE;
    UINT16	pciClass;
    UINT8	secBus;
    UINT32  classCodeReg;
    
    //TA_Intel8254x_cardindex = gTaNetCardNum;

        for (deviceNo = 0; ((cont == 1) && (deviceNo < PCI_MAX_DEV));deviceNo++)
            for (funcNo = 0; cont == 1 && funcNo < PCI_MAX_FUNC; funcNo++)
    {
 
        pcibios_read_config_dword (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID, &vendor);

		if ( ((vendor & 0x0ffff) == 0xffff) ||((vendor & 0x0ffff) == 0x0000) )
		{
			if (funcNo == 0)
			{
				break;
			}
			else
			{
				continue;
			}
		}

         pcibios_read_config_word (busNo, deviceNo, funcNo, PCI_CFG_SUBCLASS, &pciClass);
         
        if ( pciClass != hostBridge )
        {
             
             pcibios_read_config_dword (busNo, deviceNo, funcNo, PCI_CFG_REVISION,&classCodeReg);
             if(((classCodeReg >> 8) & 0x00ffffff) == PCI_NET_ETHERNET_CLASS)
             {
             	 device  = vendor >> 16;
	        	 device &= 0x0000FFFF;
	        	 vendor &= 0x0000FFFF;
	             if((vendor == 0x8086)&&((device == 0x1079) ||(device == 0x1010)))
	             {

	                    if(TA_Intel8254x_cardindex > 0)
	                    {
	                        TA_Intel8254x_cardindex--;
	                        continue;
	                    }

	                    *find_card = 1;
	                    *pBusNo = busNo;
	                    *pDeviceNo = deviceNo;
	                    *pFuncNo = funcNo;
	
	                     return 1;
	
	            }
             }

        }

	    if ( recurse )
		{
			if ( pciClass == ((PCI_CLASS_BRIDGE_CTLR << 8) + PCI_SUBCLASS_P2P_BRIDGE) )
			{
	            pcibios_read_config_byte(busNo, deviceNo, funcNo, PCI_CFG_SECONDARY_BUS, &secBus);
	            if ( secBus > 0 )
				{
					 int ret;
					 ret = pci_find_devs(secBus, recurse,pBusNo,pDeviceNo,pFuncNo,find_card);
					 if(1 == ret)
					 {
					 	return 1;
					 }
				}
			}
		}
		
		if (funcNo == 0)
		{
			char btemp;
			pcibios_read_config_byte (busNo, deviceNo, funcNo,PCI_CFG_HEADER_TYPE, &btemp);
			if ((btemp & PCI_HEADER_MULTI_FUNC) == 0)
			{
				break;
			}
		}
        
    }

    return -1;

}
/********************************************************************
 * 函数名:   fnTA_net_open
 --------------------------------------------------------------------
 * 功能:     完成网络设备的初始化。
 --------------------------------------------------------------------
 * 输入参数: mac ---本地MAC地址，对于那些手动填写MAC地址的网卡来说，
 *        本参数是输入参数，输入用户配置的MAC地址。
 --------------------------------------------------------------------
 * 输出参数: mac ---本地MAC地址，对于那些自带MAC地址的网卡来说，本参
 *        数是输出参数，输出网卡本身的MAC地址。
 --------------------------------------------------------------------
 * 出入参数: 无
 --------------------------------------------------------------------
 * 返回值:   如果成功返回TRUE，否则返回FALSE.
 --------------------------------------------------------------------
 * 作用域:   全局 ********************************************************************/
int  monitorRawNetOpen(unsigned char* mac)
{

	T_UWORD  pci_bus=0, pci_device = 0,pci_func = 0,pci_irq_line=0;
	T_UHWORD device=0;
	T_UWORD find_card = 0;

	TA_Intel8254x_cardindex = gTaNetCardNum;

	CPU_FlushCaches((unsigned int)tx_desc_ring_buf, sizeof(tx_desc_ring_buf));
	CPU_FlushCaches((unsigned int)rcv_desc_ring_buf, sizeof(rcv_desc_ring_buf));
	
	CPU_FlushCaches((unsigned int)tx_buf, sizeof(tx_buf));
	CPU_FlushCaches((unsigned int)rx_buf, sizeof(rx_buf));
	cacheLsn2ePipeFlush();
	pDevData->tx_ring.count = RCV_BUFF_NUM;
	pDevData->rx_ring.count = TX_BUFF_NUM;
	pDevData->MultiCount    = 0;
	pDevData->rxBufSize     = E1000_RXBUFFER;
	pDevData->tx_ring.buffer_info = tx_buf;
	pDevData->rx_ring.buffer_info = rx_buf;
	
	
	/*
	 * PCI initialize
	 */ 
	//pcibios_init();

    pci_find_devs(0,1,&pci_bus, &pci_device, &pci_func, &find_card);


    pcibios_read_config_word(pci_bus, pci_device,pci_func,PCI_DEVICE_ID, &device);


	if(find_card != 1)
	{
#if(TA_ETHER_TYPE == TA_INTEL82541)
		printk("No 82541 Card find, need device id 0x%x\n", TA_Intel82541_device);
#endif
#if(TA_ETHER_TYPE == TA_INTEL82546)
		printk("No 82546 Card find, need device id 0x%x or 0x%x\n", TA_Intel82546EB_device, TA_Intel82546GB_device);
#endif
#if(TA_ETHER_TYPE == TA_INTEL82573)
		printk("No 82573 Card find, need device id 0x%x\n", TA_Intel82573_device);
#endif
		while(1)
			;
	}

    pcibios_read_config_dword(pci_bus, pci_device,pci_func,PCI_BASE_ADDRESS_0, (unsigned int *)&TA_Intel8254x_membase);


#if(TA_ETHER_TYPE == TA_INTEL82546)

	/*通过设备号判断，82546为0x1010 0x1079*/
	if((TA_Intel82546EB_device == device)||(TA_Intel82546GB_device == device))
	{
            if( (TA_Intel8254x_membase & 0x04) == 0x04)
            {
    		    pcibios_read_config_dword(pci_bus, pci_device,pci_func,PCI_BASE_ADDRESS_4, (unsigned int *)&TA_Intel8254x_iobase);
            }
            else if( (TA_Intel8254x_membase & 0x04) == 0)
            {
    		    pcibios_read_config_dword(pci_bus, pci_device,pci_func,PCI_BASE_ADDRESS_2, (unsigned int *)&TA_Intel8254x_iobase);
            }
	}
#endif

	pcibios_read_config_byte(pci_bus, pci_device,pci_func,PCI_INTERRUPT_LINE, (unsigned char*)(&pci_irq_line) );

	TA_tra_port_vector = 0x20 + pci_irq_line ;

	/* Remove I/O space marker in bit 0. */
    	TA_Intel8254x_membase &= PCI_BASE_ADDRESS_MEM_MASK;
	TA_Intel8254x_iobase &= PCI_BASE_ADDRESS_IO_MASK;
	
	pDevData->MemBaseAddr = TA_Intel8254x_membase;
	pDevData->IoBaseAddr  = TA_Intel8254x_iobase;


	pciConfigOutWord(pci_bus, pci_device,pci_func,PCI_COMMAND,0x5);
	
	pciConfigOutByte (pci_bus, pci_device, pci_func, 0x43,0);


	//i386_outport_long(0xCF8, CONFIG_CMD(pci_bus,pci_device_fn,4));
	//i386_outport_word(0xCFC + (4&2), 7);
	/*
	 * Read MAC address
	 */ 
	INTEL8254x_Read_Mac_Addr(pDevData);

	tamemcpy(mac, pDevData->MacAddr,6);

	
	/*
	 * Get resource
	 */	 
	INTEL8254x_Setup_TX_Resources(pDevData);
	INTEL8254x_Alloc_TX_Buffers(pDevData);
	
	INTEL8254x_Setup_RX_Resources(pDevData);
	INTEL8254x_Alloc_RX_Buffers(pDevData);

	
	/*
	 *Reset chip
	 */	 
	INTEL8254x_Reset(pDevData);
	INTEL8254x_Up(pDevData);
	
    return TRUE;

}

/********************************************************************
 * 函数名:   fnTA_net_close
 --------------------------------------------------------------------
 * 功能:     关闭网络设备。
 --------------------------------------------------------------------
 * 输入参数: 无
 --------------------------------------------------------------------
 * 输出参数: 无
 --------------------------------------------------------------------
 * 出入参数: 无
 --------------------------------------------------------------------
 * 返回值:   无
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
void fnTA_net_close(void)
{

}

/********************************************************************
 * 函数名:   fnTA_net_isr
 --------------------------------------------------------------------
 * 功能:     网卡中断服务程序
 --------------------------------------------------------------------
 * 输入参数: 无
 --------------------------------------------------------------------
 * 输出参数: 无
 --------------------------------------------------------------------
 * 出入参数: 无
 --------------------------------------------------------------------
 * 返回值:   无
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
void fnTA_net_isr(void(*callback)(void))
{
    T_UHWORD isr       = 0;
    T_UHWORD ISRstatus = 0;
    
	ISRstatus = E1000_READ_REG(pDevData, ICR);

	if(ISRstatus & E1000_ICR_RXT0)
	{
		isr = 1;
	}


#if 0
    /*判断是否是正确的收到一个帧*/
    if(isr == 1 )
    {
        /*标志数据包来自于设备异常的被动接收*/
        TAgent_net_pkt_come_from_isr = true;

        if(!setjmp(TAgent_net_longjmp_buf))
        {
            callback();
        }
        else
        {
            /*清除状态*/
            TAgent_net_pkt_come_from_isr = false;

        }

    }
#endif

 callback();
    return;

}

/********************************************************************
 * 函数名:   fnTA_net_getpkt
 --------------------------------------------------------------------
 * 功能:     从网络设备读取一个数据包
 --------------------------------------------------------------------
 * 输入参数: buf ---存放数据包的缓冲
 --------------------------------------------------------------------
 * 输出参数: offset ---有些网卡会在以太网帧的前面添加部分状态字，该
 *        状态字不属于以太网帧，并且依赖于不同的网卡，offset就指明了这
 *        部分数据的长度，以通知协议栈丢弃该部分数据。
 --------------------------------------------------------------------
 * 出入参数: 无
 --------------------------------------------------------------------
 * 返回值:   无
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
int  monitorRawNetGetpkt(char* buf, int* offset)
{
	struct e1000_desc_ring *rx_ring = &pDevData->rx_ring;
	struct e1000_rx_desc   *rx_desc = NULL;
	int * tmp = NULL;	
	T_WORD  i       = 0;
	T_UWORD rdt     = 0;
	T_UWORD wpKtLen = 0;

	/*
	*added by yuany, 22th Dec. 2009
	*每次读取数据前，清除中断状态标志，
	*防止硬件设备意外触发中断，
	*这种现象在双口的82546中可能会出现。
	*/
	E1000_READ_REG(pDevData, ICR);
	
	i = rx_ring->next_to_clean;
	
	rx_desc = E1000_RX_DESC(*rx_ring, i);
	
	rx_desc = (struct e1000_rx_desc *)(((unsigned long)rx_desc)|0xa0000000);
	if(rx_desc->status & E1000_RXD_STAT_DD)
	{
		tmp = (int *)rx_desc;
	
		wpKtLen = (T_UWORD)rx_desc->length;

		tamemcpy((void*)(buf+2),(void*)(rx_desc->buffer_addr|0xa0000000),wpKtLen);

       
//		rx_desc->buffer_addr &= 0x00000000ffffffffll;
//		rx_desc->buffer_addr |= 0xa0000000;
		
		rx_desc->status = 0;
		rx_ring->next_to_clean = (i + 1) % rx_ring->count;
		
		rdt = E1000_READ_REG(pDevData, RDT);
		E1000_WRITE_REG(pDevData, RDT, ((rdt + 1) % rx_ring->count));
		cacheLsn2ePipeFlush();	
		*offset = 2;
		return wpKtLen;
	}
	else
	{
		return 0;
	}
}

/********************************************************************
 * 函数名:   fnTA_net_putpkt
 --------------------------------------------------------------------
 * 功能:     向网络设备写一个数据包。
 --------------------------------------------------------------------
 * 输入参数: buf ---数据包
 *             cnt ---数据包的长度
 --------------------------------------------------------------------
 * 输出参数: 无
 --------------------------------------------------------------------
 * 出入参数: 无
 --------------------------------------------------------------------
 * 返回值:   无
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
void monitorRawNetPutpkt(char* buf, int cnt)
{
	struct e1000_desc_ring *tx_ring = &pDevData->tx_ring;
	struct e1000_tx_desc   *tx_desc = NULL;
	T_WORD i = 0;

	i = tx_ring->next_to_use;
	tx_desc = E1000_TX_DESC(*tx_ring, i);
    tx_desc = (struct e1000_rx_desc *)(((unsigned long)tx_desc)|0xa0000000);

	tamemcpy((T_VOID*)(tx_desc->buffer_addr|0xa0000000),(T_VOID*)buf,cnt);

	tx_desc->lower.data =
		(T_UWORD)(E1000_TXD_CMD_IFCS | E1000_TXD_CMD_EOP | E1000_TXD_CMD_RS | cnt);
    
	tx_desc->upper.data = (T_UWORD)0;

   
	i = (i + 1) % tx_ring->count;

	tx_ring->next_to_use = i;
	E1000_WRITE_REG(pDevData, TDT, i);
    cacheLsn2ePipeFlush();   
    
	return;
}


/********************************************************************
 * 函数名:   fnTA_net_enable_rx_int
 --------------------------------------------------------------------
 * 功能:     使能/禁止网卡接收中断
 --------------------------------------------------------------------
 * 输入参数: bool ---是否使能网卡接收中断。
 --------------------------------------------------------------------
 * 输出参数: 无
 --------------------------------------------------------------------
 * 出入参数: 无
 --------------------------------------------------------------------
 * 返回值:   无
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
void fnTA_net_enable_rx_int(int bool)
{
	
    if(bool)
    {
        /*允许接收中断*/
     	E1000_WRITE_REG(pDevData, IMS, E1000_IMS_RXT0);
		E1000_WRITE_FLUSH(pDevData);

    }
    else
    {
        /*禁止接收中断*/
        E1000_WRITE_REG(pDevData, IMC, ~0);
		E1000_WRITE_FLUSH(pDevData);
    }

}

/********************************************************************
 * 函数名:   fnTA_net_enable_tx_int
 --------------------------------------------------------------------
 * 功能:     使能/禁止网卡的发送中断。
 --------------------------------------------------------------------
 * 输入参数: bool ---是否使能网卡中断。
 --------------------------------------------------------------------
 * 输出参数: 无
 --------------------------------------------------------------------
 * 出入参数: 无
 --------------------------------------------------------------------
 * 返回值:   无
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
void fnTA_net_enable_tx_int(int bool)
{	
}

/********************************************************************
 * 函数名:   fnTA_net_get_rx_vector
 --------------------------------------------------------------------
 * 功能:     获得网卡的接收中断号。
 --------------------------------------------------------------------
 * 输入参数: 无
 --------------------------------------------------------------------
 * 输出参数: 无
 --------------------------------------------------------------------
 * 出入参数: 无
 --------------------------------------------------------------------
 * 返回值:   返回网卡的接收中断号。
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
int  fnTA_net_get_rx_vector(void)
{
    return TA_tra_port_vector;
}

/********************************************************************
 * 函数名:   fnTA_net_AddMultiAddr
 --------------------------------------------------------------------
 * 功能:     增加一个组播地址到该网卡
 --------------------------------------------------------------------
 * 输入参数: bMacAddr ----指向一个6字节的存储空间,内容为需要增加的组
 *			  播MAC地址
 *	     vMultiMacChain ----指向一个组播地址链
 --------------------------------------------------------------------
 * 输出参数: 无
 --------------------------------------------------------------------
 * 出入参数: 无
 --------------------------------------------------------------------
 * 返回值:   FALSE 	失败
 *	     TRUE	成功
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
int fnTA_net_AddMultiAddr(T_CHAR *pMacAddr, T_VOID *pMultiMacChain)
{
	//硬件操作
	INTEL8254x_AddMultiAddr(pDevData, pMacAddr, pMultiMacChain);
	
	return TRUE;
}

/********************************************************************
 * 函数名:   fnTA_net_DelMultiAddr
 --------------------------------------------------------------------
 * 功能:     增加一个组播地址到该网卡
 --------------------------------------------------------------------
 * 输入参数: bMacAddr ----指向一个6字节的存储空间,内容为需要删除的组
 *			  播MAC地址
 *	     vMultiMacChain ----指向一个组播地址链
 --------------------------------------------------------------------
 * 输出参数: 无
 --------------------------------------------------------------------
 * 出入参数: 无
 --------------------------------------------------------------------
 * 返回值:   FALSE 	失败
 *	     TRUE	成功
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
int fnTA_net_DelMultiAddr(T_CHAR *pMacAddr, T_VOID *pMultiMacChain)
{
	//硬件操作
	INTEL8254x_DelMultiAddr(pDevData, pMacAddr, pMultiMacChain);
	
	return TRUE;
}

/***************************内部函数的实现****************************/
/**
 * Writes a value to one of the devices registers using port I/O (as opposed to
 * memory mapped I/O).
 *
 * pDevData - Struct containing variables accessed by shared code
 * offset - offset to write to
 * value - value to write
 */
T_MODULE T_VOID
INTEL8254x_Write_Reg_IO(T_HAL_INTEL8254x_DEV_DATA *pDevData, T_UWORD offset, T_UWORD value)
{
    T_UWORD io_addr = pDevData->IoBaseAddr;
    T_UWORD io_data = pDevData->IoBaseAddr + 4;
    
    BSP_OutLong(io_addr, offset);
    BSP_OutLong(io_data, value);
    
}

/**
 * To read a value from one of the devices registers using port I/O (as opposed to
 * memory mapped I/O).
 *
 * pDevData - Struct containing variables accessed by shared code
 * offset - offset to read
 */
T_MODULE T_UWORD
INTEL8254x_Read_Reg_IO(T_HAL_INTEL8254x_DEV_DATA *pDevData, T_UWORD offset)
{
	T_UWORD value;
    T_UWORD io_addr = pDevData->IoBaseAddr;
    T_UWORD io_data = pDevData->IoBaseAddr + 4;
    
    BSP_OutLong(io_addr, offset);
    BSP_InLong((io_data+offset%4), value);
    
    return value;
}

/**
 * Writes a value to one of the devices registers using port I/O (as opposed to
 * memory mapped I/O).
 *
 * pDevData - Struct containing variables accessed by shared code
 * reg - register to write to
 * offset - offset to write to
 * value - value to write to
 */
T_MODULE T_VOID
INTEL8254x_Write_Reg_Array_IO(T_HAL_INTEL8254x_DEV_DATA *pDevData, T_UWORD reg, T_UWORD offset, T_UWORD value)
{
    T_UWORD io_addr = pDevData->IoBaseAddr;
    T_UWORD io_data = pDevData->IoBaseAddr + 4;
    
    BSP_OutLong(io_addr, (reg+offset));
    BSP_OutLong(io_data, value);
}

/**
 * To read a value from one of the devices registers using port I/O (as opposed to
 * memory mapped I/O).
 *
 * pDevData - Struct containing variables accessed by shared code
 * reg - the register to read
 * offset - offset to read
 */
T_MODULE T_UWORD
INTEL8254x_Read_Reg_Array_IO(T_HAL_INTEL8254x_DEV_DATA *pDevData, T_UWORD reg, T_UWORD offset)
{
	T_UWORD value;
    T_UWORD io_addr   = pDevData->IoBaseAddr;
    T_UWORD io_data = pDevData->IoBaseAddr + 4;
    
    BSP_OutLong(io_addr, (reg+offset));
    BSP_InLong((io_data+(reg+offset)%4), value);
    
    return value;
}


/**
 * Reset the transmit and receive units; mask and clear all interrupts.
 *
 * pDevData - Struct containing variables accessed by shared code
 */
T_MODULE T_BOOL
INTEL8254x_Reset_HW(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UWORD ctrl;
    T_UWORD icr;

    /* Clear interrupt mask to stop board from generating interrupts */
    E1000_WRITE_REG(pDevData, IMC, 0xffffffff);

    /* Disable the Transmit and Receive units.  Then delay to allow
     * any pending transactions to complete before we hit the MAC with
     * the global reset.
     */
    E1000_WRITE_REG(pDevData, RCTL, 0);
    E1000_WRITE_REG(pDevData, TCTL, E1000_TCTL_PSP);
    E1000_WRITE_FLUSH(pDevData);

    /* Delay to allow any outstanding PCI transactions to complete before
     * resetting the device
     */ 
    msec_delay(100);

    /* Issue a global reset to the MAC.  This will reset the chip's
     * transmit, receive, DMA, and link units.  It will not effect
     * the current PCI configuration.  The global reset bit is self-
     * clearing, and should clear within a microsecond.
     */
    ctrl = E1000_READ_REG(pDevData, CTRL);

    if (1==Flag82573) {
 		T_UWORD timeout;
    	T_UWORD extcnf_ctrl;
	
        timeout = 10;

        extcnf_ctrl = E1000_READ_REG(pDevData, EXTCNF_CTRL);
        extcnf_ctrl |= E1000_EXTCNF_CTRL_MDIO_SW_OWNERSHIP;

        do {
            E1000_WRITE_REG(pDevData, EXTCNF_CTRL, extcnf_ctrl);
            extcnf_ctrl = E1000_READ_REG(pDevData, EXTCNF_CTRL);

            if (extcnf_ctrl & E1000_EXTCNF_CTRL_MDIO_SW_OWNERSHIP)
                break;
            else
                extcnf_ctrl |= E1000_EXTCNF_CTRL_MDIO_SW_OWNERSHIP;

            msec_delay(2);
            timeout--;
        } while (timeout);
 
 	}

    E1000_WRITE_REG(pDevData, CTRL, (ctrl | E1000_CTRL_RST));
 
	/* Wait for EEPROM reload (it happens automatically) */
    msec_delay(400);

	if (Flag82573==1) {
        T_UWORD eecd = 0;
    	T_UWORD ctrl_ext;

        eecd = E1000_READ_REG(pDevData, EECD);

        /* Isolate bits 15 & 16 */
        eecd = ((eecd >> 15) & 0x03);

        /* If both bits are set, device is Flash type */
        if (eecd == 0x03) {
                usec_delay(10);
                ctrl_ext = E1000_READ_REG(pDevData, CTRL_EXT);
                ctrl_ext |= E1000_CTRL_EXT_EE_RST;
                E1000_WRITE_REG(pDevData, CTRL_EXT, ctrl_ext);
              
            }
 	}
    /* Dissable HW ARPs on ASF enabled adapters */
    /*manc = E1000_READ_REG(pDevData, MANC);
    manc &= ~(E1000_MANC_ARP_EN);
    E1000_WRITE_REG(pDevData, MANC, manc);*/
    
    /* Clear interrupt mask to stop board from generating interrupts */
    E1000_WRITE_REG(pDevData, IMC, 0xffffffff);

    /* Clear any pending interrupt events. */
    icr = E1000_READ_REG(pDevData, ICR);

    return TRUE;
}


/**
 * Reads the adapter's MAC address from the EEPROM and inverts the LSB for the
 * second function of dual function devices
 *
 * pDevData - Struct containing variables accessed by shared code
 */
T_MODULE T_BOOL
INTEL8254x_Read_Mac_Addr(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UHWORD offset;
    T_UHWORD eeprom_data, i;

    if (Flag82573==0) 
	{
    	for(i = 0; i < NODE_ADDRESS_SIZE; i += 2) 
    	{
        	offset = i >> 1;
        	if(!INTEL8254x_Read_Eeprom(pDevData, offset, &eeprom_data)) 
        	{

            	return FALSE;
        	}
		
        	pDevData->MacAddr[i] = (T_UBYTE) (eeprom_data & 0x00FF);
        	pDevData->MacAddr[i+1] = (T_UBYTE) (eeprom_data >> 8);
    	}
		if(E1000_READ_REG(pDevData, STATUS) & E1000_STATUS_FUNC_1) 
    	{
        	if(pDevData->MacAddr[5] & 0x01)
        	{
            	pDevData->MacAddr[5] &= ~(0x01);
			}
        	else
        	{
            	pDevData->MacAddr[5] |= 0x01;
    		}
    	}
	}
	else {
		T_UBYTE adr[6];
		T_UHWORD wordAddr;
		T_WORD *wordp;

		wordp =(int*) (pDevData->MemBaseAddr+ 0x00014);
		*(wordp) = 1;
		wordAddr = (*wordp) >> 16;

		*(wordp) = 1;
		while (!((*wordp) & 0x00000002));
		wordAddr = (*wordp) >> 16;
		adr[0] = (T_UBYTE) (wordAddr & 0xff);
		adr[1] = (T_UBYTE) (wordAddr >> 8);

		*(wordp) = 5;
		while (!((*wordp) & 0x00000002));
		wordAddr = (*wordp) >> 16;
		adr[2] = (T_UBYTE) (wordAddr & 0xff);
		adr[3] = (T_UBYTE) (wordAddr >> 8);

		*(wordp) = 9;
		while (!((*wordp) & 0x00000002));
		wordAddr = (*wordp) >> 16;
		adr[4] = (T_UBYTE) (wordAddr & 0xff);
		adr[5] = (T_UBYTE) (wordAddr >> 8);

		for(i = 0; i < NODE_ADDRESS_SIZE; i ++) 
    	{
 		
        	pDevData->MacAddr[i] = adr[i];
    	}
	}

    
    return TRUE;
}


/**
 * Reads a 16 bit word from the EEPROM.
 *
 * pDevData - Struct containing variables accessed by shared code
 * offset - offset of  word in the EEPROM to read
 * data - word read from the EEPROM 
 */
T_MODULE T_BOOL
INTEL8254x_Read_Eeprom(T_HAL_INTEL8254x_DEV_DATA *pDevData,
                  T_UHWORD offset,
                  T_UHWORD *data)
{
    T_UWORD eecd;
    T_UWORD i = 0;
    T_BOOL large_eeprom = FALSE;

    /* Request EEPROM Access */
    eecd = E1000_READ_REG(pDevData, EECD);
    
    if(eecd & E1000_EECD_SIZE) 
    {
    	large_eeprom = TRUE;
    }
    
    eecd |= E1000_EECD_REQ;
    E1000_WRITE_REG(pDevData, EECD, eecd);
    eecd = E1000_READ_REG(pDevData, EECD);
    
    while((!(eecd & E1000_EECD_GNT)) && (i < 100)) 
    {
        i++;
        usec_delay(5);
        eecd = E1000_READ_REG(pDevData, EECD);
    }
    
    if(!(eecd & E1000_EECD_GNT)) 
    {
        eecd &= ~E1000_EECD_REQ;
        E1000_WRITE_REG(pDevData, EECD, eecd);
        return FALSE;
    }

    /*  Prepare the EEPROM for reading  */
    INTEL8254x_Setup_Eeprom(pDevData);

    /*  Send the READ command (opcode + addr)  */
    INTEL8254x_Shift_Out_EE_Bits(pDevData, EEPROM_READ_OPCODE, 3);
    
    if(large_eeprom) 
    {
        /* If we have a 256 word EEPROM, there are 8 address bits */
        INTEL8254x_Shift_Out_EE_Bits(pDevData, offset, 8);
    } 
    else 
    {
        /* If we have a 64 word EEPROM, there are 6 address bits */
        INTEL8254x_Shift_Out_EE_Bits(pDevData, offset, 6);
    }

    /* Read the data */
    *data = INTEL8254x_Shift_In_EE_Bits(pDevData);

    /* End this read operation */
    INTEL8254x_Standby_Eeprom(pDevData);

    /* Stop requesting EEPROM access */
    eecd = E1000_READ_REG(pDevData, EECD);
    eecd &= ~E1000_EECD_REQ;
    E1000_WRITE_REG(pDevData, EECD, eecd);

    return TRUE;
}


/**
 * Prepares EEPROM for access
 *
 * pDevData - Struct containing variables accessed by shared code
 *
 * Lowers EEPROM clock. Clears input pin. Sets the chip select pin. This 
 * function should be called before issuing a command to the EEPROM.
 */
T_MODULE T_VOID
INTEL8254x_Setup_Eeprom(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UWORD eecd;

    eecd = E1000_READ_REG(pDevData, EECD);

    /* Clear SK and DI */
    eecd &= ~(E1000_EECD_SK | E1000_EECD_DI);
    E1000_WRITE_REG(pDevData, EECD, eecd);

    /* Set CS */
    eecd |= E1000_EECD_CS;
    E1000_WRITE_REG(pDevData, EECD, eecd);
}


/**
 * Shift data bits out to the EEPROM.
 *
 * pDevData - Struct containing variables accessed by shared code
 * data - data to send to the EEPROM
 * count - number of bits to shift out
 */
T_MODULE T_VOID
INTEL8254x_Shift_Out_EE_Bits(T_HAL_INTEL8254x_DEV_DATA *pDevData,
                        T_UHWORD data,
                        T_UHWORD count)
{
    T_UWORD eecd;
    T_UWORD mask;

    /* We need to shift "count" bits out to the EEPROM. So, value in the
     * "data" parameter will be shifted out to the EEPROM one bit at a time.
     * In order to do this, "data" must be broken down into bits. 
     */
    mask = 0x01 << (count - 1);
    eecd = E1000_READ_REG(pDevData, EECD);
    eecd &= ~(E1000_EECD_DO | E1000_EECD_DI);
    
    do 
    {
        /* A "1" is shifted out to the EEPROM by setting bit "DI" to a "1",
         * and then raising and then lowering the clock (the SK bit controls
         * the clock input to the EEPROM).  A "0" is shifted out to the EEPROM
         * by setting "DI" to "0" and then raising and then lowering the clock.
         */
        eecd &= ~E1000_EECD_DI;

        if(data & mask)
        {
            eecd |= E1000_EECD_DI;
		}
		
        E1000_WRITE_REG(pDevData, EECD, eecd);
        E1000_WRITE_FLUSH(pDevData);

        usec_delay(50);

        INTEL8254x_Raise_EE_Clk(pDevData, &eecd);
        INTEL8254x_Lower_EE_Clk(pDevData, &eecd);

        mask = mask >> 1;

    } while(mask);

    /* We leave the "DI" bit set to "0" when we leave this routine. */
    eecd &= ~E1000_EECD_DI;
    E1000_WRITE_REG(pDevData, EECD, eecd);
}

/**
 * Shift data bits in from the EEPROM
 *
 * pDevData - Struct containing variables accessed by shared code
 */
T_MODULE T_UHWORD
INTEL8254x_Shift_In_EE_Bits(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UWORD  eecd;
    T_UWORD  i;
    T_UHWORD data;

    /* In order to read a register from the EEPROM, we need to shift 16 bits 
     * in from the EEPROM. Bits are "shifted in" by raising the clock input to
     * the EEPROM (setting the SK bit), and then reading the value of the "DO"
     * bit.  During this "shifting in" process the "DI" bit should always be 
     * clear..
     */

    eecd = E1000_READ_REG(pDevData, EECD);
    eecd &= ~(E1000_EECD_DO | E1000_EECD_DI);
    
    data = 0;
    for(i = 0; i < 16; i++) 
    {
        data = data << 1;
        INTEL8254x_Raise_EE_Clk(pDevData, &eecd);

        eecd = E1000_READ_REG(pDevData, EECD);

        eecd &= ~(E1000_EECD_DI);
        if(eecd & E1000_EECD_DO)
        {
            data |= 1;
		}
		
        INTEL8254x_Lower_EE_Clk(pDevData, &eecd);
    }

    return data;
}

/**
 * Returns EEPROM to a "standby" state
 * 
 * pDevData - Struct containing variables accessed by shared code
 */
T_MODULE T_VOID
INTEL8254x_Standby_Eeprom(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UWORD eecd;

    eecd = E1000_READ_REG(pDevData, EECD);

    /* Deselct EEPROM */
    eecd &= ~(E1000_EECD_CS | E1000_EECD_SK);
    E1000_WRITE_REG(pDevData, EECD, eecd);
    E1000_WRITE_FLUSH(pDevData);
    usec_delay(50);

    /* Clock high */
    eecd |= E1000_EECD_SK;
    E1000_WRITE_REG(pDevData, EECD, eecd);
    E1000_WRITE_FLUSH(pDevData);
    usec_delay(50);

    /* Select EEPROM */
    eecd |= E1000_EECD_CS;
    E1000_WRITE_REG(pDevData, EECD, eecd);
    E1000_WRITE_FLUSH(pDevData);
    usec_delay(50);

    /* Clock low */
    eecd &= ~E1000_EECD_SK;
    E1000_WRITE_REG(pDevData, EECD, eecd);
    E1000_WRITE_FLUSH(pDevData);
    usec_delay(50);
}

/**
 * Raises the EEPROM's clock input.
 *
 * pDevData - Struct containing variables accessed by shared code
 * eecd - EECD's current value
 */
T_MODULE T_VOID
INTEL8254x_Raise_EE_Clk(T_HAL_INTEL8254x_DEV_DATA *pDevData,
                   T_UWORD *eecd)
{
    /* Raise the clock input to the EEPROM (by setting the SK bit), and then
     * wait 50 microseconds.
     */
    *eecd = *eecd | E1000_EECD_SK;
    E1000_WRITE_REG(pDevData, EECD, *eecd);
    E1000_WRITE_FLUSH(pDevData);
    usec_delay(50);
}

/**
 * Lowers the EEPROM's clock input.
 *
 * pDevData - Struct containing variables accessed by shared code 
 * eecd - EECD's current value
 */
T_MODULE T_VOID
INTEL8254x_Lower_EE_Clk(T_HAL_INTEL8254x_DEV_DATA *pDevData,
                   T_UWORD *eecd)
{
    /* Lower the clock input to the EEPROM (by clearing the SK bit), and then 
     * wait 50 microseconds. 
     */
    *eecd = *eecd & ~E1000_EECD_SK;
    E1000_WRITE_REG(pDevData, EECD, *eecd);
    E1000_WRITE_FLUSH(pDevData);
    usec_delay(50);
}

T_MODULE T_BOOL
INTEL8254x_Reset(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
	/* Repartition Pba for greater than 9k mtu
	 * To take effect CTRL.RST is required.
	 */

    if (1==Flag82573) {
		if(pDevData->rxBufSize > E1000_RXBUFFER_8192)
		{
			E1000_WRITE_REG(pDevData, PBA, 0x000c);
		}
		else
		{
			E1000_WRITE_REG(pDevData, PBA, 0x0014);
		}
    }
	else {
		if(pDevData->rxBufSize > E1000_RXBUFFER_8192)
		{
			E1000_WRITE_REG(pDevData, PBA, E1000_JUMBO_PBA);
		}
		else
		{
			E1000_WRITE_REG(pDevData, PBA, E1000_DEFAULT_PBA);
		}
	}
	
	if(!INTEL8254x_Reset_HW(pDevData))
	{
		return FALSE;
	}
	
	E1000_WRITE_REG(pDevData, WUC, 0);
	
	if(!INTEL8254x_Init_HW(pDevData))
	{
		return FALSE;
	}

	if (Flag82573==1) {
	  /* Enable h/w to recognize an 802.1Q VLAN Ethernet packet */
	  E1000_WRITE_REG(pDevData, VET, 0x8100/*ETHERNET_IEEE_VLAN_TYPE*/);
	}
    cacheLsn2ePipeFlush();
	return TRUE;
}

/**
 * Performs basic configuration of the adapter.
 *
 * pDevData - Struct containing variables accessed by shared code
 * 
 * Assumes that the controller has previously been reset and is in a 
 * post-reset uninitialized state. Initializes the receive address registers,
 * multicast table, and VLAN filter table. Calls routines to setup link
 * configuration and flow control settings. Clears all on-chip counters. Leaves
 * the transmit and receive units disabled and uninitialized.
 */
T_MODULE T_BOOL
INTEL8254x_Init_HW(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UWORD ctrl;
    T_UWORD i;

  if (Flag82573==0) {
    /* Disabling VLAN filtering. */
    E1000_WRITE_REG(pDevData, VET, 0);

    INTEL8254x_Clear_Vfta(pDevData);
  }
  else {
        /* Settings common to all PCI-express silicon */
        T_UWORD reg_ctrl, reg_ctrl_ext;
        T_UWORD reg_tarc0;
        
        T_UWORD reg_txdctl, reg_txdctl1;

        reg_tarc0 = E1000_READ_REG(pDevData, TARC0);
        /* link autonegotiation/sync workarounds */
        reg_tarc0 &= ~((1 << 30)|(1 << 29)|(1 << 28)|(1 << 27));

        reg_txdctl = E1000_READ_REG(pDevData, TXDCTL);
        /* Enable not-done TX descriptor counting */
        reg_txdctl |= E1000_TXDCTL_COUNT_DESC;
        E1000_WRITE_REG(pDevData, TXDCTL, reg_txdctl);

        reg_txdctl1 = E1000_READ_REG(pDevData, TXDCTL1);
        reg_txdctl1 |= E1000_TXDCTL_COUNT_DESC;
        E1000_WRITE_REG(pDevData, TXDCTL1, reg_txdctl1);

                reg_ctrl_ext = E1000_READ_REG(pDevData, CTRL_EXT);
                reg_ctrl = E1000_READ_REG(pDevData, CTRL);

                reg_ctrl_ext &= ~(1 << 23);
                reg_ctrl_ext |= (1 << 22);
                /* TX byte count fix */
                reg_ctrl &= ~(1 << 29);

                E1000_WRITE_REG(pDevData, CTRL_EXT, reg_ctrl_ext);
                E1000_WRITE_REG(pDevData, CTRL, reg_ctrl);

        E1000_WRITE_REG(pDevData, TARC0, reg_tarc0);

  }
    /* Setup the receive address. This involves initializing all of the Receive
     * Address Registers (RARs 0 - 15).
     */
    INTEL8254x_Init_RX_Addrs(pDevData);

    /* Zero out the Multicast HASH table */
    for(i = 0; i < E1000_MC_TBL_SIZE; i++)
    {
        E1000_WRITE_REG_ARRAY(pDevData, MTA, i, 0);
	}
	
    /* Call a subroutine to configure the link and setup flow control. */
    if(!INTEL8254x_Setup_Link(pDevData))
    {
		return FALSE;
	}
	
    /* Set the transmit descriptor write-back policy */
    ctrl = E1000_READ_REG(pDevData, TXDCTL);
    ctrl = (ctrl & ~E1000_TXDCTL_WTHRESH) | E1000_TXDCTL_FULL_TX_DESC_WB;
    E1000_WRITE_REG(pDevData, TXDCTL, ctrl);

    /* Clear all of the statistics registers (clear on read).  It is
     * important that we do this after we have tried to establish link
     * because the symbol error count will increment wildly if there
     * is no link.
     */
    if (1 == Flag82573) {
        T_UWORD gcr = E1000_READ_REG(pDevData, GCR);
        gcr |= E1000_GCR_L1_ACT_WITHOUT_L0S_RX;
        E1000_WRITE_REG(pDevData, GCR, gcr);
    }



	INTEL8254x_Clear_HW_Cntrs(pDevData);
    

    return TRUE;
}

/**
 * Clears the VLAN filer table
 *
 * pDevData - Struct containing variables accessed by shared code
 */
T_MODULE T_VOID
INTEL8254x_Clear_Vfta(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UWORD offset;

    for(offset = 0; offset < E1000_VLAN_FILTER_TBL_SIZE; offset++)
    {
        E1000_WRITE_REG_ARRAY(pDevData, VFTA, offset, 0);
	}
}

/**
 * Initializes receive address filters.
 *
 * pDevData - Struct containing variables accessed by shared code 
 *
 * Places the MAC address in receive address register 0 and clears the rest
 * of the receive addresss registers. Clears the multicast table. Assumes
 * the receiver is in reset when the routine is called.
 */
T_MODULE T_VOID
INTEL8254x_Init_RX_Addrs(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UWORD i;
    T_UWORD addr_low;
    T_UWORD addr_high;

    /* Setup the receive address. */
    addr_low = (pDevData->MacAddr[0] |
                (pDevData->MacAddr[1] << 8) |
                (pDevData->MacAddr[2] << 16) | (pDevData->MacAddr[3] << 24));

    addr_high = (pDevData->MacAddr[4] |
                 (pDevData->MacAddr[5] << 8) | E1000_RAH_AV);

    E1000_WRITE_REG_ARRAY(pDevData, RA, 0, addr_low);
    E1000_WRITE_REG_ARRAY(pDevData, RA, 1, addr_high);

    /* Zero out the other 15 receive addresses. */
    for(i = 1; i < E1000_RAR_ENTRIES; i++) 
    {
        E1000_WRITE_REG_ARRAY(pDevData, RA, (i << 1), 0);
        E1000_WRITE_REG_ARRAY(pDevData, RA, ((i << 1) + 1), 0);
    }
}

/**
 * Configures flow control and link settings.
 * 
 * pDevData - Struct containing variables accessed by shared code
 * 
 * Determines which flow control settings to use. Calls the apropriate media-
 * specific link configuration function. Configures the flow control settings.
 * Assuming the adapter has a valid link partner, a valid link should be
 * established. Assumes the hardware has previously been reset and the 
 * transmitter and receiver are not enabled.
 */
T_MODULE T_BOOL
INTEL8254x_Setup_Link(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    /* Call the necessary subroutine to configure the link. */
    INTEL8254x_Setup_Copper_Link(pDevData);

    /* Initialize the flow control address, type, and PAUSE timer
     * registers to their default values.  This is done even if flow
     * control is disabled, because it does not hurt anything to
     * initialize these registers.
     */

    E1000_WRITE_REG(pDevData, FCAL, 0);//FLOW_CONTROL_ADDRESS_LOW);
    E1000_WRITE_REG(pDevData, FCAH, 0);//FLOW_CONTROL_ADDRESS_HIGH);
    E1000_WRITE_REG(pDevData, FCT, 0);//FLOW_CONTROL_TYPE);
    E1000_WRITE_REG(pDevData, FCTTV, 0);//FC_DEFAULT_TX_TIMER);

    /* Set the flow control receive threshold registers.  Normally,
     * these registers will be set to a default threshold that may be
     * adjusted later by the driver's runtime code.  However, if the
     * ability to transmit pause frames in not enabled, then these
     * registers will be set to 0. 
     */
    E1000_WRITE_REG(pDevData, FCRTL, 0);
    E1000_WRITE_REG(pDevData, FCRTH, 0);
    
    return TRUE;
}

/**
* Detects which PHY is present and the speed and duplex
*
* pDevData - Struct containing variables accessed by shared code
*/
T_MODULE T_WORD 
INTEL8254x_Setup_Copper_Link(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UWORD  ctrl;
    T_UHWORD i;
    T_UHWORD phy_data;

    ctrl = E1000_READ_REG(pDevData, CTRL);
    
    /* With 82543, we need to force speed and duplex on the MAC equal to what
     * the PHY speed and duplex configuration is. In addition, we need to
     * perform a hardware reset on the PHY to take it out of reset.
     */
    ctrl |= E1000_CTRL_SLU;
    ctrl &= ~(E1000_CTRL_FRCSPD | E1000_CTRL_FRCDPX);
    E1000_WRITE_REG(pDevData, CTRL, ctrl);

    /* Enable CRS on TX. This must be set for half-duplex operation. */
    if(!INTEL8254x_Read_PHY_Reg(pDevData, M88E1000_PHY_SPEC_CTRL, &phy_data))
    { 
        return FALSE;
	}
    
    phy_data |= M88E1000_PSCR_ASSERT_CRS_ON_TX;
    phy_data |= M88E1000_PSCR_AUTO_X_MODE;
    phy_data &= ~M88E1000_PSCR_POLARITY_REVERSAL;
    
    if(!INTEL8254x_Write_PHY_Reg(pDevData, M88E1000_PHY_SPEC_CTRL, phy_data)) 
    {
        return FALSE;
	}
	
    /* Force TX_CLK in the Extended PHY Specific Control Register
     * to 25MHz clock.
     */
    if(!INTEL8254x_Read_PHY_Reg(pDevData, M88E1000_EXT_PHY_SPEC_CTRL, &phy_data)) 
    {
        return FALSE;
    }
    
    phy_data |= M88E1000_EPSCR_TX_CLK_25;
    
    /* Configure Master and Slave downshift values */
    phy_data &= ~(M88E1000_EPSCR_MASTER_DOWNSHIFT_MASK |
                  M88E1000_EPSCR_SLAVE_DOWNSHIFT_MASK);
    phy_data |= (M88E1000_EPSCR_MASTER_DOWNSHIFT_1X |
                 M88E1000_EPSCR_SLAVE_DOWNSHIFT_1X);
    
    if(!INTEL8254x_Write_PHY_Reg(pDevData, M88E1000_EXT_PHY_SPEC_CTRL, phy_data)) 
	{
		return FALSE;
	}
	
    /* SW Reset the PHY so all changes take effect */
    if(!INTEL8254x_PHY_Reset(pDevData))
    {
        return FALSE;
    }
    
    if(!INTEL8254x_PHY_Setup_AutoNeg(pDevData))
    {
        return FALSE;
    }
        
    /* Restart auto-negotiation by setting the Auto Neg Enable bit and
     * the Auto Neg Restart bit in the PHY control register.
     */
    if(!INTEL8254x_Read_PHY_Reg(pDevData, PHY_CTRL, &phy_data)) 
    {
        return FALSE;
    }
    phy_data |= (MII_CR_AUTO_NEG_EN | MII_CR_RESTART_AUTO_NEG);
    if(!INTEL8254x_Write_PHY_Reg(pDevData, PHY_CTRL, phy_data)) 
    {
        return FALSE;
    }

    /* Check link status. Wait up to 100 microseconds for link to become
     * valid.
     */
    for(i = 0; i < 10; i++) 
    {
        if(!INTEL8254x_Read_PHY_Reg(pDevData, PHY_STATUS, &phy_data)) 
        {
            return FALSE;
        }
        
        if(!INTEL8254x_Read_PHY_Reg(pDevData, PHY_STATUS, &phy_data)) 
        {
            return FALSE;
        }
        
        if(phy_data & MII_SR_LINK_STATUS) 
        {
            /* We have link, so we need to finish the config process:
             *   1) Set up the MAC to the current PHY speed/duplex
             *      if we are on 82543.  If we
             *      are on newer silicon, we only need to configure
             *      collision distance in the Transmit Control Register.
             *   2) Set up flow control on the MAC to that established with
             *      the link partner.
             */
            INTEL8254x_Config_Collision_Dist(pDevData);
            INTEL8254x_Force_Mac_FC(pDevData); 
        
        	return TRUE;
        }  
        usec_delay(1000);
    }

    return TRUE;
}

/**
* Reads the value from a PHY register
*
* pDevData - Struct containing variables accessed by shared code
* reg_addr - address of the PHY register to read
*/
T_MODULE T_BOOL
INTEL8254x_Read_PHY_Reg(T_HAL_INTEL8254x_DEV_DATA *pDevData,
                   T_UWORD reg_addr,
                   T_UHWORD *phy_data)
{
    T_UWORD         i;
    T_UWORD         mdic = 0;
    T_CONST T_UWORD phy_addr = 1;

    if(reg_addr > MAX_PHY_REG_ADDRESS) 
    {
        return FALSE;
	}
	
    /* Set up Op-code, Phy Address, and register address in the MDI
     * Control register.  The MAC will take care of interfacing with the
     * PHY to retrieve the desired data.
     */
    mdic = ((reg_addr << E1000_MDIC_REG_SHIFT) |
            (phy_addr << E1000_MDIC_PHY_SHIFT) | 
            (E1000_MDIC_OP_READ));

    E1000_WRITE_REG(pDevData, MDIC, mdic);

    /* Poll the ready bit to see if the MDI read completed */
    for(i = 0; i < 64; i++) 
    {
        usec_delay(10);
        mdic = E1000_READ_REG(pDevData, MDIC);
        if(mdic & E1000_MDIC_READY) break;
    }
    
    if(!(mdic & E1000_MDIC_READY)) 
    {
        return FALSE;
    }
    
    if(mdic & E1000_MDIC_ERROR) 
    {
        return FALSE;
    }
    
    *phy_data = (T_UHWORD) mdic;
    return TRUE;
}

/**
* Writes a value to a PHY register
*
* pDevData - Struct containing variables accessed by shared code
* reg_addr - address of the PHY register to write
* data - data to write to the PHY
*/
T_MODULE T_BOOL
INTEL8254x_Write_PHY_Reg(T_HAL_INTEL8254x_DEV_DATA *pDevData,
                    T_UWORD reg_addr,
                    T_UHWORD phy_data)
{
    T_UWORD         i;
    T_UWORD         mdic = 0;
    T_CONST T_UWORD phy_addr = 1;

    if(reg_addr > MAX_PHY_REG_ADDRESS) 
    {
        return FALSE;
    }

    /* Set up Op-code, Phy Address, register address, and data intended
     * for the PHY register in the MDI Control register.  The MAC will take
     * care of interfacing with the PHY to send the desired data.
     */
    mdic = (((T_UWORD) phy_data) |
            (reg_addr << E1000_MDIC_REG_SHIFT) |
            (phy_addr << E1000_MDIC_PHY_SHIFT) | 
            (E1000_MDIC_OP_WRITE));

    E1000_WRITE_REG(pDevData, MDIC, mdic);

    /* Poll the ready bit to see if the MDI read completed */
    for(i = 0; i < 64; i++) 
    {
        usec_delay(10);
        mdic = E1000_READ_REG(pDevData, MDIC);
        if(mdic & E1000_MDIC_READY) break;
    }
    
    if(!(mdic & E1000_MDIC_READY)) 
    {
        return FALSE;
    }
    
    return TRUE;
}

/**
* Resets the PHY
*
* pDevData - Struct containing variables accessed by shared code
*
* Sets bit 15 of the MII Control regiser
*/
T_MODULE T_BOOL
INTEL8254x_PHY_Reset(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UHWORD phy_data;

    if(!INTEL8254x_Read_PHY_Reg(pDevData, PHY_CTRL, &phy_data) ) 
    {
        return FALSE;
    }
    
    phy_data |= MII_CR_RESET;
    if(!INTEL8254x_Write_PHY_Reg(pDevData, PHY_CTRL, phy_data)) 
    {
        return FALSE;
    }
    usec_delay(100);
    
    return TRUE;
}

/**
* Configures PHY autoneg and flow control advertisement settings
*
* pDevData - Struct containing variables accessed by shared code
*/
T_MODULE T_BOOL
INTEL8254x_PHY_Setup_AutoNeg(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UHWORD mii_autoneg_adv_reg;
    T_UHWORD mii_1000t_ctrl_reg;

    /* Read the MII Auto-Neg Advertisement Register (Address 4). */
    if(!INTEL8254x_Read_PHY_Reg(pDevData, PHY_AUTONEG_ADV, &mii_autoneg_adv_reg)) 
    {
        return FALSE;
    }

    /* Read the MII 1000Base-T Control Register (Address 9). */
    if(!INTEL8254x_Read_PHY_Reg(pDevData, PHY_1000T_CTRL, &mii_1000t_ctrl_reg)) 
    {
        return FALSE;
    }

    /* First we clear all the 10/100 mb speed bits in the Auto-Neg
     * Advertisement Register (Address 4) and the 1000 mb speed bits in
     * the  1000Base-T Control Register (Address 9).
     */
    mii_autoneg_adv_reg &= ~REG4_SPEED_MASK;
    mii_1000t_ctrl_reg &= ~REG9_SPEED_MASK;

    /* Do we want to advertise 10 Mb Half Duplex? */
        mii_autoneg_adv_reg |= NWAY_AR_10T_HD_CAPS;

    /* Do we want to advertise 10 Mb Full Duplex? */
        mii_autoneg_adv_reg |= NWAY_AR_10T_FD_CAPS;

    /* Do we want to advertise 100 Mb Half Duplex? */
        mii_autoneg_adv_reg |= NWAY_AR_100TX_HD_CAPS;

    /* Do we want to advertise 100 Mb Full Duplex? */
        mii_autoneg_adv_reg |= NWAY_AR_100TX_FD_CAPS;

    /* Do we want to advertise 1000 Mb Full Duplex? */
        mii_1000t_ctrl_reg |= CR_1000T_FD_CAPS;
     
    /* Flow control (RX & TX) is completely disabled by a
     * software over-ride.
     */
    mii_autoneg_adv_reg &= ~(NWAY_AR_ASM_DIR | NWAY_AR_PAUSE);

    if(!INTEL8254x_Write_PHY_Reg(pDevData, PHY_AUTONEG_ADV, mii_autoneg_adv_reg)) 
    {
        return FALSE;
    }

    if(!INTEL8254x_Write_PHY_Reg(pDevData, PHY_1000T_CTRL, mii_1000t_ctrl_reg)) 
    {
        return FALSE;
    }
    
    return TRUE;
}

/**
* Sets the collision distance in the Transmit Control register
*
* pDevData - Struct containing variables accessed by shared code
*
* Link should have been established previously. Reads the speed and duplex
* information from the Device Status register.
*/
T_MODULE T_VOID
INTEL8254x_Config_Collision_Dist(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UWORD tctl;

    tctl = E1000_READ_REG(pDevData, TCTL);

    tctl &= ~E1000_TCTL_COLD;
    tctl |= E1000_COLLISION_DISTANCE << E1000_COLD_SHIFT;

    E1000_WRITE_REG(pDevData, TCTL, tctl);
    E1000_WRITE_FLUSH(pDevData);
}

/**
 * Forces the MAC's flow control settings.
 * 
 * pDevData - Struct containing variables accessed by shared code
 *
 * Sets the TFCE and RFCE bits in the device control register to reflect
 * the adapter settings. TFCE and RFCE need to be explicitly set by
 * software when a Copper PHY is used because autonegotiation is managed
 * by the PHY rather than the MAC. Software must also configure these
 * bits when link is forced on a fiber connection.
 */
T_MODULE T_VOID
INTEL8254x_Force_Mac_FC(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_UWORD ctrl;

    /* Get the current configuration of the Device Control Register */
    ctrl = E1000_READ_REG(pDevData, CTRL);

   /* Flow control is completely disabled */
    ctrl &= (~(E1000_CTRL_TFCE | E1000_CTRL_RFCE));
    E1000_WRITE_REG(pDevData, CTRL, ctrl);
}

/**
 * Clears all hardware statistics counters. 
 *
 * pDevData - Struct containing variables accessed by shared code
 */
T_MODULE T_VOID
INTEL8254x_Clear_HW_Cntrs(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
    T_VUWORD temp;

    temp = E1000_READ_REG(pDevData, CRCERRS);
    temp = E1000_READ_REG(pDevData, SYMERRS);
    temp = E1000_READ_REG(pDevData, MPC);
    temp = E1000_READ_REG(pDevData, SCC);
    temp = E1000_READ_REG(pDevData, ECOL);
    temp = E1000_READ_REG(pDevData, MCC);
    temp = E1000_READ_REG(pDevData, LATECOL);
    temp = E1000_READ_REG(pDevData, COLC);
    temp = E1000_READ_REG(pDevData, DC);
    temp = E1000_READ_REG(pDevData, SEC);
    temp = E1000_READ_REG(pDevData, RLEC);
    temp = E1000_READ_REG(pDevData, XONRXC);
    temp = E1000_READ_REG(pDevData, XONTXC);
    temp = E1000_READ_REG(pDevData, XOFFRXC);
    temp = E1000_READ_REG(pDevData, XOFFTXC);
    temp = E1000_READ_REG(pDevData, FCRUC);
    temp = E1000_READ_REG(pDevData, PRC64);
    temp = E1000_READ_REG(pDevData, PRC127);
    temp = E1000_READ_REG(pDevData, PRC255);
    temp = E1000_READ_REG(pDevData, PRC511);
    temp = E1000_READ_REG(pDevData, PRC1023);
    temp = E1000_READ_REG(pDevData, PRC1522);
    temp = E1000_READ_REG(pDevData, GPRC);
    temp = E1000_READ_REG(pDevData, BPRC);
    temp = E1000_READ_REG(pDevData, MPRC);
    temp = E1000_READ_REG(pDevData, GPTC);
    temp = E1000_READ_REG(pDevData, GORCL);
    temp = E1000_READ_REG(pDevData, GORCH);
    temp = E1000_READ_REG(pDevData, GOTCL);
    temp = E1000_READ_REG(pDevData, GOTCH);
    temp = E1000_READ_REG(pDevData, RNBC);
    temp = E1000_READ_REG(pDevData, RUC);
    temp = E1000_READ_REG(pDevData, RFC);
    temp = E1000_READ_REG(pDevData, ROC);
    temp = E1000_READ_REG(pDevData, RJC);
    temp = E1000_READ_REG(pDevData, TORL);
    temp = E1000_READ_REG(pDevData, TORH);
    temp = E1000_READ_REG(pDevData, TOTL);
    temp = E1000_READ_REG(pDevData, TOTH);
    temp = E1000_READ_REG(pDevData, TPR);
    temp = E1000_READ_REG(pDevData, TPT);
    temp = E1000_READ_REG(pDevData, PTC64);
    temp = E1000_READ_REG(pDevData, PTC127);
    temp = E1000_READ_REG(pDevData, PTC255);
    temp = E1000_READ_REG(pDevData, PTC511);
    temp = E1000_READ_REG(pDevData, PTC1023);
    temp = E1000_READ_REG(pDevData, PTC1522);
    temp = E1000_READ_REG(pDevData, MPTC);
    temp = E1000_READ_REG(pDevData, BPTC);
    temp = E1000_READ_REG(pDevData, ALGNERRC);
    temp = E1000_READ_REG(pDevData, RXERRC);
    temp = E1000_READ_REG(pDevData, TNCRS);
    temp = E1000_READ_REG(pDevData, CEXTERR);
    temp = E1000_READ_REG(pDevData, TSCTC);
    temp = E1000_READ_REG(pDevData, TSCTFC);
    temp = E1000_READ_REG(pDevData, MGTPRC);
    temp = E1000_READ_REG(pDevData, MGTPDC);
    temp = E1000_READ_REG(pDevData, MGTPTC);
}

/**
 * INTEL8254x_Setup_TX_Resources - allocate Tx resources (Descriptors)
 * @pDevData: board private structure
 *
 * Return TRUE on success, FALSE on failure
 */
T_MODULE T_BOOL
INTEL8254x_Setup_TX_Resources(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
	struct e1000_desc_ring *txdr = &pDevData->tx_ring;
	/*struct e1000_tx_desc   tx_buffer[1039];*/
	
	txdr->size = txdr->count * sizeof(struct e1000_tx_desc);
	//txdr->descTemp =((T_UWORD)tx_desc_ring_buf) | 0xa0000000;  
	txdr->descTemp = tx_desc_ring_buf;

	if(txdr->descTemp == 0)
	{
		return FALSE;
	}
	txdr->desc = (T_VOID *)(txdr->descTemp );

	if(!txdr->desc) 
	{
		return FALSE;
	}
	//tamemset(txdr->desc, 0, txdr->size);
	tamemset((T_VOID*)((T_UWORD)(txdr->desc) | 0xa000000), 0, txdr->size);

	txdr->next_to_use = 0;
	txdr->next_to_clean = 0;

    cacheLsn2ePipeFlush();
	return TRUE;
}

/**
 * INTEL8254x_Setup_RX_Resources - allocate Rx resources (Descriptors)
 * @pDevData: board private structure
 *
 * Returns TRUE on success, FALSE on failure
 */
T_MODULE T_BOOL
INTEL8254x_Setup_RX_Resources(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
	struct e1000_desc_ring *rxdr = &pDevData->rx_ring;
	/*T_UWORD 	           rx_buffer[1039];*/

	rxdr->size = rxdr->count * sizeof(struct e1000_rx_desc);

	//rxdr->descTemp =((T_UWORD)rcv_desc_ring_buf) | 0xa0000000;
	rxdr->descTemp = rcv_desc_ring_buf;
	rxdr->desc = (T_VOID *)(rxdr->descTemp);

	if(!rxdr->desc) 
	{
		return FALSE;
	}
	//tamemset(rxdr->desc , 0, rxdr->size);
	tamemset((T_VOID*)((T_UWORD)(rxdr->desc) | 0xa0000000), 0, rxdr->size);

	rxdr->next_to_clean = 0;
	rxdr->next_to_use = 0;

    cacheLsn2ePipeFlush();
	return TRUE;
}

T_MODULE T_BOOL
INTEL8254x_Up(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
	/* hardware has been reset, we need to reload some things */

	INTEL8254x_Set_Multi(pDevData);
	INTEL8254x_Configure_TX(pDevData);
	INTEL8254x_Setup_Rctl(pDevData);
	INTEL8254x_Configure_RX(pDevData);
    cacheLsn2ePipeFlush();

	return TRUE;
}

/**
 * INTEL8254x_Set_Multi - Multicast and Promiscuous mode set
 * @pDevData: network interface device structure
 *
 * The set_multi entry point is called whenever the multicast address
 * list or the network interface flags are updated.  This routine is
 * resposible for configuring the hardware for proper multicast,
 * promiscuous mode, and all-multi behavior.
 */
T_MODULE T_VOID
INTEL8254x_Set_Multi(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
	T_UWORD rctl;
	T_WORD  i;

	/* Check for Promiscuous and All Multicast modes */

	rctl = E1000_READ_REG(pDevData, RCTL);
	rctl &= ~(E1000_RCTL_UPE | E1000_RCTL_MPE);
	E1000_WRITE_REG(pDevData, RCTL, rctl);

	/* load the first 15 multicast address into the exact filters 1-15
	 * RAR 0 is used for the station MAC adddress
	 * if there are not 15 addresses, go ahead and clear the filters
	 */
	for(i = 1; i < E1000_RAR_ENTRIES; i++) 
	{
		E1000_WRITE_REG_ARRAY(pDevData, RA, i << 1, 0);
		E1000_WRITE_REG_ARRAY(pDevData, RA, (i << 1) + 1, 0);
	}

	/* clear the old settings from the multicast hash table */
	for(i = 0; i < E1000_NUM_MTA_REGISTERS; i++)
	{
		E1000_WRITE_REG_ARRAY(pDevData, MTA, i, 0);
	}	
}

/**
 * INTEL8254x_Configure_TX - Configure 8254x Transmit Unit after Reset
 * @pDevData: board private structure
 *
 * Configure the Tx unit of the MAC after a reset.
 */
T_MODULE T_VOID
INTEL8254x_Configure_TX(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
	T_UDWORD tdba  = (T_UDWORD)(pDevData->tx_ring.desc);
	T_UWORD  tdlen = pDevData->tx_ring.count * sizeof(struct e1000_tx_desc);
	T_UWORD  tctl, tipg;

	E1000_WRITE_REG(pDevData, TDBAL, (tdba & 0x00000000ffffffffULL));
	E1000_WRITE_REG(pDevData, TDBAH, 0);

	E1000_WRITE_REG(pDevData, TDLEN, tdlen);

	/* Setup the HW Tx Head and Tail descriptor pointers */

	E1000_WRITE_REG(pDevData, TDH, 0);
	E1000_WRITE_REG(pDevData, TDT, 0);

	/* Set the default values for the Tx Inter Packet Gap timer */

	tipg = DEFAULT_82543_TIPG_IPGT_COPPER;
	tipg |= DEFAULT_82543_TIPG_IPGR1 << E1000_TIPG_IPGR1_SHIFT;
	tipg |= DEFAULT_82543_TIPG_IPGR2 << E1000_TIPG_IPGR2_SHIFT;
		
	E1000_WRITE_REG(pDevData, TIPG, tipg);

	/* Set the Tx Interrupt Delay register */

	E1000_WRITE_REG(pDevData, TIDV, 0);
	E1000_WRITE_REG(pDevData, TADV, 0);

	/* Program the Transmit Control Register */

	tctl = E1000_READ_REG(pDevData, TCTL);

	tctl &= ~E1000_TCTL_CT;
	tctl |= E1000_TCTL_EN | E1000_TCTL_PSP |
	       (E1000_COLLISION_THRESHOLD << E1000_CT_SHIFT);

	E1000_WRITE_REG(pDevData, TCTL, tctl);

	INTEL8254x_Config_Collision_Dist(pDevData);
}

/**
 * INTEL8254x_Setup_Rctl - configure the receive control register
 * @pDevData: Board private structure
 */
T_MODULE T_VOID
INTEL8254x_Setup_Rctl(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
	T_UWORD rctl;

	rctl = E1000_READ_REG(pDevData, RCTL);
	

	rctl &= ~(3 << E1000_RCTL_MO_SHIFT);

	rctl |= E1000_RCTL_EN | E1000_RCTL_BAM |
	        E1000_RCTL_LBM_NO | E1000_RCTL_RDMTS_HALF ;
    rctl &= ~E1000_RCTL_SBP;

	rctl &= ~(E1000_RCTL_SZ_4096);
	switch (pDevData->rxBufSize) 
	{
		case E1000_RXBUFFER_2048:
		default:
			rctl |= E1000_RCTL_SZ_2048;
			rctl &= ~(E1000_RCTL_BSEX | E1000_RCTL_LPE);
			break;
		case E1000_RXBUFFER_4096:
			rctl |= E1000_RCTL_SZ_4096 | E1000_RCTL_BSEX | E1000_RCTL_LPE;
			break;
		case E1000_RXBUFFER_8192:
			rctl |= E1000_RCTL_SZ_8192 | E1000_RCTL_BSEX | E1000_RCTL_LPE;
			break;
		case E1000_RXBUFFER_16384:
			rctl |= E1000_RCTL_SZ_16384 | E1000_RCTL_BSEX | E1000_RCTL_LPE;
			break;
	}
	E1000_WRITE_REG(pDevData, RCTL, rctl);
}

/**
 * INTEL8254x_Configure_RX - Configure 8254x Receive Unit after Reset
 * @pDevData: board private structure
 *
 * Configure the Rx unit of the MAC after a reset.
 */
T_MODULE T_VOID
INTEL8254x_Configure_RX(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
	T_UDWORD rdba  = (T_UDWORD)pDevData->rx_ring.desc;
	T_UWORD  rdlen = pDevData->rx_ring.count * sizeof(struct e1000_rx_desc);
	T_UWORD  rctl;

	rctl = E1000_READ_REG(pDevData, RCTL);
	E1000_WRITE_REG(pDevData, RCTL, rctl & ~E1000_RCTL_EN);

	/* set the Receive Delay Timer Register */
	E1000_WRITE_REG(pDevData, RDTR, 5);
	E1000_WRITE_REG(pDevData, RADV,5);

	/* Set the interrupt throttling rate.  Value is calculated
	 * as DEFAULT_ITR = 1/(MAX_INTS_PER_SEC * 256ns) */
	E1000_WRITE_REG(pDevData, ITR, DEFAULT_ITR);

	/* Setup the Base and Length of the Rx Descriptor Ring */
	E1000_WRITE_REG(pDevData, RDBAL, (rdba & 0x00000000ffffffffULL));
	//E1000_WRITE_REG(pDevData, RDBAH, (rdba >> 32));
	E1000_WRITE_REG(pDevData, RDBAH,0);

	E1000_WRITE_REG(pDevData, RDLEN, rdlen);

	/* Setup the HW Rx Head and Tail Descriptor Pointers */
	E1000_WRITE_REG(pDevData, RDH, 0);
	E1000_WRITE_REG(pDevData, RDT, (pDevData->rx_ring.count -1));

	E1000_WRITE_REG(pDevData, RCTL, rctl);
}

/**
 * INTEL8254x_Alloc_RX_Buffers - Replace used receive buffers
 * @pDevData: address of board private structure
 */
T_MODULE T_VOID
INTEL8254x_Alloc_RX_Buffers(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
	struct e1000_desc_ring *rx_ring = &pDevData->rx_ring;
	struct e1000_rx_desc *rx_desc;

	T_WORD i;
	T_UWORD *addr;
	
	for(i = 0;i < rx_ring->count;i++) 
	{
		rx_desc = E1000_RX_DESC(*rx_ring, i);
		
		rx_desc = (struct e1000_rx_desc *)(((unsigned long)rx_desc)|0xa0000000);

		addr = (T_UWORD*)((T_UWORD)(&rx_ring->buffer_info[i].length) | 0xa0000000);
		*addr = pDevData->rxBufSize;	
		//rx_ring->buffer_info[i].length = pDevData->rxBufSize;

		rx_desc->buffer_addr = (T_UDWORD)(&(rx_ring->buffer_info[i].skb[0]));	
		rx_desc->buffer_addr &= 0x00000000ffffffffll;
		
	}

    cacheLsn2ePipeFlush();
}

/**
 * INTEL8254x_Alloc_RX_Buffers - Replace used receive buffers
 * @pDevData: address of board private structure
 */
T_MODULE T_VOID
INTEL8254x_Alloc_TX_Buffers(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
	struct e1000_desc_ring *tx_ring = &pDevData->tx_ring;
	struct e1000_tx_desc *tx_desc;

	T_WORD i;
	T_UWORD *addr;
	for(i = 0;i < tx_ring->count;i++) 
	{
		tx_desc = E1000_TX_DESC(*tx_ring, i);
             tx_desc = (struct e1000_rx_desc *)(((unsigned long)tx_desc)|0xa0000000);
		/*这个赋值没有意义*/
		//tx_ring->buffer_info[i].length = pDevData->rxBufSize;
		
		addr = (T_UWORD*)((T_UWORD)(&tx_ring->buffer_info[i].length) | 0xa0000000);
		*addr = pDevData->rxBufSize;	
		tx_desc->buffer_addr = (T_UDWORD)(&(tx_ring->buffer_info[i].skb[0]));	
		tx_desc->buffer_addr &= 0x00000000ffffffffll;		
	}
    cacheLsn2ePipeFlush();

}


/**
 * INTEL8254x_Free_TX_Resources - Free Tx Resources
 * @pDevData: board private structure
 *
 * Free all transmit software resources
 */
T_MODULE T_VOID
INTEL8254x_Free_TX_Resources(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
}

/**
 * INTEL8254x_Free_RX_Resources - Free Rx Resources
 * @pDevData: board private structure
 *
 * Free all receive software resources
 */
T_MODULE T_VOID
INTEL8254x_Free_RX_Resources(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
}

/**
 * INTEL8254x_Free_RX_Buffers - Free used receive buffers
 * @pDevData: address of board private structure
 */
T_MODULE T_VOID
INTEL8254x_Free_RX_Buffers(T_HAL_INTEL8254x_DEV_DATA *pDevData)
{
	struct e1000_desc_ring *rx_ring = &pDevData->rx_ring;
	struct e1000_rx_desc   *rx_desc;
	T_WORD                 i;

	for(i = 0;i < rx_ring->count;i++) 
	{
		rx_desc = E1000_RX_DESC(*rx_ring, i);
		rx_desc = (struct e1000_rx_desc *)(((unsigned long)rx_desc)|0xa0000000);
		rx_ring->buffer_info[i].length = 0;
		rx_desc->buffer_addr = 0;
	}
}

/**
 * Puts an ethernet address into a receive address register.
 *
 * pDevData - Struct containing variables accessed by shared code
 * addr - Address to put into receive address register
 * index - Receive address register to write
 */
T_MODULE T_VOID
INTEL8254x_Rar_Set(T_HAL_INTEL8254x_DEV_DATA *pDevData,
              T_UBYTE *addr,
              T_UWORD index)
{
    T_UWORD rar_low, rar_high;

    /* HW expects these in little endian so we reverse the byte order
     * from network order (big endian) to little endian              
     */
    rar_low = ((T_UWORD) addr[0] |
               ((T_UWORD) addr[1] << 8) |
               ((T_UWORD) addr[2] << 16) | ((T_UWORD) addr[3] << 24));

    rar_high = ((T_UWORD) addr[4] | ((T_UWORD) addr[5] << 8) | E1000_RAH_AV);

    E1000_WRITE_REG_ARRAY(pDevData, RA, (index << 1), rar_low);
    E1000_WRITE_REG_ARRAY(pDevData, RA, ((index << 1) + 1), rar_high);
}

/**
 * @brief
 *	  增加一个组播地址到该网卡
 *
 * @param[in] vpHalDevData 设备数据块指针,该结构的类型由HAL定义
 *
 * @param[in] bMacAddr 指向一个6字节的存储空间,内容为需要增加的组播MAC地址
 *
 * @param[in] vMultiMacChain 指向一个组播地址链
 * @return
 * 		FALSE 	失败
 *		TRUE	成功
 */
T_MODULE T_BOOL INTEL8254x_AddMultiAddr( T_HAL_INTEL8254x_DEV_DATA *pDevData, T_CHAR *bMacAddr, T_VOID *vMultiMacChain)
{
	if(pDevData->MultiCount >= (E1000_RAR_ENTRIES -1))
	{
	   	return FALSE;
	}

	pDevData->MultiCount++;
	INTEL8254x_Rar_Set(pDevData, bMacAddr, pDevData->MultiCount);

	return TRUE;
}

/**
 * @brief
 *	  删除该网卡的一个组播地址
 *
 * @param[in] vpHalDevData 设备数据块指针,该结构的类型由HAL定义
 *
 * @param[in] bMacAddr 指向一个6字节的存储空间,内容为需要删除的组播MAC地址
 *
 * @param[in] vMultiMacChain 指向一个组播地址链
 * @return
 * 		FALSE 	失败
 *		TRUE	成功
 */
T_MODULE T_BOOL INTEL8254x_DelMultiAddr( T_HAL_INTEL8254x_DEV_DATA * pDevData, T_CHAR *bMacAddr, T_VOID * vMultiMacChain)
{
	if(pDevData->MultiCount < 1)
	{
	    	return FALSE;
	}

	E1000_WRITE_REG_ARRAY(pDevData, RA, (pDevData->MultiCount << 1), 0);
    E1000_WRITE_REG_ARRAY(pDevData, RA, ((pDevData->MultiCount << 1) + 1), 0);

	pDevData->MultiCount--;

	return TRUE;
}
