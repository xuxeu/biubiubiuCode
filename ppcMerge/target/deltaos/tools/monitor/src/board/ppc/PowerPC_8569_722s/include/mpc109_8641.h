/* sysTsi108Mpic.h - Tundra TSI108 MPIC definitions */

/*
Description:

Contains definitions for the Tundra Tsi108 MPIC
*/


#ifndef	__INCsysTsi108Mpich
#define	__INCsysTsi108Mpich

#ifdef __cplusplus
extern "C" {
#endif

/* includes */
#include <config.h>

/* structures */
#ifndef	_ASMLANGUAGE

typedef struct intHandlerDesc	    /* interrupt handler desciption */
    {
    void		    *vec;    /* interrupt vector */
    int			    arg;    /* interrupt handler argument */
    struct intHandlerDesc*  next;   /* next interrupt handler & argument */
    } INT_HANDLER_DESC;

typedef struct intVector
    {
    unsigned char intVec;         /* interrupt vector */
    unsigned int  reg;            /* interrupt contrl register */
    }INT_VECTOR_DESC;

/******************************************************************************
* GPIO Interface Registers (BL_GPIO)
*/
typedef enum
{
	PIN_IO,      /*gpio act as gernal io pin */
	PIN_INT,     /*gpio act as interrtup input pin */
}PIN_TYPE;

typedef enum
{
	PIN_INPUT,
	PIN_OUTPUT,
}IO_DIR;

typedef struct
    {
    PIN_TYPE pinType;
    IO_DIR   ioDir;
    unsigned int   intSenses;
    } GPIO_CFG;

#define GENERAL_STD_IRQ_LAST (32)
#endif	/* _ASMLANGUAGE */

#if ( CONFIG_ENDIAN == big )
  #define LONGSWAP(x)        ((((UINT32)(x) & 0xff000000) >> 24) |(((UINT32)(x) & 0x00ff0000) >>  8) | (((UINT32)(x) & 0x0000ff00)<<8)|(((UINT32)(x) & 0x000000ff) << 24))
#else
 #define LONGSWAP(x)   ((UINT32)(x))
#endif

/* macros */
#define VECTOR_MASK		0x000000ff


#define TSI108_MAX_VECTORS	(36 + 4) /* 36 sources + PCI INTAD demux */
#define MAX_TASK_PRIO		0xF

#define DEFAULT_PRIO_LVL	10 /* default priority level for all sources */

/*
 * Interrupt vectors assignment to external and internal
 * sources of requests.
 */
#define TSI109_CSR_BASE	(CCSRBAR)

#ifndef TSI109_IRQ_BASE
#define TSI109_IRQ_BASE 	(0)
#endif

#define TSI109_IRQ(x)		(TSI109_IRQ_BASE + (x))

#define INT_VEC_SPURIOUS	0xFF

#define GPIO_DATA              (0x000000A0)
#define GPIO_CTRL              (0x000000A4)
#define GPIO_EDGE_CTRL         (0x000000A8)
#define GPIO_INT_STATUS        (0x000000AC)

/*
 * GPIO0 - 15  lines demultiplexor
 */
#define IRQ_GPIO_BASE	    TSI109_IRQ(40)
#define IRQ_GPIO0		    (IRQ_GPIO_BASE + 0)
#define IRQ_GPIO1		    (IRQ_GPIO_BASE + 1)
#define IRQ_GPIO2		    (IRQ_GPIO_BASE + 2)
#define IRQ_GPIO3		    (IRQ_GPIO_BASE + 3)
#define IRQ_GPIO4		    (IRQ_GPIO_BASE + 4)
#define IRQ_GPIO5		    (IRQ_GPIO_BASE + 5)
#define IRQ_GPIO6		    (IRQ_GPIO_BASE + 6)
#define IRQ_GPIO7		    (IRQ_GPIO_BASE + 7)
#define IRQ_GPIO8		    (IRQ_GPIO_BASE + 8)
#define IRQ_GPIO9		    (IRQ_GPIO_BASE + 9)
#define IRQ_GPIO10		    (IRQ_GPIO_BASE + 10)
#define IRQ_GPIO11		    (IRQ_GPIO_BASE + 11)
#define IRQ_GPIO12		    (IRQ_GPIO_BASE + 12)
#define IRQ_GPIO13		    (IRQ_GPIO_BASE + 13)
#define IRQ_GPIO14		    (IRQ_GPIO_BASE + 14)
#define IRQ_GPIO15		    (IRQ_GPIO_BASE + 15)

#define NUM_GPIO_IRQS		    (16)

/* number of entries in vector dispatch table */
#define IRQ_TSI108_TAB_SIZE	(TSI108_MAX_VECTORS + 1)

/* Mapping of MPIC outputs to processors' interrupt pins */

#define IDIR_INT_OUT0   0x1
#define IDIR_INT_OUT1   0x2
#define IDIR_INT_OUT2   0x4
#define IDIR_INT_OUT3   0x8

/*---------------------------------------------------------------
 * IRQ line configuration parameters */

#define TSI109_IRQ_SENSE_EDGE	  0   /* Edge Sensitive */
#define TSI109_IRQ_SENSE_LEVEL	  2   /* Level Sensitive */

#define TSI109_IRQ_ACTIVE_LOW	  0   /* Active Low (level) / Negative Edge */
#define TSI109_IRQ_ACTIVE_HIGH	  1   /* Active High (level) / Positive Edge */

/* Interrupt delivery modes */
#define	TSI109_IRQ_DIRECTED       0
#define TSI109_IRQ_DISTRIBUTED    1

/*内部中断由原来的32个变为现在的48个，
*外部中断12个，消息中断8个
*/
/* A组timer */
#define INT_VEC_TIMER0		TSI109_IRQ(32+16+12+8)	/* Global Timer 0 */
#define INT_VEC_TIMER1		TSI109_IRQ(33+16+12+8)	/* Global Timer 1 */
#define INT_VEC_TIMER2		TSI109_IRQ(34+16+12+8)	/* Global Timer 2 */
#define INT_VEC_TIMER3		TSI109_IRQ(35+16+12+8)	/* Global Timer 3 */

#define EXTERN_INT_LAST             (INT_VEC_TIMER3 +4)

/******************************************************************************
* MPIC Registers (BL_MPIC)
*/

/*
 * Registers offsets from the block base
 */

/* Global interrupt registers */
#define MPIC_INT_M				(0x80000000)		/* Interrupt MASK */
#define MPIC_INT_GCR_MIXED_MODE (0x20000000)
#define MPIC_INT_A				(0x40000000)		/* Active Interrupt */
#define MPIC_INT_FRR		    (0x00000000)
#define MPIC_INT_GCR		    (0x00041020)
#define MPIC_INT_ICR		    (0x00000008)
#define MPIC_INT_MVI		    (0x0000000c)
#define MPIC_INT_SVR		    (0x000410E0)
#define MPIC_INT_TFRR		    (0x00000014)
#define MPIC_INT_SOFT_SET	    (0x00000020)
#define MPIC_INT_SOFT_ENABLE	(0x00000024)


/* External interrupt */
#define MPIC_INT_EVPR(x)	    (0x50000 + 0x20*(x))
#define MPIC_INT_EDR(x)		    (0x50010 + 0x20*(x))
/* Internal interrupt */
#define MPIC_INT_IVPR(x)	    (0x50200 + 0x20*(x))
#define MPIC_INT_IDR(x)		    (0x50210 + 0x20*(x))
/* message interrupt */
#define MPIC_INT_MVPR(x)	    (0x51c00 + 0x20*(x))
#define MPIC_INT_MDR(x)		    (0x51c10 + 0x20*(x))

/* Timer registers */
#define MAX_MPIC_GT		        4
#define MPIC_INT_TFRRA(x)        (0x410F0+0x1000*(x))
#define MPIC_INT_GTBCRA0(x)      (0x41110+0x40*(x))
#define MPIC_INT_GTVPRA0(x)        (0x41120+0x40*(x))
#define MPIC_INT_GTDRA0(x)          (0x41130+0x40*(x))
#define MPIC_INT_GTBBRA0(x)          (0x41100+0x40*(x))


#define MPIC_INT_GTVPRB0(x)        (0x42120+0x40*(x))

/* Doorbell registers */
#define MPIC_INT_DAR		    (0x00000200)

#define MAX_MPIC_DOORS		    4
#define MPIC_INT_DVPR(x)	    (0x204 + 0xc*(x))
#define MPIC_INT_DDR(x)		    (0x208 + 0xc*(x))
#define MPIC_INT_DMR(x)		    (0x20c + 0xc*(x))

/* Mailbox registers */
#define MAX_MPX_MAILBOXES	    4
#define MPIC_INT_MBR(x)	        (0x280 + 0x10*(x))
#define MPIC_INT_MBVPR(x)	    (0x284 + 0x10*(x))
#define MPIC_INT_MBDR(x)	    (0x288 + 0x10*(x))

/* Interrupt controller output pins */
#define MAX_MPIC_INT_OUTPUTS	4

#define MPIC_INT_TASKP(x)	    (0x40080)
#define MPIC_INT_VECTOR(x)	    (0x400a0)
#define MPIC_INT_EOI(x)		    (0x400b0)
#define MPIC_INT_CSR(x)		    (0x30c+ 0x40*(x))

#define MPIC_UART1_OFFSET   0x50540
#define MPIC_UART1_VALUE    (0x800000 | 10 << 16 | 26)
#define UART1_IRQ (26)

#define MPIC_UART2_OFFSET   0x50380
#define MPIC_UART2_VALUE    (0x800000 | 10 << 16 | 12)
#define UART2_IRQ (12)
#define MPIC_ETSEC3_OFFSET (0x50400)
#define MPIC_ETSEC3_VALUE  (0x80800000 | 10 << 16 | 16)
#define ETSEC3_IRQ (16)

#define MPIC_ETSEC4_OFFSET (0x50400)
#define MPIC_ETSEC4_VALUE  (0x80800000 | 10 << 16 | 16)
#define ETSEC4_IRQ (16)
/*
 * Register bits and fields
 */
#define MPIC_INT_FRR_VID	        (0x000000ff)
#define MPIC_INT_FRR_NCPU	        (0x00001f00)
#define MPIC_INT_FRR_NITM	        (0x0000e000)
#define MPIC_INT_FRR_NIRQ	        (0x07ff0000)
#define MPIC_INT_FRR_NIDOOR	        (0xe0000000)
#define MPIC_INT_GCR_R		        (0x80000000)
#define MPIC_INT_ICR_R		        (0x0000000f)
#define MPIC_INT_MVI_VID	        (0x000000ff)
#define MPIC_INT_MVI_DID	        (0x0000ff00)
#define MPIC_INT_MVI_STEP	        (0x00ff0000)
#define MPIC_INT_SVR_VECTOR	        (0x000000ff)
#define MPIC_INT_SOFT_SET_S	        (0x00ffffff)
#define MPIC_INT_SOFT_ENABLE_EN		(0x00ffffff)
#define MPIC_INT_GTCCR_COUNT	    (0x7fffffff)
#define MPIC_INT_GTCCR_T	        (0x80000000)
#define MPIC_INT_GTBCR_B_COUNT      (0x7fffffff)
#define MPIC_INT_GTBCR_CI	        (0x80000000)
#define MPIC_INT_GTVPR_VECTOR		(0x0000ffff)
#define MPIC_INT_GTVPR_PRIORITY		(0x000f0000)
#define MPIC_INT_GTVPR_PRESCALE		(0x00f00000)
#define MPIC_INT_GTVPR_A		    (0x40000000)
#define MPIC_INT_GTVPR_M		    (0x80000000)
#define MPIC_INT_GTDR_SEL_OUT		(0x0000000f)
#define MPIC_INT_IVPR_VECTOR		(0x000000ff)
#define MPIC_INT_IVPR_PRIORITY		(0x000f0000)
#define MPIC_INT_IVPR_P			    (0x01000000)
#define MPIC_INT_IVPR_S			    (0x02000000)
#define MPIC_INT_IVPR_MODE		    (0x20000000)
#define MPIC_INT_IVPR_A			    (0x40000000)
#define MPIC_INT_IVPR_M			    (0x80000000)
#define MPIC_INT_IDR_SEL_OUT		(0x0000000f)
#define MPIC_INT_DAR_A			    (0x0000000f)
#define MPIC_INT_DVPR_VECTOR		(0x000000ff)
#define MPIC_INT_DVPR_PRIORITY		(0x000f0000)
#define MPIC_INT_DVPR_A			    (0x40000000)
#define MPIC_INT_DVPR_M			    (0x80000000)
#define MPIC_INT_DDR_SEL_OUT		(0x0000000f)
#define MPIC_INT_MBVPR_VECTOR		(0x000000ff)
#define MPIC_INT_MBVPR_PRIORITY		(0x000f0000)
#define MPIC_INT_MBVPR_A		    (0x40000000)
#define MPIC_INT_MBVPR_M		    (0x80000000)
#define MPIC_INT_MBDR_SEL_OUT		(0x0000000f)
#define MPIC_INT_TASKP_TASKP		(0x0000000f)
#define MPIC_INT_VECTOR_VECTOR		(0x000000ff)
#define MPIC_INT_VECTOR_LS_VECTOR	(0xff000000)
#define MPIC_INT_EOI_EOI		    (0x000000ff)
#define MPIC_INT_CSR_P			    (0x00000001)
#define MPIC_INT_CSR_S			    (0x00000002)

#ifdef __cplusplus
}
#endif

#endif	/* __INCsysTsi108Mpich */

