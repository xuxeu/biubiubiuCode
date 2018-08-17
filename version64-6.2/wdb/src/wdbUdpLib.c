/* wdbUdpLib.c - WDB communication interface UDP datagrams */

/*
 * Copyright (c) 1994-2005 Wind River Systems, Inc.  
 *
 * The right to copy, distribute, modify or otherwise make use  of this 
 * software may be licensed only pursuant to the terms of an applicable Wind 
 * River license agreement.
 */

/*
modification history
--------------------
01o,05oct07,dbt  No longer call semBCreate() in udpCommIfInit() since this can
                 be called before memory manager is initialized
                 (CQ:WIND00107114).
01n,06dec05,dbt  Updated with new sendto() and recvfrom() prototypes.
01m,01dec05,dbt  Removed references to timeval.
01l,31mar04,elg  fix memory leak in udpRcv() (SPR #95437).
01k,27feb03,elg  Merge file with BSD.
01j,14sep01,jhw  Fixed warnings from compiling with gnu -pedantic flag
01i,31oct00,elp  fixed freed buffer in udpRcv() (SPR #35871).
01h,27mar00,elg  Code cleanup.
01g,22jul98,dbt  in udpSendTo(), test wdbComMode rather thant wdbMode (with
                 wdbIsNowTasking) to choose if we must use semTake and
                 semGive.
01f,07apr97,gnn  turned version number into a #define
01e,07apr97,gnn  really added version number, was not really there.
01d,22nov96,vin  made modifications required for BSD44 cluster code.
		 added udpIpHdrBuf.
01c,29feb96,ms   added IP version number to IP header (SPR #5867).
01b,03jun95,ms	 use new commIf structure (sendto/rvcfrom replaces read/write).
01a,06oct94,ms   written.
*/

/*
DESCRIPTION

The WDB agent communicates with the host using sendto/rcvfrom function
pointers contained in a WDB_COMM_IF structure.
This library implements these methods using a lightweight UDP/IP stack
as the transport.
It supports two modes of operation; interrupt and polled, and the
mode can be dynamically toggled.

During agent initialization (in the file target/src/config/usrWdb.c), this
libraries udpCommIfInit() routine is called to initializes a WDB_COMM_IF
structure. A pointer to this structure is then passed to the agent's
wdbInstallCommIf().

Note: This particular library is not required by the WDB target agent.
Other communication libraries can be used instead simply by initializing
a WDB_COMM_IF structure and installing it in the agent.
*/

#include <wdb/wdb.h>
#include <wdb/wdbCommIfLib.h>
#include <wdb/wdbUdpLib.h>
#include <wdb/wdbMbufLib.h>

#include <string.h>
#include <sysLib.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define IP_HDR_SIZE	20
#define UDP_HDR_SIZE	8
#define UDP_IP_HDR_SIZE IP_HDR_SIZE + UDP_HDR_SIZE

#define MILLION		1000000
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* locals */

LOCAL UINT32    commMode;		/* POLL or INT */
LOCAL UINT32	agentIp;		/* in network byte order */

LOCAL struct mbuf *	pInputMbufs;
LOCAL WDB_DRV_IF * pWdbDrvIf;

LOCAL void	(*udpHookRtn) (u_int arg);
LOCAL UINT32	udpHookArg;

LOCAL SEMAPHORE	readSyncSem;
LOCAL SEMAPHORE	writeSyncSem;
LOCAL UCHAR 	udpIpHdrBuf [128]; 	/* 128 byte buffer for header */

/* foward declarations */

LOCAL INT32  udpRcvfrom (WDB_COMM_ID commId, caddr_t addr, UINT32 len,
			 void * pAddr, UINT32 addrLength, INT32 timeout);
LOCAL INT32  udpSendto	(WDB_COMM_ID commId, caddr_t addr, UINT32 len,
			 void * pAddr, UINT32 addrLength);
LOCAL INT32  udpModeSet (WDB_COMM_ID commId, UINT32 newMode);
LOCAL INT32  udpCancel	(WDB_COMM_ID commId);
LOCAL INT32  udpHookAdd (WDB_COMM_ID commId, void (*rout) (), UINT32 arg);

/* externals */

IMPORT UINT32 wdbCksum (UINT16 * pData, UINT32 nBytes,
			UINT32 init, BOOL moreData);

/******************************************************************************
*
* udpCommIfInit - Initialize the WDB communication function pointers.
*
* The configlette for the selected WDB_COMM_TYPE calls this function
* from its wdbCommDevInit routine. This function can only be called once.
*
* RETURNS: OK.
*
* NOMANUAL
*/

STATUS udpCommIfInit
    (
    WDB_COMM_IF * pWdbCommIf,		/* function pointers to install */
    WDB_DRV_IF *  pDrvIf
    )
    {
    pWdbCommIf->rcvfrom	= udpRcvfrom;
    pWdbCommIf->sendto	= udpSendto;
    pWdbCommIf->modeSet = udpModeSet;
    pWdbCommIf->cancel  = udpCancel;
    pWdbCommIf->hookAdd = udpHookAdd;
    pWdbCommIf->notifyHost = NULL;

    pWdbDrvIf		= pDrvIf;

    commMode = WDB_COMM_MODE_INT;	/* default communication mode */

    /*
     * Make use of semBInit() instead of semBCreate() because memory manager
     * may not be yet initialized when this routine is called.
     */

    semBInit (&readSyncSem, SEM_Q_PRIORITY, SEM_EMPTY);
    semBInit (&writeSyncSem, SEM_Q_PRIORITY, SEM_EMPTY);

    return (OK);
    }


/******************************************************************************
*
* udpModeSet - Set the communication mode to POLL or INT.
*
* RETURNS: OK on success, or ERROR if the new mode is not supported.
*
* NOMANUAL
*/

LOCAL INT32 udpModeSet
    (
    WDB_COMM_ID	commId,
    UINT32	newMode
    )
    {
    if (pWdbDrvIf->modeSetRtn == NULL)
	return (ERROR);

    if ((*pWdbDrvIf->modeSetRtn) (pWdbDrvIf->devId, newMode) == ERROR)
	return (ERROR);

    commMode = newMode;

    return (OK);
    }

/******************************************************************************
*
* udpRcv - receive a UDP/IP datagram from the driver.
*
* This routine reads UDP packets. When in system mode, a special hook is called
* to suspend the system and read the packet.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void udpRcv
    (
    struct mbuf * pMbuf		/* mbuf chain received */
    )
    {
    /* store away the data for the next agent read */

    if (pInputMbufs != NULL)		/* only allow one queued request */
	{
	wdbMbufChainFree (pMbuf);
	return;
	}
    pInputMbufs        = pMbuf;

    /* interrupt on first packet mode? Then call the input hook */

    if ((commMode == WDB_COMM_MODE_INT) && (udpHookRtn != NULL))
	{
	(*udpHookRtn) (udpHookArg);
	return;
	}

    /* normal application mode => unblock the task */

    if (commMode == WDB_COMM_MODE_INT)
	semGive (&readSyncSem);
    }

/******************************************************************************
*
* udpRcvfrom - read a UDP/IP datagram (after removing the UDP/IP headers).
*
* This routine assumes that the mbuf chain received from the driver
* has a UDP/IP header in the first mbuf.
*
* In polled mode operation, there is no timeout. This needs to be fixed 
* because it could cause an infinite loop waiting for data.
*
* RETURNS: number of read bytes.
*
* NOMANUAL
*/

LOCAL int udpRcvfrom
    (
    WDB_COMM_ID			commId,
    caddr_t			addr,		/* receive buffer */
    UINT32			len,		/* max packet length */
    void *			pAddr,		/* sender address */
    UINT32			addrLength,	/* sender address length */
    INT32			timeout		/* timeout in ms */
    )
    {
    struct udphdr *		pUdpHdr;
    struct ip *			pIpHdr;
    UINT32			nBytes;
    struct sockaddr_in *	pSockAddr = (struct sockaddr_in *) pAddr;

    /* Check sender address length value */

    if (addrLength < (sizeof (struct sockaddr_in)))
    	return (-1);

    /* WDB_COMM_MODE_INT means "task mode". Use semaphore to wait for data */

    if (commMode == WDB_COMM_MODE_INT)
	semTake (&readSyncSem, (timeout == -1 ?
				WAIT_FOREVER :
				(timeout * sysClkRateGet ()) / 1000));

    /* 
     * Otherwise poll the driver until data arrives or a timeout occurs.
     * Note that presently the timeout is not implemented
     */

    else
	{
	/* while ((volatile struct mbuf *) pInputMbufs == NULL) */
       timeout = timeout * 500;
       while (((volatile struct mbuf *) pInputMbufs == NULL) && (timeout-- > 0))
	    (*pWdbDrvIf->pollRtn) (pWdbDrvIf->devId);
	}

    /* Any data available ? */

    if (pInputMbufs == NULL)
	return (0);

    /* If so, break the packet up into components */

    pIpHdr  = mtod (pInputMbufs, struct ip *);
    pUdpHdr = (struct udphdr *) ((ulong_t) pIpHdr + IP_HDR_SIZE);

    /* If this packet is not for the agent, ignore it */

    if ((pIpHdr->ip_p      != IPPROTO_UDP) ||
	(pUdpHdr->uh_dport != htons(WDBPORT)) ||
	(pInputMbufs->m_len < UDP_IP_HDR_SIZE))
	{
	nBytes = 0;
        goto done;
	}

    /* Save away reply address info */

    agentIp		= pIpHdr->ip_dst.s_addr;
    pSockAddr->sin_port	= pUdpHdr->uh_sport;
    pSockAddr->sin_addr	= pIpHdr->ip_src;

    /* copy the RPC data into the uses buffer */

    pInputMbufs->m_len -= UDP_IP_HDR_SIZE;
    pInputMbufs->m_data += UDP_IP_HDR_SIZE;
    wdbMbufDataGet (pInputMbufs, addr, len, &nBytes);

done:
    /*
     * Free the mbuf chain:
     */

    wdbMbufChainFree (pInputMbufs);

    pInputMbufs = NULL;
    return (nBytes);
    }

/******************************************************************************
*
* udpCancel - cancel a udpRcvfrom
*
* This routine releases the UDP packet so that it can now be handled by the
* WDB task.
*
* RETURNS: OK.
*
* NOMANUAL
*/

LOCAL INT32 udpCancel
    (
    WDB_COMM_ID commId
    )
    {
    semGive (&readSyncSem);
    return (OK);
    }

/******************************************************************************
*
* udpSendto - send a reply packet (after adding a UDP/IP header).
*
* This routine builds and sends a UDP packet.*
*
* RETURNS: number of sent bytes.
*
* NOMANUAL
*/

LOCAL INT32 udpSendto
    (
    WDB_COMM_ID			commId,		/* unused */
    caddr_t			addr,		/* data */
    UINT32			len,		/* data length */
    void *			pAddr,		/* receiver address */
    UINT32			addrLength	/* receiver address length */
    )
    {
    static UINT16		ip_id;
    struct udphdr *    		pUdpHdr;
    struct ip *        		pIpHdr;
    STATUS			status;
    struct mbuf *		pDataMbuf;
    struct mbuf *		pHeaderMbuf;
    struct sockaddr_in *	pSockAddr = (struct sockaddr_in *) pAddr;

    /* allocate a couple of mbufs */

    if ((pDataMbuf = wdbMbufAlloc()) == NULL)
	return 0;

    if ((pHeaderMbuf = wdbMbufAlloc()) == NULL)
    	{
	wdbMbufFree (pDataMbuf);
	return 0;
	}

    /* put the reply data in an mbuf cluster */

    wdbMbufClusterInit (pDataMbuf, addr, len, NULL, 0);

    if (commMode == WDB_COMM_MODE_INT)
	{
	pDataMbuf->m_extFreeRtn	= semGive;
	pDataMbuf->m_extArg1	= (int)&writeSyncSem;
	}

    /* put the header data in the other mbuf */

    wdbMbufClusterInit (pHeaderMbuf, udpIpHdrBuf, len, NULL, 0); 

    pHeaderMbuf->m_data +=	40;
    pHeaderMbuf->m_len =	UDP_IP_HDR_SIZE;
    pHeaderMbuf->m_type =	MT_DATA;
    pHeaderMbuf->m_next  =	pDataMbuf;

    pIpHdr	= mtod (pHeaderMbuf, struct ip *);
    pUdpHdr	= (struct udphdr *)((int)pIpHdr + IP_HDR_SIZE);

    pIpHdr->ip_v        = IPVERSION;
    pIpHdr->ip_hl	= 0x45;
    pIpHdr->ip_tos	= 0;
    pIpHdr->ip_off	= 0;
    pIpHdr->ip_ttl	= 0x20;
    pIpHdr->ip_p	= IPPROTO_UDP;
    pIpHdr->ip_src.s_addr = agentIp;
    pIpHdr->ip_dst.s_addr = pSockAddr->sin_addr.s_addr;
    pIpHdr->ip_len	= htons (UDP_IP_HDR_SIZE + len);
    pIpHdr->ip_id	= ip_id++;
    pIpHdr->ip_sum	= 0;
    pIpHdr->ip_sum	= ~wdbCksum ((UINT16 *) pIpHdr, IP_HDR_SIZE, 0, FALSE);

    pUdpHdr->uh_sport	= htons(WDBPORT);
    pUdpHdr->uh_dport	= pSockAddr->sin_port;
    pUdpHdr->uh_sum	= 0;
    pUdpHdr->uh_ulen 	= htons (UDP_HDR_SIZE + len);

    /* tell the driver to output the data */

    if (pWdbDrvIf == NULL)
	return (ERROR);
    status = (*pWdbDrvIf->pktTxRtn) (pWdbDrvIf->devId, pHeaderMbuf);
    if (status == ERROR)
	return (ERROR);

    /* wait for the driver to finish transmition before returning */

    if (commMode == WDB_COMM_MODE_INT)
	semTake (&writeSyncSem, WAIT_FOREVER);

    return (UDP_IP_HDR_SIZE + len);
    }

/******************************************************************************
*
* udpHookAdd - add a hook at UDP packet receiving.
*
* This routine adds a hook that will be called when reading a UDP packet in
* interrupt mode.
*
* RETURNS: OK
*
* NOMANUAL
*/

LOCAL INT32 udpHookAdd
    (
    WDB_COMM_ID commId,
    void	(*rout) (),
    UINT32	arg
    )
    {
    udpHookRtn = rout;
    udpHookArg = arg;

    return (OK);
    }
