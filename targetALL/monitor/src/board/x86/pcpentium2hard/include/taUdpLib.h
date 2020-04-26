/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taUdpLib.h
 * @brief:
 *             <li>�ṩUDPЭ����صĺ궨������Ͷ���</li>
 */
#ifndef TA_UDP_LIB_H
#define TA_UDP_LIB_H

/************************ͷ�ļ�********************************/
#include "ta.h"
#include "config_ta.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************�궨��********************************/

/***************��̫��������ݶ���*****************************/

/* ��̫�����֡�ߴ� */
#define TA_ETHER_FRAME_MAX_SIZE (1520)

/* ��̫����С֡�ߴ� */
#define TA_ETHER_FRAME_MIN_SIZE (64)

/* ��̫��ͷ���ߴ� */
#define TA_ETHER_HEAD_SIZE (14)

/* ��̫��β���ߴ� */
#define TA_ETHER_TAIL_SIZE (4)

/* ARP�����ܳ��� */
#define TA_ARP_PACKET_SIZE (46)

/* ��̫��ARP֡�ܳ��ȡ���̫���ײ�14�ֽڣ�ARP���ı���46�ֽڣ���̫У��4�ֽڣ��ϼ�64�ֽ� */
#define TA_ETHER_ARP_FRAME_SIZE (TA_ETHER_HEAD_SIZE + TA_ARP_PACKET_SIZE + TA_ETHER_TAIL_SIZE)

/* UDPͷ���ߴ� */
#define TA_UDP_HEAD_SIZE   (8)

/* ��̫�����ݰ����� */
/* IP���ݱ� */
#define TA_ETFR_TYPE_IP  (0x0800) 

/* ARP���ݱ� */
#define TA_ETFR_TYPE_ARP (0x0806)

/* IP Э����غ궨�� */
/* �汾����, IPV4 */
#define TA_IP_VERSION_4 (4) 

/* �ײ����� , 5�ֽ�*/
#define TA_IP_HEAD_SIZE (5) 

/************************���ú꺯��***********************************/
#define makeip(ip_array) \
    (((UINT32)(ip_array)[IP0] << 24) | \
     ((UINT32)(ip_array)[IP1] << 16) | \
     ((UINT32)(ip_array)[IP2] << 8 ) | \
     ((UINT32)(ip_array)[IP3]))

/* ����С�˻��� */
#ifdef __LITTLE_ENDIAN__ 
#define lswap(x)    ((((unsigned long)(x) & 0xff000000) >> 24) |(((unsigned long)(x) & 0x00ff0000) >>  8) | (((unsigned long)(x) & 0x0000ff00)<<8)|(((unsigned long)(x) & 0x000000ff) << 24))
#define htonl(l)    ((unsigned long)lswap(l))
#define ntohl(l)    ((unsigned long)lswap(l))
#define htons(s)    ((((UINT16)(s) >> 8) & 0xFF) | (((UINT16)(s) << 8) & 0xFF00))
#define ntohs(s)    htons(s)
#else /* ��˻��� */
#define lswap(x)    ((unsigned long)(x))
#define htonl(l)    ((unsigned long)lswap(l))
#define ntohl(l)    ((unsigned long)lswap(l))
#define htons(s)    ((UINT16)(s) )
#define ntohs(s)    htons(s)
#endif

/************************���Ͷ���******************************/
/* IPЭ�� ����*/
typedef enum
{
    IPPROTO_IP = 0,
    IPPROTO_ICMP = 1,
    IPPROTO_IGMP = 2,
    IPPROTO_TCP = 6,
    IPPROTO_UDP = 17
}TIP_Protocol;


/*ARP Э���������*/
enum TARP_TYPE
{
    ARP_AREQ = 0x1,  /* ARP���� */
    ARP_AACK = 0x2,  /* ARPӦ�� */
    ARP_RREQ = 0x3,  /* RARP���� */
    ARP_RACK = 0x4   /* RARPӦ�� */
};


/*ICMPЭ���������*/
enum TICMP_TYPE
{
    ICMP_PING_ACK = 0x0, /* PingӦ�� */
    ICMP_PING_REQ = 0x8  /* Ping���� */
};

/* �������� */
#define TA_TOS_NORMAL (0) /* һ����� */

/* MAC��ַ�������� */
enum TAGENT_MAC_ADDR
{
    MAC0 = 0,
    MAC1,
    MAC2,
    MAC3,
    MAC4,
    MAC5,
    TA_MAC_BYTE_SUM
};

/* IP��ַ�������� */
enum TAGENT_IP_ADDR
{
    IP0 = 0,
    IP1,
    IP2,
    IP3,
    TA_IP_BYTE_SUM
};

/* ��̫��֡ͷ��ʽ���� */
typedef struct
{
    /* Ŀ�ĵ�ַ */
    UINT8 des_addr[TA_MAC_BYTE_SUM];

    /* Դ��ַ */
    UINT8 src_addr[TA_MAC_BYTE_SUM];

    UINT16 type;

    /* ���� */
    UINT8 data[1];

} TA_EtherFrame;

/* IP���ݱ�ͷ��ʽ���� */
typedef struct
{
    /* С�˻��� */
#ifdef __LITTLE_ENDIAN__
    UINT8     ipLen:4;
    
    UINT8    version:4;
#else /* ��˻��� */
    UINT8    version:4;

    UINT8     ipLen:4;
#endif

    UINT8    tos;

    /* ���ݱ����� */
    UINT16   length;

    /* ��ʶ */
    UINT16    id;

    /* ��־ */
    UINT16    ipFlag:3;

    /* ƫ�� */
    UINT16    ipOffset:13;

    /* ����ʱ�� */
    UINT8    ttl;

    UINT8    protocol;

    /* �ײ�У��� */
    UINT16    ta_Check_Sum;

    /* Դ��ַ */
    UINT32    src_addr;

    /* Ŀ�ĵ�ַ */
    UINT32    des_addr;

} TA_IpPacket;

/* ARP���ݱ���ʽ����*/
typedef struct
{
    /* Ӳ������ */
#define TA_ARP_HD_TYPE_ETHER (0x0001) 
    UINT16 hd_type;

    /* Э������ */
#define TA_ARP_PR_TYPE_IP (0x0800) 
    UINT16 pr_type;

    /* Ӳ����ַ���� */
    UINT8  hd_size;

    /* Э���ַ���� */
    UINT8  pr_size;

    /* ���� */
    UINT16 op;

    /* ���Ͷ���̫����ַ */
    UINT8  src_mac[TA_MAC_BYTE_SUM];

    /* ���Ͷ�IP��ַ */
    UINT8  src_ip[TA_IP_BYTE_SUM];

    /* Ŀ����̫����ַ */
    UINT8  des_mac[TA_MAC_BYTE_SUM];

    /* Ŀ��IP��ַ */
    UINT8  des_ip[TA_IP_BYTE_SUM];

} TA_ArpPacket;

/* UDP���ݱ���ʽ���� */
typedef struct
{
    /* 16λԴ�˿ں� */
    UINT16 src_port;

    /* 16λĿ�Ķ˿ں� */
    UINT16 des_port;

    /* 16λUDP���� */
    UINT16 length;

    /* 16λUDPУ��� */
    UINT16 ta_Check_Sum;

    /* ���� */
    UINT8 data[1];

} TA_UdpPacket;

/* ICMP���ݱ���ʽ����*/
typedef struct
{
    /* ���� */
    UINT8 type;

    /* ���� */
    UINT8 code;

    /* У��� */
    UINT16 ta_Check_Sum;

    /* ��ʶ�� */
    UINT16 id;

    /* ���к� */
    UINT16 serial;

} TA_IcmpPacket;

/************************�ӿ�����******************************/

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
T_TA_ReturnCode taUDPInit(T_TA_NET_CONFIG *netCfg);

/**
 * @brief:
 *    �豸��Э�顣
 * @param[in]: buf: ��
 * @param[in]: cnt: �����������С
 * @param[in]: dlen: ʵ�ʽ��յİ���С
 * @return:
 *    TA_OK: ��ȡ�ɹ���
 *    TA_GET_PACKET_DATA_FAIL: ��ȡʧ�ܡ�
 */
T_TA_ReturnCode taUdpRead(UINT8* buf,UINT32 cnt,UINT32* dlen);

/**
 * @brief:
 *    �豸дЭ�顣
 * @param[in]: buf: ��
 * @param[in]: cnt: �����������С
 * @param[in]: dlen: ʵ��д�İ���С
 * @return:
 *    TA_OK: д�ɹ���
 *    TA_FAIL: дʧ�ܡ�
 */
T_TA_ReturnCode taUdpWrite(UINT8* buf,UINT32 cnt,UINT32* dlen);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* TA_UDP_LIB_H */
