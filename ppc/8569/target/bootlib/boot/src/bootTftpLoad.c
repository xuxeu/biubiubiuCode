/* bootTftpLoad.c - Boot loader application TFTP file load support component */

/* 
*  Copyright (c) 2005, 2007, 2009-2010, 2014 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
07jul14,p_x  Fix overrun issue in dataBuf[num]
01e,02mar10,pgh  Save ELF file class ELF32 or ELF64
01d,11dec09,pgh  64-bit conversion
01c,29apr09,pgh  Update documentation.
01d,21apr09,hps  LP64: move drv_val_t to private header iosLibP.h
01c,14apr09,hps  LP64: iosFdValue now returns drv_val_t
01b,06feb07,jmt  Add bootAppPrintf to optimize size
01a,11jul05,jmt  written
*/

/*
DESCRIPTION
This module contains the support routines for the VxWorks bootrom loader application 
TFTP file load component.  This module is added to the boot loader application when 
the INCLUDE_BOOT_TFTP_LOADER component is included.  This module consists of 
an initialization routine for the TFTP file load component, and the TFTP file 
load handler.  The TFTP file load handler is installed by initializing the 
bootTftpLoaderFcn function pointer with the address of the TFTP file load 
handler.  The bootTftpLoaderFcn function pointer is executed by the 
bootNetLoad.c module when this routine pointer is not NULL.

INCLUDE FILES: bootApp.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <bootLib.h>
#include <bootLoadLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <private/iosLibP.h>
#include <tftpLib.h>
#include <sysLib.h>
#include <bootApp.h>
#include <bootNetLoad.h>

/* defines */

#define DATA_BUF_SIZE   512

/* typedefs */

/* globals */

extern bootNetLoadPrototype * bootTftpLoaderFcn;
extern int bootElfClass;        /* ELF file class: ELF32 or ELF64 */

/* forward declarations */

void bootTftpLoadInit(void);
LOCAL STATUS bootTftpLoad (char *hostName, char *fileName, char *usr,
                            char *passwd,  void **pEntry);


/*******************************************************************************
*
* bootTftpLoadInit - Initialize TFTP file load component
*
* This routine initializes the boot loader application TFTP file load 
* component, by registering the TFTP file load handler with the bootNetLoad.c
* module.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void bootTftpLoadInit()
    {
    /* setup TFTP Loader function pointer */
    bootTftpLoaderFcn = (bootNetLoadPrototype *) bootTftpLoad;
    }

/*******************************************************************************
*
* bootTftpLoad - Load a file from a remote host via TFTP.
*
* This routine is the TFTP file load handler that is registered with the boot
* loader application to load files from a remote host machine via the TFTP 
* protocol.  This routine uses the TFTP protocol to load the file specified 
* by the boot parameters from the remote host machine specified by the boot 
* parameters.
*
* RETURNS: 
*   OK if the file is loaded via TFTP
*   ERROR if the file does not exist on the remote host, or the transfer failed
*
* ERRNO: N/A
*
* \NOMANUAL
*/
#if 0
LOCAL STATUS bootTftpLoad
    (
    char *  hostName,
    char *  fileName,
    char *  usr,
    char *  passwd,
    void ** pEntry
    )
    {
    STATUS  rtv = OK;
    int     fd;
    int     errFd;      /* for receiving standard error messages from Unix */
    int     num;
    char    dataBuf [DATA_BUF_SIZE];

    rtv = tftpXfer (hostName, 0, fileName, "get", "binary", &fd, &errFd);
    if (rtv == ERROR)
        {
        bootAppPrintf("TFTP transfer failed\n",0,0,0,0,0,0);
        return (ERROR);
        }

    rtv = bootLoadModule (fd, pEntry, &bootElfClass);
    if (rtv != OK)
        {
    	bootAppPrintf("File load failed\n",0,0,0,0,0,0);
        goto readErr;
        }
		
    /*
     * Discard any remaining data from the TFTP server.
     */

    while (read (fd, dataBuf, sizeof (dataBuf)) > 0)
        {
        }
    close (fd);

readErr:
    /*
    * Get and display any error messages from TFTP server.
    */

    num = (int)read (errFd, dataBuf, sizeof (dataBuf));
    if (num > 0)
        {
        int dataBufSize = sizeof (dataBuf);

        if (num < dataBufSize)
            dataBuf[num] = '\0';
        else
            dataBuf[dataBufSize - 1] = '\0';

        bootAppPrintf("TFTP error: %s\n",(_Vx_usr_arg_t)dataBuf,0,0,0,0,0);
        }
    close (errFd);
    return (rtv);
    }
#endif
LOCAL STATUS bootTftpLoad
    (
    char *  hostName,
    char *  fileName,
    char *  usr,
    char *  passwd,
    void ** pEntry
    )
    {
    STATUS  rtv = OK;
    int     fd;
    int     errFd;      /* for receiving standard error messages from Unix */
    int     num;
    char    dataBuf [DATA_BUF_SIZE];
    BOOT_PARAMS tftpBootParams;
    bootStringToStruct (sysBootLine, &tftpBootParams);
    if(strcmp(tftpBootParams.Type,"bin")==0)
    					{
    					typeIsELForBIN=2;

    					}
    					else if(strcmp(tftpBootParams.Type,"elf")==0)
    					{
    					typeIsELForBIN=1;
    					}
    if(typeIsELForBIN==1 || typeIsELForBIN==0)
{
    rtv = tftpXfer (hostName, 0, fileName, "get", "binary", &fd, &errFd);
    if (rtv == ERROR)
        {
        bootAppPrintf("TFTP transfer failed\n",0,0,0,0,0,0);
        return (ERROR);
        }

    rtv = bootLoadModule (fd, pEntry, &bootElfClass);
    if (rtv != OK)
        {
    	bootAppPrintf("File load failed\n",0,0,0,0,0,0);
        goto readErr;
        }

    /*
     * Discard any remaining data from the TFTP server.
     */

    while (read (fd, dataBuf, sizeof (dataBuf)) > 0)
        {
        }
    close (fd);

readErr:
    /*
    * Get and display any error messages from TFTP server.
    */

    num = (int)read (errFd, dataBuf, sizeof (dataBuf));
    if (num > 0)
        {
        int dataBufSize = sizeof (dataBuf);

        if (num < dataBufSize)
            dataBuf[num] = '\0';
        else
            dataBuf[dataBufSize - 1] = '\0';

        bootAppPrintf("TFTP error: %s\n",(_Vx_usr_arg_t)dataBuf,0,0,0,0,0);
        }
	typeIsELForBIN=0;
    close (errFd);
}
else

{ M29DW256SectorErase(0x1c00000,63);
	char buf[1024]={0};
	UINT32 uOffset=0x1c00000;
	rtv = tftpXfer (hostName, 0, fileName, "get", "binary", &fd, &errFd);
    if (rtv == ERROR)
	{
		bootAppPrintf("TFTP transfer failed\n\n",0,0,0,0,0,0);
		return (ERROR);
	}
    printf("Programing...\n\n");
#if 0
    if(M29DW256ProgramBuffer(uOffset,buf,)!=OK)
    {
    	printf("error:write to 0x%X flash failed\n",uOffset);
    	return (ERROR);
    }
#endif


	while ((num = read (fd, buf, sizeof (buf))) > 0)
	{int numm;
		if(num%2==0)
		{
			numm=num;
		}
		else
		{
		numm=num+1;

		}

		if(M29DW256ProgramBuffer(uOffset,buf,numm/2)!=OK)
		{
			printf("error:write to 0x%X flash failed\n",uOffset);
			return (ERROR);
		}
		uOffset+=num;
		memset(buf,0,1024);
	}
	/*
	*pEntry=((volatile void*)(0xfe000000+0xf00));
	*/
	close (fd);
	num = read (errFd, buf, 1024);
	if (num > 0)
	{
		buf [num] = '\0';
		printf ("An error occurred!:%s\en", buf);
	}
	close (errFd);


    return (rtv);
}
    }


