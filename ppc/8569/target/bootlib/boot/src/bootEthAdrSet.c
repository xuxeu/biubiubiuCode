/* bootEthAdrSet.c - Boot loader application ethernet address set component */

/* 
 *  Copyright (c) 2006-2009, 2012 Wind River Systems, Inc.
 * 
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01g,12aug12,mpc  Remove building warnings. (WIND00354175)
01f,11jan12,pgh  Replace sscanf() call.
01e,11dec09,pgh  64-bit conversion
01d,27apr09,pgh  Update documentation.
01c,07may08,pgh  Remove endianess from MAC handling.
01b,06feb07,jmt  Add bootAppPrintf to optimize size
01a,28nov06,h_k  written. (CQ:81271)
*/

/*
DESCRIPTION
This module contains support routines for the VxWorks boot loader 
application Ethernet Address Set component.  This module is added 
to the boot loader application when the INCLUDE_BOOT_ETH_ADR_SET 
component is included.  This module contains an initialization 
routine, adds a command to the boot shell to set the ethernet MAC 
address.  This component uses the bootCommandHandlerAdd() routine 
to add the 'N' command, which only sets the last three bytes of the 
MAC address, to the boot shell.  The 'N' command does not allow the 
vendor specific bytes of the MAC address to be modified; unlike the 
bootEthMacHandler.c component, which adds the 'M' command to the boot 
shell.  Some BSPs use the 'M' command, and others use the 'N' command.  
Therefore, the usage of the 'M' versus 'N' commands is BSP specific.

The 'N' command takes an optional unit number argument.  If no unit 
number is specified, then unit number zero is assumed.  The 'N' command 
then displays the current ethernet MAC address, then prompts for the 
last three bytes of the new MAC address as follows:

\cs
Current Ethernet Address is: 00:A0:1E:00:01:1C
Modify only the last 3 bytes (board unique portion) of Ethernet Address.
The first 3 bytes are fixed at manufacturer's default address block.
00- 00
A0- A0
1E- 1E
00- 
\ce

The first three bytes of the MAC address are fixed and can't be changed.  
The remaining three subsequent values will be displayed for modification.  
If no entry is made, the previous value remains unchanged.

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

/* maximum line length for cmd input to 'N' command. */
#define MAX_LINE        160
#define MAX_ADR_SIZE    6

/* externs */

extern void sysEnetAddrGet ();
extern void sysEnetAddrSet ();

/* local vars */

static UINT8 defaultEnetAddress[3];

/* forward declarations */

void bootEthAdrSetInit (UINT8, UINT8, UINT8);
LOCAL int bootAppMEnet (char * cmd);

/*******************************************************************************
*
* bootEthAdrSetInit - Initialize Ethernet Address Set Component
*
* This routine initializes the boot loader application ethernet address set 
* component.  This routine adds a command to the boot shell to set the
* ethernet MAC address.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void bootEthAdrSetInit 
    (
    UINT8   defEnetAdrs0,   /* Default 1st byte of vendor code */
    UINT8   defEnetAdrs1,   /* Default 2nd byte of vendor code */
    UINT8   defEnetAdrs2    /* Default 3rd byte of vendor code */
    )
    {
    /* setup local vars */

    defaultEnetAddress[0] = defEnetAdrs0;
    defaultEnetAddress[1] = defEnetAdrs1;
    defaultEnetAddress[2] = defEnetAdrs2;

    /* setup command handlers */
    
    bootCommandHandlerAdd ("N", bootAppMEnet, BOOT_CMD_MATCH_STRICT,
                           "N",  "- set ethernet address");
    }

/* Local functions */

/*******************************************************************************
*
* mEnet - Modify the last three bytes of the ethernet address
*
* This routine is given the command line as an argument, and examines it for
* a device unit number.  If no unit number is found, it defaults to a unit
* number of zero.  This routine then displays the current MAC address for the
* specified unit number, and prompts the user for entry of the last three bytes
* of the MAC address.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void mEnet
    (
    char *  pNum   /* Boot line, including unit number. */
    )
    {
    uchar_t byte [MAX_ADR_SIZE];    /* array to hold new Enet addr */
    uchar_t curArr [MAX_ADR_SIZE];  /* array to hold current Enet addr */
    char line [MAX_LINE + 1];
    char *pLine;        /* ptr to current position in line */
    int value;          /* value found in line */
    char *buf;
    _Vx_usr_arg_t unitNum;
    int ix;

    /* Search for unit number of network device. */

    buf = pNum;
    if (bootScanNum (&buf, &unitNum, FALSE) != OK)  /* Use 0 if no unit #. */
       unitNum = 0;

    sysEnetAddrGet ((UINT32)unitNum, curArr);
    bootAppPrintf ("Current Ethernet Address is: ",0,0,0,0,0,0);

    bootAppPrintf ("%02x:%02x:%02x:%02x:%02x:%02x\n", 
                    (_Vx_usr_arg_t)curArr[0], (_Vx_usr_arg_t)curArr[1], 
                    (_Vx_usr_arg_t)curArr[2], (_Vx_usr_arg_t)curArr[3], 
                    (_Vx_usr_arg_t)curArr[4], (_Vx_usr_arg_t)curArr[5]);
    byte[0] = defaultEnetAddress[0];
    byte[1] = defaultEnetAddress[1];
    byte[2] = defaultEnetAddress[2];
    byte[3] = curArr[3];
    byte[4] = curArr[4];
    byte[5] = curArr[5];

    bootAppPrintf ("Modify only the last 3 bytes (board unique portion) of Ethernet Address.\n",0,0,0,0,0,0);
    bootAppPrintf ("The first 3 bytes are fixed at manufacturer's default address block.\n",0,0,0,0,0,0);

    for (ix = 0; ix < 3; ix++)
        bootAppPrintf ("%02x- %02x\n", 
                        (_Vx_usr_arg_t)byte[ix], (_Vx_usr_arg_t)byte[ix],
                        0,0,0,0);


    /* start on fourth byte of enet addr */
    for (ix = 3; ix < 6; ix++)
    {
    /* prompt for substitution */

    bootAppPrintf ("%02x- ", (_Vx_usr_arg_t)byte[ix],0,0,0,0,0);

    /* get substitution value:
     *   skip empty lines (CR only);
     *   quit on end of file or invalid input;
     *   otherwise put specified value at address */

    if (fioRdString (STD_IN, line, MAX_LINE) == EOF)
        break;

    line [MAX_ADR_SIZE] = EOS;  /* make sure input line has EOS */

    for (pLine = line; isspace ((int) *pLine); ++pLine) /* skip leading spaces*/
        ;

    if (*pLine == EOS)          /* skip field if just CR */
        continue;

    if (bootScanNum (&pLine, (_Vx_usr_arg_t *)&value, TRUE) != OK)
        break;              /* quit if not number */

    byte[ix]  = (uchar_t)value;     /* assign new value */
    }

    bootAppPrintf ("\n",0,0,0,0,0,0);

    sysEnetAddrSet (byte[0], byte[1], byte[2], byte[3], byte[4], byte[5]);

    bootAppPrintf ("New Ethernet Address is: ",0,0,0,0,0,0);
    for (ix = 0; ix < 5; ix++)
        bootAppPrintf ("%02x:", (_Vx_usr_arg_t)byte[ix],0,0,0,0,0);
    bootAppPrintf ("%02x\n", (_Vx_usr_arg_t)byte[5],0,0,0,0,0);
    }

/*******************************************************************************
*
* bootAppMEnet - Set the ethernet MAC address
*
* This routine is the command handler for the 'N' command, which sets the 
* ethernet MAC address the network device.
* 
* RETURNS: BOOT_STATUS_COMPLETE
*
* NOMANUAL
*/

LOCAL int bootAppMEnet
    (
    char * cmd
    )
    {
    /* skip the command name and point to first parameter */
    bootCmdNameSkip (&cmd, 1);

    mEnet(cmd);

    return BOOT_STATUS_COMPLETE;
    }
