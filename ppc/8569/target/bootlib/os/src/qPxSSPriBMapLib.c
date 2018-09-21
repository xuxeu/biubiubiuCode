/* qPxSSPriBMapLib.c - POSIX ready queue management with SCHED_SPORADIC */

/* 
 * Copyright (c) 2006-2010 Wind River Systems, Inc. 
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
01k,06may08,zl   switch to READY_Q_NODE_TO_TCB
01j,06feb08,zl   added qPriBMapNext as nextRtn
01i,13apr07,lei  updated qPxSSPriBMapClass.
01h,06mar07,lei  changed READYQ_NODE_TO_TCB to NODE_PTR_TO_TCB.
01b,06feb07,lei  used READYQ_NODE_TO_TCB instead of OFFSET to get the tcb. 
01a,23may06,jln  created based on qPxPriBMapLib.c/01e
*/

/*
DESCRIPTION
This library contains routines to manage a priority queue for POSIX 
scheduler with SCHED_SPORADIC policy supported. The queue structure 
and algorithm are similar to qPxPriBMapLib.c. This library is included
when the component INCLUDE_PX_SCHED_SPORADIC_POLICY is defined.

The queue is maintained in priority order with no sorting time, so its 
performance is constant. Its restrictions are that it requires a 2K byte 
bit map, and it can only prioritize nodes with keys in the range 0 to 255.

This queue complies with the multi-way queue data structures and thus may be
utilized by any multi-way queue.  The priority bit mapped multi-way queue
class is accessed by the global id qPxSSPriBMapClassId.

SEE ALSO: qLib()
*/

#include <vxWorks.h>
#include <private/qLibP.h>

#include <taskLib.h>
#include <private/taskPxLibP.h>
#include <private/kernelLibP.h>
#include <private/readyQLibP.h>

/* extern declarations */

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

LOCAL void   qPxSSPriBMapPut		(Q_HEAD * pQHead,
    	 	  	     		 Q_NODE * pQNode, ULONG key);
LOCAL STATUS qPxSSPriBMapRemove		(Q_HEAD * pQHead,
                                	 Q_NODE * pQNode);
LOCAL void   qPxSSPriBMapResort		(Q_HEAD * pQHead,
			        	 Q_NODE * pQNode, ULONG newKey);
_WRS_INLINE void qPxSSPriBMapPthreadPut	(Q_HEAD * pQHead, Q_NODE * pQNode,
					 ULONG key, WIND_TCB * pTcb,
					 TASK_PX_SCHED_INFO * pSchedInfo);

LOCAL Q_CLASS qPxSSPriBMapClass =
    {
    qPxSSPriBMapPut,	/* new qPut for POSIX with SCHED_SPORADIC */
    qPriBMapGet,
    qPxSSPriBMapRemove, /* new qRemove for POSIX with SCHED_SPORADIC */
    qPxSSPriBMapResort, /* new qResort for POSIX with SCHED_SPORADIC */
    NULL,
    NULL,
    qPriBMapKey,
    qPriBMapInfo,
    qPriBMapEach,
    qPriBMapRestore,
    qPriBMapNext,
    &qPxSSPriBMapClass
    };

Q_CLASS_ID qPxSSPriBMapClassId = &qPxSSPriBMapClass;

/*******************************************************************************
* 
* qPxSSPriBMapPut - insert a node into a priority bit mapped queue
*
* This routine inserts a node into a priority bit mapped queue.  The insertion
* is based on the specified priority key which is constrained to the range
* 0 to 255.  The highest priority is zero.
*
* RETURNS: N/A 
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void qPxSSPriBMapPut
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

    if (pSchedInfo == NULL)
        qPriBMapPut (pQHead, pQNode, key);
    else
	qPxSSPriBMapPthreadPut (pQHead, pQNode, key, pTcb, pSchedInfo);
    }

/*******************************************************************************
*
* qPxSSPriBMapRemove - remove a node from a priority bit mapped queue
*
* This routine removes a node from the specified bit mapped queue.
* 
* RETURNS: OK
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL STATUS qPxSSPriBMapRemove
    (
    Q_HEAD *	pQHead,
    Q_NODE *	pQNode
    )
    {
    TASK_PX_SCHED_INFO * pSchedInfo;

    /* use qPriBMapRemove() since it is optimized in most arch */

    qPriBMapRemove (pQHead, pQNode);

    /* get the address to TASK_PX_SCHED_INFO */

    pSchedInfo = (TASK_PX_SCHED_INFO *) 
                  (READY_Q_NODE_TO_TCB (pQNode))->pSchedInfo;

    if ((pSchedInfo != NULL) &&	
        (pSchedInfo->pxBaseSched.schedPolicy & VX_POSIX_SCHED_SPORADIC) &&  
        ((((TASK_PX_SS_SCHED_INFO *) pSchedInfo)->curState == 
	PX_SS_STATE_ACTIVE))) 
        {
	pxSSThreadStateBlock ((TASK_PX_SS_SCHED_INFO *) pSchedInfo);
        }

    return (OK);
    }

/*******************************************************************************
*
* qPxSSPriBMapResort - resort a node to a new position based on a new key
*
* This routine resorts a node to a new position based on a new priority key.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void qPxSSPriBMapResort
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

    /* if not POSIX task then use resort routine of native tasks */

    if (pSchedInfo == NULL)
	_func_qPriBMapNativeResort (pQHead, pQNode, newKey);
    else
	{
        /* 
         * qPxSSPriBmapRemove() is not involved since (1) if a SCHED_SPORADIC 
         * thread is resorted in the ready queue (thread is READY already),
         * it is not necessary to call the qPriSSBmapRemove() to mark the
         * thread to be BLOCK since it is not necessary to remark new 
         * activation time in qPxSSPriBMapPut(); (2) If a SCHED_SPOARDIC thread
         * is resorted when it has been in the low-priority state, it is also 
         * not need to call qPxSSPriBmapRemove() since it is not necessary to 
         * track the execution time when thread is in low-priority state; 
         * (3) if a SCHED_SPORADIC thread's priority is lowered due to running 
         * out of its execution capacity, its state has also been changed to 
         * LOWPRIO; (4) if a SCHED_SPORADIC thread's priority is rasied due to 
         * replenishment timeout, its state has been marked ACTIVE so that it is 
         * no need to call the qPxSSPriBmapRemove() to mark the thread being 
         * BLOCK and then calling qPxSSPriBmapPut() to mark the thread ACTIVE 
         * again.
         */

	qPriBMapRemove (pQHead, pQNode);
	qPxSSPriBMapPthreadPut (pQHead, pQNode, newKey, pTcb, pSchedInfo);
	}
    }

/*******************************************************************************
*
* qPxSSPriBMapPthreadPut - insert a POSIX thread into a priority queue
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

_WRS_INLINE void qPxSSPriBMapPthreadPut
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

    sched = pSchedInfo->pxBaseSched.schedPolicy;

    /* 
     * The SCHED_SPORADIC thread becomes ACTIVE. Using <priNormal> rather
     * than running <priority> (key) below implies that the execution 
     * time will also be counted for the SCHED_SPORADIC thread running with
     * raised priority due to priority inheritance. A VxWorks's task/thread 
     * is always running with the priority equal to or higher than the 
     * <priNormal> priority. 
     *
     * In the case of taskDelay(0), Q_REMOVE() and then Q_PUT() are invoked.
     * The SCHED_SPOARDIC thread is regarded as changing state from 
     * NOT-READY to READY at zero period, a new activation time is marked.   
     * 
     * In the case of the priority being raised due to priority-inversion
     * mutex, the Q_REMOVE() and then Q_PUT() are also invoked. This 
     * operation may be achieved by Q_RESORT(). In this case, there are two
     * options (1) the SCHED_SPOARDIC thread is regarded as changing state 
     * from BLOCK at its regular high priority level to READY at the higher
     * priority level within zero duration, a new activation time is marked;
     * (2) If calling via Q_RESORT(), Q_REMOVE() would simply remove 
     * the node from the ready queue without changing its state, and the 
     * following Q_PUT() does not need to mark the new activation time 
     * since the thread has been ACTIVE. Both implementation options work 
     * fine with POSIX SCHED_SPOARDIC policy.     
     */

    if ((sched & VX_POSIX_SCHED_SPORADIC) && /* SCHED_SPORADIC thread */
        ((((TASK_PX_SS_SCHED_INFO *) pSchedInfo)->curState &
         (PX_SS_STATE_LOWPRIO | PX_SS_STATE_ACTIVE)) == 0) &&
        (pTcb->priNormal == ((TASK_PX_SS_SCHED_INFO *) pSchedInfo)->regPrio))
        {
	/* 
         * The key (priority) may change if there is no execution capacity 
         * for the SCHED_SPOARDIC thread. In this case, the SCHED_SPORADIC 
         * thread may lower its priority.
	 */

        pxSSThreadStateActive ((TASK_PX_SS_SCHED_INFO *) pSchedInfo, &key);
	}

    if ((sched & VX_POSIX_INSERT_HEAD) &&      /* insert-head is flagged */
        (sched & VX_POSIX_SCHED_MASK) &&       /* POSIX thread */
        (pTcb->priNormal == key))              /* drop to normal prio */
        {
        pSchedInfo->pxBaseSched.schedPolicy = sched & ~VX_POSIX_INSERT_HEAD;
        }
    else
        sched = 0;

    /* check the POSIX thread with SCHED_SPORADIC policy */

    pQPriNode->key = key;

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
