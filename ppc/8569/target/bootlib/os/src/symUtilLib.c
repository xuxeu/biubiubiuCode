/* symUtilLib.c - symbol table utilities library */

/* 
 * Copyright 1999,2003-2004,2009-2010 Wind River Systems, Inc.
 * 
 * The right to copy, distribute, modify, or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */

/*
modification history
--------------------
01m,18nov10,jmp  Fixed coverity issue in symNodeAdd (CQ:WIND00241879).
01l,30oct09,jmp  Completed LP64 adapation: Enabled C++ demangler code.
01k,22jun09,jmp  Updated to include symLibP.h instead of funcBindP.h.
		 Removed useless symLib.h inclusion.
01j,20apr09,jmp  Partially adapted for 64-bit support.
01i,27feb09,jmp  Updated symEach calls for 64-bit support.
01h,18aug04,bpn  Modified to support C++ demangled symbol names in a better
                 way.
01g,30jun04,jn   Comment change - explain why coverity-detected
                 possible dereference of NULL pointer is not actually a 
		 problem
01f,04mar04,bpn  Added support for C++ symbol namespace.
01e,28jan04,bpn  Include vxWorks.h first (SPR #90034). Modified symListGet()
                 API.
01d,08jul03,bpn  Updated symListGet() API. Modified node allocation to simplify
                 symbols list freeing.
01c,27jun03,bpn  Removed PD references.
01b,20dec99,rlp  modified the symListGet routine.
01a,17aug99,jmp  written.
*/

/*
DESCRIPTION
This library provide some utilities for symbol table, like symListGet().

INCLUDE FILES: symUtilLib.h
*/

/* includes */

#include <vxWorks.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lstLib.h>
#include <symUtilLib.h>

#include <private/cplusLibP.h>
#include <private/symLibP.h>


/* defines */

/* typedefs */

typedef struct
    {
    LIST *	    pSymList;	/* pointer to the matching symbol list */
    const char *    substr;	/* sub-string to match */
    } MATCH_INFO;

/* globals */

/* locals */

/* forward declarations */

static BOOL	symSimpleMatchAdd (char * name, SYM_VALUE val, SYM_TYPE type,
				   _Vx_usr_arg_t matchInfoArg, SYM_GROUP group);
static BOOL	symNodeAdd (const char * symbolName, const char * string,
			    MATCH_SYMBOL_TYPE type, LIST * pSymList);

/*******************************************************************************
*
* symListGet - get a list of symbols matching the given substring
*
* This routine searches for a list of symbols matching the given partial symbol
* name <symbol> and the partial namespace <nameSpace> (for C++ symbols).
* <nameSpace> can be NULL.
*
* The format of the namespace string is <namespace1::namespace2::...>.
*
* The symbols found are copied into the list pointed by <pSymList>. This list
* has to be initialized before calling the routine and may contain symbols.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void symListGet
    (
    SYMTAB_ID		symTabId,	/* symbol table to search in */
    const char *	string,		/* partial symbol name to match */
    BOOL		demangleMatching, /* TRUE for demangle matching */
    LIST *		pSymList	/* symbol list to fill */
    )
    {
    MATCH_SYMBOL *	pMatchSym;
    MATCH_INFO		matchInfo;
    size_t		len;
    DEMANGLER_MODE 	mode;
    LIST		symList;

    if (symTabId == NULL)
    	return;

    lstInit (&symList);
    matchInfo.pSymList = &symList;

    if (demangleMatching)
	{
	const char * pIdentifierEnd = string;

	/* Extract the first identifier name */

	while (isalnum ((int)*pIdentifierEnd) || *pIdentifierEnd == '_')
	    pIdentifierEnd++;

	matchInfo.substr = (char *) calloc ((size_t)1,
					    (size_t)(pIdentifierEnd
								- string + 1));
	if (matchInfo.substr == NULL)
	    return;

	strncpy ((char *)matchInfo.substr, string,
		 (size_t)(pIdentifierEnd - string));
	mode = DMGL_MODE_COMPLETE;
	}
    else
	{
	matchInfo.substr = string;
	mode = DMGL_MODE_TERSE;
	}

    /*
     * Get the list of the symbol that contains the substrings stored
     * into <matchInfo.nameSpace>. This way, the list of the symbols to
     * demangle and to compare is reduced.
     */

    if (_func_symEach == NULL)
	return;

    _func_symEach (symTabId, symSimpleMatchAdd, (_Vx_usr_arg_t)&matchInfo);

    if (demangleMatching)
	free ((char *)matchInfo.substr);

    if (lstCount (&symList) == 0)
	return;

    /* Do a comparison between the demangle names and the substrings */

    len = strlen (string); 

    pMatchSym = (MATCH_SYMBOL *) lstGet (&symList);
    while (pMatchSym != NULL)
	{
	MATCH_SYMBOL_TYPE	type;
	char *			name;
	BOOL			isSymbolMatching = FALSE;
	char			demangled[MAX_SYS_SYM_LEN + 1];
	size_t			bufferSize;

	/* 
	 * pMatchSym cannot be NULL unless the list has been corrupted, 
	 * since nodeCount was positive when this block was entered, 
	 * and lstGet returns NULL only if the list is empty.
	 */

	bufferSize = sizeof (demangled);
	cplusDemangleToBuffer ((char *)pMatchSym->name, demangled, 
			       &bufferSize, mode, &name);

	if (demangleMatching)
	    {
	    if (strncmp (name, string, len) == 0)
		{
		isSymbolMatching = TRUE;
		type = DEMANGLE_MATCH;
		}
	    }
	else
	    {
	    /*
	     * Check the symbol name against the C symbol name or the demangled
	     * name (as TERSE mode).
	     */

	    if (strncmp (name, string, len) == 0
		&& cplusMangledSymCheck (pMatchSym->name))
		{
		isSymbolMatching = TRUE;
		type = TERSE_MATCH;
		}
	    else if (strncmp (pMatchSym->name, string, len) == 0)
		{
		isSymbolMatching = TRUE;
		type = C_MATCH;
		name = NULL;
		}
	    }

	/* If the symbol is matching, add it back to the list */

	if (isSymbolMatching)
	    symNodeAdd (pMatchSym->name, name, type, pSymList);

	free (pMatchSym);
	pMatchSym = (MATCH_SYMBOL *) lstGet (&symList);
	}
    }

/*******************************************************************************
*
* symNodeAdd - add a symbol node to the list
*
* This function adds a symbol node to the matching symbol list.
*
* RETURNS: TRUE of FALSE if an error occurs
*
* ERRNO:
*/

static BOOL symNodeAdd
    (
    const char *	symbolName,
    const char *	string,
    MATCH_SYMBOL_TYPE	type,
    LIST *		pSymList
    )
    {
    MATCH_SYMBOL *  pMatchSym;
    char *	    name;

    /*
     * If type is TERSE_MATCH or DEMANGLE_MATCH, then string should not be NULL.
     * This should never occured, but we enforce this rule to avoid potential
     * strcpy failure.
     */

    if (((type == TERSE_MATCH) || (type == DEMANGLE_MATCH)) && (string == NULL))
	return (FALSE);

    pMatchSym = (MATCH_SYMBOL *) calloc ((size_t)1, sizeof (MATCH_SYMBOL)
			    + strlen (symbolName) + 1
			    + (string != NULL ? strlen (string) + 1 : 0));
    if (pMatchSym == NULL)
	return FALSE;

    pMatchSym->name = (char *) pMatchSym + sizeof (MATCH_SYMBOL);

    strcpy (pMatchSym->name, symbolName);
    pMatchSym->type = type;

    name = pMatchSym->name + strlen (symbolName) + 1;

    if (type == TERSE_MATCH)
	{
	pMatchSym->terseName = name;
	strcpy (pMatchSym->terseName, string);
	}
    else if (type == DEMANGLE_MATCH)
	{
	pMatchSym->signature = name;
	strcpy (pMatchSym->signature, string);
	}

    lstAdd (pSymList, &pMatchSym->node);
    return TRUE;
    }

/*******************************************************************************
*
* symSimpleMatchAdd - add matching symbols to the list
*
* This function is written to be called by symEach, it tests if the symbol
* provided by symEach match the given sub-string.
*
* RETURNS:
*
* ERRNO:
*/

static BOOL symSimpleMatchAdd
    (
    char *		name,		/* symbol's name */
    SYM_VALUE		val,		/* symbol's value */
    SYM_TYPE		type,		/* symbol's type */
    _Vx_usr_arg_t	matchInfoArg,	/* match information */
    SYM_GROUP		group		/* symbol's group */
    )
    {
    MATCH_INFO * pMatchInfo = (MATCH_INFO *) matchInfoArg;

    /* Check if the symbol name contains the substring */

    if (strstr (name, pMatchInfo->substr) == NULL)
	return TRUE;	/* substring cannot be found */

    /* If the symbol name contains the substring, add the symbol to the list */

    return symNodeAdd (name, NULL, STR_MATCH, pMatchInfo->pSymList);
    }
