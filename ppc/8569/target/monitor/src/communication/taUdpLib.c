/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                         创建该文件。
 */

/*
 * @file: taUdpLib.c
 * @brief:
 *             <li>udp协议库</li>
 */
 
/************************头 文 件******************************/
#include "ta.h"
#include "taUdpLib.h"

/************************宏 定 义********************************/

#define TP_DEBUG(...)

/************************类型定义*******************************/

/************************外部声明*******************************/

/************************前向声明*******************************/

/************************模块变量*******************************/

/* 目标机端网络MAC地址 */
T_MODULE UINT8 saTaUbLocalMac[TA_MAC_BYTE_SUM]={0};

/* 主机端网络MAC地址 */
T_MODULE UINT8 saTaUbHostMac[TA_MAC_BYTE_SUM]={0};

/* 目标机端网络IP地址 */
T_MODULE UINT8 saTaUbLocalIP[TA_IP_BYTE_SUM]={0};

/* 主机端网络IP地址 */
T_MODULE UINT32  sTagentHostIp=0;

/* 目标机端网络端口号 */
T_MODULE UINT16 sTaUhLocalPort=0;

/* 主机端网络端口号 */
T_MODULE UINT16 sTaUhHostPort=0;

/* 目标机端接收数据包缓冲 */
T_MODULE UINT32 TaRcvBuf[(TA_ETHER_FRAME_MAX_SIZE + 16) / 4];

/* 目标机端发送数据包缓冲 */
T_MODULE UINT32 TaSendBuf[(TA_ETHER_FRAME_MAX_SIZE + 16) / 4];

T_MODULE UINT8 *sTaRcvBuf = ((UINT8 *)TaRcvBuf);
T_MODULE UINT8 *sTaSendBuf = ((UINT8 *)TaSendBuf + 2);

/************************全局变量*******************************/

/************************函数实现*******************************/

/*
 * @brief:
 *    将字符串格式IP地址转换成字节数组格式
 * @param[in]: string: 指向字符串格式IP的指针
 * @param[out]: array: 指向数组格式IP的指针
 * @return:
 *    TRUE: 转换成功
 *    FALSE: 转换后IP地址的第一个字节为零，
 *          字符串格式IP地址没有用"."作分隔
 */
T_MODULE BOOL taIpAddrConvert(UINT8 *string, UINT8 *array)
{
    UINT32 idx;
    UINT8 *ptr = string;
    UINT32 ipNum;

    /* 首先解析IP的第一个字节 */
    ipNum = strtoi(ptr,10);
    if ((0 == ipNum) || (ipNum > 255))
    {
        /* IP地址的第一个字节不得为零或大于255 */
        TP_DEBUG("TA_ERROR: Invalid IP: %s\n",string);
        return FALSE;
    }

    array[IP0] = (UINT8)ipNum;

    /* 解析后续的IP地址 */
    for (idx = IP1; idx < TA_IP_BYTE_SUM; idx++)
    {
        ptr = strchr(ptr,'.');
        if (ptr == NULL)
        {
            /* IP地址没有用"."字符作分隔 */
            TP_DEBUG("TA_ERROR: Invalid IP: %s\n",string);
            return FALSE;
        }

        /* 跳过"."字符 */
        ptr++;

        /* 转换后续的字节 */
        ipNum = strtoi(ptr,10);
        if (ipNum > 255)
        {
            TP_DEBUG("TA_ERROR: Invalid IP: %s\n",string);
            return FALSE;
        }
        array[idx] = (UINT8)ipNum;
    }

    ptr = strchr(ptr,'.');
    if (ptr != NULL)
    {
        /* IP地址格式不正确 */
        TP_DEBUG("TA_ERROR: Invalid IP: %s\n",string);
        return FALSE;
    }

    return TRUE;
}

/*
 * @brief:
 *    将字符串格式的MAC地址转换成字节数组格式
 * @param[in]: string: 指向字符串格式MAC地址的指针
 * @param[out]: array: 指向字节数组格式MAC地址的指针
 * @return:
 *    TRUE: 转换成功。
 *    FALSE: 字符串格式MAC地址没有用"-"作分隔
 */
T_MODULE BOOL taMacAddrConvert(UINT8 *string, UINT8 *array)
{
    UINT32 idx;
    UINT8 *ptr = string;
    UINT32 macNum;

    if (string[17] != (UINT8)'\0')
    {
        /* MAC地址格式不正确 */
        TP_DEBUG("TA_ERROR: Invalid MAC\n");
        return FALSE;
    }

    /* 首先解析MAC的第一个字节 */
    macNum = strtoi(ptr,16);
    if (macNum > 0xff)
    {
        TP_DEBUG("TA_ERROR: Invalid MAC: %s\n",string);
        return FALSE;
    }
    
    array[MAC0] = (UINT8)macNum;

    /* 解析后续的MAC地址 */
    for (idx = MAC1; idx < TA_MAC_BYTE_SUM; idx++)
    {
        ptr = strchr(ptr,'-');
        if (ptr == NULL)
        {
            /* MAC地址没有用"-"作分隔 */
            TP_DEBUG("TA_ERROR: Invalid MAC: %s\n",string);
            return FALSE;
        }

        /* 跳过"-"字符 */
        ptr++;

        /* 获得后续的字节 */
        macNum = strtoi(ptr,16);
        if (macNum > 0xff)
        {
            TP_DEBUG("TA_ERROR: Invalid MAC: %s\n",string);
            return FALSE;
        }
        array[idx] = (UINT8)macNum;
    }

    return TRUE;
}

/**
 * @brief:
 *    UDP设备读协议
 * @param[in]: buf: 包
 * @param[in]: cnt: 允许的最大包大小
 * @param[in]: dlen: 实际接收的包大小
 * @return:
 *    TA_OK: 读取成功
 *    TA_GET_PACKET_DATA_FAIL: 读取失败
 */
T_TA_ReturnCode taUdpRead(UINT8* buf,UINT32 cnt,UINT32* dlen)
{
    INT32 loopCount = 5000;
    INT32 idx = 0;

    /* 真实接收大小清0 */
    *dlen = 0;

    /* 从以太网设备读取新的以太数据帧 */
    while (loopCount --> (INT32)0)
    {
        TA_EtherFrame *ether = NULL;

        /* 为了保证在ARM环境下的字节对齐，这里将rawUdpOffset设置为2 */
        INT32 rawUdpOffset = 2;
        {
            /* 调用驱动层接口接收数据包 */
            INT32 len = taNetGetpkt(0,sTaRcvBuf,&rawUdpOffset);

            if (0 == len)
            {
                /* 收包失败,重复读取 */
                continue;
            }

        }

        /* 获得以太帧头地址 */
        ether = (TA_EtherFrame *)(sTaRcvBuf+rawUdpOffset);

        /* 进行字节顺序转换 */
        ether->type = ntohs(ether->type);
		
        /* 当前帧是一个ARP数据报文 */
        /* ARP请求数据帧中包含目的主机的IP地址，
         * 其意思是 "如果你是这个IP地址的拥有者，请回答你的硬件地址"
         */
        if (ether->type == TA_ETFR_TYPE_ARP)
        {
            /* 获得ARP报文 */
            TA_ArpPacket *arp = (TA_ArpPacket*)ether->data;

            /* 进行字节顺序转换 */
            arp->op = ntohs(arp->op);

            /* 检查ARP报文是否是ARP请求操作，如果不是则接收下一个数据包 */
            if (arp->op != ARP_AREQ)
            {
                continue;
            }

            /* 检查ARP报文中的目的主机IP地址是否与本地IP地址匹配，如果匹配
             * 则组包发送本机的硬件地址，否则直接接收下一个报文
             */
            if ((arp->des_ip[IP0] != saTaUbLocalIP[IP0]) ||
            (arp->des_ip[IP1] != saTaUbLocalIP[IP1]) ||
            (arp->des_ip[IP2] != saTaUbLocalIP[IP2]) ||
            (arp->des_ip[IP3] != saTaUbLocalIP[IP3]))
            {
                continue;
            }

            /* 组装ARP应答报文，首先是以太帧头 */
            {
                TA_ArpPacket *ack_arp = NULL;

                /* 获取目标机端发送数据包缓冲的地址 */
                ether = (TA_EtherFrame*)sTaSendBuf;

                /* 字节顺序转换 */
                ether->type = htons(TA_ETFR_TYPE_ARP);

                /* ARP报文填写 */
                ack_arp = (TA_ArpPacket*)ether->data;
                ack_arp->hd_type = htons(TA_ARP_HD_TYPE_ETHER);
                ack_arp->pr_type = htons(TA_ARP_PR_TYPE_IP);
                ack_arp->hd_size = TA_MAC_BYTE_SUM;
                ack_arp->pr_size = TA_IP_BYTE_SUM;
                ack_arp->op = htons(ARP_AACK);

                /* MAC地址的源头和目的互换 */
                for (idx = MAC0; idx < TA_MAC_BYTE_SUM; idx++)
                {
                    ether->des_addr[idx] = arp->src_mac[idx];
                    ether->src_addr[idx] = saTaUbLocalMac[idx];

                    ack_arp->src_mac[idx] = saTaUbLocalMac[idx];
                    ack_arp->des_mac[idx] = arp->src_mac[idx];
                }

                /* IP地址的源头和目的互换 */
                for (idx = IP0; idx < TA_IP_BYTE_SUM; idx++)
                {
                    ack_arp->src_ip[idx] = arp->des_ip[idx];
                    ack_arp->des_ip[idx] = arp->src_ip[idx];
                }

            }

            /* 发送ARP报文 */
            taNetPutpkt(0,sTaSendBuf,TA_ETHER_ARP_FRAME_SIZE);
            continue;
        }/* ARP数据报处理结束 */

        /* 当前帧是一个IP数据报文 */
        if (ether->type == TA_ETFR_TYPE_IP)
        {
            /* 获得IP报头 */
            TA_IpPacket* ip = (TA_IpPacket*)ether->data;

            /* 如果IP报文的目的地址与本地地址不匹配则丢弃该报文 */
            if (ntohl(ip->des_addr) != makeip(saTaUbLocalIP))
            {
                TP_DEBUG("des ip error, 0x%x, packet:0x%x, local:0x%x\n", ip->des_addr, ntohl(ip->des_addr), makeip(saTaUbLocalIP));
                continue;
            }

            /* 当前报文是一个ICMP报文 */
            if (ip->protocol == IPPROTO_ICMP)
            {
                /* 获得ICMP报头 */
                TA_IcmpPacket *icmp = (TA_IcmpPacket*)(ether->data + (ip->ipLen * 4));

                /* 如果报文类型不是一个Pinq请求则丢弃该报文 */
                if (icmp->type != ICMP_PING_REQ)
                {
                    continue;
                }

                /* 组装Ping应答报文，首先是以太帧头 */
                {
                    TA_EtherFrame *ack_ether = NULL;
                    TA_IpPacket *ack_ip = NULL;
                    TA_IcmpPacket *ack_icmp = NULL;
                    idx = 0;

                    ack_ether = (TA_EtherFrame*)sTaSendBuf;
                    ack_ether->type = htons(TA_ETFR_TYPE_IP);

                    /* 交换源头和目的MAC地址 */
                    for (idx = MAC0; idx < TA_MAC_BYTE_SUM; idx++)
                    {
                        ack_ether->des_addr[idx] = ether->src_addr[idx];
                        ack_ether->src_addr[idx] = ether->des_addr[idx];
                    }

                    /* 然后是IP报文 */
                    ack_ip = (TA_IpPacket*)ack_ether->data;
                    memcpy(ack_ip,ip,ntohs(ip->length));

                    /* 交换IP地址 */
                    ack_ip->src_addr = ip->des_addr;
                    ack_ip->des_addr = ip->src_addr;

                    ack_ip->ta_Check_Sum = 0;

                    /* 获得IP首部校验和 */
                    ack_ip->ta_Check_Sum = taCheckSum(ack_ip,(ack_ip->ipLen * 4));

                    /* 最后是ICMP报文 */
                    ack_icmp = (TA_IcmpPacket*)(ack_ether->data + (ack_ip->ipLen * 4));

                    /* 标志当前报文是Ping应答报文 */
                    ack_icmp->type = ICMP_PING_ACK;

                    ack_icmp->ta_Check_Sum = 0;

                    /* 获得ICMP校验和 */
                    ack_icmp->ta_Check_Sum = taCheckSum(ack_icmp,
                    (ntohs(ack_ip->length) - (ack_ip->ipLen * 4)));

                }

                /* 发送ICMP报文 */
                taNetPutpkt(0,sTaSendBuf,MAX((ntohs(ip->length)
                + TA_ETHER_HEAD_SIZE
                + TA_ETHER_TAIL_SIZE),
                TA_ETHER_FRAME_MIN_SIZE));
                continue;
            }

            /* 当前报文不是UDP报文则丢弃 */
            if (ip->protocol != IPPROTO_UDP)
            {
                continue;
            }

            /* 获得UDP报文 */
            {
                TA_UdpPacket *udp = (TA_UdpPacket*)
                (ether->data + (ip->ipLen * 4));
                INT32 udpDataLen;

                /* 检查UDP报文中的目的端口号是否与本地端口号匹配，
                 * 如果不匹配则丢弃当前报文
                 */
                if (ntohs(udp->des_port) != sTaUhLocalPort)
                {
                    continue;
                }

                /* 获得主机端MAC地址 */
                for (idx = MAC0; idx < TA_MAC_BYTE_SUM; idx++)
                {
                    saTaUbHostMac[idx] = ether->src_addr[idx];
                }

                /* 获得主机IP端地址 */
                sTagentHostIp = ntohl(ip->src_addr);

                /* 获得主机端端口号 */
                sTaUhHostPort = ntohs(udp->src_port);

                /* 获得UDP报文中的数据长度 */
                udpDataLen = ntohs(udp->length) - TA_UDP_HEAD_SIZE;
				
                /* 缓冲长度确认，取短者 */
                udpDataLen = MIN(udpDataLen,cnt);

                /* 拷贝数据到用户数据缓冲中 */
                memcpy(buf,udp->data,udpDataLen);
                buf[udpDataLen] = '\0';
		
                /* 返回UDP报文中的数据长度 */
                *dlen = (UINT32)udpDataLen;
                
                return (TA_OK);
            }
        }/* IP数据报处理结束 */

    }/* 结束循环收包 */

    return (TA_GET_PACKET_DATA_FAIL);
}

/**
 * @brief:
 *    设备写协议。
 * @param[in]: buf: 包
 * @param[in]: cnt: 允许的最大包大小
 * @param[in]: dlen: 实际写的包大小
 * @return:
 *    TA_OK: 写成功
 *    TA_FAIL: 写失败
 */
T_TA_ReturnCode taUdpWrite(UINT8* buf,UINT32 cnt,UINT32* dlen)
{
    INT32 putSize = 0;

    /* 装配以太网帧，获取目标机端发送数据包缓冲的地址 */
    TA_EtherFrame *ether = (TA_EtherFrame*)sTaSendBuf;
    UINT32 idx;
    UINT32 level = 0;

    if (0 == sTaUhHostPort)
    {
        return (TA_FAIL);
    }

    /* 填入MAC地址 */
    for (idx = MAC0; idx < TA_MAC_BYTE_SUM; idx++)
    {
        ether->des_addr[idx] = saTaUbHostMac[idx];
        ether->src_addr[idx] = saTaUbLocalMac[idx];
    }

    /* 填入帧类型为IP报文 */
    ether->type = htons(TA_ETFR_TYPE_IP);

    /* 装配IP数据报 */
    {
        TA_IpPacket* ip = (TA_IpPacket*)ether->data;

        /* IP数据报版本为IPV4 */
        ip->version = TA_IP_VERSION_4;

        /* IP数据报长度 */
        ip->ipLen = TA_IP_HEAD_SIZE;
        ip->tos = TA_TOS_NORMAL;
        ip->length = htons(cnt + TA_UDP_HEAD_SIZE + (ip->ipLen * 4));
        ip->id = htons(10);
        ip->ipFlag = 0;
        ip->ipOffset = 0;
        ip->ttl = 0xff;

        /* IP协议方式为UDP */
        ip->protocol = IPPROTO_UDP;
        ip->ta_Check_Sum = 0;
        ip->des_addr = htonl(sTagentHostIp);
        ip->src_addr = htonl(makeip(saTaUbLocalIP));

        /* 获取首部校验和 */
        ip->ta_Check_Sum = taCheckSum(ip,(ip->ipLen * 4));

        /* 装配UDP数据报 */
        TA_UdpPacket* udp = (TA_UdpPacket*)
        (ether->data + (TA_IP_HEAD_SIZE * 4));
        udp->src_port = htons(sTaUhLocalPort);
        udp->des_port = htons(sTaUhHostPort);
        udp->length = htons(cnt + TA_UDP_HEAD_SIZE);
        udp->ta_Check_Sum = 0;

        /* 拷贝要发送数据到UDP数据报 */
        memcpy(udp->data,buf,cnt);
        TP_DEBUG("udp data:%s, ori data:%s,  cnt:%d\n", udp->data+16+2, buf+16+2, cnt);
    }

    putSize = MAX((cnt + TA_UDP_HEAD_SIZE +
    (TA_IP_HEAD_SIZE * 4) +
    TA_ETHER_HEAD_SIZE +
    TA_ETHER_TAIL_SIZE),
    TA_ETHER_FRAME_MIN_SIZE);
    
    /* 发送以太网帧 */
    taNetPutpkt(0,sTaSendBuf,putSize);

    /* 返回实际发送数据包长度 */
    *dlen = (putSize);
    
    return (TA_OK);
}

/**
 * @brief:
 *    初始化TA协议需要初始值
 * @param[in]: netCfg: 网络配置表
 * @return:
 *    TA_OK: 初始化成功
 *    TA_FAIL: 初始化过程中失败
 *    TA_INVALID_PARAM: 无效参数
 *    TA_INVALID_NET_CFG: 参数配置错误
 */
T_TA_ReturnCode taUDPInit(T_TA_NET_CONFIG *netCfg)
{
    T_BOOL flag = FALSE;

    if (NULL == netCfg)
    {
        return (TA_INVALID_PARAM);
    }

    /* 获得缺省的网络配置参数 */
    if ((taIpAddrConvert(netCfg->ip,(UINT8 *)saTaUbLocalIP) == FALSE) || (netCfg->port == (INT16)0))
    {
        /* 非法的配置参数，网络驱动初始化失败 */
        return (TA_INVALID_NET_CFG);
    }

    /* 获得缺省的端口号 */
    sTaUhLocalPort = netCfg->port;

    /* 获取mac */
    taMacAddrConvert((UINT8 *)(netCfg->mac),(UINT8 *)saTaUbLocalMac);

    /* 保存ip数值信息，便于打印显示 */
    netCfg->ip = saTaUbLocalIP;
    
    /* 初始化网卡 */
    flag = taNetOpen(netCfg->minor, saTaUbLocalMac);
    if (flag == FALSE)
    {
        return (TA_FAIL);
    }
    
    return (TA_OK);
}

