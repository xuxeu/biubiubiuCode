/* hookShow.c - hook show routines */

/* 
 * Copyright (c) 2003-2006, 2009 Wind River Systems, Inc. 
 *
 * The right to copy, distribute, modify or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */

/*
modification history
--------------------
01h,19aug09,jpb  Updated for LP64 support.
01g,22jun09,jmp  Updated to include symLibP.h instead of funcBindP.h.
		 Removed useless symLib.h inclusion.
01f,17mar09,jmp  Fixed _func_symValueGet call.
01e,07apr06,dgp  doc: fix programmer's guide reference, library
01d,09jul05,yvp  Deleted hookShowInit.
01c,04nov04,job  Docs update
01b,09sep04,job  Docs fixes
01a,15nov03,yvp  written by extracting from taskHookShow.c.
*/

/*
DESCRIPTION

This library provides routines which summarize the installed kernel
hook routines in a given hook table. These routines are generic, and can 
be used to display any kind of hooks.

To include this library, select the INCLUDE_HOOK_SHOW component.

INCLUDE FILES: hookLib.h

SEE ALSO: hookLib,
\tb "VxWorks Kernel Programmer's Guide: Basic OS"
*/

#include <vxWorks.h>
#include <hookLib.h>
#include <sysSymTbl.h>
#include <stdio.h>
#include <private/cplusLibP.h>
#include <private/taskLibP.h>
#include <private/symLibP.h>

/* defines */

#define DEMANGLE_PRINT_LEN	256  /* Length of demangled names to print */


/*******************************************************************************
*
* hookShow - show the hooks in the given hook table
*
* Shows the contents of a hook table symbolically.
*
* RETURNS: N/A.
*/

void hookShow
    (
    void *	table[],	/* table from which to show*/
    int		maxEntries	/* max entries in table */
    )
    {
    FAST int   ix;
    char *     name;	         /* pointer to symTbl copy of name */
    ULONG      displacement;
    SYM_VALUE  symValue;         /* actual symbol value */
    SYMBOL_ID  symId;            /* symbol identifier   */

    char     demangled [DEMANGLE_PRINT_LEN+1];
    char *   nameToPrint;

    /* 
     * Only check one symLib function pointer (for performance's sake). 
     * All symLib functions are provided by the same library, by convention.    
     */

    if ((_func_symFindSymbol != NULL) && (sysSymTbl != NULL))
        {
	for (ix = 0; ix < maxEntries; ++ix)
	    {
	    if (table [ix] == NULL)
	        break;

	    if (((* _func_symFindSymbol) (sysSymTbl,  NULL, 
					 table[ix], 
					 SYM_MASK_NONE, SYM_MASK_NONE, 
					 &symId) == OK) && 
		((* _func_symNameGet) (symId, &name) == OK) && 
		((* _func_symValueGet) (symId, &symValue) == OK)) 
	        {
		nameToPrint = cplusDemangle (name, demangled, 
					     sizeof (demangled));
		printf ("%-25s", nameToPrint);
                displacement = (ULONG) table[ix] - (ULONG) symValue;
		if (displacement != 0)
		    printf ("+0x%-4x", displacement);
		else
		    printf ("%6s", "");         /* no displacement */
		}
	    printf ("\n");
	    }
	}
    else
        {
	for (ix = 0; ix < maxEntries; ++ix)
	    {
	    if (table [ix] == NULL)
	        break;

	    printf ("%#21x", table [ix]);
	    printf ("\n");
	    }
	}
    }
