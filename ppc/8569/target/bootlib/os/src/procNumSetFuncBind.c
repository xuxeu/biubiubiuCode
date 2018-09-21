/* procNumSetFuncBind.c - CPU number set function binding library */

/*
 * Copyright (c) 2010 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
01a,04nov10,swg  Created
*/

/*
DESCRIPTION
This module provides function pointers and global variables about setting CPU
number.
*/

#include <vxWorks.h>

BOOL        _procNumWasSet = FALSE;

