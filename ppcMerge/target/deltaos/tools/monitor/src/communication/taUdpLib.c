/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                         �������ļ���
 */

/*
 * @file: taUdpLib.c
 * @brief:
 *             <li>udpЭ���</li>
 */
 
/************************ͷ �� ��******************************/
#include "ta.h"
#include "taUdpLib.h"

/************************�� �� ��********************************/

#define TP_DEBUG(...)

/************************���Ͷ���*******************************/

/************************�ⲿ����*******************************/

/************************ǰ������*******************************/

/************************ģ�����*******************************/

/* Ŀ���������MAC��ַ */
T_MODULE UINT8 saTaUbLocalMac[TA_MAC_BYTE_SUM]={0};

/* ����������MAC��ַ */
T_MODULE UINT8 saTaUbHostMac[TA_MAC_BYTE_SUM]={0};

/* Ŀ���������IP��ַ */
T_MODULE UINT8 saTaUbLocalIP[TA_IP_BYTE_SUM]={0};

/* ����������IP��ַ */
T_MODULE UINT32  sTagentHostIp=0;

/* Ŀ���������˿ں� */
T_MODULE UINT16 sTaUhLocalPort=0;

/* ����������˿ں� */
T_MODULE UINT16 sTaUhHostPort=0;

/* Ŀ����˽������ݰ����� */
T_MODULE UINT32 TaRcvBuf[(TA_ETHER_FRAME_MAX_SIZE + 16) / 4];

/* Ŀ����˷������ݰ����� */
T_MODULE UINT32 TaSendBuf[(TA_ETHER_FRAME_MAX_SIZE + 16) / 4];

T_MODULE UINT8 *sTaRcvBuf = ((UINT8 *)TaRcvBuf);
T_MODULE UINT8 *sTaSendBuf = ((UINT8 *)TaSendBuf + 2);

/************************ȫ�ֱ���*******************************/

/************************����ʵ��*******************************/

/*
 * @brief:
 *    ���ַ�����ʽIP��ַת�����ֽ������ʽ
 * @param[in]: string: ָ���ַ�����ʽIP��ָ��
 * @param[out]: array: ָ�������ʽIP��ָ��
 * @return:
 *    TRUE: ת���ɹ�
 *    FALSE: ת����IP��ַ�ĵ�һ���ֽ�Ϊ�㣬
 *          �ַ�����ʽIP��ַû����"."���ָ�
 */
T_MODULE BOOL taIpAddrConvert(UINT8 *string, UINT8 *array)
{
    UINT32 idx;
    UINT8 *ptr = string;
    UINT32 ipNum;

    /* ���Ƚ���IP�ĵ�һ���ֽ� */
    ipNum = strtoi(ptr,10);
    if ((0 == ipNum) || (ipNum > 255))
    {
        /* IP��ַ�ĵ�һ���ֽڲ���Ϊ������255 */
        TP_DEBUG("TA_ERROR: Invalid IP: %s\n",string);
        return FALSE;
    }

    array[IP0] = (UINT8)ipNum;

    /* ����������IP��ַ */
    for (idx = IP1; idx < TA_IP_BYTE_SUM; idx++)
    {
        ptr = strchr(ptr,'.');
        if (ptr == NULL)
        {
            /* IP��ַû����"."�ַ����ָ� */
            TP_DEBUG("TA_ERROR: Invalid IP: %s\n",string);
            return FALSE;
        }

        /* ����"."�ַ� */
        ptr++;

        /* ת���������ֽ� */
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
        /* IP��ַ��ʽ����ȷ */
        TP_DEBUG("TA_ERROR: Invalid IP: %s\n",string);
        return FALSE;
    }

    return TRUE;
}

/*
 * @brief:
 *    ���ַ�����ʽ��MAC��ַת�����ֽ������ʽ
 * @param[in]: string: ָ���ַ�����ʽMAC��ַ��ָ��
 * @param[out]: array: ָ���ֽ������ʽMAC��ַ��ָ��
 * @return:
 *    TRUE: ת���ɹ���
 *    FALSE: �ַ�����ʽMAC��ַû����"-"���ָ�
 */
T_MODULE BOOL taMacAddrConvert(UINT8 *string, UINT8 *array)
{
    UINT32 idx;
    UINT8 *ptr = string;
    UINT32 macNum;

    if (string[17] != (UINT8)'\0')
    {
        /* MAC��ַ��ʽ����ȷ */
        TP_DEBUG("TA_ERROR: Invalid MAC\n");
        return FALSE;
    }

    /* ���Ƚ���MAC�ĵ�һ���ֽ� */
    macNum = strtoi(ptr,16);
    if (macNum > 0xff)
    {
        TP_DEBUG("TA_ERROR: Invalid MAC: %s\n",string);
        return FALSE;
    }
    
    array[MAC0] = (UINT8)macNum;

    /* ����������MAC��ַ */
    for (idx = MAC1; idx < TA_MAC_BYTE_SUM; idx++)
    {
        ptr = strchr(ptr,'-');
        if (ptr == NULL)
        {
            /* MAC��ַû����"-"���ָ� */
            TP_DEBUG("TA_ERROR: Invalid MAC: %s\n",string);
            return FALSE;
        }

        /* ����"-"�ַ� */
        ptr++;

        /* ��ú������ֽ� */
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
 *    UDP�豸��Э��
 * @param[in]: buf: ��
 * @param[in]: cnt: �����������С
 * @param[in]: dlen: ʵ�ʽ��յİ���С
 * @return:
 *    TA_OK: ��ȡ�ɹ�
 *    TA_GET_PACKET_DATA_FAIL: ��ȡʧ��
 */
T_TA_ReturnCode taUdpRead(UINT8* buf,UINT32 cnt,UINT32* dlen)
{
    INT32 loopCount = 5000;
    INT32 idx = 0;

    /* ��ʵ���մ�С��0 */
    *dlen = 0;

    /* ����̫���豸��ȡ�µ���̫����֡ */
    while (loopCount --> (INT32)0)
    {
        TA_EtherFrame *ether = NULL;

        /* Ϊ�˱�֤��ARM�����µ��ֽڶ��룬���ｫrawUdpOffset����Ϊ2 */
        INT32 rawUdpOffset = 2;
        {
            /* ����������ӿڽ������ݰ� */
            INT32 len = taNetGetpkt(0,sTaRcvBuf,&rawUdpOffset);

            if (0 == len)
            {
                /* �հ�ʧ��,�ظ���ȡ */
                continue;
            }

        }

        /* �����̫֡ͷ��ַ */
        ether = (TA_EtherFrame *)(sTaRcvBuf+rawUdpOffset);

        /* �����ֽ�˳��ת�� */
        ether->type = ntohs(ether->type);
		
        /* ��ǰ֡��һ��ARP���ݱ��� */
        /* ARP��������֡�а���Ŀ��������IP��ַ��
         * ����˼�� "����������IP��ַ��ӵ���ߣ���ش����Ӳ����ַ"
         */
        if (ether->type == TA_ETFR_TYPE_ARP)
        {
            /* ���ARP���� */
            TA_ArpPacket *arp = (TA_ArpPacket*)ether->data;

            /* �����ֽ�˳��ת�� */
            arp->op = ntohs(arp->op);

            /* ���ARP�����Ƿ���ARP�����������������������һ�����ݰ� */
            if (arp->op != ARP_AREQ)
            {
                continue;
            }

            /* ���ARP�����е�Ŀ������IP��ַ�Ƿ��뱾��IP��ַƥ�䣬���ƥ��
             * ��������ͱ�����Ӳ����ַ������ֱ�ӽ�����һ������
             */
            if ((arp->des_ip[IP0] != saTaUbLocalIP[IP0]) ||
            (arp->des_ip[IP1] != saTaUbLocalIP[IP1]) ||
            (arp->des_ip[IP2] != saTaUbLocalIP[IP2]) ||
            (arp->des_ip[IP3] != saTaUbLocalIP[IP3]))
            {
                continue;
            }

            /* ��װARPӦ���ģ���������̫֡ͷ */
            {
                TA_ArpPacket *ack_arp = NULL;

                /* ��ȡĿ����˷������ݰ�����ĵ�ַ */
                ether = (TA_EtherFrame*)sTaSendBuf;

                /* �ֽ�˳��ת�� */
                ether->type = htons(TA_ETFR_TYPE_ARP);

                /* ARP������д */
                ack_arp = (TA_ArpPacket*)ether->data;
                ack_arp->hd_type = htons(TA_ARP_HD_TYPE_ETHER);
                ack_arp->pr_type = htons(TA_ARP_PR_TYPE_IP);
                ack_arp->hd_size = TA_MAC_BYTE_SUM;
                ack_arp->pr_size = TA_IP_BYTE_SUM;
                ack_arp->op = htons(ARP_AACK);

                /* MAC��ַ��Դͷ��Ŀ�Ļ��� */
                for (idx = MAC0; idx < TA_MAC_BYTE_SUM; idx++)
                {
                    ether->des_addr[idx] = arp->src_mac[idx];
                    ether->src_addr[idx] = saTaUbLocalMac[idx];

                    ack_arp->src_mac[idx] = saTaUbLocalMac[idx];
                    ack_arp->des_mac[idx] = arp->src_mac[idx];
                }

                /* IP��ַ��Դͷ��Ŀ�Ļ��� */
                for (idx = IP0; idx < TA_IP_BYTE_SUM; idx++)
                {
                    ack_arp->src_ip[idx] = arp->des_ip[idx];
                    ack_arp->des_ip[idx] = arp->src_ip[idx];
                }

            }

            /* ����ARP���� */
            taNetPutpkt(0,sTaSendBuf,TA_ETHER_ARP_FRAME_SIZE);
            continue;
        }/* ARP���ݱ�������� */

        /* ��ǰ֡��һ��IP���ݱ��� */
        if (ether->type == TA_ETFR_TYPE_IP)
        {
            /* ���IP��ͷ */
            TA_IpPacket* ip = (TA_IpPacket*)ether->data;

            /* ���IP���ĵ�Ŀ�ĵ�ַ�뱾�ص�ַ��ƥ�������ñ��� */
            if (ntohl(ip->des_addr) != makeip(saTaUbLocalIP))
            {
                TP_DEBUG("des ip error, 0x%x, packet:0x%x, local:0x%x\n", ip->des_addr, ntohl(ip->des_addr), makeip(saTaUbLocalIP));
                continue;
            }

            /* ��ǰ������һ��ICMP���� */
            if (ip->protocol == IPPROTO_ICMP)
            {
                /* ���ICMP��ͷ */
                TA_IcmpPacket *icmp = (TA_IcmpPacket*)(ether->data + (ip->ipLen * 4));

                /* ����������Ͳ���һ��Pinq���������ñ��� */
                if (icmp->type != ICMP_PING_REQ)
                {
                    continue;
                }

                /* ��װPingӦ���ģ���������̫֡ͷ */
                {
                    TA_EtherFrame *ack_ether = NULL;
                    TA_IpPacket *ack_ip = NULL;
                    TA_IcmpPacket *ack_icmp = NULL;
                    idx = 0;

                    ack_ether = (TA_EtherFrame*)sTaSendBuf;
                    ack_ether->type = htons(TA_ETFR_TYPE_IP);

                    /* ����Դͷ��Ŀ��MAC��ַ */
                    for (idx = MAC0; idx < TA_MAC_BYTE_SUM; idx++)
                    {
                        ack_ether->des_addr[idx] = ether->src_addr[idx];
                        ack_ether->src_addr[idx] = ether->des_addr[idx];
                    }

                    /* Ȼ����IP���� */
                    ack_ip = (TA_IpPacket*)ack_ether->data;
                    memcpy(ack_ip,ip,ntohs(ip->length));

                    /* ����IP��ַ */
                    ack_ip->src_addr = ip->des_addr;
                    ack_ip->des_addr = ip->src_addr;

                    ack_ip->ta_Check_Sum = 0;

                    /* ���IP�ײ�У��� */
                    ack_ip->ta_Check_Sum = taCheckSum(ack_ip,(ack_ip->ipLen * 4));

                    /* �����ICMP���� */
                    ack_icmp = (TA_IcmpPacket*)(ack_ether->data + (ack_ip->ipLen * 4));

                    /* ��־��ǰ������PingӦ���� */
                    ack_icmp->type = ICMP_PING_ACK;

                    ack_icmp->ta_Check_Sum = 0;

                    /* ���ICMPУ��� */
                    ack_icmp->ta_Check_Sum = taCheckSum(ack_icmp,
                    (ntohs(ack_ip->length) - (ack_ip->ipLen * 4)));

                }

                /* ����ICMP���� */
                taNetPutpkt(0,sTaSendBuf,MAX((ntohs(ip->length)
                + TA_ETHER_HEAD_SIZE
                + TA_ETHER_TAIL_SIZE),
                TA_ETHER_FRAME_MIN_SIZE));
                continue;
            }

            /* ��ǰ���Ĳ���UDP�������� */
            if (ip->protocol != IPPROTO_UDP)
            {
                continue;
            }

            /* ���UDP���� */
            {
                TA_UdpPacket *udp = (TA_UdpPacket*)
                (ether->data + (ip->ipLen * 4));
                INT32 udpDataLen;

                /* ���UDP�����е�Ŀ�Ķ˿ں��Ƿ��뱾�ض˿ں�ƥ�䣬
                 * �����ƥ��������ǰ����
                 */
                if (ntohs(udp->des_port) != sTaUhLocalPort)
                {
                    continue;
                }

                /* ���������MAC��ַ */
                for (idx = MAC0; idx < TA_MAC_BYTE_SUM; idx++)
                {
                    saTaUbHostMac[idx] = ether->src_addr[idx];
                }

                /* �������IP�˵�ַ */
                sTagentHostIp = ntohl(ip->src_addr);

                /* ��������˶˿ں� */
                sTaUhHostPort = ntohs(udp->src_port);

                /* ���UDP�����е����ݳ��� */
                udpDataLen = ntohs(udp->length) - TA_UDP_HEAD_SIZE;
				
                /* ���峤��ȷ�ϣ�ȡ���� */
                udpDataLen = MIN(udpDataLen,cnt);

                /* �������ݵ��û����ݻ����� */
                memcpy(buf,udp->data,udpDataLen);
                buf[udpDataLen] = '\0';
		
                /* ����UDP�����е����ݳ��� */
                *dlen = (UINT32)udpDataLen;
                
                return (TA_OK);
            }
        }/* IP���ݱ�������� */

    }/* ����ѭ���հ� */

    return (TA_GET_PACKET_DATA_FAIL);
}

/**
 * @brief:
 *    �豸дЭ�顣
 * @param[in]: buf: ��
 * @param[in]: cnt: �����������С
 * @param[in]: dlen: ʵ��д�İ���С
 * @return:
 *    TA_OK: д�ɹ�
 *    TA_FAIL: дʧ��
 */
T_TA_ReturnCode taUdpWrite(UINT8* buf,UINT32 cnt,UINT32* dlen)
{
    INT32 putSize = 0;

    /* װ����̫��֡����ȡĿ����˷������ݰ�����ĵ�ַ */
    TA_EtherFrame *ether = (TA_EtherFrame*)sTaSendBuf;
    UINT32 idx;
    UINT32 level = 0;

    if (0 == sTaUhHostPort)
    {
        return (TA_FAIL);
    }

    /* ����MAC��ַ */
    for (idx = MAC0; idx < TA_MAC_BYTE_SUM; idx++)
    {
        ether->des_addr[idx] = saTaUbHostMac[idx];
        ether->src_addr[idx] = saTaUbLocalMac[idx];
    }

    /* ����֡����ΪIP���� */
    ether->type = htons(TA_ETFR_TYPE_IP);

    /* װ��IP���ݱ� */
    {
        TA_IpPacket* ip = (TA_IpPacket*)ether->data;

        /* IP���ݱ��汾ΪIPV4 */
        ip->version = TA_IP_VERSION_4;

        /* IP���ݱ����� */
        ip->ipLen = TA_IP_HEAD_SIZE;
        ip->tos = TA_TOS_NORMAL;
        ip->length = htons(cnt + TA_UDP_HEAD_SIZE + (ip->ipLen * 4));
        ip->id = htons(10);
        ip->ipFlag = 0;
        ip->ipOffset = 0;
        ip->ttl = 0xff;

        /* IPЭ�鷽ʽΪUDP */
        ip->protocol = IPPROTO_UDP;
        ip->ta_Check_Sum = 0;
        ip->des_addr = htonl(sTagentHostIp);
        ip->src_addr = htonl(makeip(saTaUbLocalIP));

        /* ��ȡ�ײ�У��� */
        ip->ta_Check_Sum = taCheckSum(ip,(ip->ipLen * 4));

        /* װ��UDP���ݱ� */
        TA_UdpPacket* udp = (TA_UdpPacket*)
        (ether->data + (TA_IP_HEAD_SIZE * 4));
        udp->src_port = htons(sTaUhLocalPort);
        udp->des_port = htons(sTaUhHostPort);
        udp->length = htons(cnt + TA_UDP_HEAD_SIZE);
        udp->ta_Check_Sum = 0;

        /* ����Ҫ�������ݵ�UDP���ݱ� */
        memcpy(udp->data,buf,cnt);
        TP_DEBUG("udp data:%s, ori data:%s,  cnt:%d\n", udp->data+16+2, buf+16+2, cnt);
    }

    putSize = MAX((cnt + TA_UDP_HEAD_SIZE +
    (TA_IP_HEAD_SIZE * 4) +
    TA_ETHER_HEAD_SIZE +
    TA_ETHER_TAIL_SIZE),
    TA_ETHER_FRAME_MIN_SIZE);
    
    /* ������̫��֡ */
    taNetPutpkt(0,sTaSendBuf,putSize);

    /* ����ʵ�ʷ������ݰ����� */
    *dlen = (putSize);
    
    return (TA_OK);
}

/**
 * @brief:
 *    ��ʼ��TAЭ����Ҫ��ʼֵ
 * @param[in]: netCfg: �������ñ�
 * @return:
 *    TA_OK: ��ʼ���ɹ�
 *    TA_FAIL: ��ʼ��������ʧ��
 *    TA_INVALID_PARAM: ��Ч����
 *    TA_INVALID_NET_CFG: �������ô���
 */
T_TA_ReturnCode taUDPInit(T_TA_NET_CONFIG *netCfg)
{
    T_BOOL flag = FALSE;

    if (NULL == netCfg)
    {
        return (TA_INVALID_PARAM);
    }

    /* ���ȱʡ���������ò��� */
    if ((taIpAddrConvert(netCfg->ip,(UINT8 *)saTaUbLocalIP) == FALSE) || (netCfg->port == (INT16)0))
    {
        /* �Ƿ������ò���������������ʼ��ʧ�� */
        return (TA_INVALID_NET_CFG);
    }

    /* ���ȱʡ�Ķ˿ں� */
    sTaUhLocalPort = netCfg->port;

    /* ��ȡmac */
    taMacAddrConvert((UINT8 *)(netCfg->mac),(UINT8 *)saTaUbLocalMac);

    /* ����ip��ֵ��Ϣ�����ڴ�ӡ��ʾ */
    netCfg->ip = saTaUbLocalIP;
    
    /* ��ʼ������ */
    flag = taNetOpen(netCfg->minor, saTaUbLocalMac);
    if (flag == FALSE)
    {
        return (TA_FAIL);
    }
    
    return (TA_OK);
}

