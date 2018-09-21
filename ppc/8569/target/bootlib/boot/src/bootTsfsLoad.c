/* bootTsfsLoad.c - Boot loader application TSFS file system load component */

/* 
*  Copyright (c) 2005, 2007-2009, 2012 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
01h,10jan12,pgh  Fix sprintf() usage().
01g,11dec09,pgh  64-bit conversion
01f,03aug09,pgh  Fix boot device parsing.
01e,21apr09,pgh  Fix the parsing of boot devices.
01d,07jul08,pgh  Add support for SATA.
01c,25feb07,jmt  Modify during integration with IPNet
01b,06feb07,jmt  Add bootAppPrintf to optimize size
01a,20jul05,jmt   written
*/

/*
DESCRIPTION
This module contains the support routines for the VxWorks boot loader 
application Target Server File System (TSFS) component.  This module is 
added to the boot loader application when the INCLUDE_BOOT_TSFS_LOADER 
component is included.  This module contains an initialization routine 
for the TSFS file system component, and the TSFS file load handler.  The 
routine bootLoaderAdd() is executed to add the TSFS file load handler to 
the boot loader application.

INCLUDE FILES: bootFsLoad.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <bootLoadLib.h>
#include <wdb/wdbEvtLib.h>
#include <string.h>
#include <sysLib.h>
#include <usrConfig.h>

#include <bootApp.h>
#include <bootFsLoad.h>


/* defines */

/* typedefs */

/* globals */

/* local declarations */

/* forward declarations */

STATUS bootTsfsLoadInit (void);
void bootFsPrintDevsTsfs();
LOCAL STATUS bootFsLoadTsfs (char * bootString, void ** pEntry);
#if (WDB_COMM_TYPE != WDB_COMM_SERIAL)
LOCAL void wdbRebootEventGet (void * pNode, WDB_EVT_DATA * pEvtData);
#endif  /* WDB_COMM_TYPE != WDB_COMM_SERIAL */

/*******************************************************************************
*
* bootTsfsLoadInit - Initialize TSFS file system load component
*
* This routine initializes the boot loader application TSFS file load 
* component, and registers the TSFS file load handler with the boot loader 
* application.
* 
* RETURNS:  
*   OK if the TSFS file load handler was registered
*   ERROR if the TSFS file load handler was not registered
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS bootTsfsLoadInit (void)
    {
    STATUS status;
    
    /* register the TSFS file load handler */
    status = bootLoaderAdd((bootLoaderPrototype *) bootFsLoadTsfs);
    
    return status;
    }

/*******************************************************************************
*
* bootFsLoadTsfs - TSFS file load handler
*
* This routine is the TSFS file system load handler that is registered with 
* the boot loader application to load files from the TSFS file system.  This 
* routine verifies that the boot device specified in the boot line parameters 
* is the TSFS, and if so, it loads the file specified by the boot string.
* 
* RETURNS: 
*   OK if file loaded successfully,
*   ERROR if an error occurs while loading the file
*   BOOT_STATUS_CONTINUE if the boot device is not a TSFS device
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL STATUS bootFsLoadTsfs
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
    
    if (strcmp (bootDev, "tsfs") == 0)
        {
#if (WDB_COMM_TYPE != WDB_COMM_SERIAL)
        WDB_EVT_NODE rebootEventNode;
#endif  /* WDB_COMM_TYPE != WDB_COMM_SERIAL */
        char corefile [PATH_MAX + 1];

        /* add a leading slash if the filename is a relative path */

        if ((sysBootParams.bootFile[0] != '/') && 
            (sysBootParams.bootFile[0] != '\\'))
            {
            (void)snprintf (corefile, (PATH_MAX + 1), "/tgtsvr/%s", 
                            sysBootParams.bootFile);
            }
        else
            (void)snprintf (corefile, (PATH_MAX + 1), "%s", 
                            sysBootParams.bootFile);

        /* wait for Target Server connection */

        bootAppPrintf (" Waiting for Target Server connection.\n", 0,0,0,0,0,0);
        while (!wdbTargetIsConnected())
            taskDelay (sysClkRateGet());
        bootAppPrintf (" Done.\n", 0,0,0,0,0,0);

        if (bootFsFileLoad (corefile, pEntry) != OK)
            {
            bootAppPrintf ("\nError loading file: errno = 0x%x.\n", 
                            (_Vx_usr_arg_t)errno, 0,0,0,0,0);
            bootAppPrintf ("Make sure that your Target Server is started with -R[oot] option.\n", 0,0,0,0,0,0);
            status = ERROR;
            }
        else
            {
#if (WDB_COMM_TYPE != WDB_COMM_SERIAL)
            /* Notify the host of the target reboot */
	
            wdbEventNodeInit (&rebootEventNode, wdbRebootEventGet, NULL, NULL);
            wdbEventPost (&rebootEventNode);
	
            /* let some time to WDB to post the event */
	
            taskDelay (sysClkRateGet() / 10);
#endif  /* WDB_COMM_TYPE != WDB_COMM_SERIAL */

            status = OK;
            }
        }
 
    return (status);
    }

#if (WDB_COMM_TYPE != WDB_COMM_SERIAL)
/******************************************************************************
*
* wdbRebootEventGet - dummy eventGet routine to force a target server restart
*
* This routine suspends the WDB task so the target server will get an 
* RPC_SYSTEMERROR while trying to get an event, so it will restart and try to 
* re-attach to the target.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void wdbRebootEventGet
    (
    void *      pNode,
    WDB_EVT_DATA *  pEvtData
    )
    {
    taskSuspend (0);
    }
#endif  /* WDB_COMM_TYPE != WDB_COMM_SERIAL */

