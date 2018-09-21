/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-28        ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:taPacket.h
 * @brief:
 *             <li>���ݰ����õĲ���</li>
 */
#ifndef _TAPACKET_H
#define _TAPACKET_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ �� ��******************************/
#include "taTypes.h"

/************************�궨��********************************/

/* ��ϢЭ��ͷ��С */
#define TA_PROTOCOL_HEAD_SIZE 16

/* ��Ϣ��Ч�����С */
#define TA_PKT_DATA_SIZE 1024

/* У��ʹ�С */
#define TA_PROTOCOL_CHECKSUM_SIZE 2

/* ��Ϣ�����С */
#define TA_PACKET_DATA_LEN (TA_PKT_DATA_SIZE+TA_PROTOCOL_HEAD_SIZE+TA_PROTOCOL_CHECKSUM_SIZE)

/************************���Ͷ���******************************/

/* TAͨ�����ݰ��İ�ͷ */
typedef struct
{
    UINT16 senderId;            /* ���Ͷ�ID,��srcAid��Ӧ */
    UINT16 receiverId;          /* ���ն�ID����desAid��Ӧ */
    UINT16 channelOfSender;     /* ���Ͷ�ͨ���ţ���ͨ�������ڷ��Ͷ��ڽ�������ʱ������ȷ�������յ����ݷŵ��ĸ�ͨ���У���srcSaid��Ӧ */
    UINT16 channelOfReceiver;   /* ���ն�ͨ���ţ���ͨ��������ȷ�������ն��ڽ��յ������Ժ󣬽����ݷŵ��ĸ�ͨ���У���desSaid��Ӧ */
} T_TA_PacketHead;

/* ���ݰ��ṹ */
typedef struct
{
    struct T_TA_Packet *next;
    T_TA_PacketHead header;
    UINT32 size;       /* ������Ч������ռ�ռ䣬�������ݲ����ܹ���1000���ֽڣ�����ֻ��size���ֽڲ�����Ч�� */
    UINT32 offset;     /* ��Ч��������ƫ��λ�ã��������ݲ��ֻ���ַ��ʼ��Ҫƫ��offset���ֽ�֮����ܵõ���Ч���� */
    UINT32 seqNum;     /* ����ţ�Э���ڲ�ʹ��*/
    UINT8 data[TA_PACKET_DATA_LEN];
}T_TA_Packet;

/************************�ӿ�����******************************/

/**
 * @brief:
 *      �����ݰ�ͷ����
 * @param[in]:header:���ݰ�ͷ
 * @return:
 *     ��
 */
static inline void taZeroPktHead(T_TA_PacketHead* header)
{
    /* �����ͷ�е���ϢԴID,Ŀ��ID, ��ϢԴͨ����, ��ϢĿ��ͨ���� */
    header->channelOfReceiver = 0;
    header->channelOfSender = 0;
    header->receiverId = 0;
    header->senderId = 0;
}

/**
 * @brief:
 *      �������ݰ�ͷ�е���ϢԴID
 * @param[in]:header:���ݰ�ͷ
 * @param[in]:sender:��ϢԴID
 * @return:
 *     ��
 */
static inline void taSetPktHeadSender(T_TA_PacketHead* header, UINT16 sender)
{
    /* �������ݰ�ͷ�е�ԴID */
    header->senderId = sender;
}

/**
 * @brief:
 *      ��ȡ���ݰ�ͷ�е���ϢԴID
 * @param[in]:header:���ݰ�ͷ
 * @return:
 *     ���ݰ�ͷ�е�ԴID
 */
static inline UINT16 taGetPktHeadSender(const T_TA_PacketHead* header)
{
    /* �������ݰ�ͷ�е���ϢԴID*/
    return header->senderId;
}

/**
 * @brief:
 *      �������ݰ�ͷ�е���ϢĿ��ID
 * @param[in]:header:���ݰ�ͷ
 * @param[in]:receiver: ��ϢĿ��ID
 * @return:
 *     ��
 */
static inline void taSetPktHeadReceiver(T_TA_PacketHead* header, UINT16 receiver)
{
    /* ���ð�ͷ�е�����Ŀ��ID */
    header->receiverId = receiver;
	
    if(0 == receiver)
    {
         header->receiverId = taGetSaid();
    }
}

/**
 * @brief:
 *      ��ȡ���ݰ�ͷ�е���ϢĿ��ID
 * @param[in]:header:���ݰ�ͷ
 * @return:
 *     ���ݰ�ͷ�е���ϢĿ��ID
 */
static inline UINT16 taGetPktHeadReceiver(const T_TA_PacketHead* header)
{
    /* �������ݰ�ͷ�е���ϢĿ��ID*/
    return header->receiverId;
}

/**
 * @brief:
 *      �������ݰ�ͷ�е���ϢԴͨ����
 * @param[in]:header:���ݰ�ͷ
 * @param[in]:sendChannel:��ϢԴͨ����
 * @return:
 *    ��
 */
static inline void taSetPktHeadSendChannel(T_TA_PacketHead* header, UINT16 sendChannel)
{
    /* �������ݰ�ͷ�е���ϢԴͨ���� */
    header->channelOfSender = sendChannel;
}

/**
 * @brief:
 *      ��ȡ���ݰ�ͷ�е���ϢԴͨ����
 * @param[in]:header:���ݰ�ͷ
 * @return:
 *     ���ݰ�ͷ�е���ϢԴͨ����
 */
static inline UINT16 taGetPktHeadSendChannel(const T_TA_PacketHead* header)
{
    /* �������ݰ�ͷ�е���ϢԴͨ����*/
    return header->channelOfSender;
}

/**
 * @brief:
 *      �������ݰ�ͷ�е���ϢĿ��ͨ����
 * @param[in]:header:���ݰ�ͷ
 * @param[in]:receiverChannel:��ϢĿ��ͨ����
 * @return:
 *    ��
 */
static inline void taSetPktHeadRecvChannel(T_TA_PacketHead* header, UINT16 receiverChannel)
{
    /* �������ݰ�ͷ�е���ϢĿ��ͨ���� */
    header->channelOfReceiver = receiverChannel;
}

/**
 * @brief:
 *      ��ȡ���ݰ�ͷ�е���ϢĿ��ͨ����
 * @param[in]:header:���ݰ�ͷ
 * @return:
 *     ���ݰ�ͷ�е���ϢĿ��ͨ����, 0��ʾ����ʧ�ܡ�
 */
static inline UINT16 taGetPktHeadRecvChannel(const T_TA_PacketHead* header)
{
    /* �������ݰ�ͷ�е���ϢĿ��ͨ���� */
    return header->channelOfReceiver;
}

/**
 * @brief:
 *      �������ݰ�ͷ�е���ϢԴID��Ŀ��ID, ��ϢԴͨ���ź�Ŀ��ͨ����
 * @param[in]:header:���ݰ�ͷ
 * @return:
 *     ��
 */
static inline void taSwapSenderAndReceiver(T_TA_PacketHead* header)
{
    UINT16 tmp = 0;

    /* �������ݰ�ͷ�е���ϢԴID��Ŀ��ID */
    tmp = header->senderId;
    header->senderId = header->receiverId;
    header->receiverId = tmp;

    /* �������ݰ�ͷ�е���ϢԴͨ���ź�Ŀ��ͨ���� */
    tmp = header->channelOfSender;
    header->channelOfSender = header->channelOfReceiver;
    header->channelOfReceiver = tmp;
}

/**
 * @brief:
 *      �������ݰ����û����ݴ�С
 * @param[in]:packet:���ݰ�
 * @param[in]:size:���ݰ����û����ݴ�С
 * @return:
 *     ��
 */
static inline void taSetPktSize(T_TA_Packet* packet, UINT32 size)
{
    /* �������ݰ����û����ݴ�С */
    packet->size = size;
}

/**
 * @brief:
 *      ��ȡ���ݰ����û����ݴ�С
 * @param[in]:packet:���ݰ�
 * @return:
 *     �������ݰ����û����ݴ�С��
 */
static inline UINT32 taGetPktSize(T_TA_Packet* packet)
{
    /* �������ݰ����û����ݴ�С*/
    return packet->size;
}

/**
 * @brief:
 *     �������ݰ����û�������ʼƫ��
 * @param[in]:packet:���ݰ�
 * @param[in]:offset: �û���������Ϣ�����ݲ��ֵ�ƫ��
 * @return:
 *     ��
 */
static inline void taSetPktOffset(T_TA_Packet* packet, UINT32 offset)
{
    /* �������ݰ����û����ݵ�ƫ�� */
    packet->offset = offset;
}

/**
 * @brief:
 *     ��ȡ���ݰ����û�������ʼƫ��
 * @param[in]:packet:���ݰ�
 * @return:
 *     �������ݰ����û����������ݰ��е�ƫ�ơ�
 */
static inline UINT32 taGetPktOffset(T_TA_Packet* packet)
{
    /* �������ݰ����û�����ƫ��*/
    return packet->offset;
}

/**
 * @brief:
 *     ��ȡ���ݰ����û����ݵ��������
 * @param[in]:packet:���ݰ�
 * @return:
 *     �������ݰ����û��������������
 */
static inline UINT32 taGetPktCapacity(T_TA_Packet* packet)
{
    /* �������ݰ����û������������ */
    return (sizeof(packet->data) - taGetPktOffset(packet));
}

/**
 * @brief:
 *     �������ݰ�����Ϣ���к�
 * @param[in]:packet:���ݰ�
 * @param[in]:seq:��Ϣ���к�
 * @return:
 *     ��
 */
static inline void taSetPktSeq(T_TA_Packet* packet, UINT32 seq)
{
    /* �������ݰ�����Ϣ���к� */
    packet->seqNum = seq;
}

/**
 * @brief:
 *    ��ȡ���ݰ�����Ϣ���к�
 * @param[in]:packet:���ݰ�
 * @return:
 *     ��
 */
static inline UINT32 taGetPktSeq(T_TA_Packet* packet)
{
    /* �������ݰ�����Ϣ���к� */
    return packet->seqNum;
}

/**
 * @brief:
 *    ��ȡ���ݰ������ݻ�����ʼ��ַ
 * @param[in]:packet:���ݰ�
 * @return:
 *     �������ݰ������ݻ�����ʼ��ַ, NULL��ʾ����ʧ�ܡ�
 */
static inline UINT8* taGetPktRawData(T_TA_Packet* packet)
{
    /*�������ݰ������ݻ�����ʼ��ַ */
    return packet->data;
}

/**
 * @brief:
 *    ��ȡ���ݰ����û����ݻ�����ʼ��ַ
 * @param[in]:packet:���ݰ�
 * @return:
 *     �������ݰ������ݻ�����ʼ��ַ, NULL��ʾ����ʧ�ܡ�
 */
static inline UINT8* taGetPktUsrData(T_TA_Packet* packet)
{
    /* ���ݰ����û����ݴ�С�����û������������ */
    if(packet->size + packet->offset > TA_PACKET_DATA_LEN)
    {
        return NULL;
    }

    /* �������ݰ��û����ݻ����ַ */
    return (packet->data + packet->offset);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TAPACKET_H */
