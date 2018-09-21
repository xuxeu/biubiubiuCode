/* bootParseLib.c - boot ROM bootline interpreter library */

/*
 * Copyright (c) 2004, 2009, 2012, 2013 Wind River Systems, Inc. 
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
13dec13,xms  Fix type mismatch issue. (WIND00446317)
01e,27feb13,mpc  Fix parsing error without hostname. (WIND00402154)
01d,14aug12,mpc  Fix missing length verification for host name and boot file. 
                 (WIND00362535)
01c,09jan12,pgh  Fix missing return code checks, and remove sscanf().
01b,11dec09,pgh  64-bit conversion
01a,09sep04,job  Created from bootLib.c (02w,21oct03) to separate out 
                 bootline interpreting code for improved scalability.
                 Removed some GNU compile warnings.
*/

/*
DESCRIPTION
This library contains routines for interpreting a boot line.

INCLUDE FILES: bootLib.h

SEE ALSO: bootLib.c
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
#include "taskLib.h"

/* defines */

#define MAX_INT_STRING_LEN   11

/* macros that fill in size parameter */

#define GET_WORD(pp, field, delims) \
    getWord (pp, field, sizeof (field), delims)

#define GET_ASSIGN(pp, name, field) \
    getAssign (pp, name, field, sizeof (field))

/* forward static functions */

static STATUS bootSubfieldExtract (char *string, int *pValue, char
        delimeter);
static BOOL getWord (char ** ppString, char *pWord, int length, char *delim);
static BOOL getConst (char ** ppString, char *pConst);
static BOOL getAssign (char ** ppString, char *valName, char *pVal, int maxLen);
static BOOL getAssignNum (char ** ppString, char *valName, int *pVal);
static void skipSpace (char ** strptr);

/******************************************************************************
*
* bootStringToStructAdd - interpret the boot parameters from the boot line
*
* This routine parses the ASCII string <bootString> and returns the values into
* the provided parameters <pBootParams>.  The fields of <pBootParams> may be
* previously set to default values.
*
* For a description of the format of the boot line, see the manual entry 
* for bootLib 
*
* RETURNS:
* A pointer to the last character successfully parsed plus one
* (points to EOS, if OK).  The entire boot line is parsed.
*
*/

char * bootStringToStructAdd
    (
    char *         bootString,  /* boot line to be parsed */
    FAST BOOT_PARAMS * pBootParams  /* where to return parsed boot line */
    )
    {
    char *p1 = bootString;
    char *p1Save;
    char *p2;
    char hostAndFile [BOOT_HOST_LEN + BOOT_FILE_LEN];
    _Vx_usr_arg_t arg1, arg2;

    /* get boot device and proc num */

    if (!GET_WORD(&p1, pBootParams->bootDev, " \t(")                        ||
        !getConst(&p1, "(")                                                 ||
        (bootScanNum(&p1, &arg1, FALSE) != OK))
        {
        return (p1);
        }
    pBootParams->unitNum = (int)arg1;
    if (!getConst(&p1, ",")                                                 ||
        (bootScanNum(&p1, &arg2, FALSE) != OK))
        {
        return (p1);
        }
    pBootParams->procNum = (int)arg2;
    if (!getConst(&p1, ")"))
        {
        return (p1);
        }

    /* get host name and boot file, if present */

    p1Save = p1;
    (void)GET_WORD  (&p1, hostAndFile, " \t");

    if (index (hostAndFile, ':') != NULL)
        {
        p2 = hostAndFile;
        (void)GET_WORD  (&p2, pBootParams->hostName, ":");

	/* 
	 * Don't check the length of <hostName> for backward compatibility. 
	 * If the <hostName> is more than BOOT_HOST_LEN characters, trim
	 * all the characters before the colon. Otherwise, the <bootFile> will
	 * be parsed incorrectly without any boot line error information.
	 */
 
	while (*p2++ != ':');

        (void)GET_WORD  (&p2, pBootParams->bootFile, " \t");
        }
    else
        p1 = p1Save;

    /* get optional assignments */

    while (skipSpace (&p1), (*p1 != EOS))
        {
        if (!GET_ASSIGN (&p1, "ead", pBootParams->ead) &&
            !GET_ASSIGN (&p1, "e",   pBootParams->ead) &&
            !GET_ASSIGN (&p1, "bad", pBootParams->bad) &&
            !GET_ASSIGN (&p1, "b",   pBootParams->bad) &&
            !GET_ASSIGN (&p1, "had", pBootParams->had) &&
            !GET_ASSIGN (&p1, "h",   pBootParams->had) &&
            !GET_ASSIGN (&p1, "gad", pBootParams->gad) &&
            !GET_ASSIGN (&p1, "g",   pBootParams->gad) &&
            !GET_ASSIGN (&p1, "usr", pBootParams->usr) &&
            !GET_ASSIGN (&p1, "u",   pBootParams->usr) &&
            !GET_ASSIGN (&p1, "pw",  pBootParams->passwd) &&
            !GET_ASSIGN (&p1, "o",   pBootParams->other) &&
            !GET_ASSIGN (&p1, "tn",  pBootParams->targetName) &&
            !GET_ASSIGN (&p1, "hn",  pBootParams->hostName) &&
            !GET_ASSIGN (&p1, "fn",  pBootParams->bootFile) &&
            !GET_ASSIGN (&p1, "s",  pBootParams->startupScript) &&
            !GET_ASSIGN (&p1, "Type",  pBootParams->Type) &&
            !getAssignNum (&p1, "n", &pBootParams->procNum) &&
            !getAssignNum (&p1, "f", &pBootParams->flags))
            {
            break;
            }
        }

    return (p1);
    }

/*******************************************************************************
*
* bootStringToStruct - interpret the boot parameters from the boot line
*
* This routine parses the ASCII string and returns the values into
* the provided parameters.
*
* For a description of the format of the boot line, see the manual entry 
* for bootLib 
*
* RETURNS:
* A pointer to the last character successfully parsed plus one
* (points to EOS, if OK).  The entire boot line is parsed.
*/

char * bootStringToStruct
    (
    char * bootString,          /* boot line to be parsed */
    FAST BOOT_PARAMS * pBootParams  /* where to return parsed boot line */
    )
    {
    /* initialize boot parameters */

    bzero ((char *) pBootParams, sizeof (*pBootParams));

    return bootStringToStructAdd (bootString, pBootParams);
    }

/******************************************************************************
*
* bootParamsErrorPrint - print boot string error indicator
*
* print error msg with '^' where parse failed
*
* NOMANUAL
*/

void bootParamsErrorPrint
    (
    char *bootString,
    char *pError
    )
    {
    printf ("\nError in boot line:\n%s\n%*c\n", bootString,
            (int)(pError - bootString + 1), '^');
    }

/*******************************************************************************
*
* bootLeaseExtract - extract the lease information from an Internet address
*
* This routine extracts the optional lease duration and lease origin fields 
* from an Internet address field for use with DHCP.  The lease duration can be 
* specified by appending a colon and the lease duration to the netmask field.
* For example, the "inet on ethernet" field of the boot parameters could be 
* specified as:
* .CS
*     inet on ethernet: 90.1.0.1:ffff0000:1000
* .CE
*
* If no netmask is specified, the contents of the field could be:
* .CS
*     inet on ethernet: 90.1.0.1::ffffffff
* .CE
*
* In the first case, the lease duration for the address is 1000 seconds. The 
* second case indicates an infinite lease, and does not specify a netmask for
* the address. At the beginning of the boot process, the value of the lease
* duration field is used to specify the requested lease duration. If the field 
* not included, the value of DHCP_DEFAULT_LEASE is used
* instead.
* 
* The lease origin is specified with the same format as the lease duration,
* but is added during the boot process. The presence of the lease origin
* field distinguishes addresses assigned by a DHCP server from addresses
* entered manually. Addresses assigned by a DHCP server may be replaced
* if the bootstrap loader uses DHCP to obtain configuration parameters.
* The value of the lease origin field at the beginning of the boot process
* is ignored.
*
* This routine extracts the optional lease duration by replacing the preceding 
* colon in the specified string with an EOS and then scanning the remainder as 
* a number.  The lease duration and lease origin values are returned via
* the <pLeaseLen> and <pLeaseStart> pointers, if those parameters are not NULL.
*
* RETURNS:
*   2 if both lease values are specified correctly in <string>, or 
*  -2 if one of the two values is specified incorrectly.
* If only the lease duration is found, it returns:
*   1 if the lease duration in <string> is specified correctly,
*   0 if the lease duration is not specified in <string>, or
*  -1 if an invalid lease duration is specified in <string>.
*/

int bootLeaseExtract
    (
    char *string,       /* string containing addr field */
    u_long *pLeaseLen,  /* pointer to storage for lease duration */
    u_long *pLeaseStart /* pointer to storage for lease origin */
    )
    {
    FAST char *pDelim;
    FAST char *offset;
    int result;
    int status = 0;
    int start;
    int length;

    /* find delimeter for netmask */

    offset = index (string, ':');
    if (offset == NULL)
        return (0);     /* no subfield specified */

    /* Start search after netmask field. */

    pDelim = offset + 1;

    /* Search for lease duration field. */

    offset = index (pDelim, ':');
    if (offset == NULL)         /* No lease duration tag. */
        return (0);

    /* Start search after duration. */

    pDelim = offset + 1;

    status = bootSubfieldExtract (pDelim, &start, ':');
    if (status == 1 && pLeaseStart != NULL)
        *pLeaseStart = (u_long)start;

    /* Reset search pointer to obtain lease duration. */

    offset = index (string, ':');
    if (offset == NULL)      /* Sanity check - should not occur. */
        return (0);

    /* Lease duration follows netmask. */

    pDelim = offset + 1;

    /* Store lease duration if found. */

    result = bootSubfieldExtract (pDelim, &length, ':');
    if (result == 1 && pLeaseLen != NULL)
        *pLeaseLen = (u_long)length;

    if (status != 0)    /* Both lease values were present. */
        {
        if (status < 0 || result < 0)    /* Error reading one of the values. */
            return (-2);
        else
            return (2);     /* Both lease values read successfully. */
        }

    return (result);
    }

/*******************************************************************************
*
* bootNetmaskExtract - extract the net mask field from an Internet address
*
* This routine extracts the optional subnet mask field from an Internet address
* field.  Subnet masks can be specified for an Internet interface by appending
* to the Internet address a colon and the net mask in hexadecimal.  
* For example, the "inet on ethernet" field of the boot parameters could 
* be specified as:
* .CS
*     inet on ethernet: 90.1.0.1:ffff0000
* .CE
* In this case, the network portion of the address (normally just 90)
* is extended by the subnet mask (to 90.1).  This routine extracts the
* optional trailing subnet mask by replacing the colon in the specified
* string with an EOS and then scanning the remainder as a hex number.
* This number, the net mask, is returned via the <pNetmask> pointer.
* 
* This routine also handles an empty netmask field used as a placeholder
* for the lease duration field (see bootLeaseExtract() ). In that case,
* the colon separator is replaced with an EOS and the value of netmask is
* set to 0. 
*
* RETURNS:
*   1 if the subnet mask in <string> is specified correctly,
*   0 if the subnet mask in <string> is not specified, or
*  -1 if an invalid subnet mask is specified in <string>.
*/

STATUS bootNetmaskExtract
    (
    char *string,       /* string containing addr field */
    int *pNetmask       /* pointer where to return net mask */
    )
    {
    FAST char *pDelim;
    char *offset;

    /* find delimeter */

    pDelim = index (string, ':');
    if (pDelim == NULL)
        return (0);     /* no subfield specified */

    /* Check if netmask field precedes timeout field. */
    offset = pDelim + 1;
    skipSpace(&offset);
    if (*offset == ':' || *offset == EOS)  /* Netmask field is placeholder. */
        {
        *pDelim = EOS;
        *pNetmask = 0;
        return (1);
        }
         
    return (bootSubfieldExtract (string, pNetmask, ':'));
    }

/******************************************************************************
*
* bootBpAnchorExtract - extract a backplane address from a device field
*
* This routine extracts the optional backplane anchor address field from a
* boot device field.  The anchor can be specified for the backplane
* driver by appending to the device name (i.e., "bp") an equal sign (=) and the
* address in hexadecimal.  For example, the "boot device" field of the boot
* parameters could be specified as:
* .CS
*     boot device: bp=800000
* .CE
* In this case, the backplane anchor address would be at address 0x800000,
* instead of the default specified in config.h.
*
* This routine picks off the optional trailing anchor address by replacing
* the equal sign (=) in the specified string with an EOS and then scanning the
* remainder as a hex number.
* This number, the anchor address, is returned via the <pAnchorAdrs> pointer.
*
* RETURNS:
*   1 if the anchor address in <string> is specified correctly,
*   0 if the anchor address in <string> is not specified, or
*  -1 if an invalid anchor address is specified in <string>.
*/

STATUS bootBpAnchorExtract
    (
    char *string,       /* string containing adrs field */
    char **pAnchorAdrs  /* pointer where to return anchor address */
    )
    {
    return (bootSubfieldExtract (string, (int *) pAnchorAdrs, '='));
    }

/******************************************************************************
*
* bootSubfieldExtract - extract a numeric subfield from a boot field
*
* Extracts subfields in fields of the form "<field><delimeter><subfield>".
* i.e. <inet>:<netmask> and bp=<anchor>
*/

LOCAL STATUS bootSubfieldExtract
    (
    char *string,       /* string containing field to be extracted */
    int *pValue,        /* pointer where to return value */
    char delimeter      /* character delimeter */
    )
    {
    FAST char *     pDelim;
    _Vx_usr_arg_t   value;

    /* find delimeter */

    pDelim = index (string, delimeter);
    if (pDelim == NULL)
        return (0);         /* no subfield specified */

    /* scan remainder for numeric subfield */

    string = pDelim + 1;

    if (bootScanNum (&string, &value, TRUE) != OK)
        return (-1);        /* invalid subfield specified */

    *pDelim = EOS;          /* terminate string at the delimeter */
    *pValue = (int)value;   /* return value */
    return (1);             /* valid subfield specified */
    }

/******************************************************************************
*
* bootScanNum - scan string for numeric value
*
* This routine scans the specified string for a numeric value.  The string
* pointer is updated to reflect where the scan stopped, and the value is
* returned via pValue.  The value will be scanned by default in decimal unless
* hex==TRUE.  In either case, preceding the value with "0x" or "$" forces
* hex.  Spaces before and after number are skipped.
*
* RETURNS: OK if value scanned successfully, otherwise ERROR.
*
* EXAMPLES:
*   bootScanNum (&string, &value, FALSE);
*    with:
*   string = "   0xf34  ";
*       returns OK, string points to EOS, value=0xf34
*   string = "   0xf34r  ";
*       returns OK, string points to 'r', value=0xf34
*   string = "   r0xf34  ";
*       returns ERROR, string points to 'r', value unchanged
*
* NOMANUAL
*/

STATUS bootScanNum
    (
    FAST char **    ppString,
    _Vx_usr_arg_t * pValue,
    FAST BOOL       hex
    )
    {
    FAST char *         pStr;
    FAST char           ch;
    FAST int            n;
    FAST _Vx_usr_arg_t  value = 0;

    skipSpace (ppString);

    /* pick base */

    if (**ppString == '$')
        {
        ++*ppString;
        hex = TRUE;
        }
    else if (strncmp (*ppString, "0x", 2) == 0)
        {
        *ppString += 2;
        hex = TRUE;
        }

    /* scan string */

    pStr = *ppString;

    FOREVER
        {
        ch = *pStr;

        if (!isascii (ch))
            break;

        if (isdigit ((int)ch))
            n = ch - '0';
        else
            {
            if (!hex)
                break;

            if (isupper ((int)ch))
                ch = (char)tolower ((int)ch);

            if ((ch < 'a') || (ch > 'f'))
                break;

            n = ch - 'a' + 10;
            }

        value = (value * (hex ? 16 : 10)) + n;
        ++pStr;
        }


    /* error if we didn't scan any characters */

    if (pStr == *ppString)
        return (ERROR);

    /* update users string ptr to point to character that stopped the scan */

    *ppString = pStr;
    skipSpace (ppString);

    *pValue = value;

    return (OK);
    }

/*******************************************************************************
*
* skipSpace - advance pointer past white space
*
* Increments the string pointer passed as a parameter to the next
* non-white-space character in the string.
*/

LOCAL void skipSpace
    (
    FAST char **strptr  /* pointer to pointer to string */
    )
    {
    while (isspace ((int) **strptr))
        ++*strptr;
    }

/*******************************************************************************
*
* getWord - get a word out of a string
*
* Words longer than the specified max length are truncated.
*
* RETURNS: TRUE if word is successfully extracted from string, FALSE otherwise;
* Also updates ppString to point to next character following extracted word.
*/

LOCAL BOOL getWord
    (
    char **ppString,    /* ptr to ptr to string from which to get word */
    FAST char *pWord,   /* where to return word */
    int length,         /* max length of word to get including EOS */
    char *delim         /* string of delimiters that can terminate word */
    )
    {
    FAST char *pStr;

    skipSpace (ppString);

    /* copy up to any specified delimeter, EOS, or max length */

    pStr = *ppString;
    while ((--length > 0) && (*pStr != EOS) && (index (delim, *pStr) == 0))
        *(pWord++) = *(pStr++);

    *pWord = EOS;

    /* if we copied anything at all, update pointer and return TRUE */

    if (pStr != *ppString)
        {
        *ppString = pStr;
        return (TRUE);
        }

    /* no word to get */

    return (FALSE);
    }

/*******************************************************************************
*
* getConst - get a constant string out of a string
*
* case insensitive compare for identical strings
*/

LOCAL BOOL getConst
    (
    char **ppString,
    FAST char *pConst
    )
    {
    FAST int ch1;
    FAST int ch2;
    FAST char *pString;

    skipSpace (ppString);

    for (pString = *ppString; *pConst != EOS; ++pString, ++pConst)
        {
        ch1 = *pString;
        ch1 = (isascii (ch1) && isupper (ch1)) ? tolower (ch1) : ch1;
        ch2 = *pConst;
        ch2 = (isascii (ch2) && isupper (ch2)) ? tolower (ch2) : ch2;

        if (ch1 != ch2)
            return (FALSE);
        }

    /* strings match */

    *ppString = pString;
    return (TRUE);
    }

/*******************************************************************************
*
* getAssign - get an assignment out of a string
*/

LOCAL BOOL getAssign
    (
    FAST char **ppString,
    char *valName,
    char *pVal,
    int maxLen
    )
    {
    skipSpace (ppString);
    if (!getConst (ppString, valName))
        return (FALSE);

    skipSpace (ppString);
    if (!getConst (ppString, "="))
        return (FALSE);

    return (getWord (ppString, pVal, maxLen, " \t"));
    }

/*******************************************************************************
*
* getAssignNum - get a numeric assignment out of a string
*/

LOCAL BOOL getAssignNum
    (
    FAST char **ppString,
    char *valName,
    int *pVal
    )
    {
    char buf [BOOT_FIELD_LEN];
    char *pBuf = buf;
    char *pTempString;
    _Vx_usr_arg_t tempVal;

    /* we don't modify *ppString or *pVal until we know we have a good scan */

    /* first pick up next field into buf */

    pTempString = *ppString;
    if (!getAssign (&pTempString, valName, buf, sizeof (buf)))
        return (FALSE);

    /* now scan buf for a valid number */

    if ((bootScanNum (&pBuf, &tempVal, FALSE) != OK) || (*pBuf != EOS))
        return (FALSE);

    /* update string pointer and return value */

    *ppString = pTempString;
    *pVal = (int)tempVal;

    return (TRUE);
    }
