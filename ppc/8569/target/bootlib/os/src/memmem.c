/* memmem.c - search string for substring */

/*
 * Copyright (c) 2011 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01c,30nov11,jpb  WIND00317410.  Fixed memmem documentation.
01b,15aug11,jpb  Defect WIND00289111. Check to see if empty string.
01a,04may11,jpb  written and documented.
*/

/*
DESCRIPTION
This library provides a string search to find the first occurrence of a 
substring within another string.

CONFIGURATION
To include this library, configure VxWorks with the INCLUDE_STRING_SEARCH
component.

SAMPLE CODE
The below example shows how to use memmem().

\cs
    const char haystack[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const void * result;

    result = memmem ((const void *) haystack, strlen (haystack), "JKL", 3);

    if (result != NULL)
        {
        printf ("Needle found in haystack\n");
        }
\ce


INCLUDE FILES: 
*/

/* includes */

#include <vxWorks.h>
#include <string.h>
#include <limits.h>		/* UCHAR_MAX */
#include <ctype.h>		/* tolower(), toupper() */

LOCAL void * memSearch (const void * haystack, size_t haystackLen,
                        const void * needle, size_t needleLen,
                        BOOL caseSensitive);

/*******************************************************************************
*
* memmem - a case-sensitive search to find the first occurrence of a substring 
*          within another string.
*
* This function finds the start of the first occurrence of a substring in a
* memory area of another string.
*
* A description of the memmem() arguments follows:
*
* The argument <haystack> is the text buffer to be searched and the argument 
* <haystackLen> is the length of the buffer.
*
* The argument <needle> is the substring pattern to search for and the argument
* <needleLen> is the length of the substring pattern.  If the needle length is
* 0, a pointer to haystack is returned.
*
* RETURNS: a pointer to the beginning of the substring.  NULL is returned if 
*          the substring is not found, the haystack or needle string is NULL,
*          if the haystack length is equal to zero, or if the needle length
*          is greater than the haystack length.
*/

void * memmem
    (
    const void * haystack, 		/* text buffer to search in */
    size_t       haystackLen,		/* length of text buffer */
    const void * needle, 		/* pattern to search for */
    size_t       needleLen		/* length of pattern */
    )
    {
    return (memSearch (haystack, haystackLen, needle, needleLen, TRUE));
    }

/*******************************************************************************
*
* memimem - a non-case-sensitive search to find the first occurrence of a 
*           substring within another string.
*
* This function finds the start of the first occurrence of a substring in a
* memory area of another string.
*
* A description of the memimem() arguments follows:
*
* The argument <haystack> is the text buffer to be searched and the argument
* <haystackLen> is the length of the buffer.
*
* The argument <needle> is the substring pattern to search for and the argument
* <needleLen> is the length of the substring pattern.  If the needle length is
* 0, a pointer to haystack is returned.
*
* NOTE: This search is not case-sensitive
*
* RETURNS: a pointer to the beginning of the substring.  NULL is returned if
*          the substring is not found, the haystack or needle string is NULL,
*          if the haystack length is equal to zero, or if the needle length
*          is greater than the haystack length.
*/

void * memimem
    (
    const void * haystack,              /* text buffer to search in */
    size_t       haystackLen,           /* length of text buffer */
    const void * needle,                /* pattern to search for */
    size_t       needleLen              /* length of pattern */
    )
    {
    return (memSearch (haystack, haystackLen, needle, needleLen, FALSE));
    }

/*******************************************************************************
*
* memSearch - the guts of finding the first occurrence of a substring within 
*             another string.
*
* A description of the memimem() arguments follows:
*
* The argument <haystack> is the text buffer to be searched and the argument
* <haystackLen> is the length of the buffer.
*
* The argument <needle> is the substring pattern to search for and the argument
* <needleLen> is the length of the substring pattern.  If the needle length is
* 0, a pointer to haystack is returned.
*
* The agrument <caseSensitive> is used to determine if the search should be
* case-sensitive or not.
*
* Boyer-Moore Sunday is a more efficient simplification of the Boyer-Moore
* algorithm. It performs comparisons between a character in the pattern and
* a character in the text buffer from left to right. After each mismatch it
* uses bad character heuristic to shift the pattern to the right. For more
* details on the algorithm, refer to "A Very Fast Substring Search Algorithm",
* Daniel M. Sunday, Communications of the ACM, Vol. 33 No. 8, August 1990,
* pp. 132-142.
*
* It has a O(Pattern Length x Text Buffer Length) worst-case time complexity.
* But empirical results have shown that this algorithm is one of the
* fastest in practice
* 
* \NOMANUAL
*/

LOCAL void * memSearch
    (
    const void * haystack,              /* text buffer to search in */
    size_t       haystackLen,           /* length of text buffer */
    const void * needle,                /* pattern to search for */
    size_t       needleLen,             /* length of pattern */
    BOOL	 caseSensitive
    )
    {
    size_t skipTable[UCHAR_MAX + 1];
    char * pPattern;
    char * pBuffer;
    char * pBufferCompare;
    char * pPatternEnd = (char *) needle + needleLen;
    char * pBufferEnd = (char *) haystack + haystackLen - needleLen + 1;
    size_t b;		/* used to convert to lowercase */
    int    i;		/* used to populate skipTable */

    /* Validation check */

    if ((haystack == NULL) || (haystackLen == 0) || 
        (*(char *)haystack == EOS) || (needle == NULL) || 
        (needleLen > haystackLen))
        return NULL;

    /*
     * The first occurrence of the empty string is deemed to occur at
     * the beginning of the string.
     */

    if (needleLen == 0)
        return ((void *) haystack);

    /* Setup the skip table for the needle */

    for (i = 0; i < UCHAR_MAX + 1; i++)
        skipTable[i] = needleLen + 1;

    for (pPattern = (char *) needle; pPattern < pPatternEnd; pPattern++)
        {
        if (caseSensitive == TRUE)
            skipTable[(size_t)*pPattern] = (size_t)(pPatternEnd - pPattern);
        else
            {
            b = (size_t) tolower((size_t)*pPattern);
            skipTable[b] = (size_t) (pPattern - pPattern);
            b = (size_t) toupper((size_t)*pPattern);
            skipTable[b] = (size_t) (pPatternEnd - pPattern);
            }
        }

    /* Main loop is the fast skip loop */

    for (pBuffer = (char *) haystack; pBuffer < pBufferEnd;
         pBuffer += skipTable[(size_t)*(pBuffer + needleLen)])
        {

        /* Comparison loop */

        if (caseSensitive == TRUE)
            {
            for (pPattern = (char *) needle, pBufferCompare = pBuffer;
                 (pPattern < pPatternEnd) && (*pPattern == *pBufferCompare);
                 pPattern++, pBufferCompare++)
                 ;
            }
        else
            {
            for (pPattern = (char *) needle, pBufferCompare = pBuffer;
		 (pPattern < pPatternEnd) && 
		 (toupper((size_t)*pPattern) == 
                  toupper((size_t)*pBufferCompare));
		 pPattern++, pBufferCompare++)
		 ;
            }

        if (pPattern == pPatternEnd)
            return pBuffer;  /* match */
        }

    /* no match */

    return NULL;
    }

