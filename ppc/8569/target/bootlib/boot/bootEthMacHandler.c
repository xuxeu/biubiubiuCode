/* bootEthMacHandler.c - Bootrom loader application ethernet MAC set support */

/* 
*  Copyright (c) 2006-2009, 2012, 2013 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
01j,10jan13,wyt  replace maxMacDevices with maxMacAddresses in the for statement
01i,06nov12,h_k  increased MAX_MAC_ADRS to 20 for the board that supports more
                 than 10 devices.
01h,12aug12,mpc  Remove building warnings. (WIND00354175)
01g,11jan12,pgh  Fix sprintf() usage, and replace sscanf() calls.
01f,11dec09,pgh  64-bit conversion
01e,12jun09,pgh  Fix call to sysMacOffsetGet()
01d,27apr09,pgh  Update documentation.
01c,23apr08,pgh  Fix endian problems with MAC address.
01b,06feb07,jmt  Add bootAppPrintf to optimize size
01a,24oct06,jmt  written
*/

/*
DESCRIPTION
This module contain support routines for the VxWorks boot loader application 
ethernet MAC component.  This module is added to the boot loader application 
when the INCLUDE_BOOT_ETH_MAC_HANDLER component is included.  This module 
contains an initialization routine, which adds a command to the boot shell 
to allow the ethernet MAC address to be modified.  This component uses the 
bootCommandHandlerAdd() to add the 'M' command, which allows all six bytes of 
the MAC address to be set.  The 'M' command does allow the vendor specific 
bytes of the MAC address to be modified; unlike the bootEthAdrSet.c component, 
which adds the 'N' command to the boot shell.  Some BSPs use the 'M' command, 
and others use the 'N' command.  Therefore, the usage of the 'M' versus 'N' 
commands is BSP specific.

The 'M' command has optional arguments, and this results in the following five 
different command line examples:

\is
\i M
\i M motfcc
\i M motfcc1
\i M motfcc1 00:01:00
\i M motfcc0 00:A0:1E:00:01:00
\ie

The first example, 'M', will invoke a dialog with the user to display the 
current devices and prompt for which device address to change as below:

\cs
    0  quit
    1  motfcc0 00:A0:1E:00:01:1A
    2  motscc0 00:A0:1E:00:01:1B
  Selection :
\ce

If selection '0' is made, the dialog will quit.  If the selection is for one
of the MAC devices, then the dialog will prompt for the MAC address of the
selected device as shown below:

\cs
  00- 00
  A0- A0
  1E- 1E
  00-
\ce

The first three bytes of the MAC address are fixed and can't be changed
using this method.  The remaining three subsequent values will be displayed 
for modification.  If no entry is made, the previous value remains unchanged.

If the MAC address read from configuration memory does not include the three 
byte vendor code, it is assumed that the address read is invalid.  In this 
case, a pseudo-random address is generated before the dialog commences.  The 
address generation algorithm uses a hashing method to generate the address 
based on the sum of various fixed parameters such as the contents of the 
boot configuration.  This method generates an address that is reproducible, 
given the same build and boot parameters.  If the boot record is not valid, 
a random address is generated that is not reproducible.

The second example, 'M motfcc', shows a method used to avoid the first part of 
the user dialog.  In this case, the device is specified on the command line.  
Since the unit number is not included, unit zero is assumed.  

The third example, 'M motfcc1', shows the command format that should be used 
to select a device other than unit zero.

The fourth example, 'M motfcc1 00:01:00', shows a syntax that avoids the user 
dialog.  In this example, the first three bytes of the MAC address will be 
the WR vendor code.  The remaining three bytes will be as specified on the
command line.  This method requires that exactly three bytes of the address 
be supplied.

The fifth example, 'M motfcc0 00:A0:1E:00:01:00', shows a method that can be 
used to set all six bytes of the MAC address, including the vendor code.  
This syntax is used to override the default WR vendor code.  Care must be 
taken when using this syntax as the vendor code will be replaced with the
values specified on the command line.

NOTE: When upgrading the boot loader from VxWorks version 6.6 or older,
you will need to reprogram the MAC addresses.  The byte order of how the
MAC address is stored changed beginning with VxWorks version 6.7.  Prior
to updating a VxWorks 6.6, or older, boot loader, display and save the 
current MAC values for the target board.  Once the new VxWorks boot loader 
has been installed, reprogram the MAC addresses of the target board using 
the values saved from the older boot load MAC address display.

INCLUDE FILES: vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <bootLib.h>
#include <ctype.h>
#include <fioLib.h>
#include <stdio.h>
#include <string.h>
#include <usrConfig.h>

#include <bootApp.h>

/* defines */

/* maximum line length for cmd input to 'M' and 'sEnet' routine */
#define MAX_LINE        160

#define MAX_MAC_ADRS    20    /* must not go above this value */
#define MAX_ADR_SIZE    6
#define MAX_MAC_SHOW_BUF_SIZE 40
#define IFG_MAC_SUPPLIED 1 /* remaining boot line identifies new MAC address */
#define IFG_DEV_SUPPLIED 2 /* operator elects to quit the dialog */
#define IFG_OPR_QUIT 3     /* device is selected, but no MAC address */
#define NOTEOS(arg) (*arg!=0 && (*arg!= (char)EOF))
#define SKIPSPACE(arg) while( NOTEOS(arg) && \
                        (isspace ((int) *arg))) arg++

/* typedefs */

/* globals */

extern const char *sysNetDevName[];

extern int sysMacIndex2Dev (int index);
extern int sysMacIndex2Unit (int index);
extern STATUS sysMacOffsetGet (char *ifName, int ifUnit, char **ppEnet, 
                              int *	pOffset);
extern STATUS sysNetMacNVRamAddrGet (char *ifName, int ifUnit, 
                                     UINT8 *ifMacAddr, int ifMacAddrLen);
extern STATUS sysNetMacAddrGet (char *ifName, int ifUnit, UINT8 *ifMacAddr, 
                              int ifMacAddrLen);
extern STATUS sysNetMacAddrSet (char *ifName, int ifUnit, UINT8 *ifMacAddr, 
                              int ifMacAddrLen);

/* local vars */

static int maxMacAddresses;
static int maxMacDevices;
static UINT8 defaultEnetAddress[3];
static int macAddressLength;
static char macShowStr[MAX_MAC_ADRS] [MAX_MAC_SHOW_BUF_SIZE];
static const char macErrStr[] = "** invalid device id\n";

/* forward declarations */

void bootEthMacHandlerInit(int, int, UINT8, UINT8, UINT8, int);
LOCAL int bootAppSEnet (char * cmd);

/*******************************************************************************
*
* bootEthMacHandlerInit - Initialize the Ethernet MAC Component
*
* This routine initializes the boot loader application ethernet MAC component.
* This routine adds a command to the boot shell to allow the ethernet MAC 
* address to be modified.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void bootEthMacHandlerInit 
    (
    int     maxMacAdrs,    /* Number of MAC Addresses to store */
    int     maxMacDevs,    /* Number of MAC Devices */
    UINT8   defEnetAdrs0,  /* Default 1st byte of vendor code */
    UINT8   defEnetAdrs1,  /* Default 2nd byte of vendor code */
    UINT8   defEnetAdrs2,  /* Default 3rd byte of vendor code */
    int     macAdrsLen     /* MAC Address length */
    )
    {
    /* setup local vars */

    if (maxMacAdrs > MAX_MAC_ADRS)
        maxMacAddresses = MAX_MAC_ADRS;
    else
        maxMacAddresses = maxMacAdrs;
    maxMacDevices = maxMacDevs;
    defaultEnetAddress[0] = defEnetAdrs0;
    defaultEnetAddress[1] = defEnetAdrs1;
    defaultEnetAddress[2] = defEnetAdrs2;
    macAddressLength = macAdrsLen;

    /* setup command handlers */
    
   /* bootCommandHandlerAdd("M", bootAppSEnet, BOOT_CMD_MATCH_STRICT,
         "M [dev][unitNo] [MAC]",  "- set/display ethernet address");
    */}

/* Local functions */

/*******************************************************************************
*
* macAddressShow - Display the device and MAC address in string form.
*
* This routine returns a pointer to a string of the form:
*
*   id device unit address
*
*   where:
*    id is an integer 1..maxMacAddresses
*    device is a string (e.g. motfcc)
*    unit is unit number
*    address is a six byte hex mac address
*   example:
*    "01  motfcc0 00:A0:1E:00:10:0A
*
* This routine takes an index as an argument, where the index is in the
* range 00..(maxMacAddresses-1).
*
* If the passed index is out of range, this routine returns a pointer to an 
* error message.
*
* RETURNS: A pointer to a string.
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void * macAddressShow
    (
    int index   /* MAC device index */
    )
    {
    int devNameIndex;
    int unit;
    UINT8 macAddr[MAX_ADR_SIZE];

    /* range check passed index */

    if ((index < 0) || (index >= maxMacAddresses))
        {
        return ((char *) macErrStr);  /* if error, return error string */
	}

    /* convert MAC index to device index and unit number */

    devNameIndex = sysMacIndex2Dev (index);
    unit = sysMacIndex2Unit(index);

    /* get a copy of the current device address */

    if (sysNetMacNVRamAddrGet ((char *)sysNetDevName[devNameIndex], unit,
                           macAddr, macAddressLength) != OK)
        {

        /* macAddr is filled with  all f's if ERROR */

	memset (macAddr, 0xff, MAX_ADR_SIZE);
        }
			
    /* convert to string */
    			
    (void)snprintf (macShowStr[index], MAX_MAC_SHOW_BUF_SIZE,
                    "%2d  %6s%d  %02x:%02x:%02x:%02x:%02x:%02x\n",
                    index+1, sysNetDevName[devNameIndex],
                    unit,
                    macAddr[0],
                    macAddr[1],
                    macAddr[2],
                    macAddr[3],
                    macAddr[4],
                    macAddr[5]);
    return (macShowStr[index]);
    }


/*******************************************************************************
*
*  ifGet - Get network interface name
*
* This routine parses the remaining command line (after the command character 
* has been removed) and attempts to identify the requested device and unit 
* number using this string.  If the command line does not provide the device 
* name, a dialog is initiated to identify the device.
*
* RETURNS: 
*   IFG_MAC_SUPPLIED - if remaining boot line identifies new MAC address.
*   IFG_OPR_QUIT     - if the operator elects to quit the dialog
*   IFG_DEV_SUPPLIED - if a device is selected, but no MAC address
*
* ERRNO: N/A
*
* \NOMANUAL
*/
			
LOCAL int ifGet
    (
    char **   ifName, 	/* interface name */
    int *     unit,     /* interface unit */
    char *    pNum,     /* Command line */
    uchar_t * curArr    /* current MAC address */
    )
    {
    char line [MAX_LINE + 1];
    uchar_t passedMac [MAX_ADR_SIZE];
    int numMacDigiRead = 0;
    char *pLine;		/* ptr to current position in line */
    int nameLen = 0;
    int index;
    int macAddressPassed = 0;
    int macDevPassed = 0;
    void *  showStr;


    pLine = pNum;

    SKIPSPACE(pLine);  /* advance past any leading whitespaces */
    *ifName = pLine;

    /* determine if the bootline passed includes ifName, unit, and MAC */

     while (NOTEOS(pLine) &&  isalpha((int)*pLine))
        {
        nameLen++;   /* count device length */
	pLine++;
	}

    /* if device name found */

    if (nameLen != 0)
        {
        for (index = 0; index < maxMacAddresses; index++)
	    {
	
	    /* if indexed device matches passed device name */
	
	    if (strncmp (sysNetDevName[sysMacIndex2Dev(index)], *ifName,
	        strlen((char *)sysNetDevName[sysMacIndex2Dev(index)])) == 0)
	        {
		
		/* save return dev name */
		
                *ifName = (char *)sysNetDevName[sysMacIndex2Dev(index)];
		macDevPassed = 1;               /* set flag */
		
		/* was unit number supplied? */
                if (isdigit((int)*pLine))
		    {
		    *unit = (int)strtol(pLine, &pLine, 0); /* extract unit number */
		    }
                else
		    {
		    *unit = 0; /* if not supplied, assume unit 0 */
		    }
		
		/* look for MAC address */
		
                while (NOTEOS(pLine))
		    {
                    SKIPSPACE(pLine);  /* advance to next field */
	            if (!NOTEOS(pLine))
			{
			break; /* end of string */
			}
		    if (isxdigit((int)*pLine))
			{
			passedMac[numMacDigiRead++] =
			    (uchar_t) strtol(pLine, &pLine, 16);
			}
	            else /* some illegal character read */
			{
			break;
			}
		    if (*pLine == ':')
			    pLine++;
			
		    }
		
		/* if we were able to read the MAC address */
		
        if (numMacDigiRead == 6) /* if full MAC address */
            {
            macAddressPassed = 1;
            for (index = 0; index < 6; index++)
                {
                curArr[index] = passedMac[index];
                }
            }
        else if (numMacDigiRead == 3) /* if half MAC address */
            {
            macAddressPassed = 1;
            curArr[0] = defaultEnetAddress[0];
            curArr[1] = defaultEnetAddress[1];
            curArr[2] = defaultEnetAddress[2];
            for (index = 3; index < 6; index++) /* 3 bytes only */
                {
                curArr[index] = passedMac[index - 3];
                }
            }
		break; /* matching device found.  Look no further */
		} /* end if indexed device matches passed device name */
	
	    }
	
	} /* end if no device name found */

    if (macDevPassed == 0)  /* if the command line lacks a device name */
        {
	
	/* print known devices */
	
        bootAppPrintf ("Set device MAC address\n\n",0,0,0,0,0,0);
        bootAppPrintf ("  0  quit\n",0,0,0,0,0,0);
        for (index = 0; index < maxMacAddresses; index++)
            {
            showStr = macAddressShow(index);
            bootAppPrintf (" %s", (_Vx_usr_arg_t)showStr,0,0,0,0,0);
            }


        index = -1; /* initialize to invalid index */

	/* query for new network device (1..n) */

	while ((index < 0) || ( index >= maxMacAddresses))
            {
            bootAppPrintf ("  Selection : ",0,0,0,0,0,0);
	    if (fioRdString (STD_IN, line, MAX_LINE) == EOF) /* end of line */
		{
		continue;
		}

	    line [MAX_LINE] = EOS;	/* make sure input line has EOS */

            /* skip leading spaces*/

	    for (pLine = line; isspace ((int) *pLine); ++pLine)	
		;

	    if (*pLine == EOS)		/* if field is just CR */
		{
		continue;
		}

            /* if not number */

	    if (bootScanNum (&pLine, (_Vx_usr_arg_t *)&index, FALSE) != OK)
		{
        	bootAppPrintf("\ninvalid selection\n",0,0,0,0,0,0);
		index  = -1;
                return (IFG_OPR_QUIT); /* quit */
		}

            if (index == 0)
	        {
		index  = -1;
                return (IFG_OPR_QUIT); /* quit */
                }

  	        index--;   /* set to zero based index */

           }  /* end query for new network device (1..n) */


        /* return device string and unit number */

        *ifName = (char *)sysNetDevName[sysMacIndex2Dev(index)];
	*unit = sysMacIndex2Unit(index);
	}
	
    if (macAddressPassed)
        return (IFG_MAC_SUPPLIED);
    return (IFG_DEV_SUPPLIED);

    }


/*******************************************************************************
*
* dynamicMacAddrGen - Generate a dynamic MAC address
*
* This routine generates a 3-byte network interface MAC address.  This should 
* be used as the last three bytes of the device address.  This routine 
* attempts to make the address unique, but is not guaranteed to do so.
*
* This routine should only be called if both sysNetMacAddrGet() and 
* sysNVRamNetMacAddrGet() return ERROR, indicating that there is no other 
* suitable mechanism for generating a MAC address.
*
* RETURNS:
*   1 for an address which will be reproducible across multiple runs
*   0 if the address is likely to change between vxWorks and bootrom or 
*     between two invocations within either vxWorks or bootrom.
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int dynamicMacAddrGen
    (
    UINT8 *	ifName,     /* interface name */
    int		ifUnit,     /* interface unit number */
    UINT8 *	ifMacAddr   /* MAC address for the interface */
    )
    {
    BOOT_PARAMS params;
    int			i = 0;
    UINT32		sum = 0;
    char 		adrs [BOOT_TARGET_ADDR_LEN];


    /* hash sum of specified interface name */

    while ( EOS != ifName[i] )
        sum += ifName[i++];

    /* check boot command */

    if (usrBootLineCrack (sysBootLine, &params) == OK)
        {
	if ( ( params.ead[0] != EOS ) && ( params.ead[1] != EOS ) &&
	     ( params.ead[2] != EOS ) && ( params.ead[3] != EOS ) &&
	     ( params.ead[4] != EOS ) && ( params.ead[5] != EOS ) )
	    {
	    /* OK, use target address */
	    *ifMacAddr++ = (UINT8)((UINT32)params.ead[3] + sum + (UINT32)ifUnit);
	    *ifMacAddr++ = (UINT8)params.ead[4];
	    *ifMacAddr++ = (UINT8)params.ead[5];
	    return(1);
	    }

	adrs[0] = (char)(params.bad[0] + params.had[0] + params.gad[0]);
	adrs[1] = (char)(params.bad[1] + params.had[1] + params.gad[1]);
	adrs[2] = (char)(params.bad[2] + params.had[2] + params.gad[2]);
	adrs[3] = (char)(params.bad[3] + params.had[3] + params.gad[3]);
	adrs[4] = (char)(params.bad[4] + params.had[4] + params.gad[4]);
	adrs[5] = (char)(params.bad[5] + params.had[5] + params.gad[5]);

	if ( ( adrs[0] != EOS ) && ( adrs[1] != EOS ) &&
	     ( adrs[2] != EOS ) && ( adrs[3] != EOS ) &&
	     ( adrs[4] != EOS ) && ( adrs[5] != EOS ) )
	    {
	
	    /* use host + gateway + backplane */
	
	    *ifMacAddr++ = (UINT8)((UINT32)(adrs[0] + adrs[3]) + sum + 
                                    (UINT32)ifUnit);
	    *ifMacAddr++ = (UINT8)(adrs[1] + adrs[4]);
	    *ifMacAddr++ = (UINT8)(adrs[2] + adrs[5]);
	    return(1);
	    }

	/* hash some strings together */

	for ( i = 0 ; i < BOOT_DEV_LEN ; i++ )
	    sum += (UINT8)params.bootDev[i];
	for ( i = 0 ; i < BOOT_HOST_LEN ; i++ )
	    sum += (UINT8)params.hostName[i];
	for ( i = 0 ; i < BOOT_HOST_LEN ; i++ )
	    sum += (UINT8)params.targetName[i];
	for ( i = 0 ; i < BOOT_FILE_LEN ; i++ )
	    sum += (UINT8)params.bootFile[i];
	for ( i = 0 ; i < BOOT_FILE_LEN ; i++ )
	    sum += (UINT8)params.startupScript[i];
	for ( i = 0 ; i < BOOT_USR_LEN ; i++ )
	    sum += (UINT8)params.usr[i];
	for ( i = 0 ; i < BOOT_PASSWORD_LEN ; i++ )
	    sum += (UINT8)params.passwd[i];
	for ( i = 0 ; i < BOOT_OTHER_LEN ; i++ )
	    sum += (UINT8)params.other[i];

	adrs[0] = 0;
	adrs[1] = 4;
	adrs[2] = (char)(( sum >> 24 ) & 0x00ff);
	adrs[3] = (char)(( sum >> 16 ) & 0x00ff);
	adrs[4] = (char)(( sum >>  8 ) & 0x00ff);
	adrs[5] = (char)(( sum >>  0 ) & 0x00ff);

	*ifMacAddr++ = (UINT8)((UINT32)(adrs[0] + adrs[3]) + sum + 
                                (UINT32)ifUnit);
	*ifMacAddr++ = (UINT8)(adrs[1] + adrs[4]);
	*ifMacAddr++ = (UINT8)(adrs[2] + adrs[5]);
	return(1);
	}

    /*
     * boot command not available, generate address
     * which is as close to random as we can make it.
     * If we get here, there isn't any reasonable way to
     * make a highly unique number, but we'll do the
     * best we reasonably can.
     *
     * NOTE: This is guaranteed to cause problems when
     * booting, unless rommable vxWorks image is used.
     * If we get here, the vxWorks and bootrom images
     * will use different MAC addresses.  When the interface
     * changes, vxWorks will correctly send a gratuitous
     * ARP packet.  However, some switches will ignore
     * the gratuitous ARP packet which came so quickly
     * after the vxWorks image download.  In this case,
     * the target will be able to send traffic out, but
     * until it has done so, no other computer will be able
     * to contact it.  To resolve this problem, the target
     * should initiate outbound IP traffic after a suitable
     * delay.  This can be a simple ping, or it can be
     * a symbol table download, or whatever.
     */

    sum += (UINT32)tickGet();   /* time component */
    sum += (UINT32)((ptrdiff_t)&params);   /* stack addr: runtime mem use component */
    sum += (UINT32)((ptrdiff_t)ifMacAddr); /* data addr: compile-time component */

    adrs[0] = 0;
    adrs[1] = 4;
    adrs[2] = (char)((sum >> 24) & 0x00ff);
    adrs[3] = (char)((sum >> 16) & 0x00ff);
    adrs[4] = (char)((sum >>  8) & 0x00ff);
    adrs[5] = (char)((sum >>  0) & 0x00ff);

    *ifMacAddr++ = (UINT8)(adrs[0] + adrs[3] + ifUnit);
    *ifMacAddr++ = (UINT8)(adrs[1] + adrs[4]);
    *ifMacAddr++ = (UINT8)(adrs[2] + adrs[5]);
    return(0);
    }

/*******************************************************************************
*
* mEnet - Modify the last three bytes of the device ethernet address.
*
* This routine is called if the command line lacks a network address for the 
* selected device.  It initiates a dialog with the user to obtain the address.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/
LOCAL void mEnet
    (
    char *  pNum,       /* returned MAC address */
    int     unitNum,    /* interface unit number */
    char *  ifName      /* interface name */
    )
    {
    uchar_t byte [MAX_ADR_SIZE];	/* array to hold new Enet addr */
    uchar_t curArr [MAX_ADR_SIZE];	/* array to hold current Enet addr */
    char line [MAX_LINE + 1];
    char *pEnet;                /* dummy variable */
    int pOffset = 0;        /* dummy variable */
    char *pLine;		/* ptr to current position in line */
    int value;			/* value found in line */
    STATUS status;
    int ix;

    /* make sure device is valid */

    if (sysMacOffsetGet (ifName, unitNum, &pEnet, &pOffset) != OK)
        {
	bootAppPrintf ("** invalid device\n",0,0,0,0,0,0);
	return;
	}

    /* read current value */

    status = sysNetMacNVRamAddrGet (ifName, unitNum, curArr, macAddressLength);

    /* if at first we don't succeed, try second method */

    if (status !=OK)
        sysNetMacAddrGet (ifName, unitNum, curArr, macAddressLength);
	
    /* if current MAC address is invalid */

    if ((status != OK) ||
        (curArr[0] != defaultEnetAddress[0]) ||
        (curArr[1] != defaultEnetAddress[1]) ||
        (curArr[2] != defaultEnetAddress[2]))
	{
	curArr[0] = defaultEnetAddress[0];
	curArr[1] = defaultEnetAddress[1];
	curArr[2] = defaultEnetAddress[2];
        
	ix = dynamicMacAddrGen((UINT8 *) ifName, unitNum, (UINT8 *) &curArr[3]);

	bootAppPrintf ("Mac address for %s%d invalid. ", 
                        (_Vx_usr_arg_t)ifName, (_Vx_usr_arg_t)unitNum,
                        0,0,0,0);
	if  (ix == 1)
            {
	    bootAppPrintf ("Using a reproducible random address.\n",
                            0,0,0,0,0,0);
            }
	else
            {
	    bootAppPrintf ("Using a non-reproducible random address.\n",
                            0,0,0,0,0,0);
            }
	}

    bootAppPrintf ("%s%d current Ethernet Address is: ", 
                    (_Vx_usr_arg_t)ifName, (_Vx_usr_arg_t)unitNum,
                    0,0,0,0);

    bootAppPrintf ("%02x:%02x:%02x:%02x:%02x:%02x\n", 
                    (_Vx_usr_arg_t)curArr[0], (_Vx_usr_arg_t)curArr[1], 
                    (_Vx_usr_arg_t)curArr[2], (_Vx_usr_arg_t)curArr[3],
                    (_Vx_usr_arg_t)curArr[4], (_Vx_usr_arg_t)curArr[5]);
    byte[5] = curArr[5];
    byte[4] = curArr[4];
    byte[3] = curArr[3];
    byte[2] = curArr[2];
    byte[1] = curArr[1];
    byte[0] = curArr[0];

    bootAppPrintf ("Modify only the last 3 bytes (board unique portion) of"
                  " Ethernet Address.\nThe first 3 bytes are fixed at "
                  "manufacturer's default address block.\n",0,0,0,0,0,0);

    for (ix = 0; ix < MAX_ADR_SIZE / 2; ix++)
        bootAppPrintf ("%02x- %02x\n", 
                        (_Vx_usr_arg_t)byte[ix], (_Vx_usr_arg_t)byte[ix],
                        0,0,0,0);


    /* start on fourth byte of enet addr */

    for (ix = MAX_ADR_SIZE / 2; ix < MAX_ADR_SIZE; ix++)	
	{

	/* prompt for substitution */

	bootAppPrintf ("%02x- ", (_Vx_usr_arg_t)byte[ix], 0,0,0,0,0);

	/* 
	 * get substitution value:
	 *   skip empty lines (CR only);
	 *   quit on end of file or invalid input;
	 * otherwise put specified value at address 
	 */

	if (fioRdString (STD_IN, line, MAX_LINE) == EOF)
	    break;

	line [MAX_ADR_SIZE] = EOS;	/* make sure input line has EOS */

	for (pLine = line; isspace ((int) *pLine); ++pLine)	/* skip leading spaces*/
	    ;

	if (*pLine == EOS)			/* skip field if just CR */
	    continue;

        if (bootScanNum (&pLine, (_Vx_usr_arg_t *)&value, TRUE) != OK)
	    break;				/* quit if not number */

	byte[ix]  = (uchar_t)value;		/* assign new value */
	}

    bootAppPrintf ("\n",0,0,0,0,0,0);

    if (sysNetMacAddrSet (ifName, unitNum, byte, macAddressLength) != OK)
        {
	bootAppPrintf ("** error setting address for %s%d\n", 
                        (_Vx_usr_arg_t)ifName, (_Vx_usr_arg_t)unitNum,
                        0,0,0,0);
	return;
	}
    if (sysNetMacNVRamAddrGet (ifName, unitNum, byte, macAddressLength) == OK)
        {
    bootAppPrintf ("New Ethernet Address is: ",0,0,0,0,0,0);
    for (ix = 0; ix < 5; ix++)
        bootAppPrintf ("%02x:", (_Vx_usr_arg_t)byte[ix],0,0,0,0,0);
    bootAppPrintf ("%02x\n", (_Vx_usr_arg_t)byte[5],0,0,0,0,0);
	}
    else
        {
	bootAppPrintf ("error getting Mac address from NVRAM \n",0,0,0,0,0,0);    
        }

    }

/*******************************************************************************
*
* sEnet - Set the device ethernet address
*
* This routine checks the command line to see if the MAC address can be set 
* for the requested device.  The command line is parsed to extract the device 
* name, unit name, and MAC address.  All three items are optional.  Example 
* command lines are as follows:
*
*    M
*    M motfcc
*    M motfcc1
*    M motfcc1 00:01:00
*    M motfcc0 00:A0:1E:00:01:00
*
* The first example will invoke a dialog with the user to display the current 
* devices and prompt for which device address to change as below:
*
*      0  quit
*      1  motfcc0 00:A0:1E:00:01:1A
*      2  motscc0 00:A0:1E:00:01:1B
*    Selection :
*
* If selection '0' is made, the dialog will quit.  If the selection is for one
* of the MAC devices, then the dialog will prompt for the MAC address of the
* selected device as shown below:
*
*    00- 00
*    A0- A0
*    1E- 1E
*    00-
*
* The first three bytes of the MAC address are fixed and can't be changed
* using this method.  The remaining three subsequent values will be displayed 
* for modification.  If no entry is made, the previous value remains unchanged.
*
* If the MAC address read from configuration memory does not include the three 
* byte vendor code, it is assumed that the address read is invalid.  In this 
* case, a pseudo-random address is generated before the dialog commences.  The 
* address generation algorithm uses a hashing method to generate the address 
* based on the sum of various fixed parameters such as the contents of the 
* boot configuration.  This method generates an address that is reproducible, 
* given the same build and boot parameters.  If the boot record is not valid, 
* a random address is generated that is not reproducible.
*
* The second example 'M motfcc' shows a method used to avoid the first part of 
* the user dialog.  In this case, the device is specified on the command line.  
* Since the unit number is not included, so unit zero is assumed.  
*
* The third example 'M motfcc1' shows the command format that should be used 
* to select a device other than unit zero.
*
* The fourth example 'M motfcc1 00:01:00' shows a syntax that avoids the user 
* dialog.  In this example, the first three bytes of the MAC address will be 
* the WR vendor code.  The remaining three bytes will be as specified on the
* command line.  This method requires that exactly three bytes of the address 
* be supplied.
*
* The last example 'M motfcc0 00:A0:1E:00:01:00' shows a method that can be 
* used to set all six bytes of the MAC address, including the vendor code.  
* This syntax is used to override the default WR vendor code.  Care must be 
* taken when using this syntax as the vendor code will be replaced with the
* values specified on the command line.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void sEnet
    (
    char * 	pNum   /* Boot line, including unit number. */
    )
    {
    uchar_t curArr [MAX_ADR_SIZE];	/* array to hold current Enet addr */
    int     unitNum;
    char *  ifName ="\n";
    int     ifGetStatus;

    /* identify the network device */

    ifGetStatus = ifGet (&ifName, &unitNum, pNum, curArr);

    if (ifGetStatus == IFG_OPR_QUIT)
        return; /* operator abort */
	
    if (ifGetStatus == IFG_MAC_SUPPLIED)
        {
        if (sysNetMacAddrSet (ifName, unitNum, curArr, macAddressLength) != OK)
            {
            bootAppPrintf ("** error setting address for %s%d\n", 
                            (_Vx_usr_arg_t)ifName, (_Vx_usr_arg_t)unitNum,
                            0,0,0,0);
            return;
            }
        }
    else
        {
        mEnet (pNum, unitNum, ifName);
        }	
    }


/*******************************************************************************
*
* bootAppSEnet - Set Ethernet MAC Address
*
* This routine is the boot shell command handler routine for the modify 
* ethernet MAC address command.  This routine sets the Ethernet MAC address 
* for a network device.
* 
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppSEnet
    (
    char * cmd
    )
    {
    /* skip the command name and point to first parameter */
    bootCmdNameSkip (&cmd, 1);

    sEnet(cmd);

    return BOOT_STATUS_COMPLETE;
    }

