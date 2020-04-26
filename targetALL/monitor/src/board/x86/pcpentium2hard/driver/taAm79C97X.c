/********************************************************************
 * 文件名:        taAm79C97X.c
 --------------------------------------------------------------------
 * 模块:        系统总代理
 --------------------------------------------------------------------
 * 功能:        Am79C97X_H网卡驱动程序
 --------------------------------------------------------------------
 * 作者:       袁镱
 --------------------------------------------------------------------
 * 单位:        科银-系统部
 --------------------------------------------------------------------
 * 创建日期：   2008-12-22
 --------------------------------------------------------------------
 * 修改记录:    日期        修改者      描述
 *                2008-12-22    袁镱        创建该文件
 ********************************************************************/
#include "taTypes.h"
#include "taIoAccess.h"
#include "taAm79C97X.h"
#include "taPci.h"
#include "ta.h"

#define PCIBIOS_SUCCESSFUL		0x00

//延时函数定义
#define DelayTime(i) \
	    do \
	    { \
	        volatile unsigned int m; \
	        volatile unsigned int n; \
			\
		    for(m=0;m<i;m++)\
		    {\
				for(n=0;n<i;n++)\
				{\
					;\
				}\
		    }\
	    }while(0);

T_EXTERN T_VOID printk(T_CHAR *fmt, ...);

/***********************************************************************/
/*         PCI initialization                                          */
/***********************************************************************/

static long pcibios_entry = 0;
static unsigned char tx_entry = 0;

#define	PCI_CFG_COMMAND		0x04
#define PCI_CMD_IO_ENABLE	0x0001	/* IO access enable */
#define PCI_CMD_MEM_ENABLE	0x0002	/* memory access enable */
#define PCI_CMD_MASTER_ENABLE	0x0004	/* bus master enable */


#define KERNEL_CS	0x18

/* BIOS32 signature: "_32_" */
#define BIOS32_SIGNATURE	(('_' << 0) + ('3' << 8) + ('2' << 16) + ('_' << 24))

/* PCI signature: "PCI " */
#define PCI_SIGNATURE		(('P' << 0) + ('C' << 8) + ('I' << 16) + (' ' << 24))

/* PCI service signature: "$PCI" */
#define PCI_SERVICE		(('$' << 0) + ('P' << 8) + ('C' << 16) + ('I' << 24))
T_UWORD gTaNetCardNum = 0;
/*
 * This is the standard structure used to identify the entry point
 * to the BIOS32 Service Directory, as documented in
 * 	Standard BIOS 32-bit Service Directory Proposal
 * 	Revision 0.4 May 24, 1993
 *  the PCI BIOS specification.
 */

/*
 * BIOS32的服务目录
 */
union bios32 {
    struct {
        T_UWORD signature;      /* _32_ */
        T_UWORD entry;          /* 32位的物理地址 */
        T_UBYTE revision;       /* 修订版本号, 须为0 */
        T_UBYTE  length;        /* 本数据结构的长度，16字节为单位 */
        T_UBYTE  checksum;      /* 检查和，须为0 */
        T_UBYTE  reserved[5];   /* 保留字段，须为0 */
    } fields;
    T_CHAR chars[16];
};

/*
 * Physical address of the service directory.  I don't know if we're
 * allowed to have more than one of these or not, so just in case
 * we'll make pcibios_present() take a memory start parameter and store
 * the array there.
 */

static struct {
	unsigned long address;
	unsigned short segment;
} pci_indirect = { 0, KERNEL_CS };

static unsigned long bios32_entry = 0;
static struct {
	unsigned long address;
	unsigned short segment;
} bios32_indirect = { 0, KERNEL_CS };

static unsigned long bios32_service(unsigned long service)
{
	unsigned char return_code;	/* %al */
	unsigned long address;		/* %ebx */
	unsigned long length;		/* %ecx */
	unsigned long entry;		/* %edx */

	__asm__("lcall (%%edi)"
		: "=a" (return_code),
		  "=b" (address),
		  "=c" (length),
		  "=d" (entry)
		: "0" (service),
		  "1" (0),
		  "D" (&bios32_indirect));

	switch (return_code) {
		case 0:
			return address + entry;
		case 0x80:	/* Not present */

			return 0;
		default: /* Shouldn't happen */

			return 0;
	}
}

int pcibios_read_config_dword (int bus,
	int device_fn, int where, unsigned int *value)
{
	unsigned long ret;
	unsigned long bx = (bus << 8) | device_fn;

	__asm__("lcall (%%esi)\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (ret)
		: "1" (PCIBIOS_READ_CONFIG_DWORD),
		  "b" (bx),
		  "D" ((long) where),
		  "S" (&pci_indirect));

	return (int) (ret & 0xff00) >> 8;
}


 int pcibios_read_config_byte(int bus,
	int device_fn, int where, unsigned char *value)
{
	unsigned long ret;
	unsigned long bx = (bus << 8) | device_fn;

	__asm__("lcall (%%esi)\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (ret)
		: "1" (PCIBIOS_READ_CONFIG_BYTE),
		  "b" (bx),
		  "D" ((long) where),
		  "S" (&pci_indirect));

	return (int) (ret & 0xff00) >> 8;
}

 int pcibios_read_config_word (unsigned char bus,
	unsigned char device_fn, unsigned char where, unsigned short *value)
{
	unsigned long ret;
	unsigned long bx = (bus << 8) | device_fn;

	__asm__("lcall (%%esi)\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (ret)
		: "1" (PCIBIOS_READ_CONFIG_WORD),
		  "b" (bx),
		  "D" ((long) where),
		  "S" (&pci_indirect));

	return (int) (ret & 0xff00) >> 8;
}

static int pci_bios_write_config_byte( unsigned char bus,unsigned char device_fn,unsigned char where,unsigned char value )
{
    int ret;
    int bx = (bus << 8) | device_fn;

    __asm__("lcall *(%%esi)\n\t"
            "jc 1f\n\t"
            "xor %%ah, %%ah\n"
            "1:"
:                         "=a" (ret)
:                         "0" (PCIBIOS_WRITE_CONFIG_BYTE),
            "c" (value),
            "b" (bx),
            "D" ((int) where),
            "S" (&pci_indirect));

    return (T_WORD) (ret & 0xff00) >> 8;
}

static int pci_bios_write_config_word( unsigned char bus,unsigned char device_fn,unsigned char where,unsigned char value )
{
    int ret;
    int bx = (bus << 8) | device_fn;

    __asm__("lcall *(%%esi)\n\t"
            "jc 1f\n\t"
            "xor %%ah, %%ah\n"
            "1:"
:                         "=a" (ret)
:                         "0" (PCIBIOS_WRITE_CONFIG_WORD),
            "c" (value),
            "b" (bx),
            "D" ((int) where),
            "S" (&pci_indirect));

    return (T_WORD) (ret & 0xff00) >> 8;
}


 int pcibios_find_class (unsigned int class_code, unsigned short index,
	unsigned char *bus, unsigned char *device_fn)
{
	unsigned long bx;
	unsigned long ret;

	__asm__ ("lcall (%%edi)\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=b" (bx),
		  "=a" (ret)
		: "1" (PCIBIOS_FIND_PCI_CLASS_CODE),
		  "c" (class_code),
		  "S" ((int) index),
		  "D" (&pci_indirect));

	*bus = (bx >> 8) & 0xff;
	*device_fn = bx & 0xff;
	return (int) (ret & 0xff00) >> 8;
}



 int check_pcibios(void)
{
	unsigned long signature;
	unsigned char present_status;
	unsigned char major_revision;
	unsigned char minor_revision;
	int pack;

	if ((pcibios_entry = bios32_service(PCI_SERVICE)))
	{
		pci_indirect.address = pcibios_entry;

		__asm__("lcall (%%edi)\n\t"
			"jc 1f\n\t"
			"xor %%ah, %%ah\n"
			"1:\tshl $8, %%eax\n\t"
			"movw %%bx, %%ax"
			: "=d" (signature),
			  "=a" (pack)
			: "1" (PCIBIOS_PCI_BIOS_PRESENT),
			  "D" (&pci_indirect)
			: "bx", "cx");
		present_status = (pack >> 16) & 0xff;
		major_revision = (pack >> 8) & 0xff;
		minor_revision = pack & 0xff;
		if (present_status || (signature != PCI_SIGNATURE))
		{
			if (signature != PCI_SIGNATURE)
				pcibios_entry = 0;
		}
		if (pcibios_entry)
		{

			return 1;
		}
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

/*-------yuan defined-----------------------------*/
static void 	ln97xCsrWrite (LN_97X_DRV_CTRL * pDrvCtrl, int reg,
                               UINT32 value);
static void 	ln97xRestart (LN_97X_DRV_CTRL * pDrvCtrl);
static STATUS 	ln97xRestartSetup (LN_97X_DRV_CTRL * pDrvCtrl);
static UINT32 	ln97xCsrRead (LN_97X_DRV_CTRL * pDrvCtrl, int reg);
static void	ln97xBcrWrite (LN_97X_DRV_CTRL * pDrvCtrl, int reg,
                               UINT32 value);
static void 	ln97xTRingScrub (LN_97X_DRV_CTRL * pDrvCtrl);
static void	ln97xConfig (LN_97X_DRV_CTRL * pDrvCtrl);
static STATUS 	ln97xMemInit (LN_97X_DRV_CTRL * pDrvCtrl);
static UINT32 	ln97xBcrRead (LN_97X_DRV_CTRL * pDrvCtrl, int reg);
static STATUS    ln97xPollSend (LN_97X_DRV_CTRL * pDrvCtrl, char* pPktBuf, int len);
static STATUS    ln97xPollReceive (LN_97X_DRV_CTRL * pDrvCtrl, char* pBuf);
static STATUS ln97xPollStart(LN_97X_DRV_CTRL *);

#define PCI_CFG_MODE            0x43
#define SLEEP_MODE_DIS          0x00    /* sleep mode disable */
#define PCI2DRAM_BASE_ADRS   (0)     /* DRAM base address from PCI */

#define LN97X_OFFS_VALUE        (0)       /* driver <offset> value */
#define LN97X_CSR3_VALUE        (0)       /* CSR3 register value */
#define LN97X_RSVD_FLAGS        (0)       /* driver <flags> value */

#define LN97_BUF_MEM_SIZE 1024*2

/**********************************************************/
/*   RTL8139   TA					                        */
/***********************************************************/
#define LN_KICKSTART_TX TRUE

static unsigned short 	rtl8139_vendor = 0x1022;
static unsigned short 	rtl8139_device = 0x2000;
//static unsigned char 	rtl8139_rcv_buf[RX_BUF_LEN + 16];
static unsigned int 	rtl8139_rcv_curpos = 0;
static unsigned long  	rtl8139_iobase = 0;
static unsigned long  	rtl8139_memIO = 0;
static unsigned short 	tra_port_vector = 0;

static BOOL lnKickStartTx = LN_KICKSTART_TX;

static LN_97X_DRV_CTRL ln97_ctrl = {0};
static LN_97X_DRV_CTRL* p_ln97_ctrl = &ln97_ctrl;
static int lnTsize = LN_TMD_MIN;    /* deflt xmit ring size as power of 2 */
static int lnRsize = LN_RMD_MIN;    /* deflt recv ring size as power of 2 *//*yuany: 这个值的调整需要和接收缓冲ln97_rcv_buf 的大小同步*/
#define RCV_BUF_SIZE 1024*2
#define SND_BUF_SIZE 1024*2

static char ln97_bufMem[LN97_BUF_MEM_SIZE] = {0}; 			/*控制描述符缓冲的大小*/
static char ln97_rcv_buf[1<<LN_RMD_MIN][RCV_BUF_SIZE];	/*接收缓冲*/
static char ln97_snd_buf[1<<LN_TMD_MIN][SND_BUF_SIZE];

/*******************************************************************************
*
* ln97xMemInit - initialize memory for Lance chip
*
* Using data in the control structure, setup and initialize the memory
* areas needed.  If the memory address is not already specified, then allocate
* cache safe memory.
*
* RETURNS: OK or ERROR.
*/

static STATUS ln97xMemInit
    (
    LN_97X_DRV_CTRL * pDrvCtrl /* device to be initialized */
    )
    {
    UINT32      sz;             /* temporary size holder */
    int         ix;
    LN_RMD *    pRmd;
    void *      pTemp;
    char *      pTempBuf;

    /* Remember register addresses */

    pDrvCtrl->pRdp   = LN_97X_RDP;
    pDrvCtrl->pRap   = LN_97X_RAP;
    pDrvCtrl->pReset = LN_97X_RST;
    pDrvCtrl->pBdp   = LN_97X_BDP;

	sz = LN97_BUF_MEM_SIZE;
	pDrvCtrl->pShMem = ln97_bufMem;
	
    /*                        Turkey Carving
     *                        --------------
     *
     *                          LOW MEMORY
     *
     *             |-------------------------------------|
     *             |       The initialization block      |
     *             |         (sizeof (LN_IB))            |
     *             |-------------------------------------|
     *             |         The Rx descriptors          |
     *             | (1 << lnRsize) * sizeof (LN_RMD)|
     *             |-------------------------------------|
     *             |         The Tx descriptors          |
     *             | (1 << lnTsize) * sizeof (LN_TMD)|
     *             |-------------------------------------|
     */

    /* Save some things */

    pDrvCtrl->memBase  = (char *)((UINT32)pDrvCtrl->pShMem & 0xff000000);

    if ((int) pDrvCtrl->memAdrs == NONE)
        pDrvCtrl->flags |= LS_MEM_ALLOC_FLAG;

    /* first let's clear memory */

    memset ( (char *) pDrvCtrl->pShMem, (int) sz ,0);

    /* setup Rx memory pointers */

    pDrvCtrl->pRring    = (LN_RMD *) ((int)pDrvCtrl->pShMem + IB_SIZ);
    pDrvCtrl->rringLen  = lnRsize;
    pDrvCtrl->rringSize = 1 << lnRsize;
    pDrvCtrl->rmdIndex  = 0;

    /* setup Tx memory pointers. */

    /* Note: +2 is to round up to alignment. */

    pDrvCtrl->pTring = (LN_TMD *) (int)(pDrvCtrl->pShMem + IB_SIZ +
                        ((1 << lnRsize) + 1) * RMD_SIZ + 0xf);
    pDrvCtrl->pTring = (LN_TMD *) (((int)pDrvCtrl->pTring + 0xf) & ~0xf);

    pDrvCtrl->tringSize = 1 << lnTsize;
    pDrvCtrl->tringLen  = lnTsize;
    pDrvCtrl->tmdIndex  = 0;
    pDrvCtrl->tmdIndexC = 0;

    /* Set up the structures to allow us to free data after sending it. */

  /*yuany主要用于异步发送
  for (ix = 0; ix < pDrvCtrl->rringSize; ix++)
	{
	pDrvCtrl->freeRtn[ix] = NULL;
	pDrvCtrl->freeData[ix].arg1 = NULL;
	pDrvCtrl->freeData[ix].arg2 = NULL;
	}
*/
    /* Longword align rmd ring */

    pDrvCtrl->pRring = (LN_RMD *) (((int)pDrvCtrl->pRring + 0xf) & ~0xf);
    pDrvCtrl->pRring = (LN_RMD *) (((int)pDrvCtrl->pRring + 0xf) & ~0xf);
    pRmd = pDrvCtrl->pRring;
    //printk( "rcv_buf \n");

    for (ix = 0; ix < pDrvCtrl->rringSize; ix++, pRmd++)
        {
	 pTempBuf = ln97_rcv_buf[ix];
        pTempBuf += pDrvCtrl->offset;

        //LN_RMD_BUF_TO_ADDR (pRmd, pTemp, pTempBuf);
	pRmd->rBufAddr = pTempBuf;
        }

    return OK;
    }


STATUS sysLan97xEnetAddrGet
    (
    LN_97X_DRV_CTRL *  pDrvCtrl,   /* Driver control */
    char *             enetAdrs
    )
    {
    char          aprom [LN_97X_APROM_SIZE];  /* copy of address PROM space */

    int           numBytes = (LN_97X_APROM_SIZE >> 1);
    register int  ix;

    /* get IO address of unit */

    UINT8 * const ioaddr = (UINT8 * const)(pDrvCtrl->devAdrs);


    /* load APROM into an array */

    if (pDrvCtrl->flags & LS_MODE_MEM_IO_MAP)
        {
        for (ix = 0; ix < numBytes; ++ix)
            {
            aprom[ix] = *(ioaddr + ix);
            }
        }
    else
        {
        for (ix = 0; ix < numBytes; ++ix)
            {
		BSP_InByte((int)(ioaddr + ix), aprom[ix] );
            }
        }


    /* check for ASCII 'W's at APROM bytes 14 and 15 */

    if ((aprom [0xe] != 'W') || (aprom [0xf] != 'W'))
        {
        printk ("W's not stored in aprom\n");

        return ERROR;
        }
	memcpy(enetAdrs,aprom,6);

    	return (OK);
}

int  ln97xEndLoad
    (
                 int endUnit,                     /* END unit number */
                 int membase,     /* memory-mapped IO base */
                 int iobase,     /* IO address space base */
                 int pci_mem_base,          /* host PCI mem. base */
                 int irqvec,     /* IRQ vector */
                 int irq,        /* IRQ number */
                 int csr3_value,            /* csr3 register value */
                 int offset,            /* offset */
                 int flag            /* flags (reserved) */
    )
{
    LN_97X_DRV_CTRL *	pDrvCtrl= p_ln97_ctrl;
	int i;
	
	//yuany
	//printk("InitString : %d:0x%x:0x%x:0x%x:%d:%d:-1:-1:-1:0x%x:%d:0x%x", endUnit, membase, iobase, pci_mem_base, irqvec, irq, 
	//		csr3_value, offset, flag);
	


    /* parse the init string, filling in the device structure */

    /*if (ln97xInitParse (pDrvCtrl, initString) == ERROR)
	{
	DRV_LOG (DRV_DEBUG_LOAD, "Parse failed ...\n", 1, 2, 3, 4, 5, 6);
        goto errorExit;
	}*/
	pDrvCtrl->unit = endUnit;

	if (membase != NONE)
        {
	        pDrvCtrl->devAdrs = membase;
	        pDrvCtrl->flags   |= LS_MODE_MEM_IO_MAP;
        }
    	else
	        pDrvCtrl->devAdrs = iobase;

	pDrvCtrl->pciMemBase = pci_mem_base;
	pDrvCtrl->ivec = irqvec;
	pDrvCtrl->ilevel = irq;
	pDrvCtrl->memAdrs = NONE;
	pDrvCtrl->memSize = -1;
	pDrvCtrl->memWidth = -1;
	pDrvCtrl->csr3B = csr3_value; 
	pDrvCtrl->offset = offset;
	pDrvCtrl->flags |= flag;
	#if 0
	//yuany
	printk("%d 0x%x 0x%x %d %d     0x%x %d %d 0x%x %d 0x%x\n", pDrvCtrl->unit, 
				pDrvCtrl->devAdrs,
				pDrvCtrl->pciMemBase,
				pDrvCtrl->ivec,
				pDrvCtrl->ilevel ,
				pDrvCtrl->memAdrs ,
				pDrvCtrl->memSize ,
				pDrvCtrl->memWidth ,
				pDrvCtrl->csr3B ,
				pDrvCtrl->offset ,
				pDrvCtrl->flags 
				);
#endif
    /* Have the BSP hand us our address. */

    sysLan97xEnetAddrGet (pDrvCtrl, &(pDrvCtrl->enetAddr [0]));
	

    /* initialize the END and MIB2 parts of the structure */
#if 0
    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ *)pDrvCtrl, LN_97X_DEV_NAME,
                      pDrvCtrl->unit, &ln97xFuncTable,
                      "AMD 79C970 Lance PCI Enhanced Network Driver") == ERROR
     || END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
                      &pDrvCtrl->enetAddr[0], 6, ETHERMTU,
                      LN_SPEED)
                    == ERROR)
        goto errorExit;
#endif

   // DRV_LOG (DRV_DEBUG_LOAD, "END init done ...\n", 1, 2, 3, 4, 5, 6);

    /* Perform memory allocation */

    if (ln97xMemInit (pDrvCtrl) == ERROR)
        goto errorExit;


    /* Perform memory distribution and reset and reconfigure the device */

    if (ln97xRestartSetup (pDrvCtrl) == ERROR)
        goto errorExit;

	pDrvCtrl->txCleaning = FALSE;
	//ln97xPollStart(pDrvCtrl);
    return OK;

errorExit:
    return ERROR;

}
/********************************************************************
 * 函数名:   taNetOpen
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
 * 返回值:   如果成功返回SUCC，否则返回FAIL.
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
T_BOOL taNetOpen(T_UBYTE minor, UINT8 * mac)
{
    int i;
    int full_duplex;
    unsigned short lpar;
    int pci_index = 0;
	unsigned char pci_bus, pci_device_fn,pci_irq_line;
	unsigned short pci_command,new_command, vendor, device;
	unsigned short ininw = 0;

	pcibios_init();

	for (;pci_index < 0xff; pci_index++)
	{
		if (pcibios_find_class (PCI_CLASS_NETWORK_ETHERNET << 8, pci_index,	&pci_bus, &pci_device_fn) != PCIBIOS_SUCCESSFUL)
		{
			break;
		}
		pcibios_read_config_word(pci_bus, pci_device_fn,PCI_VENDOR_ID, &vendor);
		pcibios_read_config_word(pci_bus, pci_device_fn,PCI_DEVICE_ID, &device);

		if((vendor!=rtl8139_vendor)||((device!=rtl8139_device)&&(device!=0x8129)))/*related the eeprom's 00h*/
		    continue;

		break;
	}

	/*读取IO基地址*/
    	pcibios_read_config_dword  (pci_bus, pci_device_fn,
                      PCI_BASE_ADDRESS_0, &rtl8139_iobase);
   	pcibios_read_config_dword  (pci_bus, pci_device_fn,
                      PCI_BASE_ADDRESS_1, &rtl8139_memIO);


	/* Remove I/O space marker in bit 0. */
	rtl8139_iobase &= PCI_BASE_ADDRESS_IO_MASK;
	rtl8139_memIO &= PCI_BASE_ADDRESS_MEM_MASK;

	// TODO: rtl8139_memIO需要映射

	/*读取中断地址*/
	pcibios_read_config_byte(pci_bus, pci_device_fn,PCI_INTERRUPT_LINE, &pci_irq_line);

	tra_port_vector = 0x20 + pci_irq_line ;

	printk("Find a ln97 io %x, memIO %x irq: %x\n", rtl8139_iobase, rtl8139_memIO, pci_irq_line);

    pci_bios_write_config_word(pci_bus, pci_device_fn, PCI_CFG_COMMAND,
                      PCI_CMD_MEM_ENABLE | PCI_CMD_IO_ENABLE |
                      PCI_CMD_MASTER_ENABLE);
    /* disable sleep mode */
    pci_bios_write_config_byte (pci_bus, pci_device_fn, PCI_CFG_MODE,
                      SLEEP_MODE_DIS);


	/*加载网卡*/	
	ln97xEndLoad(0, NONE, rtl8139_iobase, 
			PCI2DRAM_BASE_ADRS, 
			0x20 + pci_irq_line, /*yuany, 中断向量号*/
			pci_irq_line , 
			LN97X_CSR3_VALUE , /*yuany屏蔽发送中断*/
			LN97X_OFFS_VALUE, 
			LN97X_RSVD_FLAGS);


	/*将mac地址交给ta*/
    memcpy(mac, p_ln97_ctrl->enetAddr, 6);

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
	printk("Null func: fnTA_net_close\n");
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
    unsigned char isr;
    unsigned short ISRstatus;
    LN_97X_DRV_CTRL * 	pDrvCtrl = p_ln97_ctrl; /* device to be initialized */
		

     UINT32	   	stat;
     
    /* Read the device status register */
    stat = ln97xCsrRead (pDrvCtrl, 0);

    /* If false interrupt, return. */
    if (! (stat & CSR0_INTR))
        {
        return;
        }

    /*
     * enable interrupts, clear receive and/or transmit interrupts, and clear
     * any errors that may be set.
     * Writing back what was read clears all interrupts
     */
    ln97xCsrWrite (pDrvCtrl, 0, stat);

    /* Check for errors */

    if (stat & (CSR0_BABL | CSR0_MISS | CSR0_MERR))
	{

        if (stat & CSR0_BABL)
            {
            printk( "CSR0_BABL Error\n");
            }
        if (stat & CSR0_MISS)
            {
            /*清除MISS标志位*/
	     //ln97xCsrWrite (pDrvCtrl, 0, (stat |CSR0_MISS));
            printk( "CSR0_MISS Error\t");
            }

        //DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad error\n", 1, 2, 3, 4, 5, 6);
	//END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

        /* restart chip on fatal error */

        if (stat & CSR0_MERR)        /* memory error */
            {
            printk( "memory error, will restart\n");
            ln97xRestart (pDrvCtrl);
            return;
	    }
	}


    /* Have netTask handle any input packets */

    if ((stat & CSR0_RINT) && (stat & CSR0_RXON))
	{
       // if (!(pDrvCtrl->flags & LS_RCV_HANDLING_FLAG))
	    {
		isr = 1;
	    }
	}

    /*判断是否是正确的收到一个帧*/
    if(isr == 1 )
    {
    	#if 0
        /*标志数据包来自于设备异常的被动接收*/
        TAgent_net_pkt_come_from_isr = true;

        if(!setjmp(TAgent_net_longjmp_buf))
        {
        	//printk( "callback\n");
            callback();
        }
        else
        {
            /*清除状态*/
            TAgent_net_pkt_come_from_isr = false;

        }
       #endif

    }

    return;

}

/********************************************************************
 * 函数名:   taNetGetpkt
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
T_WORD taNetGetpkt(T_UBYTE minor,T_CHAR* buf,T_WORD* offset)
{
    int rx_count = 0;
    int rx_size = 0;
	unsigned char ioin;
	
        while(1)
        {
		rx_size = ln97xPollReceive(p_ln97_ctrl, buf);
		if(rx_size== ERROR)
		{
			if(rx_count ++>10)
				return 0;
			continue;
		}
		*offset = 0;
		return rx_size;
        }
}

/********************************************************************
 * 函数名:   taNetPutpkt
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
T_VOID taNetPutpkt(T_UBYTE minor,T_CHAR* buf,T_WORD cnt)
{
	int i;
	ln97xPollSend( p_ln97_ctrl, buf, cnt);
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
     UINT32	   	stat;
     
	//这个中断的开关方案是针对VmWare的临时方案。
	//实际上在中断处理过程中，中断并没有被关掉
	//VmWare的虚拟网卡有时会在进入中断时自动关掉中断
	//具体原因还不清楚。
    if(bool)
    {
        /*允许接收中断*/
  	     stat = ln97xCsrRead (p_ln97_ctrl , 0);
		if(!(stat & CSR0_INEA))
		{
			stat |= CSR0_INEA; 
            		ln97xCsrWrite (p_ln97_ctrl, 0, stat);
		}
    }
    else
    {
        /*禁止接收中断*/
    	 //    stat = ln97xCsrRead (p_ln97_ctrl , 0);
	//	stat &= ~CSR0_INEA; 
       //     ln97xCsrWrite (p_ln97_ctrl, 0, stat);
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
     UINT32	   	stat;
	printk("Null func: fnTA_net_enable_rx_int %d\n", bool);
    if(bool)
    {
        /*允许接收中断*/
    	     stat = ln97xCsrRead (p_ln97_ctrl , 3);
		stat &= ~CSR3_TINTM; 
            ln97xCsrWrite (p_ln97_ctrl, 3, stat);
    }
    else
    {
        /*禁止接收中断*/
    	     stat = ln97xCsrRead (p_ln97_ctrl , 3);
		stat |= CSR3_TINTM; 
            ln97xCsrWrite (p_ln97_ctrl, 3, stat);
    }
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
    return tra_port_vector;
}

/*******************************************************************************
*
* ln97xReset - hardware reset of chip (stop it)
*
* This routine is responsible for resetting the device and switching into
* 32 bit mode.
*
* RETURNS: OK/ERROR
*/

static int ln97xReset
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    UINT32 		resetTmp;

    /* Enable 32 bit access by doing a 32 bit write */

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRdp, 0);

    ln97xCsrWrite (pDrvCtrl, CSR(0), CSR0_STOP);
    
    /* Generate a soft-reset of the controller */
    SYS_IN_LONG(pDrvCtrl, pDrvCtrl->pReset, resetTmp);

    /* This isn't a real test - it just stops the compiler ignoring the read */

    if (resetTmp == 0x12345678)
	return (ERROR);

    ln97xBcrWrite (pDrvCtrl, BCR(20), BCR20_SWSTYLE_PCNET);

    /* autoselect port tye - 10BT or AUI */
    ln97xBcrWrite (pDrvCtrl, BCR(2), BCR2_AUTO_SELECT); 

    /* read BCR */
    resetTmp = ln97xBcrRead (pDrvCtrl, BCR(20));

    return (OK);
    }

/*******************************************************************************
*
* ln97xCsrWrite - select and write a CSR register
*
* This routine selects a register to write, through the RAP register and
* then writes the CSR value to the RDP register.
*
* RETURNS: N/A
*/

static void ln97xCsrWrite
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    int 		reg,	 /* CSR register to select */
    UINT32 		value	 /* CSR value to write */
    )
    {
    //int 		level;

    //level = intLock ();

    /* select CSR */

    reg &= 0xff;

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRap, PCI_SWAP (reg));

    //CACHE_PIPE_FLUSH ();

    value &=0xffff;

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRdp, PCI_SWAP (value));

    //CACHE_PIPE_FLUSH ();

    //intUnlock (level);
    }

/*******************************************************************************
*
* ln97xCsrRead - select and read a CSR register
*
* This routine selects a register to read, through the RAP register and
* then reads the CSR value from the RDP register.
*
* RETURNS: N/A
*/

static UINT32 ln97xCsrRead
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    int			reg  	 /* register to select */
    )
    {
    //int 		level;
    UINT32 		result;
    
    //level = intLock ();
    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRap, PCI_SWAP (reg));
    //CACHE_PIPE_FLUSH ();
    SYS_IN_LONG (pDrvCtrl, pDrvCtrl->pRdp, result);
    //intUnlock (level);

    return (PCI_SWAP (result) & 0x0000FFFF);
    }

/*******************************************************************************
*
* ln97xBcrWrite - select and write a BCR register
*
* This routine writes the bus configuration register. It first selects the
* BCR register to write through the RAP register and then it writes the value
* to the BDP register.
*
* RETURNS: N/A
*/

static void ln97xBcrWrite
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    int 		reg,     /* BCR register to select */
    UINT32 		value    /* BCR value to write */
    )
    {
    //int 		level;

    reg &= 0xff;

    value &=0xffff;

    //level = intLock ();

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRap, PCI_SWAP(reg));
    //CACHE_PIPE_FLUSH ();

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pBdp, PCI_SWAP(value));
    //intUnlock (level);
    }

/*******************************************************************************
*
* ln97xBcrRead - select and read a BCR register
*
* This routine reads the bus configuration register. It first selects the
* BCR register to read through the RAP register and then it reads the value
* from the BDP register.
*
* RETURNS: N/A
*/

static UINT32 ln97xBcrRead
    (
    LN_97X_DRV_CTRL *	pDrvCtrl,		/* driver control */
    int		reg			/* register to select */
    )
    {
    //int level;
    UINT32 result;
    
   // level = intLock ();

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRap, PCI_SWAP (reg));

    SYS_IN_LONG (pDrvCtrl, pDrvCtrl->pBdp, result);

    //intUnlock (level);

    return (PCI_SWAP (result) & 0x0000FFFF);
    }

/*******************************************************************************
*
* ln97xRestartSetup - setup memory descriptors and turn on chip
*
* This routine initializes all the shared memory structures and turns on
* the chip.
*
* RETURNS OK/ERROR
*/

static STATUS ln97xRestartSetup
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {  
    int         	rsize;	/* recv ring length */
    int         	tsize;	/* xmit ring length */
    LN_TMD *    	pTmd;

    /* reset the device */

    ln97xReset (pDrvCtrl);

    /* setup Rx buffer descriptors  - align on 000 boundary */

    rsize = pDrvCtrl->rringLen;

    /* 
     * setup Tx buffer descriptors  - 
     *      save unaligned location and align on 000 boundary 
     */

    pTmd = pDrvCtrl->pTring;
    tsize = pDrvCtrl->tringLen;

    /* setup the initialization block */

    pDrvCtrl->ib = (LN_IB *)pDrvCtrl->pShMem;
    
    //yuany 拷贝MAC地址bcopy ((char *) END_HADDR(&pDrvCtrl->endObj), pDrvCtrl->ib->lnIBPadr, 6);
    memcpy(pDrvCtrl->ib->lnIBPadr, p_ln97_ctrl->enetAddr, 6);
	
    //CACHE_PIPE_FLUSH ();

    /* point to Rx ring */

    LN_ADDR_TO_IB_RMD (pDrvCtrl->pRring, pDrvCtrl->ib, rsize);

    /* point to Tx ring */

    LN_ADDR_TO_IB_TMD (pDrvCtrl->pTring, pDrvCtrl->ib, tsize);

    //DRV_LOG (DRV_DEBUG_LOAD, "Memory setup complete\n", 1, 2, 3, 4, 5, 6);

    /* reconfigure the device */

    ln97xConfig (pDrvCtrl);

    return (OK);
    }

/*******************************************************************************
*
* ln97xRestart - restart the device after a fatal error
*
* This routine takes care of all the messy details of a restart.  The device
* is reset and re-initialized.  The driver state is re-synchronized.
*
* RETURNS: N/A
*/

static void ln97xRestart
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    ln97xReset (pDrvCtrl);

    ln97xRestartSetup (pDrvCtrl);

    }

/******************************************************************************
*
* ln97xConfig - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* RETURNS: N/A
*/

static void ln97xConfig
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    UINT16 		stat;
    void * 		pTemp;
    LN_RMD *		pRmd;
    LN_TMD *		pTmd;
    int 		ix;
    int 		timeoutCount = 0;

    ln97xCsrWrite (pDrvCtrl, 0, CSR0_STOP);     /* set the stop bit */

    /* set the bus mode to little endian */

    ln97xCsrWrite (pDrvCtrl, 3, pDrvCtrl->csr3B);

    /* set the Bus Timeout to a long time */
    /* This allows other stuff to hog the bus a bit more */

    ln97xCsrWrite (pDrvCtrl, 100, BUS_LATENCY_COUNT  );

    pRmd = pDrvCtrl->pRring;

    for (ix = 0; ix < pDrvCtrl->rringSize; ix++, pRmd++)
        {
        LN_CLEAN_RXD (pRmd);
        }
    pDrvCtrl->rmdIndex = 0;

    pTmd = pDrvCtrl->pTring;

    for (ix = 0; ix < pDrvCtrl->tringSize; ix++, pTmd++)
        {
        pTmd->tBufAddr = 0;                    /* no message byte count yet */
	pTemp = (void *)(TMD1_CNST | TMD1_ENP | TMD1_STP);
        pTmd->tBufTmd1 = (UINT32) PCI_SWAP (pTemp);
        LN_TMD_CLR_ERR (pTmd);
        }

    pDrvCtrl->tmdIndex = 0;
    pDrvCtrl->tmdIndexC = 0;

    /* Point the device to the initialization block */

    pTemp = LN_CACHE_VIRT_TO_PHYS (pDrvCtrl->ib);
    pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));
    //printk( "IB mode 0x%x\n", pDrvCtrl->ib->lnIBMode);

    ln97xCsrWrite (pDrvCtrl, CSR(2), (((ULONG)pTemp >> 16) & 0x0000ffff));
    ln97xCsrWrite (pDrvCtrl, CSR(1), ((ULONG)pTemp & 0x0000ffff));
    ln97xCsrWrite (pDrvCtrl, CSR(0), CSR0_INIT);    /* init chip (read IB) */

    /* hang until Initialization DONe, ERRor, or timeout */

    while (((stat = ln97xCsrRead (pDrvCtrl, 0)) & (CSR0_IDON | CSR0_ERR)) == 0)
        {
        if (timeoutCount++ > 0x100)
            break;

        DelayTime (2 * timeoutCount);
        }

    //printk( "Timeoutcount %d\n", timeoutCount);

    /* log chip initialization failure */

    if (((stat & CSR0_ERR) == CSR0_ERR) || (timeoutCount >= 0x10000))
        {
        printk("Device initialization failed\n");
        return;
        }

   /* Setup LED controls */

    ln97xBcrWrite (pDrvCtrl, BCR(4), 0x0090);
    ln97xBcrWrite (pDrvCtrl, BCR(5), 0x0084);
    ln97xBcrWrite (pDrvCtrl, BCR(7), 0x0083);

    if (!(pDrvCtrl->flags & LS_POLLING))
	{
        //printk("int mode");
        ln97xCsrWrite (pDrvCtrl, 0, CSR0_INTMASK | CSR0_STRT);
    	}
	else
	{
        //printk("polling mode");
        ln97xCsrWrite (pDrvCtrl, 0, CSR0_STRT);
	}
	
        //printk("Device initialization OK\n");
    }

/*******************************************************************************
*
* ln97xFullRMDGet - get next received message RMD
*
* Returns ptr to next Rx desc to process, or NULL if none ready.
*/

static LN_RMD * ln97xFullRMDGet
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    LN_RMD *		pRmd;

    pRmd = pDrvCtrl->pRring + pDrvCtrl->rmdIndex;  /* form ptr to Rx desc */

    /* If receive buffer has been released to us, return it */

    if ((PCI_SWAP (pRmd->rBufRmd1) & RMD1_OWN) == 0)
        return (pRmd);
    else
        return ((LN_RMD *) NULL);
    }


/*******************************************************************************
*
* ln97xPollReceive - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device. This routine return OK if it is successful in getting the packet
*
* RETURNS: OK or EAGAIN.
*/
void init_check(LN_97X_DRV_CTRL *  pDrvCtrl);

static STATUS ln97xPollReceive
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    char* 		pBuf
    )
    {
    LN_RMD *		pRmd;
    UINT16 		stat;
    char * 		pPacket;
    int 		len;
#if 0
if(TAgent_net_pkt_come_from_isr != true){
    /* Read the device status register */

    stat = ln97xCsrRead (pDrvCtrl, CSR(0));
	ln97xCsrWrite (pDrvCtrl, 0, stat);
    /* Check for errors */

    if (stat & (CSR0_BABL | CSR0_MISS | CSR0_MERR))
        {
        if (stat & CSR0_BABL)
            {
            printk( "CSR0_BABL Error\n");
            }
        if (stat & CSR0_MISS)
            {
            /*清除MISS标志位*/
	     //ln97xCsrWrite (pDrvCtrl, 0, (stat |CSR0_MISS));
            printk( "CSR0_MISS Error\t");
            }

        //DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad error\n", 1, 2, 3, 4, 5, 6);
	//END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

        /* restart chip on fatal error */

        if (stat & CSR0_MERR)        /* memory error */
            {
            printk( "memory error, will restart\n");
	    //END_FLAGS_CLR (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));
            //DRV_LOG (DRV_DEBUG_POLL_RX, "PRX restart\n", 1, 2, 3, 4, 5, 6);
            //ln97xRestart (pDrvCtrl);
            }

	return (ERROR);
        }

    /* If no interrupt then return. */

    if (!(stat & CSR0_RINT))
	{
            //printk( "nr\t");
        //DRV_LOG (DRV_DEBUG_POLL_RX, "PRX no rint\n", 1, 2, 3, 4, 5, 6);
        return (ERROR);
	}

    /*
     * clear receive interrupts, and clear any errors that may be set.
     */

    ln97xCsrWrite (pDrvCtrl, 0, (stat & (CSR0_INTMASK | CSR0_STRT)));
}
#endif
    /* Packet must be checked for errors. */

    pRmd = ln97xFullRMDGet (pDrvCtrl);

    if (pRmd == NULL)
	{
           // printk( "PRX no rmd\n");
        //DRV_LOG (DRV_DEBUG_POLL_RX, "PRX no rmd\n", 1, 2, 3, 4, 5, 6);
	return (ERROR);
	}

    if  (LN_RMD_ERR (pRmd))
	{
            printk( "PRX bad rmd\n");
	//END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        //DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad rmd\n", 1, 2, 3, 4, 5, 6);
	goto cleanRXD;
	}

    //END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);
    len = LN_PKT_LEN_GET (pRmd);		/* get packet length */

    /* Get pointer to packet */
            //printk( "\nPoll get a pkt");
    LN_RMD_TO_ADDR (pRmd, pPacket);
    
    LN_CACHE_INVALIDATE (pPacket, len);   /* make the packet coherent */

    /* Upper layer provides the buffer. */
	memset(pBuf +len, 0, 0x20);
	memcpy(pBuf, pPacket, len);
    /* Done with descriptor, clean up and give it to the device. */
cleanRXD:
    LN_CLEAN_RXD (pRmd);

    /* Flush the write pipe */

    //CACHE_PIPE_FLUSH ();

    /* Advance our management index */

    pDrvCtrl->rmdIndex = (pDrvCtrl->rmdIndex + 1) & (pDrvCtrl->rringSize - 1);

    //DRV_LOG (DRV_DEBUG_POLL_RX, "PRX ok\n", 1, 2, 3, 4, 5, 6);

    return (len);
    }


static int TMD_Clean(
    LN_97X_DRV_CTRL * 	pDrvCtrl/* device to be initialized */
)
{
    LN_TMD * 		pTmd;
    UINT32 		tmpTmd2 = 0;
    int i = 0;
	
    while (pDrvCtrl->tmdIndexC != pDrvCtrl->tmdIndex)
        {
        /* disposal has not caught up */

        pTmd = pDrvCtrl->pTring + pDrvCtrl->tmdIndexC;

        /* if the buffer is still owned by LANCE, don't touch it */

        LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

        if (PCI_SWAP (pTmd->tBufTmd1) & TMD1_OWN)
        {
        	  //printk( "Lance owned\n", pDrvCtrl->tmdIndex, pDrvCtrl->tmdIndexC);

            	break;
        }
        /* now bump the tmd disposal index pointer around the ring */

        pDrvCtrl->tmdIndexC = (pDrvCtrl->tmdIndexC + 1) &
			      (pDrvCtrl->tringSize - 1);


	LN_TMD_CLR_ERR (pTmd);
	i++;
        }
	return i;
  	
}
/*******************************************************************************
*
* ln97xPollSend - routine to send a packet in polled mode.
*
* This routine is called by a user to try and send a packet on the
* device.
*
* RETURNS: OK or EAGAIN.
*/

static STATUS ln97xPollSend
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    char* pPktBuf,
    int pktlen
    )
    {
    LN_TMD *		pTmd;
    void *       	pTemp;
    int         	len = 0;
    //int         	oldLevel;
    ULONG		ltmd1;
    UINT16		stat;
    char *       	pBuf = NULL;
    char *       	pOrig = NULL;
    int timeoutCount = 0;
	
	//printk( "\tPollSend.");
    //DRV_LOG (DRV_DEBUG_POLL_TX, "PTX b\n", 1, 2, 3, 4, 5, 6);

    /* See if next TXD is available */

    pTmd = pDrvCtrl->pTring + pDrvCtrl->tmdIndex;

    LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

    //if (pTmd->tBufTmd1 & TMD1_OWN)
		
   if(((pDrvCtrl->tmdIndex + 1) &(pDrvCtrl->tringSize - 1)) == pDrvCtrl->tmdIndexC)
        {
          //printk( "Out of tmds.");

        //DRV_LOG (DRV_DEBUG_POLL_TX, "Out of tmds.\n", 1, 2, 3, 4, 5, 6);

	//if (!pDrvCtrl->txCleaning)
	//	{
	//    ln97xTRingScrub(pDrvCtrl);
	//	}
	//return (ERROR);
//
	if( TMD_Clean(pDrvCtrl) == 0)
		return (ERROR);
	
     }
	
	

    /*
     * If we don't have alignment issues then we can transmit
     * directly from the M_BLK otherwise we have to copy.
     */
	if(pktlen < ETHERSMALL)
		len = ETHERSMALL;
	else
		len = pktlen;
    memcpy(ln97_snd_buf[pDrvCtrl->tmdIndex], pPktBuf, len);
    LN_TMD_BUF_TO_ADDR(pTmd, pTemp, ln97_snd_buf[pDrvCtrl->tmdIndex]);
    /* place a transmit request */

   // oldLevel = intLock ();          /* disable ints during update */

    pTmd->tBufTmd2 = 0;                     /* clear buffer error status */
    //pTmd->tBufTmd3 = 0;

    ltmd1 = TMD1_STP | TMD1_ENP | TMD1_CNST;

    ltmd1 |= TMD1_BCNT_MSK & -len;
    pTmd->tBufTmd1 = PCI_SWAP (ltmd1);

    LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

    ltmd1 |= TMD1_OWN;
    pTmd->tBufTmd1 = PCI_SWAP (ltmd1);



   // intUnlock (oldLevel);   /* now ln97xInt won't get confused */

    /* Advance our management index */

    pDrvCtrl->tmdIndex = (pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1);

    /* kick start the transmitter, if selected */

    if (lnKickStartTx){
            ln97xCsrWrite (pDrvCtrl, 0, CSR0_TDMD);
	}

    return (OK);
}

/********************************************************************
 * 函数名:   taNetClose
 --------------------------------------------------------------------
 * 功能:     关闭网络设备。

 ********************************************************************/
void taNetClose(void)
{
    fnTA_net_close();
}

/********************************************************************
 * 函数名:   taNetIsr
 --------------------------------------------------------------------
 * 功能:     网卡中断服务程序
 --------------------------------------------------------------------
 ********************************************************************/
T_VOID taNetIsr(T_UBYTE minor,callbackhandler callback)
{
    fnTA_net_isr(callback);
}

/********************************************************************
 * 函数名:   taNetEnableRxInt
 --------------------------------------------------------------------
 * 功能:     使能/禁止网卡接收中断

 ********************************************************************/
T_VOID taNetEnableRxInt(T_UBYTE minor,T_BOOL isEnable)
{
    fnTA_net_enable_rx_int(isEnable);
}

/********************************************************************
 * 函数名:   taNetGetRxVector
 --------------------------------------------------------------------
 * 功能:     获得网卡的接收中断号。

 ********************************************************************/
T_UBYTE taNetGetRxVector(T_UBYTE minor)
{
    return fnTA_net_get_rx_vector();
}
