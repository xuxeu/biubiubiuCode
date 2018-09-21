/* bootNetLoad.c - Boot loader application network file load component */

/* 
 * Copyright (c) 2005-2012 Wind River Systems, Inc.
 * 
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01w,26apr12,mpc  Remove fix for INCLUDE_BOOT_DELAYED_INIT component (WIND00347629)
01w,19feb12,yjw  Fix strcat netmask to ipaddr cause ifconfig failed.
                 (WIND00261709)
01v,16jan12,mpc  Fix boot device parsing for sm=anchorAddress. (WIND00306698)
01u,10jan12,pgh  Fix INCLUDE_BOOT_DELAYED_INIT component
01t,19jan11,jjk  Also config ethernet interface when using DHCP, fix WIND00251524 
01s,11nov10,pgh  Add support for USB pegasus network device
01r,11dec09,pgh  64-bit conversion
01q,08oct09,spw  Fix WIND00183913
01p,24sep09,pgh  Fix WIND00176844
01o,17aug09,pgh  Check return value from usrBootLineCrack()
01n,03aug09,pgh  Fix boot device parsing.
01m,20jul09,pgh  Add missing return code checks.
01l,20apr09,zhw  fix wrong config when using "sm"interface.(WIND00163010)		
01n,14jul09,mdk  merge from networking branch
01m,21apr09,pgh  Fix the parsing of boot devices.
01l,22jan09,e_d  fixed boot from smEnd. (WIND00092864)
01k,02jul08,spw  Port changes for WIND00122447
01j,12jun08,pgh  Add support for SATA.
01i,15jan08,pgh  Don't ipDetach() non-network devices.
01h,30oct07,rlg  fix for defect WIND00108811 - issue with the modification for
                 bootline for DHCP and causing nightly tests to crash.
                 Modification only takes place if DHCP now.
01g,06sep07,pgh  Add DHCP boot support to bootapp.
01f,23apr07,jmt  Defect 93088 - networking not setup correctly for some
                 bootline updates
01e,26feb07,jmt  Update to add "tsfs" as a filesystem boot device
01d,06feb07,jmt  Add bootAppPrintf to optimize size
01c,25jan07,jmt  Add Network parameter update routine
01b,17jul06,jmt  Remove network init and netifAdrsPrint.
01a,11jul05,jmt  written
*/

/*
DESCRIPTION
This module contains the support routines for the VxWorks boot loader 
application network file load component.  This module is added to the 
boot loader application when the INCLUDE_BOOT_NETWORK component is 
included.  The initialization routine for this component adds a network 
file load handler to the boot loader application using the bootLoaderAdd() 
routine, and executes bootFlagHelpAdd() to add a boot flag help sting for 
TFTP.  The network file load handler checks if a network boot device is 
to be used, and calls the appropriate network file load handler to load 
the file over the network.  The network file load handlers supported by 
this module are DHCP, FTP, RSH, and TFTP.  The following list shows the 
component that must be included for the various network load handlers:

\is
\i DHCP - INCLUDE_BOOT_DHCPC
\i FTP - INCLUDE_BOOT_FTP_LOADER
\i RSH - INCLUDE_BOOT_RSH_LOADER
\i TFTP - INCLUDE_BOOT_TFTP_LOADER
\ie

This module uses routine pointers for the above network load handlers.  The
individual network load handlers initialize their specific routine pointer
when they are included into the boot loader application.  This module checks 
these routine pointers to determine if they are available.

This module determines if the boot device is a network device by examining the 
boot device specified in the boot line parameters.  The names of the network 
devices are defined by the device drivers, and are not known by this module.  
Therefore, it assumes the boot device is a network device if the specified 
boot device is one of the known non-network boot devices.  The following lists 
the known non-network boot devices:

\is
\i 'scsi' - SCSI file system.  Included for backward compatibility. 'fs' should 
be used instead.
\i 'fs' - File system boot device
\i 'ide' - IDE file system.  Included for backward compatibility. 'fs' should 
be used instead.
\i 'ata' - ATA file system.  Included for backward compatibility. 'fs' should 
be used instead.
\i 'fd' - Floppy disk file system.  Included for backward compatibility. 'fs' 
should be used instead.
\i 'tsfs' - Target server file system.
\i 'tffs' - True flash file system.  Included for backward compatibility. 'fs' 
should be used instead.
\ie

The only known network boot device is the 'sm' or shared memory boot device.
For more information on the shared memory boot device look for <shared> 
<memory> <anchor> in the Wind River documentation.


INCLUDE FILES: bootNetLoad.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <bootLoadLib.h>
#include <ipProto.h>
#include <sysLib.h>
#include <usrConfig.h>

#include <bootApp.h>
#include <bootNetLoad.h>

/* defines */

#define RSHD            514 /* rshd service */
#define MASK_STR_LEN    16  /* length of netmask string */

/* typedefs */

/* globals */

bootNetLoadPrototype * bootTftpLoaderFcn;
bootNetLoadPrototype * bootFtpLoaderFcn;
bootNetLoadPrototype * bootRshLoaderFcn;
bootNetLoadPrototype * bootSmNetLoaderFcn;
bootNetAdrAdjustPrototype * bootDhcpcAdrAdjustFcn = NULL;
bootDhcpEnableDisablePrototype * bootDhcpcEnableFcn = NULL;
bootDhcpEnableDisablePrototype * bootDhcpcDisableFcn = NULL;

extern bootNetSettingsUpdatePrototype * bootNetSettingsUpdateFunc;
extern BOOL bootDelayedInit;

extern STATUS usrNetBootConfig (char *devName, int unit, 
                 char *addr, int netmask, char *gateway);

/* local declarations */


/* forward declarations */

STATUS bootNetLoadInit(void);
STATUS bootAppNetDelayInit ();
LOCAL STATUS bootNetLoad (char * bootString, void **pEntry);
LOCAL STATUS netLoad (char *hostName, char *fileName, char *usr,
                      char *passwd, void **pEntry);
STATUS bootNetSettingsUpdate (BOOT_PARAMS * pOldBootParams, 
             BOOT_PARAMS * pBootParams);

/*******************************************************************************
*
* bootNetLoadInit - Initialize network file load component
*
* This routine initializes the boot loader application network file load 
* component.  This routine registers the network file load handler with the 
* boot loader application.
* 
* RETURNS: OK
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS bootNetLoadInit()
    {
    /* initialize variables */
    bootTftpLoaderFcn = NULL;
    bootFtpLoaderFcn = NULL;
    bootRshLoaderFcn = NULL;
    bootSmNetLoaderFcn = NULL;

    /* register network file load handler */
    bootLoaderAdd((bootLoaderPrototype *) bootNetLoad);
    
    /*
     * Setup network setting update function, regardless if DHCP is included.
     * We need to call usrNetBootConfig which will configure the ethernet boot
     * device for us.
     */

    bootNetSettingsUpdateFunc = (bootNetSettingsUpdatePrototype *) bootNetSettingsUpdate;
    
    /* setup network flag help */
    bootFlagHelpAdd(0x80, "  use tftp to get boot image");
    
    return OK;
    }

/*******************************************************************************
*
* bootNetLoad - Network file system load handler
*
* This routine is the network file system load handler that is registered with 
* the boot loader application to load files from the network.  This routine 
* loads the file specified by the boot line parameters, using the appropriate 
* network load components.  The network load components are DHCP, FTP, RSH, and
* TFTP.
* 
* RETURNS: 
*   OK if file loaded successfully,
*   ERROR if an error occurs while loading the file
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL STATUS bootNetLoad
    (
    char *  bootString,
    void ** pEntry
    )
    {
    IMPORT int  netTaskPriority;
    int         oldTaskPriority;
    STATUS      status = ERROR;
    char *      pBootAddr;
    char        bootDev[BOOT_DEV_LEN];
    int         i;

    /* Extract the boot device name. */

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

    /* verify unsupported booting devices */

    if (strcmp (bootDev, "sl") == 0)
        {
        bootAppPrintf ("booting via slip is unsupported.\n",0,0,0,0,0,0);
        return (ERROR);
        }

    /* attach and configure boot interface */

    pBootAddr = sysBootParams.ead;

    /* Set up for using loading over backplane or shared memory */
    
    if ((bootSmNetLoaderFcn != NULL) &&
        ((strcmp (bootDev, "bp") == 0) ||
         (strcmp (bootDev, "sm") == 0)))
        {
        pBootAddr = sysBootParams.bad;
        }

    /* Set up for loading via DHCP */

    if (bootDhcpcAdrAdjustFcn != NULL)
        {
        if (sysFlags & SYSFLG_AUTOCONFIG)
            {
            /* DHCP flag is set, so bootline parameters with DHCP values. */

            bootDhcpcEnableFcn (sysBootParams.unitNum, bootDev);
            bootDhcpcAdrAdjustFcn ();
            }
        else
            {
            /* DHCP flag is not set, so disable DHCP on the network device. */

            status = bootNetSettingsUpdate (&sysBootParams, &sysBootParams);
            if (status == ERROR)
                return (status);
            }
        }

    /* Transfer updated boot parameters from structure to boot line string. */

    bootStructToString (sysBootLine, &sysBootParams);

    /* load specified file */

    taskPriorityGet (0, &oldTaskPriority);
    taskPrioritySet (0, netTaskPriority + 1);

    if (netLoad (sysBootParams.had, sysBootParams.bootFile, sysBootParams.usr,
                 sysBootParams.passwd, pEntry) != OK)
        {
        bootAppPrintf ("\nError loading file: errno = 0x%x.\n", 
                        (_Vx_usr_arg_t)errno, 0,0,0,0,0);
        status = ERROR;
        }
    else
        {
        /* Disable DHCP client */
        if (bootDhcpcDisableFcn != NULL)
            bootDhcpcDisableFcn(sysBootParams.unitNum, bootDev);
        status = OK;
        }

    taskPrioritySet (0, oldTaskPriority);

    return (status);
    }

/* Local functions */

/*******************************************************************************
*
* netLoad - Load a file from a remote machine via the network.
*
* This routine loads the specified file from the specified IP address, using
* the appropriate network load routine.  This routine determines if the file
* should be loaded with either the TFTP, FTP, or RSH network load component.
*
* RETURNS: 
*   OK if the file is loaded
*   ERROR if the file cannot be loaded
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL STATUS netLoad
    (
    char *  hostAdr,    /* IP address to load from */
    char *  fileName,   /* name of file to load */
    char *  usr,        /* FTP user name */
    char *  passwd,     /* FTP password */
    void ** pEntry      /* Address to start execution */
    )
    {
    BOOL bootFtp = (passwd[0] != EOS);

    bootAppPrintf ("Loading... ",0,0,0,0,0,0);

    if (sysFlags & SYSFLG_TFTP)     /* use tftp to get image */
        {
        if (bootTftpLoaderFcn != NULL)
			{
			return (bootTftpLoaderFcn(hostAdr, fileName, usr, passwd, pEntry));
			}
		else
			{
			bootAppPrintf("tftp not included.\n",0,0,0,0,0,0);
			return ERROR;
			}
		}

   else if (bootFtp)
        {
        if (bootFtpLoaderFcn != NULL)
			{
			return (bootFtpLoaderFcn(hostAdr, fileName, usr, passwd, pEntry));
			}
		else
			{
			bootAppPrintf("ftp not included.\n",0,0,0,0,0,0);
			return ERROR;
			}
        }
    else
        {
        /* Default - use RSH */
        if (bootRshLoaderFcn != NULL)
			{
			return (bootRshLoaderFcn(hostAdr, fileName, usr, passwd, pEntry));
			}
		else
			{
			bootAppPrintf("rsh not included.\n",0,0,0,0,0,0);
			return ERROR;
			}
        }

    return (OK);
    }

/*******************************************************************************
*
* bootNetSettingsUpdate - Update the network settings.
*
* This routine checks for changes in the boot device.  If the previous boot 
* device was a network device, the network stack is disabled for the previous 
* network boot device.  If the new boot device is a network device, the network 
* stack is enabled for the new network boot device.
*
* RETURNS: OK
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS bootNetSettingsUpdate 
    (
    BOOT_PARAMS * pOldBootParams,   /* previous boot line parameters */
    BOOT_PARAMS * pBootParams       /* new boot line parameters */
    )
    {
    int	    netmask;                        /* Device netmask. */
    int     netmaskResult;                  /* result of netmask extraction */
    char    ipAddr[BOOT_TARGET_ADDR_LEN];  /* IP address parameter string */
    char    bootDev[BOOT_DEV_LEN];          /* new boot device name only */
    char    oldBootDev[BOOT_DEV_LEN];       /* previous boot device name only */
    int     i;

    /* Extract the new boot device name. */
    for (i = 0; i < BOOT_DEV_LEN; i++)
        {
        if (isalpha ((int)pBootParams->bootDev[i]))
            {
            bootDev[i] = pBootParams->bootDev[i];
            }
        else
            {
            bootDev[i] = 0;
            break;
            }
        }

    /* Extract the previous boot device name. */
    for (i = 0; i < BOOT_DEV_LEN; i++)
        {
        if (isalpha ((int)pOldBootParams->bootDev[i]))
            {
            oldBootDev[i] = pOldBootParams->bootDev[i];
            }
        else
            {
            oldBootDev[i] = 0;
            break;
            }
        }

    /* Check to see if the boot device was a network device */
    
    if ((strcmp (oldBootDev, "scsi") != 0) &&
        (strcmp (oldBootDev, "fs") != 0) &&
        (strcmp (oldBootDev, "ide") != 0) &&
        (strcmp (oldBootDev, "ata") != 0) &&
        (strcmp (oldBootDev, "fd") != 0) &&
        (strcmp (oldBootDev, "tsfs") != 0) &&
        (strcmp (oldBootDev, "tffs") != 0))
        {
        /* Disable DHCP client */
        if (bootDhcpcDisableFcn != NULL)
            bootDhcpcDisableFcn(pOldBootParams->unitNum, oldBootDev);

        /* cleanup old settings */
        if (ipDetach(pOldBootParams->unitNum, oldBootDev) == ERROR)
            {
        	/*  bootAppPrintf("Error detaching from network device\n",
                          0,0,0,0,0,0);*/
            }
        }

    /* Check to see if this is a network boot or filesystems */
    if ((strcmp (bootDev, "scsi") != 0) &&
        (strcmp (bootDev, "fs") != 0) &&
        (strcmp (bootDev, "ide") != 0) &&
        (strcmp (bootDev, "ata") != 0) &&
        (strcmp (bootDev, "fd") != 0) &&
        (strcmp (bootDev, "tsfs") != 0) &&
        (strcmp (bootDev, "tffs") != 0))
        {
        /* Determine if the ethernet or backplane IP address is to be used. */
        if ((strcmp (bootDev, "bp") == 0) ||
            (strcmp (bootDev, "sm") == 0))
            {
            /* backplane IP parameter */
            strncpy (ipAddr, pBootParams->bad, sizeof(ipAddr) - 1); 
            }
        else
            { 
            /* ethernet IP parameter */
            strncpy (ipAddr, pBootParams->ead, sizeof (ipAddr) - 1);
            }

        /* extract the network mask from the IP address parameter string */
        netmask = 0;
        netmaskResult = bootNetmaskExtract (ipAddr, &netmask);

        /* Invalid or no network mask is set, so set a default */
        if (netmaskResult <= 0)
            {
            char    msbStr[8];      /* MSB of IP address string */
            int     msbVal;         /* value of MSB of IP address */

            /* Extract the 8 MSB of IP addr, or digits before the period */
            i = (int)strspn(ipAddr, "0123456789");
            strncpy(&msbStr[0], ipAddr, (size_t)i); /* get MSB of IP addr */
            msbStr[i] = 0;                      /* null terminate */
            msbVal = atoi(msbStr);              /* convert it to an integer */

            /*
             * Determine if the IP address is a Class A, B, or C network,
             * and set the default network mask accordingly.
             * When DHCP is enabled, do not bother to assign a valid target IP
             * or mask. DHCP callback function bootappDhcpOptionCallback takes
             * care of that.
             */
            if (bootDhcpcEnableFcn == NULL)
                {
                if ((0 <= msbVal) && (msbVal < 128))
                    {
                 /*   bootAppPrintf ("Error reading target netmask information.\n"
                                   "Defaulting netmask to 0xFF000000.\n",
                                   0,0,0,0,0,0);*/
                    netmask = (int)0xFF000000;  /* Class A network default mask */
                    }
                else if ((128 <= msbVal) && (msbVal < 192))
                    {
                   /* bootAppPrintf ("Error reading target netmask information.\n"
                                   "Defaulting netmask to 0xFFFF0000.\n",
                                   0,0,0,0,0,0);*/
                    netmask = (int)0xFFFF0000;  /* Class B network default mask */
                    }
                else
                    {
                   /* bootAppPrintf ("Error reading target netmask information.\n"
                                   "Defaulting netmask to 0xFFFFFF00.\n",
                                   0,0,0,0,0,0);*/
                    netmask = (int)0xFFFFFF00;  /* Class C network default mask */
                    }
                }

            /* With DHCP we do not care about target IP or mask, do not print */
            if (bootDhcpcEnableFcn == NULL)
                {
            	/*  bootAppPrintf("Using default network mask = 0x%X\n",
                            (_Vx_usr_arg_t)netmask, 0,0,0,0,0);*/
                }
            }

        /* enable new device */
        if (ipAttach(pBootParams->unitNum, bootDev) == ERROR)
            {
            bootAppPrintf("Error attaching to network device\n",
                          0,0,0,0,0,0);
            return (ERROR);
            }

        /* Enable DHCP client, if configured. */
        if ((pBootParams->flags & SYSFLG_AUTOCONFIG) && (bootDhcpcEnableFcn != NULL)) 
            {
            bootDhcpcEnableFcn(pBootParams->unitNum, bootDev);
            }
        else
            {
            /* Configure new network settings, if DHCP not configured. */
            if (usrNetBootConfig (bootDev, 
                                  pBootParams->unitNum, 
                                  ipAddr, 
                                  netmask, 
                                  pBootParams->gad) == ERROR)
                {
                bootAppPrintf("Error configuring network device\n",
                              0,0,0,0,0,0);
                return (ERROR);
                }
            }
        }
    
    return (OK);
    }
