/* bLib.c - buffer manipulation library */

/*
 * Copyright (c) 1984-2007, 2009, 2011 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
03l,01feb11,jpb  Defect WIND00253474. Fixed build warning.
03k,09dec09,pcs  Pull in fixes that Dan did to resolve bfill issues when
                 _WRS_BLIB_ALIGNMENT is set a value other than 1.
03j,22jan09,pcs  Updated to add support for LP64 data model.
03i,16oct07,kk   fix CQ:WIND00037599, update include header info
03h,03may07,zl   updated portable library build configuration.
03g,03jul06,jmt  Fix type in define
03f,30jun06,agf  enable optimized bLib for Pentium only (CQ:WIND00044910)
03f,06apr06,dgp  doc: fix SEE ALSO, library; ERRNO, bcmp(); synopsis, bswap(),
		 binvert(), bzero(), index(), rindex()
03e,29sep05,gls  updated documentation (SPR #88038)
03d,04nov04,job  Docs update
03c,06jan04,vik  SPR#91504: bcopy works incorrectly
03b,06dec03,mcm  Included strings.h in user mode - for prototypes of bfill etc.
03a,09may03,pes  PAL conditional compilation cleanup. Phase 2.
02z,11nov01,dee  Add CPU_FAMILY==COLDFIRE
02y,01dec98,cdp  make ARM CPUs with ARM_THUMB==TRUE use portable routines;
	    jpd  improve swab for ARM.
02y,03mar00,zl   merged SH support from T1
02x,22apr97,jpd  added support for ARM optimised routines.
02w,19mar95,dvs  removing tron references.
02v,06mar95,jdi  doc addition to swab() and uswab().
02u,11feb95,jdi  doc tweak.
02u,21mar95,caf  added PPC support.
02t,20jul94,dvs  fixed documentation for return values of bcmp (SPR #2493).
02s,09jun93,hdn  added a support for I80X86
02r,17oct94,rhp  delete obsolete doc references to strLib, spr#3712	
02q,20jan93,jdi  documentation cleanup for 5.1.
02p,14sep92,smb  changed prototype of bcopy to include const.
02o,23aug92,jcf  fixed uswab to swap all of buffer.  removed memcmp call.
02n,30jul92,jwt  fixed error in bcopy() backwards overrunning dest address.
02m,08jul92,smb  removed string library ANSI routines.  added index and rindex.
02l,08jun92,ajm  added mips to list of optimized cpu's
02k,26may92,rrr  the tree shuffle
02j,09dec91,rrr  removed memcpy for 960, already in i960/bALib.s
02i,26nov91,llk  fixed error with placement of bLib_PORTABLE.
02h,25nov91,llk  changed bfillBytes() parameter from unsigned char to int.
		 added ansi routines:  memcpy(), memmove(), memcmp(),
		   strcoll(), strxfrm(), strchr(), strcspn(), strpbrk(),
		   strrchr(), strspn(), strstr(), strtok(), memset()
02g,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
02f,30jul91,hdn  added conditional macro for optimized TRON codes.
02e,29jul91,del  fixed comment.
02d,16jul91,ajm/del  added uswab for unaligned byte swaps (see dosFsLib.c)
02c,09jun91,del  mods to allow using gnu960 library functions.
02b,26apr91,hdn  added defines and macros for TRON architecture,
		 modified bcopy(), bcopyBytes(), bfill().
02a,05apr91,jdi	 documentation -- removed header parens and x-ref numbers;
		 doc review by dnw.
01z,24mar91,del  added I960 to check for portable version.
01y,24jan91,jaa	 documentation.
01x,20dec90,gae  added forward declaration of bfill, ALIGNMENT set to 1 for 020.
           +shl  fixed bug in bfill of (occasionally) filling too far.
01w,20sep90,dab  changed ALIGNMENT value to be 3 across architectures.
01v,19jul90,dnw  fixed mangen problem
01u,10may90,jcf  fixed PORTABLE definition.
01t,14mar90,jdi  documentation cleanup.
01s,07aug89,gae  added C versions of bcopy,bfill,etc.
01r,30aug88,gae  more documentation tweaks.
01q,20aug88,gae  documentation.
01p,05jun88,dnw  changed from bufLib to bLib.
01o,30may88,dnw  changed to v4 names.
01n,19mar87,gae  oops in 01m!
01m,19mar87,gae  fixed swab to work right when from/to are the same.
		 made swab type void.
01l,16feb87,llk  added swab().
01k,21dec86,dnw  changed to not get include files from default directories.
01j,01jul86,jlf  minor documentation cleanup.
01i,06aug85,jlf  removed cpybuf,movbuf, and filbuf, which are now in
		 asm language.  Made the remaining routines accept ints instead
		 of shorts, and removed the screwy loop constructs.
01h,06sep84,jlf  Added copyright notice.
01g,29jun84,ecs  changed cmpbuf to return -1, 0, 1.
01f,18jun84,dnw  Added LINTLIBRARY for lint.
01e,17nov83,jlf  Added movbuf
01d,09jun83,ecs  added some commentary
01c,07may83,dnw  enabled filbuf to fill more than 64K bytes
01b,04apr83,dnw  added invbuf.
01a,15mar83,dnw  created from old utillb.c
*/

/*
DESCRIPTION
This library contains routines to manipulate buffers of variable-length
byte arrays.  Operations are performed on long words when possible, even
though the buffer lengths are specified in bytes.  This occurs only when
source and destination buffers start on addresses that are both odd or both
even.  If one buffer is even and the other is odd, operations must be done
one byte at a time, thereby slowing down the process.

Certain applications, such as byte-wide memory-mapped peripherals, may
require that only byte operations be performed.  For this purpose, the
routines bcopyBytes() and bfillBytes() provide the same functions as bcopy()
and bfill(), but use only byte-at-a-time operations.  These routines do
not check for null termination.

\IFSET_START KERNEL
INCLUDE FILES: string.h
\IFSET_END
\IFSET_START USER
INCLUDE FILES: strings.h
\IFSET_END

SEE ALSO: 'ansiString'
*/

/* LINTLIBRARY */


#include <vxWorks.h>
#include <string.h>
#ifndef _WRS_KERNEL
#include <strings.h>
#endif
#include <private/vxPortableP.h>

#undef bcmp /* so routine gets built for those who don't include header files */
/*******************************************************************************
*
* bcmp - compare one buffer to another
*
* This routine compares the first <nbytes> characters of <buf1> to <buf2>.
*
* RETURNS
* \ms
* \m - 
* 0 if the first <nbytes> of <buf1> and <buf2> are identical,
* \m -
* less than 0 if <buf1> is less than <buf2>, or
* \m -
* greater than 0 if <buf1> is greater than <buf2>.
* \me
*
* ERRNO: N/A
*/

int bcmp
    (
    FAST char * buf1,            /* pointer to first buffer    */
    FAST char * buf2,            /* pointer to second buffer   */
    FAST size_t nbytes           /* number of bytes to compare */
    )
    {
    const unsigned char *p1;
    const unsigned char *p2;

    /* size of memory is zero */

    if (nbytes == 0)
	return (0);

    /* compare array 2 into array 1 */

    p1 = (const unsigned char *)buf1;
    p2 = (const unsigned char *)buf2;

    while (*p1++ == *p2++)
	{
	if (--nbytes == 0)
	    return (0);
        }

    return ((*--p1) - (*--p2));
    }
/*******************************************************************************
*
* binvert - invert the order of bytes in a buffer
*
* This routine inverts an entire buffer, byte by byte.  For example,
* the buffer {1, 2, 3, 4, 5} would become {5, 4, 3, 2, 1}.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

void binvert
    (
    FAST char *  buf,             /* pointer to buffer to invert  */
    size_t       nbytes           /* number of bytes in buffer    */
    )
    {
    FAST char *buf_end = buf + nbytes - 1;
    FAST char temp;

    while (buf < buf_end)
	{
	temp       = *buf;
	*buf       = *buf_end;
	*buf_end   = temp;

	buf_end--;
	buf++;
	}
    }
/*******************************************************************************
*
* bswap - swap buffers
*
* This routine exchanges the first <nbytes> of the two specified buffers.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

void bswap
    (
    FAST char *  buf1,            /* pointer to first buffer  */
    FAST char *  buf2,            /* pointer to second buffer */
    FAST size_t  nbytes           /* number of bytes to swap  */
    )
    {
    FAST char temp;
    size_t i;

    while (nbytes >= 1)
        {
        temp = *buf1;
        *buf1++ = *buf2;
        *buf2++ = temp;
        nbytes--;
        }
    }

#ifdef	_WRS_PORTABLE_swab

/*******************************************************************************
*
* swab - swap bytes
*
* This routine gets the specified number of bytes from <source>,
* exchanges the adjacent even and odd bytes, and puts them in <destination>.
* The buffers <source> and <destination> should not overlap.
*
* \&NOTE:  On some CPUs, swab() will cause an exception if the buffers are
* unaligned.  In such cases, use uswab() for unaligned swaps.  On ARM
* family CPUs, swab() may reorder the bytes incorrectly without causing
* an exception if the buffers are unaligned.  Again, use uswab() for
* unaligned swaps.
*
* The value of <nBytes> must not be odd.  Failure to adhere to this may yield
* incorrect results.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: uswab()
*/

void swab
    (
    char *source,               /* pointer to source buffer      */
    char *destination,          /* pointer to destination buffer */
    size_t nbytes               /* number of bytes to exchange   */
    )
#if defined(_WRS_IMPROVED_PORTABLE_SWAB)
    {
    /*
     * This generates much better code for the ARM, and might well be
     * faster on gcc-based compilers for other architectures.
     */

    FAST unsigned short *src = (unsigned short *) source;
    FAST unsigned short *dst = (unsigned short *) destination;
    FAST unsigned short *dst_end = (unsigned short *) (destination + nbytes);

    for (; dst < dst_end; dst++, src++)
	{
	*dst = (short)(((*src & 0x00ff) << 8) | ((*src & 0xff00) >> 8));
	}
    }
#else /* _WRS_IMPROVED_PORTABLE_SWAB */
    {
    FAST short *src = (short *) source;
    FAST short *dst = (short *) destination;
    FAST short *dst_end = dst + (nbytes / 2);

    for (; dst < dst_end; dst++, src++)
	{
	*dst = (short)(((*src & 0x00ff) << 8) | ((*src & 0xff00) >> 8));
	}
    }
#endif /* _WRS_IMPROVED_PORTABLE_SWAB */

#endif	/* _WRS_PORTABLE_swab */


/*******************************************************************************
*
* uswab - swap bytes with buffers that are not necessarily aligned
*
* This routine gets the specified number of bytes from <source>,
* exchanges the adjacent even and odd bytes, and puts them in <destination>.
*
* \&NOTE:  Due to speed considerations, this routine should only be used when
* absolutely necessary.  Use swab() for aligned swaps.
*
* The value of <nBytes> must not be odd.  Failure to adhere to this may yield
* incorrect results.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: swab()
*/

void uswab
    (
    char *source,               /* pointer to source buffer      */
    char *destination,          /* pointer to destination buffer */
    size_t nbytes               /* number of bytes to exchange   */
    )
    {
    FAST char *dst = (char *) destination;
    FAST char *dst_end = dst + nbytes;
    FAST char byte1;
    FAST char byte2;
    while (dst < dst_end)
	{
	byte1 = *source++;
	byte2 = *source++;
	*dst++ = byte2;
	*dst++ = byte1;
	}
    }

/*******************************************************************************
*
* bzero - zero out a buffer
*
* This routine fills the first <nbytes> characters of the
* specified buffer with 0.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

void bzero
    (
    char *  buffer,               /* buffer to be zeroed       */
    size_t  nbytes                /* number of bytes in buffer */
    )
    {
    bfill (buffer, nbytes, 0);
    }

#ifdef _WRS_PORTABLE_bLib

/*******************************************************************************
*
* bcopy - copy one buffer to another
*
* This routine copies the first <nbytes> characters from <source> to
* <destination>.  Overlapping buffers are handled correctly.  Copying is done
* in the most efficient way possible, which may include long-word, or even
* multiple-long-word moves on some architectures.  In general, the copy
* will be significantly faster if both buffers are long-word aligned.
* (For copying that is restricted to byte, word, or long-word moves, see
* the manual entries for bcopyBytes(), bcopyWords(), and bcopyLongs().)
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: bcopyBytes(), bcopyWords(), bcopyLongs()
*/

void bcopy
    (
    const char *source,       	/* pointer to source buffer      */
    char *destination,  	/* pointer to destination buffer */
    size_t nbytes          	/* number of bytes to copy       */
    )
    {
    FAST char *dstend;
    FAST long *src;
    FAST long *dst;
    FAST char *dsttmp;

    /*
     * Note: the condition (size_t)(destination - source) >= nbytes
     * is true also when destination < source, provided the source buffer
     * does not wrap around the end of memory and then past destination.
     * The only cases that this algorithm cannot handle are those
     * where the source and destination buffers overlap at both
     * ends, not just one. This requires wraparound; an example
     * is the following:
     *
     *   0                                               WRAP
     *   |                                               |
     *         S------------------------------>|
     *                                    D--------------
     *   ----------------->|
     *
     * In such cases, starting the transfer at either end of the
     * source buffer results in overwriting a source datum that hasn't
     * been read yet when first data is stored in the destination
     * buffer. One could perhaps start somewhere in the middle,
     * but the complication to the code is not worth it, for this
     * ultimate edge case involving both wrapping and buffers
     * longer than half of the address space.
     *
     * We also do not bother to optimize the unusual case
     * that (destination == source).
     */

    if ((size_t)(destination - source) >= nbytes)
	{
	/* forward copy */

	dstend = destination + nbytes;

	/*
	 * Do a byte copy if nbytes is too small for amount of blind
	 * copying, or if the source and destination alignments do not match.
	 */
	if (nbytes < (2 * sizeof (long) - 1) ||
	    (((long)destination ^ (long)source) & (sizeof (long) - 1)))
	    goto byte_copy_fwd;

	/* if odd-aligned copy byte */

	while ((long)destination & (sizeof (long) - 1))
	    *destination++ = *source++;

	src = (long *) source;
	dst = (long *) destination;

	/* 
         * dsttmp is highest sizeof(long) aligned address no greater than 
         * dstend 
         */
	dsttmp = (char *)((size_t)dstend & ~(sizeof(long) - 1));
	do
	    {
	    *dst++ = *src++;
	    }
	while ((char *)dst != dsttmp);

	destination = (char *)dst;
	source      = (char *)src;

byte_copy_fwd:
	while (destination != dstend)
	    *destination++ = *source++;
	}
    else
	{
	/* backward copy */

	dstend       = destination;
	destination += nbytes;
	source      += nbytes;

	/* do byte copy if less than ten or alignment mismatch */

	if (nbytes < (2 * sizeof(long) - 1) ||
	    (((long)destination ^ (long)source) & (sizeof(long) - 1)))
	    goto byte_copy_bwd;

	/* if odd-aligned copy byte */

	while ((long)destination & (sizeof(long) - 1))
	    *--destination = *--source;

	src = (long *) source;
	dst = (long *) destination;

	/*
	 * dsttmp is the smallest sizeof(long) aligned address
	 * no less than dstend;
	 */
	dsttmp = (char *)(((size_t)dstend + sizeof(long) - 1) &
			  ~(sizeof(long) - 1));
	do
	    {
	    *--dst = *--src;
	    }
	while ((char *)dst != dsttmp);
	destination = (char *)dst;
	source      = (char *)src;

byte_copy_bwd:
	while (destination != dstend)
	    *--destination = *--source;
	}
    }

/*******************************************************************************
*
* bcopyBytes - copy one buffer to another one byte (8 bits) at a time
*
* This routine copies the first <nbytes> characters from <source> to
* <destination> one byte at a time.  This may be desirable if a buffer can
* only be accessed with byte instructions, as in certain byte-wide
* memory-mapped peripherals.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: bcopy()
*/

void bcopyBytes
    (
    char *source,       /* pointer to source buffer      */
    char *destination,  /* pointer to destination buffer */
    size_t nbytes       /* number of bytes to copy       */
    )
    {
    FAST char *dstend;
    size_t offset = (size_t)(destination - source);

    if (offset == 0)
	return;

    if (offset >= nbytes)  /* true also when destination < source if no wrap */
    	{
	/* forward copy */
	dstend = destination + nbytes;
	while (destination != dstend)
	    *destination++ = *source++;
	}
    else
	{
	/* backward copy */
	dstend       = destination;
	destination += nbytes;
	source      += nbytes;
	while (destination != dstend)
	    *--destination = *--source;
	}
    }

/*******************************************************************************
*
* bcopyWords - copy one buffer to another one word (16 bits) at a time
*
* This routine copies the first <nwords> words from <source> to <destination>
* one word at a time.  This may be desirable if a buffer can only be accessed
* with word instructions, as in certain word-wide memory-mapped peripherals.
* The source and destination must be word-aligned.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: bcopy()
*/

void bcopyWords
    (
    char *source,       /* pointer to source buffer      */
    char *destination,  /* pointer to destination buffer */
    size_t nwords       /* number of words to copy       */
    )
    {
    FAST short *dstend;
    FAST short *src = (short *) source;
    FAST short *dst = (short *) destination;
    size_t nbytes = nwords << 1;           /* convert to bytes */
    size_t offset = (size_t)(destination - source);

    if (offset == 0)
	return;

    if (offset >= nbytes)  /* true also when destination < source if no wrap */
	{
	/* forward copy */
	dstend = dst + nwords;
	while (dst != dstend)
	    *dst++ = *src++;
	}
    else
	{
	/* backward copy */
	dstend = dst;
        dst   += nwords;
	src   += nwords;
	while (dst != dstend)
	    *--dst = *--src;
	}
    }

/*******************************************************************************
*
* bcopyLongs - copy one buffer to another one long word (32 bits) at a time
*
* This routine copies the first <nlongs> 32-bit words from <source> to
* <destination> one 32-bit word at a time.  This may be desirable if a buffer
* can only be accessed with 32-bit loads and stores, as in certain 32-bit-wide
* memory-mapped peripherals.  The source and destination must be
* 4-byte aligned.
*
* Note that in LP64 builds, a 'long' is 64 bits; nevertheless, this
* routine copies in units of 32-bits, and <nlongs> is the number of
* 32-bit data in the buffer.  See bcopyQuads() to copy data in units
* of 64 bits.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: bcopy()
*/

void bcopyLongs
    (
    char *source,       /* pointer to source buffer      */
    char *destination,  /* pointer to destination buffer */
    size_t nlongs       /* number of longs to copy       */
    )
    {
    FAST uint32_t *dstend;
    FAST uint32_t *src = (uint32_t *) source;
    FAST uint32_t *dst = (uint32_t *) destination;
    size_t nbytes = nlongs * sizeof(uint32_t);    /* convert to bytes */
    size_t offset = (size_t)(destination - source);

    if (offset == 0)
        return;

    if (offset >= nbytes)  /* true also when destination < source if no wrap */
	{
	/* forward copy */
	dstend = dst + nlongs;
	while (dst != dstend)
	    *dst++ = *src++;
	}
    else
	{
	/* backward copy */
        dstend = dst;
	dst   += nlongs;
	src   += nlongs;
	while (dst != dstend)
	    *--dst = *--src;
	}
    }

#ifdef _WRS_CONFIG_LP64
/*******************************************************************************
*
* bcopyQuads - copy one buffer to another quad word (64 bits) at a time (64-bit)
*
* This routine copies the first <nquads> 64-bit words from <source> to
* <destination>, one 64-bit word at a time.  This may be desirable if a buffer
* can only be accessed with 64-bit loads and stores, as in certain 64-bit-wide
* memory-mapped peripherals.  The source and destination must be
* 8-byte aligned.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: bcopy()
*/

void bcopyQuads
    (
    char *source,       /* pointer to source buffer      */
    char *destination,  /* pointer to destination buffer */
    size_t nquads       /* number of quads (64-bit units) to copy */
    )
    {
    FAST uint64_t *dstend;
    FAST uint64_t *src = (uint64_t *) source;
    FAST uint64_t *dst = (uint64_t *) destination;
    size_t nbytes = nquads * sizeof(uint64_t);    /* convert to bytes */
    size_t offset = (size_t)(destination - source);

    if (offset == 0)
        return;

    if (offset >= nbytes)  /* true also when destination < source if no wrap */
	{
	/* forward copy */
	dstend = dst + nquads;
	while (dst != dstend)
	    *dst++ = *src++;
	}
    else
	{
	/* backward copy */
        dstend = dst;
	dst   += nquads;
	src   += nquads;
	while (dst != dstend)
	    *--dst = *--src;
	}
    }
#endif

#undef bfill /* so bfill gets built for those who don't include header files */
/*******************************************************************************
*
* bfill - fill a buffer with a specified character
*
* This routine fills the first <nbytes> characters of a buffer with the
* character <ch>.  Filling is done in the most efficient way possible,
* which may be long-word, or even multiple-long-word stores, on some
* architectures.  In general, the fill will be significantly faster if
* the buffer is long-word aligned.  (For filling that is restricted to
* byte stores, see the manual entry for bfillBytes().)
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: bfillBytes()
*/

void bfill
    (
    FAST char *buf,           /* pointer to buffer              */
    size_t nbytes,            /* number of bytes to fill        */
    FAST int ch     	      /* char with which to fill buffer */
    )
    {
    FAST long *pBuf;
    char *bufend = buf + nbytes;
    FAST char *buftmp;
    FAST long val;

    if (nbytes < 2 * sizeof(long) - 1)
	goto byte_fill;

    /*
     * Start on necessary alignment. This loop advances buf
     * at most sizeof(long) - 1 bytes.
     */

    while ((long)buf & (sizeof(long) - 1))
        {
	*buf++ = (char) ch;
        }

    /* write sizeof(long) bytes at a time */
    val = (unsigned char)ch;  /* memset() wants the cast */
    val |= (val << 8);
    val |= (val << 16);
#ifdef  _WRS_CONFIG_LP64
    val |= (val << 32);
#endif

    /* Find the last long word boundary at or before bufend */
    buftmp = (char *)((long)bufend & ~(sizeof(long) - 1));

    pBuf = (long *)buf;

    /* fill 4 bytes (8 bytes for LP64) at a time; don't exceed buf endpoint */

    do
	{
	/* Assert: buftmp and pBuf are sizeof(long) aligned */
	/* Assert: buftmp - (char*)pBuf >= sizeof (long) */
	*pBuf++ = val;
	}
    while ((char *)pBuf != buftmp);

    buf = (char *)pBuf;

    /* fill remaining bytes one at a time */

byte_fill:
    while (buf != bufend)
        {
	*buf++ = (char) ch;
        }
    }

/*******************************************************************************
*
* bfillBytes - fill buffer with a specified character one byte at a time
*
* This routine fills the first <nbytes> characters of the specified buffer
* with the character <ch> one byte at a time.  This may be desirable if a
* buffer can only be accessed with byte instructions, as in certain
* byte-wide memory-mapped peripherals.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: bfill()
*/

void bfillBytes
    (
    FAST char *buf,        /* pointer to buffer              */
    size_t nbytes,         /* number of bytes to fill        */
    FAST int ch		   /* char with which to fill buffer */
    )
    {
    FAST char *bufend = buf + nbytes;

    while (buf != bufend)
	*buf++ = (char) ch;
    }
#endif  /* _WRS_PORTABLE_bLib */

#undef index /* so index gets built for those who don't include header files */
/*******************************************************************************
*
* index - find the first occurrence of a character in a string
*
* This routine finds the first occurrence of character <c>
* in string <s>.
*
* RETURNS:
* A pointer to the located character, or
* NULL if <c> is not found.
*
* ERRNO: N/A
*
* SEE ALSO: strchr().
*/

char *index
    (
    FAST const char * s,      /* string in which to find character */
    FAST int          c       /* character to find in string       */
    )
    {
    FAST char ch;

    while (((ch = *(s++)) != c) && (ch != EOS))
	;

    return (((ch == EOS) && c != EOS) ? NULL : (char *) --s);
    }

#undef rindex /* so rindex is built for those who don't include header files */
/*******************************************************************************
*
* rindex - find the last occurrence of a character in a string
*
* This routine finds the last occurrence of character <c>
* in string <s>.
*
* RETURNS:
* A pointer to <c>, or
* NULL if <c> is not found.
*
* ERRNO: N/A
*/

char *rindex
    (
    FAST const char * s,   /* string in which to find character */
    int               c    /* character to find in string       */
    )
    {
    long i;	/* must be signed! */

    i = (long) strlen (s);	/* point to null terminator of s */

    while (i >= 0)
	{
	if (s [i] == c)
	    return ((char *) &s [i]);

	i--;
	}

    return ((char *) NULL);
    }

