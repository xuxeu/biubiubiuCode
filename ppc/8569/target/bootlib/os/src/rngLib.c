/* rngLib.c - ring buffer subroutine library */

/* 
 * Copyright (c) 1984-1993, 1995, 2004, 2009-2010 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */ 

/*
modification history
--------------------
02e,24feb10,pch  small footprint
02d,27apr09,jpb  Changed variable type from size_t to long for situations 
		 when variable could be negative.
02c,14apr09,jpb  Updated for LP64 support.
02b,14dec04,aeg  added include of strings.h (SPR #105335).
02a,22oct04,aeg  removed duplicate function rngCreateKernel() (SPR #102956).
01z,26may04,pad  Replaced usage of KHEAP_ALLOC and KHEAP_FREE macros with
		 calls to malloc() .
01y,20jan04,dat  Adding rngCreateKernel for SPR 88437
01x,13feb95,jdi  doc tweaks.
01w,20jan93,jdi  documentation cleanup for 5.1.
01v,26may92,rrr  the tree shuffle
01u,11jan92,wmd  fix bug spr #1210, race condition based on updates of buf
---		 -pointers in more than 1 place in rngBufPut() and rngBufGet().
01t,13dec91,gae  ANSI cleanup.
01s,04oct91,rrr  passed through the ansification filter
---		 -changed functions to ansi style
---		 -changed VOID to void
---		 -changed copyright notice
01r,05apr91,jdi	 documentation -- removed header parens and x-ref numbers;
---		 -doc review by dnw.
01q,11mar91,jaa	 documentation cleanup.
01p,11may90,yao  added missing modification history (01o) for the last checkin.
01o,09may90,yao  typecasted malloc to (char *).
01n,22mar89,dab  changed shorts to ints in rngBufGet, rngBufPut.
01m,03feb89,dab  added rngDelete to delete a ring buffer.
01l,20aug88,gae  documentation.
01k,07jul88,jcf  fixed malloc to match new declaration.
01j,30may88,dnw  changed to v4 names.
01i,03nov87,ecs  documentation.
01h,20oct87,gae  documentation.
01g,23mar87,jlf  documentation.
01f,21dec86,dnw  changed to not get include files from default directories.
01e,14apr86,rdc  changed memAllocates to mallocs.
01d,20jul85,jlf  documentation.
01c,09sep84,jlf  added comments and copyright.
01b,15aug84,dnw  changed rngBufCreate to allocate ring buffer one byte larger
---		 -than requested, because ring buffer algorithm always leaves
---		 -at least one empty byte in buffer.
---		 -added rngEmpty, rngFull, rngFreeBytes, rngNBytes, rngFlush,
---		 -rngPutAhead, rngMoveAhead, some of which used to be
---		 -macros in rngLib.h.
---		 -changed rngBufGet,rngBufPut to rngGetBuf,rngPutBuf.
---		 -changed rngBufCreate to rngCreate.
01a,06jun84,dnw  culled from old drvLib.c
*/

/*
DESCRIPTION
This library provides routines for creating and using ring buffers, which
are first-in-first-out circular buffers.  The routines simply manipulate
the ring buffer data structure; no kernel functions are invoked.  In
particular, ring buffers by themselves provide no task synchronization or
mutual exclusion.

However, the ring buffer pointers are manipulated in such a way that a
reader task (invoking rngBufGet()) and a writer task (invoking
rngBufPut()) can access a ring simultaneously without requiring mutual
exclusion.  This is because readers only affect a \f2read\f1 pointer and
writers only affect a \f2write\f1 pointer in a ring buffer data structure.
However, access by multiple readers or writers \f2must\fP be interlocked
through a mutual exclusion mechanism (i.e., a mutual-exclusion semaphore
guarding a ring buffer).


This library also supplies two macros, RNG_ELEM_PUT and RNG_ELEM_GET,
for putting and getting single bytes from a ring buffer.  They are defined
in rngLib.h.
.CS
    size_t RNG_ELEM_GET (ringId, pch, fromP)
    size_t RNG_ELEM_PUT (ringId, ch, toP)
.CE
Both macros require a temporary variable <fromP> or <toP>, which
should be declared as `register int' for maximum efficiency.  RNG_ELEM_GET
returns 1 if there was a character available in the buffer; it returns 0
otherwise.  RNG_ELEM_PUT returns 1 if there was room in the buffer; it returns
0 otherwise.  These are somewhat faster than rngBufPut() and rngBufGet(),
which can put and get multi-byte buffers.

INCLUDE FILES: rngLib.h
*/

/* LINTLIBRARY */

#include <vxWorks.h>
#include <memLib.h>
#include <memPartLib.h>
#include <rngLib.h>
#include <stdlib.h>
#ifdef	_WRS_KERNEL
#include <string.h>
#else	/* _WRS_KERNEL */
#include <strings.h>
#endif	/* _WRS_KERNEL */

/*******************************************************************************
*
* rngCreate - create an empty ring buffer
*
* This routine creates a ring buffer of size <nbytes>, and initializes
* it.  Memory for the buffer is allocated from the system memory partition.
*
* RETURNS
* The ID of the ring buffer, or NULL if memory cannot be allocated.
*
* ERRNO: N/A.
*/

RING_ID rngCreate
    (
    size_t nbytes          /* number of bytes in ring buffer */
    )
    {
    char *buffer;
#if defined(_WRS_CONFIG_MEM_ALLOT) && defined(_WRS_KERNEL)
    RING_ID ringId = (RING_ID) memAllot (sizeof (RING),
					 MAX(sizeof (size_t), sizeof(char *)));
#else	/* _WRS_CONFIG_MEM_ALLOT && _WRS_KERNEL */
    RING_ID ringId = (RING_ID) malloc (sizeof (RING));
#endif	/* _WRS_CONFIG_MEM_ALLOT && _WRS_KERNEL */

    if (ringId == NULL)
	return (NULL);

    /*
     * bump number of bytes requested because ring buffer algorithm
     * always leaves at least one empty byte in buffer 
     */

#if defined(_WRS_CONFIG_MEM_ALLOT) && defined(_WRS_KERNEL)
    buffer = (char *) memAllot (++nbytes, sizeof(char));
#else	/* _WRS_CONFIG_MEM_ALLOT && _WRS_KERNEL */
    buffer = (char *) malloc (++nbytes);
#endif	/* _WRS_CONFIG_MEM_ALLOT && _WRS_KERNEL */

    if (buffer == NULL)
	{
#if !(defined(_WRS_CONFIG_MEM_ALLOT) && defined(_WRS_KERNEL))
	free ((char *)ringId);
#endif	/* !(_WRS_CONFIG_MEM_ALLOT && _WRS_KERNEL) */
	return (NULL);
	}

    ringId->bufSize = nbytes;
    ringId->buf	    = buffer;

    rngFlush (ringId);

    return (ringId);
    }


#if !(defined(_WRS_CONFIG_MEM_ALLOT) && defined(_WRS_KERNEL))
/*******************************************************************************
*
* rngDelete - delete a ring buffer
*
* This routine deletes a specified ring buffer.
* Any data currently in the buffer will be lost.
*
* RETURNS: N/A
*
* ERRNO: N/A.
*/

void rngDelete
    (
    FAST RING_ID ringId         /* ring buffer to delete */
    )
    {
    free (ringId->buf);
    free ((char *)ringId);
    }
#endif	/* !(_WRS_CONFIG_MEM_ALLOT && _WRS_KERNEL) */


/*******************************************************************************
*
* rngFlush - make a ring buffer empty
*
* This routine initializes a specified ring buffer to be empty.
* Any data currently in the buffer will be lost.
*
* RETURNS: N/A
*
* ERRNO: N/A.
*/

void rngFlush
    (
    FAST RING_ID ringId         /* ring buffer to initialize */
    )
    {
    ringId->pToBuf   = 0;
    ringId->pFromBuf = 0;
    }


/*******************************************************************************
*
* rngBufGet - get characters from a ring buffer
*
* This routine copies bytes from the ring buffer <rngId> into <buffer>.
* It copies as many bytes as are available in the ring, up to <maxbytes>.
* The bytes copied will be removed from the ring.
*
* RETURNS:
* The number of bytes actually received from the ring buffer;
* it may be zero if the ring buffer is empty at the time of the call.
*
* ERRNO: N/A.
*/

size_t rngBufGet
    (
    FAST RING_ID rngId,         /* ring buffer to get data from      */
    char *buffer,               /* pointer to buffer to receive data */
    size_t maxbytes             /* maximum number of bytes to get    */
    )
    {
    size_t bytesgot;
    size_t pToBuf = rngId->pToBuf;
    size_t bytes2;
    size_t pRngTmp;

    if (pToBuf >= rngId->pFromBuf)
	{
	/* pToBuf has not wrapped around */

	bytesgot = min (maxbytes, pToBuf - rngId->pFromBuf);
	bcopy (&rngId->buf [rngId->pFromBuf], buffer, bytesgot);
	rngId->pFromBuf += bytesgot;
	}
    else
	{
	/* pToBuf has wrapped around.  Grab chars up to the end of the
	 * buffer, then wrap around if we need to. */

	bytesgot = min (maxbytes, rngId->bufSize - rngId->pFromBuf);
	bcopy (&rngId->buf [rngId->pFromBuf], buffer, bytesgot);
	pRngTmp = rngId->pFromBuf + bytesgot;

	/* If pFromBuf is equal to bufSize, we've read the entire buffer,
	 * and need to wrap now.  If bytesgot < maxbytes, copy some more chars
	 * in now. */

	if (pRngTmp == rngId->bufSize)
	    {
	    bytes2 = min (maxbytes - bytesgot, pToBuf);
	    bcopy (rngId->buf, buffer + bytesgot, bytes2);
	    rngId->pFromBuf = bytes2;
	    bytesgot += bytes2;
	    }
	else
	    rngId->pFromBuf = pRngTmp;
	}
    return (bytesgot);
    }


/*******************************************************************************
*
* rngBufPut - put bytes into a ring buffer
*
* This routine puts bytes from <buffer> into ring buffer <ringId>.  The
* specified number of bytes will be put into the ring, up to the number of
* bytes available in the ring.
*
* INTERNAL
* Always leaves at least one byte empty between pToBuf and pFromBuf, to
* eliminate ambiguities which could otherwise occur when the two pointers
* are equal.
*
* RETURNS:
* The number of bytes actually put into the ring buffer;
* it may be less than number requested, even zero,
* if there is insufficient room in the ring buffer at the time of the call.
*
* ERRNO: N/A.
*/

size_t rngBufPut
    (
    FAST RING_ID rngId,         /* ring buffer to put data into  */
    char * buffer,              /* buffer to get data from       */
    size_t nbytes               /* number of bytes to try to put */
    )
    {
    size_t bytesput;
    size_t pFromBuf = rngId->pFromBuf;
    size_t bytes2;
    size_t pRngTmp;

    if (pFromBuf > rngId->pToBuf)
	{
	/* pFromBuf is ahead of pToBuf.  We can fill up to two bytes
	 * before it */

	bytesput = min (nbytes, pFromBuf - rngId->pToBuf - 1);
	bcopy (buffer, &rngId->buf [rngId->pToBuf], bytesput);
	rngId->pToBuf += bytesput;
	}
    else if (pFromBuf == 0)
	{
	/* pFromBuf is at the beginning of the buffer.  We can fill till
	 * the next-to-last element */

	bytesput = min (nbytes, rngId->bufSize - rngId->pToBuf - 1);
	bcopy (buffer, &rngId->buf [rngId->pToBuf], bytesput);
	rngId->pToBuf += bytesput;
	}
    else
	{
	/* pFromBuf has wrapped around, and its not 0, so we can fill
	 * at least to the end of the ring buffer.  Do so, then see if
	 * we need to wrap and put more at the beginning of the buffer. */

	bytesput = min (nbytes, rngId->bufSize - rngId->pToBuf);
	bcopy (buffer, &rngId->buf [rngId->pToBuf], bytesput);
	pRngTmp = rngId->pToBuf + bytesput;

	if (pRngTmp == rngId->bufSize)
	    {
	    /* We need to wrap, and perhaps put some more chars */

	    bytes2 = min (nbytes - bytesput, pFromBuf - 1);
	    bcopy (buffer + bytesput, rngId->buf, bytes2);
	    rngId->pToBuf = bytes2;
	    bytesput += bytes2;
	    }
	else
	    rngId->pToBuf = pRngTmp;
	}
    return (bytesput);
    }


/*******************************************************************************
*
* rngIsEmpty - test if a ring buffer is empty
*
* This routine determines if a specified ring buffer is empty.
*
* RETURNS:
* TRUE if empty, FALSE if not.
*
* ERRNO: N/A.
*/

BOOL rngIsEmpty
    (
    RING_ID ringId      /* ring buffer to test */
    )
    {
    return (ringId->pToBuf == ringId->pFromBuf);
    }


/*******************************************************************************
*
* rngIsFull - test if a ring buffer is full (no more room)
*
* This routine determines if a specified ring buffer is completely full.
*
* RETURNS:
* TRUE if full, FALSE if not.
*
* ERRNO: N/A.
*/

BOOL rngIsFull
    (
    FAST RING_ID ringId         /* ring buffer to test */
    )
    {
    size_t n = ringId->pToBuf - ringId->pFromBuf + 1;

    return ((n == 0) || (n == ringId->bufSize));
    }


/*******************************************************************************
*
* rngFreeBytes - determine the number of free bytes in a ring buffer
*
* This routine determines the number of bytes currently unused in a specified
* ring buffer.
*
* RETURNS: The number of unused bytes in the ring buffer.
*
* ERRNO: N/A.
*/

size_t rngFreeBytes
    (
    FAST RING_ID ringId         /* ring buffer to examine */
    )
    {
    FAST long n = ringId->pFromBuf - ringId->pToBuf - 1;

    if (n < 0)
	n += ringId->bufSize;

    return (n);
    }


/*******************************************************************************
*
* rngNBytes - determine the number of bytes in a ring buffer
*
* This routine determines the number of bytes currently in a specified
* ring buffer.
*
* RETURNS: The number of bytes filled in the ring buffer.
*
* ERRNO: N/A.
*/

size_t rngNBytes
    (
    FAST RING_ID ringId         /* ring buffer to be enumerated */
    )
    {
    FAST long n = ringId->pToBuf - ringId->pFromBuf;

    if (n < 0)
	n += ringId->bufSize;

    return (n);
    }


/*******************************************************************************
*
* rngPutAhead - put a byte ahead in a ring buffer without moving ring pointers
*
* This routine writes a byte into the ring, but does not move the ring buffer
* pointers.  Thus the byte will not yet be available to rngBufGet() calls.
* The byte is written <offset> bytes ahead of the next input location in the
* ring.  Thus, an offset of 0 puts the byte in the same position as would
* RNG_ELEM_PUT would put a byte, except that the input pointer is not updated.
*
* Bytes written ahead in the ring buffer with this routine can be made available
* all at once by subsequently moving the ring buffer pointers with the routine
* rngMoveAhead().
*
* Before calling rngPutAhead(), the caller must verify that at least
* <offset> + 1 bytes are available in the ring buffer.
*
* RETURNS: N/A
*
* ERRNO: N/A.
*/

void rngPutAhead
    (
    FAST RING_ID ringId,   /* ring buffer to put byte in    */
    char byte,             /* byte to be put in ring        */
    size_t offset          /* offset beyond next input byte where to put byte */
    )
    {
    FAST size_t n = ringId->pToBuf + offset;

    if (n >= ringId->bufSize)
	n -= ringId->bufSize;

    *(ringId->buf + n) = byte;
    }


/*******************************************************************************
*
* rngMoveAhead - advance a ring pointer by <n> bytes
*
* This routine advances the ring buffer input pointer by <n> bytes.  This makes
* <n> bytes available in the ring buffer, after having been written ahead in
* the ring buffer with rngPutAhead().
*
* RETURNS: N/A
*
* ERRNO: N/A.
*/

void rngMoveAhead
    (
    FAST RING_ID ringId,  /* ring buffer to be advanced                  */
    FAST size_t n         /* number of bytes ahead to move input pointer */
    )
    {
    n += ringId->pToBuf;

    if (n >= ringId->bufSize)
	n -= ringId->bufSize;

    ringId->pToBuf = n;
    }
