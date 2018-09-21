/* qPriListLib.c - priority ordered linked list queue library */

/* 
 * Copyright (c) 1989-1992, 2004-2010 Wind River Systems, Inc. 
 *
 * The right to copy, distribute, modify or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */ 

/*
modification history
--------------------
01r,09aug10,cww  Clean up compiler warnings
01q,06feb09,zl   streamlined multi-way queues. LP64 updates.
01p,28aug08,zl   added qTradPriListClass.
01o,06feb08,zl   added qPriListNext()
01n,13apr07,lei  added qNullRtn for restoreRtn in both qPriListClass 
                 and qPriListFromTailClass.
01m,07feb07,jmg  Changed vxTicks to vxAbsTicks.
01l,16aug05,yvp  Moved references to Create/Delete into qXXXCreateLib.
01k,09jul05,yvp  Moved queue class structure back here.
01j,10mar05,yvp  Moved qPriListCreate & qPriListDelete to qPriListCreateLib.c.
                 qPriListClass & qPriListFromTailClass definitions made global.
01i,30mar04,ans  initialized 'attribute' field of Q_HEAD.
01h,19jul92,pme  made qPriListRemove return STATUS.
01g,26may92,rrr  the tree shuffle
01f,19nov91,rrr  shut up some ansi warnings.
01e,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -changed VOID to void
		  -changed copyright notice
01d,28sep90,jcf	 documentation.
01c,05jul90,jcf  added qPriListCalibrate().
01b,26jun90,jcf	 fixed qPriListResort();
01a,14jun89,jcf	 written.
*/

/*
DESCRIPTION
This library contains routines to manage a priority queue.  The queue is
maintained in priority order with simple priority insertion into a linked list.
This queue performs a qPriListPut() operation preportional in time with number
of nodes in the queue.  This general purpose priority queue has many uses
including the basis for priority semaphore queues.

This library cannot be used for ready queue management.

This queue complies with the multi-way queue data structures and thus may be
utilized by any multi-way queue.  The priority list multi-way queue
class is accessed by the global id qPriListClassId.

SEE ALSO: qLib.
*/

#include <vxWorks.h>
#include <dllLib.h>
#include <private/qLibP.h>

Q_NODE * qPriListEach		(Q_HEAD * pQHead, QEACH_USR_FUNC routine,
				 void * arg);
Q_NODE * qPriListGet		(Q_HEAD * pQHead);
ULONG 	 qPriListKey		(Q_NODE * pQNode);
int 	 qPriListInfo		(Q_HEAD * pQHead, Q_NODE * nodes[],
				 int maxNodes);
void 	 qPriListPut		(Q_HEAD * pQHead, Q_NODE * pQNode, ULONG key);
void 	 qPriListPutFromTail	(Q_HEAD * pQHead, Q_NODE * pQNode, ULONG key);
STATUS 	 qPriListRemove		(Q_HEAD * pQHead, Q_NODE * pQNode);
void 	 qPriListResort		(Q_HEAD * pQHead, Q_NODE * pQNode,
				 ULONG newKey);
Q_NODE * qPriListNext		(Q_HEAD * pQHead, Q_NODE * pQNode);

LOCAL void qPriListReadyQResort (Q_HEAD * pQHead, Q_NODE * pQPriNode,
				 ULONG newKey);

LOCAL Q_CLASS qPriListClass =
    {
    qPriListPut,
    qPriListGet,
    qPriListRemove,
    qPriListResort,
    NULL,
    NULL,
    qPriListKey,
    qPriListInfo,
    qPriListEach,
    NULL,
    qPriListNext,
    &qPriListClass
    };

LOCAL Q_CLASS qTradPriListClass =
    {
    qPriListPut,
    qPriListGet,
    qPriListRemove,
    qPriListReadyQResort,
    NULL,
    NULL,
    qPriListKey,
    qPriListInfo,
    qPriListEach,
    NULL,
    qPriListNext,
    &qTradPriListClass
    };

LOCAL Q_CLASS qPriListFromTailClass =
    {
    qPriListPutFromTail,
    qPriListGet,
    qPriListRemove,
    qPriListResort,
    NULL,
    NULL,
    qPriListKey,
    qPriListInfo,
    qPriListEach,
    NULL,
    qPriListNext,
    &qPriListFromTailClass
    };

Q_CLASS_ID qPriListClassId         = &qPriListClass;
Q_CLASS_ID qTradPriListClassId     = &qTradPriListClass;
Q_CLASS_ID qPriListFromTailClassId = &qPriListFromTailClass;


/******************************************************************************
*
* qPriListInit - initialize a priority list queue
*
* This routine initializes the specified priority list queue.
*/

STATUS qPriListInit
    (
    Q_HEAD *	pQHead,
    Q_CLASS_ID  qClassId,
    UINT	attribute
    )
    {
    Q_PRI_HEAD * pQPriHead = (Q_PRI_HEAD *) pQHead;

    pQPriHead->pQClass = qClassId;
    DLL_INIT (&pQPriHead->list);

    /* intialize attribute field of Q_HEAD */

    pQPriHead->attr = attribute;

    return (OK);
    }

/******************************************************************************
*
* qTradPriListSetCompatible - set compatible mode for ready queue class
*
* This routine sets compatible mode for the priority list queue class. When 
* compatible mode is set, a task node is always added as last of of the 
* equal-priority task list.
*
* This routine will be maintained only for a limited time to allow easier
* migration to the new, correct behaviour.
*
* \NOMANUAL
*/

void qTradPriListSetCompatible (void)
    {
    /* override resort routine */

    qTradPriListClassId->resortRtn = qPriListResort;
    }

/*******************************************************************************
*
* qPriListPut - insert a node into a priority list queue
*
* This routine inserts a node into a priority list queue.  The insertion is
* based on the specified priority key.  The lower the key the higher the
* priority.
*/

void qPriListPut
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pNode,
    ULONG	key
    )
    {
    Q_PRI_HEAD *	pQPriHead = (Q_PRI_HEAD *) pQHead;
    Q_PRI_NODE *	pQPriNode = (Q_PRI_NODE *) pNode;
    FAST Q_PRI_NODE *	pQNode	  = (Q_PRI_NODE *) DLL_FIRST (&pQPriHead->list);
    DL_NODE *		pPrevNode;

    pQPriNode->key = key;

    while (pQNode != NULL)
        {
	if (key < pQNode->key)		/* it will be last of same priority */
	    {
	    pPrevNode = DLL_PREVIOUS (&pQNode->node);
	    goto insertNode;
	    }
	pQNode = (Q_PRI_NODE *) DLL_NEXT (&pQNode->node);
	}

    pPrevNode = DLL_LAST (&pQPriHead->list);

insertNode:
    dllInsert (&pQPriHead->list, pPrevNode, &pQPriNode->node);
    }

/*******************************************************************************
*
* qPriListPutFirst - insert a node into a priority list, first of same priority
*
* This routine inserts a node into a priority list queue.  In case of 
* equal priorities the node will be placed first in the queue. 
*
* \NOMANUAL
*/

_WRS_INLINE void qPriListPutFirst
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pNode,
    ULONG	key
    )
    {
    Q_PRI_HEAD *	pQPriHead = (Q_PRI_HEAD *) pQHead;
    Q_PRI_NODE *	pQPriNode = (Q_PRI_NODE *) pNode;
    FAST Q_PRI_NODE *	pQNode    = (Q_PRI_NODE *) DLL_FIRST (&pQPriHead->list);
    DL_NODE *		pPrevNode;

    pQPriNode->key = key;

    while (pQNode != NULL)
        {
	if (key <= pQNode->key)		/* it will be first of same priority */
	    {
	    pPrevNode = DLL_PREVIOUS (&pQNode->node);
	    goto insertNode;
	    }
	pQNode = (Q_PRI_NODE *) DLL_NEXT (&pQNode->node);
	}

    pPrevNode = DLL_LAST (&pQPriHead->list);

insertNode:
    dllInsert (&pQPriHead->list, pPrevNode, &pQPriNode->node);
    }

/*******************************************************************************
*
* qPriListPutFromTail - insert a node into a priority list queue from tail
*
* This routine inserts a node into a priority list queue.  The insertion is
* based on the specified priority key.  The lower the key the higher the
* priority.  Unlike qPriListPut(2), this routine searches for the correct
* position in the queue from the queue's tail.  This is useful if the
* caller has a priori knowledge that the key is of low priority.
*/

void qPriListPutFromTail
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pNode,
    ULONG	key
    )
    {
    Q_PRI_HEAD * pQPriHead = (Q_PRI_HEAD *) pQHead;
    Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) pNode;
    Q_PRI_NODE * pQNode    = (Q_PRI_NODE *) DLL_LAST (&pQPriHead->list);
    DL_NODE *    pPrevNode;

    pQPriNode->key = key;

    while (pQNode != NULL)
        {
	if (key >= pQNode->key)		/* it will be last of same priority */
	    {
	    pPrevNode = &pQNode->node;
	    goto insertNode;
	    }
	pQNode = (Q_PRI_NODE *) DLL_PREVIOUS (&pQNode->node);
	}

    pPrevNode = NULL;

insertNode:
    dllInsert (&pQPriHead->list, pPrevNode, &pQPriNode->node);
    }

/*******************************************************************************
*
* qPriListGet - remove and return first node in priority list queue
*
* This routine removes and returns the first node in a priority list queue.  If
* the queue is empty, NULL is returned.
*
* RETURNS
*  Pointer to first queue node in queue head, or
*  NULL if queue is empty.
*/

Q_NODE * qPriListGet
    (
    Q_HEAD *	pQHead
    )
    {
    Q_PRI_HEAD * pQPriHead = (Q_PRI_HEAD *) pQHead;

    if (DLL_EMPTY (pQPriHead))
	return (NULL);

    return ((Q_NODE *) dllGet (&pQPriHead->list));
    }

/*******************************************************************************
*
* qPriListRemove - remove a node from a priority list queue
*
* This routine removes a node from the specified priority list queue.
*/

STATUS qPriListRemove
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode
    )
    {
    Q_PRI_HEAD * pQPriHead = (Q_PRI_HEAD *) pQHead;
    Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) pQNode;

    dllRemove (&pQPriHead->list, &pQPriNode->node);

    return (OK);
    }

/*******************************************************************************
*
* qPriListResort - resort a node to a new position based on a new key
*
* This routine resorts a node to a new position based on a new key.
*/

void qPriListResort
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode,
    FAST ULONG	newKey
    )
    {
    FAST Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) pQNode;
    FAST Q_PRI_NODE * pPrev     = (Q_PRI_NODE *)DLL_PREVIOUS (&pQPriNode->node);
    FAST Q_PRI_NODE * pNext     = (Q_PRI_NODE *)DLL_NEXT (&pQPriNode->node);

    if (((pPrev == NULL) || (newKey >= pPrev->key)) &&
	((pNext == NULL) || (newKey <= pNext->key)))
	{
	pQPriNode->key = newKey;
	}
    else
	{
	qPriListRemove (pQHead, pQNode);
	qPriListPut (pQHead, pQNode, newKey);
	}
    }

/*******************************************************************************
*
* qPriListReadyQResort - resort a node to a new position based on a new key
*
* This routine resorts a node to a new position based on a new key. When the 
* key is increased (i.e. a task priority lowered) the node is placed ahead of
* other nodes with the same key. This is the preferred behaviour for task 
* ready queues.
*
* \NOMANUAL
*/

LOCAL void qPriListReadyQResort
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode,
    FAST ULONG	newKey
    )
    {
    FAST Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) pQNode;
    FAST Q_PRI_NODE * pPrev     = (Q_PRI_NODE *)DLL_PREVIOUS (&pQPriNode->node);
    FAST Q_PRI_NODE * pNext     = (Q_PRI_NODE *)DLL_NEXT (&pQPriNode->node);

    if (((pPrev == NULL) || (newKey >= pPrev->key)) &&
	((pNext == NULL) || (newKey <= pNext->key)))
	{
	pQPriNode->key = newKey;
	}
    else
	{
	qPriListRemove (pQHead, pQNode);

	if (pQPriNode->key > newKey)
	    qPriListPut (pQHead, pQNode, newKey);
	else
	    qPriListPutFirst (pQHead, pQNode, newKey);
	}
    }

/*******************************************************************************
*
* qPriListKey - return the key of a node
*
* This routine returns the key of a node currently in a priority list queue.
*
* RETURNS
*  Node's key.
*/

ULONG qPriListKey
    (
    Q_NODE *	pQNode
    )
    {
    Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) pQNode;

    return (pQPriNode->key);
    }

/*******************************************************************************
*
* qPriListInfo - gather information on a priority list queue
*
* This routine fills up to maxNodes elements of a nodeArray with nodes
* currently in a priority list queue.  The actual number of nodes copied to the
* array is returned.  If the nodeArray is NULL, then the number of nodes in
* the priority list queue is returned.
*
* RETURNS
*  Number of node pointers copied into the nodeArray, or
*  Number of nodes in priority list queue if nodeArray is NULL
*/

int qPriListInfo
    (
    Q_HEAD *	pQHead,		/* priority queue to gather list for */
    Q_NODE *	nodeArray[],	/* array of node pointers to be filled in */
    int		maxNodes	/* max node pointers nodeArray can accomodate */
    )
    {
    Q_PRI_HEAD * pQPriHead = (Q_PRI_HEAD *) pQHead;
    DL_NODE *    pNode     = DLL_FIRST (&pQPriHead->list);
    Q_NODE **    pElement  = nodeArray;

    /* NULL node array means return count */

    if (nodeArray == NULL)
	return (dllCount (&pQPriHead->list));

    while ((pNode != NULL) && (--maxNodes >= 0))
	{
	*(pElement++) = (Q_NODE *) pNode;
	pNode = DLL_NEXT (pNode);
	}

    return (int) (pElement - nodeArray);    /* return count of active tasks */
    }

/*******************************************************************************
*
* qPriListEach - call a routine for each node in a queue
*
* This routine calls a user-supplied routine once for each node in the
* queue.  The routine should be declared as follows:
* .CS
*  BOOL routine (pQNode, arg)
*      Q_NODE * pQNode;		/@ pointer to a queue node          @/
*      void *   arg;		/@ arbitrary user-supplied argument @/
* .CE
* The user-supplied routine should return TRUE if qPriListEach (2) is to
* continue calling it for each entry, or FALSE if it is done and
* qPriListEach can exit.
*
* RETURNS: NULL if traversed whole queue, or pointer to Q_NODE that
*          qPriListEach stopped on.
*/

Q_NODE * qPriListEach
    (
    Q_HEAD  *	   pQHead,	/* queue head of queue to call routine for */
    QEACH_USR_FUNC routine,	/* the routine to call for each table entry */
    void *         routineArg	/* arbitrary user-supplied argument */
    )
    {
    Q_PRI_HEAD *   pQPriHead = (Q_PRI_HEAD *) pQHead;
    FAST DL_NODE * pNode     = DLL_FIRST (&pQPriHead->list);

    while ((pNode != NULL) && ((* routine) ((Q_NODE *)pNode, routineArg)))
	pNode = DLL_NEXT (pNode);

    return ((Q_NODE *) pNode);			/* return node we ended with */
    }

/*******************************************************************************
*
* qPriListNext - return next node in a priority list queue 
*
* This routine returns a pointer to the next node in a priority list queue.
* If the end of the queue has been reached, it returns NULL.
*
* RETURNS: pointer to next node, or NULL if the end of the queue was reached
*/

Q_NODE * qPriListNext
    (
    Q_HEAD * pQHead,		/* queue head */
    Q_NODE * pQNode		/* current node */
    )
    {
    return ((Q_NODE *) DLL_NEXT ((DL_NODE *) pQNode));
    }
