/* poolLib.c - Memory Pool Library */

/*
 * Copyright (c) 1998, 2003, 2005-2007, 2009-2011, 2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. 
 */

/*
modification history
--------------------
01w,16apr13,pcs  Fix an issue in function poolPartIdGet on LP64 running in the
                 MAX performance mode. (WIND00389736)
01v,27sep11,pcs  Updated function poolCreate to allocate memory based on the
                 passed partId.
01u,01feb11,cww  Added freeCount to avoid using the slow dllCount routine
		 (WIND00253635)
01t,15sep10,s_s  documentation update
01s,14sep09,zl   added poolInitialize()
01r,08sep09,pcs  Updated to reflect 64bit heap changes.
01q,26aug09,jpb  Defect# 113517.  Fixed documentation error.
01p,01apr09,jpb  Updated for LP64 support.
01p,23jun09,x_s  Added VSB consideration comment and removed _BUILD_POOL_LIB.
01o,13mar07,kk   call poolLibInit for RTP space
01n,26feb07,jmg  Added return OK to poolLibInit
01m,09feb07,jmg  Created poolLibInit() and removed pollLibInitialized
01l,28apr06,dgp  doc: fix fonts, various routines; synopsis poolCreate()
01k,25apr06,pcs  update doc for fn poolIncrementGet SPR#119819
01j,01mar06,zl   documentation fixes (SPR#117098)
01i,18nov05,zl   initialize numTotl of pool.
01h,26aug05,zl   updated documentation for publishing.
01g,12aug05,zl   exclude building in unsupported layers
01f,08aug05,zl   embedded semaphore in POOL structure
01e,27jul05,mil  Added scaled down version for minimal kernel.
01d,27oct03,zl   support for user mode
01c,28aug03,zl   fixed poolBlockIsFree() and poolUnusedBlocksFree(),
                 cleaned up documentation.
01b,20aug03,zl   added item alignment, creation options (significant rewrite).
01a,16feb98,dat, initial version.  TOR 1.0.1/VXW 5.3 compatible.
	    mas,tm
*/

/*
DESCRIPTION
This module contains the Memory Pool library. Pools provide a fast and 
efficient memory management when an aplication uses a large number of 
identically sized memory items (e.g. structures, objects) by minimizing the
number of allocations from a memory partition. The use of pools also reduces
possible fragmentation caused by frequent memory allocation and freeing.

A pool is a dynamic set of statically sized memory items. All items 
in a pool are of the same size, and all are guaranteed a power of two 
alignment. The size and alignemnt of items are specified at pool creation
time. An item can be of arbitrary size, but the actual memory used up by
each item is at least 8 bytes, and it is a multiple of the item alignment.
The minimum alignment of items is the architecture specific allocation 
alignment.

Pools are created and expanded using a specified number of items for 
initial size and another number of items for incremental pool additions.
The initial set of items and the incremental pool items are added as one
block of memory. Each memory block can be allocated from either the
system memory partition (when the partition ID passed to poolCreate() 
is NULL), a user-provided memory partition. A block can be also added
to the pool using any memory specified by the user using poolBlockAdd().
For example, if all items in a pool have to be in some specific memory
zone, the pool can be created with initial and incremental item count 
as zero in order to prevent automatic creation of blocks from memory 
partitions, and explicitly adding blocks with poolBlockAdd() as needed.
The memory provided to the pool must be writable. Allocation and free 
from memory pools are performed using the poolItemGet() and 
poolItemReturn() routines.

If the pool item increment is specified as zero, the pool will be static,
unable to grow dynamically. A static pool is more deterministic.

Pools are intended for use in systems requiring frequent allocating and freeing
of memory in statically sized blocks such as used in messaging systems, data-
bases, and the like.  This pool system is dynamic and grows upon request,
eventually allowing a system to achieve a stable state with no further memory
requests needed.

This library is considered as thread safe as long as the pool is created with
the POOL_THREAD_SAFE option. Otherwise, it is the responsibility of the 
customer to make sure that the library can be accessed concurrently by other 
threads.

\IFSET_START KERNEL
The poolItemGet() and poolItemReturn() functions may be called from interrupt
context as long as the pool was created without the POOL_THREAD_SAFE. Other
routines provided by this library should not be called from an ISR.

Pool system statistics are available for specific pools as well as the overall
pool system. These show routines are available only if INCLUDE_SHOW_ROUTINES
is defined.
\IFSET_END

VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
selected, this library will not be available.

INTERNAL

The internal structure of a pool is as follows:

         Pool 
    +-------------| 
    |  pool name  | 
    |  item size  | 
    |  alignment  | 
    |  increment  | 
    |  FREE LIST  |<----+
    |  BLOCK LIST |<-+  |
    +-------------|  |  |
                     |  |
 +-------------------+  |
 |                      |
 |  +-------------+     |  +-------------+        +-------------+
 +->|    NODE     |<------>|    NODE     |<------>|    NODE     |
    |    block    |---+ |  |    block    |---+    |    block    |---+
    +-------------+   | |  +-------------+   |    +-------------+   |
                      | |                    |                      |
                      | |                    |                      |
        Block_1       | |       Block_2      |        Block_m       |
    +-------------+   | |  +-------------+   |    +-------------+   |
    |    Item_1   |<--+ |  |   Item_n+1  |<--+    |   Item_m+1  |<--+ 
    |             |     |  |             |        |             | 
    +-------------+     |  +-------------+        +-------------+ 
    |             |     +->|     NODE    |<--+    |   Item_m+2  | 
    |    Item_2   |        |             |   |    |             | 
    +-------------+        +-------------+   |    +-------------+ 
    |             |        |             |   |    |             | 
    .             .        .             .   |    .             . 
    |             |        |             |   |    |             | 
    +-------------+        +-------------+   |    +-------------+ 
    |             |        |   Item_m    |   +--->|     NODE    | 
    |    Item_n   |        |             |        |             | 
    +-------------+        +-------------+        +-------------+ 
    
When the SMALL_FOOTPRINT option is selected, the hidden vsb option MEM_ALLOT is 
enabled causing this library to not be available.

INCLUDE FILE: poolLib.h

SEE ALSO: memPartLib
\IFSET_START KERNEL
poolShow
\IFSET_END
*/

#include <vxWorks.h>
#include <vwModNum.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <semLib.h>
#include <errno.h>
#include <memLib.h>
#ifdef _WRS_KERNEL
#include <vsbConfig.h>			/* used for _Vx_usr_arg_t */
#include <intLib.h>
#endif
#include <private/poolLibP.h>
#include <private/memPartLibP.h>
#include <dllLib.h>

/* local variables */

LOCAL SEMAPHORE poolListMutex; /* Mutex used in pool list change */

/* globals */

DL_LIST poolListGbl = {NULL,NULL};  /* global pool list */

/* forward declarations */

LOCAL STATUS poolBlockBuild  (POOL_ID poolId, ULONG itmSize, void * pBlock);
LOCAL BOOL   poolItemIsValid (POOL_ID poolId, void *  pItem);
LOCAL BOOL   poolBlockIsFree (POOL_ID poolId, POOL_BLOCK * pBlk);
LOCAL PART_ID poolPartIdGet (PART_ID partId);

/******************************************************************************
*
* poolLibInit - initialize the poolLib.
*
* This routine creates a semaphore that is used to ensure mutual exclusion
* of DL_LIST poolListGbl[]. 
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available.
*
* RETURNS: OK or ERROR if initialization of the mutex fails.
*
* \NOMANUAL
*
*/

#ifndef _WRS_KERNEL
_WRS_CONSTRUCTOR (poolLibInit, 5)
#else
STATUS poolLibInit (void)
#endif /* !_WRS_KERNEL */
    {
    STATUS status;
    status = semMInit (&poolListMutex, POOL_SEM_CREATE_ATTRIB);

#ifdef _WRS_KERNEL
    return (status);
#else
    return;
#endif /* _WRS_KERNEL */
    }

    
/******************************************************************************
*
* poolCreate - create a pool
*
* This routine creates a pool by allocating an initial block of memory which is
* guarenteed to contain at least <initCnt> items.  The pool will hold items of
* the specified size and alignment only.  The alignment defaults to the 
* architecture specific allocation alignment size, and it must be a power of 
* two value. As items are allocated from the pool, the initial block may be 
* emptied. When a block is emptied and more items are requested, another block 
* of memory is dynamically allocated which is guaranteed to contain <incrCnt> 
* items. If <incrCnt> is zero, no automatic pool expansion is done.
*
* The partition ID parameter can be used to request all item blocks being 
* allocated from a specific memory partition. If this parameter is NULL, the 
* item blocks are allocated from the system memory partition.
*
* POOL OPTIONS
* The options parameter can be used to set the following properties of the 
* pool. Options cannot be changed after the pool has been created. The 
* following options are supported:
*
* \ts
* Option           | Description
* -----------------|---------------------------------------------------
* POOL_THREAD_SAFE | Pool operations are protected with mutex semaphore 
* POOL_CHECK_ITEM  | Items returned to the pool are verified to be valid
* \te
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available. 
*
* RETURNS: ID of pool or NULL if any zero count or size or insufficient
* memory.
*
* ERRNO
* \is
* \i 'S_poolLib_ARG_NOT_VALID'
* one or more invalid input arguments.
* \ie
*
* SEE ALSO: poolDelete()
*/

POOL_ID poolCreate
    (
    const char * pName,		/* optional name to assign to pool */
    size_t	 itmSize,	/* size in bytes of a pool item */
    size_t	 alignment,	/* alignment of a pool item */
				/* (must be power of 2, or 0) */
    ULONG	 initCnt,	/* initial number of items to put in pool */
    ULONG	 incrCnt,	/* min no of items to add to pool dynamically */
				/* (if 0, no pool expansion is done) */
    PART_ID	 partId,	/* memory partition ID */
    ULONG	 options	/* initial options for pool */
    )
    {
    FAST POOL_ID poolId;
    char *	 pPool;
    PART_ID      poolPartId;

    if (alignment == 0)
	alignment = _ALLOC_ALIGN_SIZE;

    /* Validate input args */

    if ((itmSize == 0) || ((alignment & (alignment - 1)) != 0))
	{
	errno = S_poolLib_ARG_NOT_VALID;
	return (NULL);
	}

    /* Determine what partition id to use. */

    poolPartId = poolPartIdGet(partId);

    if ((pPool = memPartAlloc (poolPartId, sizeof (POOL))) == NULL)
            return (NULL);

    poolId = poolInitialize (pPool, pName, itmSize, alignment,
			     initCnt, incrCnt, poolPartId, options);

    if (poolId == NULL)
	memPartFree (poolPartId, pPool);

    return (poolId);
    }

/******************************************************************************
*
* poolInitialize - initialize a pool
*
* This routine initializes a pool
*
* RETURNS: ID of pool or NULL if any zero count or size or insufficient
* memory.
*
* ERRNO
* \is
* \i 'S_poolLib_ARG_NOT_VALID'
* one or more invalid input arguments.
* \ie
*
* \NOMANUAL
*/

POOL_ID poolInitialize
    (
    char *  pPool,	/* pool structure memory */
    const char * pName,	/* optional name to assign to pool */
    size_t  itmSize,	/* size in bytes of a pool item */
    size_t  alignment,	/* alignment of a pool item */
			/* (must be power of 2, or 0) */
    ULONG   initCnt,	/* initial number of items to put in pool */
    ULONG   incrCnt,	/* min no of items to add to pool dynamically */
			/* (if 0, no pool expansion is done) */
    PART_ID partId,	/* memory partition ID */
    ULONG   options	/* initial options for pool */
    )
    {
    POOL_ID poolId = (POOL_ID) pPool;
    POOL_BLOCK * pBlk;

    poolId->sizItem    = itmSize;	/* record item size */
    poolId->alignment  = alignment;	/* record item alignment */
    poolId->options    = options;	/* pool options */
    poolId->numIncr    = incrCnt;	/* pool increment */
    poolId->poolName   = pName;		/* save name */
    poolId->numTotl    = 0;
    poolId->freeCount  = 0;

    /* actual item size must be multiple of alignment, larger than NODE size */

    if (itmSize < sizeof (DL_NODE))
	itmSize = sizeof (DL_NODE);

    poolId->sizItmReal = ROUND_UP (itmSize, alignment);

    poolId->partId = poolPartIdGet (partId);

    /* initialize lists */

    dllInit (&poolId->freeItems);
    dllInit (&poolId->blockList);

    /* mark first static block unused */

    poolId->staticBlock.pItmBlk = NULL;

    /*
     * allocate initial block that has a POOL_BLOCK and a POOL structure 
     * at the end.
     */

    if (initCnt != 0)
	if (poolBlockBuild (poolId, initCnt, NULL) == ERROR)
	    return (NULL);

    /* create a semaphore for this pool if needed */

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
	if ((semMInit (&poolId->mutex, POOL_SEM_CREATE_ATTRIB)) == ERROR)
	    goto poolInit_2;

    /* if pool built successfully, add it to pond */

    if (POOL_LIST_LOCK() != OK)
	goto poolInit_1;

    /* Add this pool to the pool list */

    dllAdd (&poolListGbl, &poolId->poolNode);

    /* unlock access */

    POOL_LIST_UNLOCK();

    return (poolId);

    /* else, free allocated memory before returning NULL */

poolInit_1:
    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
	semTerminate (&poolId->mutex);

poolInit_2:
    if ((pBlk = (POOL_BLOCK *) dllGet (&poolId->blockList)) != NULL)
	{
	memPartFree (poolId->partId, pBlk->pItmBlk);
	}

    return (NULL);
    }

/******************************************************************************
*
* poolDelete - delete a pool
*
* This routine deletes a specified pool and all item blocks allocated for it.
* Memory provided by the user using poolBlockAdd() are not freed.
*
* If the pool is still in use (i.e. not all items have been returned to the 
* pool) deletion can be forced with the <force> parameter set to TRUE.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available.
*
* RETURNS: OK or ERROR if bad pool ID or pool in use.
*
* ERRNO
* \is
* \i 'S_poolLib_INVALID_POOL_ID'
* not a valid pool ID.
* \i 'S_poolLib_POOL_IN_USE'
* can't delete a pool still in use.
* \ie
*
* SEE ALSO: poolCreate()
*/

STATUS poolDelete
    (
    POOL_ID	 poolId,	/* ID of pool to delete */
    BOOL	 force		/* force deletion if there are items in use */
    )
    {
    STATUS	 result = ERROR;
    POOL_BLOCK * pBlk;

    /* verify valid, unused pool ID */

    if (poolId == NULL)
	{
	errno = S_poolLib_INVALID_POOL_ID;
	return (result);
	}

    /* Reserve exclusive access to this pool */

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
	{
	if (POOL_LOCK (poolId) == ERROR)
	    return (result);		/* ERROR */
	}

    if (!force && (poolId->freeCount != poolId->numTotl))
	{
	errno = S_poolLib_POOL_IN_USE;

	if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
	    POOL_UNLOCK (poolId);

	return (result);
	}

    /* remove from global pool list */

    if (POOL_LIST_LOCK() != OK)
	{
	if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
	    POOL_UNLOCK (poolId);

	return (result);		/* ERROR */
	}

    dllRemove (&poolListGbl, &poolId->poolNode);

    POOL_LIST_UNLOCK();

    /* now clean up all allocated stuff */

    result = OK;

    /* free all blocks */

    while ((pBlk = (POOL_BLOCK *) dllGet (&poolId->blockList)) != NULL)
	{
	if (((ULONG) pBlk->pItmBlk & BLOCK_DO_NOT_FREE) == 0)
	    memPartFree (poolId->partId, pBlk->pItmBlk);
	else
	   {
	   if (pBlk != &poolId->staticBlock)
		memPartFree (poolId->partId, (char *) pBlk);
	   }
	}

    /* delete semaphore */

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
        semTerminate (&poolId->mutex);

    /* free the pool structure */

    memPartFree (poolId->partId, (void *)poolId);

    return (result);
    }

/******************************************************************************
*
* poolBlockAdd - add an item block to the pool
*
* This routine adds an item block to the pool using memory provided by the 
* user. The memory provided must be sufficient for at least one properly 
* aligned item.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available.
*
* RETURNS: number of items added, or 0 in case of error
*
* ERRNO
* \is
* \i 'S_poolLib_INVALID_POOL_ID'
* not a valid pool ID.
* \i 'S_poolLib_INVALID_BLK_ADDR'
* pBlock parameter is NULL.
* \i 'S_poolLib_BLOCK_TOO_SMALL'
* size insufficient for at least one item.
* \ie
*
* SEE ALSO: poolCreate()
*/

ULONG poolBlockAdd
    (
    POOL_ID poolId,	/* ID of pool to delete */
    void *  pBlock,	/* base address of block to add */
    size_t  size	/* size of block to add */
    )
    {
    void *  pBlockAligned;
    size_t  itmCnt;

    /* verify valid, unused pool ID */

    if (poolId == NULL)
	{
	errno = S_poolLib_INVALID_POOL_ID;
	return (0);
	}

    if ((pBlock == NULL))
	{
	errno = S_poolLib_INVALID_BLK_ADDR;
	return (0);
	}

    /* do alignment and size adjustments */

    pBlockAligned = (void *) ROUND_UP (pBlock, poolId->alignment);
    itmCnt        = (size - ((char *) pBlockAligned - (char *) pBlock)) / 
		    poolId->sizItmReal;

    if (itmCnt < 1)		/* add at least one item */
	{
	errno = S_poolLib_BLOCK_TOO_SMALL;
	return (0);
	}

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
	if (POOL_LOCK (poolId) == ERROR)
	    return (0);

    if (poolBlockBuild (poolId, itmCnt, pBlockAligned) == ERROR)
	{
	itmCnt = 0;
	}

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
        POOL_UNLOCK (poolId);

    return ((ULONG) itmCnt);
    }

/*******************************************************************************
*
* poolUnusedBlocksFree -  free blocks that have all items unused
*
* This routine allows reducing the memory used by a pool by freeing item blocks
* that have all items returned to the pool. Execution time of this routine
* is not deterministic as it depends on the number of free items and the number
* of blocks in the pool. In case of multi-thread safe pools (POOL_THREAD_SAFE),
* this routine also locks the pool for that time.
*
* Blocks that were added using poolBlockAdd() are not freed by this routine,
* even if all items have been returned; only blocks that were automatically
* allocated during creation or auto-growth from the pool's memory partition
* are freed.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available. 
*
* RETURNS: OK, or ERROR in case of failure
*
* ERRNO
* \is
* \i 'S_poolLib_INVALID_POOL_ID'
* not a valid pool ID.
* \ie
*
* SEE ALSO: poolBlockAdd(), poolCreate()
*/

STATUS poolUnusedBlocksFree
    (
    POOL_ID poolId	/* ID of pool to free blocks */
    )
    {
    POOL_BLOCK * pBlkDel;
    POOL_BLOCK * pBlk;

    /* verify valid pool ID */

    if (poolId == NULL)
	{
	errno = S_poolLib_INVALID_POOL_ID;
	return (ERROR);
	}

    /* Reserve exclusive access to this pool */

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
	if (POOL_LOCK (poolId) == ERROR)
	    return (ERROR);

    /* find and remove any blocks that are all free */

    pBlk = (POOL_BLOCK *) DLL_FIRST (&poolId->blockList);

    while (pBlk != NULL)
	{
	pBlkDel = pBlk;
	pBlk = (POOL_BLOCK *) DLL_NEXT (&pBlk->blockNode);

	if (poolBlockIsFree (poolId, pBlkDel) && 
	    (((ULONG) pBlkDel->pItmBlk & BLOCK_DO_NOT_FREE) == 0))
	    {
	    DL_NODE * pItem;
	    ULONG  ix;
	    ULONG  actualItemSize = poolId->sizItmReal;

	    pItem = (DL_NODE *)((ULONG) pBlkDel->pItmBlk & 
		                (ULONG) ~BLOCK_DO_NOT_FREE);

	    /* delete all items from free list */

	    for (ix = 0; ix < pBlkDel->itemCnt; ix++)
		{
		dllRemove (&poolId->freeItems, pItem);
		poolId->freeCount--;
		pItem = (DL_NODE *) ((char *) pItem + actualItemSize);
		}

	    poolId->numTotl -= pBlkDel->itemCnt;

	    /* delete block from blockList */

	    dllRemove (&poolId->blockList, &pBlkDel->blockNode);

	    /* free memory associated to block */

	    memPartFree (poolId->partId, pBlkDel->pItmBlk);
	    }
	}

    /* exit exlusive access section */

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
        POOL_UNLOCK (poolId);

    return (OK);
    }

/******************************************************************************
*
* poolItemGet - get next free item from pool and return a pointer to it
*
* This routine gets the next free item from the specified pool and returns a
* pointer to it.  If the current block of items is empty, the pool increment 
* count is non-zero, and the routine is called from task context then a new 
* block is allocated of the given incremental size and an item from the new 
* block is returned.
*
* \IFSET_START KERNEL
* In the kernel, this routine can be called from interrupt context if the 
* pool was created without the POOL_THREAD_SAFE option. When called from ISR, 
* the pool will not automatically grow and the routine fails if there are 
* no free items in the pool.
* \IFSET_END
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available.
*
* RETURNS: pointer to item, or NULL in case of error.
*
* ERRNO
* \is
* \i 'S_poolLib_INVALID_POOL_ID'
* not a valid pool ID.
* \i 'S_poolLib_STATIC_POOL_EMPTY'
* no more items available in static pool.
* \IFSET_START KERNEL
* \i 'S_poolLib_INT_CTX_POOL_EMPTY'
* no more items in pool while called from ISR.
* \IFSET_END
* \ie
*
* SEE ALSO: poolItemReturn()
*/

void * poolItemGet
    (
    POOL_ID poolId	/* ID of pool from which to get item */
    )
    {
    BOOL   cont  = TRUE;
    void * pItem = NULL;
    
    /* verify valid pool ID */

    if (poolId == NULL)
	{
	errno = S_poolLib_INVALID_POOL_ID;
	return (NULL);
	}

    /* Reserve exclusive access to this pool */

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
	if (POOL_LOCK (poolId) == ERROR)
	    return (NULL);

    if (DLL_FIRST (&poolId->freeItems) == NULL)
	{
	/*
	 * Current block empty. Allocate another one if not fixed size and not
	 * called from ISR
	 */

	if (poolId->numIncr == 0)
	    {
	    errno = S_poolLib_STATIC_POOL_EMPTY;
	    cont = FALSE;
	    }
#ifdef _WRS_KERNEL
	else if (INT_CONTEXT())
	    {
	    errno = S_poolLib_INT_CTX_POOL_EMPTY;
	    cont = FALSE;
	    }
#endif
	else if (poolBlockBuild (poolId, poolId->numIncr, NULL) == ERROR)
	    {
	    /* keep errno from alloc error */

	    cont = FALSE;
	    }
	}

    if (cont == TRUE)
	{
	pItem = (void *) dllGet (&poolId->freeItems);
	poolId->freeCount--;
	}

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
        POOL_UNLOCK (poolId);

    return (pItem);
    }

/******************************************************************************
*
* poolItemReturn - return an item to the pool
*
* This routine returns the specified item to the specified pool. To enable
* address verification on the item, the pool should be created with the 
* POOL_CHECK_ITEM option. The verification can be an expensive operation, 
* therefore the POOL_CHECK_ITEM option should be used when error detection is
* more important than deterministic behaviour of this routine.
*
* \IFSET_START KERNEL
* In the kernel, this routine can be called from an ISR if the pool was 
* created without the POOL_THREAD_SAFE option.
* \IFSET_END
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available. 
*
* RETURNS: OK, or ERROR in case of failure.
*
* ERRNO
* \is
* \i 'S_poolLib_INVALID_POOL_ID'
* not a valid pool ID.
* \i 'S_poolLib_NOT_POOL_ITEM'
* NULL pointer or item does not belong to pool.
* \i 'S_poolLib_UNUSED_ITEM'
* item is already in pool free list.
* \ie
*
* SEE ALSO: poolItemGet()
*/

STATUS poolItemReturn
    (
    POOL_ID poolId,	/* ID of pool to which to return item */
    void *  pItem	/* pointer to item to return */
    )
    {
    STATUS  result = ERROR;
    BOOL    cont   = TRUE;

    /* verify valid pool ID */

    if (poolId == NULL)
	{
	errno = S_poolLib_INVALID_POOL_ID;
	return (result);
	}

    if (pItem == NULL)
	{
	errno = S_poolLib_NOT_POOL_ITEM;
	return (result);
	}

    /* Reserve exclusive access to this pool */

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
	if (POOL_LOCK (poolId) == ERROR)
	    return (result);

    if ((poolId->options & POOL_CHECK_ITEM) == POOL_CHECK_ITEM)
	{
	/* 
	 * verify that pool belongs to one of the blocks and it is
	 * not in free list.
	 */

	if (poolFindItem (&poolId->freeItems, (DL_NODE *) pItem) == TRUE)
	    {
	    errno = S_poolLib_UNUSED_ITEM;
	    cont  = FALSE;
	    }
	else if (!poolItemIsValid (poolId, pItem))
	    {
	    errno = S_poolLib_NOT_POOL_ITEM;
	    cont = FALSE;
	    }
	}

    if (cont)
	{
	dllAdd (&poolId->freeItems, (DL_NODE *)pItem);
	poolId->freeCount++;
	result = OK;
	}

    /* exit exlusive access section */

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
        POOL_UNLOCK (poolId);

    return (result);
    }

/*******************************************************************************
*
* poolIncrementSet - set the increment value used to grow the pool 
*
* This routine can be used to set the increment value used to grow the pool.
* The increment specifies how many new items are added to the pool when 
* there are no free items left in the pool.
*
* Setting the increment to zero disables automatic growth of the pool.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available.
*
* RETURNS: OK, or ERROR if poolId is invalid
*
* ERRNO
* \is
* \i 'S_poolLib_INVALID_POOL_ID'
* not a valid pool ID.
* \ie
*
* SEE ALSO: poolIncrementGet()
*/

STATUS poolIncrementSet
    (
    POOL_ID poolId,	/* ID of pool */
    ULONG   incrCnt	/* new increment value */
    )
    {
    /* verify valid pool ID */

    if (poolId == NULL)
	{
	errno = S_poolLib_INVALID_POOL_ID;
	return (ERROR);
	}

    poolId->numIncr = incrCnt;
    return (OK);
    }

/*******************************************************************************
*
* poolIncrementGet - get the increment value used to grow the pool
*
* This routine can be used to get the increment value used to grow the pool.
* The increment specifies how many new items are added to the pool when there
* are no free items left in the pool.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available.
*
* RETURNS: increment value, or zero if invalid pool ID.
*
* ERRNO
* \is
* \i 'S_poolLib_INVALID_POOL_ID'
* not a valid pool ID.
* \ie
*
* SEE ALSO: poolIncrementSet()
*/

ULONG poolIncrementGet
    (
    POOL_ID poolId	/* ID of pool */
    )
    {
    /* verify valid pool ID */

    if (poolId == NULL)
	{
	errno = S_poolLib_INVALID_POOL_ID;
	return (0);
	}

    return (poolId->numIncr);
    }

/******************************************************************************
*
* poolTotalCount - return total number of items in pool
*
* This routine returns the total number of items in the specified pool.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available.
*
* RETURNS: number of items, or zero if invalid pool ID.
*
* ERRNO
* \is
* \i 'S_poolLib_INVALID_POOL_ID'
* not a valid pool ID.
* \ie
*
* SEE ALSO: poolFreeCount()
*/

ULONG poolTotalCount
    (
    POOL_ID poolId	/* ID of pool */
    )
    {
    if (poolId == NULL)
	{
	errno = S_poolLib_INVALID_POOL_ID;
	return (0);
	}

    return (poolId->numTotl);
    }

/******************************************************************************
*
* poolFreeCount - return number of free items in pool
*
* This routine returns the number of free items in the specified pool.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available. 
*
* RETURNS: number of items, or zero if invalid pool ID.
*
* ERRNO
* \is
* \i 'S_poolLib_INVALID_POOL_ID'
* not a valid pool ID.
* \ie
*
* SEE ALSO: poolTotalCount()
*/

ULONG poolFreeCount
    (
    POOL_ID poolId	/* ID of pool */
    )
    {
    if (poolId == NULL)
	{
	errno = S_poolLib_INVALID_POOL_ID;
	return (0);
	}

    return poolId->freeCount;
    }

/******************************************************************************
*
* poolIdListGet - get a list of pools in the system
*
* This routine can be used to obtain a list of pool IDs present in the system 
* at the time of the call. If the return value is equal to the listSize 
* parameter the system may have additional pools.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available. 
*
* RETURNS: number of pools in system, or ERROR in case of failure
*
* SEE ALSO: poolShow()
*
* \NOMANUAL
*/

int poolIdListGet
    (
    POOL_ID poolIdList[],
    int     listSize
    )
    {
    POOL_ID poolId;
    int     count = 0;

    if (POOL_LIST_LOCK() != OK)
	return (ERROR);

    poolId = (POOL_ID) DLL_FIRST (&poolListGbl);

    while ((count < listSize) && (poolId != NULL))
	{
	poolIdList[count] = poolId;
	poolId = (POOL_ID) DLL_NEXT (&poolId->poolNode);
	count++;
	}

    POOL_LIST_UNLOCK();

    return (count);
    }


/******************************************************************************
*
* poolBlockBuild - Add a block of items to a specified pool
* 
* This routine adds a block of the given size and number of items to the
* specified pool.  It updates the the free item list.
*
* Note: no error checking is performed on the input arguments.  The specified
* pool is locked during this operation by the calling routine.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available.
*
* RETURNS: OK on success, ERROR if insufficient memory.
*
* \NOMANUAL
*/

LOCAL STATUS poolBlockBuild
    (
    POOL_ID	 poolId,	/* ID of pool to which to add block */
    ULONG	 itemCnt,	/* number of items in block */
    void *	 pUsrBlk
    )
    {
    size_t       actualItemSize = poolId->sizItmReal;
    ULONG	 noFree = 0;
    size_t       blockSize;
    POOL_BLOCK * pBlk;
    int		 ix;

    if (pUsrBlk == NULL)
	{
	/* allocate user block and POOL_BLOCK in one chunk */

	blockSize = actualItemSize * itemCnt + sizeof (POOL_BLOCK);

	pUsrBlk = memPartAlignedAlloc (poolId->partId, blockSize, 
				       poolId->alignment);

	if (pUsrBlk == NULL)
	    return (ERROR);

	/* POOL_BLOCK is at the end of the allocated chunk */

	pBlk = (POOL_BLOCK *) ((char *)pUsrBlk + actualItemSize * itemCnt);
	}
    else
 	{
	/*
	 * first block with user-provided memory will use the embedded
	 * block structure
	 */

	if (poolId->staticBlock.pItmBlk == NULL)
	    pBlk = &poolId->staticBlock;
	else
	    {
	    pBlk = memPartAlloc (poolId->partId, sizeof (POOL_BLOCK));
	    if (pBlk == NULL)
		return (ERROR);
	    }

	/* prevent freeing of this block */

	noFree = BLOCK_DO_NOT_FREE;
	}

    pBlk->pItmBlk = (void *) ((ULONG) pUsrBlk | noFree);
    pBlk->itemCnt = itemCnt;

    for (ix = 0; ix < itemCnt; ix++)
	{
	dllAdd (&poolId->freeItems, (DL_NODE *)pUsrBlk);
	poolId->freeCount++;
	pUsrBlk = (void *) ((char *) pUsrBlk + actualItemSize);
	}

    dllAdd (&poolId->blockList, &pBlk->blockNode);

    poolId->numTotl += itemCnt;

    return (OK);
    }

/*******************************************************************************
*
* poolItemIsValid - check validity of an item
*
* This routine verifies that an item is valid and belongs to a pool.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available. 
*
* RETURNS: TRUE, or FALSE if item is invalid
*
* \NOMANUAL
*/

LOCAL BOOL poolItemIsValid
    (
    POOL_ID	 poolId, 
    void *	 pItem
    )
    {
    POOL_BLOCK * pBlk;
    size_t       actualItemSize = poolId->sizItmReal;

    /* find the block this item belongs to */

    pBlk = (POOL_BLOCK *) DLL_FIRST (&poolId->blockList);

    while (pBlk != NULL)
	{
	char * pItmBlk = (char *) ((ULONG) pBlk->pItmBlk & 
		                   (ULONG) ~BLOCK_DO_NOT_FREE);
	if (((char *) pItem >= (char *) pItmBlk) &&
	    ((char *) pItem < ((char *) pItmBlk + 
			       actualItemSize * pBlk->itemCnt)))
	    {
	    /* pointer within block limits */

	    ULONG offset = ((char *) pItem - pItmBlk);

	    /* make sure offset is multiple of item size */

	    if (((offset / actualItemSize) * actualItemSize) == offset)
		return (TRUE);
	    else
		return (FALSE);
	    }

	pBlk = (POOL_BLOCK *) DLL_NEXT (&pBlk->blockNode);
	}

    /* item not in pool */

    return (FALSE);
    }

/*******************************************************************************
*
* poolBlockIsFree - test if items in a block are free
*
* This routine tests a block to see if all items are in the pool's free list.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available. 
*
* RETURNS: TRUE, or ERROR if poolId is invalid
*
* \NOMANUAL
*/

LOCAL BOOL poolBlockIsFree
    (
    POOL_ID	 poolId,	/* pool the block belongs to */
    POOL_BLOCK * pBlk		/* block to check */
    )
    {
    BOOL	 blkFree = TRUE;
    size_t	 actualItemSize = poolId->sizItmReal;
    void *	 pItem;
    ULONG	 ix;

    /* block is free if all items are in free list */

    pItem = (void *)((ULONG) pBlk->pItmBlk & (ULONG) ~BLOCK_DO_NOT_FREE);

    for (ix = 0; ix < pBlk->itemCnt; ix++)
	{
	if (poolFindItem (&poolId->freeItems, (DL_NODE *) pItem) == FALSE)
	    {
	    blkFree = FALSE;
	    break;
	    }

	pItem = (void *) ((char *) pItem + actualItemSize);
	}

    return (blkFree);
    }

/*******************************************************************************
*
* poolFindItem - support routine to find if item is in DL list.   
* 
* RETURNS: TRUE if item is in the list.  FALSE if not found.
*
* \NOMANUAL
*/

BOOL poolFindItem
    (
    DL_LIST *	   pList,
    DL_NODE *	   pItem
    )
    {
    FAST DL_NODE * pNode = DLL_FIRST (pList);

    while ((pNode != NULL) && (pNode != pItem))
	pNode = DLL_NEXT (pNode);
    
    if (pNode == NULL)
	return (FALSE);
    else
	return (TRUE);
    }

/*******************************************************************************
*
* poolPartIdGet - returns the partition id to use
* 
* This routine returns the partion id to be used for allocating the memory to
* be used to populate the pool.
* If partId is not NULL, then it is returned as is.
* If the 
* In RTP
*   - returns the id  of the heap partition.
* In Kernel (in 64-bit mode)  
*   - if the system partition is not yet initialized this routine returns the 
*     partition id of the proximity heap. Once the system heap is initialized
*     it returns the partition id of the system heap.
* In Kernel (in 32-bit mode)  
*   - returns the partition id of the system heap.
*
* RETURNS: partition id.
*
* \NOMANUAL
*/

LOCAL PART_ID poolPartIdGet
    (
    PART_ID partId       /* partition id */
    )
    {
    PART_ID poolPartId;

    if (partId != NULL)
        {
        /* The user has passed a partition id , use it as the pool's partId. */

        return (partId);
        }

#ifdef _WRS_KERNEL
#ifdef _WRS_CONFIG_LP64
    /*
     * In the kernel in LP64 mode, check if the system heap is initialized.
     * If not, select partition id of the kernel proximity heap as the 
     * default partition id.
     * Note: We cannot use the macro OBJ_VERIFY, since it always returns OK in
     * the MAX performance mode. objVerify does exist in the MAX performance 
     * mode so it is OK to use it here.
     */

    if (objVerify (memSysPartId, memPartClassId) != OK)
        poolPartId = kProxHeapPartId;
    else
#endif
    poolPartId = memSysPartId;
#else
    /* In RTP */
    poolPartId = heapPartId;
#endif

    return (poolPartId);
    }

