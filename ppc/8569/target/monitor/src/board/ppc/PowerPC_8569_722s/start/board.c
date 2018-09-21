/* $Id:lmain.c v1.0 2003-12-05 */

/******************************************************************************
 *  This source code has been made available to you by CORETEK  on
 *  AS-IS.Anyone receiving this source is licensed under
 *  CORETEK copyrights to use it in any way he or she deems fit,including
 *  copying it,modifying it,compiling it,and redistributing it either with
 *  or without modifictions.
 *
 *
 *  Any person who transfers this source code or any derivative work must
 *  include the CORETEK copyright notice, this paragraph,and the preceding
 *  two paragraphs in the transferred software.
 *
 *
 *       COPYRIGHT CORETEK CORPORATION 2003
 *     LICENSED MATERIAL - PROGRAM PROPERTY OF CORETEK
 *****************************************************************************/


/******************************************************************************
 *
 *  FILE: lmain.c
 *
 *  MODULE: start
 *
 *  PURPOSE: clear bss and install vector .
 *
 *  AUTHOR(S):Lemon
 *
 *  GROUP:System Dept.
 *
 *  DATE CREATED:2003-12-05
 *
 *  REFERENCE DOCUMENT ID:
 *
 *  MODIFICATIONS:
 *  Date          user Name       Description
 *  2003-12-05    Lemon              Create this file
 *
 *********************************************************************************/

#include "taTypes.h"

#define SYS_INIT_STACK_SIZE  (8192)
#define		CONFIG_DRIVERS_PRINTK		("COM1")

extern void sys_init();

#include "sysUart.h"
extern T_DR_UartConfigTable utDR_UartConfigTable[];

extern void _0x700Handle_start();
extern void _0x700Handle_end();

unsigned int _init_stack[SYS_INIT_STACK_SIZE];

void lmain(void)
{
  //将vector代码，拷贝到0x100-1800处
	memcpy(0x700,_0x700Handle_start,_0x700Handle_end - _0x700Handle_start);
    sys_init();
}

unsigned int sysToMonitor (
int startType
)
{
    return 0;	
}
