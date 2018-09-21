/* bootAddMem.c - Boot loader application add-memory component */

/* 
*  Copyright (c) 2006, 2009, 2010 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
01d,13jul10,wqi  Return value of memAddToPool need to be checked.(WIND00221368)
01c,16dec09,pgh  64-bit conversion
01b,29apr09,pgh  Update documentation.
01a,18aug06,jmt  written
*/

/*
DESCRIPTION
This module provides an initialization function that adds the specified 
memory to the allocatable memory pool of the boot loader application.  
The module executes the memAddToPool() routine to add the additional 
memory to the memory pool.  This module is added to the boot application 
when the INCLUDE_BOOT_ADDMEM component is included.  This component is 
typically used on pentium BSPs only.  

INCLUDE FILES: vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <sysLib.h>
#include <memLib.h>

/* defines */

/* typedefs */

/* globals */

/* forward declarations */

STATUS bootAddMemInit(ptrdiff_t addr, ptrdiff_t size);

/*******************************************************************************
*
* bootAddMemInit - Initialize add-memory-to-pool component
*
* This routine initializes the boot loader application INCLUDE_BOOT_ADDMEM 
* component, which adds additional memory to the boot loader applications
* memory pool.  The address and size of the memory to be added to the pool 
* are specified as parameters to this function.  If the address specified 
* is NULL, the address is calculated to be from the top of memory.  If the 
* specified size of the added memory is 0, the size will be defaulted to 
* the difference between the the specified address and the top of memory.  
* The address of memory to be added is specified by the ADDED_BOOTMEM_ADDR 
* configuration parameter, and the size is specified by the ADDED_BOOTMEM_SIZE 
* configuration parameter.  These parameters can be adjusted using the 
* project facility, or by editing the BSPs component definition file.
* 
* RETURNS: 
*   OK if memory is added to the pool
*   ERROR if memory can not be added to the pool
*
* ERRNO
*/

STATUS bootAddMemInit
    (
    ptrdiff_t   addr,   /* Address to allocate or NULL */
    ptrdiff_t   size    /* Size to allocate or 0 */
    )
    {
    STATUS      status = OK;
    ptrdiff_t   memAddr = addr;
    ptrdiff_t   memSize = size;

    /* check for valid values */
    if ((size == (ptrdiff_t)0) && (addr == (ptrdiff_t)NULL)) 
        return ERROR;

    /* calculate addr */
    if (addr == (ptrdiff_t)NULL)
        memAddr = (ptrdiff_t)((void *)sysPhysMemTop()) - memSize;
    else if (memSize == (ptrdiff_t)0)
        memSize = (ptrdiff_t)((void *)sysPhysMemTop()) - addr;

    if ((ptrdiff_t)((void *)sysPhysMemTop()) >= (addr + size))
        {
        /* Only do this if there is enough memory. */

        status = memAddToPool ((char *)memAddr, (size_t)memSize);
        }
    else
        status = ERROR;

    return (status);
    }
