/* bootAppExc.c - Boot loader application exception handler component */

/* 
*  Copyright (c) 2005, 2007, 2009 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
01d,16dec09,pgh  64-bit conversion
01c,03apr09,zl   corrected excHookAdd() argument
01c,28apr09,pgh  Update documentation.
01b,06feb07,jmt  Add bootAppPrintf to optimize size
01a,07jul05,jmt   written
*/

/*
DESCRIPTION
This module contains the support routines for the VxWorks boot loader 
application exception handler component.  This module is added to the boot 
loader application when the INCLUDE_BOOT_EXC_HANDLER component is included.  
This module consists of an initialization routine which installs a routine 
to handle exceptions for the boot loader application, by executing the 
excHookAdd() routine.  The exception handler routine displays task information 
for the task that suffered the exception, and reboots the target.

INCLUDE FILES: bootApp.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <excLib.h>
#include <rebootLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <trcLib.h>

#include <bootApp.h>

/* defines */

/* globals */

/* locals */

/* forward declarations */

STATUS bootAppExcInit(void);
LOCAL void bootExcHandler (TASK_ID tid, int vecNum, void * pEsf);

/* local command handlers */


/*******************************************************************************
*
* bootAppExcInit - Initializes the boot loader application exception handler
*
* This routine adds an exception handling routine for the boot loader
* application.
*
* RETURNS: OK
*
* ERRNO: N/A
*
* \NOMANUAL
*/
STATUS bootAppExcInit()
    {
    /* setup Boot Exception Handler */

    excHookAdd (bootExcHandler);
    
    return (OK);
    }

/*******************************************************************************
*
* bootExcHandler - Boot loader application exception handling routine
*
* This routine is executed when an exception occurs in the boot loader 
* application.  This routine displays task information for the task that 
* suffered the exception, and reboots the target.
*
* RETURNS: OK
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void bootExcHandler
    (
    TASK_ID tid,	/* task ID */
    int vecNum,		/* vector number, ignored */
    void * pEsf		/* exception stack fram, ignored */
    )
    {
    REG_SET regSet;       /* task's registers */

    /* get registers of task to be traced */

    if (taskRegsGet (tid, &regSet) != ERROR)
        {
        trcStack (&regSet, (FUNCPTR) NULL, tid);
        taskRegsShow (tid);
        }
    else
        bootAppPrintf ("bootExcHandler: exception caught but no valid task.\n",0,0,0,0,0,0);

    taskDelay (sysClkRateGet ());       /* pause a second */

    reboot (BOOT_NO_AUTOBOOT);
    }

