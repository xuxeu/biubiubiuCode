/* bootDhcpcLoad.c - Boot loader application DHCP protocol support component */

/* 
*  Copyright (c) 2005, 2007-2010, 2012, 2013 Wind River Systems, Inc.
* 
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
01n,02mar15,lzq  Fix user'callback wait for ever issue(VXW6-84129)
01m,12sep13,xms  Fix DHCP negotiate twice issue. (WIND00430551)
01l,05sep13,xms  fix CHECKED_RETURN error. (WIND00414265)
01k,29may13,d_l  Fix prevent errors. (WIND00419006)
01j,14mar13,lzq  Fix for WIND00404960,Up interface before running DHCP 
01i,10jan12,pgh  Fix sprintf() and strcpy() usage.
01h,18nov10,rlg  fix coverity bug
01g,15dec09,pgh  64-bit conversion
01f,28apr09,pgh  Update documentation.
01e,06jun08,pgh  Merge DHCP fixes
01d,01jun08,spw  Update API for bootappDhcpOptionCallback
01c,06sep07,pgh  Add DHCP boot support to bootapp.
01b,06feb07,jmt  Modify bootAppPrint to optimize size
01a,11jul05,jmt   written
*/

/*
DESCRIPTION
This module contains the support routines for the vxWorks boot loader 
application DHCP protocol component.  This module is added to the boot 
loader application when the INCLUDE_BOOT_DHCPC component is included.  
This module consists of an initialization routine, a routine to update 
the network configuration based on the received DHCP parameters, a call 
back routine to extract the DHCP parameters as the DHCP packets are 
received by the network stack, routines to enable and disable DHCP in 
the network stack, and a routine to extract from the network stack the 
IP address of the boot device.  

The network configuration update routine is executed through the 
bootDhcpcAdrAdjustFcn function pointer, which this module initializes.
The bootDhcpcAdrAdjustFcn function pointer is executed by the 
bootNetLoad.c module when this function pointer is not NULL.

When DHCP is enabled in the boot loader application, the network file load 
routine will call the routine that updates the network configuration with 
the received DHCP parameters.  This routine waits for all of the DHCP packets 
to be received, then updates the boot line parameters with the values received
via the DHCP protocol.

In addition to including the INCLUDE_BOOT_DHCPC component in the boot loader 
application, DHCP is enabled and disabled by a boot flag.  When enabled, the
boot device IP address, subnet mask, and default gateway parameters will be 
updated with the values received via DHCP.  An additional boot flag is used 
to determine if the boot file will be based on the boot line parameters or
based on the file specified by the DHCP protocol.  If the flag is set to use
the DHCP boot file, then the host IP address parameter is updated with the
IP address received from DHCP, and the boot file parameter is updated with
the file name specified by DHCP.

This module returns to the network file load module to then load the boot
file with the boot parameters updated by DHCP.

INCLUDE FILES: bootNetLoad.h bootAppLoad.h vxWorks.h 
*/ 

/* includes */

#include <vxWorks.h>
#include <bootLib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <taskLib.h>
#include <ipcom_cstyle.h>
#include <ipcom_sock.h>
#include <ipdhcpc_config.h>
#include <ipdhcpc.h>
#include <in.h>
#include <sysLib.h>
#include <bootLib.h>
#include <bootApp.h>
#include <bootNetLoad.h>
#include <muxLib.h>

/* defines */

#define SERV_NAME_LEN               64
#define BOOTFILE_NAME_LEN           256
#define MIN_IPADDR_LEN              6

/* DHCP options received flags */
#define DHCPC_OPT_YOUR_ADDR         0x00000001
#define DHCPC_OPT_SUBNETMASK        0x00000002
#define DHCPC_OPT_ROUTER_ADDR       0x00000004
#define DHCPC_OPT_LEASE_TIME        0x00000008
#define DHCPC_OPT_TFTP_SERV_ADDR    0x00000010
#define DHCPC_OPT_TFTP_SERV_NAME    0x00000020
#define DHCPC_OPT_BOOTFILE_NAME     0x00000040
#define DHCPC_OPT_VENDOR_OPTIONS    0x00000080

/* typedefs */

/* globals */

extern bootNetAdrAdjustPrototype * bootDhcpcAdrAdjustFcn;
extern bootDhcpEnableDisablePrototype * bootDhcpcEnableFcn;
extern bootDhcpEnableDisablePrototype * bootDhcpcDisableFcn;
extern FUNCPTR _ipdhcpc_callback_hook;
extern BOOL bootDhcpAllocDone;
int  dhcpParamDone = FALSE;   /* DHCP parameters not received */

/* locals */

LOCAL int  dhcpNetMask = 0;         /* IP network mask */
LOCAL int  dhcpGatewayAddr = 0;     /* IP default gateway address */
LOCAL int  dhcpLeaseTime = 0;       /* DHCP lease time */
LOCAL int  dhcpServerAddr = 0;      /* TFTP file server IP address */
LOCAL char dhcpServerName[SERV_NAME_LEN];       /* TFTP file server name */
LOCAL char dhcpBootFileName[BOOTFILE_NAME_LEN]; /* boot file name */
LOCAL int  dhcpOptions = 0;         /* DHCP options received flags */

/* imports */

 
/* forward declarations */

void bootDhcpcLoadInit (void);
LOCAL void bootDhcpcAdrAdjust (void);
LOCAL Ip_err bootappDhcpOptionCallback(Ipdhcpc_handle *, Ip_u8 *);
LOCAL STATUS bootDhcpcEnable (int, char *);
LOCAL STATUS bootDhcpcDisable (int, char *);
LOCAL int getNetAddr (char *, int);


/*******************************************************************************
*
* bootNetLoadInit - Initialize the boot loader application DHCP Component
*
* This routine initializes the boot loader application DHCP component.  This 
* routine installs the function pointers to the DHCP enable and disable 
* routines, and to the routine that updates the boot parameters with the 
* values received from DHCP.  In addition, it installs the call back function 
* for the network stack to call when DHCP packets are receive, and it adds 
* boot flag help strings for the two DHCP boot flags.
* 
* RETURNS: Nothing
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void bootDhcpcLoadInit ()
    {
    /* initialize variables */
    bootDhcpcAdrAdjustFcn = (bootNetAdrAdjustPrototype *) bootDhcpcAdrAdjust;
    bootDhcpcEnableFcn = (bootDhcpEnableDisablePrototype *) bootDhcpcEnable;
    bootDhcpcDisableFcn = (bootDhcpEnableDisablePrototype *) bootDhcpcDisable;

    _ipdhcpc_callback_hook = bootappDhcpOptionCallback;

    /* setup dhcp flag help */
    bootFlagHelpAdd(0x040, "  use DHCP to get IP address parameters");
    bootFlagHelpAdd(0x800, "  use DHCP to get boot file parameters");

    dhcpParamDone = FALSE;              /* DHCP parameters not received */
    dhcpNetMask = 0;                    /* IP network mask */
    dhcpGatewayAddr = 0;                /* IP default gateway address */
    dhcpLeaseTime = 0;                  /* DHCP lease time */
    dhcpServerAddr = 0;                 /* TFTP file server IP address */
    memset(dhcpServerName, 0, SERV_NAME_LEN);       /* clear server name */
    memset(dhcpBootFileName, 0, BOOTFILE_NAME_LEN); /* clear boot file name */
    dhcpOptions = 0;                    /* no options received yet */
    }

/******************************************************************************
*
* bootDhcpcAdrAdjust - Update boot parameters with DHCP values
*
* This routine updates the boot line parameters with values received from the
* DHCP packets.  This routine waits for the DHCP packets to be received, then
* updates the boot device IP address with the value received from DHCP.  It 
* then checks to see if a subnet mask was received, and if so updates the
* boot parameter with the received subnet maks.  If a default router, or
* gateway IP address was received via DHCP, then the boot parameters are
* updated with this value.  This routine then checks to see if the boot flag
* is set to use the DHCP specified boot file.  If so, the routine checks if
* a host IP address was received via DHCP, and updates the boot parameters
* with this address.  it also verifies that the boot file was received from
* DHCP and updates the boot parameters with the received boot file name.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*
* SEE ALSO: RFC 1541
*/

#define MASK_STR_LEN    16

LOCAL void bootDhcpcAdrAdjust ()
    {
    int             loopCnt = 0;
    unsigned char   addr[4];
    char            maskStr[MASK_STR_LEN];
    int             nameLen;
    int             netAddr;

    if (dhcpParamDone == FALSE)   /* Are DHCP parameters are complete? */
        {
        bootAppPrintf ("Waiting for DHCP, CTRL-X to abort.\n", 0,0,0,0,0,0);
        }

    /* Wait for DHCP parameters to complete? */

    while (dhcpParamDone == FALSE)
        {
        taskDelay(2);
        loopCnt++;
        if ((loopCnt & 0x7F) == 0)
            bootAppPrintf (".", 0,0,0,0,0,0);
        }

    /* Update bootline network addr with addr received via DHCP. */

    netAddr = getNetAddr (sysBootParams.bootDev, sysBootParams.unitNum);
    addr[0] = (unsigned char)((netAddr >> 24) & 0xFF);
    addr[1] = (unsigned char)((netAddr >> 16) & 0xFF);
    addr[2] = (unsigned char)((netAddr >> 8) & 0xFF);
    addr[3] = (unsigned char)(netAddr & 0xFF);
    (void)snprintf (sysBootParams.ead, BOOT_TARGET_ADDR_LEN, "%d.%d.%d.%d",
                                 addr[0], addr[1], addr[2], addr[3]);

    /* If subnet mask received via DHCP, then add mask to network addr. */

    if (dhcpOptions & DHCPC_OPT_SUBNETMASK)
        {
        (void)snprintf(maskStr, MASK_STR_LEN, ":%8x", dhcpNetMask);
        (void)strncat(sysBootParams.ead, maskStr, 
                      (BOOT_TARGET_ADDR_LEN - strlen(sysBootParams.ead)));
        sysBootParams.ead[BOOT_TARGET_ADDR_LEN - 1] = EOS;
        }

    /* If router addr received via DHCP, then update bootline gateway addr. */

    if (dhcpOptions & DHCPC_OPT_ROUTER_ADDR)
        {
        addr[0] = (unsigned char)((dhcpGatewayAddr >> 24) & 0xFF);
        addr[1] = (unsigned char)((dhcpGatewayAddr >> 16) & 0xFF);
        addr[2] = (unsigned char)((dhcpGatewayAddr >> 8) & 0xFF);
        addr[3] = (unsigned char)(dhcpGatewayAddr & 0xFF);
        (void)snprintf (sysBootParams.gad, BOOT_ADDR_LEN, "%d.%d.%d.%d",
                                    addr[0], addr[1], addr[2], addr[3]);
        }

    /* Get boot file from DHCP? */

    if (sysFlags & SYSFLG_AUTOFILE)
        {
        /* If server addr received via DHCP, then update bootline host addr. */

        if (dhcpOptions & DHCPC_OPT_TFTP_SERV_ADDR)
            {
            addr[0] = (unsigned char)((dhcpServerAddr >> 24) & 0xFF);
            addr[1] = (unsigned char)((dhcpServerAddr >> 16) & 0xFF);
            addr[2] = (unsigned char)((dhcpServerAddr >> 8) & 0xFF);
            addr[3] = (unsigned char)(dhcpServerAddr & 0xFF);
            (void)snprintf (sysBootParams.had, BOOT_ADDR_LEN, "%d.%d.%d.%d", 
                                addr[0], addr[1], addr[2], addr[3]);
            }

        /* If boot file received via DHCP, then update bootline file name. */

        if (dhcpOptions & DHCPC_OPT_BOOTFILE_NAME)
            {
            nameLen = (int)strlen(dhcpBootFileName);

            if (nameLen >= BOOT_FILE_LEN)
                {
                (void)strncpy(sysBootParams.bootFile, 
                              (const char *)dhcpBootFileName, 
                              BOOT_FILE_LEN - 1);
                sysBootParams.bootFile[BOOT_FILE_LEN - 1] = 0;
                }
            else
                {
                (void)strncpy(sysBootParams.bootFile, 
                              (const char *)dhcpBootFileName, 
                              (size_t)nameLen);
                sysBootParams.bootFile[nameLen] = 0;
                }
            }
        }
    }

/*****************************************************************************
*
* bootappDhcpOptionCallback - Boot loader DHCP network stack call back
*
* This routine implements the network stack DHCP call back for the boot
* loader application.  This routine is called by assigning it to the 
* DHCPC_OPTION_CALLBACK_HOOK component parameter in the 20comp_bootApp.cdf
* file.  This routine identifies the DHCP parameter type that has been
* received and saves it.  It also flags the receipt of the last DHCP packet
* to let the DHCP update boot parameters routine know that all DHCP packets
* have been received.
*
* RETURNS: IP_TRUE - let IP stack act on parameters
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL Ip_err bootappDhcpOptionCallback
    (
    Ipdhcpc_handle *handle, /* DHCP client request parameters */
    Ip_u8 *option           /* DHCP parameter ID and value */
    )
    {
    (void)handle;

    switch (*option)
    {
    case IPDHCPC_OPTCODE_SUBNET_MASK:
        dhcpNetMask = (int)IP_GET_32ON8(&option[2]);
        dhcpNetMask = (int)ntohl(dhcpNetMask);
        dhcpOptions |= DHCPC_OPT_SUBNETMASK;
        break;
    case IPDHCPC_OPTCODE_ROUTERS:
        dhcpGatewayAddr = (int)IP_GET_32ON8(&option[2]);
        dhcpGatewayAddr = (int)ntohl(dhcpGatewayAddr);
        dhcpOptions |= DHCPC_OPT_ROUTER_ADDR;
        break;
    case IPDHCPC_OPTCODE_DHCP_LEASE_TIME:
        dhcpLeaseTime = (int)IP_GET_NTOHL(&option[2]);
        dhcpOptions |= DHCPC_OPT_LEASE_TIME;
        break;
    case IPDHCPC_OPTCODE_TFTP_SERVER_ADDRESS:
        dhcpServerAddr = (int)IP_GET_32ON8(&option[2]);
        dhcpServerAddr = (int)ntohl(dhcpServerAddr);
        dhcpOptions |= DHCPC_OPT_TFTP_SERV_ADDR;
        break;
    case IPDHCPC_OPTCODE_TFTP_SERVER_NAME:
        strncpy(dhcpServerName, 
                (const char *)&option[2], 
                (size_t)option[1]);
        dhcpOptions |= DHCPC_OPT_TFTP_SERV_NAME;
        break;
    case IPDHCPC_OPTCODE_BOOTFILE_NAME:
        bcopyBytes((char *)&option[2], &dhcpBootFileName[0], (int)option[1]);
        dhcpBootFileName[option[1]] = 0;
        dhcpOptions |= DHCPC_OPT_BOOTFILE_NAME;
        break;
    case IPDHCPC_OPTCODE_VENDOR_ENCAPSULATED_OPTIONS:
        dhcpOptions |= DHCPC_OPT_VENDOR_OPTIONS;
        break;
    case IPDHCPC_OPTCODE_END:
        dhcpParamDone = TRUE;   /* DHCP parameters are complete */
        bootDhcpAllocDone = TRUE;
        break;
    default:
        break;
    }

    /* Let IPDHCPC act on option */
    return IP_TRUE;
    }

/*******************************************************************************
*
* bootDhcpcDisable - Disable DHCP client for network device
*
* This routine disables the DHCP client in the TCP/IP stack for the specified
* network device.
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL STATUS bootDhcpcDisable
    (
    int     unit,       /* Unit number  */
    char *  pDevice     /* Device name (i.e. ln, ei etc.). */
    )
    {
    Ip_fd           fd;
    struct Ip_ifreq ifr;
    int             ret;
    char            ifname[IP_IFNAMSIZ];
    END_OBJ *       pEnd;

    memset (&ifr, 0, sizeof (ifr));

    (void)snprintf(ifname, IP_IFNAMSIZ, "%s%d", pDevice, unit);
    pEnd = endFindByName (pDevice, unit);
    if ((pEnd == NULL) || (strlen (ifname) >= IP_IFNAMSIZ))
        {
        return ERROR;
        }

    /* copy interface name to interface structure */
    (void)strncpy (ifr.ifr_name, ifname, (IP_IFNAMSIZ - 1));

    /* get the fd argument */
    fd = ipcom_socket (IP_AF_INET, IP_SOCK_RAW, 0);
    if (fd < 0)
        {
        fd = ipcom_socket (IP_AF_INET6, IP_SOCK_RAW, 0);
        if (fd < 0)
            return ERROR;
        }

    /* Disable DHCP client on the interface */

    ifr.ifr_ifru.ifru_opt = 0;
    ret = ipcom_socketioctl (fd, IP_SIOCXSDHCPRUNNING, &ifr);
    ipcom_socketclose(fd);

    return (ret);
    }

/*******************************************************************************
*
* bootDhcpcEnable - Enable DHCP client for network device
*
* This routine enables the DHCP client in the TCP/IP stack for the specified
* network device.
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL STATUS bootDhcpcEnable
    (
    int     unit,       /* Unit number  */
    char *  pDevice     /* Device name (i.e. ln, ei etc.). */
    )
    {
    Ip_fd           fd;
    struct Ip_ifreq ifr;
    int             ret;
    char            ifname[IP_IFNAMSIZ];
    END_OBJ *       pEnd;

    memset (&ifr, 0, sizeof (ifr));

    (void)snprintf(ifname, IP_IFNAMSIZ, "%s%d", pDevice, unit);
    pEnd = endFindByName (pDevice, unit);
    if ((pEnd == NULL) || (strlen (ifname) >= IP_IFNAMSIZ))
        {
        return ERROR;
        }

    /* copy interface name to interface structure */
    (void)strncpy (ifr.ifr_name, ifname, (IP_IFNAMSIZ - 1));

    /* get the fd argument */
    fd = ipcom_socket (IP_AF_INET, IP_SOCK_RAW, 0);
    if (fd < 0)
        {
        fd = ipcom_socket (IP_AF_INET6, IP_SOCK_RAW, 0);
        if (fd < 0)
            return ERROR;
        }

    ret = ipcom_socketioctl (fd, IP_SIOCGIFFLAGS, &ifr);
    if (ret < 0)
        {
        ipcom_socketclose(fd);
        return ret;
        }

    IP_BIT_SET (ifr.ip_ifr_flags, IP_IFF_UP);

    ret =  ipcom_socketioctl (fd, IP_SIOCSIFFLAGS, &ifr);
    if (ret < 0)
        {
        ipcom_socketclose(fd);
        return ret;
        }

    /* Enable DHCP client on the interface */

    ifr.ifr_ifru.ifru_opt = 1;
    ret = ipcom_socketioctl (fd, IP_SIOCXSDHCPRUNNING, &ifr);
    ipcom_socketclose(fd);

    return (ret);
    }

/*******************************************************************************
*
* getNetAddr - Get network address obtained via DHCP
*
* This routine gets the network address obtained via DHCP from the network 
* stack.
*
* RETURNS: binary value of network address
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL int getNetAddr
    (
    char *  pDevice,    /* boot device name (i.e. ln, ei etc.). */
    int     unit        /* boot device unit number  */
    )
    {
    Ip_fd           fd;
    struct Ip_ifreq ifr;
    struct Ip_sockaddr_in *sin;
    int             ret;
    char            ifname[IP_IFNAMSIZ];
    END_OBJ *       pEnd;

    memset (&ifr, 0, sizeof (ifr));

    (void)snprintf(ifname, IP_IFNAMSIZ, "%s%d", pDevice, unit);
    pEnd = endFindByName (pDevice, unit);
    if ((pEnd == NULL) || (strlen (ifname) >= IP_IFNAMSIZ))
        {
        return (0);
        }

    /* copy interface name to interface structure */
    (void)strncpy (ifr.ifr_name, ifname, (IP_IFNAMSIZ - 1));

    /* get the fd argument */
    fd = ipcom_socket (IP_AF_INET, IP_SOCK_RAW, 0);
    if (fd < 0)
        {
        fd = ipcom_socket (IP_AF_INET6, IP_SOCK_RAW, 0);
        if (fd < 0)
            return (0);
        }

    /* Get network address for the interface */

    ret = ipcom_socketioctl (fd, IP_SIOCGIFADDR, &ifr);
    ipcom_socketclose(fd);
    if (ret == ERROR)
        return (0);

    sin = (struct Ip_sockaddr_in*)&ifr.ip_ifr_addr;
    return ((int)ntohl(sin->sin_addr.s_addr));
    }

