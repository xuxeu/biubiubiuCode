/* bootAppShell.c - Boot loader application command shell component */

/*
 *  Copyright (c) 2005-2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01x,04sep13,xms  fix CHECKED_RETURN error. (WIND00414265)
01w,20feb13,clx  fix copyright date information. (WIND00404266)
01v,28jun12,wqi  adjust helpMsg size. (WIND00357220)
01u,16mar12,y_c  Fix helpMsg size. (WIND00253912)
01t,17nov11,rlg  fix WIND00318015  - add sys debug flag help no longer based
                 off of EDR
01s,14jan11,pch  CQ250052: back out 01q as underlying problem has been fixed
01r,26apr10,j_l  Fix entry address print in bootAppBootFileLoad() and
		 copyright date change.
01q,03feb10,pgh  Fix console read file descriptor.
01p,10dec09,pgh  64-bit conversion.
01o,30jul09,pgh  Expand number of devices
01n,13jul09,pgh  Fix return value for bootNetSettingsUpdate()
01m,27apr09,pgh  Update documentation.
01l,21aug08,pgh  Fix WIND00130193.
01k,18jun08,pgh  Add support for SATA.
01j,15jan08,pgh  Fix help formatting.
01i,23oct07,h_k  updated Copyright info in printBootLogo(). (CQ:109120)
01h,23apr07,jmt  Defect 93088 - networking not setup correctly for some
		 bootline updates
01g,02mar07,jmt  Move devs command to bootFsLoad.c
01f,23feb07,jmt  Add devs command
01e,06feb07,jmt  Add bootAppPrintf to optimize size
01d,25jan07,jmt  Update network settings after bootline update.
01c,14aug06,jmt  Fix problem with bootAppGo parameter parsing
		 Move LoadAndGo to bootApp.c
01b,20jul06,jmt  Add banner print to shell init function
		 Modify LoadAndGo command to use loose matching.
		 vxTestV2 uses loose version for testing.
01a,07jul05,jmt  written
*/

/*
DESCRIPTION
This module contains routines to implement the VxWorks boot loader application
command shell component.  This module is added to the boot loader application
when the INCLUDE_BOOT_SHELL component is included.  This module consists of a
component intialization routine, which adds help display routines and
command handlers to the boot loader application.  This module implements the
boot shell, which processes user command line inputs.  In addition, it contains
the routines that implement the following features:

\is
\i Command help
Allows other boot loader application components to add command help strings.
\i Device help
Allows other boot loader application components to add device format help
strings.
\i Boot flag help
Allows other boot loader application components to add boot flag help strings.
\i List devices
Allows other boot loader application components to add to the list of devices
that are displayed.
\i Text display
Provides formatted text display, with limited arguments, to the boot shell and
boot loader application components.
\ie

The bootCommandHandlerAdd() routine is used to add the following commands to
the boot shell:

\is
\i Help
The '?' and 'h' commands display all of the available command, their formats, and help on the various devices and boot flags.
\i Load and go
The '@' command loads the file specified by the boot line parameters, and
executes it.
\i Print parameters
The 'p' command displays the current values of the boot line parameters.
\i Change parameters
The 'c' command prompts the user for new values for each of the boot line
parameters.
\i Load file
The 'l' command loads the file specified by the boot line parameters.
\i Execute at address
The 'g' command starts execution at the specified address.
\i Print exception
The 'e' command displays information on exceptions.
\i Version
The 'v' command displays the version information for the boot loader
application.
\ie

This module also uses the bootDevFormatHelpAdd() routine to add help strings
for device formats, and the bootFlagHelpAdd() routine to add help strings for
the boot flags.

INCLUDE FILES: bootApp.h vxWorks.h
*/

/* includes */
#include <vxWorks.h>
#include <bootLib.h>
#include <ctype.h>
#include <fioLib.h>
#include <ioLib.h>
#include <rebootLib.h>
#include <selectLib.h>
#include <stdio.h>
#include <string.h>
#include <sysLib.h>
#include <taskLib.h>
#include <tickLib.h>

#include <bootApp.h>
#include <bootAppShell.h>
#include <bootNetLoad.h>

/* defines */

#define MAX_HELP_ENTRIES	    50 /* maximum number of help entries */
#define HELP_SIZE                   ((MAX_HELP_ENTRIES+1) * 2 * sizeof (char *))
#define MAX_DEV_FORMAT_HELP_ENTRIES 20 /* max # of Device Format help entries */
#define HELP_DEV_FORMAT_SIZE	    ((MAX_DEV_FORMAT_HELP_ENTRIES + 1) \
				     * sizeof (char *))
#define MAX_FLAGS_HELP_ENTRIES	    32 /* maximum number of flag help entries */
#define HELP_FLAG_SIZE		    ((MAX_FLAGS_HELP_ENTRIES + 1) \
				     * sizeof (char *))
#define MAX_PRINT_DEVS_ENTRIES	    50 /* max # of Print devices help entries */
#define MAX_PRINT_DEVS_SIZE	    (MAX_PRINT_DEVS_ENTRIES \
				     * sizeof (bootVoidPrototype *))

/* globals */

extern int sysStartType;
extern bootPrintHeaderPrototype * bootAppPrintHeaderFunc;
extern bootShellAutobootPrototype * bootShellAutobootFunc;
extern bootShellProcPrototype * bootAppShellFunc;
extern bootShellHelpAddPrototype * bootShellHelpAddFunc;
extern bootShellDevFormatHelpAddPrototype * bootShellDevFormatHelpAddFunc;
extern bootShellFlagHelpAddPrototype * bootShellFlagHelpAddFunc;
extern bootShellPrintfPrototype * bootShellPrintfFunc;
extern bootShellPrintDevsAddPrototype * bootShellPrintDevAddFunc;
extern bootNetSettingsUpdatePrototype * bootNetSettingsUpdateFunc;

extern int bootAppBootFileLoadAndGo (char * cmd);
extern STATUS usrBootLineCrack (char * bootString, BOOT_PARAMS * pParams);
extern BOOL printBootShellPrompt;

/* locals */

static char bootShellKey = 0;

static char * helpMsg[(MAX_HELP_ENTRIES + 1) * 2];
static char * helpDevFormatMsg[MAX_DEV_FORMAT_HELP_ENTRIES + 1];
static char * helpFlagMsg[MAX_FLAGS_HELP_ENTRIES + 1];
static bootVoidPrototype * helpPrintDevsFuncs[MAX_PRINT_DEVS_ENTRIES];
int    numberOfHelpEntries;
int    numberOfDevFormatHelpEntries;
int    numberOfPrintDevsEntries;
static fd_set bootReadFds;
static struct timeval bootSelectTimeout;

/* forward declarations */

STATUS bootAppShellInit(void);
LOCAL char * bootAppShell (char * cmd, int length);
LOCAL STATUS bootAppPrintHeader ();
LOCAL void bootAppShellAutoboot (int timeout);
LOCAL void bootAppShellHelpAdd (char * helpProto, char * helpDesc);
LOCAL void bootAppShellDevFormatHelpAdd (char * helpDesc);
LOCAL void bootAppShellFlagHelpAdd (UINT32 flag, char * helpDesc);
LOCAL void bootAppShellPrintf(char *, _Vx_usr_arg_t, _Vx_usr_arg_t,
				      _Vx_usr_arg_t, _Vx_usr_arg_t,
				      _Vx_usr_arg_t, _Vx_usr_arg_t);
LOCAL void bootAppShellPrintDevsAdd(bootVoidPrototype * pPrintDevsFunc);

LOCAL void bootAppExcMsgPrint (char *string);
LOCAL int printBootLogo (char * cmd);
LOCAL char autoboot (int timeout);
/*ADD BY HUHAHA*/
LOCAL int bootAppParamsChangeIP(char * cmd,int namecmp);
/*ADD BY HUHAHA*/
LOCAL int bootAppIPShoworChange(char * cmd);
LOCAL int bootAppFtpChange(char * cmd);
/* local command handlers */

LOCAL int printBootLogo (char * cmd);
LOCAL int bootHelp (char * cmd);
LOCAL int bootAppSysExcMsgPrint (char * cmd);
LOCAL int bootAppParamsShow (char * cmd);
LOCAL int bootAppParamsChange (char * cmd);
LOCAL int bootAppBootFileLoad (char * cmd);
LOCAL int bootAppGo (char * cmd);
LOCAL int bootAppappointedfileload( char * cmd );
LOCAL int bootAppBootLoad (char * cmd);
LOCAL int bootAppdevShow ();
/*******************************************************************************
*
* bootAppShellInit - Initializes the boot shell component
*
* This routine initializes the boot shell variables, and adds the standard set
* of boot shell commands to the command handler.  In addition, it initializes
* the boot shell device and boot flag help strings.
*
* RETURNS: OK
*
* ERRNO: N/A
*
* \NOMANUAL
*/
STATUS bootAppShellInit()
    {


    /* initialize help message area */
    bfill((char *) helpMsg, HELP_SIZE, 0);
    bfill((char *) helpDevFormatMsg, HELP_DEV_FORMAT_SIZE, 0);
    bfill((char *) helpFlagMsg, HELP_FLAG_SIZE, 0);
    bfill((char *) helpPrintDevsFuncs, MAX_PRINT_DEVS_SIZE, 0);

    /* Initialize variables */
    numberOfHelpEntries = 0;
    numberOfDevFormatHelpEntries = 0;
    numberOfPrintDevsEntries = 0;

    /* initialize bootReadFds for select */
    FD_ZERO(&bootReadFds);
    FD_SET(STD_IN, &bootReadFds);
    bootSelectTimeout.tv_sec = 0;
    bootSelectTimeout.tv_usec = 100;

    /* initialize processing routine pointer */
    bootAppShellFunc = (bootShellProcPrototype *) bootAppShell;
    bootAppPrintHeaderFunc = bootAppPrintHeader;
    bootShellAutobootFunc = (bootShellAutobootPrototype *) bootAppShellAutoboot;
    bootShellHelpAddFunc = (bootShellHelpAddPrototype *) bootAppShellHelpAdd;
    bootShellDevFormatHelpAddFunc =
	(bootShellDevFormatHelpAddPrototype *) bootAppShellDevFormatHelpAdd;
    bootShellFlagHelpAddFunc =
	(bootShellFlagHelpAddPrototype *) bootAppShellFlagHelpAdd;
    bootShellPrintfFunc = (bootShellPrintfPrototype *) bootAppShellPrintf;
    bootShellPrintDevAddFunc =
	(bootShellPrintDevsAddPrototype *) bootAppShellPrintDevsAdd;

    /* initialize commands */
    bootCommandHandlerAdd("help", bootHelp, BOOT_CMD_MATCH_STRICT,
			    "help", "- print this list");
    bootCommandHandlerAdd("h", bootHelp, BOOT_CMD_MATCH_STRICT,
			    NULL, NULL);
    /*add by huhaha*/
    bootCommandHandlerAdd("ftp", bootAppFtpChange, BOOT_CMD_MATCH_STRICT,
    			    "ftp (usr) (passwd)",  "- change ftp usr and passwd");
    	bootCommandHandlerAdd("ifconfig", bootAppIPShoworChange, BOOT_CMD_MATCH_STRICT,
    			    "ifconfig  (netDev) (IP)   ",  "- print or change target IP");
    /*bootCommandHandlerAdd("dev", bootAppdevShow, BOOT_CMD_MATCH_STRICT,
    			    "dev", "- print available bootway and device,bootParams format");*/
    /* "@" already registered */
    bootShellHelpAddFunc("L",  "- boot (load and go)");
    bootCommandHandlerAdd("p", bootAppParamsShow, BOOT_CMD_MATCH_STRICT,
			    "p",  "- print boot params");
   /* bootCommandHandlerAdd("c", bootAppParamsChange, BOOT_CMD_MATCH_STRICT,
			    "c",  "- change boot params");*/
   /*   bootCommandHandlerAdd("l", bootAppBootFileLoad, BOOT_CMD_MATCH_STRICT,
			    "l",  "- load boot file");
			    */
    bootCommandHandlerAdd("go", bootAppGo, BOOT_CMD_MATCH_STRICT,
			    "go ",  "- go to adrs");
	/*bootCommandHandlerAdd("nl", bootAppappointedfileload, BOOT_CMD_MATCH_STRICT,
			    "nl",  "load appointed boot file");
	*/
    /*bootCommandHandlerAdd("e", bootAppSysExcMsgPrint, BOOT_CMD_MATCH_STRICT,
			    "e", "- print fatal exception");
    bootCommandHandlerAdd("v", printBootLogo, BOOT_CMD_MATCH_STRICT,
			    "v", "- print boot logo with version");
			    */
  /*  bootCommandHandlerAdd("$", bootAppBootFileLoadAndGo, BOOT_CMD_MATCH_LOOSE,
			    NULL,NULL);
			    */
    bootCommandHandlerAdd("load", bootAppBootLoad, BOOT_CMD_MATCH_STRICT,
    			    "load filename type bootway (hostip)", "- load flie");
	
    /* add bootline help */
   /* bootDevFormatHelpAdd (
	    "\nBootline Format:\n"
	    "  $dev(0,procnum)host:/file h=# e=# b=# g=# u=usr [pw=passwd] f=#"
	    "\n                          tn=targetname s=script o=other"
	    "\n\nFile System Boot Device Formats:");
    */
    /* initialize flag help */
    /* Flag 0x01 not currently used
     * bootFlagHelpAdd(0x01, "  don't be system controller");
     */
    /*
    bootFlagHelpAdd(0x02, "  load local system symbols");
    bootFlagHelpAdd(0x04, "  don't autoboot");
    bootFlagHelpAdd(0x08, "  quick autoboot (no countdown)");
    bootFlagHelpAdd(0x400, "  System in debug mode");
    */
    /* print header */
    bootAppPrintHeader();

    return (OK);
    }

/*******************************************************************************
*
* bootAppPrintHeader - Prints the header for the boot loader application
*
* This routine prints the header for the boot loader application on a cold
* boot, then it displays any new system exception messages.
*
* RETURNS: OK
*
* ERRNO: N/A
*
* \NOMANUAL
*/
LOCAL STATUS bootAppPrintHeader()
    {
	printf("                                                                 \n\
	                      /---                         |                     \
	                    //////---                      |                     \
	                  //////////---                    |                     \
	                //////////////---                  |  CoreTek bootloader \
	              //////////////////---                |                     \
	             /////////////////////---              |                     \
	          /--- /////////////////////---            |                     \
	        /////--- /////////////////////---          |                     \
	      /////////--- /////////////////////---        |                     \
	    /////////////--- /////////////////////---      |  Development System \
	  /////////////////--- /////////////////////---    |                     \
	   //////////////////--- /////////////////////---  |                     \
	     /////////////--- /--- /////////////////---    |                     \
	       /////////--- /////--- /////////////---      |                     \
	         /////--  /////////--- /////////---        |                     \
	           /--  /////////////--- /////---          |  Version  0.1       \
	              /////////////////--- /--             |                     \
	               //////////////////---               |                     \
	                 //////////////---                 |                     \
	                   //////////---                   |                     \
	                     //////---                     |                     \
	                       //---                       |                     \n");
    return (OK);
    }

/*******************************************************************************
*
* bootAppShell - Boot shell command line processing
*
* This routine implements the boot shell command line processing.  It prompts
* the user for a command input, then reads the text input by the user, and
* returns a pointer to the users input string.
*
* RETURNS: The input command string
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL char * bootAppShell
    (
    char * line,    /* command line input buffer */
    int    length   /* length of input buffer */
    )
    {
    char *pLine;

    if (bootShellKey == '@')
	{
	line [0] = bootShellKey;
	line [1] = EOS;
	bootShellKey = 0;
	}
    else
	{

	/* check for valid bootReadFds */
	if (FD_ISSET(STD_IN, &bootReadFds) == 0)
	    {
	    /* STD_IN is not part of set, add it. */
	    FD_ZERO(&bootReadFds);
	    FD_SET(STD_IN, &bootReadFds);
	    }

	if (select(FD_SETSIZE, &bootReadFds, NULL, NULL, &bootSelectTimeout)
	    > 0)
	    {
	    (void)fioRdString (STD_IN, line, (size_t)length);

	    /* setup for new shell prompt.
	     * In case a return was hit without a command
	     */
	    printBootShellPrompt = TRUE;
	    }
	else
	    {
	    line[0] = EOS;
	    }
	}

    /* take blanks off end of line */

    if (strlen(line) > 0)
	{
	pLine = line + strlen (line) - 1;       /* point at last char */
	while ((pLine >= line) && (*pLine == ' '))
	    {
	    *pLine = EOS;
	    pLine--;
	    }
	}

    /* take blanks off beginning of line */

    pLine = line;
    bootSpaceSkip (&pLine);

    return pLine;
    }

/*******************************************************************************
*
* bootAppShellHelpAdd - Add help string for a boot shell command
*
* This routine adds a boot shell command help string to the list of command help
* messages output by the boot shell.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void bootAppShellHelpAdd
    (
    char * helpProto,  /* command prototype string */
    char * helpDesc    /* help description for command */
    )
    {
    if ((numberOfHelpEntries < MAX_HELP_ENTRIES) && (helpProto != NULL))
	{
	    /* save help Prototype and Description */
	    helpMsg[2*numberOfHelpEntries] = helpProto;
	    helpMsg[2*numberOfHelpEntries + 1] = helpDesc;
	    numberOfHelpEntries++;
	}
    }

/*******************************************************************************
*
* bootAppShellDevFormatHelpAdd - Add help string for a device format
*
* This routine adds a device format help string to the list of device format
* help messages output by the boot shell.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void bootAppShellDevFormatHelpAdd
    (
    char * helpDesc    /* format description for device */
    )
    {
    if ((numberOfDevFormatHelpEntries < MAX_DEV_FORMAT_HELP_ENTRIES) &&
	(helpDesc != NULL))
	{
	    /* save Device Format Description */
	    helpDevFormatMsg[numberOfDevFormatHelpEntries] = helpDesc;
	    numberOfDevFormatHelpEntries++;
	}
    }

/*******************************************************************************
*
* bootAppShellFlagHelpAdd - Add help string for a boot flag
*
* This routine adds a boot flag description string to the list of boot flag
* description strings output by the boot shell.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void bootAppShellFlagHelpAdd
    (
    UINT32 flag,      /* boot flag */
    char * helpDesc   /* help description for flag */
    )
    {
    int     bit = 0;
    UINT32  tempFlag = flag;

    /* Determine which bit flag this string applies to. */
    while ((tempFlag & 1) == 0)
	{
	tempFlag >>= 1;
	bit++;
	}

    /* save help Description */
    helpFlagMsg[bit] = helpDesc;
    }

/*******************************************************************************
*
* bootAppShellPrintDevsAdd - Add a device to the list of supported devices
*
* This routine adds a new device print routine to the list of device print
* routines executed by the boot shell.  This list of routines is executed to
* display the list of supported devices for the help command.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/
LOCAL void bootAppShellPrintDevsAdd
    (
    bootVoidPrototype * pPrintDevsFunc  /* Print device function */
    )
    {
    if (numberOfPrintDevsEntries < MAX_PRINT_DEVS_ENTRIES)
	{
	helpPrintDevsFuncs[numberOfPrintDevsEntries] = pPrintDevsFunc;
	numberOfPrintDevsEntries++;
	}
    }

/*******************************************************************************
*
* bootAppShellPrintf - Boot shell printf routine
*
* This routine executes the printf() routine passing it the specified print
* string, and six variables.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* /NOMANUAL
*/

LOCAL void bootAppShellPrintf
    (
    char *          x,  /* print format string */
    _Vx_usr_arg_t   a,  /* first print argument */
    _Vx_usr_arg_t   b,  /* second print argument */
    _Vx_usr_arg_t   c,  /* third print argument */
    _Vx_usr_arg_t   d,  /* fourth print argument */
    _Vx_usr_arg_t   e,  /* fifth print argument */
    _Vx_usr_arg_t   f   /* sixth print argument */
    )
    {
    (void) printf(x, a, b, c, d, e, f);
    }

/*******************************************************************************
*
* bootAppExcMsgPrint - Print exception message
*
* This routine is used to display exception messages, and avoids printing
* possible control characters in exception message area.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void bootAppExcMsgPrint
    (
    char *string
    )
    {
    bootAppPrintf ("\n", 0,0,0,0,0,0);

    while (isascii ((UINT) * string) && (isprint ((UINT) * string) ||
			isspace ((UINT) * string)))
	bootAppPrintf ("%c", (_Vx_usr_arg_t)*string++, 0,0,0,0,0);

    bootAppPrintf ("\n", 0,0,0,0,0,0);
    }

/******************************************************************************
*
* printBootLogo - Print initial boot banner page
*
* This routine prints the initial boot banner for the boot shell.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int printBootLogo
    (
    char * cmd
    )
    {
    bootAppPrintf ("\n\n\n\n\n\n\n\n\n\n\n",0,0,0,0,0,0);
    bootAppPrintf ("%28s%s", (_Vx_usr_arg_t)"",
		   (_Vx_usr_arg_t)"VxWorks System Boot",0,0,0,0);
    bootAppPrintf
	("\n\n\nCopyright 1984-%4.4s  Wind River Systems, Inc.\n\n\n\n\n\n",
	 (_Vx_usr_arg_t)(creationDate + 7),0,0,0,0,0);
    bootAppPrintf ("CPU: %s\n", (_Vx_usr_arg_t)((void *)sysModel()),0,0,0,0,0);
    bootAppPrintf ("Version: %s\n", (_Vx_usr_arg_t) vxWorksVersion,0,0,0,0,0);
    bootAppPrintf ("BSP version: %s\n", (_Vx_usr_arg_t)((void *)sysBspRev()),
		   0,0,0,0,0);
    bootAppPrintf ("Creation date: %s\n\n", (_Vx_usr_arg_t) creationDate,
		   0,0,0,0,0);
    return BOOT_STATUS_COMPLETE;
    }

/******************************************************************************
*
* bootAppShellAutoboot - do automatic boot sequence
*
* This routine implements the automatic boot sequence.  It first checks
* if the automatic boot routine has been turned off, and just returns if it
* has.  It then checks to see if a quick automatic boot has been configured,
* and changes the count down from 7 seconds to 1 if it has been set.  The
* autoboot() routine is then called to execute the countdown followed by
* the image boot.  If the image boot is successful, this routine does not
* return.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void bootAppShellAutoboot
    (
    int timeout     /* timeout time in seconds */
    )
    {
    BOOT_PARAMS params;

    /* start autoboot, unless no-autoboot specified */

    bootStringToStruct (sysBootLine, &params);
    sysFlags = params.flags;

    if (!(sysStartType & BOOT_NO_AUTOBOOT) &&
	!(sysFlags & SYSFLG_NO_AUTOBOOT))
	{
	if ((sysStartType & BOOT_QUICK_AUTOBOOT) ||
	    (sysFlags & SYSFLG_QUICK_AUTOBOOT))
	    {
	    timeout = 1;
	    }

	bootShellKey = autoboot (timeout);   /* doesn't return if successful */
	}

    /* did not autoboot, return */
    bootAppPrintf ("\n",0,0,0,0,0,0);
    }

/******************************************************************************
*
* autoboot - Execute the automatic boot sequence
*
* This routine executes the specified countdown, while looking for character
* input on the console, then boots the image specified by the boot line
* parameters.  If the image boot is successful, this routine does not
* return.  Otherwise, it returns the character input on the console.
*
* RETURNS: Character input on the console port.
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL char autoboot
    (
    int timeout     /* timeout time in seconds */
    )
    {
    _Vx_usr_arg_t   timeLeft;
    int             bytesRead = 0;
    int             i;
    void *          entry;
    char            key;

    /*(void) ioctl (STD_IN, FIOBAUDRATE, CONSOLE_BAUD_RATE);*/
    (void) ioctl (STD_IN, FIOSETOPTIONS,
		  OPT_ECHO | OPT_CRMOD | OPT_TANDEM | OPT_7_BIT);

    if (timeout > 0)
	{
	bootAppPrintf ("\nPress any key to stop auto-boot...\n",0,0,0,0,0,0);

	/* Loop looking for a char, or timeout after specified seconds */

	for (timeLeft = timeout; (timeLeft > 0) && (bytesRead == 0); timeLeft--)
	    {
	    bootAppPrintf ("%2d\r", timeLeft,0,0,0,0,0);
	    for (i = 10; i > 0; i--)
		{
		(void) ioctl (STD_IN, FIONREAD, (_Vx_usr_arg_t) &bytesRead);
		if (bytesRead != 0)
		    break;

		taskDelay(sysClkRateGet() / 10);
		}
	    }
	}

    if (bytesRead == 0)    /* nothing typed so auto-boot */
	{
	/* put the console back in line mode so it echoes (so's you can bang
	 * on it to see if it's still alive) */

	(void) ioctl (STD_IN, FIOSETOPTIONS, OPT_TERMINAL);

	bootAppPrintf ("\nauto-booting...\n\n",0,0,0,0,0,0);

	if (bootLoad (sysBootLine, &entry) == OK)
	    executeImage (entry);             /* ... and never return */
	else
	    {
	    bootAppPrintf ("Can't load boot file!!\n",0,0,0,0,0,0);
	    taskDelay (sysClkRateGet ());   /* pause a second */
	    reboot (BOOT_NO_AUTOBOOT);      /* something is awry */
	    }

	/* should never get here */
	key = (char) ERROR;
	}
    else
	{

	/*
     *  read the key that stopped autoboot 
     *  don't need return from function call
     */

	(void) read (STD_IN, &key, 1);
	key = (char)((int)key & 0x7f);       /* mask off parity in raw mode */
	}

    /* put it back into line mode */
    (void) ioctl (STD_IN, FIOSETOPTIONS, OPT_TERMINAL);

    return (key);
    }

/*******************************************************************************
*
* bootHelp - Display the boot shell help
*
* This routine is the command handler for the '?' command.  This routine
* displays the help strings associated with the boot shell commands, followed
* by the device format strings associated with the devices, followed by the
* help strings associated with the boot flags, and finally the device print
* routines are executed to display information on the devices.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootHelp
    (
    char * cmd
    )
    {
    FAST char **pMsg;
  /*  int offset;*/
   /* UINT32 flag;*/

    /* print command help */
    bootAppPrintf ("\nCommands:\n",0,0,0,0,0,0);
    for (pMsg = helpMsg; *pMsg != NULL; pMsg += 2)
	{
	bootAppPrintf (" %-35s %s\n", (_Vx_usr_arg_t) *pMsg,
				      (_Vx_usr_arg_t) *(pMsg + 1),
				      0,0,0,0);
	}
    bootAppdevShow();

    /* print Device Format help */
  /*  for (pMsg = helpDevFormatMsg; *pMsg != NULL; pMsg += 1)
	{
	bootAppPrintf (" %s\n", (_Vx_usr_arg_t) *pMsg, 0,0,0,0,0);
	}
    */
    /* print boot flags */
  /*  bootAppPrintf("\nBoot Flags:\n",0,0,0,0,0,0);
    flag = 1;
    for (offset = 0; offset < MAX_FLAGS_HELP_ENTRIES; offset++, flag <<= 1)
	{
	if (helpFlagMsg[offset] != NULL)
	bootAppPrintf ("  0x%4.4x - %s\n", (_Vx_usr_arg_t) flag,
					   (_Vx_usr_arg_t) helpFlagMsg[offset],
					   0,0,0,0);
	}
    */
    /* print the devices */

/*    bootAppPrintf ("\nAvailable Boot Devices:",0,0,0,0,0,0);
    for (offset = 0; offset < numberOfPrintDevsEntries; offset++)
	{

	if (helpPrintDevsFuncs[offset] != NULL)
	    helpPrintDevsFuncs[offset]();
	}*/

    bootAppPrintf ("\n",0,0,0,0,0,0);
    return BOOT_STATUS_COMPLETE;
    }
/*********************************************************************
 *                          bootAppdevShow
 */LOCAL int bootAppdevShow ()
 {
	 int offset;

	 bootAppPrintf ("\nAvailable Boot Devices:",0,0,0,0,0,0);
	    for (offset = 0; offset < numberOfPrintDevsEntries; offset++)
		{
		/* call print devices function */
		if (helpPrintDevsFuncs[offset] != NULL)
		    helpPrintDevsFuncs[offset]();
		}
	    printf("\n\nbootway:\n");
	    printf("  tffs\n");
	    printf("  tftp  ftp\n");

	    printf("\n\nall bootParams:\n");
	    printf("  bootDev\n");
	    printf("  unitNum\n");
	    printf("  target ip\n");
	    printf("  host ip\n");
	    printf("  ftp username\n");
	    printf("  ftp password\n");
	    printf("  Type\n");


	    bootAppPrintf ("\n",0,0,0,0,0,0);
	    return BOOT_STATUS_COMPLETE;


 }





/************************************************************************ */






/******************************************************************************
*
* bootAppSysExcMsgPrint - Display system exception message
*
* This routine is the command handler for the 'e' boot shell command.  It
* displays the exception message from the system exception message string.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppSysExcMsgPrint
    (
    char * cmd
    )
    {
    bootAppExcMsgPrint(sysExcMsg+1);
    return BOOT_STATUS_COMPLETE;
    }

/******************************************************************************
*
* bootAppParamsShow - Print boot parameters
*
* This routine is the command handler for the 'p' boot shell command.  It
* displays the current settings for each of the boot parameters.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppParamsShow
    (
    char * cmd
    )
    {

    bootParamsShow (sysBootLine);
    return BOOT_STATUS_COMPLETE;
    }

/******************************************************************************
*
* bootAppParamsChange - Change boot parameters
*
* This routine is the command handler for the 'c' boot shell command.  It
* loops through each boot parameter, displaying the current settings for the
* parameter, and waiting a new value to be input before moving to the next
* boot parameter.  Once all of the boot parameters have been processed, the
* new boot parameters are written to NVRAM, and the network configuration is
* updated with the changes.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppParamsChange
    (
    char * cmd
    )
    {
    STATUS  rtv;
    BOOT_PARAMS oldBootParams;

    bootParamsPrompt (sysBootLine);
    if (strlen(sysBootLine) <= BOOT_CMD_MAX_LINE)
	{
	rtv = sysNvRamSet (sysBootLine, (int)strlen(sysBootLine) + 1, 0);
	if (rtv == ERROR)
	    {
	    bootAppPrintf("\nNOTE: Bootline not saved to NVRAM\n",0,0,0,0,0,0);
	    }

	if (bootNetSettingsUpdateFunc != NULL)
	    {
	    /* save old network settings */
	    memcpy(&oldBootParams, &sysBootParams, sizeof(BOOT_PARAMS));
	    }

	rtv = usrBootLineCrack(sysBootLine, &sysBootParams);
	if (rtv == ERROR)
	    {
	    bootAppPrintf("\nNOTE: Error parsing Bootline.  "
			  "Settings may be corrupted.\n",0,0,0,0,0,0);
	    return BOOT_STATUS_COMPLETE;
	    }

	/* update network settings */
	if (bootNetSettingsUpdateFunc != NULL)
	    {
	    /* networking enabled, setup network settings */
	    (void) bootNetSettingsUpdateFunc(&oldBootParams, &sysBootParams);
	    }
	}
    else
	bootAppPrintf("\nWARNING:Bootline longer than BOOT_LINE_SIZE "
		      "(%d bytes). Not saved to NVRAM\n",
		      (_Vx_usr_arg_t) BOOT_CMD_MAX_LINE,0,0,0,0,0);

    return BOOT_STATUS_COMPLETE;
    }

/******************************************************************************
*
* bootAppBootFileLoad - Load boot file
*
* This routine is the command handler for the 'l' boot shell command.  It
* loads the file the file specified by the boot parameters into memory.  If
* the load operation fails, this routine delays for one second, then reboots
* with the system start type set to disable the auto boot routine.  If the
* file is loaded successfully, then this routine returns normally.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppBootFileLoad
    (
    char * cmd
    )
    {
    void * entry;

    /* skip the command name and point to first parameter */
    bootCmdNameSkip (&cmd, 1);

    if (bootLoad (cmd, &entry) == OK)
	{

	}
    else
	{
	taskDelay (sysClkRateGet ());   /* pause a second */
	reboot (BOOT_NO_AUTOBOOT);      /* something is awry */
	}

    return BOOT_STATUS_COMPLETE;
    }

/******************************************************************************
*
* bootAppGo - Go to Address
*
* This routine is the command handler for the 'g' boot shell command.  It
* causes execution to begin at the address specified on the command line.
*
* RETURNS: BOOT_STATUS_COMPLETE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int bootAppGo
    (
    char * cmd
    )
    {
    /*void *  entry;*/
	if(ShellGOEntryAddress==0)
	{
		printf("error: please load first\n");
		 return BOOT_STATUS_COMPLETE;
	}
    /* skip the command name and point to first parameter */
    /*bootCmdNameSkip (&cmd, 2);*/
	executeImage (ShellGOEntryAddress);
	/*
    if (bootGetArg (&cmd, (_Vx_usr_arg_t *) &entry, HEX, !OPT) == OK)
	{
	executeImage (entry);
	}
	*/
    return BOOT_STATUS_COMPLETE;
    }
/*********************************************************************************************/
	LOCAL int bootAppappointedfileload( char * cmd )
		{
		void * entry;

    /* skip the command name and point to first parameter */

		    bootCmdNameSkip (&cmd, 2);


    if (bootappointedfileLoad (cmd, &entry) == OK)
	{
	bootAppPrintf ("entry = %p\n", (_Vx_usr_arg_t) entry,0,0,0,0,0);
	}
    else
	{
	taskDelay (sysClkRateGet ());   /* pause a second */
	reboot (BOOT_NO_AUTOBOOT);      /* something is awry */
	}

    return BOOT_STATUS_COMPLETE;

		
		}
/**********************************************************************************/


	/*********************************************************************************************/
		LOCAL int bootAppBootLoad( char * cmd )
			{
			void * entry;

	    /* skip the command name and point to first parameter */

			    bootCmdNameSkip (&cmd, 4);


	    if (bootfileLoad (cmd, &entry) == OK)
		{

		}
	    else
		{
		taskDelay (sysClkRateGet ());   /* pause a second */
		reboot (BOOT_NO_AUTOBOOT);      /* something is awry */
		}

	    return BOOT_STATUS_COMPLETE;


			}
	/**********************************************************************************/



		/*ADD BY HUHAHA*/
		LOCAL int bootAppParamsChangeIP
		    (
		    char * cmd,
		    int namecmp
		    )
		    {

		    STATUS  rtv;
		    BOOT_PARAMS oldBootParams;
		    char buf [BOOT_FIELD_LEN];
			int unitname=0;

			if(namecmp==1)
			{
				unitname=*(cmd+5)-'0';
				if(unitname>=4 && unitname<0)
				{
					printf("error:illegal Device number\n");
					return BOOT_STATUS_COMPLETE;
				}
				bootCmdNameSkip (&cmd, 6);
			}


			strcpy(buf,cmd);

		    char *  pStrStart;
		    char *  pStrEnd;
		    //bootParamsPrompt (sysBootLine);
			BOOT_PARAMS bp;

		    /* interpret the boot parameters */

		    (void) bootStringToStruct (sysBootLine, &bp);

			 /* skip leading white space */
		    pStrStart = &buf[0];
		    while ((isspace ((UINT) *pStrStart) != 0) && (*pStrStart != EOS))
		        {
		        pStrStart++;
		        }

		    /* find beginning of white space at end of string */
		    pStrEnd = &buf[strlen(buf) - 1];
		    while ((isspace ((UINT) *pStrEnd) != 0) && (pStrEnd > pStrStart))
		        {
		        pStrEnd--;
		        }
		    pStrEnd++;

		    /* remove trailing white space */
		    *pStrEnd = EOS;

		    strncpy (bp.ead, pStrStart, sizeof (bp.ead));	/* update parameter */
			strcpy (bp.bootDev,"qefcc" );
			if(bp.unitNum!=unitname)
				bp.unitNum=unitname;
		    bp.ead[sizeof (bp.ead) - 1] = EOS;
		    (void) bootStructToString (sysBootLine, &bp);

		    if (strlen(sysBootLine) <= BOOT_CMD_MAX_LINE)
			{
			rtv = sysNvRamSet (sysBootLine, (int)strlen(sysBootLine) + 1, 0);
			if (rtv == ERROR)
			    {
			    bootAppPrintf("\nNOTE: Bootline not saved to NVRAM\n",0,0,0,0,0,0);
			    }

			if (bootNetSettingsUpdateFunc != NULL)
			    {
			    /* save old network settings */
			    memcpy(&oldBootParams, &sysBootParams, sizeof(BOOT_PARAMS));
			    }

			rtv = usrBootLineCrack(sysBootLine, &sysBootParams);
			if (rtv == ERROR)
			    {
			    bootAppPrintf("\nNOTE: Error parsing Bootline.  "
					  "Settings may be corrupted.\n",0,0,0,0,0,0);
			    return BOOT_STATUS_COMPLETE;
			    }

			/* update network settings */
			if (bootNetSettingsUpdateFunc != NULL)
			    {
			    /* networking enabled, setup network settings */
			    (void) bootNetSettingsUpdateFunc(&oldBootParams, &sysBootParams);
			    }
			}
		    else
			bootAppPrintf("\nWARNING:Bootline longer than BOOT_LINE_SIZE "
				      "(%d bytes). Not saved to NVRAM\n",
				      (_Vx_usr_arg_t) BOOT_CMD_MAX_LINE,0,0,0,0,0);

		    return BOOT_STATUS_COMPLETE;
		    }



		LOCAL int bootAppFtpChange
						(
						char * cmd
						)
						{
							BOOT_PARAMS bps;
							BOOT_PARAMS oldBootParams;
							(void) bootStringToStruct (sysBootLine, &bps);
							if(strlen(cmd)<=4)
							{
								printf("user name:   %s\n",bps.usr);
								printf("passwd:   %s\n",bps.passwd);
								return BOOT_STATUS_COMPLETE;
							}
							else
							{
								STATUS  rtv;
				    BOOT_PARAMS oldBootParams;

				    BOOT_PARAMS bp;
					(void) bootStringToStruct (sysBootLine, &bp);
					bootCmdNameSkip (&cmd, 3);
					int count_cmd=0;
					char bufuser[15]={0};
					char bufpasswd[15]={0};

					char *para_cmd=cmd;
					char *skip_space=NULL;
					for(count_cmd=0;*para_cmd!=' ';count_cmd++,para_cmd++)
					{
						 bufuser[count_cmd]=*para_cmd;
					}
					strcpy(bp.usr,bufuser);
					if((skip_space=strstr(cmd," "))==NULL)
					{
						printf("error:enter passwd\n");
						return BOOT_STATUS_COMPLETE;
					}
					bootCmdNameSkip (&skip_space, 0);
					para_cmd=skip_space;
					for(count_cmd=0;*para_cmd!=NULL;count_cmd++,para_cmd++)
					{
				 bufpasswd[count_cmd]=*para_cmd;
					}
					strcpy(bp.passwd,bufpasswd);
				    /* interpret the boot parameters */

					(void) bootStructToString (sysBootLine, &bp);

				    if (strlen(sysBootLine) <= BOOT_CMD_MAX_LINE)
					{
					rtv = sysNvRamSet (sysBootLine, (int)strlen(sysBootLine) + 1, 0);
					if (rtv == ERROR)
					    {
					    bootAppPrintf("\nNOTE: Bootline not saved to NVRAM\n",0,0,0,0,0,0);
					    }

					if (bootNetSettingsUpdateFunc != NULL)
					    {
					    /* save old network settings */
					    memcpy(&oldBootParams, &sysBootParams, sizeof(BOOT_PARAMS));
					    }

					rtv = usrBootLineCrack(sysBootLine, &sysBootParams);
					if (rtv == ERROR)
					    {
					    bootAppPrintf("\nNOTE: Error parsing Bootline.  "
							  "Settings may be corrupted.\n",0,0,0,0,0,0);
					    return BOOT_STATUS_COMPLETE;
					    }

					/* update network settings */
					if (bootNetSettingsUpdateFunc != NULL)
					    {
					    /* networking enabled, setup network settings */
					    (void) bootNetSettingsUpdateFunc(&oldBootParams, &sysBootParams);
					    }
					}
				    else
					bootAppPrintf("\nWARNING:Bootline longer than BOOT_LINE_SIZE "
						      "(%d bytes). Not saved to NVRAM\n",
						      (_Vx_usr_arg_t) BOOT_CMD_MAX_LINE,0,0,0,0,0);

					(void) bootStringToStruct (sysBootLine, &bps);
					printf("new user name:   %s\n",bps.usr);
					printf("newpasswd:   %s\n",bps.passwd);
					return BOOT_STATUS_COMPLETE;
						}
					}

		LOCAL int bootAppIPShoworChange
				    (
				    char * cmd
				    )
				    {
				    BOOT_PARAMS bps;
					int compareCmdWithDevName=1;
				    if(strlen(cmd)<=9)
				    {
						(void) bootStringToStruct (sysBootLine, &bps);
						printf("ethernet IP:   %s\n",bps.ead);
						printf("Network adapter:   %s%d\n",bps.bootDev,bps.unitNum);
						return BOOT_STATUS_COMPLETE;
				    }
					else
					{
				    	/* skip the command name and point to first parameter */
				    	bootCmdNameSkip (&cmd, 8);
						char *verify_cmd=cmd;
						if(strstr(cmd," ")==NULL)
						{
							while((*verify_cmd)!=NULL)
							{
							if((((*verify_cmd)<'0') || ((*verify_cmd)>'9')) && ((*verify_cmd)!='.'))
							{
								printf("debug:verify_cmd:%s\n",verify_cmd);
								printf("error:illegal IP\n");
								return BOOT_STATUS_COMPLETE;
							}
							verify_cmd++;
							}
						}
						else
						{
							verify_cmd=strstr(cmd," ");
							bootCmdNameSkip (&verify_cmd, 0);
							if((*verify_cmd)==NULL)
							{
								verify_cmd=cmd;
								while((*verify_cmd)!=NULL)
								{
								if(((((*verify_cmd)<'0') || ((*verify_cmd)>'9')) && ((*verify_cmd)!='.'))&&((*verify_cmd)!=' '))
								{
									printf("debug:verify_cmd:%s\n",verify_cmd);
									printf("error:illegal IP.\n");
									return BOOT_STATUS_COMPLETE;
								}
								verify_cmd++;
								}
							}
							else
							{
								while((*verify_cmd)!=NULL)
								{
								if(((((*verify_cmd)<'0') || ((*verify_cmd)>'9')) && ((*verify_cmd)!='.'))&&((*verify_cmd)!=' '))
								{
									printf("debug:verify_cmd:%s\n",verify_cmd);
									printf("error:illegal IP..\n");
									return BOOT_STATUS_COMPLETE;
								}
								verify_cmd++;
								}
								verify_cmd=cmd;
								if(strncmp(verify_cmd,"qefcc",5)!=0)
								{
									printf("error:illegal Network adapter\n");
									return BOOT_STATUS_COMPLETE;
								}
							}
						}
						if(strncmp(cmd,"qefcc",5)!=0)
							compareCmdWithDevName=0;
						bootAppParamsChangeIP(cmd,compareCmdWithDevName);
				    	//bootParamsShowIP (sysBootLine);
				    	(void) bootStringToStruct (sysBootLine, &bps);
				    	printf("target IP:   %s\n",bps.ead);
				    	printf("Network adapter:   %s%d\n",bps.bootDev,bps.unitNum);
				    	return BOOT_STATUS_COMPLETE;
					}
				    }
