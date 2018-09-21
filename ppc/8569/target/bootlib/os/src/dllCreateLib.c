/* dllCreateLib.c - library for creation & deletion of doubly linked lists  */

/*
 * Copyright (c) 2005, 2009, 2012 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01c,04jun12,rlp  CQ:WIND00352873 - Checked malloc() return value.
01b,12mar09,pcs  In fn. dllCreate remove the typecast to the parameter to fn.
                 malloc.
01a,10mar05,yvp  created by splitting dllCreate & dllDelete from dllLib.c.
*/


/*
DESCRIPTION

This file is a part of dllLib - the library that supports the creation and 
maintenance of doubly linked lists. This file contains functions dllCreate()
and dllDelete(). These two functions have been seperated out from the original
dllLib implementation because they use dynamic memory allocation and freeing.
This has been done to achieve layering of the VxWorks 6.2 source base.

NOMANUAL

INCLUDE FILE: dllLib.h
*/

/* includes */

#include "vxWorks.h"
#include "stdlib.h"
#include "dllLib.h"


/*********************************************************************
*
* dllCreate - create a doubly linked list descriptor
*
* This routine returns a pointer to an initialized doubly linked list
* descriptor.
*
* RETURNS: Pointer to a doubly linked list descriptor, or NULL if ERROR.
*/

DL_LIST *dllCreate (void)
    {
    FAST DL_LIST *pList = (DL_LIST *) malloc (sizeof (DL_LIST));

    if (pList != NULL)
	dllInit (pList);

    return (pList);
    }


/*******************************************************************************
*
* dllDelete - terminate doubly linked list head free associated memory
*
* Terminate the specified list and deallocated associated memory.
*
* RETURNS: OK, or ERROR if doubly linked list could not be deleted.
*
* ARGSUSED
*/

STATUS dllDelete
    (
    DL_LIST *pList     /* pointer to list head to be initialized */
    )
    {
    free ((char *) pList);			/* free list */
    return OK;
    }

