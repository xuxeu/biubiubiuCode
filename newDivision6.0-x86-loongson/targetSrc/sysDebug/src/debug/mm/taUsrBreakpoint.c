/* usrBreakpoint.c - user configurable breakpoint management */

/* 
 * Copyright (c) 1998-2007 Wind River Systems, Inc. 
 *
 * The right to copy, distribute, modify or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */ 

/*
modification history
--------------------
01g,27sep07,rlp  Added a cache flush policy parameter to usrBreakpointSet()
                 (CQ:WIND00104297, CQ:WIND00104323, CQ:WIND00104325).
01f,30oct06,rlp  Modified for VxDBG library (REQ00007655).
01e,12apr06,dbt  Fixed cache support (SPR #119927).
01d,03aug05,bpn  Do not updated cache if the write into TEXT area failed
                 (SPR#108298).
01c,27nov03,elg  Memory context is needed to unprotect page before installing
                 breakpoint.
01b,02oct03,elg  Rework way breakpoints are planted.
01a,13jan98,dbt  written.
*/

/*
DESCRIPTION

This file contains user configurable breakpoint management routines used 
by target shell debugger and WDB debugger.
This file is included by usrConfig.c.

SEE ALSO: usrExtra.c

NOMANUAL
*/

#ifndef _KERNEL_DEBUG_
#include "ta.h"
#ifndef	__INC_usrBreakpointc
#define	__INC_usrBreakpointc

#ifdef __MIPS__
/* includes */

#include <vxWorks.h>
#include <cacheLib.h>

#include <private/vxdbgLibP.h>
#include <private/vxdbgUtilLibP.h>
#include <private/rtpLibP.h>


static unsigned long *		vxdbgNextPc;	/* next pc of the task */
static unsigned long 		vxdbgNextInstr;	/* next instruction of the task */
static unsigned long *		vxdbgIsrNextPc;	/* next pc at interrrupt level */
static unsigned long 		vxdbgIsrNextInstr;	/* next instruction at */

#define VXDBG_NEXT_PC(tid)		vxdbgNextPc
#define VXDBG_NEXT_INSTR(tid)		vxdbgNextInstr
#define VXDBG_ISR_NEXT_PC()		vxdbgIsrNextPc
#define VXDBG_ISR_NEXT_INSTR()		vxdbgIsrNextInstr

#define	DBG_BREAK_INST		0x0000000d
#define VXDBG_LOCAL_FLUSH        0x1

REG_SET RegSet ;

/******************************************************************************
*
* usrBreakpointSet - set a text breakpoint.
*
* This routine is used by both target shell and WDB debuggers to set a 
* text breakpoint in memory.
* This routine can be modified by the user to set a breakpoint differently
* (for example to set a text breakpoint in a ROM emulator).
*
* RETURNS : N/A
*
* NOMANUAL
*/ 

void usrBreakpointSet
    (
    INSTR *		addr,		/* breakpoint address */
    INSTR 		value,		/* breakpoint instruction */
    RTP_ID		rtpId,		/* process ID */
    VXDBG_CACHE_POLICY	policy		/* cache flush policy */
    )
    {
    STATUS		status;
    VM_CONTEXT_ID	vmCtx = NULL;

    if (rtpId != NULL)
	vmCtx = RTP_VM_CONTEXT_GET (rtpId);

    if (addr == (INSTR *) NULL)
        return;			/* no error messages available! */

    //status = VM_CONTEXT_BUFFER_WRITE (vmCtx, &value, addr, sizeof (INSTR));
    status = taDebugWriteMemory(rtpId, addr, &value, sizeof (INSTR),Align_None);

    if (status == OK)
	{
	switch (policy)
	    {
	    case VXDBG_LOCAL_FLUSH :

		CACHE_TEXT_LOCAL_UPDATE (addr, sizeof (INSTR));
	    	break;

	    case VXDBG_FLUSH :

		CACHE_TEXT_UPDATE (addr, sizeof (INSTR));
	    	break;

	    case VXDBG_NO_FLUSH :
	    default :

	    	break;
	    }
	}
}

/******************************************************************************
*
* wdbDbgGetNpc - get the next pc
*
* If not in the delay slot of a jump or branch instruction,
* the next instruction is always pc+4 (all branch and jump instructions
* have 1 delay slot). Otherwise the npc is the target of the jump or branch.
*
* For branch and jump instructions, the instruction in the delay slot is
* never reported.  The expectation is that a breakpoint will be set on
* the instruction that is returned.  When a resuming from a breakpoint
* hit in a delay slot, execution restarts at the branch or jump.
*
* RETURNS: The next instruction
*
* NOMANUAL
*/

INSTR *wdbDbgGetNpc
    (
    REG_SET *pRegs		/* pointer to task registers */
    )
    {
    int	rsVal;
    int	rtVal;
    int	disp;
    INSTR	machInstr;
    ULONG	npc;
    ULONG	pc;

    pc = (ULONG) pRegs->pc;
    machInstr = *(INSTR *) pRegs->pc;	/* Get the Instruction */

    /* Default instruction is the next one. */

    npc = pc + 4;

    /*
     * Do not report the instruction in a branch delay slot as the
     * next pc.  Doing so will mess up the WDB_STEP_OVER case as
     * the branch instruction is re-executed.
     */

    /*
     * Check if we are on a branch likely instruction, which will nullify
     * the instruction in the slot if the branch is taken.
     * Also, pre-extract some of the instruction fields just to make coding
     * easier.
     */

    rsVal = pRegs->gpreg[(machInstr >> 21) & 0x1f];
    rtVal = pRegs->gpreg[(machInstr >> 16) & 0x1f];
    disp = ((int) ((machInstr & 0x0000ffff) << 16)) >> 14;
    if ((machInstr & 0xf3ff0000) == 0x41020000)	/* BCzFL  */
	{
	int copId = (machInstr >> 26) & 0x03;
	npc = pc + 8;
	switch (copId)
	    {
	  case 1:
#ifndef SOFT_FLOAT
	    if ((pRegs->fpcsr & FP_COND) != FP_COND)
		npc = disp + pc + 4;
#endif	/* !SOFT_FLOAT */
	    break;
	    }
	}
    else if ((machInstr & 0xf3ff0000) == 0x41030000)	/* BCzTL  */
	{
	int copId = (machInstr >> 26) & 0x03;
	npc = pc + 8;
	switch (copId)
	    {
	  case 1:
#ifndef SOFT_FLOAT
	    if ((pRegs->fpcsr & FP_COND) == FP_COND)
		npc = disp + pc + 4;
#endif	/* !SOFT_FLOAT */
	    break;
	    }
	}
    else if (((machInstr & 0xfc1f0000) == 0x04130000)		/* BGEZALL*/
	     || ((machInstr & 0xfc1f0000) == 0x04030000))	/* BGEZL  */
	{
	if (rsVal >= 0)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
    else if ((machInstr & 0xfc1f0000) == 0x5c000000)	/* BGTZL  */
	{
	if (rsVal > 0)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
    else if ((machInstr & 0xfc1f0000) == 0x58000000)	/* BLEZL  */
	{
	if (rsVal <= 0)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
    else if (((machInstr & 0xfc1f0000) == 0x04120000)		/* BLTZALL*/
	     || ((machInstr & 0xfc1f0000) == 0x04020000))	/* BLTZL  */
	{
	if (rsVal < 0)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
    else if ((machInstr & 0xfc000000) == 0x50000000)	/* BEQL   */
	{
	if (rsVal == rtVal)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
    else if ((machInstr & 0xfc000000) == 0x54000000)	/* BNEL   */
	{
	if (rsVal != rtVal)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
    else if (((machInstr & 0xfc000000) == 0x08000000)	 	/* J    */
	|| ((machInstr & 0xfc000000) == 0x0c000000)) 	/* JAL  */
	npc = ((machInstr & 0x03ffffff) << 2) | (pc & 0xf0000000);
    else if (((machInstr & 0xfc1f07ff) == 0x00000009)	/* JALR */
	     || ((machInstr & 0xfc1fffff) == 0x00000008))	/* JR   */
	npc = pRegs->gpreg[(machInstr >> 21) & 0x1f];
    else if ((machInstr & 0xf3ff0000) == 0x41000000)	/* BCzF   */
	{
	int copId = (machInstr >> 26) & 0x03;
	npc = pc + 8;
	switch (copId)
	    {
	  case 1:
#ifndef SOFT_FLOAT
	    if ((pRegs->fpcsr & FP_COND) != FP_COND)
		npc = disp + pc + 4;
#endif	/* !SOFT_FLOAT */
	    break;
	    }
	}
    else if ((machInstr & 0xf3ff0000) == 0x41010000)	/* BCzT   */
	{
	int copId = (machInstr >> 26) & 0x03;
	npc = pc + 8;
	switch (copId)
	    {
	  case 1:
#ifndef SOFT_FLOAT
	    if ((pRegs->fpcsr & FP_COND) == FP_COND)
		npc = disp + pc + 4;
#endif	/* !SOFT_FLOAT */
	    break;
	    }
	}
    else if ((machInstr & 0xfc000000) == 0x10000000)	/* BEQ    */
	{
	if (rsVal == rtVal)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
    else if (((machInstr & 0xfc1f0000) == 0x04010000)	/* BGEZ   */
	     || ((machInstr & 0xfc1f0000) == 0x04110000))	/* BGEZAL */
	{
	if (rsVal >= 0)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
    else if ((machInstr & 0xfc1f0000) == 0x1c000000)	/* BGTZ   */
	{
	if (rsVal > 0)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
    else if ((machInstr & 0xfc1f0000) == 0x18000000)	/* BLEZ   */
	{
	if (rsVal <= 0)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
    else if (((machInstr & 0xfc1f0000) == 0x04000000)	/* BLTZ   */
	     || ((machInstr & 0xfc1f0000) == 0x04100000))	/* BLTZAL */
	{
	if (rsVal < 0)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
    else if ((machInstr & 0xfc000000) == 0x14000000)	/* BNE    */
	{
	if (rsVal != rtVal)
	    npc = disp + pc + 4;
	else
	    npc = pc + 8;
	}
#ifdef MIPSXX_cav_cn3xxx
    /*
     * Cavium-specific jump instruction opcodes are 0x32, 0x36,
     * 0x3a and 0x3e. Note that the binary pattern of these 6-bit
     * opcodes is 11XX10. So, by ignoring the 'XX' bits in the opcode,
     * we can identify these instructions with a single test.
     */
    else if (((machInstr & CAVIUM_JUMP_MASK) >> 26) == 0x32)
	{
	int	offset;
	int	bitnr;
	int 	regnr;
	long	reg;

    	ULONG	op;

	/* We know it's a Cavium jump instruction. Which one? */

	offset = (machInstr & 0xffff) << 2;
	bitnr  = (machInstr & (0x1f << 16)) >> 16;
	regnr  = (machInstr & (0x1f << 21)) >> 21;
	reg    = pRegs->gpreg[regnr];

	/* isolate opcode */

	op = (machInstr >> 26);

	switch (op)
	    {
	    case BBIT0:			/* BBIT0 */
		if (!(reg & (0x1 << bitnr)))
		    npc = pc + offset;
		break;
	    case BBIT032:		/* BBIT032 */
		if (!(reg & (0x1 << (bitnr + 32))))
		    npc = pc + offset;
		break;
	    case BBIT1:			/* BBIT1 */
		if (reg & (0x1 << bitnr))
		    npc = pc + offset;
		break;
	    case BBIT132:		/* BBIT132 */
		if (reg & (0x1 << (bitnr + 32)))
		    npc = pc + offset;
		break;
	    default:
		break; /* shouldn't happen! */
	    }
	}
#endif
    else
	{

	/* normal instruction */
	}
    return (INSTR *) npc;
    }

/*
 * @brief:
 *     在下一条要执行的指令出打断点
 * @return:
 *     None
 */
void SetBreakAtNextPc(T_DB_ExceptionContext* context)
{
     CORE_TCB *	pTcbCurrent = taskTcb(taTaskIdSelf());
	 
    memset(&RegSet,0,sizeof(REG_SET));
    taDebugContextConvert(context,&RegSet);
    VXDBG_ISR_NEXT_PC() = wdbDbgGetNpc (&RegSet);
    VXDBG_ISR_NEXT_INSTR() = *VXDBG_ISR_NEXT_PC();
    usrBreakpointSet (VXDBG_ISR_NEXT_PC(),
    			DBG_BREAK_INST, pTcbCurrent->rtpId,
			VXDBG_LOCAL_FLUSH);
}

/*
 * @brief:
 *     删除当前断点恢复断点处原来的指令
 * @return:
 *     None
 */
void ClearBreakAtCurPc()
{
     /* restore instructions */
    CORE_TCB *	pTcbCurrent = taskTcb(taTaskIdSelf());
    usrBreakpointSet (VXDBG_ISR_NEXT_PC(),
    			VXDBG_ISR_NEXT_INSTR(), pTcbCurrent->rtpId,
			VXDBG_LOCAL_FLUSH);      
}
#endif
#endif	/* __INC_usrBreakpointc */
#endif
