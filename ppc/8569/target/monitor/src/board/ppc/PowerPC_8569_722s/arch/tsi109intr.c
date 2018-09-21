#include "taTypes.h"
//#include <drv/intr/tsi109_8641.h>
//#include <drv/intr/mpc109_8641.h>
#include "taErrorDefine.h"
#include "tsi109_8641.h"
#include "mpc109_8641.h"

static const INT_VECTOR_DESC sysIntVecTbl[]={
	{0,MPIC_INT_IVPR(0)},	{1,MPIC_INT_IVPR(1)},
	{2,MPIC_INT_IVPR(2)},	{3,MPIC_INT_IVPR(3)},
	{4,MPIC_INT_IVPR(4)},	{5,MPIC_INT_IVPR(5)},
	{6,MPIC_INT_IVPR(6)},	{7,MPIC_INT_IVPR(7)},
	{8,MPIC_INT_IVPR(8)},	{9,MPIC_INT_IVPR(9)},
	{10,MPIC_INT_IVPR(10)},	{11,MPIC_INT_IVPR(11)},
	{12,MPIC_INT_IVPR(12)},	{13,MPIC_INT_IVPR(13)},
	{14,MPIC_INT_IVPR(14)},	{15,MPIC_INT_IVPR(15)},
	{16,MPIC_INT_IVPR(16)},	{17,MPIC_INT_IVPR(17)},
	{18,MPIC_INT_IVPR(18)},	{19,MPIC_INT_IVPR(19)},
	{20,MPIC_INT_IVPR(20)},	{21,MPIC_INT_IVPR(21)},
	{22,MPIC_INT_IVPR(22)},	{23,MPIC_INT_IVPR(23)},
	{24,MPIC_INT_IVPR(24)},	{25,MPIC_INT_IVPR(25)},
	{26,MPIC_INT_IVPR(26)},	{27,MPIC_INT_IVPR(27)},
	{28,MPIC_INT_IVPR(28)},	{29,MPIC_INT_IVPR(29)},
	{30,MPIC_INT_IVPR(30)},	{31,MPIC_INT_IVPR(31)},	
	{32,MPIC_INT_IVPR(32)},	{33,MPIC_INT_IVPR(33)},
	{34,MPIC_INT_IVPR(34)},	{35,MPIC_INT_IVPR(35)},
	{36,MPIC_INT_IVPR(36)},	{37,MPIC_INT_IVPR(37)},
	{38,MPIC_INT_IVPR(38)},	{39,MPIC_INT_IVPR(39)},
	{40,MPIC_INT_IVPR(40)},	{41,MPIC_INT_IVPR(41)},
	{42,MPIC_INT_IVPR(42)},	{43,MPIC_INT_IVPR(43)},
	{44,MPIC_INT_IVPR(44)},	{45,MPIC_INT_IVPR(45)},
	{46,MPIC_INT_IVPR(46)},	{47,MPIC_INT_IVPR(47)},
	{48,MPIC_INT_EVPR(0)},	{49,MPIC_INT_EVPR(1)},	/* External interrupt */
	{50,MPIC_INT_EVPR(2)},	{51,MPIC_INT_EVPR(3)},
	{52,MPIC_INT_EVPR(4)},	{53,MPIC_INT_EVPR(5)},
	{54,MPIC_INT_EVPR(6)},	{55,MPIC_INT_EVPR(7)},
	{56,MPIC_INT_EVPR(8)},	{57,MPIC_INT_EVPR(9)},
	{58,MPIC_INT_EVPR(10)},	{59,MPIC_INT_EVPR(11)},
	{60,MPIC_INT_MVPR(0)},	{61,MPIC_INT_MVPR(1)},	/* Messaging interrupt */
	{62,MPIC_INT_MVPR(2)},	{63,MPIC_INT_MVPR(3)},
	{64,MPIC_INT_MVPR(4)},	{65,MPIC_INT_MVPR(5)},	
	{66,MPIC_INT_MVPR(6)},	{67,MPIC_INT_MVPR(7)},	
	{68,MPIC_INT_GTVPRA0(0)}, {69,MPIC_INT_GTVPRA0(1)},/* Timer interrupt */
	{70,MPIC_INT_GTVPRA0(2)},  {71,MPIC_INT_GTVPRA0(3)},
	{72,MPIC_INT_GTVPRB0(0)}, {73,MPIC_INT_GTVPRB0(1)},
	{74,MPIC_INT_GTVPRB0(2)},  {75,MPIC_INT_GTVPRB0(3)},
};

/*
 * The table below defines external and internal IRQ senses and polarities.
 */
static UINT32 tsi109IntSenses[] =
	{
	/*
	 * First four entries of the table will be updated
	 * by the MPIC initialization routine based on actual board design
	 */
	(TSI109_IRQ_SENSE_LEVEL | TSI109_IRQ_ACTIVE_LOW),  /* INT[0] */
	(TSI109_IRQ_SENSE_LEVEL | TSI109_IRQ_ACTIVE_LOW),  /* INT[1] */
	(TSI109_IRQ_SENSE_LEVEL | TSI109_IRQ_ACTIVE_LOW),  /* INT[2] */
	(TSI109_IRQ_SENSE_LEVEL | TSI109_IRQ_ACTIVE_LOW),  /* INT[3] */

	/* All interrupt sources below are internal for Tsi109 */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* Reserved */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* Reserved */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* Reserved */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* Reserved */

	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* DMA ch 0,no used */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* DMA ch 1,no used  */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* DMA ch 2,no used  */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* DMA ch 3,no used  */
	(TSI109_IRQ_SENSE_LEVEL | TSI109_IRQ_ACTIVE_HIGH),  /* UART0 */
	(TSI109_IRQ_SENSE_LEVEL | TSI109_IRQ_ACTIVE_HIGH),  /* UART1 */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* I2C,no used  */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* GPIO,no used  */

	(TSI109_IRQ_SENSE_LEVEL | TSI109_IRQ_ACTIVE_HIGH),  /* GIGE0 */
	(TSI109_IRQ_SENSE_LEVEL | TSI109_IRQ_ACTIVE_HIGH),  /* GIGE1 */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* Clock Generator,no used  */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* HLP,no used  */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* SDRAM Controller,no used  */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* PI1,no used  */
	(TSI109_IRQ_SENSE_EDGE  | TSI109_IRQ_ACTIVE_HIGH),  /* PI2,no used  */
	(TSI109_IRQ_SENSE_LEVEL | TSI109_IRQ_ACTIVE_HIGH)   /* PCI block */
};

static int cpuNum = 0;


void initPIC(void)
{
	UINT32 tmp, i;

	/* Reset MPIC */
	TSI109_REG_WR(MPIC_INT_GCR, 0, MPIC_INT_M);
	/* Wait for reset completion */
	while(TSI109_REG_RD(MPIC_INT_GCR, 0) & MPIC_INT_M)
		EIEIO_SYNC;

    /* 初始化其他设备寄存器INT_VEC_TIMER0 */
    for (i = 0; i < INT_VEC_TIMER0; i++)
    {
        /* Disabled, Priority 10 */
        tmp = 0x80800000 | ((DEFAULT_PRIO_LVL) << 16) | (i);
        TSI109_REG_WR(0, sysIntVecTbl[i].reg, tmp);
        /* mapped to processor 0 */
        //TSI109_REG_WR(sysIntVecTbl[i].reg, 0, 0);
    }
    /* 初始化A组定时器寄存器 */
	for (i = 0; i < MAX_MPIC_GT; i++)
	{
		/* Disabled, Priority 10 */
		tmp = MPIC_INT_GTVPR_M | ((DEFAULT_PRIO_LVL) << 16) | (INT_VEC_TIMER0 + i);
		TSI109_REG_WR(0, MPIC_INT_GTVPRA0(i), tmp);
		/* mapped to processor 0 */
		TSI109_REG_WR(MPIC_INT_GTDRA0(i), 0, 0);
	}

    /* 初始化B组定时器寄存器 */
    for (i = 0; i < MAX_MPIC_GT; i++)
    {
        /* Disabled, Priority 10 */
        tmp = MPIC_INT_GTVPR_M | ((DEFAULT_PRIO_LVL) << 16) | (INT_VEC_TIMER0 + i);
        TSI109_REG_WR(0, MPIC_INT_GTVPRB0(i), tmp);
        /* mapped to processor 0 */
        TSI109_REG_WR(MPIC_INT_GTDRA0(i), 0, 0);
    }

	/* Initialize the spurious */
	TSI109_REG_WR(0, MPIC_INT_SVR, 0xff);
	
	/* Set the current processor priority level */
	TSI109_REG_WR(MPIC_INT_TASKP(0), 0, 0x0);
	
	TSI109_REG_WR(MPIC_INT_GCR, 0, MPIC_INT_GCR_MIXED_MODE);

	
    /* 注意:这里使用默认值设置为core0 响应该网卡中断 */    
    TSI109_REG_WR(MPIC_ETSEC3_OFFSET, 0, MPIC_ETSEC3_VALUE);
	
	while(((TSI109_REG_RD(0,MPIC_INT_VECTOR(0)) & VECTOR_MASK) != INT_VEC_SPURIOUS)
		||
		((TSI109_REG_RD(0,MPIC_INT_VECTOR(1)) & VECTOR_MASK) != INT_VEC_SPURIOUS))
	{
	
		TSI109_REG_WR(0,MPIC_INT_EOI(0), 0);
		TSI109_REG_WR(0,MPIC_INT_EOI(1), 0);	
	};
	TSI109_REG_WR(0,MPIC_INT_EOI(0), 0);
	TSI109_REG_WR(0,MPIC_INT_EOI(1), 0);

	return 1;
}

T_TA_ReturnCode enableIrqLine(UINT32 irqLine)
{
	UINT32 curValue;

	if (irqLine > EXTERN_INT_LAST )
	{
		return (TA_INVALID_INPUT);
	}

			/* get current XXDR */
	curValue = TSI109_REG_RD(0, sysIntVecTbl[irqLine].reg);
			
			TSI109_REG_WR(0,sysIntVecTbl[irqLine].reg,(curValue & (~MPIC_INT_M)));
			/* read it to ensure that register is set before continue */
			TSI109_REG_RD(0,sysIntVecTbl[irqLine].reg);

	return (TA_OK);
}

T_TA_ReturnCode disableIrqLine(UINT32 irqLine)
{
	UINT32 curValue;

	if ( irqLine > EXTERN_INT_LAST )
	{
		return (TA_INVALID_INPUT);
	}

	curValue = TSI109_REG_RD(0,sysIntVecTbl[irqLine].reg);
	TSI109_REG_WR(0,sysIntVecTbl[irqLine].reg,(curValue|MPIC_INT_M));
	EIEIO_SYNC;

	return (TA_OK);
}

T_TA_ReturnCode disableIrqByVector(UINT32 vector)
{
	return disableIrqLine(vector-GENERAL_STD_IRQ_LAST);
}

volatile T_VOID deliverEOI( UINT32 vector )
{
    EIEIO_SYNC;
        TSI109_REG_WR(0, MPIC_INT_EOI(0), 0);
    EIEIO_SYNC;

}

/*
 * @brief
 *    获取中断号 。
 * @param[in] 无。
 * @param[out] : 无。
 * @returns: 返回中断号。
 */
volatile UINT32 getVectorNum( T_VOID )
{
	T_UBYTE irqLine = 0xff;
	irqLine = TSI109_REG_RD(0,MPIC_INT_VECTOR(0));
	EIEIO_SYNC;
	return (irqLine);
}

