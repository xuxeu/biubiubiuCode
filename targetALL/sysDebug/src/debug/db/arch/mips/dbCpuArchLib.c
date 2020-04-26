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
01j,25sep08,jmp  fixed intExit opcode saving (CQ:WIND00135859).
01i,05jul08,slk  fix excIntStub_intExitOpAddr to properly handle sda sections
01h,11feb08,jmp  changed STATUS to VXDBG_STATUS for vxdbg routines
                 (CQ:WIND00116500).
		 added missing type for vxdbgCpuCtrlHookRtnInstalled boolean.
01g,31oct07,jmp  made vxdbgCpuCtrlEnabled an atomic variable.
01b,19sep07,jmp  removed direct references to cacheTextLocalUpdate()
                 (CQ:WIND00105194).
01a,15may07,jmp  written.
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
interrupted by a debugger IPI. On MIPS, this is done by updating the
excIntStub() to exit the interrupt handling using intVxdbgCpuRegsGet()
routine rather than the default intExit() routine.

The excIntStub() update is performed by replacing the instruction
"j intExit" which is identified by <excIntStub_intExitOpAddr> label
by the instruction "j intVxdbgCpuRegsGet" which is identified by
<excIntStub_intVxdbgCpuRegsGetOpAddr> label.

For a global overview of the VxDBG CPU Control, please refer to vxdbgCpuLib.c
module description.

INCLUDE FILES: private/vxdbgCpuLibP.h
*/

/* includes */
#ifndef _KERNEL_DEBUG_
#include <vxWorks.h>
#include <cacheLib.h>
#include <private/vmLibP.h>
#include <private/vxdbgCpuLibP.h>
#include <arch/mips/private/vxdbgMipsLibP.h>

/* locals */
LOCAL INSTR origExitOpcode;	/* origin intExit opcode */

/* forward declarations */

LOCAL void vxdbgCpuCacheTextUpdateHookRtn (void); /* VxDBG CPU ctrl hook rtn */

/*******************************************************************************
*
* vxdbgCpuCtrlEnable - enable CPU control
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
*	VXDBG_OK if CPU control is successfully enabled.
*	VXDBG_ERR_INIT if failed to install intVxdbgCpuRegsGet().
*	Error values returned by vxdbgCpuSuspendHookAdd().
*	Error values returned by vxdbgCpuResumeHookAdd().
*
* ERRNO: N/A
*
* NOMANUAL
*/
VXDBG_STATUS taDbgCpuCtrlEnable (void)
    {
    LOCAL BOOL vxdbgCpuCtrlHookRtnInstalled = FALSE;
    VXDBG_STATUS status;
#if 0 
//xucq Add if 0
    if (! vxdbgCpuCtrlHookRtnInstalled)
	{
	/*
	 * Install MIPS specific hook to be exectuted when entering and
	 * leaving VxDBG CPU control loop.
	 */

	if ((status = vxdbgCpuSuspendHookAdd (vxdbgCpuCacheTextUpdateHookRtn))
								!= VXDBG_OK)
	    return (status);

	if ((status = vxdbgCpuResumeHookAdd (vxdbgCpuCacheTextUpdateHookRtn))
								!= VXDBG_OK)
	    return (status);

	vxdbgCpuCtrlHookRtnInstalled = TRUE;	/* Do this only once */
	}
#endif
    /* Save current intExit() branch instruction before updating it */

    origExitOpcode = *((INSTR *) excIntStub_intExitOpAddr);

    /*
     * Install intVxdbgCpuRegsGet() by copying "j intVxdbgCpuRegsGet"
     * instruction located at &excIntStub_intVxdbgCpuRegsGetOpAddr
     * over "j intExit" instruction at &excIntStub_intExitOpAddr.
     */

    if (VM_CONTEXT_BUFFER_WRITE (NULL,
				 &excIntStub_intVxdbgCpuRegsGetOpAddr,
				 &excIntStub_intExitOpAddr,
				 sizeof (INSTR)) != OK)
	return (VXDBG_ERR_INIT);

    /*
     * Update text cache on local cpu only (Prevent CPC generation).
     * The cache update will be performed on other CPUs using a VxDBG
     * CPU suspend hook (See vxdbgCpuCacheTextUpdateHookRtn()).
     */

    CACHE_TEXT_LOCAL_UPDATE (&excIntStub_intExitOpAddr, sizeof(INSTR));

    /* Indicate that VxDBG CPU control is enabled */
#if 0
//xucq Add if 0
    vxAtomicSet (&vxdbgCpuCtrlEnabled, TRUE);
#endif

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
*	VXDBG_OK if CPU control is successfully enabled.
*	VXDBG_ERR_INIT if failed to remove intVxdbgCpuRegsGet().
*
* ERRNO: N/A
*
* NOMANUAL
*/

VXDBG_STATUS taDbgCpuCtrlDisable (void)
    {
    /*
     * Uninstall intVxdbgCpuRegsGet() by restoring "j intExit" instruction
     * previously saved by vxdbgCpuCtrlEnable().
     */

    if (VM_CONTEXT_BUFFER_WRITE (NULL,
				 &origExitOpcode,
				 &excIntStub_intExitOpAddr,
				 sizeof (INSTR)) != OK)
	return (VXDBG_ERR_INIT);

    /*
     * Update text cache on local cpu only (Prevent CPC generation).
     * The cache update will be performed on other CPUs using a VxDBG
     * CPU resume hook (See vxdbgCpuCacheTextUpdateHookRtn()).
     */

    CACHE_TEXT_LOCAL_UPDATE (&excIntStub_intExitOpAddr, sizeof(INSTR));

    /* Indicate that VxDBG CPU control is disabled */
#if 0
//xucq Add if 0
    vxAtomicSet ((atomic_t *)&vxdbgCpuCtrlEnabled, FALSE);
#endif

    return (VXDBG_OK);
    }

/*******************************************************************************
*
* vxdbgCpuCacheTextUpdateHookRtn - hook routine for VxDBG CPU control
*
* This routine is a VxDBG CPU control hook routine that is executed by slave
* CPUs before entering the VxDBG CPU control loop (VxDBG suspend hook), and
* when leaving the VxDBG CPU control loop (VxDBG resume hook). 
*
* The purpose of this hook is to update the text cache on local CPU following
* the code update performed by vxdbgCpuCtrlEnable() and vxdbgCpuCtrlDisable().
*
* This hook routine is installed on first call to vxdbgCpuCtrlEnable().
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL void taDbgCpuCacheTextUpdateHookRtn (void)
    {
    /* Update text cache on local CPU */

    CACHE_TEXT_LOCAL_UPDATE (&excIntStub_intExitOpAddr, sizeof(INSTR));
    }
#endif
