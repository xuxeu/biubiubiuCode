/* bootFsLoad.c - Boot loader application file system load support component */

/* 
*  Copyright (c) 2005,2007-2010,2012-2014 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
07jul14,p_x  Add FS dev mounting check. (VXW6-25020)
01s,12aug12,mpc  Remove building warnings. (WIND00354175)
01r,02aug12,ljg  Add "Loading" message in bootUsbFileLoad (WIND00364639)
01q,27jul12,wqi  Fix the "ls" can't work in currect directory. (WIND00362556)
01p,09jul12,wqi  Fix the confusing message when loading file. (WIND00358690)
01o,05jan12,pgh  Add USB file system component support.
01n,04jan12,d_c  Fix warning in bootUsbFsLoad.c
01m,12nov10,pgh  Add USB file system support.
01l,02mar10,pgh  Save ELF file class ELF32 or ELF64
01k,11dec09,pgh  64-bit conversion
01j,03aug09,pgh  Fix boot device parsing.
01i,21apr09,pgh  Fix the parsing of boot devices.
01h,02dec08,pgh  Add file system support.
01g,07jul08,pgh  Add SATA disk boot support.
01f,02mar07,jmt  Move devs command from bootAppShell.c
01e,27feb07,jmt  Update help format
01d,06feb07,jmt  Add bootAppPrintf to optimize size
01c,29jan07,jmt  Add call to bootDevFormatHelpAdd
01b,23jan07,jmt  Remove devSplit routine, use version in usrExtra.c
01a,12may05,jmt   written
*/

/*
DESCRIPTION
This module contains the support routines for the VxWorks boot loader 
application file system component.  This module is added to the boot loader 
application when the INCLUDE_BOOT_FILESYSTEMS component is included.  This 
module consists of an initialization routine for the file system load 
component, a help string display routine, routines to load the file from the 
file system, and file system command handlers.  The help strings are added to 
the boot shell by the bootPrintDevsFuncAdd() and bootDevFormatHelpAdd() 
routines.  The file system shell commands are added to the boot shell by the 
bootCommandHandlerAdd() routine.  The file load handler is added to the boot 
loader application by the bootLoaderAdd() routine.  The file system command 
handlers provide support for the following boot shell commands:

\is
\i Print devices
The 'devs' command prints a list of the available devices in the boot 
loader application.
\i Change directory
The 'cd' command changes the current directory path to the path specified on 
the command line.
\i Print working directory
The 'pwd' command prints the current directory path.
\i Remove file
The 'rm' command removes the file specified on the command line.
\i List contents
The 'ls' command lists the contents of the specified directory path.  If no 
directory is specified, the current directory path contents are displayed.
\i Copy file
The 'cp' command copies the specified file from the source location to the specified destination.
\ie

This module also contains support for the VxWorks boot loader application USB 
file system component, which is enabled when the INCLUDE_BOOT_USB_FS_LOADER 
component is included.  Since it can take awhile for USB devices to be mounted,
the INCLUDE_BOOT_USB_FS_LOADER component has two timing parameters associated
with it.  The BOOT_USB_OPEN_ATTEMPTS parameter specifies the number of file
open attempts to make before giving up, and defaults to ten attempts.  The 
BOOT_USB_ATTEMPT_DELAY parameter specifies the number of system ticks to delay 
between file open attempts, and defaults to sixty ticks.

Note that mounting may consume much time for many FS devices besides USB. In this case 
enable BOOT_FS_DEV_MOUNT_CHECK to wait for device to be ready before loading image. 
Tune BOOT_FS_CHECK_ATTEMPTS and BOOT_FS_ATTEMPT_DELAY for specific devices.
Actually USB FS loader can be merged with it. Currently leave it there for backward compatibility.

INCLUDE FILES: bootFsLoad.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <iosLib.h>
#include <private/iosLibP.h>
#include <stat.h>
#include <usrConfig.h>
#include <bootApp.h>
#include <bootLoadLib.h>
#include <private/bootFsLoad.h>


/* defines */

#define FS_DEV_CHECK_ATTEMPTS   30    /* default FS device check attempts */
#define FS_DEV_CHECK_DELAY      60    /* default FS device check delay */

#define USB_OPEN_ATTEMPTS   10      /* USB open() attempts */
#define USB_OPEN_DELAY      60      /* USB open() delay */
#define USB_DEV_NAME_LEN    16      /* USB file system device name length */
#define USB_HELP_STR_LEN    80      /* USB file system help string length */

/* typedefs */

/* globals */

int bootUsbFsSupport = FALSE;   /* default to no USB files system support */
extern bootShellPrintfPrototype * bootShellPrintfFunc;
extern int bootElfClass;        /* ELF file class: ELF32 or ELF64 */

/* locals */

LOCAL BOOL   fsDevCheckEnable = TRUE; /* enable FS device mounting check */
LOCAL UINT32 fsDevCheckAttempts = FS_DEV_CHECK_ATTEMPTS; /* FS device check attempts */
LOCAL UINT32 fsDevCheckDelay = FS_DEV_CHECK_DELAY; /* FS device check delay */

int     usbOpenAttempts = USB_OPEN_ATTEMPTS;    /* USB open() call attempts */
int     usbOpenDelay = USB_OPEN_DELAY;          /* USB open() call delay */
char    usbFileSysName[USB_DEV_NAME_LEN + 1] = {'/','b','d','\0'};
char    usbFsHelpStr[USB_HELP_STR_LEN];         /* USB file systems help str */

/* forward declarations */

LOCAL void bootFsPrintDevs(void);
STATUS bootFsLoad (char *, void **);

/* local command handlers */

void bootFsLoadInit(void);
LOCAL int bootAppDevsShow (char * cmd);
LOCAL int bootAppChdir (char * cmd);
LOCAL int bootAppGetcwd (char * cmd);
LOCAL int bootAppRemove (char * cmd);
LOCAL int bootAppListDir (char * cmd);
LOCAL int bootAppCopyFile (char * cmd);

/*******************************************************************************
*
* bootFsLoadInit - Initialize file system load component
*
* Initializes the boot loader application file system load component.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void bootFsLoadInit()
    {
    /* Add print devs function */
    bootPrintDevsFuncAdd(bootFsPrintDevs);

    /* Add commands to boot shell */
    bootCommandHandlerAdd("devs", bootAppDevsShow, BOOT_CMD_MATCH_STRICT,
 		"devs", "- print system devices");
    bootCommandHandlerAdd("cd", bootAppChdir, BOOT_CMD_MATCH_STRICT,
 		"cd path", "- change current directory path");
    bootCommandHandlerAdd("pwd", bootAppGetcwd, BOOT_CMD_MATCH_STRICT,
 		"pwd", "- print current directory path");
    bootCommandHandlerAdd("rm", bootAppRemove, BOOT_CMD_MATCH_STRICT,
 		"rm file", "- remove file");
    bootCommandHandlerAdd("ls", bootAppListDir, BOOT_CMD_MATCH_STRICT,
 		"ls path", "- list directory path");
    bootCommandHandlerAdd("cp", bootAppCopyFile, BOOT_CMD_MATCH_STRICT,
 		"cp src tgt", "- copy file from src to tgt");

    /* register file system loader component */
    (void) bootLoaderAdd((bootLoaderPrototype *) bootFsLoad);

    /* add command help */
    bootDevFormatHelpAdd(
        "boot device: fs\n "
        "file name: /FileSystemDeviceName/vxWorks\n "
        "other: network device name\n");
    }

/***************************************************************************
*
* bootFsDevCheckConfig - Configure mounting check parameters
*
* This routine Configures mounting check parameters for boot file system dev.
* 
* RETURNS: OK
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS bootFsDevCheckConfig
    (
    BOOL    needCheck,      /* Need mount checking ? */
    UINT32  checkAttemps,   /* Number of checking attempts */
    UINT32  attemptDelay    /* Tick delay between checking attempts */
    )
    {
    fsDevCheckEnable = needCheck;
    fsDevCheckAttempts = checkAttemps;
    fsDevCheckDelay = attemptDelay;

    return OK;
    }

/***************************************************************************
*
* bootUsbFsLoadInit - Initialize boot loader USB file system load component
*
* This routine initializes the boot loader application USB file system load 
* component.  This routine is executed when the INCLUDE_BOOT_USB_FS_LOADER
* component is initialized.
* 
* RETURNS: OK
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS bootUsbFsLoadInit
    (
    int     usbAttempts,    /* Number of USB open() call attempts */
    int     usbTickDelay,   /* Tick delay between USB open() call attempts */
    char *  usbDriveName    /* name of USB file system device */
    )
    {
    /* save number of USB file open() call attempts */
    usbOpenAttempts = usbAttempts;

    /* save tick delay between USB file open() call attempts */
    usbOpenDelay = usbTickDelay;

    /* save name of USB file system device */
    strncpy(usbFileSysName, usbDriveName, USB_DEV_NAME_LEN);

    /* Set flag to indicate support for USB file systems */
    bootUsbFsSupport = TRUE;

    /* add command help */
    (void)snprintf(&usbFsHelpStr[0], USB_HELP_STR_LEN,
            "USB boot device: fs                   file name: %s[0-9]/vxWorks",
            usbDriveName);
    bootDevFormatHelpAdd(&usbFsHelpStr[0]);
    
    return OK;
    }

/*******************************************************************************
*
* bootFsPrintDevs - Displays file system devices
*
* This routine is executed by the boot shell to output the header for the list 
* of file system devices.  It follows this header with a list of each file 
* system device that is available in the system.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void bootFsPrintDevs()
    {
    STATUS          rtv;
    FAST DEV_HDR *  pDevHdr;
    struct stat     devData;
    char *          pStr;

    bootAppPrintf ("\n  File System Devices: ",0,0,0,0,0,0);
    for (pDevHdr = (DEV_HDR *) DLL_FIRST (&iosDvList);
            pDevHdr != NULL;
                pDevHdr = (DEV_HDR *) DLL_NEXT (&pDevHdr->node))
        {
        /* stat() won't return for tgtsvr if not running on host */

        pStr = strstr(pDevHdr->name, "tgtsvr");
        if (pStr != NULL)
            {
            bootAppPrintf ("%s ", (_Vx_usr_arg_t)pDevHdr->name,0,0,0,0,0);
            }
        else
            {
            rtv = stat (pDevHdr->name, &devData);
            if (rtv == OK)
                {
                /* File system device if a directory */

                if (S_ISDIR(devData.st_mode))
                    bootAppPrintf ("%s ", (_Vx_usr_arg_t)pDevHdr->name,
                                    0,0,0,0,0);
                }
            }
        }
    }

/******************************************************************************
*
* bootFsFileLoad - Load a VxWorks image file from a file system
*
* This routine loads a file from an already mounted file system.  This routine
* is called by the various file system device load handlers to load the VxWorks
* image file. Check mounting ready for some devices is necessary.
* 
* RETURNS: 
*   OK if the file is loaded.
*   ERROR if file can not be loaded.
*
* ERRNO: N/A
*/

STATUS bootFsFileLoad
    (
    char *  fileName,   /* path and file name */
    void ** pEntry      /* address to start execution */
    )
    {
    STATUS status = OK;
    int fd;
    BOOT_PARAMS fsBootParams;
    bootStringToStruct (sysBootLine, &fsBootParams);
	/* wait for slow device to mount if required */
	
    if (fsDevCheckEnable)
		{
		char * pSlashPos;
		char * pDevName = strdup(fileName);

		/* search for file system device name */

		pSlashPos = strchr(pDevName + 1, '/');
		
		if (pSlashPos != NULL)
			{
			char * pNameTail = NULL;
			UINT32 checkAttempts = fsDevCheckAttempts;

			/* extract file system device name */
			
			pDevName[pSlashPos - pDevName] = EOS;
			
			do
				{
				/* wait until file system device appears in the IO system */
				
				if (!((iosDevFind ((const char *)pDevName, (const char **)&pNameTail) == NULL) ||
							(pNameTail == pDevName)))
					{
					int fd;
					
					/* check if file system device is already initialized */
					
					if ((fd = open (fileName, O_RDONLY, 0)) != ERROR)
						{
						close(fd);
						break;
						}
					}

				if (checkAttempts == fsDevCheckAttempts)
					bootAppPrintf ("Waiting for \"%s\" ", (_Vx_usr_arg_t) pDevName, 
											0,0,0,0,0);

				if (checkAttempts > 1)
					{
					taskDelay(fsDevCheckDelay);
					bootAppPrintf (".", 0,0,0,0,0,0);
					}
				} while (--checkAttempts);

			/* check for timeout */
			
			if (checkAttempts <= 0)
				{
				bootAppPrintf (" failed.\n", 0,0,0,0,0,0);
				free(pDevName);
				return (ERROR);
				}
			else if (checkAttempts < fsDevCheckAttempts)
				bootAppPrintf ("\n", 0,0,0,0,0,0);
			}
		
		free(pDevName);
		}

    /* load the boot file */

    bootAppPrintf ("Loading %s...", (_Vx_usr_arg_t) fileName, 0,0,0,0,0);
    
    if ((fd = open (fileName, O_RDONLY, 0)) == ERROR)
        {
        bootAppPrintf ("\nCannot open \"%s\".\n", (_Vx_usr_arg_t) fileName, 
                        0,0,0,0,0);
        return (ERROR);
        }
    /*                   add loader for .bin                       */

   if(strcmp(fsBootParams.Type,"bin")==0)
   {
	char buf[1024]={0};
	int num;
	UINT32 uOffset=0x1c00000;

	M29DW256SectorErase(0x1c00000,63);
	printf("programing...\n");
	while ((num = read (fd, buf, sizeof (buf))) > 0)
		{
		int numm;
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
   }

    /*                   add loader for .bin                       */
   else if(strcmp(fsBootParams.Type,"elf")==0)
   {
   if (bootLoadModule (fd, pEntry, &bootElfClass) != OK)
        {
        bootAppPrintf ("\nFile load failed.\n", 0,0,0,0,0,0);
        status = ERROR;
        }
   }
   else
   {
	   printf("\nno this type\n");

   }

    close (fd);
    return (status);
    }


/******************************************************************************
*
* bootUsbFileLoad - Load a file from a USB file system device
*
* This routine loads a file from a USB file system device.  The name of the 
* USB device is included as the path name to the file to be loaded.  This 
* routine includes a retry loop with a delay to allow the USB device time to 
* get mounted.
*
* RETURNS: 
*   OK if the file is loaded.
*   ERROR if the file can not be loaded.
* 
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS bootUsbFileLoad
    (
    char *  fileName,   /* name of file to load */
    void ** pEntry      /* address to start execution of file */
    )
    {
    STATUS  rtv;
    int     fd = ERROR;
    int     openAttempts = 0;
    
    /* load the boot file */
 
    bootAppPrintf ("Loading %s...", (_Vx_usr_arg_t) fileName, 0,0,0,0,0);

    while ((fd == ERROR) && (openAttempts < usbOpenAttempts))
        {
        fd = open (fileName, O_RDONLY, 0);
        openAttempts++;
        if (fd == ERROR)
            {
            bootAppPrintf("Try to open \"%s\".  Attempt %d of %d.\n",
                          (_Vx_usr_arg_t)fileName,
                          (_Vx_usr_arg_t)openAttempts,
                          (_Vx_usr_arg_t)usbOpenAttempts,
                          0,0,0);
            if (openAttempts < usbOpenAttempts)
                taskDelay (usbOpenDelay);
            }
        }
    if (fd == ERROR)
        {
        bootAppPrintf ("\nCannot open \"%s\".\n", 
                       (_Vx_usr_arg_t)fileName,0,0,0,0,0);
        return (ERROR);
        }

    rtv = bootLoadModule (fd, pEntry, &bootElfClass);
    if (rtv == ERROR)
        {
        bootAppPrintf("File load failed\n",0,0,0,0,0,0);
        }

    close (fd);
    return (rtv);
    }


/*******************************************************************************
*
* bootFsLoad - File system load handler
*
* This routine is the file system load handler that is registered with the
* boot loader application to load files from file system devices.  It checks that the
* boot device is a file system device, and if so loads the file specified by
* the boot line parameters.
* 
* RETURNS: 
*   OK if file loaded successfully,
*   ERROR if an error occurs while loading the file
*   BOOT_STATUS_CONTINUE if the boot device is not a file system device
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS bootFsLoad
    (
    char *  bootString, /* boot command string */
    void ** pEntry      /* address to start execution */
    )
    {
    STATUS  status = BOOT_STATUS_CONTINUE;
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

    /* Verify the boot device is a file system device */

    if ((strcmp (bootDev, "fs") == 0) ||
        (strcmp (bootDev, "fd") == 0) ||
        (strcmp (bootDev, "ata") == 0) ||
        (strcmp (bootDev, "ide") == 0) ||
        (strcmp (bootDev, "scsi") == 0) ||
        (strcmp (bootDev, "tffs") == 0) ||
        (strcmp (bootDev, "pcmcia") == 0))
        {
        /* load boot file */
        if ((bootUsbFsSupport == TRUE) && (strcmp (bootDev, "fs") == 0) &&
            (strncmp (sysBootParams.bootFile, usbFileSysName, strlen(usbFileSysName)) == 0))
            status = bootUsbFileLoad (sysBootParams.bootFile, pEntry);
        else
            status = bootFsFileLoad (sysBootParams.bootFile, pEntry);

        if (status == ERROR)
            {
            bootAppPrintf ("\nError loading file: errno = 0x%x.\n", 
                            (_Vx_usr_arg_t)errno, 0,0,0,0,0);
            }
        }

    return (status);
    }
    

/******************************************************************************
*
* bootAppDevsShow - Display system devices
* 
* This routine is the command handler for the "devs" boot shell command.  It
* calls the I/O system to display the mounted devices.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppDevsShow
    (
    char * cmd  /* input command string */
    )
    {
    iosDevShow (); 
    return BOOT_STATUS_COMPLETE;
    }

/******************************************************************************
*
* bootAppChdir - Change current directory path
* 
* This routine is the command handler for the "cd" boot shell command to 
* change to a new current default directory path.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppChdir
    (
    char * cmd  /* input command string */
    )
    {
    STATUS  rtv;

    /* skip the command name and point to first parameter */
    bootCmdNameSkip (&cmd, 2);

    /* change to directory path specified by the user */
    rtv = chdir (cmd); 
    if (rtv == ERROR)
        {
        switch (errno)
            {
            case S_ioLib_NO_DEVICE_NAME_IN_PATH:
                bootAppPrintf ("No device name found in path.\n", 
                                0,0,0,0,0,0);
                break;
            case S_ioLib_NAME_TOO_LONG:
                bootAppPrintf ("Path name exceeds internal limits.\n", 
                                0,0,0,0,0,0);
                break;
            default:
                bootAppPrintf ("Could not change directory to %s.\n", 
                                (_Vx_usr_arg_t)cmd, 0,0,0,0,0);
                break;
            }
        }

    return BOOT_STATUS_COMPLETE;
    }


/******************************************************************************
*
* bootAppGetcwd - Get current directory path
* 
* This routine is the command handler for the "pwd" boot shell command to 
* print the current default directory path.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppGetcwd
    (
    char * cmd  /* input command string */
    )
    {
    char    pwdBuf[256];
    char *  pBuf;

    /* change to directory path specified by the user */
    pBuf = getcwd (pwdBuf, 256); 

    if (pBuf == pwdBuf)
        {
        bootAppPrintf ("%s\n", (_Vx_usr_arg_t)pwdBuf, 0,0,0,0,0);
        }

    return BOOT_STATUS_COMPLETE;
    }


/******************************************************************************
*
* bootAppRemove - Remove a file
* 
* This routine is the command handler for the "rm" boot shell command to 
* remove a file.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppRemove
    (
    char * cmd  /* input command string */
    )
    {
    STATUS  rtv;

    /* skip the command name and point to first parameter */
    bootCmdNameSkip (&cmd, 2);

    /* remove the file specified by the user */
    rtv = remove (cmd); 

    if (rtv == ERROR)
        bootAppPrintf ("ERROR: could not remove %s\n", (_Vx_usr_arg_t)cmd,
                        0,0,0,0,0);

    return BOOT_STATUS_COMPLETE;
    }


/******************************************************************************
*
* bootAppListDir - list directory contents
* 
* This routine is the command handler for the "ls" boot shell command to list 
* the directory contents of the input command string.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppListDir
    (
    char * cmd  /* input command string */
    )
    {
    STATUS  rtv;

    /* skip the command name and point to first parameter */
    bootCmdNameSkip (&cmd, 2);

    /* list the contents of the directory specified by the user */
    
    if (*cmd == EOS)
        rtv = ls (NULL, FALSE); 
    else
    rtv = ls (cmd, FALSE); 

    if (rtv == ERROR)
        bootAppPrintf ("ERROR: could not read %s\n", (_Vx_usr_arg_t)cmd,
                        0,0,0,0,0);

    return BOOT_STATUS_COMPLETE;
    }


/******************************************************************************
*
* bootAppCopyFile - Copy file from source to destination
* 
* This routine is the command handler for the "cp" shell command to copy a 
* file from the source location to the destination.  The source and destination
* are specified in the input command string
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppCopyFile
    (
    char * cmd  /* input command string */
    )
    {
    STATUS  rtv;
    char *  pStr;
    char    src[200];

    /* skip the command name and point to first parameter */
    bootCmdNameSkip (&cmd, 2);

    /* get the source argument */
    pStr = &src[0];

    /* skip to the next argument */
    while (!(isspace ((UINT) *cmd)))
        {
        *pStr++ = *cmd++;
        }
    *pStr++ = '\0';


    /*
     * move past the space between the parameters to 
     * get the destination argument.
     */

    bootSpaceSkip (&cmd);

    /* copy the source to the destination */
    rtv = cp (&src[0], cmd); 

    if (rtv == ERROR)
        bootAppPrintf ("ERROR: could not copy file %s\n", (_Vx_usr_arg_t)src,
                        0,0,0,0,0);

    return BOOT_STATUS_COMPLETE;
    }

