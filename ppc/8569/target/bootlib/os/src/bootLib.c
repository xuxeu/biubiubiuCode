/* bootLib.c - boot ROM subroutine library */

/*
 * Copyright (c) 1995-2004, 2008-2009, 2012 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
03d,07mar12,wqi  Remove '\0' and 'CR' from the string length count in function
                 promptRead. (WIND00261246)
03c,09jan12,pgh  Fix strcpy usage.
03b,16dec09,pgh  64-bit conversion
03a,29jul09,pgh  Strip space characters from boot parameters.
02z,11mar08,to   include taskLibP.h to use taskIdCurrent for UP
02y,06apr06,dgp  doc: fix SEE ALSO formatting
02x,09sep04,job  Refactored to move bootline parsing routines to bootParseLib.c
                 Added RTP bootline startup examples to docs.
                 Removed some GNU compiler warnings.
02w,21oct03,bpn  Added bootStringToStructAdd().
02v,13mar02,jkf  Fixing SPR#74251, using bootLib.h macros for array sizes
                 instead of numeric constants.  Changed copyright to 2002.
02u,30mar99,jdi  doc: corrected misspelling of NOMANUAL which caused
                 bootScanNum() to publish.
02t,28mar99,jdi  doc: fixed tiny formatting errors in .iP section.
02s,14mar99,jdi  doc: removed refs to config.h and/or configAll.h (SPR 25663).
02r,04feb99,dbt  when bootParamsShow() routine is called without any
                 parameter, print an error message (Fixed SPR #24885).
02q,26aug97,spm  modified bootLeaseExtract to allow improved treatment of DHCP
                 address information by startup routines
02p,27nov96,spm  added DHCP client and support for network device unit numbers.
02p,12sep97,dgp  doc: fix SPR 5330 boot line has required parameters
02o,19jun96,dbt  fixed spr 6691. Modified function promptParamString in order
                 to take into account the size of the field to read
                 Modified promptRead to read the entire string even if it is
                 too long. This is to empty the reception buffer of the
                 standard input.
                 Update copyright.
02n,14oct95,jdi  doc: revised pathnames for Tornado.
02m,16feb95,jdi  doc format corrections.
02l,12jan93,jdi  fixed poor style in bootParamsPrompt().
02k,20jan93,jdi  documentation cleanup for 5.1.
02j,28jul92,rdc  made bootStructToString write enable memory before writing.
02i,26may92,rrr  the tree shuffle
02h,01mar92,elh  changed printParamNum to take an int param (instead of char).
02g,05dec91,rrr  shut up some ansi warnings.
02f,19nov91,rrr  shut up some ansi warnings.
02e,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
                  -changed includes to have absolute path from h/
                  -changed VOID to void
                  -changed copyright notice
02d,30apr91,jdi	 documentation tweak.
02c,05apr91,jdi	 documentation -- removed header parens and x-ref numbers;
		 doc review by dnw.
02b,24mar91,jaa	 documentation.
02a,15jul90,dnw  added "targetName", "startupScript", and "other" fields
                 replaced bootParamsToString() with bootStructToString()
                 replaced bootStringToParams() with bootStringToStruct()
                 changed to initialize procNum to 0 instead of NONE
                 routines accept and generate boot strings w/o host & filename
                 (required for bootp)
                 removed clearFlag from promptParamString - now all fields
                 can be cleared
                 promptParamNum now accepts "." to be clear (0)
                 added bootParamsErrorPrint
01r,10jul90,dnw  lint clean-up, esp to allow void to be void one day
                 fixed incorrect declaration of promptParamString() &
                 promptParamNum(); was void, now int
                 declared to return int instead of void
                 changed name of scanNum() to bootScanNum() to not conflict
                 with routine in fioLib.c
01q,02jan90,dab  fixed bootStringsToParams() bug in parsing vbnum value.
                 disabled clearing of boot device, host name, or boot file
                 fields.
01p,23may89,dnw  made promptRead be LOCAL.
01o,20aug88,gae  documentation
01n,30may88,dnw  changed to v4 names.
01m,29may88,dnw  changed calls to atoi() to sscanf().
01l,28may88,dnw  moved skipSpace here from fioLib.
                 changed calls to fioStdIn to STD_IN.
                 made promptParam...() LOCAL again (copies in nfsLib now).
01k,19apr88,llk  made promptParamNum and promptParamString global routines.
01j,19feb88,dnw  added bootExtractNetmask().
01i,18nov87,ecs  lint.
                 added include of strLib.h.
                 documentation.
01h,15nov87,dnw  changed to print '?' for unprintable chars in parameters.
01g,06nov87,jlf  documentation
01f,13oct87,dnw  added flags field to boot line encoding and decoding.
01e,09oct87,jlf  changed to deal with new ISI boot format with ethernet adrs.
01d,14may87,rdc  procnum can now be defined by a line VBNUM=n or VB=n for
           +dnw    compatibility with isi bootproms.
                 changed prompt to indicate password is for ftp only.
                 fixed bootEncodeParams() to add EOS.
01c,23mar87,jlf  documentation.
                 changed routine names to meet naming conventions.
01b,20dec86,dnw  added promptBootParams().
                 changed to not get include files from default directories.
01a,18dec86,llk  created.
*/

/*
DESCRIPTION
This library contains routines for manipulating a boot line.
Routines are provided to construct, print, and prompt for a boot line.

When VxWorks is first booted, certain parameters can be specified,
such as network addresses, boot device, host, and start-up file.
This information is encoded into a single ASCII string known as the boot line.
The boot line is placed at a known address (specified in config.h)
by the boot ROMs so that the system being booted can discover the parameters
that were used to boot the system.
The boot line is the only means of communication from the boot ROMs to
the booted system.

The boot line is of the form:
.CS
bootdev(unitnum,procnum)hostname:filename e=# b=# h=# g=# u=userid pw=passwd f=#
tn=targetname s=startupscript#rtp.vxe o=other
.CE
where:
.iP <bootdev> 12
the boot device (required); for example, "ex" for Excelan Ethernet, "bp" for
backplane.  For the backplane, this field can have an optional anchor
address specification of the form "bp=<adrs>" (see bootBpAnchorExtract()).
.iP <unitnum>
the unit number of the boot device (0..n).
.iP <procnum>
the processor number on the backplane, 0..n (required for VME boards).
.iP <hostname>
the name of the boot host (required).
.iP <filename>
the file to be booted (required).
.iP "e" "" 3
the Internet address of the Ethernet interface.
This field can have an optional subnet mask
of the form <inet_adrs>:<subnet_mask>. If DHCP
is used to obtain the configuration parameters,
lease timing information may also be present.
This information takes the form <lease_duration>:<lease_origin>
and is appended to the end of the field.
(see bootNetmaskExtract() and bootLeaseExtract()).
.iP "b"
the Internet address of the backplane interface.
This field can have an optional subnet mask
and/or lease timing information as "e".
.iP "h"
the Internet address of the boot host.
.iP "g"
the Internet address of the gateway to the boot host.
Leave this parameter blank if the host is on same network.
.iP "u"
a valid user name on the boot host.
.iP "pw"
the password for the user on the host.
This parameter is usually left blank.
If specified, FTP is used for file transfers.
.iP "f"
the system-dependent configuration flags.
This parameter contains an `or' of option bits defined in
sysLib.h.
.iP "tn"
the name of the system being booted
.iP "s"
the name of a file to be executed as a start-up script. In addition, if a #
separator is used, this parameter can contain a list of RTPs to start.
.iP "o"
"other" string for use by the application.
.LP
The Internet addresses are specified in "dot" notation (e.g., 90.0.0.2).
The order of assigned values is arbitrary.

EXAMPLE
.CS
enp(0,0)host:/usr/wpwr/target/config/mz7122/vxWorks e=90.0.0.2 b=91.0.0.2
 h=100.0.0.4    g=90.0.0.3 u=bob pw=realtime f=2 tn=target
 s=host:/usr/bob/startup#/romfs/helloworld.vxe o=any_string
.CE

INCLUDE FILES: bootLib.h

SEE ALSO: 'bootConfig', bootParseLib.c, usrRtpAppInitBootline.c
*/

/* LINTLIBRARY */

#include "vxWorks.h"
#include "ctype.h"
#include "string.h"
#include "sysLib.h"
#include "bootLib.h"
#include "stdio.h"
#include "fioLib.h"
#include "private/vmLibP.h"
#include "private/taskLibP.h"

#define PARAM_PRINT_WIDTH	21
#define PRINT_NUM_PARAM_WIDTH   31
#define PRINT_STR_PARAM_WIDTH   175

LOCAL char strBootDevice []     = "boot device";
LOCAL char strHostName []	= "host name";
LOCAL char strTargetName []	= "target name (tn)";
LOCAL char strFileName []	= "file name";
LOCAL char strInetOnEthernet []	= "target IP";
LOCAL char strInetOnBackplane []= "inet on backplane (b)";
LOCAL char strHostInet []	= "host IP";
LOCAL char strGatewayInet []	= "gateway inet (g)";
LOCAL char strUser []		= "ftp user";
LOCAL char strFtpPw []		= "ftp password ";
LOCAL char strFtpPwLong []	= "ftp password (pw) (blank = use rsh)";
LOCAL char strUnitNum []        = "unit number";
LOCAL char strProcNum []	= "processor number";
LOCAL char strFlags []		= "flags (f)";
LOCAL char strStartup []	= "startup script (s)";
LOCAL char strOther []		= "other (o)";
/*add by huhaha*/
LOCAL char strType []		= "Type";



/* forward static functions */

static void addAssignNum (char *string, char *code, int value);
static void addAssignString (char *string, char *code, char *value);
static void printClear (char *param);
static void printParamNum (char *paramName, int value, BOOL hex);
static void printParamString (char *paramName, char *param);
static int  promptParamBootDevice (char *paramName, char *param, int *pValue, 
                                   int sizeParamName);
static int  promptParamString (char *paramName, char *param, int sizeParamName);
static int  promptParamNum (char *paramName, int *pValue, BOOL hex);
static int  promptRead (char *buf, int bufLen);

/*******************************************************************************
*
* bootStructToString - construct a boot line
*
* This routine encodes a boot line using the specified boot parameters.
*
* For a description of the format of the boot line, see the manual
* entry for bootLib.
*
* RETURNS: OK.
*/

STATUS bootStructToString
    (
    char *paramString,		    /* where to return the encoded boot line */
    FAST BOOT_PARAMS *pBootParams   /* boot line structure to be encoded */
    )
    {
    UINT        state;
    BOOL        writeProtected = FALSE;
    size_t      pageSize = 0;
    VIRT_ADDR   pageAddr = (VIRT_ADDR)NULL;

    /* see if we need to write enable the memory */

    if (vmLibInfo.vmLibInstalled)
	{
	pageSize = VM_PAGE_SIZE_GET();

	pageAddr = (VIRT_ADDR)(((ptrdiff_t)paramString / (ptrdiff_t)pageSize) * 
                               (ptrdiff_t)pageSize);

	if (VM_STATE_GET (NULL, pageAddr, &state) != ERROR)
	    if ((state & VM_STATE_MASK_WRITABLE) == VM_STATE_WRITABLE_NOT)
		{
		writeProtected = TRUE;
		TASK_SAFE();
		VM_STATE_SET (NULL, pageAddr, pageSize, VM_STATE_MASK_WRITABLE,
			      VM_STATE_WRITABLE);
		}

	}

    if ((pBootParams->hostName[0] == EOS) && (pBootParams->bootFile[0] == EOS))
        (void)snprintf (paramString, MAX_BOOT_LINE_SIZE, "%s(%d,%d)", 
                        pBootParams->bootDev, pBootParams->unitNum, 
                        pBootParams->procNum);
    else
        (void)snprintf (paramString, MAX_BOOT_LINE_SIZE, "%s(%d,%d)%s:%s", 
                        pBootParams->bootDev, pBootParams->unitNum, 
                        pBootParams->procNum, pBootParams->hostName, 
                        pBootParams->bootFile);

    addAssignString (paramString, "e", pBootParams->ead);
    addAssignString (paramString, "b", pBootParams->bad);
    addAssignString (paramString, "h", pBootParams->had);
    addAssignString (paramString, "g", pBootParams->gad);
    addAssignString (paramString, "u", pBootParams->usr);
    addAssignString (paramString, "pw", pBootParams->passwd);
    addAssignNum (paramString, "f", pBootParams->flags);
    addAssignString (paramString, "tn", pBootParams->targetName);
    addAssignString (paramString, "s", pBootParams->startupScript);
    addAssignString (paramString, "o", pBootParams->other);
	addAssignString (paramString, "Type", pBootParams->Type);

    if (writeProtected)
	{
	VM_STATE_SET (NULL, pageAddr, pageSize,
		      VM_STATE_MASK_WRITABLE, VM_STATE_WRITABLE_NOT);
	TASK_UNSAFE();
	}

    return (OK);
    }

/*******************************************************************************
*
* bootParamsShow - display boot line parameters
*
* This routine displays the boot parameters in the specified boot string
* one parameter per line.
*
* RETURNS: N/A
*/

void bootParamsShow
    (
    char *paramString           /* boot parameter string */
    )
    {
    BOOT_PARAMS bootParams;
    char *pS;

    if (paramString == NULL)
	{
	printf ("No boot parameter string specified.\n");
	return;
	}

    pS = bootStringToStruct (paramString, &bootParams);
    if (*pS != EOS)
	{
	bootParamsErrorPrint (paramString, pS);
	return;
	}

    printf ("\n");

    printParamString (strBootDevice, bootParams.bootDev);
    printParamNum (strUnitNum, bootParams.unitNum, FALSE);
   // printParamNum (strProcNum, bootParams.procNum, FALSE);
   // printParamString (strHostName, bootParams.hostName);
    printParamString (strFileName, bootParams.bootFile);
    printParamString (strInetOnEthernet, bootParams.ead);
    //printParamString (strInetOnBackplane, bootParams.bad);
    printParamString (strHostInet, bootParams.had);
   //printParamString (strGatewayInet, bootParams.gad);
    printParamString (strUser, bootParams.usr);
    printParamString (strFtpPw, bootParams.passwd);
   // printParamNum (strFlags, bootParams.flags, TRUE);
    //printParamString (strTargetName, bootParams.targetName);
    //printParamString (strStartup, bootParams.startupScript);
    //printParamString (strOther, bootParams.other);
	printParamString (strType, bootParams.Type);
    printf ("\n");
    }

//ADD BY HUHAHA
void bootParamsShowIP
    (
    char *paramString           /* boot parameter string */
    )
    {
    BOOT_PARAMS bootParams;
    char *pS;

    if (paramString == NULL)
	{
	printf ("No boot parameter string specified.\n");
	return;
	}

    pS = bootStringToStruct (paramString, &bootParams);
    if (*pS != EOS)
	{
	bootParamsErrorPrint (paramString, pS);
	return;
	}

    printParamString (strInetOnEthernet, bootParams.ead);
   

    printf ("\n");
    }


/*******************************************************************************
*
* bootParamsPrompt - prompt for boot line parameters
*
* This routine displays the current value of each boot parameter and prompts
* the user for a new value.  Typing a RETURN leaves the parameter unchanged.
* Typing a period (.) clears the parameter.
*
* The parameter <string> holds the initial values.  The new boot line is
* copied over <string>.  If there are no initial values, <string> is
* empty on entry.
*
* RETURNS: N/A
*/

void bootParamsPrompt
    (
    char *string        /* default boot line */
    )
    {
    BOOT_PARAMS bp;
    FAST int n = 0;
    FAST int i;

    /* interpret the boot parameters */

    (void) bootStringToStruct (string, &bp);

    printf ("\n'.' = clear field;  '-' = go to previous field;  ^D = quit\n\n");

    /* prompt the user for each item;
     *   i:  0 = same field, 1 = next field, -1 = previous field,
     *     -98 = error, -99 = quit
     */

    FOREVER
	{
	switch (n)
	    {
	    case 0:  i = promptParamBootDevice (strBootDevice, bp.bootDev,
                                &bp.unitNum, sizeof (bp.bootDev));  break;
	    case 1:  i = promptParamNum (strProcNum, &bp.procNum, FALSE);break;
	    case 2:  i = promptParamString (strHostName, bp.hostName,
				sizeof (bp.hostName));	break;
	    case 3:  i = promptParamString (strFileName, bp.bootFile,
				sizeof (bp.bootFile));	break;
	    case 4:  i = promptParamString (strInetOnEthernet, bp.ead,
				sizeof (bp.ead));	break;
	    case 5:  i = promptParamString (strInetOnBackplane, bp.bad,
				sizeof (bp.bad));break;
	    case 6:  i = promptParamString (strHostInet, bp.had,
				sizeof (bp.had));	break;
	    case 7:  i = promptParamString (strGatewayInet, bp.gad,
				sizeof (bp.gad));	break;
	    case 8:  i = promptParamString (strUser, bp.usr,
				sizeof (bp.usr));		break;
	    case 9:  i = promptParamString (strFtpPwLong, bp.passwd,
				sizeof (bp.passwd));	break;
	    case 10: i = promptParamNum (strFlags, &bp.flags, TRUE);	break;
	    case 11: i = promptParamString (strTargetName, bp.targetName,
				sizeof (bp.targetName));break;
	    case 12: i = promptParamString (strStartup, bp.startupScript,
				sizeof (bp.startupScript));break;
	    case 13: i = promptParamString (strOther, bp.other,
				sizeof (bp.other));	break;
	    default: i = -99; break;
	    }

	/* check for QUIT */

	if (i == -99)
	    {
	    printf ("\n");
	    break;
	    }

	/* move to new field */

	if (i != -98)
	    n += i;
	}

    (void) bootStructToString (string, &bp);
    }

/*******************************************************************************
*
* addAssignNum - add a numeric value assignment to a string
*/

LOCAL void addAssignNum
    (
    FAST char *string,
    char *code,
    int value
    )
    {
    if (value != 0)
	{
	string += strlen (string);
        (void)snprintf (string, PRINT_NUM_PARAM_WIDTH, 
                        (value <= 7) ? " %s=%d" : " %s=0x%x", code, value);
	}
    }

/*******************************************************************************
*
* addAssignString - add a string assignment to a string
*/

LOCAL void addAssignString
    (
    FAST char *string,
    char *code,
    char *value
    )
    {
    if (value[0] != EOS)
	{
	string += strlen (string);
        (void)snprintf (string, PRINT_STR_PARAM_WIDTH, " %s=%s", code, value);
	}
    }

/*******************************************************************************
*
* printClear - print string with '?' for unprintable characters
*/

LOCAL void printClear
    (
    FAST char *param
    )
    {
    FAST char ch;

    while ((ch = *(param++)) != EOS)
	printf ("%c", (isascii (ch) && isprint ((int) ch)) ? ch : '?');
    }

/*******************************************************************************
*
* printParamNum - print number parameter
*/

LOCAL void printParamNum
    (
    char *paramName,
    int  value,
    BOOL hex
    )
    {
    printf (hex ? "%-*s: 0x%x \n" : "%-*s: %d \n", PARAM_PRINT_WIDTH,
	    paramName, value);
    }

/*******************************************************************************
*
* printParamString - print string parameter
*/

LOCAL void printParamString
    (
    char *paramName,
    char *param
    )
    {
    if (param [0] != EOS)
	{
	printf ("%-*s: ", PARAM_PRINT_WIDTH, paramName);
	printClear (param);
	printf ("\n");
	}
    }

/*******************************************************************************
*
* promptParamBootDevice - prompt the user for the boot device
*
* This routine reads the boot device name, and an optional unit number.
* If the unit number is not supplied, it defaults to 0.
*
* - carriage return leaves the parameter unmodified;
* - "." clears the parameter (null boot device and 0 unit number).
*/

LOCAL int promptParamBootDevice
    (
    char *      paramName,
    FAST char * param,
    int *       pValue,
    int         sizeParamName
    )
    {
    FAST int        i;
    _Vx_usr_arg_t   value;
    char            buf[BOOT_FIELD_LEN];
    char *          pBuf;

    (void)snprintf(buf, BOOT_FIELD_LEN, "%s%d", param, *pValue);
    printf ("%-*s: ", PARAM_PRINT_WIDTH, strBootDevice);
    printClear (buf);
    if (*buf != EOS)
	printf (" ");

    if ((i = promptRead (buf, sizeParamName)) != 0)
	return (i);

    if (buf[0] == '.')
	{
	param[0] = EOS;               /* just '.'; make empty fields */
        *pValue = 0;
	return (1);
	}

    i = 0;

    /* Extract first part of name of boot device. */
    while (!isdigit((int) buf[i]) && buf[i] != '=' && buf[i] != EOS)
        {
        param[i] = buf[i];
        i++;
        }
    param[i] = EOS;

    /* Extract unit number, if specified. */
    if (isdigit((int) buf[i]))     /* Digit before equal sign is unit number. */
        {
        pBuf = &buf[i];
        if (bootScanNum (&pBuf, &value, FALSE) != OK)
            {
            printf ("invalid unit number.\n");
            return (-98);
            }
        strncat(param, pBuf, BOOT_DEV_LEN - i);
        param[BOOT_DEV_LEN - 1] = EOS;
        }
    else                          /* No unit number specified. */
        {
        value = 0;
        if (buf[i] == '=')
            {
            /* Append remaining boot device name. */
            strncat(param, &buf[i], BOOT_DEV_LEN - i);
            param[BOOT_DEV_LEN - 1] = EOS;
            }
        }

    *pValue = (int)value;
    return (1);
    }

/*******************************************************************************
*
* promptParamString - prompt the user for a string parameter
*
* - carriage return leaves the parameter unmodified;
* - "." clears the parameter (null string).
*/

LOCAL int promptParamString
    (
    char *paramName,
    FAST char *param,
    int sizeParamName
    )
    {
    FAST int i;
    char buf [BOOT_FIELD_LEN];
    char *  pStrStart;
    char *  pStrEnd;

    printf ("%-*s: ", PARAM_PRINT_WIDTH, paramName);
    printClear (param);
    if (*param != EOS)
	printf (" ");

    if ((i = promptRead (buf, sizeParamName)) != 0)
	return (i);

    if (buf[0] == '.')
	{
	param [0] = EOS;	/* just '.'; make empty field */
	return (1);
	}

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

    strncpy (param, pStrStart, sizeParamName);	/* update parameter */
    param[sizeParamName - 1] = EOS;
    return (1);
    }

/*******************************************************************************
*
* promptParamNum - prompt the user for a parameter
*
* - carriage return leaves the parameter unmodified;
* - "." clears the parameter (0)
*/

LOCAL int promptParamNum
    (
    char *paramName,
    int *pValue,
    BOOL hex
    )
    {
    char buf [BOOT_FIELD_LEN];
    char *pBuf;
    _Vx_usr_arg_t value;
    int i;

    printf (hex ? "%-*s: 0x%x " : "%-*s: %d ", PARAM_PRINT_WIDTH, paramName,
	    *pValue);

    if ((i = promptRead (buf, sizeof (buf))) != 0)
	return (i);

    if (buf[0] == '.')
	{
	*pValue = 0;		/* just '.'; make empty field (0) */
	return (1);
	}

    /* scan for number */

    pBuf = buf;
    if ((bootScanNum (&pBuf, &value, FALSE) != OK) || (*pBuf != EOS))
	{
	printf ("invalid number.\n");
	return (-98);
	}

    *pValue = (int)value;
    return (1);
    }

/*******************************************************************************
*
* promptRead - read the result of a prompt and check for special cases
*
* Special cases:
*    '-'  = previous field
*    '^D' = quit
*    CR   = leave field unchanged
*    too many characters = error
*
* RETURNS:
*    0 = OK
*    1 = skip this field
*   -1 = go to previous field
*  -98 = ERROR
*  -99 = quit
*/

LOCAL int promptRead
    (
    char *buf,
    int bufLen
    )
    {
    FAST ssize_t i;

    i = fioRdString (STD_IN, buf, (size_t)bufLen);

    if (i == EOF)
	return (-99);			/* EOF; quit */

    if (i == 1)
	return (1);			/* just CR; leave field unchanged */

    if ((i == 2) && (buf[0] == '-'))
	return (-1);			/* '-'; go back up */

    if (i >= bufLen)
	{

        /* the '\0' and 'CR' should be removed from the count */

        printf ("too big - maximum field width = %d.\n", bufLen - 2);

	/* we mustn't take into account the end of the string */

        while((i = fioRdString(STD_IN, buf, (size_t)bufLen)) >= bufLen);
	return (-98);
	}

    return (0);
    }

