/* strSearchLib.c - Efficient string search library */

/* 
 * Copyright (c) 2004-2005, 2010-2011 Wind River Systems, Inc. 
 * 
 * The right to copy, distribute, modify or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */

/*
modification history
--------------------
01g,27may11,jpb  Call memmem or memimem within bmsStrSearch.  Marked
                 library deprecated.
01f,19oct10,jpb  Updated for LP64 adaptation.
01e,04apr04,svk  mods to move library into GPP
01d,09mar05,svk  Fix compilation warning
01c,14sep04,myz  use a UINT8 variable as the skipTable index.
01b,24mar04,svk  Change cast for skipTable index from int to UINT8
01a,13jan04,svk  written
*/

/*
WARNING
This library has been deprecated.  It has been replaced by memmem.

DESCRIPTION
This library supplies functions to efficiently find the first occurrence 
of a string (called a pattern) in a text buffer. Neither the pattern
nor the text buffer needs to be null-terminated.

The functions in this library search the text buffer using a "sliding
window" whose length equals the pattern length. First the left end of 
the window is aligned with the beginning of the text buffer, then the 
window is compared with the pattern. If a match is not found, the 
window is shifted to the right and the same procedure is repeated until
the right end of the window moves past the end of the text buffer.

This library supplies the following search functions:

.iP fastStrSearch()
Optimally chooses the search algorithm based on the pattern size
.iP bmsStrSearch()
Uses the efficient Boyer-Moore-Sunday search algorithm; may not be optimal 
for small patterns
.iP bfStrSearch()
Uses the simple Brute Force search algorithm; best suited for small patterns
.LP

To include this library, configure VxWorks with the INCLUDE_STRING_SEARCH
component.

INTERNAL
Currently the Firewall and NAT MSP components depend on this library. 
Therefore, modifying this library may adversely affect these components.

INCLUDE FILE: strSearchLib.h
*/

/* includes */

#include <vxWorks.h>
#include <ctype.h>
#include <limits.h>
#include <strSearchLib.h>
#include <string.h>		/* memmem */

/* defines */

#define SMALL_PATTERN_SIZE  5


/****************************************************************************
* fastStrSearch - Search by optimally choosing the search algorithm
*
* This routine has been deprecated and will be removed in a future release.
* It is being replaced by memmem().
*
* Depending on the pattern size, this function uses either the 
* Boyer-Moore-Sunday algorithm or the Brute Force algorithm. The
* Boyer-Moore-Sunday algorithm requires pre-processing, therefore for small 
* patterns it is better to use the Brute Force algorithm.
*
* RETURNS: 
* A pointer to the located pattern, or a NULL pointer if the pattern is 
* not found
*/

char * fastStrSearch
    (
    char * pattern,       /* pattern to search for */
    size_t patternLen,    /* length of the pattern */
    char * buffer,        /* text buffer to search in */
    size_t bufferLen,     /* length of the text buffer */
    BOOL   caseSensitive  /* case-sensitive search? */
    )
    {
    if (patternLen >= SMALL_PATTERN_SIZE)
        return bmsStrSearch (pattern, patternLen, buffer, bufferLen, 
                             caseSensitive);
    else
        return bfStrSearch (pattern, patternLen, buffer, bufferLen, 
                            caseSensitive);                  
    }

/****************************************************************************
* bmsStrSearch - Search using the Boyer-Moore-Sunday (Quick Search) algorithm
*
* This routine has been deprecated and will be removed in a future release.
* It is being replaced by memmem().
*
* The Boyer-Moore-Sunday algorithm is a more efficient simplification of
* the Boyer-Moore algorithm. It performs comparisons between a character
* in the pattern and a character in the text buffer from left to right.
* After each mismatch it uses bad character heuristic to shift the
* pattern to the right. For more details on the algorithm, refer to
* "A Very Fast Substring Search Algorithm", Daniel M. Sunday, Communications
* of the ACM, Vol. 33 No. 8, August 1990, pp. 132-142.
*
* It has a O(Pattern Length x Text Buffer Length) worst-case time complexity.
* But empirical results have shown that this algorithm is one of the
* fastest in practice. 
* 
* RETURNS: 
* A pointer to the located pattern, or a NULL pointer if the pattern is 
* not found
*/

char * bmsStrSearch
    (
    char * pattern,       /* pattern to search for */
    size_t patternLen,    /* length of the pattern */
    char * buffer,        /* text buffer to search in */
    size_t bufferLen,     /* length of the text buffer */
    BOOL   caseSensitive  /* case-sensitive search? */
    )
    {

    /* 
     * memmem() & memimem do validation checks but differs because  
     * patternLen will return a pointer to the buffer.  For backwards
     * compatibility, we return NULL if pattern length is equal to 0
     * when calling bmsStrSearch. 
     */

    if (patternLen == 0)
        return NULL;
 
    if (caseSensitive == TRUE)
        return ((char *) memmem ((const void *) buffer, bufferLen,
                                 (const void *) pattern, patternLen));

    else
        return ((char *) memimem ((const void *) buffer, bufferLen,
                                 (const void *) pattern, patternLen));
    
    }

/****************************************************************************
* bfStrSearch - Search using the Brute Force algorithm
*
* This routine has been deprecated and will be removed in a future release.
* It is being replaced by memmem().
*
* The Brute Force algorithm is the simplest string search algorithm. It
* performs comparisons between a character in the pattern and a character
* in the text buffer from left to right. After each attempt it shifts the 
* pattern by one position to the right.
* 
* The Brute Force algorithm requires no pre-processing and no extra space.
* It has a O(Pattern Length x Text Buffer Length) worst-case time complexity.
*
* RETURNS:
* A pointer to the located pattern, or a NULL pointer if the pattern is 
* not found
*/

char * bfStrSearch
    (
    char * pattern,       /* pattern to search for */
    size_t patternLen,    /* length of the pattern */
    char * buffer,        /* text buffer to search in */
    size_t bufferLen,     /* length of the text buffer */
    BOOL   caseSensitive  /* case-sensitive search? */
    )
    {
    char * pPattern;
    char * pBuffer;
    char * pBufferCompare;
    char * pPatternEnd = pattern + patternLen;
    char * pBufferEnd = buffer + bufferLen - patternLen + 1;

    /* Validation check */

    if ((pattern == NULL) || (patternLen == 0) ||
        (buffer == NULL) || (bufferLen == 0) ||
        (patternLen > bufferLen))
        {
        return NULL;
        }

    for (pBuffer = buffer; pBuffer < pBufferEnd; pBuffer++)
        {
        if (caseSensitive == TRUE)
            {
	    for (pPattern = pattern, pBufferCompare = pBuffer;
		 (pPattern < pPatternEnd) && (*pPattern == *pBufferCompare);
		 pPattern++, pBufferCompare++)
		 ;
            }
        else
            {
	    for (pPattern = pattern, pBufferCompare = pBuffer;
		 (pPattern < pPatternEnd) && 
		 (toupper((int)*pPattern) == toupper((int)*pBufferCompare));
		 pPattern++, pBufferCompare++)
		 ;
            }

	if (pPattern == pPatternEnd)
	    return pBuffer;  /* match */
        }

    /* no match */

    return NULL;
    }
