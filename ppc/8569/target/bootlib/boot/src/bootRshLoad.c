/* bootRshLoad.c - Boot loader application RSH file load support component */

/* 
*  Copyright (c) 2005, 2007, 2009-2010, 2012 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
01f,10jan12,pgh  Fix sprintf() usage.
01e,02mar10,pgh  Save ELF file class ELF32 or ELF64
01d,11dec09,pgh  64-bit conversion
01c,29apr09,pgh  Update documentation.
01b,06feb07,jmt  Add bootAppPrintf to optimize size
01a,11jul05,jmt   written
*/

/*
DESCRIPTION
This module contains the support routines for the VxWorks bootrom loader 
application RSH file load component.  This module is added to the boot 
loader application when the INCLUDE_BOOT_RSH_LOADER component is included.  
This module consists of an initialization routine for the RSH file load 
component, and the RSH file load handler.  The RSH file load handler is 
installed by initializing the bootRshLoaderFcn routine pointer with the 
address of the RSH file load handler.  The bootRshLoaderFcn routine 
pointer is executed by the bootNetLoad.c module when this routine pointer 
is not NULL.

INCLUDE FILES: bootApp.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <bootLib.h>
#include <bootLoadLib.h>
#include <fioLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <remLib.h>
#include <stdio.h>

#include <bootApp.h>
#include <bootNetLoad.h>

/* defines */

/* typedefs */

#define RSHD        514 /* rshd service */

/* globals */

extern bootNetLoadPrototype * bootRshLoaderFcn;
extern int bootElfClass;        /* ELF file class: ELF32 or ELF64 */

/* forward declarations */

void bootRshLoadInit(void);
LOCAL STATUS    bootRshLoad (char *hostName, char *fileName, char *usr,
             char *passwd, void **pEntry);


/*******************************************************************************
*
* bootRshLoadInit - Initialize RSH file load component
*
* This routine initializes the boot loader application RSH file load component.
* This routine registers the RSH file load handler with the bootNetLoad.c 
* module.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void bootRshLoadInit()
    {
    /* setup RSH loader function pointer */
    bootRshLoaderFcn = (bootNetLoadPrototype *) bootRshLoad;
    }

/*******************************************************************************
*
* bootRshLoad - Boot loader application RSH file load handler
*
* This routine is the RSH file load handler that is registered with the boot
* loader application to load files from a remote host machine via the RSH 
* protocol.  The remote shell daemon on the machine 'host' is used to load the 
* file specified by the boot parameters.
*
* RETURNS: 
*   OK if the file is loaded via RSH
*   ERROR if the file is not found or could not be loaded via RSH
*
* \NOMANUAL
*/

LOCAL STATUS bootRshLoad
    (
    char *  hostName,   /* name of remote host */
    char *  fileName,   /* name of file to be loaded */
    char *  usr,        /* user ID */
    char *  passwd,     /* password */
    void ** pEntry      /* address to start execution of loaded file */
    )
    {
    int     fd;
    int     errFd;      /* for receiving standard error messages from Unix */
    char    command [BOOT_FILE_LEN + BOOT_HOST_LEN];

    (void)snprintf(command, (BOOT_FILE_LEN + BOOT_HOST_LEN), "cat %s", 
                   fileName);
    fd = rcmd (hostName, RSHD, usr, usr, command, &errFd);
    if (fd == ERROR)
        {
        bootAppPrintf("rcmd transfer failed\n",0,0,0,0,0,0);
        return (ERROR);
        }

    if (bootLoadModule (fd, pEntry, &bootElfClass) != OK)
	{
    	bootAppPrintf("File load failed\n",0,0,0,0,0,0);
        goto readErr;
	}
		
	/* close the file descriptors */
	close (fd); 
	close (errFd);
	
	return (OK);

readErr:
    {
    /* check standard error on Unix */
	
    char buf [BOOT_FILE_LEN + BOOT_HOST_LEN];
    int errBytesRecv = (int)fioRead (errFd, buf, sizeof (buf));
	
    if (errBytesRecv > 0)
        {
        /* print error message on standard error fd */
	
        buf [errBytesRecv] = EOS;
        bootAppPrintf ("\n%s:%s: %s\n", 
                       (_Vx_usr_arg_t)hostName, 
                       (_Vx_usr_arg_t)fileName, 
                       (_Vx_usr_arg_t)buf, 0,0,0);
        }

    close (fd);
    close (errFd);
    }
    return (ERROR);
    }

