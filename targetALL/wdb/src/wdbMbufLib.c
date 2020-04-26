/* wdbMbufLib.c - WDB Mbuf library */

/*
 * Copyright (c) 2003, 2007-2009 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify, or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01e,04jun09,rlp  Fixed unintialized data (WIND00165222).
01d,27aug08,jpb  Renamed VSB header file
01c,18jun08,jpb  Renamed _WRS_VX_SMP to _WRS_CONFIG_SMP.  Added include path 
                 for kernel configurations options set in vsb.
01b,02jul07,rlp  Used its own buffer management.
01a,18mar03,elg  Created from usrWdbCore.c.
*/

/*
DESCRIPTION

This library contains Mbuf initialization and management.
*/

/* includes */

#include <vxWorks.h>
#include <vsbConfig.h>
#include <wdb/wdbMbufLib.h>
#include <wdb/wdbLibP.h>

/* defines */

#define FREE_MAGIC		0x753a012b
#define ALLOC_MAGIC		0x2e63d498

#ifdef	_WRS_CONFIG_SMP
#define	BUF_POOL_LOCK_INIT(pBufPool)				\
	    SPIN_LOCK_ISR_INIT (&pBufPool->lock, 0);
#define	BUF_POOL_LOCK(pBufPool, lockLvl)			\
	    if (!WDB_IS_NOW_EXTERNAL())				\
		SPIN_LOCK_ISR_TAKE (&pBufPool->lock)
#define	BUF_POOL_UNLOCK(pBufPool, lockLvl)			\
	    if (!WDB_IS_NOW_EXTERNAL())				\
		SPIN_LOCK_ISR_GIVE (&pBufPool->lock)
#else	/* _WRS_CONFIG_SMP */
#define	BUF_POOL_LOCK_INIT(pBufPool)
#define	BUF_POOL_LOCK(pBufPool, lockLvl)	lockLvl = INT_CPU_LOCK()
#define	BUF_POOL_UNLOCK(pBufPool, lockLvl)	INT_CPU_UNLOCK(lockLvl)
#endif	/* _WRS_CONFIG_SMP */


/* data types */

typedef struct
    {
    char *		pBuffers;	/* array of buffers */
    int			numBufs;	/* number of buffers in the array */
    int			bufferSize;	/* size of each buffer */
    char *		pFreeBufs;	/* free list */
#ifdef	_WRS_CONFIG_SMP
    spinlockIsr_t	lock;		/* buffer pool lock */
#endif	/* _WRS_CONFIG_SMP */
    } BUF_POOL;

typedef struct
    {
    char *	next;		/* next free buffer */
    int		type;		/* FREE_MAGIC or ALLOC_MAGIC */
    } BUFFER;

/* locals */

LOCAL BUF_POOL	wdbMbufPool;
LOCAL BUF_POOL	wdbClBlkPool;

/* forward declarations */

LOCAL void	wdbBufPoolInit (BUF_POOL * pBufPool, char * pBufs,
					int nBufs, int bufSize);
LOCAL char *	wdbBufAlloc (BUF_POOL * pBufPool);
LOCAL void	wdbBufFree (BUF_POOL * pBufPool, char * pBuf);

/******************************************************************************
*
* wdbBufPoolInit - initialize a buffer pool
*
* This function initializes a pool of buffers
*
* RETURNS: N/A
* 
* /NOMANUAL
*/

LOCAL void wdbBufPoolInit
    (
    BUF_POOL *	pBufPool,
    char *	pBufs,
    int		nBufs,
    int		bufSize
    )
    {
    int ix;

    pBufPool->pBuffers = pBufPool->pFreeBufs = (char *) pBufs;
    pBufPool->numBufs = nBufs;		
    pBufPool->bufferSize = bufSize;

    for (ix = 0; ix < nBufs - 1; ix++)
	{
        ((BUFFER *)(pBufs + (bufSize) * ix))->next =
				(char *) (pBufs + (bufSize) * (ix + 1));
	((BUFFER *)(pBufs + (bufSize) * ix))->type =	FREE_MAGIC;
	}

    ((BUFFER *)(pBufs + (bufSize) * ix))->next = NULL;
    ((BUFFER *)(pBufs + (bufSize) * ix))->type = FREE_MAGIC;

    BUF_POOL_LOCK_INIT(pBufPool)					
    }

/******************************************************************************
*
* wdbBufAlloc - allocate a buffer
*
* This function allocates a buffer from a pool of buffers
*
* RETURNS: a buffer or NULL on error.
* 
* /NOMANUAL
*/

LOCAL char * wdbBufAlloc
    (
    BUF_POOL *	pBufPool
    )
    {
    int		key;				
    char *	pBuf = NULL;

    BUF_POOL_LOCK(pBufPool, key);

    if (pBufPool->pFreeBufs != NULL)
    	{			
	pBuf = pBufPool->pFreeBufs;
	pBufPool->pFreeBufs = ((BUFFER *) pBuf)->next;	
	((BUFFER *) pBuf)->type = ALLOC_MAGIC;	
	((BUFFER *) pBuf)->next = NULL;	
        }			
	
    BUF_POOL_UNLOCK(pBufPool, key);

    return (pBuf);
    }

/******************************************************************************
*
* wdbBufFree - free a buffer
*
* This function frees a buffer
*
* RETURNS: N/A
* 
* /NOMANUAL
*/
LOCAL void wdbBufFree
    (
    BUF_POOL *	pBufPool,
    char *	pBuf
    )
    {
    int	key;						

    BUF_POOL_LOCK(pBufPool, key);		

    if (!((((BUFFER *)pBuf)->type == FREE_MAGIC) ||
	 ((unsigned int)pBuf < (unsigned int) pBufPool->pBuffers) ||
	 ((unsigned int)pBuf >=	(unsigned int) pBufPool->pBuffers +
	 	pBufPool->bufferSize * pBufPool->numBufs) ||	
	 (((unsigned int)pBuf -	(unsigned int) pBufPool->pBuffers)
		% pBufPool->bufferSize != 0)))
	{				
	((BUFFER *) pBuf)->next	= pBufPool->pFreeBufs;
	pBufPool->pFreeBufs	= pBuf;		
	((BUFFER *) pBuf)->type	= FREE_MAGIC;
        }				

    BUF_POOL_UNLOCK(pBufPool, key);
    }

/******************************************************************************
*
* wdbMbufInit - initialize the agent's mbuf memory allocator.
*
* wdbMbufLib manages I/O buffers for the agent since the agent
* can't use malloc().
*
* If the agent is ever hooked up to a network driver that uses standard
* MGET/MFREE for mbuf managment, then the routines wdbMBufAlloc()
* and wdbMBufFree() below should be changed accordingly.
*
* RETURNS: N/A
*
* /NOMANUAL
*/ 

void wdbMbufInit
    (
    struct mbuf *	mbufs,
    UINT32		numMbufs,
    CL_BLK *		wdbClBlks,
    UINT32		numClBlks
    )
    {
    wdbBufPoolInit (&wdbMbufPool, (char *) mbufs,
    					numMbufs, sizeof (struct mbuf));
    wdbBufPoolInit (&wdbClBlkPool, (char *) wdbClBlks,
    					numClBlks, sizeof (CL_BLK));
    }

/******************************************************************************
*
* wdbMbufAlloc - allocate an mbuf
*
* This routine allocates a mbuf.
*
* RETURNS: a pointer to an mbuf, or NULL on error.
*
* /NOMANUAL
*/ 

struct mbuf *	wdbMbufAlloc (void)
    {
    struct mbuf *	pMbuf;
    CL_BLK_ID    	pClBlk;

    pMbuf = (struct mbuf *) wdbBufAlloc (&wdbMbufPool);
    if (pMbuf == NULL)
        return (NULL);

    pClBlk = (CL_BLK_ID) wdbBufAlloc (&wdbClBlkPool);
    if (pClBlk == NULL)
        {
        wdbMbufFree (pMbuf);
        return (NULL);
        }

    pMbuf->m_next       = NULL;
    pMbuf->m_nextpkt    = NULL;
    pMbuf->m_flags      = 0;
    pMbuf->pClBlk       = pClBlk;
    return (pMbuf);
    }

/******************************************************************************
*
* wdbMbufFree - free an mbuf
*
* This routine frees a mbuf.
*
* RETURNS: N/A
*
* /NOMANUAL
*/ 

void wdbMbufFree
    (
    struct mbuf *	pMbuf		/* mbuf chain to free */
    )
    {
    /* if it is a cluster, see if we need to perform a callback */

    if (pMbuf->m_flags & M_EXT)
        {
        if (--(pMbuf->m_extRefCnt) <= 0)
            {
            if (pMbuf->m_extFreeRtn != NULL)
                {
                (*pMbuf->m_extFreeRtn) (pMbuf->m_extArg1, pMbuf->m_extArg2,
					pMbuf->m_extArg3);
                }
            /* free the cluster blk */

            wdbBufFree (&wdbClBlkPool, (char *) pMbuf->pClBlk);
            }
        }

    wdbBufFree (&wdbMbufPool, (char *) pMbuf);
    }
