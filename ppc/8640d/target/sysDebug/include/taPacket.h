/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-28        彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:taPacket.h
 * @brief:
 *             <li>数据包常用的操作</li>
 */
#ifndef _TAPACKET_H
#define _TAPACKET_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************头 文 件******************************/
#include "taTypes.h"

/************************宏定义********************************/

/* 消息协议头大小 */
#define TA_PROTOCOL_HEAD_SIZE 16

/* 消息有效缓冲大小 */
#define TA_PKT_DATA_SIZE 1024

/* 校验和大小 */
#define TA_PROTOCOL_CHECKSUM_SIZE 2

/* 消息缓冲大小 */
#define TA_PACKET_DATA_LEN (TA_PKT_DATA_SIZE+TA_PROTOCOL_HEAD_SIZE+TA_PROTOCOL_CHECKSUM_SIZE)

/************************类型定义******************************/

/* TA通信数据包的包头 */
typedef struct
{
    UINT16 senderId;            /* 发送端ID,与srcAid对应 */
    UINT16 receiverId;          /* 接收端ID，与desAid对应 */
    UINT16 channelOfSender;     /* 发送端通道号，该通道号用于发送端在接收数据时，用于确定将接收的数据放到哪个通道中，与srcSaid对应 */
    UINT16 channelOfReceiver;   /* 接收端通道号，该通道号用于确定，接收端在接收到数据以后，将数据放到哪个通道中，与desSaid对应 */
} T_TA_PacketHead;

/* 数据包结构 */
typedef struct
{
    struct T_TA_Packet *next;
    T_TA_PacketHead header;
    UINT32 size;       /* 包的有效数据所占空间，比如数据部分总共有1000个字节，但是只有size个字节才是有效的 */
    UINT32 offset;     /* 有效数据所在偏移位置，即从数据部分基地址开始，要偏移offset个字节之后才能得到有效数据 */
    UINT32 seqNum;     /* 包序号，协议内部使用*/
    UINT8 data[TA_PACKET_DATA_LEN];
}T_TA_Packet;

/************************接口声明******************************/

/**
 * @brief:
 *      将数据包头清零
 * @param[in]:header:数据包头
 * @return:
 *     无
 */
static inline void taZeroPktHead(T_TA_PacketHead* header)
{
    /* 清零包头中的消息源ID,目的ID, 消息源通道号, 消息目的通道号 */
    header->channelOfReceiver = 0;
    header->channelOfSender = 0;
    header->receiverId = 0;
    header->senderId = 0;
}

/**
 * @brief:
 *      设置数据包头中的消息源ID
 * @param[in]:header:数据包头
 * @param[in]:sender:消息源ID
 * @return:
 *     无
 */
static inline void taSetPktHeadSender(T_TA_PacketHead* header, UINT16 sender)
{
    /* 设置数据包头中的源ID */
    header->senderId = sender;
}

/**
 * @brief:
 *      获取数据包头中的消息源ID
 * @param[in]:header:数据包头
 * @return:
 *     数据包头中的源ID
 */
static inline UINT16 taGetPktHeadSender(const T_TA_PacketHead* header)
{
    /* 返回数据包头中的消息源ID*/
    return header->senderId;
}

/**
 * @brief:
 *      设置数据包头中的消息目的ID
 * @param[in]:header:数据包头
 * @param[in]:receiver: 消息目的ID
 * @return:
 *     无
 */
static inline void taSetPktHeadReceiver(T_TA_PacketHead* header, UINT16 receiver)
{
    /* 设置包头中的数据目的ID */
    header->receiverId = receiver;
	
    if(0 == receiver)
    {
         header->receiverId = taGetSaid();
    }
}

/**
 * @brief:
 *      获取数据包头中的消息目的ID
 * @param[in]:header:数据包头
 * @return:
 *     数据包头中的消息目的ID
 */
static inline UINT16 taGetPktHeadReceiver(const T_TA_PacketHead* header)
{
    /* 返回数据包头中的消息目的ID*/
    return header->receiverId;
}

/**
 * @brief:
 *      设置数据包头中的消息源通道号
 * @param[in]:header:数据包头
 * @param[in]:sendChannel:消息源通道号
 * @return:
 *    无
 */
static inline void taSetPktHeadSendChannel(T_TA_PacketHead* header, UINT16 sendChannel)
{
    /* 设置数据包头中的消息源通道号 */
    header->channelOfSender = sendChannel;
}

/**
 * @brief:
 *      获取数据包头中的消息源通道号
 * @param[in]:header:数据包头
 * @return:
 *     数据包头中的消息源通道号
 */
static inline UINT16 taGetPktHeadSendChannel(const T_TA_PacketHead* header)
{
    /* 返回数据包头中的消息源通道号*/
    return header->channelOfSender;
}

/**
 * @brief:
 *      设置数据包头中的消息目的通道号
 * @param[in]:header:数据包头
 * @param[in]:receiverChannel:消息目的通道号
 * @return:
 *    无
 */
static inline void taSetPktHeadRecvChannel(T_TA_PacketHead* header, UINT16 receiverChannel)
{
    /* 设置数据包头中的消息目的通道号 */
    header->channelOfReceiver = receiverChannel;
}

/**
 * @brief:
 *      获取数据包头中的消息目的通道号
 * @param[in]:header:数据包头
 * @return:
 *     数据包头中的消息目的通道号, 0表示操作失败。
 */
static inline UINT16 taGetPktHeadRecvChannel(const T_TA_PacketHead* header)
{
    /* 返回数据包头中的消息目的通道号 */
    return header->channelOfReceiver;
}

/**
 * @brief:
 *      交换数据包头中的消息源ID和目的ID, 消息源通道号和目的通道号
 * @param[in]:header:数据包头
 * @return:
 *     无
 */
static inline void taSwapSenderAndReceiver(T_TA_PacketHead* header)
{
    UINT16 tmp = 0;

    /* 交换数据包头中的消息源ID和目的ID */
    tmp = header->senderId;
    header->senderId = header->receiverId;
    header->receiverId = tmp;

    /* 交换数据包头中的消息源通道号和目的通道号 */
    tmp = header->channelOfSender;
    header->channelOfSender = header->channelOfReceiver;
    header->channelOfReceiver = tmp;
}

/**
 * @brief:
 *      设置数据包中用户数据大小
 * @param[in]:packet:数据包
 * @param[in]:size:数据包中用户数据大小
 * @return:
 *     无
 */
static inline void taSetPktSize(T_TA_Packet* packet, UINT32 size)
{
    /* 设置数据包中用户数据大小 */
    packet->size = size;
}

/**
 * @brief:
 *      获取数据包中用户数据大小
 * @param[in]:packet:数据包
 * @return:
 *     返回数据包中用户数据大小。
 */
static inline UINT32 taGetPktSize(T_TA_Packet* packet)
{
    /* 返回数据包中用户数据大小*/
    return packet->size;
}

/**
 * @brief:
 *     设置数据包中用户数据起始偏移
 * @param[in]:packet:数据包
 * @param[in]:offset: 用户数据在消息包数据部分的偏移
 * @return:
 *     无
 */
static inline void taSetPktOffset(T_TA_Packet* packet, UINT32 offset)
{
    /* 设置数据包中用户数据的偏移 */
    packet->offset = offset;
}

/**
 * @brief:
 *     获取数据包中用户数据起始偏移
 * @param[in]:packet:数据包
 * @return:
 *     返回数据包中用户数据在数据包中的偏移。
 */
static inline UINT32 taGetPktOffset(T_TA_Packet* packet)
{
    /* 返回数据包中用户数据偏移*/
    return packet->offset;
}

/**
 * @brief:
 *     获取数据包中用户数据的最大容量
 * @param[in]:packet:数据包
 * @return:
 *     返回数据包中用户数据最大容量。
 */
static inline UINT32 taGetPktCapacity(T_TA_Packet* packet)
{
    /* 返回数据包中用户数据最大容量 */
    return (sizeof(packet->data) - taGetPktOffset(packet));
}

/**
 * @brief:
 *     设置数据包中消息序列号
 * @param[in]:packet:数据包
 * @param[in]:seq:消息序列号
 * @return:
 *     无
 */
static inline void taSetPktSeq(T_TA_Packet* packet, UINT32 seq)
{
    /* 设置数据包中消息序列号 */
    packet->seqNum = seq;
}

/**
 * @brief:
 *    获取数据包中消息序列号
 * @param[in]:packet:数据包
 * @return:
 *     无
 */
static inline UINT32 taGetPktSeq(T_TA_Packet* packet)
{
    /* 返回数据包中消息序列号 */
    return packet->seqNum;
}

/**
 * @brief:
 *    获取数据包中数据缓冲起始地址
 * @param[in]:packet:数据包
 * @return:
 *     返回数据包中数据缓冲起始地址, NULL表示操作失败。
 */
static inline UINT8* taGetPktRawData(T_TA_Packet* packet)
{
    /*返回数据包中数据缓冲起始地址 */
    return packet->data;
}

/**
 * @brief:
 *    获取数据包中用户数据缓冲起始地址
 * @param[in]:packet:数据包
 * @return:
 *     返回数据包中数据缓冲起始地址, NULL表示操作失败。
 */
static inline UINT8* taGetPktUsrData(T_TA_Packet* packet)
{
    /* 数据包中用户数据大小超过用户数据最大容量 */
    if(packet->size + packet->offset > TA_PACKET_DATA_LEN)
    {
        return NULL;
    }

    /* 返回数据包用户数据缓冲地址 */
    return (packet->data + packet->offset);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TAPACKET_H */
