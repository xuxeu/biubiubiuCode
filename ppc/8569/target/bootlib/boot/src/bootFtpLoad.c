/* bootFtpLoad.c - Boot loader application FTP file load support */

/* 
*  Copyright (c) 2005,2007,2009-2010 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
01e,02mar10,pgh  Save ELF file class ELF32 or ELF64
01d,11dec09,pgh  64-bit conversion
01c,27apr09,pgh  Update documentation.
01b,06feb07,jmt  Add bootAppPrintf to optimize size
01a,11jul05,jmt   written
*/

/*
DESCRIPTION
This module contains the support routines for the VxWorks bootrom loader 
application FTP file load component.  This module is added to the boot 
loader application when the INCLUDE_BOOT_FTP_LOADER component is included.  
This module consists of an initialization routine for the FTP file load 
component, and the FTP file load handler.  The FTP file load handler is 
installed by initializing the bootFtpLoaderFcn routine pointer with the 
address of the FTP file load handler.  The bootFtpLoaderFcn routine pointer 
is executed by the bootNetLoad.c module when this routine pointer is not NULL.

INCLUDE FILES: bootApp.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <bootLib.h>
#include <bootLoadLib.h>
#include <ftpLib.h>
#include <ioLib.h>

#include <bootApp.h>
#include <bootNetLoad.h>

/* defines */

/* typedefs */

/* globals */

extern bootNetLoadPrototype * bootFtpLoaderFcn;
extern int bootElfClass;        /* ELF file class: ELF32 or ELF64 */

/* forward declarations */

void bootFtpLoadInit(void);
LOCAL STATUS bootFtpLoad (char *hostName, char *fileName, char *usr,
             char *passwd, void **pEntry);


/*******************************************************************************
*
* bootFtpLoadInit - Initialize FTP file load component
*
* This routine initializes the boot loader application FTP file load component.
* This routine registers the FTP file load handler with the boot loader 
* application.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void bootFtpLoadInit()
    {
    /* setup FTP loader function pointer */
    bootFtpLoaderFcn = (bootNetLoadPrototype *) bootFtpLoad;
    }

/*******************************************************************************
*
* bootFtpLoad - Load a file from a remote host via FTP.
*
* This routine is the FTP file load handler that is registered with the boot
* loader application to load files from a remote host machine via the FTP 
* protocol.  The remote shell daemon on the FTP server is used to download the 
* specified file to the specified previously opened network file descriptor.  
* The remote user ID should have been set previously by a call to iam().  If 
* the file does not exist, an error message from the FTP server is printed to 
* the VxWorks standard error fd and ERROR is returned.
*
* RETURNS: 
*   OK if the file was loaded.
*   ERROR if the file could not be loaded.
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL STATUS bootFtpLoad
    (
    char *  hostName,   /* Name of the FTP server */
    char *  fileName,   /* Name of file to load */
    char *  usr,        /* FTP user ID */
    char *  passwd,     /* FTP user password */
    void ** pEntry      /* Starting execution address */
    )
    {
    int fd;
    int errFd;      /* for receiving standard error messages from Unix */
    char command [BOOT_FILE_LEN + BOOT_HOST_LEN];

    if (ftpXfer (hostName, usr, passwd, "", "RETR %s", "", fileName,
                 &errFd, &fd) == ERROR)
        {
        bootAppPrintf("FTP transfer failed\n",0,0,0,0,0,0);
        return (ERROR);
        }

    if (bootLoadModule (fd, pEntry, &bootElfClass) != OK)
        {
        bootAppPrintf("File load failed\n",0,0,0,0,0,0);
        goto readErr;
        }

    /* Empty the Data Socket before close. PC FTP server hangs otherwise */

    while ((read (fd, command, sizeof (command))) > 0);
    
    /* close the data socket before checking for a ftp reply (SPR #77169) */

    close (fd);

    (void) ftpCommand (errFd, "QUIT",0,0,0,0,0,0);

    close (errFd);
    return (OK);

readErr:
    /* check standard error on Unix */

    /* Empty the Data Socket before close. PC FTP server hangs otherwise */

    while ((read (fd, command, sizeof (command))) > 0);

    /* close the data socket before checking for a ftp reply (SPR #77169) */

    close (fd);

    (void) ftpReplyGet (errFd, FALSE); /* error message on std. err */
    (void) ftpCommand (errFd, "QUIT",0,0,0,0,0,0);
    close (errFd);

    return (ERROR);
    }

