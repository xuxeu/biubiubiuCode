/* sllCreateLib.c - library for creation & deletion of singly linked lists  */

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
01b,12mar09,pcs  In fn. sllCreate remove the typecast to the parameter to fn.
                 malloc.
01a,10mar05,yvp  created by splitting sllCreate & sllDelete from sllLib.c.
*/


/*
DESCRIPTION

This file is a part of sllLib - the library that supports the creation and 
maintenance of singly linked lists. This file contains functions sllCreate()
and sllDelete(). These two functions have been seperated out from the original
sllLib implementation because they use dynamic memory allocation and freeing.
This has been done to achieve layering of the VxWorks 6.2 source base.

NOMANUAL

INCLUDE FILE: sllLib.h
*/

/* includes */

#include "vxWorks.h"
#include "stdlib.h"
#include "sllLib.h"

/*******************************************************************************
*
* sllCreate - create a singly linked list head
*
* Create, initialize and return a pointer to a singly linked list head.
*
* RETURNS: Pointer to singly linked list head, or NULL if creation failed.
*/

SL_LIST *sllCreate (void)
    {
    SL_LIST *pList = (SL_LIST *) malloc (sizeof (SL_LIST));

    if (pList != NULL)
	sllInit (pList);			/* initialize list */

    return (pList);
    }

/*******************************************************************************
*
* sllDelete - terminate singly linked list head free associated memory
*
* Terminate the specified list.
*
* RETURNS: OK, or ERROR if singly linked list head could not be deallocated.
*
* ARGSUSED
*/

STATUS sllDelete
    (
    SL_LIST *pList     /* pointer to list head to be initialized */
    )
    {
    free ((char *) pList);		/* free list */
    return OK;
    }


