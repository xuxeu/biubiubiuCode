/* bootFdLoad.c - Boot loader application floppy disk file system component */

/* 
 *  Copyright (c) 2005-2009 Wind River Systems, Inc.
 * 
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01i,11dec09,pgh  64-bit conversion
01h,03aug09,pgh  Fix boot device parsing.
01g,21apr09,pgh  Fix the parsing of boot devices.
01f,07jul08,pgh  Add support for SATA.
01e,24feb07,jmt  Cleanup boot device string requirements
01d,06feb07,jmt  Add bootAppPrintf to optimize size
                 Replace fdLoad with default bootFsFileLoad
01c,29jan07,jmt  Add call to bootDevFormatHelpAdd
01b,16aug06,jmt  Remove dosFsInit calls
01a,20jul05,jmt  written
*/

/*
DESCRIPTION
This module contains the support routines for the VxWorks boot loader 
application floppy disk file system component.  This module is added to 
the boot loader application when the INCLUDE_BOOT_FD_LOADER component is 
added.  This file is included for backward compatibility support of the 
'fd' or floppy disk boot device.  This file, and the 'fd' boot device, 
are no longer required because the bootFsLoad.c module is now used for 
loading from any file system by specifying the 'fs' or file system boot 
device.  The distinction between the various file system devices is made 
in the bootFile boot line parameter.  The name of the specific file system 
device is included in the full path and file name of the bootFile boot line 
parameter.  The full path and file name are passed to the I/O system, which 
takes care of opening the correct file system device.

INCLUDE FILES: bootFsLoad.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <bootLoadLib.h>
#include <stdio.h>
#include <string.h>
#include <sysLib.h>
#include <usrConfig.h>

#include <bootApp.h>
#include <bootFsLoad.h>


/* defines */

/* typedefs */

/* globals */

IMPORT STATUS   fdDrv (UINT, UINT);

/* local declarations */

static char * pFdDevName;

/* forward declarations */

STATUS bootFdLoadInit (char *);
LOCAL STATUS bootFsLoadFd (char * bootString, void **pEntry);

/*******************************************************************************
*
* bootFdLoadInit - Initialize floppy disk file system load component
*
* Initializes the boot loader application's floppy disk file system load 
* components.  This includes registering the floppy disk file load handler 
* with the boot loader application.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS bootFdLoadInit
    (
    char * pDevName        /* Device name */
    )
    {
    STATUS status;
    
    /* Save device name */
    pFdDevName = pDevName;
    
    /* register network loader component */
    status = bootLoaderAdd((bootLoaderPrototype *) bootFsLoadFd);
    
    return status;
    }

/*******************************************************************************
*
* bootFsLoadFd - File system floppy disk load handler
*
* This routine is the floppy disk file system load handler that is registered 
* with the boot loader application to load files from floppy disk.  This 
* routine loads the file specified by the boot line parameters.
* 
* RETURNS:
*   OK if file loaded successfully,
*   ERROR if an error occurs while loading the file
*   BOOT_STATUS_CONTINUE if the boot device is not a floppy disk
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL STATUS bootFsLoadFd
    (
    char *  bootString,
    void ** pEntry
    )
    {
    STATUS status = BOOT_STATUS_CONTINUE;
    char    bootDev[BOOT_DEV_LEN];
    int     i;

    /* Extract the new boot device name. */

    for (i = 0; i < BOOT_DEV_LEN; i++)
        {
        if (isalpha ((int)sysBootParams.bootDev[i]))
            {
            bootDev[i] = sysBootParams.bootDev[i];
            }
        else
            {
            bootDev[i] = 0;
            break;
            }
        }
    
    if (strcmp (bootDev, "fd") == 0)
        {
        /* load boot file */
        if (bootFsFileLoad (sysBootParams.bootFile, pEntry) == OK)
            {
            status = OK;
            }
        else
            {
            bootAppPrintf ("\nError loading file: errno = 0x%x.\n", 
                            (_Vx_usr_arg_t)errno,0,0,0,0,0);
            status = ERROR;
            }
        }

    return (status);
    }
    
