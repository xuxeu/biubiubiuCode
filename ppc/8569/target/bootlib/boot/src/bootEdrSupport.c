/* bootEdrSupport.c - Boot loader application ED&R support component */

/* 
*  Copyright (c) 2007,2009,2011 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
01e,17nov11,rlg  fix WIND00318015 - remove sys debug flag help msg now more
                 generic not solely based on EDR
01d,15dec09,pgh  64-bit conversion
01c,27apr09,pgh  Update documentation.
01b,10apr07,jmt  Change print command to a P instead of D
01a,28feb07,jmt  written
*/

/*
DESCRIPTION
This module contains support routines for the VxWorks boot loader application 
ED&R component.  This module is added to the boot loader application when the 
INCLUDE_BOOT_EDR_SUPPORT component is included.  This module contains an 
initialization routine, which adds help for an additional boot flag, and 
adds the following commands to the boot shell:

\is
\i Print ED&R error log
The 'P' command displays the contents of the ED&R error log.
\i Clear ED&R error log
The 'C" command clears the contents of the ED&R error log.
\ie

The commands are added to the boot shell by calling the bootCommandHandlerAdd()
routine.  The boot flag help is added by executing the bootFlagHelpAdd() 
routine.

INCLUDE FILES: vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <edrLib.h>

#include <bootApp.h>

/* defines */

/* typedefs */

/* globals */

/* forward declarations */

void bootAppEdrInit (void);
LOCAL int bootAppEdrLogDisplay (char * cmd);
LOCAL int bootAppEdrLogClear (char * cmd);

/*******************************************************************************
*
* bootAppEdrInit - Initialize boot loader application ED&R support component
*
* This routine initializes the boot loader application ED&R component.  It 
* adds two ED&R commands to the boot shell, and adds help for an ED&R boot flag.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void bootAppEdrInit (void)
    {
    /* setup command handlers */
    
    bootCommandHandlerAdd("P", bootAppEdrLogDisplay, BOOT_CMD_MATCH_STRICT,
         "P", "- print error log");
    bootCommandHandlerAdd("C", bootAppEdrLogClear, BOOT_CMD_MATCH_STRICT,
         "C", "- clear error log");
         
    }

/* Local functions */

/*******************************************************************************
*
* bootAppEdrLogDisplay - Display the contents of the ED&R error log
*
* This routine is the handler for the boot shell 'P' command.  This routine
* calls the edrShow() routine to display the contents of the ED&R error log.
* 
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppEdrLogDisplay
    (
    char * cmd
    )
    {
    edrShow (0,0,0,0);
    return BOOT_STATUS_COMPLETE;
    }

/*******************************************************************************
*
* bootAppEdrLogClear - Clear the contents of the ED&R error log
*
* This routine is the handler for the boot shell 'C' command.  This routine
* calls the edrClear() routine to clear the contents of the ED&R error log.
* 
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppEdrLogClear
    (
    char * cmd
    )
    {
    edrClear ();
    return BOOT_STATUS_COMPLETE;
    }
