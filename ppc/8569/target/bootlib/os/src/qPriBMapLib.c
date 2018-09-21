/* qPriBMapLib.c - bit mapped priority queue management library */

/* 
 * Copyright (c) 1989, 1990-1995, 1997-1998, 2003, 2005-2010
 * Wind River Systems, Inc. 
 *
 * The right to copy, distribute, modify or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */ 

/*
modification history
--------------------
03i,09aug10,cww  Clean up compiler warnings
03h,05feb09,zl   streamlined multi-way queues. LP64 updates.
03g,28aug08,zl   added qTradPriBMapClassId.
03f,06feb08,zl   added qPriBMapNext()
03e,20sep07,lei  modified qPriBMapRestore to prevent taskIdCurrent from 
                 being preempted by identical-priority non-running tasks.
03d,12jul07,zl   changed dllRemove() to DLL_REMOVE() in qPriBMapRemove().
03c,19jun07,zl   use FFS_LSB() instead of ffsLsb()
03b,13apr07,lei  added qPriBMapRestore API.
03a,03may07,zl   updated portable library build configuration.
02z,17apr07,zl   avoid reversing priority order in bitmaps.
02y,25sep06,aeg  added support for >256 priorities.
02x,30jun06,jln  change qPriBMapSet() to be global
02w,16aug05,yvp  Moved references to Create/Delete into qXXXCreateLib.
02v,13jul05,jln  add INTERNAL section in Description
02u,09jul05,yvp  Moved queue class structure back here.
02t,07jun05,yvp  Updated copyright. #include now with angle-brackets.
02s,08feb05,aeg  added ffsMsb function prototype (SPR #106381).
02r,10mar05,yvp  Moved qPriBMap(Create/Delete), qPriBMapList(Create/Delete) 
                 to qPriBMapCreateLib.c.  Deleted qPriBMapNullRtn.
                 qPriBMapClass definition made global. 
02q,18aug03,dbt  Removed VxSim references (PAL support).
02p,13may03,pes  PAL conditional compilation cleanup. Phase 2.
02o,16nov98,cdp  make ARM CPUs with ARM_THUMB==TRUE use portable routines.
02m,06jan98,cym  added simnt support
02n,22apr97,jpd  removed ARM from PORTABLE list.
02m,29jan97,elp  added ARM support
02l,12jul95,ism  added simsolaris support
02l,04nov94,yao  added PPC support.
02k,11aug93,gae  vxsim hppa from rrr.
02j,12jun93,rrr  vxsim.
02i,27jul92,jcf  all architectures now utilize this library.
                 added nPriority selection for memory conservation.
02h,19jul92,pme  made qPriBMapRemove return STATUS.
02g,18jul92,smb  Changed errno.h to errnoLib.h
02f,26may92,rrr  the tree shuffle
02e,22apr92,jwt  converted CPU==SPARC to CPU_FAMILY==SPARC; copyright.
02d,19nov91,rrr  shut up some ansi warnings.
02c,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -fixed #else and #endif
		  -changed TINY and UTINY to INT8 and UINT8
		  -changed VOID to void
		  -changed copyright notice
02b,25mar91,del  added I960 to portable checklist.
02a,10jan91,jcf  changed BMAP_LIST for portability to other architectures.
01e,20dec90,gae  added declaration of qPriBMapSet and qPriBMapClear.
01d,28sep90,jcf	 documentation.
01c,05jul90,jcf	 added null routine for calibrateRtn field in Q_CLASS.
01b,10may90,jcf	 fixed PORTABLE definition.
		 changed ffs () to ffsMsb ().
01a,14jun89,jcf	 written.
*/

/*
DESCRIPTION
This library contains routines to manage a priority queue.  The queue is
maintained in priority order with no sorting time, so its performance is
constant.  Its restrictions are that it requires a 2K byte bit map, and it
can only prioritize nodes with keys in the range 0 to 
(Q_PRI_BMAP_MAX_PRIORITIES - 1).

This queue complies with the multi-way queue data structures and thus may be
utilized by any multi-way queue.  The priority bit mapped multi-way queue
class is accessed by the global id qPriBMapClassId.

INTERNAL
The functions defined in this library are also used for the POSIX thread
queue management library - qPxPriBMapLib.c. Any change in this file should
also be reviewed to preserve the behavior of POSIX thread scheduler.

SEE ALSO: qLib()
*/

#include <vxWorks.h>
#include <dllLib.h>
#include <string.h>
#include <ffsLib.h>
#include <private/vxPortableP.h>
#include <private/qLibP.h>


/* forward declarations */

void 	 qPriBMapPut	    (Q_HEAD * pQHead, Q_NODE * pQNode, ULONG key);
Q_NODE * qPriBMapGet	    (Q_HEAD * pQHead);
ULONG 	 qPriBMapKey	    (Q_NODE * pQNode);
int 	 qPriBMapInfo	    (Q_HEAD * pQHead, Q_NODE * nodes[], int maxNodes);
void 	 qPriBMapRestore    (Q_HEAD * pQHead, Q_NODE * pQNode, ULONG key);
STATUS 	 qPriBMapRemove	    (Q_HEAD * pQHead, Q_NODE * pQNode);
void 	 qPriBMapResort	    (Q_HEAD * pQHead, Q_NODE * pQNode, ULONG newKey);
Q_NODE * qPriBMapNext	    (Q_HEAD * pQHead, Q_NODE * pQNode);
void	 qPriBMapSet	    (BMAP_LIST * pBMapList, int priority);
Q_NODE * qPriBMapEach	    (Q_HEAD * pQHead, QEACH_USR_FUNC routine,
			     void * routineArg);

#ifdef _WRS_PORTABLE_qPriBMapLib

LOCAL void qPriBMapClear    (BMAP_LIST * pBMapList, int priority);
LOCAL int  qPriBMapHigh	    (BMAP_LIST * pBMapList);

#endif	/* _WRS_PORTABLE_qPriBMapLib */

LOCAL void qPriBMapReadyQResort (Q_HEAD * pQHead, Q_NODE * pQNode,
				 ULONG newKey);


LOCAL Q_CLASS qPriBMapClass =
    {
    qPriBMapPut,
    qPriBMapGet,
    qPriBMapRemove,
    qPriBMapResort,
    NULL,
    NULL,
    qPriBMapKey,
    qPriBMapInfo,
    qPriBMapEach,
    qPriBMapRestore,
    qPriBMapNext,
    &qPriBMapClass
    };

LOCAL Q_CLASS qTradPriBMapClass =
    {
    qPriBMapPut,
    qPriBMapGet,
    qPriBMapRemove,
    qPriBMapReadyQResort,
    NULL,
    NULL,
    qPriBMapKey,
    qPriBMapInfo,
    qPriBMapEach,
    qPriBMapRestore,
    qPriBMapNext,
    &qTradPriBMapClass
    };

Q_CLASS_ID qPriBMapClassId = &qPriBMapClass;
Q_CLASS_ID qTradPriBMapClassId = &qTradPriBMapClass;

/* the POSIX scheduler uses this for native tasks */

QCLASS_RESORT_FUNC _func_qPriBMapNativeResort =  qPriBMapReadyQResort;


/******************************************************************************
*
* qPriBMapInit - initialize a bit mapped priority queue
*
* Initialize the bit mapped priority queue pointed to by the specified queue
* header.
*
* RETURNS: OK or ERROR
*/

STATUS qPriBMapInit
    (
    Q_HEAD *	pQHead,
    Q_CLASS_ID	qClassId,
    BMAP_LIST *	pBMapList,
    UINT	nPriority	/* 1 to Q_PRI_BMAP_MAX_PRIORITIES */
    )
    {
    FAST UINT ix;
    Q_PRI_BMAP_HEAD * pQPriBMapHead = (Q_PRI_BMAP_HEAD *) pQHead;

    if ((nPriority < 1) || (nPriority > Q_PRI_BMAP_MAX_PRIORITIES))
	return (ERROR);

    if (pBMapList == NULL)
	return (ERROR);

    pQPriBMapHead->pQClass   = qClassId;
    pQPriBMapHead->pBMapList = pBMapList;	/* store bmap list pointer */

    /* initialize the q */

    for (ix = 0; ix < nPriority; ++ix)
	dllInit (&pBMapList->listArray[ix]);

    pQPriBMapHead->highNode	= NULL;		/* zero the highest node */
    pQPriBMapHead->nPriority	= nPriority;	/* higest legal priority */

    /* zero the bit maps */

    pBMapList->metaBMap = 0;
    bzero ((char *) pBMapList->bMap, sizeof (pBMapList->bMap));

    return (OK);
    }

/******************************************************************************
*
* qTradPriBMapSetCompatible - set compatible mode for ready queue class
*
* This routine sets compatible mode for the bit mapped priority list queue 
* class.  When compatible mode is set, a task node is always added to the end
* of the equal-priority task list.
*
* This routine will be maintained only for a limited time to allow easier
* migration to the new, correct behaviour.
*
* \NOMANUAL
*/

void qTradPriBMapSetCompatible (void)
    {
    /* override resort routine */

    qTradPriBMapClassId->resortRtn = qPriBMapResort;
    _func_qPriBMapNativeResort	   = qPriBMapResort;
    }

#ifdef _WRS_PORTABLE_qPriBMapLib

/*******************************************************************************
*
* qPriBMapPut - insert a node into a priority bit mapped queue
*
* This routine inserts a node into a priority bit mapped queue.  The insertion
* is based on the specified priority key which is constrained to the range
* 0 to (Q_PRI_BMAP_MAX_PRIORITIES - 1).  The highest priority is zero.
*/

void qPriBMapPut
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode,
    ULONG	key
    )
    {
    Q_PRI_BMAP_HEAD *	pQPriBMapHead = (Q_PRI_BMAP_HEAD *) pQHead;
    Q_PRI_NODE *	pQPriNode     = (Q_PRI_NODE *) pQNode;

    pQPriNode->key = key;

    if ((pQPriBMapHead->highNode == NULL) ||
        (key < pQPriBMapHead->highNode->key))
	{
	pQPriBMapHead->highNode = pQPriNode;
	}

    qPriBMapSet (pQPriBMapHead->pBMapList, (int) key);

    dllAdd (&pQPriBMapHead->pBMapList->listArray[key], &pQPriNode->node);
    }

/*******************************************************************************
*
* qPriBMapGet - remove and return first node in priority bit-mapped queue
*
* This routine removes and returns the first node in a priority bit-mapped
* queue.  If the queue is empty, NULL is returned.
*
* RETURNS Pointer to first queue node in queue head, or NULL if queue is empty.
*/

Q_NODE * qPriBMapGet
    (
    Q_HEAD * pQHead
    )
    {
    Q_PRI_BMAP_HEAD *	pQPriBMapHead = (Q_PRI_BMAP_HEAD *) pQHead;
    Q_NODE *		pQNode	      = (Q_NODE *) pQPriBMapHead->highNode;

    if (pQNode != NULL)
	qPriBMapRemove (pQHead, pQNode);

    return (pQNode);
    }

/*******************************************************************************
*
* qPriBMapRemove - remove a node from a priority bit mapped queue
*
* This routine removes a node from the specified bit mapped queue.
*/

STATUS qPriBMapRemove
    (
    Q_HEAD * pQHead,
    Q_NODE * pQNode
    )
    {
    Q_PRI_BMAP_HEAD *	pQPriBMapHead = (Q_PRI_BMAP_HEAD *) pQHead;
    Q_PRI_NODE *	pQPriNode     = (Q_PRI_NODE *) pQNode;

    DLL_REMOVE (&pQPriBMapHead->pBMapList->listArray[pQPriNode->key],
	        &pQPriNode->node);

    if (DLL_EMPTY (&pQPriBMapHead->pBMapList->listArray[pQPriNode->key]))
        {
	qPriBMapClear (pQPriBMapHead->pBMapList, (int) pQPriNode->key);

	if (pQPriNode == pQPriBMapHead->highNode)
	    pQPriBMapHead->highNode =
	      (Q_PRI_NODE *) DLL_FIRST(&pQPriBMapHead->pBMapList->
	      listArray[qPriBMapHigh(pQPriBMapHead->pBMapList)]);
	}
    else if (pQPriNode == pQPriBMapHead->highNode)
	pQPriBMapHead->highNode =
	  (Q_PRI_NODE *) DLL_FIRST (&pQPriBMapHead->pBMapList->
	  listArray[pQPriBMapHead->highNode->key]);

    return (OK);
    }

/*******************************************************************************
*
* qPriBMapRestore - restore a node position in a priority bit mapped queue 
*
* This routine adds a node into a priority bit mapped queue and restores its 
* position.  The insertion is based on the specified priority key which is 
* constrained to the range 0 to (Q_PRI_BMAP_MAX_PRIORITIES - 1).  The highest 
* priority is zero.
*/

void qPriBMapRestore
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode,
    ULONG	key
    )
    {
    DL_NODE *		pNode	      = NULL;
    Q_PRI_BMAP_HEAD *	pQPriBMapHead = (Q_PRI_BMAP_HEAD *) pQHead;
    Q_PRI_NODE *	pQPriNode     = (Q_PRI_NODE *) pQNode;

    pQPriNode->key = key;

    if ((pQPriBMapHead->highNode == NULL) ||
        (key < pQPriBMapHead->highNode->key))
        {
        pQPriBMapHead->highNode = pQPriNode;
        }
    else
        {

	/* 
	 * we don't always restore to head of the queue to avoid 
	 * bumping out a running task on another CPU.
	 */

        if (key == pQPriBMapHead->highNode->key)
            pNode = &pQPriBMapHead->highNode->node;
        }

    qPriBMapSet (pQPriBMapHead->pBMapList, (int) key);

    dllInsert (&pQPriBMapHead->pBMapList->listArray[key], pNode, 
               &pQPriNode->node);

    }
#endif	/* _WRS_PORTABLE_qPriBMapLib */

#ifdef _WRS_PORTABLE_qPriBMapNext

/*******************************************************************************
*
* qPriBMapNext - return next node in a priority bit mapped queue 
*
* This routine returns a pointer to the next node in a priority bit mapped
* queue. If the end of the queue has been reached, it returns NULL.
*
* RETURNS: pointer to next node, or NULL if the end of the queue was reached
*/

Q_NODE * qPriBMapNext
    (
    Q_HEAD *		pQHead,
    Q_NODE *		pQNode
    )
    {
    int			priority;
    UINT32		highBits;
    UINT32		lowBits;
    UINT32		map;
    BMAP_LIST *		pList;
    Q_PRI_BMAP_HEAD *	pQPriBMapHead = (Q_PRI_BMAP_HEAD *) pQHead;
    Q_PRI_NODE *	pQPriNode     = (Q_PRI_NODE *) pQNode;

    /* first try the same priority list */

    if (DLL_NEXT (&pQPriNode->node) != NULL)
	return ((Q_NODE *) DLL_NEXT (&pQPriNode->node));

    pList = pQPriBMapHead->pBMapList;

    /* next check in the same meta-map group */
 
    priority = pQPriNode->key;
    highBits = priority >> 5;
    lowBits  = (priority + 1) & 0x1f;

    /* make sure we don't overlfow into next meta-map group */

    if (lowBits != 0)
	{
	/* 
	 * get the bitmap excluding bits that are for priorities smaller than
	 * or equal to the original node's priority. If there are any such bits 
	 * set, return the first node in the list corresponding to the lowest
	 * order bit.
	 */

	map = pList->bMap[highBits] & ~((1 << lowBits) - 1);
	lowBits = FFS_LSB (map);

	if (lowBits != 0)
	    {
	    priority = ((highBits << 5) | (lowBits - 1));
	    return ((Q_NODE *) DLL_FIRST (&pList->listArray[priority]));
	    }
	}

    /* 
     * finally try in higher order meta-map groups by masking off the bits 
     * corresponding to priorities equal to or smaller than the original 
     * node's priority. If there is no higher order meta-map bit set, there
     * aren't any higher priority nodes in the queue. Otherwise return the
     * first node in the list corresponding to the lowest order bit 
     * of the meta-map group.
     */

    map = pList->metaBMap & ~((1 << (highBits  + 1)) - 1);
    highBits = FFS_LSB (map);

    if (highBits == 0)
	return (NULL);

    highBits--;

    lowBits = FFS_LSB (pList->bMap[highBits]) - 1;

    priority = ((highBits << 5) | lowBits);

    return ((Q_NODE *) DLL_FIRST (&pList->listArray[priority]));
    }

#endif	/* _WRS_PORTABLE_qPriBMapNext */

/*******************************************************************************
*
* qPriBMapResort - resort a node to a new position based on a new key
*
* This routine resorts a node to a new position based on a new priority key.
*/

void qPriBMapResort
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode,
    ULONG	newKey
    )
    {
    Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) pQNode;

    if (pQPriNode->key != newKey)
	{
	qPriBMapRemove (pQHead, pQNode);
	qPriBMapPut (pQHead, pQNode, newKey);
	}
    }

/*******************************************************************************
*
* qPriBMapReadyQResort - resort a node to a new position based on a new key
*
* This routine resorts a node to a new position based on a new priority key.
* This is similar to qPriBMapResort, except that when the key is increased 
* (i.e. a task priority lowered) the node is placed ahead of other nodes with 
* the same key. This is the preferred behaviour for task ready queues.
*
* \NOMANUAL
*/

LOCAL void qPriBMapReadyQResort
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode,
    ULONG	newKey
    )
    {
    Q_PRI_BMAP_HEAD * pQPriBMapHead = (Q_PRI_BMAP_HEAD *) pQHead;
    Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) pQNode;

    /* 
     * when priority is lowered (higher key value) keep LIFO order for 
     * identical-priority nodes. When priority is raised, keep FIFO order.
     */

    if (pQPriNode->key > newKey)
	{
	qPriBMapRemove (pQHead, pQNode);
	qPriBMapPut (pQHead, pQNode, newKey);
	}
    else if (pQPriNode->key < newKey)
	{
	qPriBMapRemove (pQHead, pQNode);

	/*
	 * the following is almost the same as qPriBMapRestore(); the difference 
	 * is that qPriBMapRestore() is not allowed to add the node in front 
	 * of highNode.
	 */

	pQPriNode->key = newKey;

	if ((pQPriBMapHead->highNode == NULL) ||
	    (newKey <= pQPriBMapHead->highNode->key))
            {
            pQPriBMapHead->highNode = pQPriNode;
            }

	qPriBMapSet (pQPriBMapHead->pBMapList, (int) newKey);

	dllInsert (&pQPriBMapHead->pBMapList->listArray[newKey], NULL, 
        	   &pQPriNode->node);
	}
    }

/*******************************************************************************
*
* qPriBMapKey - return the key of a node
*
* This routine returns the key of a node currently in a multi-way queue.  The
* keyType is ignored.
*
* RETURNS: Node's key.
*
* ARGSUSED
*/

ULONG qPriBMapKey
    (
    Q_NODE  *	pQNode		/* node to get key for */
    )
    {
    Q_PRI_NODE * pQPriNode = (Q_PRI_NODE *) pQNode;
    return (pQPriNode->key);
    }

/*******************************************************************************
*
* qPriBMapInfo - gather information on a bit mapped queue
*
* This routine fills up to maxNodes elements of a nodeArray with nodes
* currently in a multi-way queue.  The actual number of nodes copied to the
* array is returned.  If the nodeArray is NULL, then the number of nodes in
* the multi-way queue is returned.
*
* RETURNS: Number of node pointers copied into the nodeArray, or number of
*	   nodes in bit mapped queue if nodeArray is NULL
*/

int qPriBMapInfo
    (
    Q_HEAD *	pQHead,		/* bmap q to gather list for */
    Q_NODE *	nodeArray[],	/* array of node pointers for filling */
    int		maxNodes	/* max node pointers for nodeArray */
    )
    {
    FAST Q_PRI_NODE *	pNode;
    Q_NODE **		pElement = nodeArray;
    FAST UINT		ix;
    int			count = 0;
    Q_PRI_BMAP_HEAD *	pQPriBMapHead = (Q_PRI_BMAP_HEAD *) pQHead;

    if (nodeArray == NULL)
	{
	for (ix = 0; ix < pQPriBMapHead->nPriority; ++ix)
	    count += dllCount (&pQPriBMapHead->pBMapList->listArray[ix]);
	return (count);
	}

    for (ix = 0; ix < pQPriBMapHead->nPriority; ++ix) /* search the array */
	{
	pNode = (Q_PRI_NODE *)
		DLL_FIRST (&pQPriBMapHead->pBMapList->listArray[ix]);

	while ((pNode != NULL) && (--maxNodes >= 0))	/* anybody left? */
	    {
	    *(pElement++) = (Q_NODE *) pNode;		/* fill in table */

	    pNode = (Q_PRI_NODE *) DLL_NEXT (&pNode->node);  /* next node */
	    }

	if (maxNodes < 0)		/* out of room? */
	    break;
	}

    return (int) (pElement - nodeArray);    /* return count of active tasks */
    }

/*******************************************************************************
*
* qPriBMapEach - call a routine for each node in a queue
*
* This routine calls a user-supplied routine once for each node in the
* queue.  The routine should be declared as follows:
* .CS
*  BOOL routine (pQNode, arg)
*      Q_NODE * pQNode;		/@ pointer to a queue node          @/
*      void *   arg;		/@ arbitrary user-supplied argument @/
* .CE
* The user-supplied routine should return TRUE if qPriBMapEach() is to
* continue calling it for each entry, or FALSE if it is done and
* qPriBMapEach() can exit.
*
* RETURNS: NULL if traversed whole queue, or pointer to Q_NODE that
*          qPriBMapEach stopped on.
*/

Q_NODE * qPriBMapEach
    (
    Q_HEAD *	   pQHead,	/* queue head of queue to call routine for */
    QEACH_USR_FUNC routine,	/* the routine to call for each table entry */
    void *	   routineArg	/* arbitrary user-supplied argument */
    )
    {
    FAST UINT		ix;
    FAST Q_PRI_NODE *	pNode	      = NULL;
    Q_PRI_BMAP_HEAD *	pQPriBMapHead = (Q_PRI_BMAP_HEAD *) pQHead;

    for (ix = 0; ix < pQPriBMapHead->nPriority; ++ix)	/* search array */
	{
	pNode = (Q_PRI_NODE *)
		DLL_FIRST (&pQPriBMapHead->pBMapList->listArray[ix]);

	while (pNode != NULL)
	    {
	    if (!((* routine) ((Q_NODE *) pNode, routineArg)))
		goto done;				/* bail out */

	    pNode = (Q_PRI_NODE *) DLL_NEXT (&pNode->node);
	    }
	}

done:

    return ((Q_NODE *) pNode);			/* return node we ended with */
    }

#ifdef _WRS_PORTABLE_qPriBMapLib

/*******************************************************************************
*
* qPriBMapSet - set the bits in the bit map for the specified priority
*
* This routine sets the bits in the bit map to reflect the addition of a node
* of the specified priority.
*/

void qPriBMapSet
    (
    BMAP_LIST *	pBMapList,
    int		priority
    )
    {
    pBMapList->metaBMap			|= (1 << (priority >> 5));
    pBMapList->bMap [priority >> 5]	|= (1 << (priority & 0x1f));
    }

/*******************************************************************************
*
* qPriBMapClear - clear the bits in the bit map for the specified priority
*
* This routine clears the bits in the bit map to reflect the removal of a node
* of the specified priority.
*/

LOCAL void qPriBMapClear
    (
    BMAP_LIST *	pBMapList,
    int		priority
    )
    {
    pBMapList->bMap [priority >> 5] &= ~(1 << (priority & 0x1f));

    if (pBMapList->bMap [priority >> 5] == 0)
	pBMapList->metaBMap &= ~(1 << (priority >> 5));
    }

/*******************************************************************************
*
* qPriBMapHigh - return highest priority in ready queue
*
* This routine utilizes the bit map structure to determine the highest active
* priority group.
*
* RETURNS: Priority of highest active priority group.
*/

LOCAL int qPriBMapHigh
    (
    BMAP_LIST *	pBMapList
    )
    {
    UINT32 highBits = FFS_LSB (pBMapList->metaBMap);
    UINT32 lowBits;

    if (highBits == 0)
	return (0);

    highBits--;

    lowBits = FFS_LSB (pBMapList->bMap[highBits]) - 1;

    return ((highBits << 5) | lowBits);
    }

#endif	/* _WRS_PORTABLE_qPriBMapLib */
