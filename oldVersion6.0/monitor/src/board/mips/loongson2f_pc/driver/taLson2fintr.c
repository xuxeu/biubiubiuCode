/****************************************************************************
*				北京科银京成技术有限公司 版权所有
* 	 Copyright (C) 2000-2008 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/
 
/*
 * 修改记录：
 *	20080311	创建该文件。
 *
 */
 
/**
 * @file	taLson2eintr.inc
 * @brief
 *	<li>功能：龙芯2E中断控制器管理</li>
 * @date 	20080311
 */

/**************************** 引用部分 *****************************************/
#include "sysTypes.h"
#include "memory.h"
#include "taLson2fintr.h"

/**************************** 定义部分 *****************************************/

/**************************** 实现部分 *****************************************/
/**
 * @brief
 *	  	初始化中断控制器。
 *
 * @return 无。
 */
void monitorInterruptInit(void)
{

    PIC_SLAVE_ICW1_IO_PORT = 0x11; //edge triggered 
    PIC_SLAVE_ICW2_IO_PORT = 0x28; //Slave base vector after Master
    PIC_SLAVE_ICW3_IO_PORT = 0x02; //slave cascaded to IR2 on master
    PIC_SLAVE_ICW4_IO_PORT = 0x01; //must be 0x01
    PIC_SLAVE_OCW1_IO_PORT = 0xFF; //disable all interrupt in slave 

    PIC_MASTER_ICW1_IO_PORT = 0x11; //edge triggered
    PIC_MASTER_ICW2_IO_PORT = 0x20; //base vector starts at byte 32
    PIC_MASTER_ICW3_IO_PORT = 0x04; //IR2 is cascaded internally
    PIC_MASTER_ICW4_IO_PORT = 0x01; //idem
    PIC_MASTER_OCW1_IO_PORT = 0xC3; //enable IRQ2 only

}

/**
 * @brief
 *	  	使能中断控制器上的某个中断。
 *
 * @param[in] vector：中断向量号。
 *
 * @return 无。
 */
void monitorEnablePIC(int vector)
{

 //  int level;
     unsigned char intrs_cache ;

     vector -= 0x20;

 //  CPU_Int_Disable(level);

     if (vector < 8)
       {
        (T_UBYTE)intrs_cache = PIC_MASTER_IMR_IO_PORT;
        intrs_cache &=  (~(1 << vector));
        PIC_MASTER_IMR_IO_PORT = (T_UBYTE)intrs_cache;
       }
       else
       {
        (T_UBYTE)intrs_cache = PIC_MASTER_IMR_IO_PORT;
        intrs_cache &=  (~(1 << (vector -8)));
        PIC_SLAVE_IMR_IO_PORT = (T_UBYTE)intrs_cache;
       }

  //  CPU_Int_Enable(level);

}

void monitorDisablePIC(int vector)
{

}

/**
 * @brief
 *	  	结束当前中断，恢复中断控制器的状态。
 *
 * @return 无。
 */
void monitorIntTerminate(void)
{
	PIC_SLAVE_OCW2_IO_PORT = 0x20;
    PIC_MASTER_OCW2_IO_PORT = 0x20;

}


