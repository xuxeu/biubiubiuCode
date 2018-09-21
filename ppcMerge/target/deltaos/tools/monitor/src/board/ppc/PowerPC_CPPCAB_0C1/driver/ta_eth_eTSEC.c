/*
 * Freescale Three Speed Ethernet Controller driver
 *
 * This software may be used and distributed according to the
 * terms of the GNU Public License, Version 2, incorporated
 * herein by reference.
 *
 * Copyright 2004-2009 Freescale Semiconductor, Inc.
 * (C) Copyright 2003, Motorola, Inc.
 * author Andy Fleming
 *
 */


//#include <ta.h>
//#include <kbsp.h>
//#include <stdio.h>
//#include <string.h>

#include "taTypes.h"
#include "ta8640tsec.h"



//DECLARE_GLOBAL_DATA_PTR;

#define TX_BUF_CNT 2
//#define PKTBUFSRX 4
#define PKTBUFSRX 32
#define EIEIO_SYNC __asm__ ("eieio; sync")

#define CONFIG_TSEC_NAME                "eTSEC1"
#define TSEC_CONFIG           (1)
#define TSEC_PHY_ADDR            (3)
#define TSEC_FLAGS     (0)

#define PACKET_SIZE (2048)

typedef struct txbd8
{
    ushort       status;         /* Status Fields */
    ushort       length;         /* Buffer length */
    uint         bufPtr;         /* Buffer Pointer */
} txbd8_t;

typedef struct rxbd8
{
    ushort       status;         /* Status Fields */
    ushort       length;         /* Buffer Length */
    uint         bufPtr;         /* Buffer Pointer */
} rxbd8_t;

typedef volatile struct rtxbd {
    txbd8_t txbd[TX_BUF_CNT];
    rxbd8_t rxbd[PKTBUFSRX];
} RTXBD;

enum eth_state_t {
    ETH_STATE_INIT,
    ETH_STATE_PASSIVE,
    ETH_STATE_ACTIVE
};



//extern T_CHAR ethShareRamSpaceStart[];

int eth_clear_rx_event(void);

int eth_initialize(unsigned char *mac);
int eth_init(void);
void eth_halt(void);
int eth_send(volatile void *packet, int length);
int eth_rx(char *packet_buff);
static int tsec_send(struct eth_device *dev,
             volatile void *packet, int length);
static int tsec_recv(struct eth_device *dev, unsigned char *packet_buff);
static int tsec_init(struct eth_device *dev);
static int tsec_initialize(struct tsec_info_struct *tsec_info);
static void tsec_halt(struct eth_device *dev);
static void init_registers(volatile tsec_t * regs);
static void startup_tsec(struct eth_device *dev);
static int init_phy(struct eth_device *dev);
void write_phy_reg(struct tsec_private *priv, uint regnum, uint value);
uint read_phy_reg(struct tsec_private *priv, uint regnum);
static struct phy_info *get_phy_info(struct eth_device *dev);
static void phy_run_commands(struct tsec_private *priv, struct phy_cmd *cmd);
static void adjust_link(struct eth_device *dev);
//static int tsec_miiphy_write(char *devname, unsigned char addr,
//               unsigned char reg, unsigned short value);
//static int tsec_miiphy_read(char *devname, unsigned char addr,
//              unsigned char reg, unsigned short *value);
static uint mii_parse_sr(uint mii_reg, struct tsec_private * priv);
static uint mii_parse_dp83865_lanr(uint mii_reg, struct tsec_private *priv);
static uint mii_parse_link(uint mii_reg, struct tsec_private *priv);

static struct tsec_private *m_priv_addr;

static uint m_rxIdx;        /* index of the current RX buffer */
static uint m_txIdx;        /* index of the current TX buffer */

static RTXBD __attribute__ ((aligned(8))) m_rtx;

/* Default initializations for TSEC controllers. */
static struct tsec_info_struct m_tsec_info[] = 
{
    {           \
        .regs = (tsec_t *)(TSEC_BASE_ADDR + ((TSEC_CONFIG - 1) * TSEC_SIZE)),
        .miiregs = (tsec_mdio_t *)(MDIO_BASE_ADDR),
        .miiregs_sgmii = (tsec_mdio_t *)(MDIO_BASE_ADDR
                         + (TSEC_CONFIG - 1) * TSEC_MDIO_OFFSET),
        .devname = CONFIG_TSEC_NAME,
        .phyaddr = TSEC_PHY_ADDR,
        .flags = TSEC_FLAGS
    },  /* TSEC/FEC */
};

static unsigned long m_mac_addr1,m_mac_addr2;

static struct eth_device m_dev,*m_eth_current;
static struct tsec_private m_priv;

volatile static UINT32 NetRxPackets[PKTBUFSRX][PACKET_SIZE] __attribute__ ((aligned(0x1000)));//[PACKET_SIZE];

uchar *NetRxPacketsPtr = NetRxPackets;

static struct phy_info m_phy_info_dp83865 = {
    0x20005c7,
    "NatSemi DP83865",
    4,
    (struct phy_cmd[]) {    /* config */
        {MIIM_CONTROL,MIIM_DP83865_CR_INIT, NULL},//  0x0100 MIIM_DP83865_CR_INIT 0xA100,
        {miim_end,}
    },
    (struct phy_cmd[]) {    /* startup */
        /* Status is read once to clear old link state */
        {MIIM_STATUS, miim_read, NULL},
        /* Auto-negotiate */
        {MIIM_STATUS, miim_read, &mii_parse_sr},
        /* Read the link and auto-neg status */
        {MIIM_DP83865_LANR, miim_read, &mii_parse_dp83865_lanr},
        {miim_end,}
    },
    (struct phy_cmd[]) {    /* shutdown */
        {miim_end,}
    },
};

/*
 * Returns which value to write to the control register.
 * For 10/100, the value is slightly different
 */
static uint mii_cr_init(uint mii_reg, struct tsec_private * priv)
{
	if (priv->flags & TSEC_GIGABIT)
		return MIIM_CONTROL_INIT;
	else
		return MIIM_CR_INIT;
}

/* Parse the vsc8244's status register for speed and duplex
 * information
 */
static uint mii_parse_vsc8244(uint mii_reg, struct tsec_private * priv)
{
	uint speed;

	if (mii_reg & MIIM_VSC8244_AUXCONSTAT_DUPLEX)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	speed = mii_reg & MIIM_VSC8244_AUXCONSTAT_SPEED;
	switch (speed) {
	case MIIM_VSC8244_AUXCONSTAT_GBIT:
		priv->speed = 1000;
		break;
	case MIIM_VSC8244_AUXCONSTAT_100:
		priv->speed = 100;
		break;
	default:
		priv->speed = 10;
		break;
	}

	return 0;
}

static struct phy_info phy_info_VSC8244 = {
	0x3f1b,
	"VSC8244",
	6,
	(struct phy_cmd[]) {	/* config */
		/* Override PHY config settings */
		/* Configure some basic stuff */
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Read the Status (2x to make sure link is right) */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_VSC8244_AUX_CONSTAT, miim_read, &mii_parse_vsc8244},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

/* 配置PHY的结构体 */
struct phy_info m_phy_info_generic =  {
    0,
    "Unknown/Generic PHY",
    32,
    (struct phy_cmd[]) { /* config */
        {PHY_BMCR, PHY_BMCR_RESET, NULL},
        {PHY_BMCR, PHY_BMCR_AUTON|PHY_BMCR_RST_NEG, NULL},
        {miim_end,}
    },
    (struct phy_cmd[]) { /* startup */
        {PHY_BMSR, miim_read, NULL},
        {PHY_BMSR, miim_read, &mii_parse_sr},
        {PHY_BMSR, miim_read, &mii_parse_link},
        {miim_end,}
    },
    (struct phy_cmd[]) { /* shutdown */
        {miim_end,}
    }
};

static struct phy_info *m_phy_info[] = {
    &m_phy_info_dp83865,
	&phy_info_VSC8244,
    &m_phy_info_generic,    /* must be last; has ID 0 and 32 bit mask */
    NULL
};


static void udelay(unsigned long usec)
{  
    //不准确延时
    unsigned long temp =10;
    int i = 0;
    int j = 0;
    for(i = 0;i<usec;i++)
    {
        for(j=0; j<temp;j++);
     }

}

/* Initialize device structure. Returns success if PHY
 * initialization succeeded (i.e. if it recognizes the PHY)
 */
static int tsec_initialize(struct tsec_info_struct *tsec_info)
{
    struct eth_device *dev;
    int i;
    struct tsec_private *priv;

    dev = &m_dev;
    m_eth_current = &m_dev;

    priv = &m_priv;

    m_priv_addr = priv;
    priv->regs = tsec_info->regs;
    priv->phyregs = tsec_info->miiregs;
    priv->phyregs_sgmii = tsec_info->miiregs_sgmii;

    priv->phyaddr = tsec_info->phyaddr;
    priv->flags = tsec_info->flags;

    strcpy(dev->name, tsec_info->devname);
    dev->iobase = 0;
    dev->priv = priv;
    dev->init = tsec_init;
    dev->halt = tsec_halt;
    dev->send = tsec_send;
    dev->recv = tsec_recv;

    /* Tell u-boot to get the addr from the env */
    for (i = 0; i < 6; i++)
        dev->enetaddr[i] = 0;

    /* Reset the MAC */
    priv->regs->maccfg1 = MACCFG1_SOFT_RESET;
    udelay(2);  /* Soft Reset must be asserted for 3 TX clocks */
    priv->regs->maccfg1 &= ~(MACCFG1_SOFT_RESET);

    /* Try to initialize PHY here, and return */
    return init_phy(dev);
}

/* Initializes data structures and registers for the controller,
 * and brings the interface up.  Returns the link status, meaning
 * that it returns success if the link is up, failure otherwise.
 * This allows u-boot to find the first active controller.
 */
static int tsec_init(struct eth_device *dev)
{
    struct tsec_private *priv = (struct tsec_private *)dev->priv;
    volatile tsec_t *regs = priv->regs;

    /* Make sure the controller is stopped */
    tsec_halt(dev);

    /* Init MACCFG2.  Defaults to 10M RMII */
    regs->maccfg2 = 0x00007105;//0x00007105;//MACCFG2_INIT_SETTINGS;

    /* Init ECNTRL */
    regs->ecntrl = 0x00001018;//0x0001018;//0x00001040//ECNTRL_INIT_SETTINGS

    /*set mac*/
    regs->macstnaddr1 = m_mac_addr1;
    regs->macstnaddr2 = m_mac_addr2;

    /* reset the indices to zero */
    m_rxIdx = 0;
    m_txIdx = 0;

    /* Clear out (for the most part) the other registers */
    init_registers(regs);

    /* Ready the device for tx/rx */
    startup_tsec(dev);

    /* If there's no link, fail */
    return (priv->link ? 0 : -1);
}

/* Writes the given phy's reg with value, using the specified MDIO regs */
static void tsec_local_mdio_write(volatile tsec_mdio_t *phyregs, uint addr,
        uint reg, uint value)
{
    int timeout = 1000000;

    phyregs->miimadd = (addr << 8) | reg;
    phyregs->miimcon = value;
    asm("sync");

    timeout = 1000000;
    while ((phyregs->miimind & MIIMIND_BUSY) && timeout--) ;
}

/* Provide the default behavior of writing the PHY of this ethernet device */
#define write_phy_reg(priv, regnum, value) \
    tsec_local_mdio_write(priv->phyregs,priv->phyaddr,regnum,value)

/* Reads register regnum on the device's PHY through the
 * specified registers.  It lowers and raises the read
 * command, and waits for the data to become valid (miimind
 * notvalid bit cleared), and the bus to cease activity (miimind
 * busy bit cleared), and then returns the value
 */
static uint tsec_local_mdio_read(volatile tsec_mdio_t *phyregs,
                uint phyid, uint regnum)
{
    uint value;

    /* Put the address of the phy, and the register
     * number into MIIMADD */
    phyregs->miimadd = (phyid << 8) | regnum;

    /* Clear the command register, and wait */
    phyregs->miimcom = 0;
    asm("sync");

    /* Initiate a read command, and wait */
    phyregs->miimcom = MIIM_READ_COMMAND;
    asm("sync");

    /* Wait for the the indication that the read is done */
    while ((phyregs->miimind & (MIIMIND_NOTVALID | MIIMIND_BUSY))) ;

    /* Grab the value read from the PHY */
    value = phyregs->miimstat;

    return value;
}

/* #define to provide old read_phy_reg functionality without duplicating code */
#define read_phy_reg(priv,regnum) \
    tsec_local_mdio_read(priv->phyregs,priv->phyaddr,regnum)

/* Discover which PHY is attached to the device, and configure it
 * properly.  If the PHY is not recognized, then return 0
 * (failure).  Otherwise, return 1
 */
static int init_phy(struct eth_device *dev)
{
    struct tsec_private *priv = (struct tsec_private *)dev->priv;
    struct phy_info *curphy;
    volatile tsec_t *regs = priv->regs;

    /* Assign a Physical address to the TBI */
    regs->tbipa = CONFIG_SYS_TBIPA_VALUE;
    asm("sync");

    /* Reset MII (due to new addresses) */
    priv->phyregs->miimcfg = MIIMCFG_RESET;
    asm("sync");
    priv->phyregs->miimcfg = 0x00000005;//MIIMCFG_INIT_VALUE;
    asm("sync");
    while (priv->phyregs->miimind & MIIMIND_BUSY) ;

    /* Get the cmd structure corresponding to the attached
     * PHY */
    curphy = get_phy_info(dev);

    if (curphy == NULL) {
        priv->phyinfo = NULL;
        printf("%s: No PHY found\n", dev->name);
        return 0;
    }

    //if (regs->ecntrl & ECNTRL_SGMII_MODE)
    //tsec_configure_serdes(priv);

    priv->phyinfo = curphy;

    phy_run_commands(priv, priv->phyinfo->config);

    return 1;
}


/*
 * Wait for auto-negotiation to complete, then determine link
 */
static uint mii_parse_sr(uint mii_reg, struct tsec_private * priv)
{      

    /*
     * Wait if the link is up, and autonegotiation is in progress
     * (ie - we're capable and it's not done)
     */
    mii_reg = read_phy_reg(priv, MIIM_STATUS);
    if ((mii_reg & PHY_BMSR_AUTN_ABLE) && !(mii_reg & PHY_BMSR_AUTN_COMP)) 
    {
        int i = 0;

        printf("Waiting for PHY auto negotiation to complete\n");
        while (!(mii_reg & PHY_BMSR_AUTN_COMP))
        {
            /*
             * Timeout reached ?
             */
            if (i > PHY_AUTONEGOTIATE_TIMEOUT) 
            {
                printf(" TIMEOUT !\n");
                priv->link = 0;
                return 0;
            }

            //if (ctrlc()) {
            //  puts("user interrupt!\n");
            //  priv->link = 0;
            //  return -EINTR;
            //}

            if ((i++ % 1000) == 0) 
            {
                printf(".");
            }
            udelay(200);    /*wait */
            mii_reg = read_phy_reg(priv, MIIM_STATUS);
        }
        printf(" done\n");

        /* Link status bit is latched low, read it again */
        mii_reg = read_phy_reg(priv, MIIM_STATUS);
        
        udelay(500);    /* another 500 ms (results in faster booting) */
    }

    priv->link = mii_reg & MIIM_STATUS_LINK ? 1 : 0;

    return 0;
}

/* Generic function which updates the speed and duplex.  If
 * autonegotiation is enabled, it uses the AND of the link
 * partner's advertised capabilities and our advertised
 * capabilities.  If autonegotiation is disabled, we use the
 * appropriate bits in the control register.
 *
 * Stolen from Linux's mii.c and phy_device.c
 */
static uint mii_parse_link(uint mii_reg, struct tsec_private *priv)
{
    /* We're using autonegotiation */
    if (mii_reg & PHY_BMSR_AUTN_ABLE) 
    {
        uint lpa = 0;
        uint gblpa = 0;

        /* Check for gigabit capability */
        if (mii_reg & PHY_BMSR_EXT) 
        {
            /* We want a list of states supported by
             * both PHYs in the link
             */
            gblpa = read_phy_reg(priv, PHY_1000BTSR);
            gblpa &= read_phy_reg(priv, PHY_1000BTCR) << 2;
        }

        /* Set the baseline so we only have to set them
         * if they're different
         */
        priv->speed = 10;
        priv->duplexity = 0;

        /* Check the gigabit fields */
        if (gblpa & (PHY_1000BTSR_1000FD | PHY_1000BTSR_1000HD)) 
        {
            priv->speed = 1000;

            if (gblpa & PHY_1000BTSR_1000FD)
                priv->duplexity = 1;

            /* We're done! */
            return 0;
        }

        lpa = read_phy_reg(priv, PHY_ANAR);
        lpa &= read_phy_reg(priv, PHY_ANLPAR);

        if (lpa & (PHY_ANLPAR_TXFD | PHY_ANLPAR_TX)) 
        {
            priv->speed = 100;

            if (lpa & PHY_ANLPAR_TXFD)
            {
                priv->duplexity = 1;
            }

        }
        else if (lpa & PHY_ANLPAR_10FD)
        {
            priv->duplexity = 1;
        }
    }
    else 
    {
        uint bmcr = read_phy_reg(priv, PHY_BMCR);

        priv->speed = 10;
        priv->duplexity = 0;

        if (bmcr & PHY_BMCR_DPLX)
            priv->duplexity = 1;

        if (bmcr & PHY_BMCR_1000_MBPS)
            priv->speed = 1000;
        else if (bmcr & PHY_BMCR_100_MBPS)
            priv->speed = 100;
    }

    return 0;
}

/* Initialized required registers to appropriate values, zeroing
 * those we don't care about (unless zero is bad, in which case,
 * choose a more appropriate value)
 */
static void init_registers(volatile tsec_t * regs)
{
    /* Clear IEVENT */
    regs->ievent = IEVENT_INIT_CLEAR;

    regs->imask = IMASK_INIT_CLEAR;

    regs->hash.iaddr0 = 0;
    regs->hash.iaddr1 = 0;
    regs->hash.iaddr2 = 0;
    regs->hash.iaddr3 = 0;
    regs->hash.iaddr4 = 0;
    regs->hash.iaddr5 = 0;
    regs->hash.iaddr6 = 0;
    regs->hash.iaddr7 = 0;

    regs->hash.gaddr0 = 0;
    regs->hash.gaddr1 = 0;
    regs->hash.gaddr2 = 0;
    regs->hash.gaddr3 = 0;
    regs->hash.gaddr4 = 0;
    regs->hash.gaddr5 = 0;
    regs->hash.gaddr6 = 0;
    regs->hash.gaddr7 = 0;

    regs->rctrl = 0x00000000;

    /* Init RMON mib registers */
    memset((void *)&(regs->rmon), 0, sizeof(rmon_mib_t));

    regs->rmon.cam1 = 0xffffffff;
    regs->rmon.cam2 = 0xffffffff;

    regs->mrblr = MRBLR_INIT_SETTINGS;

    regs->minflr = MINFLR_INIT_SETTINGS;

    regs->attr = ATTR_INIT_SETTINGS;
    regs->attreli = ATTRELI_INIT_SETTINGS;

}

/* Configure maccfg2 based on negotiated speed and duplex
 * reported by PHY handling code
 */
static void adjust_link(struct eth_device *dev)
{
    struct tsec_private *priv = (struct tsec_private *)dev->priv;
    volatile tsec_t *regs = priv->regs;

    if (priv->link) {
        if (priv->duplexity != 0)
            regs->maccfg2 |= MACCFG2_FULL_DUPLEX;
        else
            regs->maccfg2 &= ~(MACCFG2_FULL_DUPLEX);

        switch (priv->speed) {
        case 1000:
            regs->maccfg2 = ((regs->maccfg2 & ~(MACCFG2_IF))
                     | MACCFG2_GMII);
            break;
        case 100:
        case 10:
            regs->maccfg2 = ((regs->maccfg2 & ~(MACCFG2_IF))
                     | MACCFG2_MII);

            /* Set R100 bit in all modes although
             * it is only used in RGMII mode
             */
            if (priv->speed == 100)
                regs->ecntrl |= ECNTRL_R100;
            else
                regs->ecntrl &= ~(ECNTRL_R100);
            break;
        default:
            printf("%s: Speed was bad\n", dev->name);
            break;
        }

        printf("Speed: %d, %s duplex%s\n", priv->speed,
               (priv->duplexity) ? "full" : "half",
               (priv->flags & TSEC_FIBER) ? ", fiber mode" : "");

    } else {
        printf("%s: No link.\n", dev->name);
    }
}

/* Set up the buffers and their descriptors, and bring up the
 * interface
 */
static void startup_tsec(struct eth_device *dev)
{
    int i;
    struct tsec_private *priv = (struct tsec_private *)dev->priv;
    volatile tsec_t *regs = priv->regs;

    /* Point to the buffer descriptors */
    regs->tbase = (unsigned int)(&m_rtx.txbd[m_txIdx]);
    regs->rbase = (unsigned int)(&m_rtx.rxbd[m_rxIdx]);

    /* 下面使用了ethShareRamSpaceStart开始的(TX_BUF_CNT+PKTBUFSRX)*PACKET_SIZE长度的空间 */

    /* Initialize the Rx Buffer descriptors */
    for (i = 0; i < PKTBUFSRX; i++) {
        //NetRxPackets[i] = (UINT32)ethShareRamSpaceStart +(i * PACKET_SIZE);
        m_rtx.rxbd[i].status = (RXBD_EMPTY | RXBD_INTERRUPT);
        m_rtx.rxbd[i].length = 0;
        m_rtx.rxbd[i].bufPtr = (uint) NetRxPackets[i];
    }
    m_rtx.rxbd[PKTBUFSRX - 1].status |= RXBD_WRAP;

    /* Initialize the TX Buffer Descriptors */
    for (i = 0; i < TX_BUF_CNT; i++) {
        m_rtx.txbd[i].status = 0;
        m_rtx.txbd[i].length = 0;
        m_rtx.txbd[i].bufPtr = 0;
    }
    m_rtx.txbd[TX_BUF_CNT - 1].status |= TXBD_WRAP;

    /* Start up the PHY */
    if(priv->phyinfo)
        phy_run_commands(priv, priv->phyinfo->startup);

    adjust_link(dev);

    /* Enable Transmit and Receive */
    regs->maccfg1 |= (MACCFG1_RX_EN | MACCFG1_TX_EN);

    /* Tell the DMA it is clear to go */
    regs->dmactrl |= DMACTRL_INIT_SETTINGS;
    regs->tstat = TSTAT_CLEAR_THALT;
    regs->rstat = RSTAT_CLEAR_RHALT;
    regs->dmactrl &= ~(DMACTRL_GRS | DMACTRL_GTS);

    EIEIO_SYNC;
}

/* This returns the status bits of the device.  The return value
 * is never checked, and this is what the 8260 driver did, so we
 * do the same.  Presumably, this would be zero if there were no
 * errors
 */
static int tsec_send(struct eth_device *dev, volatile void *packet, int length)
{
    int i;
    int result = 0;
    struct tsec_private *priv = (struct tsec_private *)dev->priv;
    volatile tsec_t *regs = priv->regs;

    /* Find an empty buffer descriptor */
    for (i = 0; m_rtx.txbd[m_txIdx].status & TXBD_READY; i++) {
        if (i >= TOUT_LOOP) {
            printf("%s: tsec: tx buffers full\n", dev->name);
            return result;
        }
    }

    //memcpy((void *)m_rtx.txbd[m_txIdx].bufPtr, (const void *)packet, (size_t)length);

    m_rtx.txbd[m_txIdx].bufPtr = (uint) packet;
    m_rtx.txbd[m_txIdx].length = length;
    m_rtx.txbd[m_txIdx].status |=
        (TXBD_READY | TXBD_LAST | TXBD_CRC);

    /* Tell the DMA to go */
    regs->tstat = TSTAT_CLEAR_THALT;

    /* Wait for buffer to be transmitted */
    for (i = 0; m_rtx.txbd[m_txIdx].status & TXBD_READY; i++) {
        if (i >= TOUT_LOOP) {
            printf("%s: tsec: tx error\n", dev->name);
            return result;
        }
    }

    m_txIdx = (m_txIdx + 1) % TX_BUF_CNT;
    result = m_rtx.txbd[m_txIdx].status & TXBD_STATS;
    
    return result;
}

static int tsec_recv(struct eth_device *dev, unsigned char *packet_buff)
{
    int status = 0;
    int length = 0;
    struct tsec_private *priv = (struct tsec_private *)dev->priv;
    volatile tsec_t *regs = priv->regs;

    while (!(m_rtx.rxbd[m_rxIdx].status & RXBD_EMPTY)) 
    {
        length = m_rtx.rxbd[m_rxIdx].length;

        /* Send the packet up if there were no errors */
        if (!(m_rtx.rxbd[m_rxIdx].status & RXBD_STATS)) 
        {
            memcpy((void *)packet_buff, (const void *)m_rtx.rxbd[m_rxIdx].bufPtr, length);
            status = 1;
        } 
        else 
        {
            length = 0;
            status = 0;
            printf("Got error %x\n",
                   (m_rtx.rxbd[m_rxIdx].status & RXBD_STATS));
        }

        m_rtx.rxbd[m_rxIdx].length = 0;

        /* Set the wrap bit if this is the last element in the list */
        m_rtx.rxbd[m_rxIdx].status =
            RXBD_EMPTY | RXBD_INTERRUPT | (((m_rxIdx + 1) == PKTBUFSRX) ? RXBD_WRAP : 0);

        m_rxIdx = (m_rxIdx + 1) % PKTBUFSRX;

    }

    if (regs->ievent & IEVENT_BSY) {
        regs->ievent = IEVENT_BSY;
        regs->rstat = RSTAT_CLEAR_RHALT;
    }

    eth_clear_rx_event();

    if(status)
    {
           return length;
    }
    else
    {
           return -1;
    }

}

/* Stop the interface */
static void tsec_halt(struct eth_device *dev)
{
      return;
    struct tsec_private *priv = (struct tsec_private *)dev->priv;
    volatile tsec_t *regs = priv->regs;

    regs->dmactrl &= ~(DMACTRL_GRS | DMACTRL_GTS);
    regs->dmactrl |= (DMACTRL_GRS | DMACTRL_GTS);

    while ((regs->ievent & (IEVENT_GRSC | IEVENT_GTSC))
        != (IEVENT_GRSC | IEVENT_GTSC)) ;

    regs->maccfg1 &= ~(MACCFG1_TX_EN | MACCFG1_RX_EN);

    /* Shut down the PHY, as needed */
    if(priv->phyinfo)
        phy_run_commands(priv, priv->phyinfo->shutdown);
}


/* Parse the DP83865's link and auto-neg status register for speed and duplex
 * information
 */
static uint mii_parse_dp83865_lanr(uint mii_reg, struct tsec_private *priv)
{
    switch (mii_reg & MIIM_DP83865_SPD_MASK) {

    case MIIM_DP83865_SPD_1000:
        priv->speed = 1000;
        break;

    case MIIM_DP83865_SPD_100:
        priv->speed = 100;
        break;

    default:
        priv->speed = 10;
        break;

    }

    if (mii_reg & MIIM_DP83865_DPX_FULL)
        priv->duplexity = 1;
    else
        priv->duplexity = 0;

    return 0;
}


/* Grab the identifier of the device's PHY, and search through
 * all of the known PHYs to see if one matches.  If so, return
 * it, if not, return NULL
 */
static struct phy_info *get_phy_info(struct eth_device *dev)
{
    struct tsec_private *priv = (struct tsec_private *)dev->priv;
    uint phy_reg, phy_ID;
    int i;
    struct phy_info *theInfo = NULL;

    /* Grab the bits from PHYIR1, and put them in the upper half */
    phy_reg = read_phy_reg(priv, MIIM_PHYIR1);
    phy_ID = (phy_reg & 0xffff) << 16;

    /* Grab the bits from PHYIR2, and put them in the lower half */
    phy_reg = read_phy_reg(priv, MIIM_PHYIR2);
    phy_ID |= (phy_reg & 0xffff);

    /* loop through all the known PHY types, and find one that */
    /* matches the ID we read from the PHY. */
    for (i = 0; m_phy_info[i]; i++) {
        if (m_phy_info[i]->id == (phy_ID >> m_phy_info[i]->shift)) {
            theInfo = m_phy_info[i];
            break;
        }
    }

    if (theInfo == &m_phy_info_generic) {
        printf("%s: No support for PHY id %x; assuming generic\n",
            dev->name, phy_ID);
    } else {
        printf("%s: PHY is %s (%x)\n", dev->name, theInfo->name, phy_ID);
    }

    return theInfo;
}

/* Execute the given series of commands on the given device's
 * PHY, running functions as necessary
 */
static void phy_run_commands(struct tsec_private *priv, struct phy_cmd *cmd)
{
    int i;
    uint result;
    volatile tsec_mdio_t *phyregs = priv->phyregs;

    phyregs->miimcfg = MIIMCFG_RESET;

    phyregs->miimcfg = MIIMCFG_INIT_VALUE;

    while (phyregs->miimind & MIIMIND_BUSY) ;

    for (i = 0; cmd->mii_reg != miim_end; i++) {
        if (cmd->mii_data == miim_read) {
            result = read_phy_reg(priv, cmd->mii_reg);

            if (cmd->funct != NULL)
                (*(cmd->funct)) (result, priv);

        } else {
            if (cmd->funct != NULL)
                result = (*(cmd->funct)) (cmd->mii_reg, priv);
            else
                result = cmd->mii_data;

            write_phy_reg(priv, cmd->mii_reg, result);

        }
        cmd++;
    }
}

#if 0
/*
 * Read a MII PHY register.
 *
 * Returns:
 *  0 on success
 */
static int tsec_miiphy_read(char *devname, unsigned char addr,
                unsigned char reg, unsigned short *value)
{
    unsigned short ret;
    struct tsec_private *priv = m_priv_addr;

    if (NULL == priv) {
        printf("Can't read PHY at address %d\n", addr);
        return -1;
    }

    ret = (unsigned short)tsec_local_mdio_read(priv->phyregs, addr, reg);
    *value = ret;

    return 0;
}

/*
 * Write a MII PHY register.
 *
 * Returns:
 *  0 on success
 */
static int tsec_miiphy_write(char *devname, unsigned char addr,
                 unsigned char reg, unsigned short value)
{
    struct tsec_private *priv = m_priv_addr;

    if (NULL == priv) {
        printf("Can't write PHY at address %d\n", addr);
        return -1;
    }

    tsec_local_mdio_write(priv->phyregs, addr, reg, value);

    return 0;
}
#endif

int eth_initialize(unsigned char *mac)
{
    m_mac_addr2 = (*(char *)mac << 16) |(*((char *)mac + 1) << 24);
    m_mac_addr1 = (*((char *)mac + 2)) |(*((char *)mac + 3) << 8) |
            (*((char *)mac + 4) << 16) |(*((char *)mac + 5) << 24);

    return tsec_initialize(&m_tsec_info[0]);
}

int eth_init(void)
{
    if (m_eth_current->init(m_eth_current) >= 0) 
    {
        m_eth_current->state = ETH_STATE_ACTIVE;

        return 0;
    }
    return -1;
}

void eth_halt(void)
{
    if (!m_eth_current)
        return;

    m_eth_current->halt(m_eth_current);

    m_eth_current->state = ETH_STATE_PASSIVE;
}

int eth_send(volatile void *packet, int length)
{
    if (!m_eth_current)
        return -1;

    return m_eth_current->send(m_eth_current, packet, length);
}

/*
 * @brief:
 *   从网卡中读取数据。
 * @param[in]: packet_buff:接收buffer指针
 * @return: 
 *   接收数据长度。
 */ 
int eth_rx(char *packet_buff)
{
    if (!m_eth_current)
    {
    
        printk(" eth_rx, m_eth_current = NULL\n");
        return -1;
    }
    return m_eth_current->recv(m_eth_current,packet_buff);
}


int eth_enable_rx_int(int flag)
{

    if (!m_eth_current){
        return -1;
    }
    struct tsec_private *priv = (struct tsec_private *)m_eth_current->priv;
    volatile tsec_t *regs = priv->regs;
    unsigned int mask;
    
    mask = regs->imask;
    if(flag)
    {
        /*开中断*/
        regs->imask = (mask | IMASK_RXFEN0);
    }
    else
    {
        regs->imask = (mask & (~IMASK_RXFEN0));
    }
    
    EIEIO_SYNC;
    return 0;

}

int eth_clear_rx_event()
{
    if (!m_eth_current){
        return -1;
    }
    
    struct tsec_private *priv = (struct tsec_private *)m_eth_current->priv;
    volatile tsec_t *regs = priv->regs;

    regs->ievent = IEVENT_INIT_CLEAR;
    
    EIEIO_SYNC;
    return 0;

}

int get_eth_status()
{
    unsigned int status = 0;
    if (!m_eth_current){
        return -1;
    }
    
    struct tsec_private *priv = (struct tsec_private *)m_eth_current->priv;
    volatile tsec_t *regs = priv->regs;

    status = regs->ievent;
    
    EIEIO_SYNC;
    return (status);

}

int eth_get_rx_status()
{
    if (get_eth_status() & IEVENT_RXF0)
    {
        return 1;
    }
    return 0;
}


/**********************kbsp interface**********************/
/*
 * @brief:
 *    打开网络设备。
 * @param[in]: minor: 设备号
 * @param[in]: mac: MAC地址
 * @return: 
 *    0: 成功。
 *    -1: 失败。
 */ 
T_WORD  kbspRawNetOpen(T_UBYTE minor, UINT8 * mac)
{
    int ret = eth_initialize(mac);
    ret &= eth_init();
    return ret;
}

/*
 * @brief:
 *    从网络设备读取一个数据包。
 * @param[in]: minor: 未使用
 * @param[in]: buf: 存放数据包的缓冲
 * @param[out]: offset: 有些网卡会在以太网帧的前面添加部分状态字，该
 *                      状态字不属于以太网帧，并且依赖于不同的网卡，offset就指明了这
 *                      部分数据的长度，以通知协议栈丢弃该部分数据
 * @return: 
 *    获取的数据包的大小。
 */ 
T_WORD  kbspRawNetGetpkt
(
T_UBYTE minor,
T_CHAR* buf,
T_WORD* offset
)
{
    *offset = 0;
    int ret = eth_rx(buf);

    if(ret > 0)
    {
           return ret;
    }
    else
    {
           return 0;
    }
}

/*
 * @brief:
 *    向网络设备发送一个数据包。
 * @param[in]: minor: 未使用
 * @param[in]: buf: 存放数据包的缓冲
 * @param[in]: cnt: 数据包的长度
 * @return: 
 *    无
 */
T_VOID kbspRawNetPutpkt
(
T_UBYTE minor,
T_CHAR* buf,
T_WORD cnt
)
{
    eth_send(buf,cnt);
    return;
}

#if 0
/*
 * @brief:
 *    网卡中断服务程序。
 * @param[in]: minor: 未使用
 * @param[in]: callbackhandler callback: 指针，回调函数
 * @return:
 *    无
 */ 
T_VOID kbspRawNetIsr
(
T_UBYTE minor,
callbackhandler callback
)
{
    eth_clear_rx_event();
    if(NULL != callback)
    {
       return callback();
    }
}
#endif
/*
 * @brief:
 *    使能/禁止网卡接收中断。
 * @param[in]: minor: 未使用
 * @param[in]: isEnable: 是否使能网卡接收中断
 * @return: 
 *    无
 */ 
T_VOID kbspRawNetEnableRxInt
(
T_UBYTE minor,
T_BOOL isEnable
)
{
     eth_enable_rx_int(isEnable);
}

/*
 * @brief:
 *    获得网卡的接收中断号。
 * @param[in]: minor: 未使用
 * @return: 
 *    网卡的接收中断号。
 */ 
T_UBYTE  kbspRawNetGetRxVector(T_UBYTE minor)
{
    return 16;//网卡eTSEC3接收中断号
}

