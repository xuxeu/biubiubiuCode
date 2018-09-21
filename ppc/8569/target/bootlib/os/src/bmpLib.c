/* bmpLib.c - bitmap utility library */

/*
 * Copyright 1998-2000, 2009 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01d,15dec09,cww  Replaced extern ffsMsb declaration with ffsLib.h inclusion
01c,02aug00,pfl  cleaned up, removed arch specific code, SPR 32623.
01b,09sep99,hdn  added bmpLib_PORTABLE macro.
01a,15sep98,tam  created
*/

/*
DESCRIPTION
This library provides a set of macros and routine to manipulate bitmap. 
Most of the macros are defined in bmpLib.h. The <size> in bits of a 
bitmap is user definalbe and must be a multiple of 32 bits (4 bytes). 
Bitmap's bits position are defined from 0 to <size>-1. Bit 0 is the 
most significant bit starting from the left hand side and bit <size>-1 
is the least significant bit ending at the right hand side.

Available macros and routine:
bitGet      - get the value of a bit
bitSet      - set a bit
bitClear    - clear a bit 
bitChange   - toggle a bit
bitFirstGet - return the position of the first bit set within a bitmap

CAVEATS

INCLUDE FILES: bmpLib.h

SEE ALSO: 

INTERNAL

There is no bitmap creation routine. Therefore, the memory required
by the bitmap should be reserved or allocated in advance by the user.
The following is an example of a 64 bit bitmap. Note that bit position
are defined from 0 to 63. 0 is the most significant bit while 63 is 
the least significant bit. 

       MSB                               LSB
  	 ________________ ________________
	|________________|________________|
	0              31 32            63

*/

#include <vxWorks.h>
#include <bmpLib.h>
#include <ffsLib.h>

/* bitmap macros */

#define BMP_ALL_BIT_SET	0xffffffff

/*******************************************************************************
*
* bitFirstGet - return the position of the first bit set within the bitmap
*
* This routine returns the position of the first bit set within a bitmap whose
* start address is <addr>. The search start from the bit offset <offset> and
* the total size in bits of the bitmap is <size>. <size> must be in 4-byte
* boundary. If no bit is set in the bitmap, the funtion returns <size>.
*
* RETURNS: position of the first bit set in the bitmap (between 0 and <size>).
* NOMANUAL
*/

UINT bitFirstGet
    (
    UINT32 * addr,              /* starting address of bitmap */ 
    UINT     offset,		/* from which bit to start */
    UINT     size		/* total size of bitmap in bits */
    )
    {
    FAST UINT curVal;
    FAST UINT bitVal = 0;
    FAST UINT iter = 0;
    FAST UINT32 mask = BMP_ALL_BIT_SET;

    if (offset != 0)
    	{
    	/*
    	 * I guess I should return <size> right away when offset == size to 
	 * speed up this particular case. But If I do this I'll slow down 
	 * slightly the more courant cases so I let it go through the normal
	 * path for now.
    	 */

    	iter = offset >> 5;
    	addr += iter;
    	bitVal = offset & (BMP_ALL_BIT_SET << 5);
    	mask =  mask >> (offset & (BMP_WORD_BIT_SIZE-1));
    	}

    curVal = 32 - ffsMsb((*addr) & mask);

    while (iter < size/BMP_WORD_BIT_SIZE)
    	{
	bitVal += curVal;
	if (curVal != BMP_WORD_BIT_SIZE)
	    break;
    	addr++;
    	iter++;
    	curVal = 32 - ffsMsb(*addr);
	}

    return (bitVal);
    }

				
