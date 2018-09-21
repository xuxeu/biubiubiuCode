/* qPriDeltaLib.c - priority queue with relative priority sorting library */

/* 
 * Copyright (c) 1989, 1990-1995, 1997-1998, 2003, 2005-2010, 2013
 * Wind River Systems, Inc. 
 *
 * The right to copy, distribute, modify or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */ 

/*
modification history
--------------------
01r,05jan13,jpb  MILS specific fix for advancing the tick queue
01q,09aug10,cww  Clean up compiler warnings
01p,20feb09,zl   changed advanceRtn nTicks argument to ULONG
01o,09feb09,zl   streamlined multi-way queues. LP64 updates.
01n,06feb08,zl   added qPriDeltaNext
01m,13apr07,lei  added qNullRtn for restoreRtn in qPriDeltaClass 
01l,27jan06,jln  removed the vxTicks from qPriDeltaPut() 
01k,16aug05,yvp  Moved references to Create/Delete into qXXXCreateLib.
01j,09jul05,yvp  Moved queue class structure back here.
01i,10mar05,yvp  Moved qPriDelta(Create/Delete) to qPriDeltaCreateLib.c.
                 qPriDeltaClass definition made global. Delete qPriDeltaNullRtn
01h,19jul92,pme  made qPriDeltaRemove return STATUS.
01g,26may92,rrr  the tree shuffle
01f,19nov91,rrr  shut up some ansi warnings.
01e,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -changed VOID to void
		  -changed copyright notice
01d,28sep90,jcf	 documentation.
01c,05jul90,jcf	 added null routine for calibrateRtn field in Q_CLASS.
01b,26jun90,jcf	 fixed queue class definition.
01a,14jun89,jcf	 written.
*/

/*
DESCRIPTION
This library contains routines to manage a priority queue.  The queue is
maintained in priority order with each node key a priority delta to the node
ahead of it.  This queue performs a qPriDeltaPut() operation preportional in
time with number of nodes in the queue.  This queue is used for timer queues
and cannot be used for ready queue management.

This queue complies with the multi-way queue data structures and thus may be
utilized by any multi-way queue.  The priority delta multi-way queue
class is accessed by the global id qPriDeltaClassId.

SEE ALSO: qLib.
*/

#include <vxWorks.h>
#include <dllLib.h>
#include <private/qLibP.h>

/* forward declarations */

Q_NODE * qPriDeltaEach		(Q_HEAD * pQHead, QEACH_USR_FUNC routine,
				 void * pArg);
Q_NODE * qPriDeltaGet		(Q_HEAD * pQHead);
Q_NODE * qPriDeltaGetExpired	(Q_HEAD * pQHead);
ULONG 	 qPriDeltaKey		(Q_NODE * pQNode);
int 	 qPriDeltaInfo		(Q_HEAD * pQHead, Q_NODE * nodes [],
				 int maxNodes);
void 	 qPriDeltaAdvance	(Q_HEAD * pQHead, ULONG nTicks);
void 	 qPriDeltaPut		(Q_HEAD * pQHead, Q_NODE * pQNode, ULONG key);
STATUS 	 qPriDeltaRemove	(Q_HEAD * pQHead, Q_NODE * pQNode);
void 	 qPriDeltaResort	(Q_HEAD * pQHead, Q_NODE * pQNode,
				 ULONG newKey);
Q_NODE * qPriDeltaNext		(Q_HEAD * pQHead, Q_NODE * pQNode);


LOCAL Q_CLASS qPriDeltaClass =
    {
    qPriDeltaPut,
    qPriDeltaGet,
    qPriDeltaRemove,
    qPriDeltaResort,
    qPriDeltaAdvance,
    qPriDeltaGetExpired,
    qPriDeltaKey,
    qPriDeltaInfo,
    qPriDeltaEach,
    NULL,
    qPriDeltaNext,
    &qPriDeltaClass
    };

Q_CLASS_ID qPriDeltaClassId = &qPriDeltaClass;


/******************************************************************************
*
* qPriDeltaInit - initialize a priority delta queue
*
* This routine initializes the specified priority delta queue.
*/

STATUS qPriDeltaInit
    (
    Q_HEAD *	pQHead,
    Q_CLASS_ID	qClassId
    )
    {
    Q_PRI_HEAD * pQPriHead = (Q_PRI_HEAD *) pQHead;

    pQPriHead->pQClass = qClassId;
    DLL_INIT (&pQPriHead->list);	 /* initialize doubly linked list */

    return (OK);
    }

/*******************************************************************************
*
* qPriDeltaPut - insert a node into a priority delta queue
*
* This routine inserts a node into a priority delta queue.  The insertion is
* based on the priority key.  Lower key values are higher in priority.
*/

void qPriDeltaPut
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pNode,
    ULONG	key
    )
    {
    Q_PRI_HEAD *      pQPriHead = (Q_PRI_HEAD *) pQHead;
    Q_PRI_NODE *      pQPriNode = (Q_PRI_NODE *) pNode;
    FAST Q_PRI_NODE * pQNode	= (Q_PRI_NODE *) DLL_FIRST (&pQPriHead->list);

    pQPriNode->key = key;		/* relative queue keeps delta time */

    while (pQNode != NULL)		/* empty list? */
        {
	if (pQPriNode->key < pQNode->key)
	    {
	    /* We've reached the place in the delta queue to insert this task */

	    dllInsert (&pQPriHead->list, DLL_PREVIOUS (&pQNode->node),
		       &pQPriNode->node);

	    /* Now decrement the delta key of the guy behind us. */

	    pQNode->key -= pQPriNode->key;
	    return;				/* we're done */
	    }

	pQPriNode->key -= pQNode->key;
	pQNode = (Q_PRI_NODE *) DLL_NEXT (&pQNode->node);
	}

    /* if we fall through, add to end of delta q */

    dllInsert (&pQPriHead->list, DLL_LAST (&pQPriHead->list), &pQPriNode->node);
    }

/*******************************************************************************
*
* qPriDeltaGet - remove and return first node in priority delta queue
*
* This routine removes and returns the first node in a priority delta queue.  If
* the queue is empty, NULL is returned.
*
* RETURNS
*  Pointer to first queue node in queue head, or
*  NULL if queue is empty.
*/

Q_NODE * qPriDeltaGet
    (
    Q_HEAD *	pQHead
    )
    {
    Q_PRI_HEAD * pQPriHead = (Q_PRI_HEAD *) pQHead;
    DL_NODE *	 pNode;

    DLL_GET (&pQPriHead->list, pNode);

    return ((Q_NODE *) pNode);
    }

/*******************************************************************************
*
* qPriDeltaRemove - remove a node from a priority delta queue
*
* This routine removes a node from the specified priority delta queue.
*/

STATUS qPriDeltaRemove
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode
    )
    {
    Q_PRI_HEAD * pQPriHead = (Q_PRI_HEAD *) pQHead;
    Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) pQNode;
    Q_PRI_NODE * pNextNode = (Q_PRI_NODE *) DLL_NEXT (&pQPriNode->node);

    if (pNextNode != NULL)
	pNextNode->key += pQPriNode->key; /* add key to next node */

    DLL_REMOVE (&pQPriHead->list, &pQPriNode->node);

    return (OK);
    }

/*******************************************************************************
*
* qPriDeltaResort - resort a node to a new position based on a new key
*
* This routine resorts a node to a new position based on a new priority key.
*/

void qPriDeltaResort
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode,
    ULONG	newKey
    )
    {
    qPriDeltaRemove (pQHead, pQNode);
    qPriDeltaPut (pQHead, pQNode, newKey);
    }

/*******************************************************************************
*
* qPriDeltaAdvance - advance a queues concept of time
*
* Priority delta queues keep nodes prioritized by time-to-fire.  The library
* utilizes this routine to advance time.  It is usually called from within a
* clock-tick interrupt service routine.
*/

void qPriDeltaAdvance
    (
    Q_HEAD *	pQHead,
    ULONG	nTicks
    )
    {
    Q_PRI_HEAD * pQPriHead = (Q_PRI_HEAD *) pQHead;
    Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) DLL_FIRST (&pQPriHead->list);

#ifdef _WRS_CONFIG_MILS_VBI
    /*
     * This will advance all the keys in the queue to make up 
     * for any system ticks that were misseced because the guest was
     * not allowed to run due to time partitioning.
     */

    while ((pQPriNode !=NULL) && (nTicks > 0))
        {
        if (nTicks > pQPriNode->key)
            {
            nTicks -= pQPriNode->key;
	    pQPriNode->key = 0;
            }
	else
	    {
	    pQPriNode->key -= nTicks;
	    break;
	    }

        pQPriNode = (Q_PRI_NODE *) DLL_NEXT (&pQPriNode->node);
	}
#else
    if (pQPriNode != NULL)
        {
	if (nTicks > pQPriNode->key)
	    pQPriNode->key = 0;
	else
	    pQPriNode->key -= nTicks;
        }
#endif /* _WRS_CONFIG_MILS_VBI */
    }

/*******************************************************************************
*
* qPriDeltaGetExpired - return a time-to-fire expired node
*
* This routine returns a time-to-fire expired node in a priority delta timer
* queue.  Expired nodes result from a qPriDeltaAdvance(2) advancing a node
* beyond its delay.  As many nodes may expire on a single qPriDeltaAdvance(2),
* this routine should be called within a while loop until NULL is returned.
* NULL is returned when there are no expired nodes.
*
* RETURNS
*  Pointer to first queue node in queue head, or
*  NULL if queue is empty.
*/

Q_NODE * qPriDeltaGetExpired
    (
    Q_HEAD *	pQHead
    )
    {
    Q_PRI_HEAD * pQPriHead = (Q_PRI_HEAD *) pQHead;
    Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) DLL_FIRST (&pQPriHead->list);

    if (pQPriNode != NULL)
	{
	if (pQPriNode->key != 0)
	    pQPriNode = NULL;
	else
	    qPriDeltaRemove (pQHead, (Q_NODE *) pQPriNode);
	}

    return ((Q_NODE *) pQPriNode);
    }

/*******************************************************************************
*
* qPriDeltaKey - return the key of a node
*
* This routine returns the key of a node currently in a priority delta queue.
*
* RETURNS
*  Node's key.
*
* ARGSUSED
*/

ULONG qPriDeltaKey
    (
    Q_NODE *	pQNode		/* node to get key for */
    )
    {
    Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) pQNode;
    FAST ULONG   sum	   = 0;

    while (pQPriNode != NULL)
        {
	sum += pQPriNode->key;
	pQPriNode = (Q_PRI_NODE *) DLL_PREVIOUS (&pQPriNode->node);
	}

    return (sum);	/* return key */
    }

/*******************************************************************************
*
* qPriDeltaInfo - gather information on a priority delta queue
*
* This routine fills up to maxNodes elements of a nodeArray with nodes
* currently in a multi-way queue.  The actual number of nodes copied to the
* array is returned.  If the nodeArray is NULL, then the number of nodes in
* the priority delta queue is returned.
*
* RETURNS
*  Number of node pointers copied into the nodeArray, or
*  Number of nodes in multi-way queue if nodeArray is NULL
*/

int qPriDeltaInfo
    (
    Q_HEAD * pQHead,		/* priority queue to gather list for */
    Q_NODE * nodeArray[],       /* array of node pointers to be filled in */
    int      maxNodes           /* max node pointers nodeArray can accomodate */
    )
    {
    Q_PRI_HEAD * pQPriHead = (Q_PRI_HEAD *) pQHead;
    DL_NODE *    pNode     = DLL_FIRST (&pQPriHead->list);
    Q_NODE **    pElement  = nodeArray;

    if (nodeArray == NULL)		/* NULL node array means return count */
	return (dllCount (&pQPriHead->list));

    while ((pNode != NULL) && (--maxNodes >= 0))
	{
	*(pElement++) = (Q_NODE *) pNode;	/* fill in table */
	pNode = DLL_NEXT (pNode);		/* next node */
	}

    return (int) (pElement - nodeArray);    /* return count of active tasks */
    }

/*******************************************************************************
*
* qPriDeltaEach - call a routine for each node in a queue
*
* This routine calls a user-supplied routine once for each node in the
* queue.  The routine should be declared as follows:
* .CS
*  BOOL routine (pQNode, arg)
*      Q_NODE *   pQNode;	/@ pointer to a queue node          @/
*      void *     pArg;		/@ arbitrary user-supplied argument @/
* .CE
* The user-supplied routine should return TRUE if qPriDeltaEach (2) is to
* continue calling it for each entry, or FALSE if it is done and
* qPriDeltaEach can exit.
*
* RETURNS: NULL if traversed whole queue, or pointer to Q_NODE that
*          qPriDeltaEach stopped on.
*/

Q_NODE * qPriDeltaEach
    (
    Q_HEAD *	   pQHead,	/* queue head of queue to call routine for */
    QEACH_USR_FUNC routine,	/* the routine to call for each table entry */
    void *	   routineArg	/* arbitrary user-supplied argument */
    )
    {
    Q_PRI_HEAD *   pQPriHead = (Q_PRI_HEAD *) pQHead;
    FAST DL_NODE * pNode     = DLL_FIRST (&pQPriHead->list);

    while ((pNode != NULL) && ((* routine) ((Q_NODE *) pNode, routineArg)))
	pNode = DLL_NEXT (pNode);

    return ((Q_NODE *) pNode);	/* return node we ended with */
    }

/*******************************************************************************
*
* qPriDeltaNext - return next node in a delta priority queue 
*
* This routine returns a pointer to the next node in a delta priority queue. 
* If the end of the queue has been reached, it returns NULL.
*
* RETURNS: pointer to next node, or NULL if the end of the queue was reached
*/

Q_NODE * qPriDeltaNext
    (
    Q_HEAD * pQHead,	/* queue head */
    Q_NODE * pQNode		/* current node */
    )
    {
    return ((Q_NODE *) DLL_NEXT ((DL_NODE *) pQNode));
    }
