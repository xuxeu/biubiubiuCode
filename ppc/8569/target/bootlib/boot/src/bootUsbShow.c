/* bootUsbShow.c - Boot loader application USB show component */

/* 
*  Copyright (c) 2010, 2012 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
01b,12aug12,mpc  Remove building warnings. (WIND00354175)
01a,12nov10,pgh  written
*/

/*
DESCRIPTION
This module contains the support routines for the VxWorks boot loader 
application USB show routine component.  This module is added to the boot 
loader application when the INCLUDE_BOOT_USB_SHOW component is included.  
This module contains an initialization routine for the USB show routine 
component, and the USB show routine itself.  The initialization routine 
calls the routine bootCommandHandlerAdd() to add the command "usbShow" to 
the boot shell, which executes the USB show routine.

INCLUDE FILES: bootFsLoad.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <sysLib.h>
#include <bootApp.h>


/* defines */

/* typedefs */

/* globals */

extern void usbShow(void);

/* local declarations */

/* forward declarations */

void bootUsbShowInit(void);

/* local command handlers */

LOCAL int bootUsbShow (char * cmd);

/*******************************************************************************
*
* bootUsbShowInit - Initialize boot application USB show component
*
* This routine initializes the boot loader application USB show component.  
* This routine adds the command "usbShow" to the boot shell.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void bootUsbShowInit ()
    {

    /* setup command handler */

    bootCommandHandlerAdd("usbShow", bootUsbShow, 
                          BOOT_CMD_MATCH_STRICT,
                          "usbShow", "- show USB Devices");
    }

/******************************************************************************
*
* bootUsbShow - USB device show routine
* 
* This routine displays information on USB devices.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootUsbShow 
    (
    char * cmd  /* shell input command string */
    )
    {
    usbShow ();
    return BOOT_STATUS_COMPLETE;
    }

