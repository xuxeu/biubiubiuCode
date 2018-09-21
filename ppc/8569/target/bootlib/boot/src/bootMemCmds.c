/* bootMemCmds.c - Boot loader application memory command handlers component */

/* 
*  Copyright (c) 2005-2007, 2009-2010, 2012 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
01i,12aug12,mpc  Remove building warnings. (WIND00354175)
01h,11jan12,pgh  Replace sscanf() call.
01g,03feb10,pgh  Fix 64-bit execution
01f,11dec09,pgh  64-bit conversion
01e,29apr09,pgh  Update documentation.
01d,01mar07,jmt  Change memory commands to use STRICT command matching
01c,06feb07,jmt  Add bootAppPrintf to optimize size
01b,01sep06,jmt  Replace printf calls with SHELL_PRINTF
01a,11may05,jmt   written
*/

/*
DESCRIPTION
This module contains support routines for the VxWorks boot loader application 
memory commands.  This module is added to the boot loader application when the 
INCLUDE_BOOT_MEM_CMDS component is included.  This module contains an 
initialization routine, which calls the routine bootCommandHandlerAdd() 
to add the following commands to the boot shell:

\is
\i Display memory
The 'd' command displays the contents of the specified memory locations.
\i Modify memory
The 'm' command displays current memory contents and prompts for new values.
\i Fill memory
The 'f' command fills specified memory locations with the specified value.
\i Transfer memory
The 't' command transfers the contents of specified memory locations to the 
specified location.
\ie

INCLUDE FILES: vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <ctype.h>
#include <fioLib.h>
#include <stdio.h>
#include <string.h>
#include <bootLib.h>
#include <bootApp.h>

/* defines */

/* maximum line length for cmd input to 'm' and 'mEnet' routine */
#define MAX_LINE        160

/* typedefs */

/* globals */

static UINT8 * last_adrs;

/* forward declarations */

void bootAppMemInit (void *);
LOCAL int bootAppMemDisplay (char * cmd);
LOCAL int bootAppMemModify (char * cmd);
LOCAL int bootAppMemFill (char * cmd);
LOCAL int bootAppMemCopy (char * cmd);

/*******************************************************************************
*
* bootAppMemInit - Initialize boot loader application memory commands component
*
* This routine initializes the boot loader application memory commands 
* component.  It adds commands to the boot shell that allow the contents of 
* memory to be displayed, modified, filled with a pattern, and moved.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void bootAppMemInit
    (
    void *  memBase /* memory address to begin content display */
    )
    {
    /* init variables */
    last_adrs = (UINT8 *)memBase;
    
    /* setup command handlers */
    
    bootCommandHandlerAdd("dm", bootAppMemDisplay, BOOT_CMD_MATCH_STRICT,
         "dm adrs (adrs)", "- display memory");
    bootCommandHandlerAdd("mm", bootAppMemModify, BOOT_CMD_MATCH_STRICT,
         "mm adrs", "- modify memory");
    bootCommandHandlerAdd("fm", bootAppMemFill, BOOT_CMD_MATCH_STRICT,
         "fm adrs nbytes value", "- fill memory");
    bootCommandHandlerAdd("cm", bootAppMemCopy, BOOT_CMD_MATCH_STRICT,
         "cm adrs adrs nbytes", "- copy memory");
    }

/* Local functions */

/*******************************************************************************
*
* bootAppMemDisplay - Display memory command handler
*
* This routine displays the contents of memory, starting at the specified
* address, and for the specified amount of memory.  Memory is displayed in
* words.  The number of words displayed defaults to 64.  If the number of 
* words to display is non-zero, that number of words is printed, rounded up 
* to the nearest number of full lines.  That number then becomes the default.
* 
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppMemDisplay
    (
    char * cmd
    )
    {
    _Vx_usr_arg_t * startAddrs = 0; /* starting address to display */
    _Vx_usr_arg_t   nwords = 0;     /* number of words to print */
#if  _BYTE_ORDER == _LITTLE_ENDIAN
    _Vx_usr_arg_t   dispVal;        /* value to display */
#endif
    UINT8 *         memAddr;        /* current memory address */
    static long     dNbytes = 128;  /* default number to print */
    char            ascii [17];     /* ascii representation of bytes */
    FAST long       nbytes;         /* number of byte values to display */
    FAST int        byte;           /* byte counter */

    /* skip the command name and point to first parameter */
    bootCmdNameSkip (&cmd, 2);

    if ((bootGetArg (&cmd, (_Vx_usr_arg_t *)&startAddrs, HEX, OPT) == OK) &&
        (bootGetArg (&cmd, (_Vx_usr_arg_t *)&nwords, DEC, OPT) == OK))
        {
        memAddr = (UINT8 *)((_Vx_usr_arg_t)startAddrs &
                             (_Vx_usr_arg_t)~0x1);   /* even addr */;

        nbytes = 2 * nwords;    /* convert from number of words to bytes */
    
        if (nwords == 0)
            nbytes = dNbytes;   /* no count specified: use current byte count */
        else
            dNbytes = nbytes;   /* change current byte count */

        /* If no address specified: use last address */
        if (startAddrs == 0)
            memAddr = last_adrs;
    
        /* print leading spaces on first line */
    
        bfill ((char *)ascii, 16, '.');
        ascii [16] = EOS;           /* put an EOS on the string */
    
        /* display memory address */

        bootAppPrintf ("%06p: ", 
                        (_Vx_usr_arg_t)((ptrdiff_t)memAddr & (ptrdiff_t)~0xf), 
                        0,0,0,0,0);
    
        /* Display space characters before first value */

        for (byte = 0; byte < ((ptrdiff_t)memAddr & (ptrdiff_t)0xf); byte++)
            {
            bootAppPrintf ("  ",0,0,0,0,0,0);

            /* Put a space between words */
            if (byte & 1)
                bootAppPrintf (" ",0,0,0,0,0,0);

            /* Put an extra space between words 3 and 4 */
            if (byte == 7)
                bootAppPrintf (" ",0,0,0,0,0,0);
        
            ascii [byte] = ' ';
            }
    
    
        /* print out all the words */
    
        while (nbytes-- > 0)
            {
            if (byte == 16)
                {
                /* end of line:
                 *   print out ascii format values and address of next line */
        
                bootAppPrintf (" *%16s*\n%06p: ", 
                                (_Vx_usr_arg_t)ascii, (_Vx_usr_arg_t)memAddr,
                                0,0,0,0);
        
                bfill ((char *) ascii, 16, '.');    /* clear out ascii buffer */
                byte = 0;               /* reset word count */
                }

#if  _BYTE_ORDER == _BIG_ENDIAN
            bootAppPrintf ("%02x", (_Vx_usr_arg_t)(*memAddr & 0xff),
                           0,0,0,0,0);
#else  /* _BYTE_ORDER == _LITTLE_ENDIAN  */
            /* swap odd and even bytes */
            if ((ptrdiff_t)memAddr & (ptrdiff_t)1)
                {
                dispVal = (_Vx_usr_arg_t)*(memAddr - 1) & 0xff;
                bootAppPrintf ("%02x", dispVal, 0,0,0,0,0);
                }
            else
                {
                dispVal = (_Vx_usr_arg_t)*(memAddr + 1) & 0xff;
                bootAppPrintf ("%02x", dispVal, 0,0,0,0,0);
                }
#endif /* _BYTE_ORDER == _BIG_ENDIAN  */
        
            /* Put a space between words */
            if (byte & 1)
                bootAppPrintf (" ",0,0,0,0,0,0);   

            /* Put an extra space between words 3 and 4 */
            if (byte == 7)
                bootAppPrintf (" ",0,0,0,0,0,0); 
        
            if ((*memAddr == ' ') || 
                (isascii ((UINT) *memAddr) && isprint ((UINT) *memAddr)))
                ascii[byte] = *memAddr;
        
            memAddr += 1;
            byte++;
            }
    
    
        /* print remainder of last line */
    
        for (; byte < 16; byte++)
            {
            bootAppPrintf ("  ",0,0,0,0,0,0);

            /* Put a space between words */
            if (byte & 1)
                bootAppPrintf (" ",0,0,0,0,0,0);

            /* Put an extra space between words 3 and 4 */
            if (byte == 7)
                bootAppPrintf (" ",0,0,0,0,0,0);
        
            ascii [byte] = ' ';
            }
    
        bootAppPrintf (" *%16s*\n", (_Vx_usr_arg_t)ascii,
                       0,0,0,0,0);   /* print out ascii format values */
    
        last_adrs = (UINT8 *)memAddr;
        }
    return BOOT_STATUS_COMPLETE;
    }

/*******************************************************************************
*
* bootAppMemModify - Modify memory command handler
*
* This routine is the command handler for the boot shell 'm' command.  It 
* takes the specified address on the command line, and displays the current 
* contents to the user, prompting for a new value to place in the memory 
* location.  The user can respond in one of several ways:
*
*   RETURN   - No change to current memory content, and continue prompting 
*              at the next memory address.
*   <number> - Set the current memory content to <number>, and continue 
*              prompting at the next memory address.
*   . (dot)  - No change to current memory content, and quit.
*   <EOF>    - No change to current memory content, and quit.
*
* All numbers entered and displayed are in hexadecimal.  Memory is treated 
* as 16-bit words.
* 
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppMemModify
    (
    char * cmd  /* pointer to the command string */
    )
    {
    _Vx_usr_arg_t * startAddrs;         /* starting address */
    UINT16 *         memAddrs;           /* address being modified */
    UINT16           memVal;             /* value of memory being modified */
    char            line[MAX_LINE + 1]; /* leave room for EOS */
    char *          pLine;              /* ptr to current position in line */
    _Vx_usr_arg_t   newValue;           /* value found in input line */

    /* skip the command name and point to first parameter */
    bootCmdNameSkip (&cmd, 2);

    if (bootGetArg (&cmd, (_Vx_usr_arg_t *)&startAddrs, HEX, !OPT) == OK)
        {
        /* round down to word boundary */
        startAddrs = (_Vx_usr_arg_t *)((_Vx_usr_arg_t)startAddrs & 
                                       (_Vx_usr_arg_t)~0x1);   /* even addr */
    
        for (memAddrs = (UINT16 *)startAddrs;   /* starting addr */
             ;                                  /* FOREVER */
             memAddrs++)                        /* inc short ptr */
            {
            /* Get current value of memory location. */

            memVal = *memAddrs;

            /* prompt for substitution */
    
            bootAppPrintf ("%06p:  %04x-", 
                           (_Vx_usr_arg_t)memAddrs, 
                           (_Vx_usr_arg_t)memVal,
                           0,0,0,0);
        
            /* get substitution value:
             *   skip empty lines (CR only);
             *   quit on end of file or invalid input;
             *   otherwise write specified value to the current address */
        
            if (fioRdString (STD_IN, line, MAX_LINE) == EOF)
                break;
        
            line [MAX_LINE] = EOS;  /* make sure input line has EOS */
        
            /* skip leading spaces*/
        
            for (pLine = line; isspace((UINT)*pLine); ++pLine)
                ;
        
            if (*pLine == EOS)              /* skip field if just CR */
                continue;

            if (bootScanNum (&pLine, (_Vx_usr_arg_t *)&newValue, TRUE) != OK)
                break;                      /* quit if not number */
        
            *memAddrs = (UINT16)newValue;   /* assign new value */
            }
        
        bootAppPrintf ("\n",0,0,0,0,0,0);
        }

    return BOOT_STATUS_COMPLETE;
    }

/*******************************************************************************
*
* bootAppMemFill - Fill memory command handler
*
* This routine is the command handler for the boot shell 'f' command.  It
* fills memory, starting at the address specified on the command line, with
* the byte value specified on the command line, for the number of bytes 
* specified on the command line.
* 
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppMemFill
    (
    char * cmd  /* pointer to the command line */
    )
    {
    _Vx_usr_arg_t   nbytes;
    _Vx_usr_arg_t   value;
    _Vx_usr_arg_t * adr;
 
    /* skip the command name and point to first parameter */
    bootCmdNameSkip (&cmd, 2);

     if ((bootGetArg (&cmd, (_Vx_usr_arg_t *)&adr, HEX, !OPT) == OK) &&
         (bootGetArg (&cmd, (_Vx_usr_arg_t *)&nbytes, DEC, !OPT) == OK) &&
         (bootGetArg (&cmd, (_Vx_usr_arg_t *)&value, DEC, !OPT) == OK))
        {
        bfillBytes ((char *)adr, (size_t)nbytes, (int)value);
        }

    return BOOT_STATUS_COMPLETE;
    }

/*******************************************************************************
*
* bootAppMemCopy - Transfer memory command handler
*
* This routine is the command handler for the boot shell 't' command.  It
* transfers the contents of memory from one location to another.  The starting
* address, the destination address, and the number of bytes to transfer are
* all specified on the command line.
* 
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppMemCopy
    (
    char * cmd  /* pointer to the command line string */
    )
    {
    _Vx_usr_arg_t   nbytes;
    _Vx_usr_arg_t * adr2;
    _Vx_usr_arg_t * adr;

    /* skip the command name and point to first parameter */
    bootCmdNameSkip (&cmd, 2);

    if ((bootGetArg (&cmd, (_Vx_usr_arg_t *)&adr, HEX, !OPT) == OK) &&
        (bootGetArg (&cmd, (_Vx_usr_arg_t *)&adr2, HEX, !OPT) == OK) &&
        (bootGetArg (&cmd, (_Vx_usr_arg_t *)&nbytes, HEX, !OPT) == OK))
        {
        bcopy ((char *)adr, (char *)adr2, (size_t)nbytes);
        }
    return BOOT_STATUS_COMPLETE;
    }


