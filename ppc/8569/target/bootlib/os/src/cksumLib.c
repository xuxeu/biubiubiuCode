/* cksumLib.c - checksum routine */

/* Copyright (c) 1992-2001, 2010 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
01f,09aug10,cww  Clean up compiler warnings
01e,29mar10,cww  Updated to use size_t param
01d,15oct01,rae  merged from truestack version 01d (SPRs 30521, 64231, 67304)
01c,19oct00,rae  fixed error with little endian targets and odd packet sizes
                 (spr# 30521)
01b,04jul92,smb  added everything above sys/types.h
01a,23mar92,jmm  taken from net source  
*/

/*
DESCRIPTION

*/
#include <vxWorks.h>
#include <sys/types.h>

/*******************************************************************************
*
* checksum - portable internet checksum calculation routine
*
* This checksums the buffer <pAddr> of length <len>.  This routine differs
* from the standard checksum algorithm (in_cksum) in that it checksums a buffer
* while the other checksums an mbuf chain.  For of a description of the
* internet checksum algorithm, please refer to RFC 1071.
*
* RETURNS: checksum
*
* NOMANUAL
*/

u_short checksum
    (
    u_short	* pAddr,                /* pointer to buffer  */
    size_t	len                     /* length of buffer   */
    )
    {
    ULONG       nLeft   = len;
    int         sum     = 0;
    u_short *   w       = pAddr;
    u_short     answer;

    while (nLeft > 1)
        {
        sum     += *w++;
        nLeft   -= 2;
        }

    if (nLeft == 1)
#if _BYTE_ORDER == _BIG_ENDIAN
        sum += 0 | ((*(u_char *) w) << 8);
#else
        sum += *(u_char *) w;
#endif

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = (u_short) sum;

    return (u_short) (~answer & 0xffff);
    }

