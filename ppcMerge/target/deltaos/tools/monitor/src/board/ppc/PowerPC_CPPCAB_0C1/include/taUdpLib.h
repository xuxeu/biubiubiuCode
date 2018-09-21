/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taUdpLib.h
 * @brief:
 *             <li>提供UDP协议相关的宏定义和类型定义</li>
 */
#ifndef TA_UDP_LIB_H
#define TA_UDP_LIB_H

/************************头文件********************************/
#include "ta.h"
#include "config_ta.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************宏定义********************************/

/***************以太网相关数据定义*****************************/

/* 以太网最大帧尺寸 */
#define TA_ETHER_FRAME_MAX_SIZE (1520)

/* 以太网最小帧尺寸 */
#define TA_ETHER_FRAME_MIN_SIZE (64)

/* 以太网头部尺寸 */
#define TA_ETHER_HEAD_SIZE (14)

/* 以太网尾部尺寸 */
#define TA_ETHER_TAIL_SIZE (4)

/* ARP报文总长度 */
#define TA_ARP_PACKET_SIZE (46)

/* 以太网ARP帧总长度。以太网首部14字节，ARP报文本身46字节，以太校验4字节，合计64字节 */
#define TA_ETHER_ARP_FRAME_SIZE (TA_ETHER_HEAD_SIZE + TA_ARP_PACKET_SIZE + TA_ETHER_TAIL_SIZE)

/* UDP头部尺寸 */
#define TA_UDP_HEAD_SIZE   (8)

/* 以太网数据包类型 */
/* IP数据报 */
#define TA_ETFR_TYPE_IP  (0x0800) 

/* ARP数据报 */
#define TA_ETFR_TYPE_ARP (0x0806)

/* IP 协议相关宏定义 */
/* 版本定义, IPV4 */
#define TA_IP_VERSION_4 (4) 

/* 首部长度 , 5字节*/
#define TA_IP_HEAD_SIZE (5) 

/************************常用宏函数***********************************/
#define makeip(ip_array) \
    (((UINT32)(ip_array)[IP0] << 24) | \
     ((UINT32)(ip_array)[IP1] << 16) | \
     ((UINT32)(ip_array)[IP2] << 8 ) | \
     ((UINT32)(ip_array)[IP3]))

/* 定义小端环境 */
#ifdef __LITTLE_ENDIAN__ 
#define lswap(x)    ((((unsigned long)(x) & 0xff000000) >> 24) |(((unsigned long)(x) & 0x00ff0000) >>  8) | (((unsigned long)(x) & 0x0000ff00)<<8)|(((unsigned long)(x) & 0x000000ff) << 24))
#define htonl(l)    ((unsigned long)lswap(l))
#define ntohl(l)    ((unsigned long)lswap(l))
#define htons(s)    ((((UINT16)(s) >> 8) & 0xFF) | (((UINT16)(s) << 8) & 0xFF00))
#define ntohs(s)    htons(s)
#else /* 大端环境 */
#define lswap(x)    ((unsigned long)(x))
#define htonl(l)    ((unsigned long)lswap(l))
#define ntohl(l)    ((unsigned long)lswap(l))
#define htons(s)    ((UINT16)(s) )
#define ntohs(s)    htons(s)
#endif

/************************类型定义******************************/
/* IP协议 类型*/
typedef enum
{
    IPPROTO_IP = 0,
    IPPROTO_ICMP = 1,
    IPPROTO_IGMP = 2,
    IPPROTO_TCP = 6,
    IPPROTO_UDP = 17
}TIP_Protocol;


/*ARP 协议操作类型*/
enum TARP_TYPE
{
    ARP_AREQ = 0x1,  /* ARP请求 */
    ARP_AACK = 0x2,  /* ARP应答 */
    ARP_RREQ = 0x3,  /* RARP请求 */
    ARP_RACK = 0x4   /* RARP应答 */
};


/*ICMP协议操作类型*/
enum TICMP_TYPE
{
    ICMP_PING_ACK = 0x0, /* Ping应答 */
    ICMP_PING_REQ = 0x8  /* Ping请求 */
};

/* 服务类型 */
#define TA_TOS_NORMAL (0) /* 一般服务 */

/* MAC地址常量定义 */
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

/* IP地址常量定义 */
enum TAGENT_IP_ADDR
{
    IP0 = 0,
    IP1,
    IP2,
    IP3,
    TA_IP_BYTE_SUM
};

/* 以太网帧头格式定义 */
typedef struct
{
    /* 目的地址 */
    UINT8 des_addr[TA_MAC_BYTE_SUM];

    /* 源地址 */
    UINT8 src_addr[TA_MAC_BYTE_SUM];

    UINT16 type;

    /* 数据 */
    UINT8 data[1];

} TA_EtherFrame;

/* IP数据报头格式定义 */
typedef struct
{
    /* 小端环境 */
#ifdef __LITTLE_ENDIAN__
    UINT8     ipLen:4;
    
    UINT8    version:4;
#else /* 大端环境 */
    UINT8    version:4;

    UINT8     ipLen:4;
#endif

    UINT8    tos;

    /* 数据报长度 */
    UINT16   length;

    /* 标识 */
    UINT16    id;

    /* 标志 */
    UINT16    ipFlag:3;

    /* 偏移 */
    UINT16    ipOffset:13;

    /* 生存时间 */
    UINT8    ttl;

    UINT8    protocol;

    /* 首部校验和 */
    UINT16    ta_Check_Sum;

    /* 源地址 */
    UINT32    src_addr;

    /* 目的地址 */
    UINT32    des_addr;

} TA_IpPacket;

/* ARP数据报格式定义*/
typedef struct
{
    /* 硬件类型 */
#define TA_ARP_HD_TYPE_ETHER (0x0001) 
    UINT16 hd_type;

    /* 协议类型 */
#define TA_ARP_PR_TYPE_IP (0x0800) 
    UINT16 pr_type;

    /* 硬件地址长度 */
    UINT8  hd_size;

    /* 协议地址长度 */
    UINT8  pr_size;

    /* 操作 */
    UINT16 op;

    /* 发送端以太网地址 */
    UINT8  src_mac[TA_MAC_BYTE_SUM];

    /* 发送端IP地址 */
    UINT8  src_ip[TA_IP_BYTE_SUM];

    /* 目的以太网地址 */
    UINT8  des_mac[TA_MAC_BYTE_SUM];

    /* 目的IP地址 */
    UINT8  des_ip[TA_IP_BYTE_SUM];

} TA_ArpPacket;

/* UDP数据报格式定义 */
typedef struct
{
    /* 16位源端口号 */
    UINT16 src_port;

    /* 16位目的端口号 */
    UINT16 des_port;

    /* 16位UDP长度 */
    UINT16 length;

    /* 16位UDP校验和 */
    UINT16 ta_Check_Sum;

    /* 数据 */
    UINT8 data[1];

} TA_UdpPacket;

/* ICMP数据报格式定义*/
typedef struct
{
    /* 类型 */
    UINT8 type;

    /* 代码 */
    UINT8 code;

    /* 校验和 */
    UINT16 ta_Check_Sum;

    /* 标识符 */
    UINT16 id;

    /* 序列号 */
    UINT16 serial;

} TA_IcmpPacket;

/************************接口声明******************************/

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
T_TA_ReturnCode taUDPInit(T_TA_NET_CONFIG *netCfg);

/**
 * @brief:
 *    设备读协议。
 * @param[in]: buf: 包
 * @param[in]: cnt: 允许的最大包大小
 * @param[in]: dlen: 实际接收的包大小
 * @return:
 *    TA_OK: 读取成功。
 *    TA_GET_PACKET_DATA_FAIL: 读取失败。
 */
T_TA_ReturnCode taUdpRead(UINT8* buf,UINT32 cnt,UINT32* dlen);

/**
 * @brief:
 *    设备写协议。
 * @param[in]: buf: 包
 * @param[in]: cnt: 允许的最大包大小
 * @param[in]: dlen: 实际写的包大小
 * @return:
 *    TA_OK: 写成功。
 *    TA_FAIL: 写失败。
 */
T_TA_ReturnCode taUdpWrite(UINT8* buf,UINT32 cnt,UINT32* dlen);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* TA_UDP_LIB_H */
