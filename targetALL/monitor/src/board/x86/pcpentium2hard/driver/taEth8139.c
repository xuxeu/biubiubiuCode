/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                         创建该文件。
 */

/*
 * @file: taEth8139.c
 * @brief:
 *             <li>实现网卡驱动。该网卡为目标板上的网卡，使用的8139芯片</li>
 */
 
/************************头 文 件******************************/
#include "taTypes.h"
#include "taPci.h"
#include "taEth8139.h"
#include "taIoAccess.h"
#include "ta.h"

/************************宏 定 义********************************/

/* 中断向量号的基，在汇编中使用*/
#define BSP_ASM_IRQ_VECTOR_BASE     0

#define PCI_MAX_BUS 255
#define PCI_MAX_DEVICE 32
#define PCI_MAX_FUNCTION 8

#define out_b  BSP_OutByte
#define out_w  BSP_OutWord
#define out_l  BSP_OutLong

/* 位于以太帧头部之前的状态字部分数据的长度 */
#define TA_ETHER_DATA_OFFSET    (0)

#define CONFIG_CMD(bus, device_fn, where)   (0x80000000 | (bus << 16) | (device_fn << 8) | (where & ~3))

#define IOADDR(devfn, where)   ((0xC000 | ((devfn & 0x78) << 5)) + where)
#define FUNC(devfn)            (((devfn & 7) << 1) | 0xf0)

/************************类型定义*******************************/

/************************外部声明*******************************/

/************************前向声明*******************************/

/************************模块变量*******************************/
static unsigned short rtl8139_vendor = 0x10ec;
static unsigned short rtl8139_device = 0x8139;
static unsigned char tx_entry = 0;
static UINT32 net_int_vector;
static unsigned char rtl8139_rcv_buf[RX_BUF_LEN + 16];
static unsigned int rtl8139_rcv_curpos;
static unsigned long rtl8139_iobase;

/************************全局变量*******************************/

/************************函数实现*******************************/
static inline UINT8 in_b(UINT32 addr )
{
    UINT8 v;
    BSP_InByte(addr,v);

    return v;
}

static inline UINT16 in_w(UINT32 addr )
{
    UINT16 v;
    BSP_InWord(addr,v);

    return v;
}

static inline UINT32 in_l(UINT32 addr )
{
    UINT32 v;
    BSP_InLong(addr,v);

    return v;
}

/* 检查PCI配置 */
static int check_direct_pci(void)
{
    T_UWORD tmp;
    T_UBYTE tempb1,tempb2;
    T_UWORD templ1;

    /*
    * 检测1类配置是否工作
    */
    BSP_OutByte (0xCFB, 0x01);
    BSP_InLong(0xCF8,tmp);
    BSP_OutLong (0xCF8, 0x80000000);
    BSP_InLong(0xCF8,templ1);
    if (templ1 == 0x80000000)
    {
        BSP_OutLong (0xCF8, tmp);

        return PCI_PROBE_CONF1;
    }

    BSP_OutLong (0xCF8, tmp);

    /*
    * 检测2类配置是否工作
    */
    BSP_OutByte (0xCFB, 0x00);
    BSP_OutByte (0xCF8,0x00);
    BSP_OutByte (0xCFA, 0x00);

    BSP_InByte(0xCF8,tempb1);
    BSP_InByte(0xCFB,tempb2);
    if (tempb1 == 0x00 && tempb2 == 0x00)
    {
        return PCI_PROBE_CONF2;
    }

    return PCI_OP_FAILED;
}

/************************************************************
*模块: pci_conf1_read_config_byte
*
*功能：从指定设备中寄存器读取字节（寄存器字节读）
*
*参数
* 输入参数： bus    总线号
*    device_fn  设备功能号
*    value   读取的字节值
* 输出参数： where   寄存器号
*  返回：  成功   PCIBIOS_SUCCESSFUL
*    失败
*
************************************************************/
static T_WORD pci_conf1_read_config_byte( T_UBYTE bus,T_UBYTE device_fn,T_UBYTE where,T_UBYTE *value )
{
    BSP_OutLong(0xCF8, CONFIG_CMD(bus,device_fn,where));

    BSP_InByte((0xCFC + (where&3)),*value);

    return PCIBIOS_SUCCESSFUL;
}

/************************************************************
*模块: pci_conf1_read_config_dword
*
*功能：从指定设备中寄存器读取双字（寄存器双字读）
*
*参数
* 输入参数： bus    总线号
*    device_fn  设备功能号
*    where   寄存器号
* 输出参数： value   读取的双字值
*  返回：  成功   PCIBIOS_SUCCESSFUL
*    失败   PCIBIOS_BAD_REGISTER_NUMBER
*
************************************************************/
static T_WORD pci_conf1_read_config_dword( T_UBYTE bus,T_UBYTE device_fn,T_UBYTE where,T_UWORD *value )
{
    BSP_OutLong(0xCF8, CONFIG_CMD(bus,device_fn,where));

    BSP_InLong(0xCFC,*value);

    return PCIBIOS_SUCCESSFUL;
}

/************************************************************
*模块: pci_conf2_read_config_byte
*
*功能：从指定设备中寄存器读取字节字（寄存器字节读）
*
*参数
* 输入参数： bus    总线号
*    device_fn  设备功能号
*    where   寄存器号
* 输出参数： value   读取的字节值
*  返回：  成功   PCIBIOS_SUCCESSFUL
*    失败   PCIBIOS_DEVICE_NOT_FOUND
*
************************************************************/
static T_WORD pci_conf2_read_config_byte( T_UBYTE bus,T_UBYTE device_fn,T_UBYTE where,T_UBYTE *value )
{
    BSP_OutByte (0xCF8, FUNC(device_fn));
    BSP_OutByte (0xCFA, bus);
    BSP_InByte(IOADDR(device_fn,where),*value);
    BSP_OutByte (0xCF8, 0);

    return PCIBIOS_SUCCESSFUL;
}

/************************************************************
*模块: pci_conf2_read_config_dword
*
*功能：从指定设备中寄存器读取双字（寄存器双字读）
*
*参数
* 输入参数： bus    总线号
*    device_fn  设备功能号
*    where   寄存器号
* 输出参数： value   读取的双字值
*  返回：  成功   PCIBIOS_SUCCESSFUL
*    失败   PCIBIOS_DEVICE_NOT_FOUND
*
************************************************************/
static T_WORD pci_conf2_read_config_dword( T_UBYTE bus,T_UBYTE device_fn,T_UBYTE where,T_UWORD *value )
{
    BSP_OutByte (0xCF8, FUNC(device_fn));
    BSP_OutByte (0xCFA,bus);
    BSP_InLong(IOADDR(device_fn,where),*value);
    BSP_OutByte (0xCF8,0);

    return PCIBIOS_SUCCESSFUL;
}

/************************************************************
*模块: pci_find_device
*
*功能：通过厂商ID和设备ID来获取该设备在系统中的总线号和功能号
*
*参数
* 输入参数：
*     vendor 厂商ID
*     device_id 设备ID
* 	  bus  总线号
*     device_fn  设备功能号
* 输出参数： value   读取的双字值
*返回：  成功   PCIBIOS_SUCCESSFUL
*      失败   PCIBIOS_DEVICE_NOT_FOUND
************************************************************/
static int pci_direct_find_device( T_UHWORD vendor,T_UHWORD device_id,T_UHWORD index,T_UBYTE *bus,T_UBYTE *device_fn)
{
	T_UBYTE busNum = 0;
	T_UBYTE dev = 0;
	unsigned int func = 0;
	unsigned int device_fnun = 0;
	unsigned int value = 0;
	unsigned short vendorID = 0;
	unsigned short deviceID = 0;

	for(busNum = 0; busNum < PCI_MAX_BUS; busNum++)
	{
		for(dev = 0; dev < PCI_MAX_DEVICE; dev++)
		{
			for(func = 0; func < PCI_MAX_FUNCTION; func++)
			{
				device_fnun = (dev << 3) + func;

				if(PCI_PROBE_CONF1 == index)
				{
					pci_conf1_read_config_dword(busNum, device_fnun, PCI_VENDOR_ID, &value);
				}
				else
				{
					pci_conf2_read_config_dword(busNum, device_fnun, PCI_VENDOR_ID, &value);
				}

				if(PCI_ANY_ID != value)
				{
					vendorID = (unsigned short)value;
					deviceID = value >> 16;

			        if((vendorID == vendor) && (deviceID == device_id))
			        {
			        	*bus = busNum;
			        	*device_fn = device_fnun;

			        	return PCIBIOS_SUCCESSFUL;
			        }
				}
			}
		}
	}
	return PCIBIOS_DEVICE_NOT_FOUND;
}

/* 数据拷贝函数 */
static void rtl8139_mem_copy( char *dest,char *src,unsigned long count)
{
    while(count--)
        *dest++ = *src++;
}

/*
 * @brief:
 *    打开网络设备
 * @param[in]: minor: 设备号
 * @param[in]: mac: MAC地址
 * @return: 
 *    TRUE: 成功。
 *    FALSE: 失败。
 */ 
T_BOOL taNetOpen(T_UBYTE minor, UINT8 * mac)
{
	int i;
	int full_duplex;
	unsigned short lpar;
	int pci_index = 0;
	int pciConfig = 0;
	unsigned char pci_bus, pci_device_fn;
	unsigned short vendor, device;
	unsigned char pci_irq_line;

	/* 检查PCI配置 */
	pciConfig = check_direct_pci();

	if(PCI_PROBE_CONF1 == pciConfig)
	{
		if(pci_direct_find_device(rtl8139_vendor, rtl8139_device, PCI_PROBE_CONF1, &pci_bus, &pci_device_fn)
			!= PCIBIOS_SUCCESSFUL)
		{
			return FALSE;
		}
        
		pci_conf1_read_config_dword(pci_bus, pci_device_fn,
							  PCI_BASE_ADDRESS_0, (unsigned int *)&rtl8139_iobase);
		pci_conf1_read_config_byte(pci_bus, pci_device_fn,
							 PCI_INTERRUPT_LINE, &pci_irq_line);
	}
	else if(PCI_PROBE_CONF2 == pciConfig)
	{
		if(pci_direct_find_device(rtl8139_vendor, rtl8139_device, PCI_PROBE_CONF2, &pci_bus, &pci_device_fn)
				!= PCIBIOS_SUCCESSFUL)
		{
			return FALSE;
		}

		pci_conf2_read_config_dword(pci_bus, pci_device_fn,
							  PCI_BASE_ADDRESS_0, (unsigned int *)&rtl8139_iobase);
		pci_conf2_read_config_byte(pci_bus, pci_device_fn,
							 PCI_INTERRUPT_LINE, &pci_irq_line);
	}
	else
	{
		return FALSE;
	}
    
	net_int_vector = BSP_ASM_IRQ_VECTOR_BASE + pci_irq_line ;
		 
	/* Remove I/O space marker in bit 0. */
	rtl8139_iobase &= PCI_BASE_ADDRESS_IO_MASK;
	
	/* Bring the chip out of low-power mode. */
    out_b(rtl8139_iobase + R8139_CONFIG1,0x00);

    for (i = 0; i < 6; i++)
        mac[i] = in_b(rtl8139_iobase + R8139_IDR0 + i);

    /* Put the chip into low-power mode. */
    out_b(rtl8139_iobase + R8139_9346CR,0xC0);

    out_b(rtl8139_iobase + R8139_CONFIG1,0x03);

    out_b(rtl8139_iobase + R8139_CR, 0x10);          /* reset the chip */

    for (i = 5000; i > 0; i--)
    if ((in_b(rtl8139_iobase + R8139_CR) & 0x10) == 0)
        break;

	//write mac address
    out_w(rtl8139_iobase + R8139_BMCR,in_w(rtl8139_iobase + R8139_BMCR)|0x1300);

    for (i = 0; i < 6; i++)
        out_b(rtl8139_iobase + R8139_IDR0  + i, mac[i] );
	
    while ((in_w(rtl8139_iobase + R8139_BMSR) & 0x20) == 0);

    /* Must enable Tx/Rx before setting transfer thresholds! */
    out_b(rtl8139_iobase + R8139_CR,0x0c);/*enable tx and rx*/

    out_l(rtl8139_iobase + R8139_RCR,(RX_FIFO_THRESH << 13) | (RX_BUF_LEN_IDX << 11) | (RX_DMA_BURST<<8)|RX_OPTION);

    out_l(rtl8139_iobase + R8139_TCR,(TX_DMA_BURST<<8)|0x03000000);


    lpar = in_w(rtl8139_iobase + R8139_ANLPAR);
    if ((lpar & 0x0100) == 0x0100 || (lpar & 0x00C0) == 0x0040)
		full_duplex = 1;
    else
		full_duplex = 0;

    out_b(rtl8139_iobase + R8139_9346CR,0xC0 );

    out_b(rtl8139_iobase + R8139_CONFIG1,full_duplex ? 0x60 : 0x20 );

    out_b(rtl8139_iobase + R8139_9346CR, 0x00);

    out_l(rtl8139_iobase + R8139_RBSTART,(unsigned long)rtl8139_rcv_buf);

    /* Start the chip's Tx and Rx process. */
    out_l(rtl8139_iobase + R8139_MPC,0);

    out_b(rtl8139_iobase + R8139_CR, 0x0c );/*enable rx and tx*/

    /* Disable all known interrupts by setting the interrupt mask. */
    out_w(rtl8139_iobase + R8139_ISR,0xff);
    
    out_w(rtl8139_iobase + R8139_IMR,0);

    rtl8139_rcv_curpos = 0;
    tx_entry = 0;

   return TRUE;
}

/********************************************************************
 * 函数名:   taNetClose
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
void taNetClose(void)
{
	/* Disable interrupts by clearing the interrupt mask. */
	out_w(rtl8139_iobase+R8139_IMR,0x0000);

	/* Stop the chip's Tx and Rx DMA processes. */
	out_b(rtl8139_iobase+R8139_CR,0x00);

	/* Update the error counts. */
	out_l(rtl8139_iobase+R8139_MPC,0);

	/* Green! Put the chip in low-power mode. */
	out_b(rtl8139_iobase+R8139_9346CR,0xC0);
	out_b(rtl8139_iobase+R8139_CONFIG1,0x03);
}

/********************************************************************
 * 函数名:   taNetIsr
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
T_VOID taNetIsr(T_UBYTE minor, callbackhandler callback)
{
    unsigned short isr;
       
     /*获得中断状态字*/
    isr = in_w(rtl8139_iobase+R8139_ISR);
    
    /*清除中断状态字*/
   out_w(rtl8139_iobase + R8139_ISR,0xff);

    /*判断是否是正确的收到一个帧*/
    if((isr & RxOK) && callback)
    {
        callback();
    }
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
    unsigned char *rx_ring = rtl8139_rcv_buf;
    unsigned short cur_rx = rtl8139_rcv_curpos ;
    int rx_count = 0;
    int rx_size = 0;
    
    while (((in_b(rtl8139_iobase + R8139_CR) & 1)==0)&&(rx_count++<10))
    {
        int ring_offset = cur_rx % RX_BUF_LEN;
        unsigned long rx_status = *(unsigned long*)(rx_ring + ring_offset);

        rx_size = rx_status >> 16; 

        if (rx_status&(RxBadSymbol|RxRunt|RxTooLong|RxCRCErr|RxBadAlign))
        {
            /* Reset the receiver, based on RealTek recommendation. (Bug?) */
            out_b(rtl8139_iobase+R8139_CR,0x04);   /*enable tx*/
            out_b(rtl8139_iobase+R8139_CR,0x0c);  /*enable tx and rx*/
            out_l(rtl8139_iobase+R8139_RCR,
                (RX_FIFO_THRESH<<13)|(RX_BUF_LEN_IDX<<11)|(RX_DMA_BURST<<8)|RX_OPTION);

            cur_rx += rx_size + 4;
            cur_rx = (cur_rx + 3) & ~3;
            out_w(rtl8139_iobase+R8139_CAPR,cur_rx-16);
          
            continue;
        }
        else
        {
            /* Malloc up new buffer, compatible with net-2e. */
            /* Omit the four octet CRC from the length. */
            if((rx_size<64)||(rx_size>1518))
            {
                cur_rx = in_w(rtl8139_iobase+R8139_CBR) % RX_BUF_LEN;
                out_w(rtl8139_iobase+R8139_CAPR,cur_rx-16);
               
                continue;
            }

            if (ring_offset+rx_size> RX_BUF_LEN)
            {
                int semi_count = RX_BUF_LEN - ring_offset - 4;
                int semi_count2 = rx_size-4-semi_count;
                if(semi_count>0)
                {
                    rtl8139_mem_copy(buf,&rx_ring[ring_offset + 4], semi_count);
                    rtl8139_mem_copy(buf+semi_count , rx_ring, semi_count2);
                }
                else
                {
                    rtl8139_mem_copy(buf , &rx_ring[-semi_count], rx_size-4);
                }
            }
            else
            {
                rtl8139_mem_copy(buf, &rx_ring[ring_offset + 4], rx_size - 4);
            }
        }

        cur_rx += rx_size + 4;
        cur_rx = (cur_rx + 3) & ~3;
        out_w(rtl8139_iobase+R8139_CAPR,cur_rx-16);
		
        break;
     } //end of while

    rtl8139_rcv_curpos = cur_rx ;

    out_w(rtl8139_iobase + R8139_ISR,0xff);
    
    *offset = TA_ETHER_DATA_OFFSET;
	
    return rx_size;
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
    out_l(rtl8139_iobase+R8139_TSAD0 + tx_entry *4,(unsigned long)buf);
    out_l(rtl8139_iobase+R8139_TSD0 + tx_entry*4,    ((TX_FIFO_THRESH<<11) & 0x003f0000)|(cnt >= ETH_ZLEN ? cnt : ETH_ZLEN));

   while ( !(in_l(rtl8139_iobase+R8139_TSD0 + tx_entry*4)&0x8000) );

   tx_entry++;
   tx_entry = tx_entry % 4 ;
}

/********************************************************************
 * 函数名:   taNetEnableRxInt
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
T_VOID taNetEnableRxInt(T_UBYTE minor,T_BOOL isEnable)
{
    if(isEnable)
    {	
      	/*清除中断状态寄存器*/
      	out_w(rtl8139_iobase + R8139_ISR,0xff);
    
      	/*允许接收中断*/
      	out_w(rtl8139_iobase + R8139_IMR,RxOK);
    }
    else
    {
        /*禁止接收中断*/
         out_w(rtl8139_iobase + R8139_IMR,0);
     }
    out_l(rtl8139_iobase+R8139_RCR,
                (RX_FIFO_THRESH<<13)|(RX_BUF_LEN_IDX<<11)|(RX_DMA_BURST<<8)|RX_OPTION);
}

/********************************************************************
 * 函数名:   taNetGetRxVector
 --------------------------------------------------------------------
 * 功能:     获得网卡的接收中断号
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
T_UBYTE taNetGetRxVector(T_UBYTE minor)
{
    return net_int_vector;
}
