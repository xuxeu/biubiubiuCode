/* bootEndSupport.c - Boot loader application END device support component */

/* 
*  Copyright (c) 2005-2007,2009,2012 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
01h,04jan12,d_c  Defect WIND00202474 Fix WIND00318218
                 Update to use muxEndListGet() instead of iterating
		 through endDevTbl[].
01g,11dec09,pgh  64-bit conversion
01f,29apr09,pgh  Update documentation.
01e,27feb07,jmt  Update help format
01d,06feb07,jmt  Modify bootAppPrint to optimize size
01c,14nov06,jmt  Defect 81465: END Interrupt not disabled before jumping to
                 new image
01b,15aug06,jmt  Update documentation
01a,11jul05,jmt   written
*/

/*
DESCRIPTION
This module contains the support routines for the VxWorks boot loader 
application END device support component.  This module is added to the boot 
loader application when the INCLUDE_BOOT_END component is included.  This 
module consists of an initialization routine for the END device support 
component, and a routine to display a list of the available END device names.  
The initialization routine calls the routine bootPrintDevsFuncAdd() to add 
the END device name print routine to the boot shell.

INCLUDE FILES: bootApp.h bootNetLoad.h end.h endLib.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <string.h>
#include <end.h>
#include <endLib.h>
#include <muxLib.h>
#include <bootApp.h>
#include <bootNetLoad.h>

/* defines */

/* typedefs */

/* globals */

extern bootNetDevStopAllPrototype * bootNetDevStopAllFunc;
IMPORT END_TBL_ENTRY    endDevTbl[];    /* end device table */

/* forward declarations */

STATUS bootEndSupportInit(void);
LOCAL void bootEndPrintDevs(void);

/*******************************************************************************
*
* bootEndSupportInit - Initialize boot loader END Device Support Component
*
* This routine initializes the boot loader application END device support 
* component.  It adds a routine to the boot shell to display the list of 
* available END devices.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS bootEndSupportInit()
    {
    /* Add print devs function */
    bootPrintDevsFuncAdd(bootEndPrintDevs);
    
    /* setup Device Stop routine */
    bootNetDevStopAllFunc = muxDevStopAll;

    return OK;
    }

/*******************************************************************************
*
* bootEndPrintDevs - Display available END network devices for the boot shell
*
* This routine prints a list of available END network devices for the boot
* shell.  This routine is added to the list of device print functions executed
* by the boot shell.
*
* The MUX is queried to obtain the list of available END devices.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void bootEndPrintDevs()
    {
    int         idx;
    int         numEndObjs;
    END_OBJ **  pEndObjArray;
    size_t      endObjArrayLen;

    bootAppPrintf ("\n  Enhanced Network Devices: ",0,0,0,0,0,0);
    
    /* Query MUX to find out how many END devices are available. */
    
    numEndObjs = muxEndListGet (NULL, 0);

    if (numEndObjs > 0)
	{
	
	/* Allocate END object array */
	
	endObjArrayLen = sizeof(END_OBJ *) * numEndObjs;
	pEndObjArray = (END_OBJ **) malloc (endObjArrayLen);

	if (pEndObjArray != NULL)
	    {
	    
	    /* Query MUX for pointers to all available END device objects. */
	    
	    bzero ((char *)pEndObjArray, endObjArrayLen);
	    muxEndListGet (pEndObjArray, numEndObjs);
	    
	    /* Print each device and unit */
	    
	    for (idx = 0; idx < numEndObjs; idx++)
		{
		if (pEndObjArray[idx] != NULL)
		    {
		    bootAppPrintf (" %s%d",
				   (_Vx_usr_arg_t) pEndObjArray[idx]->devObject.name,
				   (_Vx_usr_arg_t) pEndObjArray[idx]->devObject.unit,
				   0,0,0,0
				   );
		    
		    /*
		     * muxEndListGet() increments the reference count for all
		     * returned END objects. This serves to release the reference
		     * to each END object (decrement the reference count) after
		     * the name and unit is printed.
		     */
		    
		    muxDevRelease (pEndObjArray[idx]);
		    }
		}
	    
	    free (pEndObjArray);
	    
	    } /* endif (pEndObjArray != NULL) */
    
	else /* could not malloc END_OBJ pointer array */
	    {
	    bootAppPrintf ("%s", (_Vx_usr_arg_t)  "Could not display END devices"
			   " - malloc failure\n", 0,0,0,0,0);
	    } 
	} /* endif (numEndObjs > 0) */
    }
