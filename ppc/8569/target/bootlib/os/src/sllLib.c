/* sllLib.c - singly linked list subroutine library */

/* 
 * Copyright (c) 1989-2007, 2009-2010 Wind River Systems, Inc. 
 *
 * The right to copy, distribute, modify or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */ 

/*
modification history
--------------------
01r,04dec10,pad  Replaced FUNCPTR with fully-qualified function pointer in
                 sllLib()'s signature.
01q,26jan09,pcs  Updated to add support for LP64 model.
01p,03may07,zl   updated portable library build configuration.
01o,10mar05,yvp  Moved sllCreate & sllDelete to sllCreateLib.
01n,27feb04,kk   fix SPR# 93622 - ported from AE653
01m,11nov01,dee  add ColdFire support
01l,04sep98,cdp  make ARM CPUs with ARM_THUMB==TRUE use portable routines.
01l,03mar00,zl   merged SH support from T1
01k,22apr97,jpd  added optimised ARM code.
01j,19mar95,dvs  removed tron references.
01i,09jun93,hdn  added a support for I80X86
01h,17jul92,jmm  fixed sllEach to cope with having its current node deleted
01g,26may92,rrr  the tree shuffle
01f,19nov91,rrr  shut up some ansi warnings.
01e,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01d,23jul91,hdn  added conditional macro for optimized TRON codes.
01c,29sep90,jcf  documentation.
01b,10may90,jcf  fixed PORTABLE definition.
01a,03jun89,jcf  written.
*/


/*
DESCRIPTION
This subroutine library supports the creation and maintenance of a
singly linked list.  The user supplies a list head (type SL_LIST)
that will contain pointers to the first and last nodes in the list.
The nodes in the list can be any user-defined structure, but they must reserve
space for a pointer as their first element.  The forward chain is terminated
with a NULL pointer.

.ne 16
NON-EMPTY LIST:
.CS

   ---------             --------          --------
   | head--------------->| next----------->| next---------
   |       |             |      |          |      |      |
   |       |             |      |          |      |      |
   | tail------          | ...  |    ----->| ...  |      |
   |-------|  |                      |                   v
              |                      |                 -----
              |                      |                  ---
              |                      |                   -
              ------------------------

.CE
.ne 12
EMPTY LIST:
.CS

	-----------
        |  head------------------
        |         |             |
        |  tail----------       |
        |         |     v       v
        |         |   -----   -----
        -----------    ---     ---
                        -	-

.CE

INCLUDE FILE: sllLib.h
*/


/* LINTLIBRARY */

#include <vxWorks.h>
#include <stdlib.h>
#include <sllLib.h>
#include <private/vxPortableP.h>


/*******************************************************************************
*
* sllInit - initialize singly linked list head
*
* Initialize the specified list to an empty list.
*
* RETURNS: OK, or ERROR if intialization failed.
*/

STATUS sllInit
    (
    SL_LIST *pList     /* pointer to list head to be initialized */
    )
    {
    pList->head	 = NULL;			/* initialize list */
    pList->tail  = NULL;

    return (OK);
    }

/*******************************************************************************
*
* sllTerminate - terminate singly linked list head
*
* Terminate the specified list.
*
* RETURNS: OK, or ERROR if singly linked list could not be terminated.
*
* ARGSUSED
*/

STATUS sllTerminate
    (
    SL_LIST *pList     /* pointer to list head to be initialized */
    )
    {
    return (OK);
    }

#ifdef _WRS_PORTABLE_sllLib

/*******************************************************************************
*
* sllPutAtHead - add node to beginning of list
*
* This routine adds the specified node to the end of the specified list.
*
* SEE ALSO: sllPutAtTail()
*/

void sllPutAtHead
    (
    SL_LIST *pList,     /* pointer to list descriptor */
    SL_NODE *pNode      /* pointer to node to be added */
    )
    {
    if ((pNode->next = pList->head) == NULL)
	pList->head = pList->tail = pNode;
    else
	pList->head = pNode;
    }

/*******************************************************************************
*
* sllPutAtTail - add node to end of list
*
* This routine adds the specified node to the end of the specified singly
* linked list.
*
* SEE ALSO: sllPutAtHead()
*/

void sllPutAtTail
    (
    SL_LIST *pList,     /* pointer to list descriptor */
    SL_NODE *pNode      /* pointer to node to be added */
    )
    {
    pNode->next = NULL;

    if (pList->head == NULL)
	pList->tail = pList->head = pNode;
    else
	pList->tail->next = pNode;
	pList->tail = pNode;
    }

/*******************************************************************************
*
* sllGet - get (delete and return) first node from list
*
* This routine gets the first node from the specified singly linked list,
* deletes the node from the list, and returns a pointer to the node gotten.
*
* RETURNS: Pointer to the node gotten, or NULL if the list is empty.
*/

SL_NODE *sllGet
    (
    FAST SL_LIST *pList         /* pointer to list from which to get node */
    )
    {
    FAST SL_NODE *pNode;

    if ((pNode = pList->head) != NULL)
        {
        pList->head = pNode->next;

        if (pList->tail == pNode)
            pList->tail = NULL;
        }

    return (pNode);
    }

#endif	/* _WRS_PORTABLE_sllLib */

/*******************************************************************************
*
* sllRemove - remove specified node in list
*
* Remove the specified node in a singly linked list.
*/

void sllRemove
    (
    SL_LIST *pList,             /* pointer to list head */
    SL_NODE *pDeleteNode,       /* pointer to node to be deleted */
    SL_NODE *pPrevNode          /* pointer to previous node or NULL if head */
    )
    {
    if (pPrevNode == NULL)
	{
	pList->head = pDeleteNode->next;
	if (pList->tail == pDeleteNode)
	    pList->tail = NULL;
	}
    else
	{
	pPrevNode->next = pDeleteNode->next;
	if (pList->tail == pDeleteNode)
	    pList->tail = pPrevNode;
	}
    }

/*******************************************************************************
*
* sllPrevious - find and return previous node in list
*
* Find and return the previous node in a singly linked list.
*/

SL_NODE *sllPrevious
    (
    SL_LIST *pList,             /* pointer to list head */
    SL_NODE *pNode              /* pointer to node to find previous node for */
    )
    {
    SL_NODE *pTmpNode = pList->head;

    if ((pTmpNode == NULL) || (pTmpNode == pNode))
	return (NULL);					/* no previous node */

    while (pTmpNode->next != NULL)
	{
	if (pTmpNode->next == pNode)
	    return (pTmpNode);

	pTmpNode = pTmpNode->next;
	}

    return (NULL);					/* node not found */
    }

/*******************************************************************************
*
* sllCount - report number of nodes in list
*
* This routine returns the number of nodes in the given list.
*
* CAVEAT
* This routine must actually traverse the list to count the nodes.
* If counting is a time critical fuction, consider using lstLib which
* maintains a count field.
*
* RETURNS: Number of nodes in specified list.
*
* SEE ALSO: lstLib.
*/

int sllCount
    (
    SL_LIST *pList      /* pointer to list head */
    )
    {
    FAST SL_NODE *pNode = SLL_FIRST (pList);
    FAST int count = 0;

    while (pNode != NULL)
	{
	count ++;
	pNode = SLL_NEXT (pNode);
	}

    return (count);
    }

/*******************************************************************************
*
* sllEach - call a routine for each node in a linked list
*
* This routine calls a user-supplied routine once for each node in the
* linked list.  The routine should be declared as follows:
* .CS
*  BOOL routine (pNode, arg)
*      SL_NODE *pNode;	/@ pointer to a linked list node    @/
*      _Vx_usr_arg_t arg;	/@ arbitrary user-supplied argument @/
* .CE
* The user-supplied routine should return TRUE if sllEach() is to
* continue calling it with the remaining nodes, or FALSE if it is done and
* sllEach() can exit.
*
* RETURNS: NULL if traversed whole linked list, or pointer to DL_NODE that
*          sllEach ended with.
*/

SL_NODE * sllEach
    (
    SL_LIST * pList,		/* linked list of nodes to call routine for */
    BOOL (* routine)		/* the routine to call for each list node */
	(
	SL_NODE * pNode,	/* pointer to a linked list node */
	_Vx_usr_arg_t arg	/* arbitrary user-supplied argument */
        ),
    _Vx_usr_arg_t routineArg	/* arbitrary user-supplied argument */
    )
    {
    FAST SL_NODE *pNode = SLL_FIRST (pList);
    FAST SL_NODE *pNext;

    while (pNode != NULL)
	{
	pNext = SLL_NEXT (pNode);
	if ((* routine) (pNode, routineArg) == FALSE)
	    break;
	pNode = pNext;
	}

    return (pNode);			/* return node we ended with */
    }
