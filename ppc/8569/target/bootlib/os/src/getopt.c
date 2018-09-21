/* getopt.c - getopt facility */

/*
 * Copyright (c) 2004-2005, 2009, 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. No license to Wind River intellectual property rights
 * is granted herein. All rights not licensed by Wind River are reserved
 * by Wind River.
 */

/*
 * Copyright (c) 1987, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
modification history
--------------------
01i,13nov14,jpb  Added documentation to getopt to mention behavior when using
                 getopt multiple times (VXW6-23078).
01h,26oct09,s_s  Fixed error in getopt() documentation (WIND00188248)
01g,03sep09,s_s  Added documentation for usage of getopt()
01f,23aug05,pad  Substituted non-POSIX STATUS with 'int' in getOptServ()
		 definition for the user side.
01e,22aug05,pad  Include vxWorks.h (required on the user side).
01d,17mar05,dlk  Make 'place' a member of the GETOPT state.
01c,25feb05,niq  Implemented reentrant version of getopt
01b,09apr04,job  Replaced __progname with nargv[0] 
01a,29mar04,job  Installed into vxWorks source base
*/

/*
DESCRIPTION

This library supplies both a POSIX compliant getopt() which is a command
line parser, as well as a rentrant version of the same command named
getopt_r(). Prior to calling getopt_r(), the caller needs to initialize the
getopt state structure by calling getoptInit(). This explicit initialization
is not needed while calling getopt() as the system is setup as if the
initialization has already been done.

The user can modify getopt() behavior by setting the the getopt variables like
optind, opterr, etc. For getopt_r(), the value needs to be updated in the
getopt state structure.
*/

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getopt.c	8.2 (Berkeley) 4/2/94";
#endif /* LIBC_SCCS and not lint */

#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define	BADCH	(int)'?'
#define	BADARG	(int)':'
#define	EMSG	""

int	opterr = 1,		/* if error message should be printed */
	optind = 1,		/* index into parent argv vector */
	optopt,			/* character checked for validity */
	optreset;		/* reset getopt */
char	*optarg;		/* argument associated with option */

LOCAL   char * optplace = EMSG;	/* additional persistent state */


/****************************************************************************
*
* getopt - parse argc/argv argument vector (POSIX)
*
* Decodes arguments passed in an argc/argv[] vector
*
* The parameters nargc and nargv are the argument count and argument array as 
* passed to main(). The argument ostr is a string of recognized option 
* characters; if a character is followed by a colon, the option takes an 
* argument. 
*
* The variable optind is the index of the next element of the nargv[] vector
* to be processed. It shall be initialized to 1 by the system, and getopt()
* shall update it when it finishes with each element of nargv[]. When an
* element of nargv[] contains multiple option characters, it is unspecified
* how getopt() determines which options have already been processed.
*
* The getopt() function shall return the next option character (if one is 
* found) from nargv that matches a character in ostr, if there is one 
* that matches. If the option takes an argument, getopt() shall set the 
* variable optarg to point to the option-argument as follows:
* 
* If the option was the last character in the string pointed to by an element 
* of nargv, then optarg shall contain the next element of nargv, and optind 
* shall be incremented by 2. If the resulting value of optind is greater than 
* nargc, this indicates a missing option-argument, and getopt() shall return
* an error indication.
*
* Otherwise, optarg shall point to the string following the option character 
* in that element of nargv, and optind shall be incremented by 1.
*
* If, when getopt() is called:
*
* nargv[optind]  is a null pointer
* nargv[optind]  is not the character -  
* nargv[optind]  points to the string "-"
*
* getopt() shall return -1 without changing optind. If:
* 
* nargv[optind]   points to the string "--"
* 
* getopt() shall return -1 after incrementing optind.
*
* If getopt() encounters an option character that is not contained in 
* ostr, it shall return the question-mark ( '?' ) character. If it detects 
* a missing option-argument, it shall return the colon character ( ':' ) if 
* the first character of ostr was a colon, or a question-mark character 
* ( '?' ) otherwise. In either case, getopt() shall set the variable optopt to 
* the option character that caused the error. If the application has not set 
* the variable opterr to 0 and the first character of ostr is not a colon, 
* getopt() shall also print a diagnostic message to stderr in the format 
* specified for the getopts utility.
* 
* The getopt() function need not be reentrant. A function that is not required 
* to be reentrant is not required to be thread-safe.
*
* In order to use getopt() to evaluate multiple sets of arguments, or to 
* evaluate a single set of arguments multiple times, the variable optreset 
* must be set to 1 before the second and each additional set of calls to 
* getopt(), and the variable optind must be reinitialized.
*
* RETURNS: 
* 
* The getopt() function shall return the next option character 
* specified on the command line.
*
* A colon ( ':' ) shall be returned if getopt() detects a missing argument 
* and the first character of ostr was a colon ( ':' ).
*
* A question mark ( '?' ) shall be returned if getopt() encounters an option 
* character not in ostr or detects a missing argument and the first 
* character of ostr was not a colon ( ':' ).
*
* Otherwise, getopt() shall return -1 when all command line options are parsed.
*
* KERNEL USAGE:
*
* Getopt() can be used in the kernel by including the component INCLUDE_GETOPT.
* There are no argc and argv values associated with tasks in the kernel. These
* arguments can be created by the user and passed to getopt() through
* a wrapper function. The example below illustrates the use of getopt() in
* kernel.
*
* \cs
*
* #include <unistd.h>
* #include <stdio.h>
* #include <vxWorks.h>
* #include <string.h>
*
* int myGetoptWrapper (char * ostr)
*     {
*     int count;  // argc argument //
*     // building the argv argument with the function name and the options //
*     char *arr[] = {"myGetoptWrapper", "-a", "xyz", "-b", "xyz", "-c", "xyz",
*                    "pqr"};
*
*     count = 8;
*     return (getopt(count, arr, ostr));
*     }
*
* void myGetoptUsage (void)
*     {
*     int ret;
*     while ((ret = myGetoptWrapper ("a:b:c")) != EOF)
*         {
*         switch (ret)
*             {
*             case 'a':
*                 //user code//
*                 break;
*             case 'b':
*                 //user code//
*                 break;
*             case 'c':
*                 //user code//
*                 break;
*
*             }
*         }
*     }
*
* \ce
*
* ERRNO: N/A
*
* SEE ALSO: POSIX 
*/

int getopt
    (
    int nargc, 
    char * const *nargv,
    const char *ostr
    )
    {
    int    ret;
    GETOPT getoptState;

    /* Setup the getopt state structure with the global values */

    getoptState.opterr = opterr;
    getoptState.optind = optind;
    getoptState.optopt = optopt;
    getoptState.optreset = optreset;
    getoptState.optarg = optarg;
    getoptState.place = optplace;

    ret = getopt_r (nargc, nargv, ostr, &getoptState);
    
    /* Update the global values from the state structure */

    opterr = getoptState.opterr;
    optind = getoptState.optind;
    optopt = getoptState.optopt;
    optreset = getoptState.optreset;
    optarg = getoptState.optarg;
    optplace = getoptState.place;

    return (ret);
    }

    
/****************************************************************************
*
* getoptInit - initialize the getopt state structure
*
* This function initializes the structure, <pGetOpt> that is used to
* maintain the getopt state. This structure is passed to getopt_r() which
* is a reentrant threadsafe version of the standard getopt() call. This
* function must be called before calling getopt_r()
*
* RETURNS: N/A
* 
*/

void getoptInit
    (
    GETOPT_ID     pArg    /* Pointer to getopt structure to be initialized */
    )
    {
    pArg->opterr = 1;
    pArg->optind = 1;
    pArg->optopt = 0;
    pArg->optreset = 0;
    pArg->optarg = NULL;
    pArg->place = EMSG;
    }


    
/****************************************************************************
*
* getopt_r - parse argc/argv argument vector (POSIX)
*
* This function is a reentrant version of the getopt() function.
* The non-reentrant version keeps the getopt state in global
* variables across multiple calls made by the application, while this
* reentrant version keeps the state in the structure provided by the
* caller, thus allowing multiple callers to use getopt simultaneously
* without requiring any synchronization between them.
*
* The parameters <nargc> and <nargv> are the argument count and argument
* array as passed to main(). The argument <ostr> is a string of recognized
* option characters; if a character is followed by a colon, the option takes
* an argument. The argument <pGetOpt> points to the structure allocated by the
* caller to keep track of the getopt state. Prior to calling getopt_r, it is
* the caller responsibility to initialize this structure by calling
* getoptInit().
*
* The variable pGetOpt->optind is the index of the next element of the nargv[]
* vector to be processed. getopt_r() shall update it when it finishes with
* each element of nargv[]. When an element of nargv[] contains multiple option
* characters, it is unspecified how getopt_r() determines which options have
* already been processed.
*
* The getopt_r() function shall return the next option character (if one is 
* found) from nargv that matches a character in ostr, if there is one 
* that matches. If the option takes an argument, getopt_r() shall set the 
* variable pGetOpt->optarg to point to the option-argument as follows:
* 
* If the option was the last character in the string pointed to by an element 
* of nargv, then pGetOpt->optarg shall contain the next element of nargv,
* and pGetOpt->optind shall be incremented by 2. If the resulting value of
* pGetOpt->optind is greater than nargc, this indicates a missing
* option-argument, and getopt_r() shall return an error indication.
*
* Otherwise, pGetOpt->optarg shall point to the string following the option
* character in that element of nargv, and pGetOpt->optind shall be
* incremented by 1.
*
* If, when getopt_r() is called:
*
* nargv[pGetOpt->optind]  is a null pointer
* nargv[pGetOpt->optind]  is not the character -  
* nargv[pGetOpt->optind]  points to the string "-"
*
* getopt_r() shall return -1 without changing pGetOpt->optind. If:
* 
* nargv[pGetOpt->optind]   points to the string "--"
* 
* getopt_r() shall return -1 after incrementing pGetOpt->optind.
*
* If getopt_r() encounters an option character that is not contained in 
* ostr, it shall return the question-mark ( '?' ) character. If it detects 
* a missing option-argument, it shall return the colon character ( ':' ) if 
* the first character of ostr was a colon, or a question-mark character 
* ( '?' ) otherwise. In either case, getopt_r() shall set the variable
* pGetOpt->optopt to the option character that caused the error. If the
* application has not set the variable pGetOpt->opterr to 0 and the first
* character of ostr is not a colon, getopt_r() shall also print a diagnostic
* message to stderr in the format specified for the getopts utility.
* 
* This function is reentrant and thread-safe.
*
* RETURNS: 
* 
* The getopt_r() function shall return the next option character 
* specified on the command line.
*
* A colon ( ':' ) shall be returned if getopt_r() detects a missing argument 
* and the first character of ostr was a colon ( ':' ).
*
* A question mark ( '?' ) shall be returned if getopt_r() encounters an option 
* character not in ostr or detects a missing argument and the first 
* character of ostr was not a colon ( ':' ).
*
* Otherwise, getopt_r() shall return -1 when all command line options are
* parsed.
*
* SEE ALSO: POSIX 
*/

int getopt_r
    (
    int nargc, 
    char * const *nargv,
    const char *ostr,
    GETOPT_ID pGetOpt
    )
    {
	char *oli;				/* option letter list index */

	/* update scanning pointer? */
	if (pGetOpt->optreset || !*pGetOpt->place) {
		pGetOpt->optreset = 0;
		if (pGetOpt->optind >= nargc ||
                    *(pGetOpt->place = nargv[pGetOpt->optind]) != '-') {
			pGetOpt->place = EMSG;
			return (EOF);
		}
		if (pGetOpt->place[1] && *++ pGetOpt->place == '-') {
			/* found "--" */
			++pGetOpt->optind;
			pGetOpt->place = EMSG;
			return (EOF);
		}
	}					/* option letter okay? */
	if ((pGetOpt->optopt = (int)* pGetOpt->place++) == (int)':' ||
	    !(oli = strchr(ostr, pGetOpt->optopt))) {
		/*
		 * if the user didn't specify '-' as an option,
		 * assume it means EOF.
		 */
		if (pGetOpt->optopt == (int)'-')
			return (EOF);
		if (!*pGetOpt->place)
			++pGetOpt->optind;
		if (pGetOpt->opterr && *ostr != ':')
			(void)fprintf(stderr, "%s: illegal option -- %c\n",
                                      nargv[0], pGetOpt->optopt);
		return (BADCH);
	}
	if (*++oli != ':') {			/* don't need argument */
		pGetOpt->optarg = NULL;
		if (!*pGetOpt->place)
			++pGetOpt->optind;
	}
	else {					/* need an argument */
		if (*pGetOpt->place)			/* no white space */
			pGetOpt->optarg = pGetOpt->place;
		else if (nargc <= ++pGetOpt->optind) {	/* no arg */
			pGetOpt->place = EMSG;
			if (*ostr == ':')
				return (BADARG);
			if (pGetOpt->opterr)
				(void)fprintf(stderr,
				    "%s: option requires an argument -- %c\n",
				    nargv[0], pGetOpt->optopt);
			return (BADCH);
		}
	 	else				/* white space */
			pGetOpt->optarg = nargv[pGetOpt->optind];
		pGetOpt->place = EMSG;
		++pGetOpt->optind;
	}
	return (pGetOpt->optopt);		/* dump back option letter */
    }


/***************************************************************************
*
* getOptServ - parse parameter string into argc, argv format
*
* SYNOPSIS
* \cs
* \IFSET_START USER
* int getOptServ
* \IFSET_END
* \IFSET_START KERNEL
* STATUS getOptServ
* \IFSET_END
*     (
*     char * ParamString,
*     const char * progName,      /@ program name value for argv[0] @/
*     int * argc,
*     char * argvloc[],
*     int argvlen
*     )
* \ce
*
* This routine modifies the passed parameter string, using strtok_r()
* to chop it into arguments separated by space or tab characters.  It
* stores pointers to each argument in the array at <argvloc>, skipping the
* first element in the array.  If non-null, <progName> is stored in the
* first element of the <argvloc> array; otherwise, a pointer to a default
* static program name string is stored.  A NULL pointer is stored in the
* array after the last argument pointer.  The total number of argument
* pointers stored, plus one for the skipped first entry, is stored at
* <argc>.  The total number of available slots in the <argvloc> array
* should be passed in <argvlen>, which must be at least 2.  If there are
* more than <argvlen> - 2 arguments in the parameter string, then
* any arguments in excess of this number will not be stored in the
* array, however *<argc> will be set to indicate the actual number of
* argments, counting those not stored.  Slot 0 is always
* reserved for the program name, and one slot is always used to store
* a NULL terminating the stored arguments.
*
* A NULL <ParamString> is treated the same as an empty string ""; it
* results in *<argc> set to 1 and argvloc[1] set to NULL.
*
* \IFSET_START USER
* RETURNS: 0 (OK) if all arguments were successfully stored; otherwise, -1
* (ERROR).
* \IFSET_END
* \IFSET_START KERNEL
* RETURNS: OK if all arguments were successfully stored; otherwise, ERROR.
* \IFSET_END
*/

#ifdef _WRS_KERNEL
STATUS getOptServ
#else
int getOptServ
#endif
    (
    char * ParamString,
    const char * progName,	/* program name value for argv[0] */
    int *argc,
    char *argvloc[],
    int argvlen
    )
    {
    char *p;
    char * pLast = NULL;
    int i = 1; /* skip first slot for program name */
    static const char argv0 [] = "???";

    if (argvlen < 2 || argc == NULL || argvloc == NULL)
	return ERROR;

    if (progName == NULL)
	progName = argv0;

    * (const char **) &argvloc[0] = progName;

    argvlen -= 1; /* reserve slot for NULL */

#define TOK_DEL_STR " \t"

    if (ParamString != NULL)
	{
	for (p = strtok_r (ParamString, TOK_DEL_STR, &pLast);
	     p != NULL;
	     p = strtok_r (NULL, TOK_DEL_STR, &pLast))
	    {
	    if (i < argvlen)
		argvloc[i] = p;
	    ++i;
	    }
	}

    *argc = i;

    if (i < argvlen)
	argvloc [i] = NULL;
    else
	argvloc [argvlen] = NULL;

    return (i <= argvlen ? OK : ERROR);
    }

