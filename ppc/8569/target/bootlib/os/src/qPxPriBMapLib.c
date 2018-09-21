/* qPxPriBMapLib.c - POSIX bit mapped priority queue management library */

/* 
 * Copyright (c) 2005-2010 Wind River Systems, Inc. 
 *
 * The right to copy, distribute, modify or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */ 

/*
modification history
--------------------
01o,09aug10,cww  Clean up compiler warnings
01n,09feb09,zl   streamlined multi-way queues. LP64 updates.
01m,28aug08,zl   use common resort routine for native tasks.
01l,18jun08,jpb  Removed #include <private/vxCpuLibP.h>
01k,06may08,zl   switch to READY_Q_NODE_TO_TCB().
01j,06feb08,zl   added qPriBMapNext as nextRtn.
01i,13apr07,lei  added qPriBMapRestore for restoreRtn in qPxPriBMapClass
01h,06mar07,lei  changed READYQ_NODE_TO_TCB to NODE_PTR_TO_TCB.
01g,06feb07,lei  used READYQ_NODE_TO_TCB instead of OFFSET to get the tcb. 
01f,30jun06,jln  replace qPxPriBMapSet() with qPriBMapSet()
01e,20jan06,jln  Used pTcb->pSchedInfo as the pointer to TASK_PX_SCHED_INFO
01d,13nov05,jln  collapsed PRI_INHERIT_DATA structure to TCB (SPR# 111864)
01c,31aug05,jln  use TASK_QNODE_TO_PSCHEDINFO macro
01b,16aug05,yvp  Eliminated _WRS_LAYER based conditionals. 
01a,12jul05,jln  created based on qPriBMapLib.c/02v
*/

/*
DESCRIPTION
This library contains routines to manage a priority queue for POSIX scheduler,
The queue structure and algorithm are similar to qPriBMapLib.c except the 
queue is used only as ready queues and has a different qPut() function. 
This library will use the management functions from qPriBMapLib.c and provide 
its own qPxPriBMapPut() function. This library is included when the component 
INCLUDE_PX_SCHED_DEF_POLICIES is defined. 

The queue is maintained in priority order with no sorting time, so its 
performance is constant. Its restrictions are that it requires a 2K byte 
bit map, and it can only prioritize nodes with keys in the range 0 to 255.

This queue complies with the multi-way queue data structures and thus may be
utilized by any multi-way queue.  The priority bit mapped multi-way queue
class is accessed by the global id qPxPriBMapClassId.

SEE ALSO: qLib()
*/

#include <vxWorks.h>
#include <private/qLibP.h>

#include <taskLib.h>
#include <private/taskPxLibP.h>
#include <private/kernelLibP.h>
#include <private/readyQLibP.h>

/* extern */

extern void 	qPriBMapPut	(Q_HEAD * pQHead, Q_NODE * pQNode, ULONG key);
extern Q_NODE *	qPriBMapGet	(Q_HEAD * pQHead);
extern ULONG 	qPriBMapKey	(Q_NODE * pQNode);
extern int 	qPriBMapInfo	(Q_HEAD * pQHead, Q_NODE * nodes[],
				 int maxNodes);
extern void 	qPriBMapRestore	(Q_HEAD * pQHead, Q_NODE * pQNode, ULONG key);
extern STATUS 	qPriBMapRemove	(Q_HEAD * pQHead, Q_NODE * pQNode);
extern void 	qPriBMapResort	(Q_HEAD * pQHead, Q_NODE * pQNode,
				 ULONG newKey);
extern Q_NODE *	qPriBMapNext	(Q_HEAD * pQHead, Q_NODE * pQNode);
extern void     qPriBMapSet	(BMAP_LIST * pBMapList, int priority);
extern Q_NODE *	qPriBMapEach	(Q_HEAD * pQHead, QEACH_USR_FUNC routine,
				 void * pArg);

extern QCLASS_RESORT_FUNC _func_qPriBMapNativeResort;

/* locals */

LOCAL void	 qPxPriBMapPut		(Q_HEAD * pQHead, Q_NODE * pQNode,
					 ULONG key);
LOCAL void	 qPxPriBMapResort	(Q_HEAD * pQHead, Q_NODE * pQNode,
					 ULONG newKey);
_WRS_INLINE void qPxPriBMapPthreadPut	(Q_HEAD * pQHead,
				         Q_NODE * pQNode, ULONG key,
				         WIND_TCB * pTcb,
				         TASK_PX_SCHED_INFO * pSchedInfo);

LOCAL Q_CLASS qPxPriBMapClass =
    {
    qPxPriBMapPut,		/* new qPut function for POSIX */
    qPriBMapGet,
    qPriBMapRemove,
    qPxPriBMapResort,		/* new qResort function for POSIX */
    NULL,
    NULL,
    qPriBMapKey,
    qPriBMapInfo,
    qPriBMapEach,
    qPriBMapRestore,
    qPriBMapNext,
    &qPxPriBMapClass
    };

Q_CLASS_ID qPxPriBMapClassId = &qPxPriBMapClass;

/*******************************************************************************
* 
* qPxPriBMapPut - insert a node into a priority bit mapped queue
*
* This routine inserts a node into a priority bit mapped queue.  The insertion
* is based on the specified priority key which is constrained to the range
* 0 to (Q_PRI_BMAP_MAX_PRIORITIES - 1).  The highest priority is zero.
*
* RETURNS: N/A 
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void qPxPriBMapPut
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode,
    ULONG	key
    )
    {
    WIND_TCB            * pTcb;
    TASK_PX_SCHED_INFO  * pSchedInfo;

    /* get the pointer to WIND_TCB */

    pTcb = READY_Q_NODE_TO_TCB (pQNode);

    /* get the address to TASK_PX_SCHED_INFO */

    pSchedInfo = (TASK_PX_SCHED_INFO *) pTcb->pSchedInfo;

    /* if not POSIX thread then use put routine of native tasks */

    if (pSchedInfo == NULL)
        qPriBMapPut (pQHead, pQNode, key);
    else
	qPxPriBMapPthreadPut (pQHead, pQNode, key, pTcb, pSchedInfo);
    }

/*******************************************************************************
*
* qPxPriBMapResort - resort a node to a new position based on a new key
*
* This routine resorts a node to a new position based on a new priority key.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void qPxPriBMapResort
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode,
    ULONG	newKey
    )
    {
    WIND_TCB            * pTcb;
    TASK_PX_SCHED_INFO  * pSchedInfo;

    /* get the pointer to WIND_TCB */

    pTcb = READY_Q_NODE_TO_TCB (pQNode);

    /* get the address to TASK_PX_SCHED_INFO */

    pSchedInfo = (TASK_PX_SCHED_INFO *) pTcb->pSchedInfo;

    /* if not POSIX thread then use resort routine of native tasks */

    if (pSchedInfo == NULL)
        _func_qPriBMapNativeResort (pQHead, pQNode, newKey);
    else
	{
	qPriBMapRemove (pQHead, pQNode);
	qPxPriBMapPthreadPut (pQHead, pQNode, newKey, pTcb, 
			      pSchedInfo);
	}
    }

/*******************************************************************************
*
* qPxPriBMapPthreadPut - insert a POSIX task node into a priority bit mapped queue
*
* This routine inserts a node corresponding to a POSIX thread into a priority
* bit mapped queue.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

_WRS_INLINE void qPxPriBMapPthreadPut
    (
    Q_HEAD *		 pQHead,
    Q_NODE *		 pQNode,
    ULONG		 key,
    WIND_TCB *		 pTcb,
    TASK_PX_SCHED_INFO * pSchedInfo
    )
    {
    FAST UINT            sched;
    Q_PRI_BMAP_HEAD *	 pQPriBMapHead = (Q_PRI_BMAP_HEAD *) pQHead;
    Q_PRI_NODE *	 pQPriNode     = (Q_PRI_NODE *) pQNode;

    pQPriNode->key = key;

    sched = pSchedInfo->pxBaseSched.schedPolicy;

    if ((sched & VX_POSIX_SCHED_MASK) &&        /* POSIX thread */
	(sched & VX_POSIX_INSERT_HEAD) &&       /* insert-head is flagged */
	(pTcb->priNormal == key))               /* drop to normal prio */
	{
	pSchedInfo->pxBaseSched.schedPolicy = sched & ~VX_POSIX_INSERT_HEAD;
	}
     else
	sched = 0;

    if ((pQPriBMapHead->highNode == NULL) ||             /* empty readyQ */
        (key < pQPriBMapHead->highNode->key) ||          /* higher prio task */
        (sched && (key == pQPriBMapHead->highNode->key))) /* pthread at head */
        {
        pQPriBMapHead->highNode = pQPriNode;
        }

    qPriBMapSet (pQPriBMapHead->pBMapList, (int) key);

    /* insert to head if needed */

    if (sched == 0)	/* insert to the end */
        {
        dllAdd (&pQPriBMapHead->pBMapList->listArray[key], &pQPriNode->node);
        }
    else 		/* insert to the head */
        {
        dllInsert (&pQPriBMapHead->pBMapList->listArray[key], NULL,
                   &pQPriNode->node);
        }
    }
