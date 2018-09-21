/* rBuffLib.c - dynamic ring buffer (rBuff) library */

/*
 * Copyright (c) 1997-1998, 2000, 2003-2011 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
02u,30aug11,vhe  Changed spinLock flag (WIND00190423). 
02z,10feb10,tcr  update for 64bit
02y,05may09,jpb  Updated logMsg() arguments for LP64 support.
02x,30jul09,jpb  Fixed build warnings.
02w,24jul09,kk   rename DETERMINISM to DETERMINISTIC
02v,17jun09,tcr  allow selection of spinlock type (WIND00167118)
02u,10nov08,tcr  change spinlock type to spinlockIsrNd_t (WIND00136245)
02t,16sep08,tcr  add documentation
02s,27aug08,jpb  Renamed VSB header file
02r,18jun08,jpb  Renamed _WRS_VX_SMP to _WRS_CONFIG_SMP.  Added include path
                 for kernel configurations options set in vsb.
02q,11sep07,tcr  Update buffer statistics (WIND00104051), initialize spinlock,
                 fix buffer-full handling (WIND00104334)
02p,22may07,tcr  fixes and tidying up
02o,04apr07,jmg  Changed spinLockIsrTake/Give to SPIN_LOCK_ISR_TAKE/GIVE.
02n,02jan07,tcr  add callbacks, SMP support
02m,27sep06,pcs  Changed macro IS_KERNEL_STATE_ME to KERNEL_ENTERED_ME.
02l,18jul05,md   Fix kernel state for SMP
02l,07apr06,dgp  doc: correct prog guide ref in library, wvRBuffMgrPrioritySet()
02k,13jan05,tcr  Fix SPR 101137: rBuff mgr priority should be configurable
02j,09sep04,job  docs fix
02i,14apr04,tcr  add call to error handler when buffer full (SPR 88701)
                 change WindView to 'System Viewer'
02h,27apr04,cjj  Removed portWorkQAdd1 implementation
02g,13jan04,dat  lint removal, char data type
02f,10oct03,tcr  fix rBuffReset()
02e,16sep03,tcr  remove surplus semaphore
02d,12may03,pes  PAL conditional compilation cleanup. Phase 2.
02c,02apr03,to   declassified the BUFFER_DESC structure.
02b,24mar03,dcc  updated to use the new classLib API.
02a,20apr00,max  Change tid == NULL to tid == 0 (like in es.coretools)
01z,28aug98,dgp  FCS man page edit
01y,27aug98,dgp  add lib description, edit wvRBuffMgrPrioritySet() for manpgs
01x,18aug98,cjtc event buffer full handled more gracefully (SPR 22133)
01w,22jul98,cjtc optimise ring buffer scheme for windview (SPR 21752)
		 Avoids multiple rBuffMgr tasks (SPR 21805)
01v,30jun98,cjtc make priority of rBuffMgr task variable (SPR 21543)
01u,06may98,pr   fix wrong spell of portWorkQAdd1 for x86.
01t,01may98,nps  reworked msg passing for rBuff maintenance.
01s,26mar98,nps  fix 2 buffer / continuous upload problem.
01r,25mar98,pr   used portable portWorkQAdd1 only for the I80X86.
                 moved portWorkQAdd1 into trgLib.c
01q,20mar98,pr   replaced workQAdd1 with portable version (for x86).
01p,19mar98,nps  only give upload threshold semaphore when necessary.
01o,13mar98,nps  added rBuffVerify.
01n,03mar98,nps  Source control structures from specfied source partition.
01m,21feb98,nps  Don't use msgQ fn to pass msgs - it causes problems when
                 used with WV instrumentation.
01l,04feb98,nps  remove diagnostic messages.
01k,03feb98,nps  Changed msgQSend option from WAIT_FOREVER to NO_WAIT.
01j,12jan98,nps  Don't initialise rBuff multiple times.
01i,18dec97,cth  updated include files, added flushRtn init to rBuffCreate
01h,25nov97,nps  Tom's fix incorporated.
                 Maintain statistic of peak buffer utilisation.
                 Fix count of empty buffers.
01g,16nov97,cth  changed every public routine to accept generic buffer id,
		 described in buffer.h, rather than rBuffId.
		 added initialization for generic buffer id in rBuffCreate
01f,15Sep97,nps  rBuffShow moved out to its own file.
                 SEMAPHOREs now init'd rather than created.
                 added support for 'infinite' extension.
                 added support for RBUFF_WRAPAROUND.
                 buffers for ring are now added and freed by dedicated task.
01e,18Aug97,nps  fixed counting of emptyBuffs.
01d,18Aug97,nps  use semBGiveDefer to signal event upload.
01c,11Aug97,nps  rBuffReset now resets *all* buffers.
01b,28jul97,nps	 further implementation/testing.
                 rBuffReset returns STATUS type.
                 added rBuffSetFd.
01a,14jul97,nps	 written.
*/

/*
DESCRIPTION

This library provides the ring buffers for use by System Viewer.
A ring buffer contains a number of blocks of data, which are linked into a
ring. The number of blocks in the ring is configurable, and the ring may
grow as collection progresses, up to a configurable maximum size. Optionally,
the ring may be allowed to wrap when it becomes full. When this happens, the
first buffer in the ring will be reused, and its contents overwritten.

When the first ring buffer in a system is created, a manager task (tWvRBuffMgr)
is created. This task handles the adding and removal of buffers from the ring. The
priority of tWvRBuffMgr is configurable, and may also be changed during data
collection.


INTERNAL
This library also contains the non-public routines creating and managing the
dynamic ring buffer.  It provides both a System Viewer-specific version and a
generic version.

For UP build, this library uses intLock to ensure atomic access. On SMP systems,
it uses either IsrSpinlock or IsrNdSpinlock, selected by the
_WRS_CONFIG_DETERMINISTIC config parameter. 

INCLUDE FILES: rBuffLib.h

SEE ALSO: memLib, rngLib,
\tb "VxWorks Kernel Programmer's Guide: Basic OS"
*/
#undef RBUFF_DEBUG

#ifndef  GENERIC_RBUFF

/*
 * Below this point the System Viewer specific version of the ring buffer
 * library. In order to compile with the old (generic version of the
 * ring buffer, you should use EXTRA_DEFINE='-DGENERIC_RBUFF' in the
 * make command line, or #define GENERIC_RBUFF at the head of this
 * file.
 */


#include <vxWorks.h>
#include <vsbConfig.h>
#include <semLib.h>
#include <classLib.h>
#include <errno.h>
#include <logLib.h>
#include <taskLib.h>
#include <fioLib.h>
#include <rBuffLib.h>


#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef _WRS_CONFIG_SMP
#ifdef _WRS_CONFIG_DETERMINISTIC
#include <spinLockLib.h>
#else
#include <spinlockIsrNdLib.h>
#endif /* _WRS_CONFIG_DETERMINISTIC */
#else
#include <intLib.h>
#endif /* _WRS_CONFIG_SMP */

#include <private/spinLockLibP.h>

extern int wvRBuffMgr();

#define WQADD1 workQAdd1

#define MARK_WRITE_BUFFER_FULL(rBuff)   rBuff->buffWrite->spaceAvail = 0

#define MARK_WRITE_BUFFER_EMPTY(rBuff)                                  \
                rBuff->buffWrite->spaceAvail = rBuff->info.buffSize;    \
                rBuff->buffWrite->dataLen =0;                           \
                rBuff->buffWrite->uncommitedRead = FALSE

#define MARK_READ_BUFFER_EMPTY(rBuff)                                   \
                rBuff->buffRead->spaceAvail = rBuff->info.buffSize;     \
                rBuff->buffRead->dataLen =0;                            \
                rBuff->buffRead->uncommitedRead = FALSE


/* locals */

LOCAL STATUS rBuffRecycle (RBUFF_ID rBuff);

LOCAL STATUS rBuffFree (RBUFF_ID rBuff, RBUFF_PTR buffToFree);

LOCAL RBUFF_PTR rBuffElementCreate (RBUFF_ID rBuff);

LOCAL STATUS rBuffInsert (RBUFF_ID rBuff, RBUFF_PTR newBuff);

LOCAL STATUS rBuffHandleEmpty (RBUFF_ID rBuff);

LOCAL void rBuffLock (BUFFER_ID buffId);

LOCAL void rBuffUnlock (BUFFER_ID buffId);


/*
 * No need to lock the buffer here because RBUFF_SEND_MSG is only used when
 * buffer is already locked
 */

#define RBUFF_SEND_MSG(RMSG_TYPE,RMSG_PRI,RB_ID,RMSG_PARAM)		    \
    {									    \
    if ((RB_ID)->nestLevel++ == 0)					    \
    	{			                                            \
    	BOOL doIt = TRUE ;						    \
        if (RMSG_TYPE == RBUFF_MSG_ADD) 				    \
            {							            \
	    if ((RB_ID)->msgOutstanding)				    \
		{							    \
	    	doIt = FALSE;						    \
	    	}							    \
	    (RB_ID)->msgOutstanding = TRUE;				    \
	    }								    \
    	if (doIt)							    \
    	    {								    \
	    WV_RBUFF_MGR_ID pMgr;					    \
	    pMgr = (WV_RBUFF_MGR_ID)(RB_ID)->rBuffMgrId;		    \
	    pMgr->msg[pMgr->msgWriteIndex].ringId = (RB_ID);		    \
	    pMgr->msg[pMgr->msgWriteIndex].msgType = RMSG_TYPE;  	    \
    	    pMgr->msg[pMgr->msgWriteIndex].arg = (_Vx_usr_arg_t)RMSG_PARAM; \
                                                              		    \
    	    if(++pMgr->msgWriteIndex >= WV_RBUFF_MGR_MSGQ_MAX)    	    \
    	    	{                                                           \
            	pMgr->msgWriteIndex = 0;                         	    \
    	    	}                                                           \
	    }								    \
    	}                                                                   \
    (RB_ID)->nestLevel--;						    \
    }

LOCAL BOOL   rBuffLibInstalled = FALSE;   /* protect from multiple inits */

/*
 * This is the lock for the message queue that allows actions to be taken
 * at task level, and protects rBuffs from concurrent access
 */

#ifdef _WRS_CONFIG_SMP
#ifdef _WRS_CONFIG_DETERMINISTIC
LOCAL SPIN_LOCK_ISR_DECL(rBuffSpinlock, _WRS_PRIVATE_LOCK_LEVEL_1);
#else
LOCAL spinlockIsrNd_t   rBuffSpinlock;
LOCAL int               rBuffSpinlockKey;
#endif /* _WRS_CONFIG_DETERMINISTIC */
#else
LOCAL   int     lockLevel;
#endif /* _WRS_CONFIG_SMP */

/* global variables */

IMPORT BOOL  wvEvtBufferFullNotify;	/* event buffer full notification */

WV_RBUFF_MGR_ID  wvRBuffMgrId = NULL;   /* WV rbuff mgr structure id */

int     rBuffPollingInterval = 10;


/* forward declarations */

LOCAL void rBuffMsgQueueLock (void);
LOCAL void rBuffMsgQueueUnlock (void);

/*******************************************************************************
*
* rBuffLibInit - initialize the rBuff library
*
* This routine initializes the ring of buffers library.
*
* RETURNS: OK, or ERROR if the library could not be initialized.
*/

STATUS rBuffLibInit (void)
    {

    if (rBuffLibInstalled)
        {
        return(TRUE);
        }

    /* Initialise the System Viewer rBuff Manager */

    if ((wvRBuffMgrId = malloc (sizeof (WV_RBUFF_MGR_TYPE))) == NULL)
	return (ERROR);

    wvRBuffMgrId->tid = 0;
    wvRBuffMgrId->priorityDefault = wvRBuffMgrPriorityDefault;

    wvRBuffMgrId->msgWriteIndex = 0;
    wvRBuffMgrId->msgReadIndex = 0;

#ifdef _WRS_CONFIG_SMP
#ifdef _WRS_CONFIG_DETERMINISTIC
    SPIN_LOCK_ISR_INIT (&rBuffSpinlock, _WRS_PRIVATE_LOCK_LEVEL_1);
#else
    spinLockIsrNdInit (&rBuffSpinlock);
#endif /* _WRS_CONFIG_DETERMINISTIC */
#endif /* _WRS_CONFIG_SMP */

    rBuffLibInstalled = TRUE;

    return (OK);
    }


/*******************************************************************************
*
* rBuffNewBuffCallbackAdd - add a callback for a new buffer
*
* This funtion adds a callback to be invoked when a new buffer in the ring is
* started. This will allow the client to save any required context information
* into the buffer, such as current CPU or time of day.
*
* Note that callbacks will be executed with a spinlock held, and must not make
* any kernel calls, and must not log any System Viewer events.
*
* RETURNS: OK, or ERROR if the callback list is full
*
* \NOMANUAL
*/

LOCAL STATUS rBuffNewBuffHookAdd
    (
    BUFFER_ID   buffer,
    STATUS      (*callback)(BUFFER_ID buffId)
    )
    {
    RBUFF_ID    rBuffId = (RBUFF_ID)buffer;

    if (callback != NULL && rBuffId->numCallbacks < NUM_NEW_BUFF_CALLBACKS)
        {
        rBuffId->callbackList [rBuffId->numCallbacks++] = callback;
        return (OK);
        }
    else
        return (ERROR);
    }


/*******************************************************************************
*
* rBuffNewBufferCallbackExecute - execute the callbacks for a new buffer
*
* This function executes all the callbacks when a new buffer is
* started. Callbacks are executed in the order in which they were added. If any
* callback returns ERROR, this function returns ERROR immediately.
*
* RETURNS: OK, or ERROR as soon as a callback returns ERROR
*
* \NOMANUAL
*/

LOCAL STATUS rBuffNewBufferHookExecute
    (
    RBUFF_ID   rBuffId          /* buffer to use */
    )
    {
    int         n;
    for (n = 0; n < rBuffId->numCallbacks; n++)
        {
        if ((rBuffId->callbackList [n])((BUFFER_ID)rBuffId) != OK)
            return (ERROR);
        }
    return (OK);
    }


/*******************************************************************************
*
* rBuffCreate - create an extendable ring of buffers
*
* This routine creates an extendable ring of buffers, based on the supplied
* creation parameters.
*
* The creation parameters are an instance of a rBuffCreateParams structure. The
* structure contains the following members:
*
* \is
* \i 'sourcePartition'
* Memory partition from which the ring buffer memory will be allocated
* \i 'minimum'
* Minimum number of buffers in the ring. Must be greater than 2.
* \i 'maximum'
* Maximum number of buffers in the ring. Once the ring contains this many
* buffers, either logging will stop, or the buffer will wrap and overwrite old
* data.
* \i 'buffSize'
* Size, in bytes, of each buffer in the ring
* \i 'threshold'
* Point at which the rBuff manager task will unblock the System Viewer upload
* task, allowing the reader to take data from the buffer. Normally, the
* threshold is set to half the buffSize parameter, in bytes.
* \i 'errorHandler'
* Pointer to a function that will be called when the buffer becomes full
* \i 'options'
* Ring buffer options.
* \ie
*
* The following ring buffer options are available:
* \is
* \i 'RBUFF_WRAPAROUND'
* Buffer will wrap when full. If using this option, it is better not to perform
* any reading from the buffer until writing has stopped. This avoids the
* situation that the the writer hits the reader, and has to stop.
* \ie
*
* RETURNS: Pointer to newly-created buffer, or NULL
*
*/

BUFFER_ID rBuffCreate
    (
    void *Params        /* rBuff creation parameters */
    )
    {
    RBUFF_ID              	rBuff;
    UINT32                	count;
    RBUFF_PTR             	newBuff;
    rBuffCreateParamsType * 	rBuffCreateParams = Params;

    if ((!rBuffLibInstalled) && (rBuffLibInit() == OK))
	{
	rBuffLibInstalled = TRUE;
	}

    if (!rBuffLibInstalled)
        {
#ifdef RBUFF_DEBUG
        logMsg ("rBuff: rBuffLib not installed\n",0,0,0,0,0,0);
#endif

        return (NULL);
        }

    /* validate params */

    if (rBuffCreateParams->minimum < 2 ||
            ((rBuffCreateParams->minimum > rBuffCreateParams->maximum) ||
               (rBuffCreateParams->maximum < 2 &&
                rBuffCreateParams->maximum != RBUFF_MAX_AVAILABLE)) ||
            rBuffCreateParams->buffSize == 0)
        {
#ifdef RBUFF_DEBUG
        logMsg ("rBuff: invalid params\n",0,0,0,0,0,0);
#endif

        return (NULL);
        }

    /*
     *  Set source partition for object class memory allocation
     *  - note this only works overall if all rBuffs share a source
     *    partition.
     */

    /* allocate control structure */

    rBuff = memPartAlloc (rBuffCreateParams->sourcePartition,
			  sizeof (RBUFF_TYPE));

    if (rBuff == NULL)
        {
#ifdef RBUFF_DEBUG
        logMsg ("rBuff: objAlloc failed\n",0,0,0,0,0,0);
#endif

        return (NULL);
        }

    /* record parameters */

    rBuff->info.srcPart   = rBuffCreateParams->sourcePartition;
    rBuff->info.options   = (UINT32)rBuffCreateParams->options;
    rBuff->info.buffSize  = rBuffCreateParams->buffSize;
    rBuff->info.threshold = rBuffCreateParams->threshold;
    rBuff->info.minBuffs  = (UINT32)rBuffCreateParams->minimum;
    rBuff->info.maxBuffs  = (UINT32) rBuffCreateParams->maximum;
    rBuff->errorHandler   = rBuffCreateParams->errorHandler;

#ifdef _WRS_CONFIG_SMP
    rBuff->buffDesc.spinLock = &rBuffSpinlock;
#else
    rBuff->buffDesc.spinLock = NULL;
#endif /* _WRS_CONFIG_SMP */

    rBuff->buffDesc.lockRtn = rBuffLock;
    rBuff->buffDesc.unlockRtn = rBuffUnlock;

    rBuff->buffDesc.bufferTypeIdent = "rBuffLib 1.0";

    rBuff->numCallbacks = 0;
    rBuff->inCallback = 0;

    rBuff->buffDesc.newBuffHookAdd = rBuffNewBuffHookAdd;

    if (semBInit (&rBuff->buffDesc.threshXSem, SEM_Q_PRIORITY,
		  SEM_EMPTY) == ERROR)
        {
	memPartFree (rBuffCreateParams->sourcePartition, (char *) rBuff);

        return (NULL);
        }

    rBuff->rBuffMgrId = 0; /* ...so we can use rBuffDestroy safely */

    /*
     *  If things go wrong from here, use rBuffDestroy to throw back what
     *  we've caught.
     */

    /* allocate 'rBuffCreateParams->minimum' buffers */

    rBuff->info.currBuffs  =
    rBuff->info.emptyBuffs = 0;

    rBuff->buffWrite = NULL;

    /* initialize the handle */

    handleInit (&rBuff->buffDesc.handle, handleTypeRbuff);

    for (count=0; count < rBuffCreateParams->minimum; count++)
        {

        /* First we need a buffer */

        newBuff = rBuffElementCreate (rBuff);

        if (newBuff == NULL)
            {
#ifdef RBUFF_DEBUG
            logMsg ("rBuff: abandoned creation\n",0,0,0,0,0,0);
#endif
            rBuffDestroy ((BUFFER_ID) rBuff);

            return (NULL);
            }

        rBuffInsert (rBuff, newBuff);
        }

    /*
     * The first time around, spawn the rBuffMgr task. Once it exists,
     * don't create another one. It should never be deleted. This bit is
     * specific to System Viewer and will need attention for generic buffers
     */

    if (wvRBuffMgrId->tid == 0)
	{
    	wvRBuffMgrId->tid = taskSpawn (
    	    "tWvRBuffMgr",
    	    wvRBuffMgrId->priorityDefault,
            WV_RBUFF_MGR_OPTIONS,
            2048,
            wvRBuffMgr,
	    (_Vx_usr_arg_t)wvRBuffMgrId,
            0,0,0,0,0,0,0,0,0);
	}

    rBuff->rBuffMgrId =	wvRBuffMgrId;

    if (wvRBuffMgrId->tid == TASK_ID_ERROR)
        {
#ifdef RBUFF_DEBUG
        logMsg ("rBuff: error creating wvRBuffMgr\n",0,0,0,0,0,0);
#endif
        rBuffDestroy ((BUFFER_ID) rBuff);

        return (NULL);
        }

    /* set control pointers */

    rBuff->nestLevel = 0;

    rBuff->buffRead  = rBuff->buffWrite;

    rBuff->dataWrite =
    rBuff->dataRead  = rBuff->buffWrite->dataStart;

    /* reset info */

    rBuff->info.maxBuffsActual   = (UINT32)rBuffCreateParams->minimum;

    rBuff->info.dataContent      = 0;
    rBuff->info.writesSinceReset = 0;
    rBuff->info.readsSinceReset  = 0;
    rBuff->info.timesExtended    = 0;
    rBuff->info.timesXThreshold  = 0;
    rBuff->info.bytesWritten     = 0;
    rBuff->info.bytesRead        = 0;
    rBuff->info.bytesPeak        = 0;

    /* Reset msg passing mechanism */

    rBuff->msgOutstanding        = FALSE;

    /* now set up func ptrs allowing it to be called */

    rBuff->buffDesc.readReserveRtn = &rBuffReadReserve;
    rBuff->buffDesc.readCommitRtn  = &rBuffReadCommit;
    rBuff->buffDesc.writeRtn       = rBuffWrite;
    rBuff->buffDesc.flushRtn       = rBuffFlush;
    rBuff->buffDesc.threshold      = rBuffCreateParams->threshold;
    rBuff->buffDesc.nBytesRtn      = rBuffNBytes;

    /* made it! */

#ifdef RBUFF_DEBUG
    logMsg("Created rBuff with ID %p\n",(_Vx_usr_arg_t)rBuff,0,0,0,0,0);
#endif

    return ((BUFFER_ID) rBuff);
    }

/*******************************************************************************
*
* rBuffOverflowHandle - handle the case when the buffer becomes full
*
* This routine is invoked when data will not fit in the current buffer. On exit,
* the rBuff->buffWrite and rBuff->dataWrite pointers will have been updated, if
* it was possible to move to a new buffer
*
* RETURNS: OK, or ERROR
*
* \NOMANUAL
*/

LOCAL STATUS rBuffOverflowHandle
    (
    RBUFF_ID    rBuff
    )
    {

    /* skip the remainder of the current buffer */

    MARK_WRITE_BUFFER_FULL(rBuff);

    /* if about to use the last empty buffer, and still allowed to get more */

    if ((rBuff->info.emptyBuffs == 1) &&
        (rBuff->info.currBuffs < rBuff->info.maxBuffs))
        {
        RBUFF_SEND_MSG (RBUFF_MSG_ADD, MSG_PRI_URGENT, rBuff, 0);
        }
    else if (rBuff->buffWrite->next == rBuff->buffRead)
        {
        /* would like to write into read buffer - see if we can */
        if (rBuffRecycle(rBuff) == ERROR)
            {
            RBUFF_SEND_MSG (RBUFF_MSG_FULL, MSG_PRI_NORMAL, rBuff, 0);
            wvEvtBufferFullNotify = TRUE;
            return (ERROR);
            }

        /* we recycled an old buffer */
        rBuff->info.emptyBuffs++;
        }

    /* at this point, we are going to write. Move to next buffer */

    rBuff->buffWrite = rBuff->buffWrite->next;

    /* reset the new buffer */

    MARK_WRITE_BUFFER_EMPTY (rBuff);

    /* Set the write pointer to the start of the data */

    rBuff->dataWrite = rBuff->buffWrite->dataStart;

    return (OK);
    }

/*******************************************************************************
*
* rBuffWrite - write data to a ring of buffers
*
* This routine is used when writing data to the ring buffer. It allocates space
* in the buffer, and returns an address at which the caller may write its data
* as a contiguous block. The allocated space will be sufficient to hold the
* requested number of bytes. If it is not possible to satisfy the request, the
* function returns NULL.
*
* If the write causes the write pointer to move into the last buffer in the
* ring, and the number of buffers has not reached the configured maximum, then a
* request will be made to add a new buffer to the ring.
*
* If the write would cause the last buffer in the ring to become full, and the
* buffer was created with the RBUFF_WRAPAROUND option, then the write may be
* made to the first buffer in the ring, discarding the previous contents. It is
* not possible to reuse a buffer that is partially read: When using the
* RBUFF_WRAPAROUND option, it is usual not to read from the ring buffer until
* writing has stopped. This avoids the situation where the writer has to stop
* because it has caught up with the reader in a partially-read buffer.
*
* This function may be called from interrupt level.
*
* Mutually exclusive access must be guaranteed by the caller.
*
* RETURNS: Pointer to destination, or NULL
*
*/

UINT8* rBuffWrite
    (
    BUFFER_ID buffId,		/* ring buffer id */
    UINT8 *   pDummy,		/* not used */
    size_t    numOfBytes	/* number of bytes to reserve */
    )
    {
    RBUFF_ID    rBuff;	            /* access this particular rBuff */
    UINT8 *     returnPtr = NULL;   /* Return a non-zero value if OK */

    /* Get access to the private members of this buffer's descriptor. */

    rBuff = (RBUFF_ID) buffId;

    /* Get out early if this request makes no sense. */

    if (numOfBytes > rBuff->info.buffSize)
        {
        return (NULL);
        }

    /* Note whether crossing the upload threshold and allow upload to run */

    if ((rBuff->buffWrite->dataLen < rBuff->info.threshold) &&
        (rBuff->buffWrite->dataLen + numOfBytes >= rBuff->info.threshold))
        if (!(rBuff->info.options & RBUFF_UP_DEFERRED))
            {
            /*
             *  Signal for uploading to begin. Note that if we have just
             *  reserved space it is imperative that uploading does not
             *  actually begin until the data is in the buffer.
             */

            rBuff->info.timesXThreshold++;

            RBUFF_SEND_MSG (RBUFF_MSG_THRESHX, MSG_PRI_NORMAL, rBuff, 0);
            }


    if (rBuff->buffWrite->spaceAvail < numOfBytes)
        {
        if (rBuffOverflowHandle (rBuff) != OK)
            return (NULL);
        }

    /* Record the start of the reserved area */

    returnPtr = rBuff->dataWrite;

    if (rBuff->buffWrite->dataLen == 0 && !rBuff->inCallback)
        {
        /*
         * first event data in buffer, allow client to add data to
         * start of buffer
         */

        rBuff->inCallback = 1;

        if (rBuffNewBufferHookExecute (rBuff) != OK)
            return (NULL);

        rBuff->inCallback = 0;

        rBuff->info.emptyBuffs--;

        returnPtr = rBuff->dataWrite;
        }

    rBuff->buffWrite->dataLen    += numOfBytes;
    rBuff->dataWrite             += numOfBytes;
    rBuff->buffWrite->spaceAvail -= numOfBytes;

    rBuff->info.dataContent  += numOfBytes;

    /* update info */

    if(rBuff->info.dataContent > rBuff->info.bytesPeak)
        {
        rBuff->info.bytesPeak = rBuff->info.dataContent;
        }

    rBuff->info.bytesWritten += numOfBytes;

    rBuff->info.writesSinceReset++;

    return (returnPtr);
    }

/*******************************************************************************
*
* rBuffRead - read data from a ring of buffers
*
* This routine reads data from an extendable ring of buffers.
*
* This function assumes mutually exclusive access is guaranteed by the caller.
*
* \NOMANUAL
*/

size_t rBuffRead
    (
    BUFFER_ID 	buffId,		/* generic buffer descriptor */
    UINT8 *   	dataDest,
    size_t  	numOfBytes
    )
    {
    size_t 	bytesToCopy;
    size_t	remaining = numOfBytes;
    RBUFF_ID 	rBuff;		/* access private members of this rBuff desc */

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    /* Critical Region Start */

    while (remaining > 0)
        {
        bytesToCopy = (rBuff->buffRead->dataLen < remaining ?
                        rBuff->buffRead->dataLen : remaining);

        if (bytesToCopy > 0)
            {
            memcpy (dataDest,rBuff->dataRead,bytesToCopy);

            remaining                -= bytesToCopy;
            dataDest                 += bytesToCopy;

            /* Update buffer */

            rBuff->buffRead->dataLen -= bytesToCopy;

            rBuff->dataRead          += bytesToCopy;
            rBuff->info.dataContent  -= bytesToCopy;
            }

        if (rBuff->buffRead->dataLen == 0)
            {
            /* This buffer is now empty */

            rBuffHandleEmpty (rBuff);
            }
        else
            {
            /* this buffer is not yet emptied */

            rBuff->dataRead += bytesToCopy;
            }
        }

    /* update info */

    rBuff->info.bytesRead += (numOfBytes - remaining);

    rBuff->info.readsSinceReset++;

    /* Critical Region End */

    return (numOfBytes - remaining);
    }

/*******************************************************************************
*
* rBuffReadReserve - return the number of bytes available for reading
*
* This function indicates how many contiguous bytes are available to be read
* from a block in the ring buffer, and indicates the address from which those
* bytes may be read. Once this function has been called, the data can be copied
* from the ring buffer. rBuffReadCommit() should then be called to indicate that
* the data has been read, and can be discarded from the buffer.
*
* This function assumes mutually exclusive access is guaranteed by the caller.
*
*
* RETURNS: Number of bytes available to read, and updates the src parameter to
* contain the address from which to read
*
* SEE ALSO: rBuffReadCommit()
*
* \INTERNAL
*
* If the buffer has some data to read, set the uncommited read flag, so the
* writer will not delete it from under the reader
*
*/

ssize_t rBuffReadReserve
    (
    BUFFER_ID 	buffId,		/* generic identifier for this buffer */
    UINT8 **	src        	/* address of pointer to buffer data */
    )
    {
    RBUFF_ID 	rBuff;		/* specific identifier for this rBuff */
    size_t	bytesAvailable;

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    /* Generate and return the available contiguous bytes. */

    rBuffLock (buffId);

    bytesAvailable = rBuff->buffRead->dataLen;

    if (bytesAvailable != 0)
        {
        *src = rBuff->dataRead;
        rBuff->buffRead->uncommitedRead = TRUE;
        }
    else
        {
        *src = NULL;
        }

    rBuffUnlock (buffId);

    return (bytesAvailable);
    }


/*******************************************************************************
*
* rBuffReadCommit - indicate that read of requested data is complete
*
* This function is called to indicate that data requested from the ring with
* rBuffReadReserve() has been processed. The data is removed from the buffer.
*
* This function assumes mutually exclusive access is guaranteed by the caller.
*
* SEE ALSO: rBuffReadReserve()
*
* RETURNS: OK, or ERROR if too many bytes requested.
*
* \INTERNAL
*
* Calling this function indicates that the data, requested by the call to
* rBuffReadReserve(), has been processed, and so it is safe to clear the
* uncommited read flag.
*
*/

STATUS rBuffReadCommit
    (
    BUFFER_ID buffId,		/* generic identifier for this buffer */
    size_t  numOfBytes          /* number of bytes used */
    )
    {
    RBUFF_ID    rBuff;		/* specific identifier for this rBuff */

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    /* Critical Region Start */

    rBuffLock (buffId);

    rBuff->buffRead->uncommitedRead = FALSE;

    if (numOfBytes == 0)
        {
        rBuffUnlock (buffId);
        return (OK);
        }

    if (numOfBytes == rBuff->buffRead->dataLen)
        {
        /* handle the empty buffer */

        rBuffHandleEmpty (rBuff);
        }
    else if (numOfBytes < rBuff->buffRead->dataLen)
        {
        rBuff->buffRead->dataLen -= numOfBytes;
        rBuff->dataRead += numOfBytes;
        }
    else
        {
        /* Moving ahead through multiple buffers */

        /* Not yet supported */
        rBuffUnlock (buffId);
        return(ERROR);
        }

    /* update info */

    rBuff->info.dataContent -= numOfBytes;

    rBuff->info.bytesRead += numOfBytes;

    rBuff->info.readsSinceReset++;

    rBuffUnlock (buffId);

    /* Critical Region End */

    return (OK);
    }


/*******************************************************************************
*
* rBuffFlush - Flush data from a ring of buffers
*
* This routine causes any data held in a buffer to be uploaded and is
* used to clear data that falls below the specified threshold.
*
* \NOMANUAL
*/

STATUS rBuffFlush
    (
    BUFFER_ID buffId		/* generic identifier for this buffer */
    )
    {
    RBUFF_ID rBuff;		/* specific identifier for this rBuff */
    int 	result;
    size_t	originalThreshold;

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    result = OK;

    if (!rBuff->info.dataContent)
        {
        return(OK);
        }

    /* Store the original threshold */

    originalThreshold = rBuff->info.threshold;

    rBuff->info.threshold = 0;

    while (rBuff->info.dataContent > 0 && result != ERROR)
        {
        if (semGive (&rBuff->buffDesc.threshXSem) == OK)
            {
            /* Cause a reschedule to allow uploader in */

            taskDelay(0);
            }
        else
            {
            result = ERROR;
            }
        }

    rBuff->info.threshold = originalThreshold;

    return(result);
}


/*******************************************************************************
*
* rBuffReset - reset an extendable ring of buffers
*
* This routine resets a ring of buffers to its initial state. This loses any
* data held in the buffer. The number of buffers specified by the minimum
* parameter are retained: The others are removed from the ring and their memory
* deallocated.
*
* RETURNS: OK or ERROR
*/

STATUS rBuffReset
    (
    BUFFER_ID buffId		/* generic identifier for this buffer */
    )
    {
    RBUFF_ID rBuff;		/* specific identifier for this rBuff */
    RBUFF_LOCK (rBuff);

    if (buffId == NULL)
        return (ERROR);

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    /* Now reset the ring of buffers. */

    if (rBuff->info.currBuffs > rBuff->info.minBuffs)
        {
        UINT32          excessBuffs, count;
        RBUFF_PTR       newHome;
        RBUFF_PTR       lastBuff;

        /* drop the excess buffers */

        excessBuffs = rBuff->info.currBuffs - rBuff->info.minBuffs;

        for (count = 0; count < excessBuffs; count++)
            {
            RBUFF_PTR tempPtr;

            tempPtr = rBuff->buffWrite->next; /* should not be NULL! */
            rBuffFree (rBuff,rBuff->buffWrite);

            rBuff->buffWrite = tempPtr;
            }

        /*
         * Now we're pointing to the first buff that will be left
         * Go through to the last one, and set its next pointer to the one we're
         * at now
         */

        newHome = rBuff->buffWrite;
        lastBuff = newHome;

        /* Step to the last one, not past it */

        for (count=0; count < rBuff->info.minBuffs - 1; count++)
            lastBuff = lastBuff->next;

        lastBuff->next = newHome;
        }


    /* Make sure the 'read' buffer pointer points somewhere sensible */

    rBuff->buffRead = rBuff->buffWrite;

    /* and reset the byte ptrs */

    rBuff->dataRead         =
    rBuff->dataWrite        = rBuff->buffWrite->dataStart;

    rBuff->info.currBuffs        =
    rBuff->info.maxBuffsActual   = rBuff->info.minBuffs;

    /* now traverse the ring resetting the individual buffers */

    {
    RBUFF_PTR backHome = rBuff->buffWrite;
    RBUFF_PTR currBuff = rBuff->buffWrite;

    do
     	{
        currBuff->dataLen    = 0;
        currBuff->spaceAvail = rBuff->info.buffSize;
        currBuff->uncommitedRead = FALSE;

        currBuff = currBuff->next;
        }
    while (currBuff != backHome);

    }

    /* Reset msg passing mechanism */

    rBuff->msgOutstanding        = FALSE;

    /*    rBuff->msgWriteIndex         =
          rBuff->msgReadIndex          = 0; */

    rBuff->nestLevel = 0;

    /* reset info */

    rBuff->info.emptyBuffs       = rBuff->info.minBuffs;
    rBuff->info.dataContent      =
    rBuff->info.writesSinceReset =
    rBuff->info.readsSinceReset  =
    rBuff->info.timesExtended    =
    rBuff->info.timesXThreshold  = 0;
    rBuff->info.bytesWritten     = 0ULL;
    rBuff->info.bytesRead        = 0;

    RBUFF_UNLOCK (rBuff);

    return (OK);
}


/*******************************************************************************
*
* rBuffNBytes - Returns the total number of bytes held in a ring of buffers
*
* \NOMANUAL
*/

ssize_t rBuffNBytes
    (
    BUFFER_ID buffId		/* generic identifier for this buffer */
    )
    {
    RBUFF_ID rBuff;		/* specific identifier for this rBuff */

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    return (rBuff->info.dataContent);
    }


/*******************************************************************************
*
* rBuffLock - gain exclusive access to the ring buffer control block
*
* \NOMANUAL
*/

LOCAL void rBuffLock
    (
    BUFFER_ID   buffId
    )
    {
#ifdef _WRS_CONFIG_SMP
#ifdef _WRS_CONFIG_DETERMINISTIC
    SPIN_LOCK_ISR_TAKE (buffId->spinLock);
#else
    rBuffSpinlockKey = spinLockIsrNdTake (buffId->spinLock);
#endif /* _WRS_CONFIG_DETERMINISTIC */
#else
    lockLevel = intLock ();
#endif /* _WRS_CONFIG_SMP */
    }

/*******************************************************************************
*
* rBuffUnlock - release exclusive access to the ring buffer control block
*
* \NOMANUAL
*/

LOCAL void rBuffUnlock
    (
    BUFFER_ID   buffId
    )
    {
#ifdef _WRS_CONFIG_SMP
#ifdef _WRS_CONFIG_DETERMINISTIC
    SPIN_LOCK_ISR_GIVE (buffId->spinLock);
#else
    spinLockIsrNdGive (buffId->spinLock, rBuffSpinlockKey);
#endif /* _WRS_CONFIG_DETERMINISTIC */
#else
    intUnlock (lockLevel);
#endif
    }

/*******************************************************************************
*
* rBuffMsgQueueLock - gain exclusive access to the rBuff message queue
*
* \NOMANUAL
*/

LOCAL void rBuffMsgQueueLock (void)
    {
#ifdef _WRS_CONFIG_SMP
#ifdef _WRS_CONFIG_DETERMINISTIC
    SPIN_LOCK_ISR_TAKE (&rBuffSpinlock);
#else
    rBuffSpinlockKey = spinLockIsrNdTake (&rBuffSpinlock);
#endif /* _WRS_CONFIG_DETERMINISTIC */
#else
    lockLevel = intLock ();
#endif /* _WRS_CONFIG_SMP */
    }

/*******************************************************************************
*
* rBuffMsgQueueUnlock - release exclusive access to the rBuff message queue
*
* \NOMANUAL
*/

LOCAL void rBuffMsgQueueUnlock (void)
    {
#ifdef _WRS_CONFIG_SMP
#ifdef _WRS_CONFIG_DETERMINISTIC
    SPIN_LOCK_ISR_GIVE (&rBuffSpinlock);
#else
   spinLockIsrNdGive (&rBuffSpinlock, rBuffSpinlockKey);
#endif /* _WRS_CONFIG_DETERMINISTIC */
#else
    intUnlock (lockLevel);
#endif /* _WRS_CONFIG_SMP */
    }

/*******************************************************************************
*
* rBuffCreate - Create and initialize a new buffer
*
* This routine creates a buffer ready for insertion into the ring. It will
* initialize the buffer contents, but not update the statistics
*
* The function must not be called while the ring buffer is locked
*
* RETURNS: A pointer to an initialized ring buffer element, or NULL
* \NOMANUAL
*/

LOCAL RBUFF_PTR rBuffElementCreate
    (
    RBUFF_ID    rBuff
    )
    {
    RBUFF_PTR   pBuff;

    pBuff = memPartAlloc (rBuff->info.srcPart,
                          sizeof(RBUFF_BUFF_TYPE) + rBuff->info.buffSize);
    if (pBuff != NULL)
        {
        pBuff->dataStart  = (UINT8 *) pBuff + sizeof(RBUFF_BUFF_TYPE);
        pBuff->dataEnd    = pBuff->dataStart + rBuff->info.buffSize;
        pBuff->spaceAvail = rBuff->info.buffSize;
        pBuff->dataLen    = 0;
        pBuff->uncommitedRead = FALSE;
        }

    return (pBuff);
    }

/*******************************************************************************
*
* rBuffInsert - insert a new buffer into an extendable ring of buffers
*
* This routine inserts a buffer to an already created or partially created
* extendable ring of buffers
*
* This function assumes mutually exclusive access is guaranteed by the caller.
*
* \NOMANUAL
*/

LOCAL STATUS rBuffInsert
    (
    RBUFF_ID 	rBuff,
    RBUFF_PTR 	newBuff
    )
    {

    /* Now link the buffer into the ring after the 'write' buffer */

    if (rBuff->buffWrite != NULL)
        {
        RBUFF_PTR tempPtr;

        tempPtr = rBuff->buffWrite->next;

        rBuff->buffWrite->next = newBuff;

        /* close the ring */

        newBuff->next = tempPtr;
        newBuff->prev = rBuff->buffWrite;
        newBuff->next->prev = newBuff;
        }
    else
        {

        /* ring is empty, must be creation time */

        rBuff->buffWrite =
        rBuff->buffRead  =
        newBuff->prev =
        newBuff->next    = newBuff;
        }

    /* Maintain statistics */

    if (++(rBuff->info.currBuffs) > rBuff->info.maxBuffsActual)
        {
        rBuff->info.maxBuffsActual++;
        }

    rBuff->info.timesExtended++;
    rBuff->info.emptyBuffs++;

    return (OK);
    }

/*******************************************************************************
*
* rBuffRecycle - Try to reuse the first buffer in the ring
*
* This function is called when the write to the last available buffer fails due
* to lack of space, and the next buffer to write to is the read buffer. There is
* no new buffer, and it is not possible to add one.
*
* It moves the read buffer pointer on, but doesn't clear out the write buffer
*
* RETURNS: OK or ERROR
*
* \NOMANUAL
*/

LOCAL STATUS rBuffRecycle
    (
    RBUFF_ID 	rBuff
    )
    {
    RBUFF_PTR   pReadBuff;

    /* Check whether wrapping is allowed */

    if ((rBuff->info.options & RBUFF_WRAPAROUND) == 0)
        return (ERROR);

    /*
     * We may be able to reuse the read buffer, if nothing has been read from
     * it. We do not support reuse for partially-read buffers
     */

    pReadBuff = rBuff->buffRead;

    /* can only reuse the buffer if it's not being read */

    if ((pReadBuff->uncommitedRead != FALSE) ||
        (rBuff->dataRead != pReadBuff->dataStart))
        {
        return (ERROR);
        }

    /* update buffer statistics - we're about to wipe the read buffer data */

    rBuff->info.dataContent -= rBuff->buffRead->dataLen;

    /* move the reader on, the caller will update the writer */

    rBuff->buffRead = rBuff->buffRead->next;
    rBuff->dataRead = rBuff->buffRead->dataStart;

    return (OK);
    }


/*******************************************************************************
*
* rBuffFree - Free a specified buffer
*
* This function unlinks the supplied buffere element from the ring, and then
* posts a request to free the memory. The function will be called with a
* spinlock held, so the buffer pointers can be updated.
*
* This function assumes the buffer is locked, and that the supplied buffer
* pointer is valid
*
* RETURNS: OK or ERROR if the requested buffer was not found
*
* \NOMANUAL
*/

LOCAL STATUS rBuffFree
    (
    RBUFF_ID 	rBuff,          /* enclosing rBuff */
    RBUFF_PTR 	buffToFree      /* ring buffer element to free */
    )
    {

    if (buffToFree == NULL)
        {
        return(ERROR);
        }

    /* unlink the buffer from the ring */

    buffToFree->prev->next = buffToFree->next;
    buffToFree->next->prev = buffToFree->prev;

    /* Schedule the rBuff Mgr to actually free the buffer */

    RBUFF_SEND_MSG (RBUFF_MSG_FREE, MSG_PRI_NORMAL, rBuff, buffToFree);

    /* memPartFree (rBuff->info.srcPart,(char *)buffToFree); */

    /* Maintain statistics */

    rBuff->info.emptyBuffs--;
    rBuff->info.currBuffs--;

    return (OK);
    }


/*******************************************************************************
*
* rBuffDestroy - destroy an extendable ring of buffers
*
* This routine destroys an already created or partially created extendable ring
* of buffers. This loses any data held in the buffer. All resources held are
* returned to the system.
*
* RETURNES: OK or ERROR
*/

STATUS rBuffDestroy
    (
    BUFFER_ID 	buffId		/* generic identifier for this buffer */
    )
    {
    RBUFF_ID 	rBuff;		/* specific identifier for this rBuff */

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    /* prevent any clients writing */

    RBUFF_LOCK(buffId);
    rBuff->info.buffSize = 0;
    RBUFF_UNLOCK(buffId);

#if 0
    /* validate rBuff ID */

    if (HANDLE_VERIFY (&rBuff->buffDesc.handle, handleTypeRbuff) != OK)
	{
	return (ERROR);
	}
#endif

    /* invalidate this object */

    handleTerminate (&rBuff->buffDesc.handle);

    /* Break the ring and traverse the list, freeing off   */
    /* the buffers. buffWrite is used as the start point   */
    /* this works if the ring creation is abandoned early. */

    if (rBuff->buffWrite != NULL)
        {
        while (rBuff->buffWrite->next != rBuff->buffWrite)
            {
            RBUFF_PTR   delPtr;

            delPtr = rBuff->buffWrite->next;
            rBuff->buffWrite->next = rBuff->buffWrite->next->next;

            memPartFree (rBuff->info.srcPart, (char *)delPtr);
            }

        memPartFree (rBuff->info.srcPart, (char *)rBuff->buffWrite);
        }
    else
        {
        /* ring hasn't been created so don't worry about it */
        }

    /* now free off the control structure */

    semTerminate (&rBuff->buffDesc.threshXSem);
    memPartFree (rBuff->info.srcPart, (char *) rBuff);

    /* we're done */

    return (OK);
    }


/*******************************************************************************
*
* rBuffSetFd - Set a new fd as the data upload destination
*
*
* \NOMANUAL
*/

STATUS rBuffSetFd
    (
    BUFFER_ID 	buffId,		/* generic identifier for this buffer */
    int 	fd
    )
    {
    RBUFF_ID 	rBuff;		/* specific identifier for this rBuff */

    RBUFF_LOCK (rBuff);

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;
    rBuff->fd = fd;
    RBUFF_UNLOCK (rBuff);

    return (OK);
    }


/*******************************************************************************
*
* rBuffHandleEmpty - handle case when reading a buffer makes empty
*
* This function is called when a read from a buffer causes it to become
* empty.
* If the buffer is both read and write buffer, then the ring is now empty. In
* that case, we reset the read and write pointers to the start of the buffer, to
* delay any future buffer overflow.
*
* If the buffer we are in is not the write buffer, then move the read pointer on
* to the start of next buffer. Increment the empty buffer count.
*
* This function assumes mutually exclusive access is guaranteed by the
* caller. It only unlinks the buffer if doing so would not reduce the number of
* buffers below the minimum.
*
* The buffer count is updated
*
* \NOMANUAL
*/

LOCAL STATUS rBuffHandleEmpty
    (
    RBUFF_ID    rBuff
    )
    {

    /* this is the buffer that has become empty */

    RBUFF_PTR   pBuffToFree = rBuff->buffRead;

    if (rBuff->buffRead != rBuff->buffWrite)
        {

        /*
         * reader has not caught up with the writer, so move the reader to the
         * next buffer
         */

        rBuff->buffRead = rBuff->buffRead->next;
        }
    else
        {
        /*
         * read and write buffers are the same. Reuse the buffer by resetting
         * the write pointers (the read pointer is done below ). The ring has
         * become empty.
         */

        MARK_READ_BUFFER_EMPTY (rBuff);

        rBuff->dataWrite = rBuff->buffWrite->dataStart;

        /*
         * if we are to free a buffer, don't free the current one (there are
         * always at least two, until the buffer is deleted
         */

        pBuffToFree = rBuff->buffWrite->next;
        }

    /* position read ptr to start of buffer */

    rBuff->dataRead = rBuff->buffRead->dataStart;

    rBuff->info.emptyBuffs++;

    /* if there are too many empty buffers, release one */

    if (rBuff->info.emptyBuffs > RBUFF_EMPTY_KEEP &&
        rBuff->info.currBuffs > rBuff->info.minBuffs )
        {

        /* otherwise, adjust buffer counts */

        rBuff->info.currBuffs--;
        rBuff->info.emptyBuffs--;

        /* unlink the buffer */

        pBuffToFree->next->prev = pBuffToFree->prev;
        pBuffToFree->prev->next = pBuffToFree->next;

        RBUFF_SEND_MSG (RBUFF_MSG_FREE, MSG_PRI_NORMAL, rBuff, pBuffToFree);
        }

    return (OK);
    }


/*******************************************************************************
*
* wvRBuffMgr - The function is spawned as a task to manage WV rBuff extension.
*
* It waits for a msg which carries the ID of a buffer which has had a change
* of the number of empty buffers held. If there are less the requested to be
* reserved than an attempt will be made to make more available. If there are
* more empty buffers than required than some will be freed.
*
* For each msg received, the task will achieve the required number of free
* buffers. This may result in redundant msgs being received.
*
* This manager is specific to Wind River System Viewer
*
* \NOMANUAL
*/

int wvRBuffMgr
    (
    WV_RBUFF_MGR_TYPE * rBuffMgrId  /* id of the rBuffMgr structure */
    )
    {
    RBUFF_PTR    	    	newBuff;
    WV_RBUFF_MGR_MSG_TYPE 	msg;
    BOOL                        msgPresent;

#ifdef RBUFF_DEBUG
    logMsg ("rBuffMgr %p created\n",(_Vx_usr_arg_t)rBuffMgrId,0,0,0,0,0);
#endif

    for(;;)
        {
        msgPresent = FALSE;

        /* take the spinlock, get a request from the queue */

        rBuffMsgQueueLock ();

        if (rBuffMgrId->msgReadIndex != rBuffMgrId->msgWriteIndex)
            {
            /* take one message from the queue */
            msgPresent = TRUE;
            memcpy ((void*)&msg, (const void *)&rBuffMgrId->msg[rBuffMgrId->msgReadIndex],
                    sizeof (msg));

            if (++rBuffMgrId->msgReadIndex >= WV_RBUFF_MGR_MSGQ_MAX)
                {
                rBuffMgrId->msgReadIndex = 0;
                }
            }

        rBuffMsgQueueUnlock ();

        /* if no messages, wait a bit, otherwise process the message */

        if (!msgPresent)
            {
            taskDelay (rBuffPollingInterval);
            }
        else
            {
#ifdef RBUFF_DEBUG
            logMsg ("rBuffMgr: (%d) rBuff %p type 0x%x arg 0x%x\n",
                    (_Vx_usr_arg_t)rBuffMgrId->msgReadIndex,
                    (_Vx_usr_arg_t)msg.ringId, 
                    (_Vx_usr_arg_t)msg.msgType, 
                    (_Vx_usr_arg_t)msg.arg, 
                    0, 0);
#endif

            /* Determine what needs to be done */

            switch (msg.msgType)
                {
                case RBUFF_MSG_ADD :
                    {
                    /*
                     *  This message may be generated one or more times by
                     *  event logging, and the condition that caused the msg
                     *  may no longer be TRUE, so just take this as a prompt
                     *  to see if the ring needs extending.
                     */

                    /*
                     * The ring needs extending if the current write buffer data
                     * content has crossed the thresholdd, and the number of
                     * free buffers is less than or equal to the minimum
                     */

                    /* Make sure that we don't request extension recursively */

                    msg.ringId->nestLevel++;

                    newBuff = rBuffElementCreate (msg.ringId);

                    /* of course, newBuff may be NULL... */

#ifdef RBUFF_DEBUG
                    logMsg ("rBuffMgr: adding buffer %p\n",
                	    (_Vx_usr_arg_t)newBuff, 0,0,0,0,0);
#endif
                    if (newBuff != NULL)
                        {
                        rBuffLock ((BUFFER_DESC *)msg.ringId);
                        rBuffInsert (msg.ringId, newBuff);
                        rBuffUnlock ((BUFFER_DESC *)msg.ringId);
                        }
                    }

                    msg.ringId->nestLevel--;

                    break;

                case RBUFF_MSG_FREE :
                    {
                    /* This message carries a ptr to the buffer to be freed */

#ifdef RBUFF_DEBUG
                    logMsg ("rBuffMgr: freeing buffer %p\n", 
                	    (_Vx_usr_arg_t) msg.arg, 0,0,0,0,0);
#endif
                    memPartFree (msg.ringId->info.srcPart, (char *)msg.arg);
                    }
                    break;

                case RBUFF_MSG_FULL :
                    logMsg ("Ring Buffer %p full. Stopped event logging.\n",
                            (_Vx_usr_arg_t) msg.ringId,0,0,0,0,0);

                    if (msg.ringId->errorHandler != NULL)
                        (msg.ringId->errorHandler)(RBUFF_ERR_FULL);

                    break;

                case RBUFF_MSG_THRESHX :
                    semGive (&msg.ringId->buffDesc.threshXSem);
                    break;

                default :
                    logMsg ("Unknown message type %d\n", 
                	    (_Vx_usr_arg_t) msg.msgType, 0,0,0,0,0);
                    break;
                }

            msg.ringId->msgOutstanding = FALSE;
            }
        }
    }

/*******************************************************************************
*
* wvRBuffMgrPrioritySet - set the priority of the System Viewer rBuff manager
*
* This routine changes the priority of the `tWvRBuffMgr' task to the value of
* <priority>.  If the task is not yet running, this priority is
* used when it is spawned.
*
* RETURNS: OK, or ERROR if the priority can not be set.
*
* SEE ALSO: taskPrioritySet(), the VxWorks programmer guides.
*/

STATUS wvRBuffMgrPrioritySet
    (
    int	     priority           /* new priority */
    )
    {

    if (!rBuffLibInstalled)
        {
    	if (rBuffLibInit() == OK)
	    rBuffLibInstalled = TRUE;
    	else
            return (ERROR);
	}

    if (wvRBuffMgrId->tid != 0)
 	if (taskPrioritySet (wvRBuffMgrId->tid, priority) != OK)
	    return (ERROR);

    wvRBuffMgrId->priorityDefault = priority;
    return (OK);
    }

/*******************************************************************************
*
* rBuffUpload - Copy data that is added to the buffer to the specified 'fd'
*
* \NOMANUAL
*/

STATUS rBuffUpload
    (
    BUFFER_ID 	buffId,		/* generic identifier for this buffer */
    int 	fd
    )
    {
    RBUFF_ID 	rBuff;		/* specific identifier for this rBuff */
    UINT8 *	src;
    STATUS 	result = OK;

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    rBuff->fd = fd;

    while (result == OK)
        {

        /* Wait for the dinner gong */

        logMsg ("Waiting for data\n",0,0,0,0,0,0);

        semTake (&rBuff->buffDesc.threshXSem,WAIT_FOREVER);

        logMsg("Uploading...\n",0,0,0,0,0,0);

        /* tuck in... */

        while (result == OK && rBuff->info.dataContent >
              (rBuff->info.threshold / 4))
            {
            size_t bytesToWrite = rBuffReadReserve(buffId, &src);
            ssize_t bytesWritten;

            if (bytesToWrite > 4096)
                {
                bytesToWrite = 4096;
                }

#ifdef RBUFF_DEBUG

            logMsg ("%#x bytes from %p (dataContent=%#x)\n",
                (_Vx_usr_arg_t) bytesToWrite,
                (_Vx_usr_arg_t) src,
                (_Vx_usr_arg_t) rBuff->info.dataContent,0,0,0);
#endif


            /* move the data */

            if ((bytesWritten =
                write (fd, (char *)src, bytesToWrite)) == -1)
                {
                logMsg ("Upload error!\n",0,0,0,0,0,0);

                if (rBuff->errorHandler)
                    {

                    /* Dial 999 / 911 */

                    (*rBuff->errorHandler)(RBUFF_ERR_UPLOAD);
                    }

                result = ERROR;
                }
            else
                {
                /* move the buffer pointers along */

                result = rBuffReadCommit (buffId,bytesWritten);

                taskDelay (0);
                }
            }
        }
    return (result);
    }

#else  /* GENERIC_RBUFF */

/*
 * The code below constitutes the generic ring buffer scheme on which
 * WindView 2.0 was originally based. If there is a need to go back to
 * it, #define GENERIC_RBUFF at the top of this file or build with
 * EXTRA_DEFINE="-DGENERIC_RBUFF" in the make command line
 */

#include "vxWorks.h"
#include "semLib.h"
#include "classLib.h"
#include "errno.h"
#include "intLib.h"
#include "logLib.h"
#include "taskLib.h"
#include "fioLib.h"
#include "rBuffLib.h"


#include "private/wvBufferP.h"
#include "private/classLibP.h"
#include "private/handleLibP.h"
#include "private/workQLibP.h"

#include "stdio.h"
#include "string.h"
#include "unistd.h"

extern int rBuffMgr();

/* locals */

LOCAL STATUS rBuffAdd
    (
    RBUFF_ID  rBuff,
    RBUFF_PTR buffToAdd
    );

LOCAL STATUS  rBuffFree
    (
    RBUFF_ID  rBuff,
    RBUFF_PTR buffToFree
    );

LOCAL STATUS rBuffHandleEmpty
    (
    RBUFF_ID rBuff
    );


#define RBUFF_SEND_MSG(RMSG_TYPE,RMSG_PRI,RMSG_ID,RMSG_PARAM) \
if((!(RMSG_ID)->nestLevel++) && !(RMSG_ID)->msgOutstanding)   \
{                                                             \
    (RMSG_ID)->msgOutstanding = TRUE;                         \
    (RMSG_ID)->msg[(RMSG_ID)->msgWriteIndex][0] = RMSG_TYPE;  \
    (RMSG_ID)->msg[(RMSG_ID)->msgWriteIndex][1] = (UINT32) RMSG_PARAM; \
                                                              \
    if(++(RMSG_ID)->msgWriteIndex > RBUFF_MAX_MSGS)           \
    {                                                         \
        (RMSG_ID)->msgWriteIndex = 0;                         \
    }                                                         \
                                                              \
    if(KERNEL_ENTERED_ME ())                                  \
    {                                                         \
        WQADD1((FUNCPTR)semCGiveDefer, (int)&(RMSG_ID)->msgSem); \
    }                                                         \
    else                                                      \
    {                                                         \
        semCGiveDefer(&(RMSG_ID)->msgSem);                    \
    }                                                         \
}                                                             \
                                                              \
(RMSG_ID)->nestLevel--;

LOCAL BOOL   rBuffLibInstalled = FALSE;   /* protect from multiple inits */

/* global variables */


/*******************************************************************************
*
* rBuffLibInit - initialize the ring of buffers library
*
* This routine initialises the ring of buffers library. If INCLUDE_RBUFF is
* defined in configAll.h, it is called by the root task, usrRoot(), in
* usrConfig.c.
*
* \NOMANUAL
*
* RETURNS: OK, or ERROR if the library could not be initialized.
*/

STATUS rBuffLibInit (void)
    {

    if(rBuffLibInstalled) {
        return(TRUE);
    }

    rBuffLibInstalled = TRUE;

    /* Initialise the rBuff Manager */

    return (OK);
    }


/*******************************************************************************
*
* rBuffCreate - create an extendable ring of buffers
*
* This routine creates an extendable ring of buffers from
* the specified partition.
*
* \NOMANUAL
*/

BUFFER_ID rBuffCreate
    (
    void *Params
    )
    {

    RBUFF_ID              rBuff;
    UINT32                count;
    RBUFF_PTR             newBuff;
    rBuffCreateParamsType *rBuffCreateParams = Params;

    if ((!rBuffLibInstalled) && (rBuffLibInit()) == OK)
	{
	rBuffLibInstalled = TRUE;
	}

    if(!rBuffLibInstalled)
        {
#ifdef RBUFF_DEBUG
        logMsg ("rBuff: rBuffLib not installed\n",0,0,0,0,0,0);
#endif

        return(NULL);
        }

    /* validate params */

    if (rBuffCreateParams->minimum < 2 ||
            ((rBuffCreateParams->minimum > rBuffCreateParams->maximum) ||
               (rBuffCreateParams->maximum < 2 &&
                rBuffCreateParams->maximum != RBUFF_MAX_AVAILABLE)) ||
            rBuffCreateParams->buffSize == 0)
        {
#ifdef RBUFF_DEBUG
        logMsg ("rBuff: invalid params\n",0,0,0,0,0,0);
#endif

        return(NULL);
        }

    /*
     *  Set source partition for object class memory allocation
     *  - note this only works overall if all rBuffs share a source
     *    partition.
     */

    /* allocate control structure */

    rBuff = memPartAlloc (rBuffCreateParams->sourcePartition,
			  sizeof (RBUFF_TYPE));

    if(rBuff == NULL)
        {
#ifdef RBUFF_DEBUG
        logMsg ("rBuff: objAlloc failed\n",0,0,0,0,0,0);
#endif

        return(NULL);
        }

    /* record parameters */

    rBuff->info.srcPart   = rBuffCreateParams->sourcePartition;
    rBuff->info.options   = rBuffCreateParams->options;
    rBuff->info.buffSize  = rBuffCreateParams->buffSize;
    rBuff->info.threshold = rBuffCreateParams->threshold;
    rBuff->info.minBuffs  = rBuffCreateParams->minimum;
    rBuff->info.maxBuffs  = (unsigned int) rBuffCreateParams->maximum;
    rBuff->errorHandler   = rBuffCreateParams->errorHandler;

    if (semBInit (&rBuff->buffDesc.threshXSem, SEM_Q_PRIORITY,
		  SEM_EMPTY) == ERROR)
        {
	memPartFree (rBuffCreateParams->sourcePartition, (char *) rBuff);

        return (NULL);
        }

#if 0
    if (semBInit (&rBuff->readBlk, SEM_Q_PRIORITY, SEM_FULL) == ERROR)
        {
        semTerminate (&rBuff->buffDesc.threshXSem);

	memPartFree (rBuffCreateParams->sourcePartition, (char *) rBuff);

        return (NULL);
        }

    if (semBInit (&rBuff->bufferFull, SEM_Q_PRIORITY, SEM_EMPTY) == ERROR)
        {
        semTerminate (&rBuff->buffDesc.threshXSem);
        semTerminate (&rBuff->readBlk);

	memPartFree (rBuffCreateParams->sourcePartition, (char *) rBuff);

        return (NULL);
        }
#endif
    rBuff->rBuffMgrId = 0; /* ...so we can use rBuffDestroy safely */

    /*
     *  If things go wrong from here, use rBuffDestroy to throw back what
     *  we've caught.
     */

    /* allocate 'rBuffCreateParams->minimum' buffers */

    rBuff->info.currBuffs  =
    rBuff->info.emptyBuffs = 0;

    rBuff->buffWrite = NULL;

    /* initialize the handle */

    handleInit (&rBuff->buffDesc.handle, handleTypeRbuff);

    for (count=0;count < rBuffCreateParams->minimum;count++)
        {

        /* First we need a buffer */

        newBuff =
            (RBUFF_PTR)
                memPartAlloc (rBuffCreateParams->sourcePartition,
                sizeof(RBUFF_BUFF_TYPE) + rBuffCreateParams->buffSize);

#ifdef RBUFF_DEBUG
        logMsg ("rBuff: adding buffer %p to ring\n", (_Vx_usr_arg_t) newBuff,
                0,0,0,0,0);
#endif

        /* newBuff will be returned as NULL if source partition is exhausted */

        /*  Don't need to lock ints around rBuffAdd as no-one knows about
         *  this rBuff.
         */

        if (newBuff == NULL || rBuffAdd (rBuff, newBuff) == ERROR)
            {
#ifdef RBUFF_DEBUG
            logMsg ("rBuff: abandoned creation\n",0,0,0,0,0,0);
#endif
            rBuffDestroy ((BUFFER_ID) rBuff);

            return (NULL);
            }
        }

    rBuff->rBuffMgrId = taskSpawn ("tRBuffMgr",
				   rBuffMgrPriorityDefault, RBUFF_MGR_OPTIONS,
				   2048, rBuffMgr, rBuff, 0,0,0,0,0,0,0,0,0);

    if(rBuff->rBuffMgrId == TASK_ID_ERROR)
        {
#ifdef RBUFF_DEBUG
        logMsg ("rBuff: error creating rBuffMgr\n",0,0,0,0,0,0);
#endif
        rBuffDestroy ((BUFFER_ID) rBuff);

        return (NULL);
        }

    /* set control pointers */

    rBuff->nestLevel = 0;

    /* rBuff->buffWrite initialised by rBuffAdd */

    rBuff->buffRead  = rBuff->buffWrite;

    rBuff->dataWrite =
    rBuff->dataRead  = rBuff->buffWrite->dataStart;

    /* reset info */

    rBuff->info.maxBuffsActual   = rBuffCreateParams->minimum;

    rBuff->info.dataContent      =
    rBuff->info.writesSinceReset =
    rBuff->info.readsSinceReset  =
    rBuff->info.timesExtended    =
    rBuff->info.timesXThreshold  =
    rBuff->info.bytesWritten     =
    rBuff->info.bytesRead        =
    rBuff->info.bytesPeak        = 0;

    /* Reset msg passing mechanism */

    rBuff->msgOutstanding        = FALSE;

    rBuff->msgWriteIndex         =
    rBuff->msgReadIndex          = 0;

    /* now set up func ptrs allowing it to be called */

    rBuff->buffDesc.readReserveRtn = &rBuffReadReserve;
    rBuff->buffDesc.readCommitRtn  = &rBuffReadCommit;
    rBuff->buffDesc.writeRtn       = rBuffWrite;
    rBuff->buffDesc.flushRtn       = rBuffFlush;
    rBuff->buffDesc.threshold      = rBuffCreateParams->threshold;
    rBuff->buffDesc.nBytesRtn      = rBuffNBytes;

    /* made it! */

#ifdef RBUFF_DEBUG
    logMsg("Created rBuff with ID %p\n",(_Vx_usr_arg_t) rBuff,0,0,0,0,0);
#endif

    return ((BUFFER_ID) rBuff);
    }

/*******************************************************************************
*
* rBuffWrite - Write data to a ring of buffers
*
* This routine writes data to an extendable ring of buffers. If the existing
* structure is full, and the existing number of buffers is less than the
* specified minimum, then this function will attempt to add another buffer to
* the ring.
*
* This function may be called from interrupt level.
*
* Mutually exclusive access must be guaranteed by the caller.
*
* \NOMANUAL
*/

UINT8 *rBuffWrite
    (
    BUFFER_ID buffId,
    UINT8 *dataSrc,
    size_t numOfBytes
    )
    {
    BOOL startUploading;
    RBUFF_ID rBuff;			/* access this particular rBuff */
    UINT8 *returnPtr = (UINT8 *) ~0;	/* Return a non-zero value if OK */

    /* Get access to the private members of this buffer's descriptor. */

    rBuff = (RBUFF_ID) buffId;

    /* Get out early if this request makes no sense. */

    if (numOfBytes > rBuff->info.buffSize)
        {
        return (NULL);
        }

    /* Critical Region Start */

    if (rBuff->info.dataContent >= rBuff->info.threshold)
        {
        startUploading = TRUE;
        }
    else
        {
        startUploading = FALSE;
        }

    if (rBuff->buffWrite->spaceAvail >= numOfBytes)
        {
        if (dataSrc)
            {
            /* We are writing the data, not just reserving the space */

            memcpy (rBuff->dataWrite, dataSrc, numOfBytes);
            }
        else
            {
            /* Record the start of the reserved area */

            returnPtr = rBuff->dataWrite;
            }

        /*
         *  If we are consuming one of our precious empty buffs,
         *  check we have enough in reserve.
         */

        if (rBuff->buffWrite->dataLen == 0)
	    {
	    --rBuff->info.emptyBuffs;
	    }

	if (rBuff->info.emptyBuffs < RBUFF_EMPTY_KEEP &&
            rBuff->info.currBuffs != rBuff->info.maxBuffs)
            {
            /* Schedule the rBuff Mgr to extend the buffer */

            RBUFF_SEND_MSG (RBUFF_MSG_ADD, MSG_PRI_URGENT, rBuff, 0);
	    }

        rBuff->buffWrite->dataLen    += numOfBytes;
        rBuff->dataWrite             += numOfBytes;
        rBuff->buffWrite->spaceAvail -= numOfBytes;

        /*
         *  It may be that the current buffer is full at this point,
         *  but don't do anything about it yet as the situation will
         *  be handled next time through. Also, it's possible that
         *  the current buffer may even have been emptied by then.
         */

        }
    else
        {
        size_t dataWritten1stBuff;
        size_t dataWritten2ndBuff;

        /*
         *  Data won't fit in this buffer (at least not entirely),
         *  is the next buffer available?
         */

        if (rBuff->buffWrite->next == rBuff->buffRead)
            {

            /*  We've caught the reader. This will only happen when
             *  we have actually filled the maximum allocation of
             *  buffers *or* the reserved buffer has been filled
             *  before the buffer has had the opportunity to
             *  be extended. We cannot extend the buffer at this point
             *  as we may be in a critical region, the design is such
             *  that if the buffer could be extended it should have
             *  been done by now (for that matter, the wrap-around
             *  should have also occurred).
             *
             *  If we have filled the reserved buffer without having
             *  the opportunity to extend the buffer then the buffer
             *  is configured badly and must be retuned.
             *
             *  Meanwhile, at this moment, options are:
             *
             *  1) If we have filled the entire buffer and cannot wrap-
             *     around then return ERROR.
             *  2) If we have filled the entire buffer but can wrap-
             *     around then do that.
             */

            if (rBuff->info.options & RBUFF_WRAPAROUND)
                {

                /*
                 *  OK, perform an inline ring extension supplying NULL
                 *  as the new buffer forcing the ring to wrap-around.
                 *  This is a deterministic action.
                 */

                /* Interrupts already locked out */

                rBuffAdd(rBuff, NULL);
                }
            else
                {
                /* Oh dear, can't wrap-round */

                return (NULL);
                }
            }

        /*
         *  OK.
         *  In the case that this function is writing the data, it
         *  can be shared between this and next buffer.
         *  In the case that we are only reserving the space, we must
         *  return a pointer to contiguous space and therefore skip the
         *  remainder of the previous buffer.
         */

        if (dataSrc == NULL)
            {
            /* skip the remainder of the current buffer */

            rBuff->buffWrite->spaceAvail = 0;

            /* Move on to the next buffer */

            rBuff->buffWrite = rBuff->buffWrite->next;

            /* Record the start of the reserved area */

            returnPtr = rBuff->buffWrite->dataStart;

            /* Point dataWrite past the reserved area */

            rBuff->dataWrite = rBuff->buffWrite->dataStart;

            dataWritten1stBuff = 0;
            dataWritten2ndBuff = numOfBytes;
            }
        else
            {
            /* first make use of the space in this buffer */

            memcpy (rBuff->dataWrite,
                dataSrc,
                (dataWritten1stBuff = rBuff->buffWrite->spaceAvail));

            rBuff->buffWrite->dataLen    += dataWritten1stBuff;
            rBuff->buffWrite->spaceAvail = 0;

            /* Move on to the next buffer */

            dataWritten2ndBuff = numOfBytes - dataWritten1stBuff;

            rBuff->buffWrite = rBuff->buffWrite->next;
            rBuff->dataWrite = rBuff->buffWrite->dataStart;

            memcpy (rBuff->dataWrite,
                dataSrc + dataWritten1stBuff,
                dataWritten2ndBuff);
            }

        rBuff->buffWrite->dataLen    = dataWritten2ndBuff;
        rBuff->dataWrite            += dataWritten2ndBuff;
        rBuff->buffWrite->spaceAvail =
            rBuff->info.buffSize - dataWritten2ndBuff;

        if (--rBuff->info.emptyBuffs < RBUFF_EMPTY_KEEP &&
            rBuff->info.currBuffs != rBuff->info.maxBuffs)
            {
            /* Schedule the rBuff Mgr to extend the buffer */

            RBUFF_SEND_MSG (RBUFF_MSG_ADD, MSG_PRI_URGENT, rBuff, 0);
            }
        }

    rBuff->info.dataContent  += numOfBytes;

    /* update info */

    if(rBuff->info.dataContent > rBuff->info.bytesPeak)
        {
        rBuff->info.bytesPeak = rBuff->info.dataContent;
        }

    rBuff->info.bytesWritten += numOfBytes;

    rBuff->info.writesSinceReset++;

    if (!startUploading && rBuff->info.dataContent >= rBuff->info.threshold)
        {
        rBuff->info.timesXThreshold++;

        if (!(rBuff->info.options & RBUFF_UP_DEFERRED))
            {
            /*
             *  Signal for uploading to begin. Note that if we have just
             *  reserved space it is imperative that uploading does not
             *  actually begin until the data is in the buffer.
             */

            WQADD1 ((FUNCPTR)semBGiveDefer,
		    (int) &rBuff->buffDesc.threshXSem);
            }
        }


    /* Critical Region End */

    return(returnPtr);
    }


/*******************************************************************************
*
* rBuffRead - Read data from a ring of buffers
*
* This routine reads data from an extendable ring of buffers.
*
* This function assumes mutually exclusive access is guaranteed by the caller.
*
* \NOMANUAL
*/

INT32 rBuffRead
    (
    BUFFER_ID buffId,		/* generic buffer descriptor */
    UINT8    *dataDest,
    UINT32  numOfBytes
    )
    {
    UINT32 bytesToCopy, remaining = numOfBytes;
    RBUFF_ID rBuff;		/* access private members of this rBuff desc */

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    /* Critical Region Start */

    while(remaining)
        {
        bytesToCopy = (rBuff->buffRead->dataLen < remaining ?
                        rBuff->buffRead->dataLen : remaining);

        if (bytesToCopy > 0)
            {
            memcpy (dataDest,rBuff->dataRead,bytesToCopy);

            remaining                -= bytesToCopy;
            dataDest                 += bytesToCopy;

            /* Update buffer */

            rBuff->buffRead->dataLen -= bytesToCopy;

            rBuff->dataRead          += bytesToCopy;
            rBuff->info.dataContent  -= bytesToCopy;
            }

        if (!rBuff->buffRead->dataLen)
            {
            /* This buffer is now empty */

            rBuffHandleEmpty (rBuff);
            }
        else
            {
            /* this buffer is not yet emptied */

            rBuff->dataRead += bytesToCopy;
            }
        }

    /* update info */

    rBuff->info.bytesRead += (numOfBytes - remaining);

    rBuff->info.readsSinceReset++;

    /* Critical Region End */

    return (numOfBytes - remaining);
    }


/*******************************************************************************
*
* rBuffReadReserve - Return the number of contiguous bytes available for
*                    reading.
*
* \NOMANUAL
*/

size_t rBuffReadReserve
    (
    BUFFER_ID buffId,		/* generic identifier for this buffer */
    UINT8 **src
    )
    {
    RBUFF_ID rBuff;		/* specific identifier for this rBuff */
    size_t bytesAvailable;

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    /* Generate and return the available contiguous bytes. */

    if ((bytesAvailable = rBuff->buffRead->dataLen))
        {
        *src = rBuff->dataRead;
        }
    else
        {
        *src = NULL;
        }
    return (bytesAvailable);
    }


/*******************************************************************************
*
* rBuffReadCommit - Move the read data ptr along a ring of buffers
*
* This routine moves the data ptr along a ring of buffers.
*
* It is equivalent to reading data from the buffers and should be used
* when the data has been copied elsewhere.
*
* This function assumes mutually exclusive access is guaranteed by the caller.
*
* \NOMANUAL
*/

STATUS rBuffReadCommit
    (
    BUFFER_ID buffId,		/* generic identifier for this buffer */
    size_t  numOfBytes
    )
    {
    RBUFF_ID rBuff;		/* specific identifier for this rBuff */

    /* Get access to the private members of this particular rBuff. */

    if (!numOfBytes)
    {
        return (OK);
    }

    rBuff = (RBUFF_ID) buffId;

    /* Critical Region Start */

    if (numOfBytes == rBuff->buffRead->dataLen)
        {
        rBuffHandleEmpty (rBuff);
        }
    else if (numOfBytes < rBuff->buffRead->dataLen)
        {
        rBuff->buffRead->dataLen -= numOfBytes;
        rBuff->dataRead += numOfBytes;
        }
    else
        {
        /* Moving ahead through multiple buffers */

        /* Not yet supported */

        return(ERROR);
        }

    rBuff->info.dataContent -= numOfBytes;

    /* update info */

    rBuff->info.bytesRead += numOfBytes;

    rBuff->info.readsSinceReset++;

    /* Critical Region End */

    return (OK);
    }


/*******************************************************************************
*
* rBuffFlush - Flush data from a ring of buffers
*
* This routine causes any data held in a buffer to be uploaded and is
* used to clear data that falls below the specified threshold.
*
* \NOMANUAL
*/

STATUS rBuffFlush
    (
    BUFFER_ID buffId		/* generic identifier for this buffer */
    )
    {
    RBUFF_ID rBuff;		/* specific identifier for this rBuff */
    int result, originalThreshold;

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    result = OK;

    if (!rBuff->info.dataContent)
        {
        return(OK);
        }

    /* Store the original threshold */

    originalThreshold = rBuff->info.threshold;

    rBuff->info.threshold = 0;

    while (rBuff->info.dataContent > 0 && result != ERROR)
        {
        if (semGive (&rBuff->buffDesc.threshXSem) == OK)
            {
            /* Cause a reschedule to allow uploader in */

            taskDelay(0);
            }
        else
            {
            result = ERROR;
            }
        }

    rBuff->info.threshold = originalThreshold;

    return(result);
}


/*******************************************************************************
*
* rBuffReset - reset an extendable ring of buffers
*
* This routine resets an already created extendable ring of buffers to its
* initial state. This loses any data held in the buffer. Any buffers held
* above the specified minimum number are deallocated.
*
* \NOMANUAL
*/

STATUS rBuffReset
    (
    BUFFER_ID buffId		/* generic identifier for this buffer */
    )
    {
    RBUFF_ID rBuff;		/* specific identifier for this rBuff */
    RBUFF_LOCK (rBuff);

    if (buffId == NULL)
        return (ERROR);

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    /* Now reset the ring of buffers. */

    if(rBuff->info.currBuffs > rBuff->info.minBuffs)
        {
        UINT32          excessBuffs, count;
        RBUFF_PTR       newHome;
        RBUFF_PTR       lastBuff;


        /* drop the excess buffers */

        excessBuffs = rBuff->info.currBuffs - rBuff->info.minBuffs;

        for (count=0;count < excessBuffs;count++)
            {
            RBUFF_PTR tempPtr;

            tempPtr = rBuff->buffWrite->next; /* should not be NULL! */

            rBuffFree (rBuff,rBuff->buffWrite);

            rBuff->buffWrite = tempPtr;
            }

        /*
         * Now we're pointing to the first buff that will be left
         * Go through to the last one, and set its next pointer to the one we're
         * at now
         */

        newHome = rBuff->buffWrite;
        lastBuff = newHome;

        /* Step to the last one, not past it */

        for (count=0; count < rBuff->info.minBuffs - 1; count++)
            lastBuff = lastBuff->next;

        lastBuff->next = newHome;
        }

    /* Make sure the 'read' buffer pointer points somewhere sensible */

    rBuff->buffRead = rBuff->buffWrite;

    /* and reset the byte ptrs */

    rBuff->dataRead         =
    rBuff->dataWrite        = rBuff->buffWrite->dataStart;

    rBuff->info.currBuffs        =
    rBuff->info.maxBuffsActual   = rBuff->info.minBuffs;

    /* now traverse the ring resetting the individual buffers */

    {
    RBUFF_PTR backHome = rBuff->buffWrite;
    RBUFF_PTR currBuff = rBuff->buffWrite;

    do  {
        currBuff->dataLen    = 0;
        currBuff->spaceAvail = rBuff->info.buffSize;

        currBuff = currBuff->next;
        }
    while (currBuff != backHome);

    }

    rBuff->nestLevel = 0;

    /* reset info */

    rBuff->info.emptyBuffs       = rBuff->info.minBuffs;
    rBuff->info.dataContent      =
    rBuff->info.writesSinceReset =
    rBuff->info.readsSinceReset  =
    rBuff->info.timesExtended    =
    rBuff->info.timesXThreshold  =
    rBuff->info.bytesWritten     =
    rBuff->info.bytesRead        = 0;

    semGive (&rBuff->bufferFull);

    RBUFF_UNLOCK (rBuff);

    return (OK);
}


/*******************************************************************************
*
* rBuffNBytes - Returns the total number of bytes held in a ring of buffers
*
* \NOMANUAL
*/

size_t rBuffNBytes
    (
    BUFFER_ID buffId		/* generic identifier for this buffer */
    )
    {
    RBUFF_ID rBuff;		/* specific identifier for this rBuff */

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    return (rBuff->info.dataContent);
    }


/*******************************************************************************
*
* rBuffAdd - Add a new buffer to an extendable ring of buffers
*
* This routine adds a further buffer to an already created or partially created
* extendable ring of buffers unless the maximum number is already allocated.
*
* This function assumes mutually exclusive access is guaranteed by the caller.
*
* \NOMANUAL
*/

STATUS rBuffAdd
    (
    RBUFF_ID rBuff,
    RBUFF_PTR newBuff
    )
    {
    if (rBuff->info.emptyBuffs >= RBUFF_EMPTY_KEEP &&
        rBuff->info.currBuffs  >= rBuff->info.minBuffs)
        {
        /*
         *  OK, the uploader got in so now we don't need to extend the
         *  buffer.
         */

#ifdef RBUFF_DEBUG
        logMsg ("rBuff: abandoned add\n",0,0,0,0,0,0);
#endif

        if (newBuff)
            {
            memPartFree (rBuff->info.srcPart, (UINT8 *) newBuff);
            }

        return (OK);
        }

    if (rBuff->info.currBuffs == rBuff->info.maxBuffs || newBuff == NULL)
        {

        /*
         *  We are at the maximum ring size already or the source
         *  partition is exhausted, there's still hope...
         *
         *  ...can we wrap-around?
         */

        if (!(rBuff->info.options & RBUFF_WRAPAROUND))
            {
            /* We aren't allowed to wrap around */

            return (ERROR);
            }
        else
            {
            RBUFF_PTR reclaim;

            /*  We can wrap around by comandeering the 'oldest'
             *  buffer in the ring.
             *
             *  It is possible (probable?) that this buffer is the
             *  one to be read from next so handle this.
             */

            reclaim = rBuff->buffWrite->next;

            if (rBuff->buffRead == reclaim)
                {
                /* Move the reader along */

                rBuff->buffRead = rBuff->buffRead->next;
                rBuff->dataRead = rBuff->buffRead->dataStart;
                }

            rBuff->info.dataContent  -= reclaim->dataLen;

            /* Reset the buffer */

            reclaim->dataLen    = 0;
            reclaim->spaceAvail = rBuff->info.buffSize;

            rBuff->info.emptyBuffs++;

            return (OK);
            }
        }

    newBuff->dataStart  = (UINT8 *) newBuff + sizeof(RBUFF_BUFF_TYPE);
    newBuff->dataEnd    = newBuff->dataStart + rBuff->info.buffSize;
    newBuff->spaceAvail = rBuff->info.buffSize;
    newBuff->dataLen    = 0;

    /* Now link the buffer into the ring */

    if (rBuff->buffWrite != NULL)
        {
        RBUFF_PTR tempPtr;

        tempPtr = rBuff->buffWrite->next;

        rBuff->buffWrite->next = newBuff;

        /* close the ring */

        newBuff->next = tempPtr;

        }
    else
        {

        /* ring is empty, must be creation time */

        rBuff->buffWrite =
        rBuff->buffRead  =
        newBuff->next    = newBuff;
        }

    /* Maintain statistics */

    if (++(rBuff->info.currBuffs) > rBuff->info.maxBuffsActual)
        {
        rBuff->info.maxBuffsActual++;
        }

    rBuff->info.timesExtended++;
    rBuff->info.emptyBuffs++;

    return (OK);
}


/*******************************************************************************
*
* rBuffFree - Free a specified buffer
*
* This function must only be called from task level.
* Semaphores are used to protect access at task-level. However, protection
* from access at interrupt level must be guaranteed by the caller.
*
* \NOMANUAL
*/

LOCAL STATUS rBuffFree
    (
    RBUFF_ID 	rBuff,          /* enclosing rBuff */
    RBUFF_PTR 	buffToFree      /* ring buffer element to free */
    )
    {
    RBUFF_PTR 	ptrToBuff, backHome;
    RBUFF_PTR   ptrToPreviousBuff = NULL;
    BOOL        found = FALSE;

    if (buffToFree == NULL)
        {
        return(ERROR);
        }

    /* find the pointer to the buffer to be freed */

    ptrToBuff = rBuff->buffWrite;

    /* this is the loop termination condition */

    backHome = rBuff->buffWrite;

    /*
     * testing the next buffer in the ring means that we always have a valid
     * previous buffer pointer. So if we want to delete the first buffer, we end
     * up going all through the list, until we get to the last one. Then,
     * ptrToPreviousBuff, becomes the last one, and we delete the first.
     */

    do
        {
        if (buffToFree == ptrToBuff->next)
            {
            found = TRUE;
            ptrToPreviousBuff = ptrToBuff;
            break;
            }

        ptrToBuff = ptrToBuff -> next;

        } while (ptrToBuff != backHome);

    if (!found)
        {
#ifdef RBUFF_DEBUG
#ifdef _WRS_CONFIG_LP64
        logMsg ("delete failed to find buffer 0x%16x\n", 
#else
        logMsg ("delete failed to find buffer 0x%08x\n",
#endif /* _WRS_CONFIG_LP64 */
                (_Vx_usr_arg_t)buffToFree, 0, 0, 0, 0, 0);
#endif
        return (ERROR);
        }

    /* We found the pointer */
    /* OK, de-link the buffer to be freed */

    ptrToPreviousBuff->next = buffToFree->next;

    /* Schedule the rBuff Mgr to actually free the buffer */

    /*
     * This doesn't work (never has) because the rBuffMgr doesn't get to run
     * until after the msgQ indicies have been reset
     */

    /* RBUFF_SEND_MSG (RBUFF_MSG_FREE, MSG_PRI_NORMAL, rBuff, buffToFree); */

    memPartFree (rBuff->info.srcPart,(UINT8 *)buffToFree);

    /* Maintain statistics */

    rBuff->info.emptyBuffs--;
    rBuff->info.currBuffs--;

    return (OK);
    }

/*******************************************************************************
*
* rBuffDestroy - Destroy an extendable ring of buffers
*
* This routine destroys an already created or partially created extendable ring
* of buffers. This loses any data held in the buffer. All resources held are
* returned to the system.
*
* \NOMANUAL
*/

STATUS rBuffDestroy
    (
    BUFFER_ID buffId		/* generic identifier for this buffer */
    )
    {
    RBUFF_ID rBuff;		/* specific identifier for this rBuff */
    RBUFF_PTR tempPtr;

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    RBUFF_LOCK (rBuff);

    if(rBuff->rBuffMgrId != NULL)
        {
        taskDelete(rBuff->rBuffMgrId);
        }

#if 0
    /* validate rBuff ID */

    if (HANDLE_VERIFY (&rBuff->buffDesc.handle, handleTypeRbuff) != OK)
	{
	RBUFF_UNLOCK (rBuff);

	return (ERROR);
	}
#endif

    /* invalidate this object */

    handleTerminate (&rBuff->buffDesc.handle);

    /* Break the ring and traverse the list, freeing off   */
    /* the buffers. buffWrite is used as the start point   */
    /* this works if the ring creation is abandoned early. */

    if (rBuff->buffWrite != NULL)
        {

        /* Break the ring */

        tempPtr = rBuff->buffWrite->next;
        rBuff->buffWrite->next = NULL;
        rBuff->buffWrite = tempPtr;

        /* traverse and free */

        while(rBuff->buffWrite != NULL)
            {
            tempPtr = rBuff->buffWrite->next;

            memPartFree (rBuff->info.srcPart,(UINT8 *)rBuff->buffWrite);

            rBuff->buffWrite = tempPtr;
            }
        }
    else
        {
        /* ring hasn't been created so don't worry about it */
        }

    /* now free off the control structure */

    semTerminate (&rBuff->buffDesc.threshXSem);
#if 0
    semTerminate (&rBuff->readBlk);
    semTerminate (&rBuff->bufferFull);
#endif
    memPartFree (rBuff->info.srcPart, (char *) rBuff);

    /* we're done */

    return (OK);
    }


/*******************************************************************************
*
* rBuffSetFd - Set a new fd as the data upload destination
*
*
* \NOMANUAL
*/

STATUS rBuffSetFd
    (
    BUFFER_ID buffId,		/* generic identifier for this buffer */
    int fd
    )
    {
    RBUFF_ID rBuff;		/* specific identifier for this rBuff */
    RBUFF_LOCK (rBuff);

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    rBuff->fd = fd;

    RBUFF_UNLOCK (rBuff);

    return (OK);
    }


/*******************************************************************************
*
* rBuffHandleEmpty - Handles a newly empty buffer by deciding whether to free
*                    it off.
*
* This function assumes mutually exclusive access is guaranteed by the caller.
*
* \NOMANUAL
*/

LOCAL STATUS rBuffHandleEmpty
    (
    RBUFF_ID rBuff
    )
    {
    BOOL      moveAlong = TRUE;
    RBUFF_PTR buffToFree;

    /*
     *  Determine if we are to move around the ring and if the
     *  newly empty buffer is to be freed, and then do the actions.
     *  This avoids having to fumble with ptrs as we move around
     *  from a buffer that has been freed.
     */

    if (rBuff->buffRead == rBuff->buffWrite)
        {

        /*
         *  We've caught the writer so might as well reset this
         *  buffer and make efficient use of it.
         */

        rBuff->dataRead  =
            rBuff->dataWrite = rBuff->buffWrite->dataStart;

        rBuff->buffRead->spaceAvail = rBuff->info.buffSize;
        rBuff->buffRead->dataLen     = 0;

        /* In this case, we don't move along to the next buffer */

        moveAlong = FALSE;
        }

    if (++(rBuff->info.emptyBuffs) > RBUFF_EMPTY_KEEP &&
           rBuff->info.currBuffs > rBuff->info.minBuffs)
        {
        /* We already have enough empty buffs, so free one off */

        if (rBuff->buffRead != rBuff->buffWrite)
            {
            buffToFree = rBuff->buffRead;
            }
            else
            {
            /*
             *  As the reader and writer are in the same buffer
             *  we know it's safe to free off the next buffer.
             */

            buffToFree = rBuff->buffRead->next;
            }
        }
    else
        {
        /* we've just emptied one of the 'core' buffers */

        rBuff->buffRead->spaceAvail = rBuff->info.buffSize;
        rBuff->buffRead->dataLen    = 0;

        /* Don't free */

        buffToFree = NULL;
        }

    if (moveAlong)
        {
        /* Move round the ring */

        rBuff->buffRead = rBuff->buffRead->next;
        rBuff->dataRead = rBuff->buffRead->dataStart;
        }
    else
        {
        /* reset flag for next time */

        moveAlong = TRUE;
        }

    if (/* there's a */ buffToFree)
        {
#ifdef RBUFF_DEBUG
        logMsg ("rBuff: Freeing buffer %p\n",
            (_Vx_usr_arg_t) buffToFree,0,0,0,0,0);
#endif
        rBuffFree (rBuff, buffToFree);
        }

    return (OK);
    }


/*******************************************************************************
*
* rBuffMgr - The function is spawned as a task to manage rBuff extension.
*
* It waits for a msg which carries the ID of a buffer which has had a change
* of the number of empty buffers held. If there are less the requested to be
* reserved than an attempt will be made to make more available. If there are
* more empty buffers than required than some will be freed.
*
* For each msg received, the task will achieve the required number of free
* buffers. This may result in redundant msgs being received.
*
* \NOMANUAL
*/

int rBuffMgr
    (
    RBUFF_TYPE *rBuffId
    )
    {
    UINT32     busy;
    int        key;
    RBUFF_PTR  newBuff;

#ifdef RBUFF_DEBUG
        logMsg ("rBuff: Mgr for rBuff %p created\n",(_Vx_usr_arg_t) rBuffId,
                0,0,0,0,0);
#endif

    for(;;)
        {

	semTake (&(rBuffId->msgSem), WAIT_FOREVER);

#ifdef RBUFF_DEBUG
        logMsg ("rBuff: Msg type 0x%x received from rBuff %p\n",
            (_Vx_usr_arg_t) rBuffId->msg[rBuffId->msgReadIndex][0],
            (_Vx_usr_arg_t) rBuffId,0,0,0,0);
#endif

        busy = TRUE;

        /* Determine what needs to be done */

        switch (rBuffId->msg[rBuffId->msgReadIndex][0])
            {
            case RBUFF_MSG_ADD :
                {

                /*
                 *  This message may be generated one or more times by
                 *  event logging, and the condition that caused the msg
                 *  may no longer be TRUE, so just take this as a prompt
                 *  to see if the ring needs extending.
                 */

                /* Make sure that we don't request extension recursively */

                rBuffId->nestLevel++;

                while
                    (rBuffId->info.emptyBuffs < RBUFF_EMPTY_KEEP &&
                    (rBuffId->info.currBuffs  < rBuffId->info.maxBuffs ||
                        rBuffId->info.options & RBUFF_WRAPAROUND) &&
                    busy)
                    {

                    /* Looks like we are going to need a buffer */

                    newBuff = (RBUFF_PTR)
                        memPartAlloc (rBuffId->info.srcPart,
                            sizeof(RBUFF_BUFF_TYPE) + rBuffId->info.buffSize);

                    /* of course, newBuff may be NULL... */

                    key = intLock();

#ifdef RBUFF_DEBUG
                    logMsg ("rBuff: adding buffer %p\n",
                            (_Vx_usr_arg_t) newBuff,0,0,0,0,0);
#endif

		    if (rBuffAdd (rBuffId, newBuff) == ERROR)
                        {
                        /* ring cannot be extended so give up trying */

                        busy = FALSE;
                        }

                    intUnlock (key);
                    }
                }

                rBuffId->nestLevel--;

            break;

            case RBUFF_MSG_FREE :
                {
                /* This message carries a ptr to the buffer to be freed */

#ifdef RBUFF_DEBUG
                logMsg ("rBuff: freeing buffer %p\n",
                        (_Vx_usr_arg_t) rBuffId->msg[rBuffId->msgReadIndex][1],
                        0,0,0,0,0);
#endif

                memPartFree (rBuffId->info.srcPart,
                    (UINT8 *) rBuffId->msg[rBuffId->msgReadIndex][1]);
                }
            break;
            }

         if(++rBuffId->msgReadIndex > RBUFF_MAX_MSGS)
             {
             rBuffId->msgReadIndex = 0;
             }

         rBuffId->msgOutstanding = FALSE;

         }

   }

/*******************************************************************************
*
* rBuffMgrPrioritySet - set the priority of ring Buffer Manager task
*
* This routine sets the priority of the rBuffMgr task to the priority given.
* Two parameters are expected:
* rBuffId : can be NULL or a valid ring Buffer Id.
*           If NULL, the default priority of any future ring buffer manager
*           tasks spawned will be set to the priority given. The priority of any
*    	    existing ring buffer manager tasks running will be unaffected.
*
*           If a valid ring buffer id is given, the priority of its associated
*           ring buffer manager will be changed to that given. In addition, the
*           priority of any future ring buffer manager tasks spawned will be
*           changed to the priority given. If the priority of the associated
*           manager cannot be set for any reason, the routine returns ERROR and
*           the default remains unchanged.
*
* priority :The desired priority of the ring buffer manager task (0-255).
*
* \NOMANUAL
*
* RETURNS: OK, or ERROR if the priority could not be set.
*/

STATUS rBuffMgrPrioritySet
    (
    RBUFF_ID rBuffId,		/* rBuff identifier */
    int	     priority           /* new priority */
    )
    {
    if (rBuffId != NULL)
 	if (taskPrioritySet (rBuffId->rBuffMgrId, priority) != OK)
	    return (ERROR);

    rBuffMgrPriorityDefault = priority;
    return (OK);
    }

/*******************************************************************************
*
* rBuffUpload - Copy data that is added to the buffer to the specified 'fd'
*
* \NOMANUAL
*/

STATUS rBuffUpload
    (
    BUFFER_ID buffId,		/* generic identifier for this buffer */
    int fd
    )
    {
    RBUFF_ID rBuff;		/* specific identifier for this rBuff */
    UINT8 *src;
    int lockKey;
    STATUS result = OK;

    /* Get access to the private members of this particular rBuff. */

    rBuff = (RBUFF_ID) buffId;

    rBuff->fd = fd;

    while(result == OK)
        {

        /* Wait for the dinner gong */

        logMsg ("Waiting for data\n",0,0,0,0,0,0);

        semTake (&rBuff->buffDesc.threshXSem,WAIT_FOREVER);

        logMsg("Uploading...\n",0,0,0,0,0,0);

        /* tuck in... */

        while (result == OK && rBuff->info.dataContent >
              (rBuff->info.threshold / 4))
            {
            size_t bytesToWrite = rBuffReadReserve(buffId, &src);
            size_t bytesWritten;

            if (bytesToWrite > 4096)
                {
                    bytesToWrite = 4096;
                }

#ifdef RBUFF_DEBUG

            logMsg ("%#x bytes from %p (dataContent=%#x)\n",
                    (_Vx_usr_arg_t) bytesToWrite,
                    (_Vx_usr_arg_t) src,
                    (_Vx_usr_arg_t) rBuff->info.dataContent,0,0,0);
#endif


            /* move the data */

#if 0
{
UINT8 buffer[100];

bcopy(src,buffer,bytesToWrite);
buffer[bytesToWrite] = '\0';

printf("%s\n",buffer);
}
#endif

            if ((bytesWritten =
                write (fd, src, bytesToWrite)) == -1)
                {
                logMsg ("Upload error!\n",0,0,0,0,0,0);

                if (rBuff->errorHandler)
                    {

                    /* Dial 999 / 911 */

                    (*rBuff->errorHandler)(RBUFF_ERR_UPLOAD);
                    }

                result = ERROR;
                }
            else
                {
                /* move the buffer pointers along */

                lockKey = intLock();

                result = rBuffReadCommit (buffId,bytesWritten);

                intUnlock (lockKey);

                taskDelay (0);
                }
            }
        }

    return (result);
    }

#endif /* GENERIC_RBUFF */

#if 0
LOCAL rBuffCreateParamsType rBuffParams;
extern rBuffCreateParamsType   wvDefaultRBuffParams;

/****************************************************************************
*
* nBytesWrite - write data to rBuff
*
* RETURNS: OK, or ERROR
*
* ERRNO:
*
* NOMANUAL
*/

LOCAL STATUS nBytesWrite
    (
    BUFFER_ID   buffId,
    size_t      nBytes
    )
    {
    unsigned char *     writePtr;
    int                 n;

    writePtr = buffId->writeRtn(buffId, NULL, nBytes);
    if (writePtr == NULL)
        {
        logMsg ( "Error allocating %d bytes in buffer", 
                (_Vx_usr_arg_t) nBytes, 0, 0, 0, 0,0 );
        return (ERROR);
        }

    for (n=0; n<nBytes; n++)
        *(writePtr+n) = (char)n;

    return (OK);
    }


/****************************************************************************
*
* nBytesRead - read data from rBuff, validate it
*
* RETURNS: OK, or ERROR
*
* ERRNO:
*
* NOMANUAL
*/

LOCAL STATUS nBytesRead
    (
    BUFFER_ID   buffId, /* buffer to test */
    size_t      nBytes  /* number of bytes */
    )
    {
    unsigned char *     readPtr;
    int                 n;
    size_t              bytesAvail;

    bytesAvail = rBuffReadReserve (buffId, &readPtr);
    if (bytesAvail < nBytes)
        {
        logMsg( "Incorrect number of bytes available "
                "(expected %d, found %d", 
                (_Vx_usr_arg_t) nBytes, 
                (_Vx_usr_arg_t) bytesAvail, 0, 0, 0, 0);
        return (ERROR);
        }

    for (n=0; n<nBytes; n++)
        if (*(readPtr+n) != (char)n)
            {
            logMsg ( "Incorrect data read at 0x%x (expected %d, got %d)\n",
                     (_Vx_usr_arg_t)(readPtr+n), (_Vx_usr_arg_t) n, 
                     (_Vx_usr_arg_t) *(readPtr+n), 0, 0, 0);
            return (ERROR);
            }

    if (rBuffReadCommit (buffId, nBytes) != OK)
        {
        logMsg ("rBuffReadCommit() returned ERROR", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    return (OK);
    }


STATUS rBuffTest01 (void)
    {
    BUFFER_ID           buffId;
    unsigned char *     readPtr;
    int                 n;

    wvEvtBufferFullNotify = 0;

    memcpy ((void *)&rBuffParams, &wvDefaultRBuffParams, sizeof (rBuffParams));

    rBuffParams.minimum = 10;
    rBuffParams.maximum = 10;
    rBuffParams.buffSize = 20;
    rBuffParams.threshold = 5;
    rBuffParams.options = 0;

    buffId = rBuffCreate (&rBuffParams);

    if (buffId == NULL)
        {
        logMsg ( "Failed to create rBuff", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    if (rBuffDestroy (buffId) != OK)
        {
        logMsg ( "Failed to destroy rBuff", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }


    buffId = rBuffCreate (&rBuffParams);

    if (buffId == NULL)
        {
        logMsg ( "Failed to create rBuff", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    logMsg ("New buffer: 0x%x\n", (_Vx_usr_arg_t)buffId, 0, 0, 0, 0, 0);
    for (n=0; n < 20; n++)
        {
        if (nBytesReadWrite (buffId, n) != OK)
            {
            logMsg ("Failed to write and verify data %d\n", (_Vx_usr_arg_t) n, 
                    0, 0, 0, 0, 0);
            taskDelay (60);
            return (ERROR);
            }
        }

    logMsg ("read/write test ok\n", 0, 0, 0, 0, 0, 0);

    for (n=0; n < 10; n++)
        {
        if (nBytesWrite (buffId, n) != OK)
            {
            return (ERROR);
            }
        }

    logMsg ("10 buffers used\n", 0, 0, 0, 0, 0, 0);

    taskDelay (120);
    rBuffShow (buffId, 1);
    taskDelay (120);


    for (n=0; n < 10; n++)
        {
        if (nBytesRead (buffId, n) != OK)
            {
            logMsg ( "Failed to read data back from rBuff", 0, 0, 0, 0, 0, 0);
            return (ERROR);
            }
        }

    if (rBuffReadReserve (buffId, &readPtr) != 0)
        {
        logMsg ( "Data still available in buffer (should be empty)", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    if (rBuffDestroy (buffId) != OK)
        {
        logMsg ("Failed to destroy rBuff", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    return (OK);
    }

/****************************************************************************
*
* nBytesReadWrite - write data to rBuff, and read it back
*
* RETURNS: OK, or ERROR
*
* ERRNO:
*
* NOMANUAL
*/

LOCAL STATUS nBytesReadWrite
    (
    BUFFER_ID   buffId, /* ring buffer to write to */
    size_t      nBytes  /* number of bytes */
    )
    {
    if (nBytesWrite (buffId, nBytes) != OK)
        {
        logMsg ("Failed to write %d bytes\n", (_Vx_usr_arg_t) nBytes, 
                0, 0, 0, 0, 0);
        return (ERROR);
        }

    return (nBytesRead (buffId, nBytes));
    }


#define RBUFF_MGR_DELAY 12

STATUS rBuffTest03 (void)
    {
    BUFFER_ID           buffId;
    unsigned char *     readPtr;
    int                 n;

    wvEvtBufferFullNotify = 0;

    memcpy ((void *)&rBuffParams, &wvDefaultRBuffParams, sizeof (rBuffParams));

    rBuffParams.minimum = 10;
    rBuffParams.maximum = 20;
    rBuffParams.buffSize = 20;
    rBuffParams.threshold = 5;
    rBuffParams.options = RBUFF_WRAPAROUND;

    buffId = rBuffCreate (&rBuffParams);

    logMsg ("rBuff created: 0x%x\n", (_Vx_usr_arg_t)buffId, 0, 0, 0, 0, 0);

    if (buffId == NULL)
        {
        logMsg( "Failed to create rBuff", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    rBuffShow (buffId, 1);

    /* use 15 buffers, which requires 5 new ones to be created and added */

    for (n=0; n < 15; n++)
        {
        if (nBytesWrite (buffId, 16) != OK)
            {
            return (ERROR);
            }
        taskDelay (RBUFF_MGR_DELAY);
        }

    /* check buffer not full */
    if (wvEvtBufferFullNotify)
        {
        logMsg( "Log became full unexpectedly", 0, 0, 0, 0, 0,0);
        return (ERROR);
        }

    /* Now use up 6 buffers */

    for (n=0; n < 6; n++)
        {
        if (nBytesWrite (buffId, 16) != OK)
            {
            logMsg ("Unable to write data into wrapping buffer\n", 0, 0, 0, 0, 0, 0);
            return (ERROR);
            }

        taskDelay (RBUFF_MGR_DELAY);
        }

    /* check buffer still not full (it's wrapped) */

    if (wvEvtBufferFullNotify)
        {
        logMsg( "Log became full unexpectedly instead of wrapping", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }


    /* There should now be 20 buffers available */

    for (n=0; n < 20; n++)
        {
        if (nBytesRead (buffId, 16) != OK)
            {
            logMsg( "Unable to read expected data from rBuff", 0, 0, 0, 0, 0, 0);
            return (ERROR);
            }
        }

    if (rBuffReadReserve (buffId, &readPtr) != 0)
        {
        logMsg( "Data still present in rBuff (should be empty)", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    if (rBuffDestroy (buffId) != OK)
        {
        logMsg( "Failed to destroy rBuff", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    return (OK);
    }

STATUS rBuffTest04 (void)
    {
    BUFFER_ID           buffId;
    unsigned char *     readPtr;
    int                 n;
    RBUFF_TYPE *        pBuff;

    wvEvtBufferFullNotify = 0;

    memcpy ((void *)&rBuffParams, &wvDefaultRBuffParams, sizeof (rBuffParams));

    rBuffParams.minimum = 10;
    rBuffParams.maximum = 20;
    rBuffParams.buffSize = 20;
    rBuffParams.threshold = 5;
    rBuffParams.options = 0;

    buffId = rBuffCreate (&rBuffParams);
    pBuff = (RBUFF_TYPE *)buffId;

    logMsg ("rBuff created: 0x%x\n", (_Vx_usr_arg_t)buffId, 0, 0, 0, 0, 0);

    if (buffId == NULL)
        {
        logMsg( "Failed to create rBuff", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /* use 15 buffers, which requires 5 new ones to be created and added */

    for (n=0; n < 15; n++)
        {
        if (nBytesWrite (buffId, 16) != OK)
            {
            return (ERROR);
            }
        taskDelay (RBUFF_MGR_DELAY);

        rBuffShow (buffId, 1);

        taskDelay (120);
        }

    /* check buffer not full */
    if (wvEvtBufferFullNotify != FALSE)
        {
        logMsg( "Log became full unexpectedly (1st test)\n", 0, 0, 0, 0, 0,0);
        return (ERROR);
        }

    /* check info stats */
    if (pBuff->info.currBuffs != 16)
        {
        logMsg( "Wrong number of buffers: got %d, expected 16\n", 
               (_Vx_usr_arg_t) pBuff->info.currBuffs, 0, 0, 0, 0,0);
        return (ERROR);
        }

    for (n=0; n < 5; n++)
        {
        /* expect 15 full buffers, plus one empty one */
        if (nBytesWrite (buffId, 16) != OK)
            {
            logMsg ("Failed to write data to buffer\n", 0, 0, 0, 0, 0, 0);
            break;
            }
        taskDelay (RBUFF_MGR_DELAY);
        }

    taskDelay (RBUFF_MGR_DELAY);

    /* check buffer not full */
    if (wvEvtBufferFullNotify != FALSE)
        {
        logMsg( "Log became full unexpectedly (2nd test)\n", 0, 0, 0, 0, 0,0);
        return (ERROR);
        }

    /* check info stats */
    if (pBuff->info.currBuffs != 20)
        {
        logMsg( "Wrong number of buffers: %d\n", 
               (_Vx_usr_arg_t) pBuff->info.currBuffs, 0, 0, 0, 0,0);
        return (ERROR);
        }

    if (nBytesWrite (buffId, 16) == OK)
        {
        logMsg( "Data written when insufficient space\n", 
               (_Vx_usr_arg_t) pBuff->info.currBuffs, 0, 0, 0, 0,0);        
        return (ERROR);
        }

    taskDelay (RBUFF_MGR_DELAY);

    /* check buffer now full */
    if (wvEvtBufferFullNotify != TRUE)
        {
        logMsg( "Log did not become full \n", 0, 0, 0, 0, 0,0);
        return (ERROR);
        }

    /* check info stats */
    if (pBuff->info.currBuffs != 20)
        {
        logMsg( "Wrong number of buffers: %d\n", 
               (_Vx_usr_arg_t) pBuff->info.currBuffs, 0, 0, 0, 0,0);
        return (ERROR);
        }


    return (OK);
    }


STATUS rBuffTest08 (void)
    {
    BUFFER_ID           buffId;
    unsigned char *     readPtr;
    int                 n;
    RBUFF_TYPE *        pBuff;

    wvEvtBufferFullNotify = 0;

    memcpy ((void *)&rBuffParams, &wvDefaultRBuffParams, sizeof (rBuffParams));

    rBuffParams.minimum = 10;
    rBuffParams.maximum = 20;
    rBuffParams.buffSize = 20;
    rBuffParams.threshold = 5;
    rBuffParams.options = 0;

    buffId = rBuffCreate (&rBuffParams);
    pBuff = (RBUFF_TYPE *)buffId;

    if (buffId == NULL)
        {
        logMsg ( "Failed to create rBuff", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /* use 15 buffers, which requires 5 new ones to be created and added */

    logMsg ("buffId 0x%x\n", (int)buffId, 0, 0, 0, 0, 0);

    for (n=0; n < 15; n++)
        {
        if (nBytesWrite (buffId, 16) != OK)
            {
            return (ERROR);
            }
        taskDelay (RBUFF_MGR_DELAY);
        }

    /* check buffer not full */
    if (wvEvtBufferFullNotify != FALSE)
        {
        logMsg ( "Log became full unexpectedly", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /* check info stats */
    if (pBuff->info.currBuffs != 16)
        {
        logMsg ( "Wrong number of buffers: got %d, expected 16\n",
                 (_Vx_usr_arg_t) pBuff->info.currBuffs, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    rBuffShow (buffId, 1);

    taskDelay (120);

    for (n=0; n < 10; n++)
        {
        /* expect 15 full buffers, plus one empty one */
        if (nBytesRead (buffId, 16) != OK)
            {
            logMsg ( "Failed to read data from buffer", 0, 0, 0, 0, 0, 0);
            break;
            }
        taskDelay (RBUFF_MGR_DELAY);
        }

    taskDelay (RBUFF_MGR_DELAY);

    /* check buffer not full */
    if (wvEvtBufferFullNotify != FALSE)
        {
        logMsg ( "Log became full unexpectedly (2nd test)", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /* check info stats */
    if (pBuff->info.currBuffs != 10)
        {
        logMsg ( "Wrong number of buffers in ring: %d",
                  (_Vx_usr_arg_t) pBuff->info.currBuffs, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    if (pBuff->info.emptyBuffs != 5)
        {
        logMsg ( "Wrong number of empty buffers: %d",
                  (_Vx_usr_arg_t) pBuff->info.emptyBuffs, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    if (rBuffDestroy (buffId) != OK)
        {
        logMsg ( "Failed to destroy buffer", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    return (OK);
    }

char *              pBuff;
char *              ptr;

STATUS rBuffTest09 (void)
    {
    BUFFER_ID           buffId;
    unsigned char *     readPtr;


    wvEvtBufferFullNotify = 0;

    memcpy ((void *)&rBuffParams, &wvDefaultRBuffParams, sizeof (rBuffParams));

    rBuffParams.minimum = 10;
    rBuffParams.maximum = 10;
    rBuffParams.buffSize = 20;
    rBuffParams.threshold = 5;
    rBuffParams.options = 0;

    buffId = rBuffCreate (&rBuffParams);

    if (buffId == NULL)
        {
        logMsg ( "Failed to create rBuff", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    logMsg ("New buffer: 0x%x\n", (int)buffId, 0, 0, 0, 0, 0);

    ptr = rBuffWrite (buffId, 0, 2);
    if (ptr == NULL)
        {
        logMsg ("Failed to write and verify data %d\n", 0, 0, 0, 0, 0, 0);
        taskDelay (60);
        return (ERROR);
        }
    *ptr = 0;
    *(ptr+1) = 1;

    logMsg ("ptr: 0x%x\n", (_Vx_usr_arg_t)ptr, 0, 0, 0, 0, 0);

    if (rBuffReadReserve (buffId, &pBuff) != 2)
        {
        logMsg ("Wrong amount of data available at %d\n", 
                (_Vx_usr_arg_t) __LINE__, 0,0, 0, 0, 0);
        taskDelay (60);
        return (ERROR);
        }

    logMsg ("read back from: 0x%x\n", (_Vx_usr_arg_t)pBuff, 0, 0, 0, 0, 0);

    if (rBuffReadCommit (buffId, 2) != OK)
        {
        logMsg ("Failed to write and verify data (1)\n", 0, 0, 0, 0, 0, 0);
        taskDelay (60);
        return (ERROR);
        }


    ptr = rBuffWrite (buffId, 0, 2);
    if (ptr == NULL)
        {
        logMsg ("Failed to write and verify data (2)\n", 0, 0, 0, 0, 0, 0);
        taskDelay (60);
        return (ERROR);
        }
    *ptr = 2;
    *(ptr+1) = 3;

    logMsg ("ptr: 0x%x\n", (_Vx_usr_arg_t)ptr, 0, 0, 0, 0, 0);

    if (rBuffReadReserve (buffId, &pBuff) != 2)
        {
        logMsg ("Failed to write and verify data (3)\n", 0, 0, 0, 0, 0, 0);
        taskDelay (60);
        return (ERROR);
        }


    if (rBuffReadCommit (buffId, 2) != OK)
        {
        logMsg ("Failed to write and verify data (4)\n", 0, 0, 0, 0, 0, 0);
        taskDelay (60);
        return (ERROR);
        }
    return (OK);
    }


STATUS callback1
    (
    BUFFER_ID   buff
    )
    {
    event_t *        eventBase;
    int              value = 0x1234567;

    eventBase = (event_t *) buff->writeRtn (buff,NULL,4);
    if (eventBase == NULL)
        return (ERROR);

    EVT_STORE(eventBase,value);

    return (OK);
    }

STATUS callback2
    (
    BUFFER_ID   buff
    )
    {
    event_t *        eventBase;
    int              value = 0x89abcdef;

    eventBase = (event_t *) buff->writeRtn (buff,NULL,4);
    if (eventBase == NULL)
        return (ERROR);

    EVT_STORE(eventBase,value);

    return (OK);
    }


LOCAL STATUS testDataVerify
    (
    BUFFER_ID       buffId,
    char *          pData,
    int             length
    )
    {
    int             available;
    char *          pResult;
    int             n;

    available = rBuffReadReserve (buffId, &pResult);
    if (available != length)
        {
        logMsg ("Wrong amount of data available, expected %d, saw %d\n",
                (_Vx_usr_arg_t) length, (_Vx_usr_arg_t) available, 0, 0, 0, 0);
        taskDelay (60);
        return (ERROR);
        }

    if (memcmp (pData, pResult, length) != 0)
        {
        logMsg ("Failed to verify data\n", 0, 0, 0, 0, 0, 0);
        for (n=0; n < length; n++)
            {
            logMsg ("Expected: 0x%02x    Got 0x%02x\n",
                    (_Vx_usr_arg_t) *(pData+n), (_Vx_usr_arg_t) *(pResult+n), 
                    0, 0, 0, 0);
            }
        taskDelay (60);
        return (ERROR);
        }

    if (rBuffReadCommit (buffId, length) != OK)
        {
        logMsg ("Failed to commit data\n", 0, 0, 0, 0, 0, 0);
        taskDelay (60);
        return (ERROR);
        }
    return (OK);
    }

STATUS rBuffTest10 (void)
    {
    BUFFER_ID           buffId;
    unsigned char *     readPtr;
    int                 n;

    char                expectedData [32];

    wvEvtBufferFullNotify = 0;

    /* preset expected data */

    n = 0x1234567;
    memcpy (expectedData, (char *)&n, sizeof (n));

    n = 0x89abcdef;
    memcpy (expectedData + sizeof (n), (char *)&n, sizeof (n));


    memcpy ((void *)&rBuffParams, &wvDefaultRBuffParams, sizeof (rBuffParams));

    rBuffParams.minimum = 10;
    rBuffParams.maximum = 10;
    rBuffParams.buffSize = 20;
    rBuffParams.threshold = 5;
    rBuffParams.options = 0;

    buffId = rBuffCreate (&rBuffParams);

    if (buffId == NULL)
        {
        logMsg ( "Failed to create rBuff", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    logMsg ("New buffer: 0x%x\n", (_Vx_usr_arg_t)buffId, 0, 0, 0, 0, 0);

    if (buffId->newBuffHookAdd (buffId, callback1) != OK)
        {
        logMsg ("Error attaching hook (1)\n", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    if (buffId->newBuffHookAdd (buffId, callback2) != OK)
        {
        logMsg ("Error attaching hook (2)\n", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    ptr = rBuffWrite (buffId, 0, 2);
    if (ptr == NULL)
        {
        logMsg ("Failed to write data %d\n", n, 0, 0, 0, 0, 0);
        taskDelay (60);
        return (ERROR);
        }

    *ptr = 0;
    *(ptr+1) = 1;

    expectedData [8] = 0;
    expectedData [9] = 1;

    /* expect to see values written by two callbacks, plus 2 bytes */

    if (testDataVerify (buffId, expectedData, sizeof (int) * 2 + 2 ) != OK)
        {
        taskDelay (60);
        return (ERROR);
        }

    /* now try again. Should see the callbacks data again */

    ptr = rBuffWrite (buffId, 0, 2);
    if (ptr == NULL)
        {
        logMsg ("Failed to write data %d\n", (_Vx_usr_arg_t) n, 0, 0, 0, 0, 0);
        taskDelay (60);
        return (ERROR);
        }

    *ptr = 2;
    *(ptr+1) = 3;

    expectedData [8] = 2;
    expectedData [9] = 3;

    /* expect to see values written by two callbacks, plus 2 bytes */

    if (testDataVerify (buffId, expectedData, sizeof (int) * 2 + 2 ) != OK)
        {
        taskDelay (60);
        return (ERROR);
        }

    rBuffDestroy (buffId);

    return (OK);
    }

#endif

