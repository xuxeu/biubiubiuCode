/* bootAppTestFuncs.c - Boot loader application test routines component */

/* 
 * Copyright (c) 2006-2007, 2009, 2012 Wind River Systems, Inc.
 * 
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01h,19apr12,wqi  Fix wrong code of bootAppTestShellPrintf. (WIND00345729)
01g,10jan12,pgh  Fix strcpy() usage.
01f,15dec09,pgh  64-bit conversion
01e,28apr09,pgh  Update documentation
01d,06mar07,jmt  Defect 89854 - bootapp test crashes
01c,15nov06,jmt  Change boot prompt
01b,05sep06,jmt  Fix runtime error in init.
01a,22aug06,jmt  written
*/

/*
DESCRIPTION
This module contains the VxWorks boot loader application test component.  This 
module is added to the boot loader application when the INCLUDE_BOOT_TEST_FUNCS
component is included.  This module consists of an initialization routine, 
which modifies the boot shell prompt to identify it as a test image, and test 
case support routines.

INCLUDE FILES: bootApp.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <ioLib.h>          /* IO library routines */
#include <semLib.h>         /* semaphore routines */
#include <stdio.h>
#include <string.h>         /* string manipulation routines */
#include <sysLib.h>
#include <taskLib.h>
#include <private/timerLibP.h>
#include <usrConfig.h>

#include <bootApp.h>
#include <bootAppShell.h>
#include <bootAppTestFuncs.h>

/* defines */
#define PRINTF_LINE_SIZE    1023

/* imports */

extern bootShellProcPrototype * bootAppShellFunc;
extern bootShellPrintfPrototype * bootShellPrintfFunc;
extern int sysStartType;
extern char * bootShellPrompt;

/* locals */

LOCAL bootShellProcPrototype * bootAppTestSavedShellFunc;
LOCAL bootShellPrintfPrototype * bootTestSavedShellPrintfFunc;
LOCAL BOOL bootAppInTestMode = FALSE;
LOCAL BOOL bootAppTestSemInit = FALSE;
LOCAL SEMAPHORE initDoneSemId;  /* Init Done semaphore id */
LOCAL SEMAPHORE inputReadySemId;  /* Input Ready semaphore id */
LOCAL char bootInput [BOOT_CMD_MAX_LINE + 1];
LOCAL char bootOutput [BOOT_APP_TEST_OUTPUT_SIZE + 1];
LOCAL int bootOutputLength;
LOCAL char * outputLine = NULL;

/* forward declarations */

STATUS bootAppTestFuncsInit (BOOL);
STATUS bootAppTestSetup (void);
STATUS bootAppTestCleanup (void);
LOCAL char * bootAppTestShell (char * cmd, int length);
char * bootAppTestOutputGet (void);
void bootAppTestOutputClear (void);
LOCAL void bootAppTestShellPrintf (char * x, 
                                   _Vx_usr_arg_t a, 
                                   _Vx_usr_arg_t b, 
                                   _Vx_usr_arg_t c, 
                                   _Vx_usr_arg_t d, 
                                   _Vx_usr_arg_t e, 
                                   _Vx_usr_arg_t f);

/*******************************************************************************
*
* bootAppTestFuncsInit - Initializes the boot loader application test component
*
* This routine initializes the boot loader application test component.  It 
* takes a single parameter, which determines if the auto boot routine is 
* disabled.  The default for this parameter is to disable the auto boot 
* routine.  This routine then allocates a buffer to store output text, and 
* modifies the boot shell prompt to identify the image as a test image.
*
* RETURNS: OK
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS bootAppTestFuncsInit
    (
    BOOL disableAutoBoot
    )
    {
    if (disableAutoBoot == TRUE)
        sysStartType |= BOOT_NO_AUTOBOOT;

    /* allocate memory for output line */
    outputLine = (char *) KMEM_ALLOC (PRINTF_LINE_SIZE+1);

    /* change the boot shell prompt to indicate test mode */
    bootShellPrompt = "[VxWorks Boot (test)]: ";
    return OK;
    }

/*******************************************************************************
*
* bootAppTestSetup - Create boot loader application test environment
*
* This routine creates the test environment for the boot loader application 
* test cases.  This routine replaces the normal boot shell with the routine
* bootAppTestShell(), which is the test shell for the boot loader application 
* test environment.  Therefore, the boot loader application will not execute 
* normally until the bootAppTestCleanup() routine is called.  The boot 
* loader application test case routines call this routine at the start of 
* the test case, and call bootAppTestCleanup() at the end of the test case 
* routine.
*
* RETURNS: 
*   OK if the test environment was set up
*   ERROR if the test environment could not be created
*
* ERRNO: N/A
*/

STATUS bootAppTestSetup ()
    {
    
    if (bootAppTestSemInit == FALSE)
        {
        /* Initialize binary semaphore for Init Done */
        if ((semBInit (&initDoneSemId, SEM_Q_FIFO, SEM_EMPTY)) != OK)
            {
            return ERROR;
            }

        /* Initialize binary semaphore for input Ready */
        if ((semBInit (&inputReadySemId, SEM_Q_FIFO, SEM_EMPTY)) != OK)
            {
            return ERROR;
            }

        bootAppTestSemInit = TRUE;
        }

    if (bootAppInTestMode == FALSE)
        {
        /* initialize local variables */
        bootOutputLength = 0;
        bootOutput[0] = 0;
        bootInput[0] = 0;

        /* setup init done semaphore */
        semTake (&initDoneSemId, NO_WAIT);

        /* initialize processing routine pointers */
        bootAppTestSavedShellFunc = bootAppShellFunc; 
        bootAppShellFunc = (bootShellProcPrototype *) bootAppTestShell; 

        bootTestSavedShellPrintfFunc = bootShellPrintfFunc;
        bootShellPrintfFunc = (bootShellPrintfPrototype *) bootAppTestShellPrintf;

        /* wait for init done semaphore */
        if (semTake (&initDoneSemId, sysClkRateGet()) == ERROR)
            {
            /* cleanup */
            bootAppInTestMode = TRUE;
            bootAppTestCleanup();
            return ERROR;
            }

        bootAppInTestMode = TRUE;
        }

    return (OK);
    }
    

/*******************************************************************************
*
* bootAppTestCleanup - Removes the boot loader application test environment
*
* This routine removes the boot loader application test environment.  This 
* routine restores the normal boot shell.  This routine is to be called at 
* the end of the boot loader application test case routine.
*
* RETURNS: OK
*
* ERRNO: N/A
*/

STATUS bootAppTestCleanup ()
    {
    if (bootAppInTestMode == TRUE)
        {
        /* restore processing routine pointers */
        bootAppShellFunc = bootAppTestSavedShellFunc;
        bootShellPrintfFunc = bootTestSavedShellPrintfFunc;
		
        /* cleanup on input task */
        bootInput[0] = 0;
        semGive(&initDoneSemId);
        semGive(&inputReadySemId);

        bootAppInTestMode = FALSE;
        }

    return (OK);
    }
    

/*******************************************************************************
*
* bootAppTestShell - Test shell for boot loader application test environment
*
* This routine implements the test shell for the boot loader application test 
* environment.  It waits for the command input to be ready for processing, 
* then returns the input string to the specified buffer.
*
* RETURNS: Pointer to the input command string
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL char * bootAppTestShell
    (
    char * line,    /* buffer to receive input command string */
    int    length   /* Maximum length of command string */
    )
    {
    int inputLength = 0;
    
    /* tell the init function that we are in the Test Shell loop */
    semGive (&initDoneSemId);

    /* wait for a line to be ready */
    semTake (&inputReadySemId, WAIT_FOREVER);
    
    /* get length of input command string */
    inputLength = (int)strlen(bootInput);

    /* Limit the length if needed */
    inputLength = (inputLength > length) ? length : inputLength;

    /* if a command string was input, copy it */
    if (inputLength > 0)
        strncpy(line, bootInput, inputLength);
    
    /* NULL terminate the string */
    line[inputLength] = 0;
    
    return line;
    }

/*******************************************************************************
*
* bootAppTestOutputGet - Get test shell output
*
* This routine is called by the boot loader application test case routines to 
* get the string output by the test shell.
* 
* RETURNS: Test shell output string
*
* ERRNO: N/A
*/

char * bootAppTestOutputGet ()
    {
    return bootOutput;
    }

/*******************************************************************************
*
* bootAppTestOutputClear - Clear test shell output buffer
*
* This routine is called by the boot loader application test case routines to
* clear the contents of the test shell output buffer.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*/

void bootAppTestOutputClear ()
    {
    bootOutput[0] = 0;
    bootOutputLength = 0;
    }

/*******************************************************************************
*
* bootAppTestInputSend - Send command string to boot test shell 
*
* This routine is called by the boot loader application test case routines to 
* send a command line input string to the test shell for processing.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*/

void bootAppTestInputSend 
    (
    char * input
    )
    {
    /* avoid buffer overrun */
    int length = (strlen(input) >= BOOT_CMD_MAX_LINE) ? 
                 BOOT_CMD_MAX_LINE : (int)strlen(input);
    
    /* don't copy if there is no string */
    if (length > 0)
        strncpy(bootInput, input, length);

    /* NULL terminate the string */
    bootInput[length] = 0;

    /* start processing */
    semGive (&inputReadySemId);
    }

/*******************************************************************************
*
* bootAppTestShellPrintf - Boot loader application test shell printf routine
*
* This routine replaces the normal boot loader application printf() routine for
* the test environment.  Rather than printing to standard output, this routine 
* prints to the test shell output buffer.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void bootAppTestShellPrintf
    (
    char * x, 
    _Vx_usr_arg_t a, 
    _Vx_usr_arg_t b, 
    _Vx_usr_arg_t c, 
    _Vx_usr_arg_t d, 
    _Vx_usr_arg_t e, 
    _Vx_usr_arg_t f
    )
    {
    int     outputLen;

    if (outputLine != NULL)
        {
        snprintf(outputLine, PRINTF_LINE_SIZE, x, a, b, c, d, e, f);
        outputLine[PRINTF_LINE_SIZE] = 0;

        outputLen = (int)strlen(outputLine);
        if ((bootOutputLength + outputLen) <= BOOT_APP_TEST_OUTPUT_SIZE)
            {
            strncpy(&bootOutput[bootOutputLength], outputLine, outputLen);
            bootOutputLength += outputLen;
            bootOutput[bootOutputLength] = EOS;
            }
        }
    }
    

