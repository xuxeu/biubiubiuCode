/* poolShow.c - Wind Memory Pool Show Library */

/*
 * Copyright (c) 2003-2005, 2007, 2009-2011 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. 
 */

/*
modification history
--------------------
01k,01feb11,cww  Added freeCount to pool structure
01j,15sep10,s_s  Removed TASK_LOCK/UNLOCK from poolShow() (CQ: WIND00113516)
01i,21aug09,jpb  Updated for LP64 support.
01h,11apr09,jpb  Replaced lstLib calls with dllLib calls.
01q,30jul09,jpb  Fixed build warnings.
01p,23jun09,x_s  Added VSB consideration comment.
01g,07feb07,jmg  Removed unreferenced extern poolListGbl.
01f,26aug05,zl   prepared documentation for publishing.
01e,12aug05,zl   exclude building in unsupported layers
01d,08aug05,zl   undo previous change, updated copyright.
01c,27jul05,mil  Scaled down for minimal kernel changes.
01b,29aug03,zl   added block info to poolShow()
01a,25aug03,zl   written.
*/

/*
DESCRIPTION
This library provides routines which display information about memory pools
in the system, and detailed statistics about individual memeory pools.

To include this library, select the INCLUDE_POOL_SHOW component.

VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
selected, this library will not be available.

INTERNAL

When the SMALL_FOOTPRINT option is selected, the hidden vsb option MEM_ALLOT is 
enabled causing this library to not be available.

INCLUDE FILES: none

SEE ALSO: poolLib
*/

#include <vxWorks.h>
#include <vsbConfig.h>  	/* for _Vx_usr_arg_t */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <semLib.h>
#include <memLib.h>
#include <taskLib.h>
#include <private/poolLibP.h>

/* definitions */

#define POOL_MAX_OPTION		2
#define POOL_OPTION_MASK	((POOL_MAX_OPTION << 1) - 1)
#define MAX_DISP_LIST		128

/* typedefs */

typedef struct block_info
    {
    ULONG	address;
    ULONG	itmCount;
    ULONG	itmFree;
    } BLOCK_INFO;

/* externals */

/* local variables */

LOCAL char * optionStrings [POOL_MAX_OPTION << 1] = 
    {
    "-",				/* 0 */
    "THREAD_SAFE",			/* 1 */
    "CHECK_ITEM",			/* 2 */
    "THREAD_SAFE CHECK_ITEM"		/* 3 */
    };

/* forward declarations */

LOCAL void  poolShowAll		  (void);
LOCAL ULONG poolBlockFreeCountGet (POOL_ID poolId, POOL_BLOCK * pBlk);


/******************************************************************************
*
* poolShow - display pool information
*
* This show routine displays information about a pool. If level is 1, it also
* displays statistics about memory usage efficiency by the pool. Some count
* values and statistics typically change dynamically, so the displayed 
* values represent a snapshot of the pool status at the time of querying.
* If the pool is not created with the POOL_THREAD_SAFE option, then the 
* poolShow() output might get corrupted. 
*
* If the pool ID passed to this routine is NULL, a summary of all pools managed
* by poolLib is displayed (up to 128 pools). The following is an example 
* for a summary info:
*
* EXAMPLE
* \cs
* 
*   -> poolShow
* 
*    NAME                 POOL ID     SIZE    TOTAL     FREE
*   -------------------- ---------- -------- -------- --------
*   fdEntries            0x02439ef0       80      450       44
*   sets                 0x02439d00       84       72        7
*   set_nodes            0x02439a60       12      288       31
*   mmuPgTables          0x02438f60     4096     1647        3
*   memEdrPool           0x02338d20       32   294913    26973
*
* \ce
*
* The following is an example for a detailed info for a specific pool, 
* with info level 1:
*
* EXAMPLE
* \cs
* 
*   -> poolShow 0x02438f60, 1
*   
*   Pool        : mmuPgTables
*   Item Size   : 4096
*   Alignment   : 0x1000
*   Increment   : 8
*   Total items : 1647
*   Free items  : 3
*   Options     : THREAD_SAFE
*   Blocks      : 2
*   Overhead    : 204 bytes (0%)
*   
*       BLOCK ADDR    ITEMS     FREE
*       ----------  --------  --------
*       0x024ea000         8         3
*       0x0243a000       175         0
*
* \ce
*
* If the pool ID passed to this routine is NULL, a summary of all pools managed
* by poolLib is displayed (up to 128 pools).
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available. 
*
* ERRNO: none
*
* RETURNS: N/A
*
* SEE ALSO: poolLib
*/

void poolShow
    (
    POOL_ID	 poolId,	/* ID of pool from which to get item */
    int		 level		/* display info level */
    )
    {
    int		 ix = 0;
    ULONG	 overhead;
    ULONG	 numFree;
    ULONG	 numBlks;
    char *	 strOptions;
    BLOCK_INFO   blkInfo[MAX_DISP_LIST];

    /* verify valid pool ID */

    if (poolId == NULL)
	{
	poolShowAll();
	return;
	}

    /* get basic info for pool */

    numFree    = poolId->freeCount;
    strOptions = optionStrings [(poolId->options) & POOL_OPTION_MASK];
    numBlks    = (ULONG) dllCount (&poolId->blockList);

    /* calculate memory usage overhead */

    overhead = sizeof (POOL) + 
	       poolId->numTotl * (poolId->sizItmReal - poolId->sizItem) +
	       sizeof (POOL_BLOCK) * numBlks;

    /* print basic info */

    printf ("\nPool        : %s\n"
	    "Item Size   : %d\n"
	    "Alignment   : 0x%x\n"
	    "Increment   : %d\n"
	    "Total items : %d\n"
	    "Free items  : %d\n"
	    "Options     : %s\n"
	    "Blocks      : %d\n"
	    "Overhead    : %d bytes (%d%%)\n",
	    poolId->poolName, poolId->sizItem, poolId->alignment,
	    poolId->numIncr,  poolId->numTotl, 
	    numFree, strOptions, numBlks,
	    overhead, 100 * overhead / poolId->numTotl / poolId->sizItem);

    if (level == 0)
	return;

    /* block information */

    /* populate blkInfo by going through the block list */

    if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
        {
    	if (POOL_LOCK (poolId) == ERROR)
	    return;
	}    

    if (dllCount (&poolId->blockList) > 0)
	{
	POOL_BLOCK * pBlk = (POOL_BLOCK *) DLL_FIRST (&poolId->blockList);

	while ((pBlk != NULL) && (ix < MAX_DISP_LIST))
	    {
	    blkInfo[ix].address  = (ULONG) pBlk->pItmBlk & ~BLOCK_DO_NOT_FREE;
	    blkInfo[ix].itmCount = (ULONG) pBlk->itemCnt;
	    blkInfo[ix].itmFree  = poolBlockFreeCountGet (poolId, pBlk);
	    pBlk = (POOL_BLOCK *) DLL_NEXT ((DL_NODE *) pBlk);
	    ix++;
	    }
	
	if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
	    POOL_UNLOCK (poolId);

#ifdef _WRS_CONFIG_LP64
	printf ("\n        BLOCK ADDR        ITEMS     FREE"
		"\n    ------------------  --------  --------\n");
#else
	printf ("\n    BLOCK ADDR    ITEMS     FREE"
		"\n    ----------  --------  --------\n");
#endif /* _WRS_CONFIG_LP64 */
	for (ix-- ; ix >= 0; ix--)
#ifdef _WRS_CONFIG_LP64
	    printf ("    0x%016x  %8d  %8d\n", blkInfo[ix].address,
#else
	    printf ("    0x%08x  %8d  %8d\n", blkInfo[ix].address,
#endif /* _WRS_CONFIG_LP64 */
		    blkInfo[ix].itmCount, blkInfo[ix].itmFree);
	}
    else
	{
    	if ((poolId->options & POOL_THREAD_SAFE) == POOL_THREAD_SAFE)
    	    POOL_UNLOCK (poolId);
	}
    }

/*******************************************************************************
*
* poolShowAll - show summary about all pools in the system
*
* This internal show routine is called when poolShow() is invoked with NULL
* pool ID. It displays summary information about all pools in the system.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available. 
*
* RETURNS: N/A
*
* \NOMANUAL
*/

LOCAL void poolShowAll (void)
    {
    POOL_ID poolIdList [MAX_DISP_LIST];
    int     ix;

    if ((ix = poolIdListGet (poolIdList, NELEMENTS (poolIdList))) == 0)
	printf ("\nThere are no pools in the system\n");
    else
#ifdef _WRS_CONFIG_LP64
        printf ("\n NAME                     POOL ID         SIZE    TOTAL     "
                "FREE"
                "\n-------------------- ------------------ -------- -------- "
                "--------"
#else
	printf ("\n NAME                 POOL ID     SIZE    TOTAL     FREE"
		"\n-------------------- ---------- -------- -------- --------"
#endif /* _WRS_CONFIG_LP64 */
		"\n");

    for (ix-- ; ix >= 0; ix--)
	{
	FAST POOL_ID poolId = poolIdList [ix];

#ifdef _WRS_CONFIG_LP64
	printf ("%-20s 0x%016x %8d %8d %8d\n",
#else
	printf ("%-20s 0x%08x %8d %8d %8d\n",
#endif  /* _WRS_CONFIG_LP64 */

		poolId->poolName, (ULONG) poolId, poolId->sizItem, 
		poolId->numTotl, poolId->freeCount);
	}
    }

/*******************************************************************************
*
* poolBlockFreeCountGet - get the number of free items in a block
*
* This internal routine used to get the number of free items in a block.
*
* VXWORKS SOURCE BUILD PROJECT CONSIDERATIONS
* When using VxWorks source build projects where the SMALL_FOOTPRINT option is 
* selected, this route will not be available. 
*
* RETURNS: number of free items in block
*
* \NOMANUAL
*/

LOCAL ULONG poolBlockFreeCountGet 
    (
    POOL_ID 	 poolId, 
    POOL_BLOCK * pBlk
    )
    {
    ULONG	 blksFree = 0;
    ULONG	 actualItemSize = poolId->sizItmReal;
    void *	 pItem;
    ULONG	 ix;

    /* count items in the free list */

    pItem = (void *)((ULONG) pBlk->pItmBlk & ~BLOCK_DO_NOT_FREE);

    for (ix = 0; ix < pBlk->itemCnt; ix++)
	{
	if (poolFindItem (&poolId->freeItems, (DL_NODE *) pItem) == TRUE)
	    {
	    blksFree++;
	    }
	pItem = (void *) ((char *) pItem + actualItemSize);
	}

    return (blksFree);
    }
