/* hookLib.c - generic hook library for VxWorks */

/* 
 * Copyright (c) 2003-2008, 2010, 2012 Wind River Systems, Inc. 
 *
 * The right to copy, distribute, modify or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */ 

/*
modification history
--------------------
01n,10may12,jpb  Defect WIND00070686.  Fixed missing hyperlinks.
01m,03feb12,jpb  Defect 00312549.  Fixed array boundary issue in hookDelete.
01l,31mar10,gls  removed reference to taskRtpLock() in documentation
01k,27aug08,jpb  Renamed VSB config file.
01j,18jun08,jpb  Renamed _WRS_VX_SMP to _WRS_CONFIG_SMP.  Added include path
                 for kernel configurations options set in vsb.
01i,13jul07,jmg  Replaced rtpTaskLock() with a semTake().
01h,08nov06,dbt  Check if the scheduler is already initialized before calling
                 taskLock()/taskUnlock().
		 Removed taskLock()/taskUnlock() calls for SMP systems.
01g,07apr06,dgp  doc: fix programmer's guide reference, library
01f,08sep05,yvp  Fixed boundary condition bug in hookDelete() (SPR 112065).
01e,21mar05,yvp  Deleted hookLibInit.
01d,16feb04,yvp  Added null-pointer checks in hookAddXXX routines.
01c,29jan04,yvp  Enhanced documentation.
01b,01dec03,md   change taskULock/taskUUnlock to taskRtpLock/taskRtpUnlock
01a,18nov03,yvp  written - generalized from ver 2a of taskHookLib.c
*/

/*
DESCRIPTION
This library provides generic functions to add and delete hooks. Hooks
are function pointers, that when set to a non-NULL value are called by
VxWorks at specific points in time. The hook primitives provided by
this module are used by many VxWorks facilities such as taskLib,
rtpLib, etc.

A hook table is an array of function pointers. The size of the array
is decided by the various facilities using this library. The head of a
hook table is the first element in the table (i.e. offset 0), while
the tail is the last element (i.e. highest offset).  Hooks can be
added either to the head or the tail of a given hook table.  When
added to the tail, a new routine is added after the last non-NULL
entry in the table. When added to the head of a table, new routines
are added at the head of the table (index 0) after existing routines
have been shifted down to make room.

Hook execution always proceeds starting with the head (index 0) till a
NULL entry is reached. Thus adding routines to the head of a table
achieves a LIFO-like effect where the most recently added routine is
executed first.  In contrast, routines added to the tail of a table
are executed in the order in which they were added. For example, task
creation hooks are examples of hooks added to the tail, while task
deletion hooks are an example of hooks added to the head of their
respective table. Hook execution macros HOOK_INVOKE_VOID_RETURN and 
HOOK_INVOKE_CHECK_RETURN (defined in hookLib.h) are handy in calling 
hook funcitons. Alternatively, users may write their own invocations.


NOTE
It is possible to have dependencies among hook routines. For example,
a delete hook may use facilities that are cleaned up and deleted by
another delete hook. In such cases, the order in which the hooks run
is important.  VxWorks runs the create and switch hooks in the order
in which they were added, and runs the delete hooks in reverse of the
order in which they were added. Thus, if the hooks are added in
"hierarchical" order, such that they rely only on facilities whose
hook routines have already been added, then the required facilities
will be initialized before any other facilities need them, and will be
deleted after all facilities are finished with them.

VxWorks facilities guarantee this by having each facility's
initialization routine first call any prerequisite facility's
initialization routine before adding its own hooks.  Thus, the hooks
are always added in the correct order.  Each initialization routine
protects itself from multiple invocations, allowing only the first
invocation to have any effect.

INCLUDE FILES: hookLib.h

\IFSET_START KERNEL
SEE ALSO: dbgLib, taskLib, taskVarLib, rtpLib, the VxWorks Kernel Programmer 
Guide.
\IFSET_END
\IFSET_START USER
SEE ALSO: taskLib, rtpLib, the VxWorks Application Programmer Guide.
\IFSET_END
*/

#include <vxWorks.h>
#ifdef _WRS_KERNEL
#include <vsbConfig.h>
#endif /* _WRS_KERNEL */
#include <errnoLib.h>
#include <hookLib.h>	
#include <taskLib.h>
#include <private/taskLibP.h>
#ifdef	_WRS_KERNEL
#include <private/kernelLibP.h>
#else
#include <private/semLibP.h>
#endif	/* WRS_KERNEL */

/* defines */

#ifndef _WRS_KERNEL

/* 
 * the hooktableSem is used to protect the hookTable. The semaphore is
 * initilized in taskLibInit, but the declaration and allocation are done
 * here.  If this semaphore is used before taskLibInit is call an error
 * will be returned and ignored.  This error is fine, before taskLibInit
 * is call the RTP will be running single threaded and therefore no 
 * protection is need. 
 */

SEM_DECL (hookTableSem);  /* Initilized in taskLibInit */
    
    /* 
     * There are no taskLock and taskUnlock functions in userland.
     * Instead a semaphore is employed.
     */

#define HOOK_TABLE_LOCK()	semTake (&hookTableSem, WAIT_FOREVER)
#define HOOK_TABLE_UNLOCK()	semGive (&hookTableSem)

#else	/* !_WRS_KERNEL */

    /*
     * Make sure the kernel is up and running before using
     * taskLock()/KERNEL_ENTER() APIs. This allows installing a task creation
     * hook before the kernel is initialized that can be called when
     * the VxWorks root task is created.
     *
     * For SMP system, the kernel spinlock is taken rather than calling
     * taskLock() because this API does not have the expected behavior
     * in this environment.
     */

#ifndef	_WRS_CONFIG_SMP
#define	HOOK_TABLE_LOCK()		\
	{				\
	if (taskIdCurrent != 0)		\
	    taskLock ();		\
	}
#define	HOOK_TABLE_UNLOCK()		\
	{				\
	if (taskIdCurrent != 0)		\
	    taskUnlock ();		\
	}
#else	/* _WRS_CONFIG_SMP */
#define	HOOK_TABLE_LOCK()		\
	{				\
	if (taskIdSelf () != 0)		\
	    KERNEL_ENTER ();		\
	}
#define	HOOK_TABLE_UNLOCK()		\
	{				\
	if (taskIdSelf () != 0)		\
	    KERNEL_EXIT ();		\
	}
#endif	/* _WRS_CONFIG_SMP */
#endif	/* _WRS_KERNEL */


/******************************************************************************
*
* hookAddToTail - add a hook routine to the end of a hook table
*
* This routine adds a hook routine into a given hook table. The routine is
* added at the first NULL entry in the table. In other words new hooks are 
* appended to the list of hooks already present.
*
* NOTE: This routine does not guard against duplicate entries.
*
* ERRNO: S_hookLib_HOOK_TABLE_FULL
*
* RETURNS: OK, or ERROR if hook table is full.
*/

STATUS hookAddToTail
    (
    void * hook,       /* routine to be added to table */
    void * table[],    /* table to which to add */
    int    maxEntries  /* max entries in table */
    )
    {
    FAST int ix;

    if (table == (void *) NULL)
	{
	errnoSet (S_hookLib_HOOK_TABLE_NULL);
	return (ERROR);
	}

    HOOK_TABLE_LOCK ();				/* disable task switching */

    /* find a slot after last hook in table */

    for (ix = 0; ix < maxEntries; ++ix)
	{
	if (table[ix] == NULL)
	    {
	    table[ix] = hook;
	    HOOK_TABLE_UNLOCK ();		/* re-enable task switching */
	    return (OK);
	    }
	}

    /* no free slot found */

    HOOK_TABLE_UNLOCK ();			/* re-enable task switching */

    errnoSet (S_hookLib_HOOK_TABLE_FULL);
    return (ERROR);
    }


/******************************************************************************
*
* hookAddToHead - add a hook routine at the start of a hook table
*
* This routine adds a hook routine into a given hook table. The routine is 
* added at the head (i.e. first entry) of the table. Existing hooks are 
* shifted down to make way for the new hook. The last entry of the table is
* always NULL. Hooks are executed from the lowest to highest index of the 
* table. Hence this routine should be used if hooks should be executed in
* LIFO order (i.e. last hook added executes first). Examples of LIFO hook
* execution are task delete hooks.
* 
* NOTE: This routine does not guard against duplicate entries.
*
* ERRNO: S_hookLib_HOOK_TABLE_FULL
*
* RETURNS: OK, or ERROR if hook table is full.
*/

STATUS hookAddToHead
    (
    void * hook,	/* routine to be added to table */
    void * table[],	/* table to which to add */
    int    maxEntries	/* max entries in table */
    )
    {
    FAST int ix;
    STATUS status = OK;

    if (table == (void *) NULL)
	{
	errnoSet (S_hookLib_HOOK_TABLE_NULL);
	return (ERROR);
	}

    HOOK_TABLE_LOCK ();				/* disable task switching */

    if (table [maxEntries - 1] != NULL)
	{
	errnoSet (S_hookLib_HOOK_TABLE_FULL);	/* no free slot found */
	status = ERROR;
	}
    else
	{
	/* move all the hooks down one slot in the table */

	for (ix = maxEntries - 2; ix >= 0; --ix)
	    table [ix + 1] = table [ix];

	table [0] = hook;
	}

    HOOK_TABLE_UNLOCK ();			/* re-enable task switching */

    return (status);
    }


/******************************************************************************
*
* hookDelete - delete a hook from a hook table
*
* Deletes a previously added hook (if found) from a given hook table. Entries
* following the deleted hook are moved up to fill the vacant spot created.
*
* ERRNO: S_hookLib_HOOK_NOT_FOUND
*
* RETURNS: OK, or ERROR if hook could not be found.
*/

STATUS hookDelete
    (
    void * hook,       /* routine to be deleted from table */
    void * table[],    /* table from which to delete */
    int maxEntries      /* max entries in table */
    )
    {
    FAST int ix;

    if (table == (void *) NULL)
	{
	errnoSet (S_hookLib_HOOK_TABLE_NULL);
	return (ERROR);
	}

    HOOK_TABLE_LOCK ();				/* disable task switching */

    /* find hook in hook table */

    for (ix = 0; ix < maxEntries; ++ix)
	{
	if (table [ix] == hook)
	    {
            do
                {
                if (ix == maxEntries - 1)
                    {
                    /* 
                     * since this the last entry in the table, just null out 
                     * the slot and return.  There are no more hooks to shift
                     * up.
                     */

                    table[ix] = NULL;
                    break;
                    }

                /* move all the remaining hooks up one slot in the table */

                table[ix] = table[ix+1];
                ix++;
                }
            while (table[ix] != NULL);

            HOOK_TABLE_UNLOCK ();

            return (OK);
	    }
	}

    /* hook not found in table */

    HOOK_TABLE_UNLOCK ();			/* re-enable task switching */

    errnoSet (S_hookLib_HOOK_NOT_FOUND);
    return (ERROR);
    }


/******************************************************************************
*
* hookFind - Search a hook table for a given hook
*
* This function searches through a given hook table for a certain hook
* function. If found TRUE is returned, otherwise FALSE is returned.
*
* ERRNO: N/A.
*
* RETURNS: TRUE, or FALSE if the hook was not found.
*/

BOOL hookFind 
    (
    void * hook,	/* routine to be deleted from table */
    void * table[],	/* table from which to delete */
    int    maxEntries	/* max entries in table */
    )
    {
    FAST int ix;

    if (table == (void *) NULL)
	{
	errnoSet (S_hookLib_HOOK_TABLE_NULL);
	return (FALSE);
	}

    HOOK_TABLE_LOCK ();			/* disable task switching */

    /* find hook in hook table */

    for (ix = 0; ix < maxEntries; ++ix)
	{
	if (table [ix] == hook)
	    {
	    HOOK_TABLE_UNLOCK ();	/* re-enable task switching */
	    return (TRUE);
	    }
	}

    /* hook not found in table */

    HOOK_TABLE_UNLOCK ();		/* re-enable task switching */

    return (FALSE);
    }
