/* usrWdbCore.c - configuration file for the WDB agent's core facilities */

/*
 * Copyright (c) 1997-2009 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use of this 
 * software may be licensed only pursuant to the terms of an applicable Wind 
 * River license agreement.
 */

/*
modification history
--------------------
03s,21aug09,rlp  Fixed the computation of the target memory size reported to
                 the host (WIND00178188).
03r,15apr08,j_z  Initialize the struct excCtx (Defect #107990).
03q,11jul07,rlp  Modified information reported on exception in system mode.
03p,06jul07,p_b  Added 64bits support
03o,25feb07,jmt  Modify for bootApp support
03n,10jul06,rlp  Replaced VX_CPU_FAMILY by CPU_FAMILY (Defect #37561).
03m,08dec05,dbt  Fixed wdbInstallCommIf() call to provide MTU.
03l,29nov05,dbt  Moved some system mode specific stuff to usrWdbSys.c.
03k,14nov05,dbt  Removed wdbMemRtLibInit() call.
03j,28jul05,bpn  Changed wdbMemChunck buffer to a configurable buffer linked to
                 the INCLUDE_WDB_REG component. Reduced the number of services
		 to 40 as DSA is not supported. Changed conditionnaly build
		 from INCLUDE_TSFS_BOOT to INCLUDE_COPROCESSOR. Removed 
		 obsolete functions.
03i,03mar05,pch  support _WRS_WDB_CPU_TYPE
03h,27sep04,dbt  Updated for merge with usrWdb.c file.
03g,13aug04,dbt  Provide signal number in case of RTP exception.
03f,06jun04,dbt  Change the way to detect if text protection is enabled or
                 not.
03e,27apr04,elg  Modify exception hook wrapper.
03d,31mar04,elg  Remove unused coprocessor routines.
03c,23mar04,elg  Rework way contexts are transmitted in events.
03b,08mar04,elg  Modify WDB_CTX structure.
03a,26feb04,mil  Added SPE support for PPC85XX.
02z,03oct03,elg  Add CPU family and CPU variant.
02y,03oct03,elg  Fix typo error.
02x,08sep03,elg  Do not set page size if MPU is used (fix SPR #90333).
02w,05mar03,jmp  removed SIMNT specific code.
02v,28feb03,elg  Merge file from BSD/OS and define new runtime interface.
02u,05feb03,elg  Add WDB merged protocol.
02t,15feb01,jhw  No longer delete breakpoints in wdbExternEnterHook(), this 
		 is now done on external agent entry (SPR #64291).
02s,18sep00,tpr  replaced vxWorksVersion by runtimeVersion.
02r,14sep00,rbl  fixing use of TOOL and BSP_NAME so that quotes are not
                 necessary on the compile line
02q,05sep00,elg  Add fields toolName and bspShortName (SPR 32724).
02p,28aug00,elg  Change pdIsValid() to pdIdVerify().
02o,08aug00,elg  Add VM_CONTEXT_LOCK() and VM_CONTEXT_UNLOCK().
02n,11jul00,dbt  Reworked context management in system mode.
02m,13jun00,elg  Change current task's context in system mode.
02l,08jun00,dbt  Use BOOT_xxx macros instead of numeric constants for WDB boot
                 line length.
02k,31may00,elp  removed useless SIMNT code.
02j,26may00,elg  Fix bug in vxMemProtect().
02i,22may00,elg  Cancel context switches to kernel.
02h,18may00,dbt  Increased MAX_LEN according modification of BOOT_FILE_LEN.
02g,05apr00,aeg  deleted conditional test for taskCtxSwitch() invocation.
02f,29mar00,elg  Code cleanup.
02e,08mar00,elg  Modify the WDB_RT_INFO structure.
02d,14feb00,elg  Improve vxTaskCtxSwitch().
02c,02feb00,dbt  Remove the use of pdListGet.
02b,31jan00,elg  Fix bug in wdbConfig().
02a,26jan00,elg  Move vxTaskCtxSwitch () from usrWdbTask.c to usrWdbCore.c.
01z,06jan00,dra  PAL update: replace CPU tests.
01y,09nov99,gls  updated to use new vmPageProtect API
                 removed incorrect byte/address calculation in vxMemProtect
01x,09nov99,elp  adapted to VxWorks 6.0 (SIMNT).
01w,22oct99,elg  Add page size info in RT interface.
01v,06oct99,dbt  WDB agent pool is now a static buffer.
01u,27sep99,elg  Add temporary patch in memory protection.
01t,13sep99,bpn  Initialized the communication port variable with the value
                 defined in the ConfigTool (SPR #28606).
01s,24aug99,bpn  Use the new defined hook pointer _func_excWdbHook for the
                 WDB agent instead of the general hook pointer
                 _func_excBaseHook (SPR #8725)
                 The step mode of the task is disabled at the beginning of the
                 hook wrapper (SPR #26050).
01r,07jul99,dbt  allocate buffer for WDB agent memory pool only at the first
                 connection.
01q,21jun99,elg  Add protection domain information.
01p,27apr99,cym  removing wait for simulator IDE signaling
01o,20apr99,tam  pVmTextProtectRtn -> pVmPageProtectRtn
01n,22feb99,cym  increased time before notifying IDE (SPR #23942.)
01m,10jan99,map  allocate memory for wdb pool.
01m,07nov02,dtr  Adding SPE support.
01l,01apr02,jkf  SPR#74251, using bootLib.h macros for array sizes
01k,28feb02,jhw  Modified hasWriteProtect assignment. (SPR 71816).
01j,04oct01,c_c  Added wdbToolName symbol.
01i,06jun01,pch  Fix length of copy to vxBootFile.
01h,09may01,dtr  Addition of wdbTgtHasCoprocessor.
01g,19apr01,kab  Added Altivec support
01f,16nov00,zl   added DSP support.
01e,27apr99,cym  removing wait for simulator IDE signaling
01d,22feb99,cym  increased time before notifying IDE (SPR #23942.)
01c,08jul98,ms   need semTake initialized early if WDB_TASK included
01b,18may98,dbt  integrated latest WDB stuff
01a,21jul97,ms   taken from 01y of usrWdb.c
*/

/*
DESCRIPTION

This library configures the WDB agent's core facilities.
Agent initialization is a separate step.
*/

/* includes */

#include "configNet.h"
#include "taErrorDefine.h"

/* defines */

#define	NUM_MBUFS		5
#define MAX_LEN			BOOT_LINE_SIZE
#define WDB_MAX_SERVICES        40      /* max # of agent services */
#define WDB_POOL_BASE		((char *) (FREE_RAM_ADRS))
#define WDB_NUM_CL_BLKS         5

#if (!defined(BOOTCONFIG) && !defined(BOOTAPP))
#undef	INCLUDE_TSFS_BOOT	/* TSFS boot is only supported with bootrom */
#endif	/* BOOTCONFIG */

/* externals */
extern STATUS	wdbCommDevInit (WDB_COMM_IF * pCommIf, char ** ppWdbInBuf,
				      char ** ppWdbOutBuf);
IMPORT char	*wdbEndDeviceAddress;

/* globals */

uint_t		wdbCommMtu;
int		wdbNumMemRegions = 0;	/* number of extra memory regions */
BOOL		wdbIsInitialized = FALSE;
UINT16		wdbPort;                /* UDP port to connect */
UINT32		wdbMode;		/* current agent mode */
UINT32      wdbTimeOut = 0;
char	wdbInetAdrs[24];
char wdbEndName[END_NAME_MAX];
UINT32 wdbEndDeviceUnit = 0;

/*
 * wdbRegsBuffer is a buffer used by WDB agent to store the coproc register
 * temporarly. All data in this buffer is lost between two WDB request.
 */

#ifdef INCLUDE_WDB_REG
char 	wdbRegsBuffer [WDB_REGS_SIZE];
UINT32	wdbRegsBufferSize = WDB_REGS_SIZE;
#endif

/* This two macros transform TOOL define into the string "TOOL" */

#define MKSTR(MACRO) MKSTR_FIRST_PASS(MACRO)
#define MKSTR_FIRST_PASS(MACRO) #MACRO

/* 
 * This symbol "wdbToolName" is used by the tgtsvr to retrieve the name of the
 * tool used to build vxWorks run-time. DO NOT REMOVE !!!
 */

#ifdef	TOOL
const char wdbToolName[]	= MKSTR(TOOL);
#else	/* TOOL */
const char wdbToolName[]	= "Unknown";
#endif	/* TOOL */

/* locals */
static struct mbuf	mbufs [NUM_MBUFS];
static CL_BLK		wdbClBlks [WDB_NUM_CL_BLKS];
static WDB_COMM_IF	wdbCommIf;
static struct sockaddr_in senderAddr;

/* forward static declarations */
static STATUS	wdbCommIfInit (void);

/******************************************************************************
*
* wdbCksum - returns the ones compliment checksum of some memory.
*
* This routine is also used by the WDB UDP/IP module (which needs to do
* a ~htons(wdbCksum())) to get it right).
*
* RETURNS: the ones compliment checksum
*
* NOMANUAL
*/

UINT32 wdbCksum
    (
    UINT16 *	pAddr,			/* start of buffer */
    UINT32 	len,			/* size of buffer in bytes */
    UINT32	sumInit,		/* initial sum */
    BOOL	moreData		/* more data to come */
    )
    {
    UINT32 	sum = 0;
    static BOOL	swap = FALSE;

    if (sumInit == 0)
    	swap = FALSE;

    /* take care of unaligned buffer address */

    if ((long) pAddr & 0x1)
	{
	sum += *(unsigned char *) pAddr;
	pAddr = (UINT16 *)(((unsigned char *) pAddr) + 1);
	len--;
	swap = TRUE;
	}

    /* evaluate checksum */

    while (len > 1)
	{
	sum += *(UINT16 *) pAddr ++;
	len -= 2;
	}

    /* take care of last byte */

    if (len > 0)
	sum = sum + ((*(unsigned char *) pAddr) << 8);

    sum += sumInit;

    if (!moreData)
    	{
    	/* fold to 16 bits */

    	sum = (sum & 0xffff) + (sum >> 16);
    	sum = (sum & 0xffff) + (sum >> 16);

    	/* swap if we started on unaligned address */

    	if (swap)
	    sum = ((sum & 0x00ff) << 8) | ((sum & 0xff00) >> 8);
	}

    return (sum);
    }

/******************************************************************************
*
* wdbConfig - configure and initialize the WDB agent.
*
* This routine configures and initializes the WDB agent.
*
* RETURNS :
* OK or ERROR if the communication link cannot be initialized.
*
* NOMANUAL
*/

STATUS wdbConfig (char* targetIP, UINT16 targetPort, char *endDeviceName, UINT32 endDeviceUnit, UINT32 timeout)
{
    strncpy(wdbInetAdrs, targetIP, 24);
    wdbEndDeviceAddress = wdbInetAdrs;
	strncpy(wdbEndName, endDeviceName, END_NAME_MAX);
	wdbEndDeviceUnit = endDeviceUnit;

    if (wdbCommIfInit () == ERROR)	/* communication interface functions */
	{
    	if (_func_printErr != NULL)
    	    _func_printErr ("wdbConfig: error configuring WDB communication interface\n");
    	return (ERROR);
	}

    wdbPort = (UINT16) targetPort;	/* set the communication port to the */
    					/* value defined in the project tool */

    wdbIsInitialized = TRUE;	/* mark WDB as initialized */

    wdbTimeOut = timeout;

    return (OK);
}

/******************************************************************************
*
* wdbCommIfInit - Initialize the agent's communction interface
*
* RETURNS : OK or error if we can't initialize the communication interface.
*
* NOMANUAL
*/

static STATUS wdbCommIfInit (void)
    {
    char *		pWdbInBuf;
    char *		pWdbOutBuf;

    wdbMbufInit (mbufs, NUM_MBUFS, wdbClBlks, WDB_NUM_CL_BLKS);

    /* initialize the agent communication interface device */

    if (wdbCommDevInit (&wdbCommIf, &pWdbInBuf, &pWdbOutBuf) == ERROR)
	return (ERROR);

    return (OK);
    }

/*******************************************接口封装*********************************/

/*
 * @brief
 *    从UDP接收数据
 * @param[in]: buf: 接收数据缓存
 * @param[in]: bufLen:  缓存大小
 * @return
 * 	  ERROR: 发送数据失败
 *    size：收到的数据大小
 */
T_TA_ReturnCode taUdpRcvfrom(UINT8* buf,UINT32 bufLen,UINT32 *receSize)
{
	*receSize = wdbCommIf.rcvfrom(NULL, (caddr_t)buf, bufLen, &senderAddr, sizeof(struct sockaddr_in), wdbTimeOut);

	return TA_OK;
}

/*
 * @brief
 *    发送数据到UDP socket
 * @param[in]: buf:  发送数据缓存
 * @param[in]: bufLen:  数据大小
 * @param[in]: writeSize:  发送的数据大小
 * @return
 * 	  ERROR: 发送数据失败
 *    nBytes：发送数据大小
 */
T_TA_ReturnCode taUdpSendto(UINT8* buf,UINT32 bufLen,UINT32 *writeSize)
{
	*writeSize = wdbCommIf.sendto(NULL, (caddr_t)buf, bufLen, &senderAddr, sizeof(struct sockaddr_in));

	return TA_OK;
}

/**
* @brief
*    将网卡收发数据的模式变为轮询模式
* @param  无
* @return
*	OK:网卡模式切换成功
*	ERROR:网卡模式切换出错
*/
INT32 taSetEndToPoll(void)
{
	return (wdbCommIf.modeSet(NULL,WDB_COMM_MODE_POLL));
}

/**
* @brief
*    将网卡收发数据的模式变为中断模式
* @param  无
* @return
*	OK: 网卡模式切换成功
*	ERROR: 网卡模式切换出错
*/
INT32 taSetEndToInt(void)
{
	return (wdbCommIf.modeSet(NULL,WDB_COMM_MODE_INT));
}
