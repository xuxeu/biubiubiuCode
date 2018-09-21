/* vxdbgCpuArchLib.c - VxDBG CPU control architecture specific library */

/* 
 * Copyright (c) 2007-2008 Wind River Systems, Inc. 
 * 
 * The right to copy, distribute, modify, or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */

/*
modification history
--------------------
01d,11feb08,jmp  changed STATUS to VXDBG_STATUS for vxdbg routines
                 (CQ:WIND00116500).
01c,31oct07,jmp  made vxdbgCpuCtrlEnabled an atomic variable.
01b,02jul07,jmp  made an internal variable LOCAL. Fixed some typos.
01a,01jun07,jmp  written.
*/

/*
DESCRIPTION
This module holds the architecture specific code of the VxDBG CPU Control
library.

The purpose of the VxDBG CPU Control library is to provide to the System Mode
and to the Core Dump facilities the ability for the master CPU to suspend and
resume the OS activity on the slave CPU(s), and to retrieve the register set of
the contexts that were running on slave CPU(s) at the time of the suspend
request.

Master CPU: The CPU that is running the system mode agent or generating
            the core dump.
Slave CPU(s): The other CPU(s) of the SMP system.

This module provides the routines to enable and disable the VxDBG CPU control.
Enabling the VxDBG CPU control consists in updating the interrupt handling
mechanism to save the register set corresponding to the context that is
interrupted by a debugger IPI. On PPC, this is done by updating (patching)
the excConnectStub[] to exit the interrupt handling using intVxdbgCpuRegsGet()
routine rather than the default intExit() routine.

For a global overview of the VxDBG CPU Control, please refer to vxdbgCpuLib.c
module description.

INCLUDE FILES: private/vxdbgCpuLibP.h
*/

/* includes */

#include <vxWorks.h>
#include <cacheLib.h>
#include <string.h>
#include <excLib.h>
#include <private/excLibP.h>
#include <arch/ppc/private/excArchLibP.h>
#include <private/vmLibP.h>
#include <private/vxdbgCpuLibP.h>

/* externals */

IMPORT INSTR ppcBrCompute (VOIDFUNCPTR, INSTR *, int);
IMPORT void intVxdbgCpuRegsGet (void);
IMPORT void intExit (void);
IMPORT void taIntVxdbgCpuRegsGet(void);
/* defines */

//#define	VXDBG_CPU_CTRL_EXIT_RTN		intVxdbgCpuRegsGet
#define	VXDBG_CPU_CTRL_EXIT_RTN taIntVxdbgCpuRegsGet
/* locals */

LOCAL INSTR origExitOpcode[2];	/* origin intExit opcode */

/*******************************************************************************
*
* vxdbgCpuCtrlEnable - enable VxDBG CPU control
*
* This routine enables the VxDBG CPU control by installing intVxdbgCpuRegsGet()
* within the interrupt handling mechanism to retrieve the register set
* of the stopped CPUs.
*
* This routine must only be invoked from vxdbgCpuAllSuspend() when all the
* slave CPU(s) are handling the debug IPI and waiting for CPU control to be
* enabled (with interrupts locked).
*
* RETURNS: A VxDBG status (VXDBG_STATUS):
*       VXDBG_OK if CPU control is successfully enabled.
*       VXDBG_ERR_INIT if failed to install intVxdbgCpuRegsGet().
*
* ERRNO: N/A
*
* NOMANUAL
*/

VXDBG_STATUS taDbgCpuCtrlEnable (void)
    {
    INSTR *	pIntrStub = (INSTR *)_EXC_OFF_INTR;
    static INSTR exitOpcode[2] = {0,0};	/* VXDBG_CPU_CTRL_EXIT_RTN opcodes */

//    if (excExtendedVectors == TRUE)
    if (TRUE)
	{
    	printk("\n taDbgCpuCtrlEnable: excExtendedVectors == TRUE\n");
	/* Save origin exit opcodes */

	bcopy ((const char *)&pIntrStub[EXT_EXIT_OFF], (char *)origExitOpcode,
	       sizeof (origExitOpcode));

	/* compute opcodes to branch to VXDBG_CPU_CTRL_EXIT_RTN */

	if (exitOpcode[0] == 0)	/* Compute the opcodes only once */
	    {
	    exitOpcode[0] = (MSW ((int)pIntrStub[EXT_EXIT_OFF]) << 16) |
			     MSW ((int)VXDBG_CPU_CTRL_EXIT_RTN);
	    exitOpcode[1] = (MSW ((int)pIntrStub[EXT_EXIT_OFF + 1]) << 16) |
			     LSW ((int)VXDBG_CPU_CTRL_EXIT_RTN);
	    }

	/* install VXDBG_CPU_CTRL_EXIT_RTN */

	if (VM_CONTEXT_BUFFER_WRITE (NULL, (void *)exitOpcode,
				     &pIntrStub[EXT_EXIT_OFF],
				     sizeof (exitOpcode)) != OK)
	    return (VXDBG_ERR_INIT);

//	CACHE_TEXT_UPDATE ((void *)&pIntrStub[EXT_EXIT_OFF],sizeof(exitOpcode));
	taCacheTextUpdata ((void *)&pIntrStub[EXT_EXIT_OFF],sizeof(exitOpcode));
	}
    else
	{
    	printk("\n taDbgCpuCtrlEnable: else\n");
	/* Save origin exit opcode */

	origExitOpcode[0] = pIntrStub[EXIT_OFF];

	/* compute opcode to branch to VXDBG_CPU_CTRL_EXIT_RTN */

	if (exitOpcode[0] == 0)	/* Compute the opcode only once */
	    exitOpcode[0] = ppcBrCompute (VXDBG_CPU_CTRL_EXIT_RTN,
				       &pIntrStub[EXIT_OFF], 1);

	/* install VXDBG_CPU_CTRL_EXIT_RTN */

	if (VM_CONTEXT_BUFFER_WRITE (NULL, (void *)exitOpcode,
				     &pIntrStub[EXIT_OFF],
				     sizeof (INSTR)) != OK)
	    return (VXDBG_ERR_INIT);

	/* Update text cache for the updated region */

//	CACHE_TEXT_UPDATE ((void *)&pIntrStub[EXIT_OFF], sizeof(INSTR));
	taCacheTextUpdata((void *)&pIntrStub[EXIT_OFF], sizeof(INSTR));
	}

    /* Indicate that VxDBG CPU control is enabled */

//    vxAtomicSet (&vxdbgCpuCtrlEnabled, TRUE);

    return (VXDBG_OK);
    }

/*******************************************************************************
*
* vxdbgCpuCtrlDisable - disable VxDBG CPU control
*
* This routine disables the VxDBG CPU contol by removing intVxdbgCpuRegsGet()
* from the interrupt handling mechanism.
*
* This routine must only be invoked from vxdbgCpuAllResume() when all the
* slave CPU(s) are executing the VxDBG CPU control loop (waiting for resume
* notification with interrupts locked).
*
* RETURNS: A VxDBG status (VXDBG_STATUS):
*       VXDBG_OK if CPU control is successfully enabled.
*       VXDBG_ERR_INIT if failed to remove intVxdbgCpuRegsGet().
*
* ERRNO: N/A
*
* NOMANUAL
*/

VXDBG_STATUS taDbgCpuCtrlDisable (void)
    {
    INSTR *		pIntrStub = (INSTR *)_EXC_OFF_INTR;

//    if (excExtendedVectors == TRUE)
    if (TRUE)
	{
	/* restore origin exit opcodes to branch to intExit() */

	if (VM_CONTEXT_BUFFER_WRITE (NULL, (void *)origExitOpcode,
				    &pIntrStub[EXT_EXIT_OFF],
				    sizeof (origExitOpcode)) != OK)
	    return (VXDBG_ERR_INIT);

	/* Update text cache for the updated region */

//	CACHE_TEXT_UPDATE ((void *)&pIntrStub[EXT_EXIT_OFF],sizeof (origExitOpcode));
	taCacheTextUpdata ((void *)&pIntrStub[EXT_EXIT_OFF],sizeof (origExitOpcode));
	}
    else
	{
	/* restore origin exit opcode to branch to intExit() */

	if (VM_CONTEXT_BUFFER_WRITE (NULL, (void *)origExitOpcode,
				     &pIntrStub[EXIT_OFF],
				     sizeof (INSTR)) != OK)
	    return (VXDBG_ERR_INIT);

	/* Update text cache for the updated region */

//	CACHE_TEXT_UPDATE ((void *)&pIntrStub[EXIT_OFF], sizeof(INSTR));
	taCacheTextUpdata ((void *)&pIntrStub[EXIT_OFF], sizeof(INSTR));
	}


    /* Indicate that VxDBG CPU control is disabled */

//    vxAtomicSet (&vxdbgCpuCtrlEnabled, FALSE);

    return (VXDBG_OK);
    }
