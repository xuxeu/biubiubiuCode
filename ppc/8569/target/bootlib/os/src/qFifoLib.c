/* qFifoLib.c - wind active queue library */

/*
 * Copyright (c) 1984-2005, 2007-2010 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01r,09aug10,cww  Clean up compiler warnings
01q,25feb09,zl   added resort routine.
01p,06feb09,zl   streamlined multi-way queues. LP64 updates.
01o,06feb08,zl   added qFifoNext.
01n,13apr07,lei  updated qFifoClass to add qFifoRestore API.
01m,16aug05,yvp  Moved references to Create/Delete into qXXXCreateLib.
01l,09jul05,yvp  Moved queue class structure back here.
01k,17may05,yvp  Made qFifoClass a global (related to SPR 109130).
                 Updated copyright notice.
01k,10mar05,yvp  Moved qFifoCreate & qFifoDelete to qFifoCreateLib.c.
                 qFifoClass definition made global. Deleted qFifoNullRtn.
01j,30mar04,ans  initialized 'attribute' Q_HEAD field.
01i,19jul92,pme  made qFifoRemove return STATUS.
01h,26may92,rrr  the tree shuffle
01g,19nov91,rrr  shut up some ansi warnings.
01f,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -changed VOID to void
		  -changed copyright notice
01e,20sep90,jcf  documentation.
01d,10aug90,dnw  changed qFifoPut() to be void.
01c,05jul90,jcf	 added null routine for calibrateRtn field in Q_CLASS.
01b,26jun90,jcf	 fixed qFifoClass definition.
01a,14jun89,jcf	 written.
*/

/*
DESCRIPTION
This library contains routines to manage a first-in-first-out queue.  The
routine qFifoPut takes a key in addition to a node to queue.  This key
determines whether the node is placed at the head or tail of the queue.

This queue complies with the multi-way queue data structures and thus may be
utilized by any multi-way queue.  The FIFO multi-way queue class is accessed
by the global id qFifoClassId.

SEE ALSO: qLib.
*/

#include <vxWorks.h>
#include <dllLib.h>
#include <private/qLibP.h>


/* forward declarations */

Q_NODE * qFifoGet	(Q_HEAD *pQHead);
int 	 qFifoInfo	(Q_HEAD *pQHead, Q_NODE * nodeArray [], int maxNodes);
void 	 qFifoPut	(Q_HEAD *pQHead, Q_NODE *pQNode, ULONG key);
void	 qFifoRestore	(Q_HEAD *pQHead, Q_NODE *pQNode, ULONG key);
STATUS	 qFifoRemove	(Q_HEAD *pQHead, Q_NODE *pQNode);
Q_NODE * qFifoEach	(Q_HEAD *pQHead, QEACH_USR_FUNC routine, void * arg);
Q_NODE * qFifoNext	(Q_HEAD *pQHead, Q_NODE *pQNode);
void	 qFifoResort	(Q_HEAD * pQHead, Q_NODE * pQNode, ULONG newKey);


Q_CLASS qFifoClass =	/* this is a global since it is needed by eventLib */
    {
    qFifoPut,
    qFifoGet,
    qFifoRemove,
    qFifoResort,
    NULL,
    NULL,
    NULL,
    qFifoInfo,
    qFifoEach,
    qFifoRestore,
    qFifoNext,
    &qFifoClass
    };

Q_CLASS_ID qFifoClassId = &qFifoClass;


/******************************************************************************
*
* qFifoInit - initialize a FIFO queue
*
* This routine initializes the specified FIFO queue.
*
* RETURNS: OK, or ERROR if FIFO queue could not be initialized.
*/

STATUS qFifoInit
    (
    Q_HEAD *	pQHead,
    Q_CLASS_ID  qClassId,
    UINT	attribute
    )
    {
    Q_FIFO_HEAD * pQFifoHead = (Q_FIFO_HEAD *) pQHead;

    pQFifoHead->pQClass = qClassId;
    DLL_INIT (&pQFifoHead->list);
 
    /* intialize attribute field of Q_HEAD */

    pQFifoHead->attr = attribute;

    return (OK);
    }

/*******************************************************************************
*
* qFifoPut - add a node to a FIFO queue
*
* This routine adds the specifed node to the FIFO queue.
*
* RETURNS: N/A
*/

void qFifoPut
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode,
    ULONG	key
    )
    {
    Q_FIFO_HEAD * pQFifoHead = (Q_FIFO_HEAD *)pQHead;
    Q_FIFO_NODE * pQFifoNode = (Q_FIFO_NODE *)pQNode;

    DLL_ADD (&pQFifoHead->list, &pQFifoNode->node);
    }

/*******************************************************************************
*
* qFifoRestore - restore a node position in a FIFO queue
*
* This routine adds the specifed node to the FIFO queue and restores its 
* position.
*
* RETURNS: N/A
*/

void qFifoRestore
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode,
    ULONG	key
    )
    {
    Q_FIFO_HEAD * pQFifoHead = (Q_FIFO_HEAD *) pQHead;
    Q_FIFO_NODE * pQFifoNode = (Q_FIFO_NODE *) pQNode;

    DLL_INSERT (&pQFifoHead->list, NULL, &pQFifoNode->node);
    }

/*******************************************************************************
*
* qFifoGet - get the first node out of a FIFO queue
*
* This routines dequeues and returns the first node of a FIFO queue.  If the
* queue is empty, NULL will be returned.
*
* RETURNS: pointer the first node, or NULL if queue is empty.
*/

Q_NODE * qFifoGet
    (
    Q_HEAD *	pQHead
    )
    {
    Q_FIFO_HEAD * pQFifoHead = (Q_FIFO_HEAD *) pQHead;
    DL_NODE *     pNode;

    DLL_GET (&pQFifoHead->list, pNode);

    return ((Q_NODE *) pNode);
    }

/*******************************************************************************
*
* qFifoRemove - remove the specified node from a FIFO queue
*
* This routine removes the specified node from a FIFO queue.
*/

STATUS qFifoRemove
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode
    )
    {
    Q_FIFO_HEAD * pQFifoHead = (Q_FIFO_HEAD *) pQHead;
    Q_FIFO_NODE * pQFifoNode = (Q_FIFO_NODE *) pQNode;

    DLL_REMOVE (&pQFifoHead->list, &pQFifoNode->node);

    return (OK);
    }

/*******************************************************************************
*
* qFifoInfo - resort routine for FIFO queue
*
* Resort routine for FIFO queues. Resort routine is used when the key of a node
* is changed. For FIFO queues the order is not key based, therefore this routine
* is a no-op.
*
* RETURNS: n/a.
*
*/

void qFifoResort 
    (
    Q_HEAD * pQHead, 
    Q_NODE * pQNode, 
    ULONG newKey
    )
    {
    /* queue order is not key based, therefore this is no-op */
    }

/*******************************************************************************
*
* qFifoInfo - get information of a FIFO queue
*
* This routine returns information of a FIFO queue.  If the parameter nodeArray
* is NULL, the number of queued nodes is returned.  Otherwise, the specified
* nodeArray is filled with a node pointers of the FIFO queue starting from the
* head.  Node pointers are copied until the TAIL of the queue is reached or
* until maxNodes has been entered in the nodeArray.
*
* RETURNS: Number of nodes entered in nodeArray, or nodes in FIFO queue.
*
* ARGSUSED
*/

int qFifoInfo
    (
    Q_HEAD *	pQHead,	/* FIFO queue to gather list for */
    Q_NODE *	nodeArray[],	/* array of node pointers to be filled in */
    FAST int	maxNodes	/* max node pointers nodeArray can accomodate */
    )
    {
    Q_FIFO_HEAD *  pQFifoHead = (Q_FIFO_HEAD *) pQHead;
    FAST DL_NODE * pNode      = DLL_FIRST (&pQFifoHead->list);
    FAST Q_NODE ** pElement   = nodeArray;

    /* NULL node array means return count */

    if (nodeArray == NULL)
	return (dllCount (&pQFifoHead->list));

    while ((pNode != NULL) && (--maxNodes >= 0))
	{
	*(pElement++) = (Q_NODE *) pNode;
	pNode = DLL_NEXT (pNode);
	}

    /* return count of active tasks */

    return (int) (pElement - nodeArray);
    }

/*******************************************************************************
*
* qFifoEach - call a routine for each node in a queue
*
* This routine calls a user-supplied routine once for each node in the
* queue.  The routine should be declared as follows:
* .CS
*  BOOL routine (pQNode, arg)
*      Q_NODE * pQNode;	   /@ pointer to a queue node          @/
*      void *	arg;       /@ arbitrary user-supplied argument @/
* .CE
* The user-supplied routine should return TRUE if qFifoEach() is to continue
* calling it for each entry, or FALSE if it is done and qFifoEach() can exit.
*
* RETURNS: NULL if traversed whole queue, or pointer to Q_NODE that
*          qFifoEach stopped on.
*/

Q_NODE * qFifoEach
    (
    Q_HEAD *	   pQHead,	/* queue head of queue to call routine for */
    QEACH_USR_FUNC routine,	/* the routine to call for each table entry */
    void *	   routineArg	/* arbitrary user-supplied argument */
    )
    {
    Q_FIFO_HEAD *  pQFifoHead = (Q_FIFO_HEAD *) pQHead;
    FAST DL_NODE * pQNode     = DLL_FIRST (&pQFifoHead->list);

    while ((pQNode != NULL) && ((* routine) ((Q_NODE *) pQNode, routineArg)))
	pQNode = DLL_NEXT (pQNode);

    return ((Q_NODE *) pQNode);	/* return node we ended with */
    }

/*******************************************************************************
*
* qFifoNext - return next node in a FIFO queue 
*
* This routine returns a pointer to the next node in a FIFO queue. If the 
* end of the queue has been reached, it returns NULL.
*
* RETURNS: pointer to next node, or NULL if the end of the queue was reached
*/

Q_NODE * qFifoNext
    (
    Q_HEAD * pQHead,	/* queue head */
    Q_NODE * pQNode	/* current node */
    )
    {
    Q_FIFO_NODE * pQFifoNode = (Q_FIFO_NODE *) pQNode;

    return ((Q_NODE *) DLL_NEXT (&pQFifoNode->node));
    }
