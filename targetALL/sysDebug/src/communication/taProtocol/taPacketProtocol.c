/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
* 	 Copyright (C)  2013 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/*
 * �޸���ʷ��
 * 2013-06-28         ��Ԫ־�������������ɼ������޹�˾
 *                               �������ļ���
*/

/**
* @file�� taPacketProtocol.c
* @brief��
*	    <li>��ģ��ʵ����һ��������Ϣ��Э��</li>
*/

/************************ͷ �� ��******************************/
#include <string.h>
#include "taPacketProtocol.h"
#include "taProtocol.h"
#include "taUtil.h"

/************************�� �� ��******************************/

/* ����С�˻��� */
#ifdef __LITTLE_ENDIAN__
#define adjustCS(s) ((UINT16)(s) )
#else /* ��˻��� */
#define adjustCS(s) (((UINT16)(s) << 8) & 0xFF00)
#endif

/************************���Ͷ���******************************/

/************************�ⲿ����******************************/

/************************ǰ������******************************/

/************************ģ�����******************************/

/* ȷ�ϰ��ַ� */
static UINT8 taAnswer[2] = "+";

/* ���Ͱ����к�ֵ */
static UINT32 taTxPacketNum = TA_TX_START;

/* ���հ����к�ֵ */
static UINT32 taRxPacketNum = TA_RX_START;

/* ��ʱ���Դ��� */
static UINT32 taWaitRetry = 10;

/* ��ϢЭ������� */
static T_TA_TREATY_OPS taTreatPacketOps = {0};

/************************ȫ�ֱ���******************************/

/************************����ʵ ��******************************/

/*
 * @brief
 *     ��������Э���е�У���
 * @param[in]: ptr: ָ���У�����ݵ�ָ��
 * @param[in]: count: ��Ҫ�������ݵĴ�С
 * @return:
 *    ����õ���У��͡�
 */
UINT16 taCheckSum(void *ptr, UINT16 count)
{
    UINT16 checkSum = 0;
    UINT16 *addr = (UINT16*)ptr;

    /* ����rfc1071��׼����ָ���ַ���У��� */
    /* ���峤���α�������У��� */
    UINT32 sum = 0;

    while (count > 1)
    {
        sum += *addr++;
        count -= 2;
    }

    if (count > 0)
    {
        sum += adjustCS(* (UINT8 *) addr);
    }

    while ((sum>>16) > 0)
    {
        sum = (sum & (UINT32)0xffff) + (sum >> 16);
    }

    checkSum = (UINT16)sum;

    /* ����ȡ����ļ���ֵ */
    return (~checkSum);
}

/*
 * @brief
 *     ���豸����ȷ�ϰ�
 * @param[in]: comm: Э�������ָ��
 * @param[in]: serialNum: ���ݰ������к�
 * @return:
 *    ��
 */
static void taSendCtrlPacketProtocol(T_TA_COMMIf* comm, UINT32 serialNum)
{
    UINT8 buff[10] = {0};
    UINT8 *pBuf = buff;
    UINT32 realCount = 0;

    /* �ֽ���ת�� */
    __store_long(serialNum, pBuf, sizeof(UINT32));
    pBuf += sizeof(UINT32);

    /* ���Ӧ�����+ */
    memcpy((void *)pBuf, (void *)taAnswer, strlen((const char *)taAnswer));
    pBuf += strlen((const char *)taAnswer);

    /* �������ݰ� */
    comm->write(buff, pBuf-buff,&realCount);
}

/*
 * @brief
 *      ���豸����ȷ�ϰ�
 * @param[in]: comm: Э�������ָ��
 * @return:
 *    TA_OK: �ɹ���
 *    TA_GET_CTRL_PACKET_FAIL: �յ����ݰ�����ȷ�ϰ����յ����ݰ���ʽ����ȷ
 */
static T_TA_ReturnCode taGetCtrlPacketProtocol(T_TA_COMMIf* comm)
{
    UINT32 ackSize = strlen((const char *)taAnswer)+sizeof(UINT32); /* ����ȷ�ϰ��Ļ�������С */
    UINT8 ackBuf[ackSize];
    UINT8 *pBuff = ackBuf; /* ��Ӧ��ͷ���� */
    UINT32 recSize = 0;
    INT32 txNum = 0;

    /* ��ȡȷ�ϰ� */
    comm->read(pBuff,ackSize,&recSize);

    /* ������ݰ��ĸ�ʽ  */
    if ((recSize == strlen((const char *)taAnswer) + sizeof(UINT32)) && (*(pBuff+sizeof(UINT32)) == (UINT8)'+'))
    {
        /* �ֽ���ת���õ�ȷ�ϰ������к� */
        __fetch_long(&txNum,pBuff,sizeof(UINT32));

        if (taTxPacketNum == txNum)
        {
            return (TA_OK);
        }
    }

    /* ���ݰ���ʽ�����кŲ���ȷ���򷵻�FAIL */
    return (TA_GET_CTRL_PACKET_FAIL);
}

/*
 * @brief
 *     ��ָ�����ݰ�����дͨ�Ű�ͷ����ӷ������к�
 * @param[in]: packet: ָ�����ݰ���ָ��
 * @return:
 *    ��
 */
static void taSetPacketHead(T_TA_Packet* packet)
{
    UINT32 rawSetSize = 0;
    UINT16 srcAid = 0;
    UINT16 srcSaid = 0;
    UINT16 desSaid = 0;
    UINT16 desAid = 0;
    UINT8 *pBuff = NULL;

    /* ��ȡԭʼͨ������ */
    pBuff = taGetPktRawData(packet);

    /* ��ȡ���ݰ�Ŀ�ĺ�Դ����� */
    rawSetSize = taGetPktSize(packet);
    srcAid = taGetPktHeadSender(&(packet->header));
    desAid = taGetPktHeadReceiver(&(packet->header));
    srcSaid = taGetPktHeadSendChannel(&(packet->header));
    desSaid = taGetPktHeadRecvChannel(&(packet->header));

    /* Ԥ��У��� */
    *(UINT16*)pBuff = 0;
    pBuff = pBuff+TA_PROTOCOL_CHECKSUM_SIZE;

    /* �����ݰ���Сת���ֽ��� */
    __store_long(rawSetSize,pBuff,sizeof(UINT32));
    pBuff += sizeof(UINT32);

    /* ��desAidת��Ϊ�ֽ��� */
    __store_short(desAid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

    /* ��srcAidת��Ϊ�ֽ��� */
    __store_short(srcAid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

     /* ��desSaidת��Ϊ�ֽ��� */
    __store_short(desSaid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

     /* ��srcSaidת��Ϊ�ֽ��� */
    __store_short(srcSaid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

    /* ת��ȷ�ϰ����� */
    __store_long(taTxPacketNum,pBuff,sizeof(UINT32));
    pBuff += sizeof(UINT32);
}

/*
 * @brief
 *     ����ϢЭ�飬����һ�����ݰ�
 * @param[in]: comm: Э�������ָ��
 * @param[out]: packet: ָ�����ݰ������ָ��
 * @return:
 *    TA_GET_PACKET_DATA_FAIL: ����UDP���ݰ�ʧ�ܡ�
 *    TA_CTRL_PACKET���յ����ݰ�Ϊȷ�����ݰ�
 *    TA_PACKET_CHECK_SUM_FAIL: �յ����ݰ�ΪУ���ʧ�ܰ���
 *    TA_OK: �յ�ȷ�ϰ����յ�UDP���ݰ���
 */
static T_TA_ReturnCode taGetpktWithPacketProtocol(T_TA_COMMIf* comm, T_TA_Packet* packet)
{
    UINT8* pBuff = NULL; /* ��ȡ���ݰ�ԭʼ���� */
    UINT32 rawGetpktSize = 0;
    UINT16 srcAid  = 0;
    UINT16 srcSaid = 0;
    UINT16 desSaid = 0;
    UINT16 desAid  = 0;
    UINT32 recSize = 0;
    UINT32 serialNum = 0;

    /* ��ȡ���� */
    pBuff = taGetPktRawData(packet);

    /* ����һ�����ݰ� */
    comm->read(pBuff, taGetPktCapacity(packet), &recSize);
    if (recSize <= 0)
    {
        /* ��ȡʧ�� */
        return (TA_GET_PACKET_DATA_FAIL);
    }
    else
    {
        if ((recSize == strlen((const char *)taAnswer) + sizeof(UINT32)) && (*(pBuff+sizeof(UINT32)) == (UINT8)'+'))
        {
            /* ���ݰ�Ϊȷ�����ݰ� */
            return (TA_CTRL_PACKET);
        }
    }
    
    /* �������ݰ��н���У��� */
    pBuff += sizeof(UINT16);
    
    /* �Խ��յ��İ�ͷ��Ϣ���н��룬ת���ֽ��� */
    __fetch_long((INT32 *)&rawGetpktSize,pBuff,sizeof(UINT32));
    pBuff += sizeof(UINT32);

     /* ������հ���У��Ͳ���֤������������ݴ��󣬲������������� */
    if((rawGetpktSize > TA_PKT_DATA_SIZE)
        || ( 0 != taCheckSum((void *)taGetPktRawData(packet), (UINT16)TA_PROTOCOL_CHECKSUM_SIZE + (UINT16)TA_PROTOCOL_HEAD_SIZE + (UINT16)rawGetpktSize)))
    {
        return (TA_PACKET_CHECK_SUM_FAIL);
    }

     /* ת��desAid */
    __fetch_short(&desAid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

     /* ת��srcAid */
    __fetch_short(&srcAid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

    /* ת��desSaid */
    __fetch_short(&desSaid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

    /* ת��srcSaid */
    __fetch_short(&srcSaid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

    /* ת��ȷ�ϰ����к� */
    __fetch_long((INT32 *)&serialNum,pBuff,sizeof(UINT32));
    pBuff += sizeof(UINT32);

    /* ����ȷ�ϰ� */
    taSendCtrlPacketProtocol(comm, (UINT32)serialNum);

    /* �Ƚ����ݰ����к� */
    if (taRxPacketNum != serialNum)
    {
        if (TA_TS_TX_START == serialNum)
        {
            /* �����˵�һ�η��� */
            taRxPacketNum = (UINT32)TA_RX_PACKET_MIN - (UINT32)1;
        }
        else
		{
			if (TA_RX_START == taRxPacketNum)
	        {
	            /* Ŀ�������,ͬ�����к� */
	            taRxPacketNum = serialNum;
	        }
	        else
			{
				if (((taRxPacketNum>serialNum) && (taRxPacketNum<=serialNum+TA_SYNC_RANGE))
		        || (serialNum+TA_SYNC_RANGE >= TA_RX_PACKET_MAX))
		        {
		            /* �Ѿ�������İ������� */
		            return (TA_HANDLED_PACKET);
		        }
		        else
		        {
		            /* ���ݰ���ͬ�� */
		            taRxPacketNum = serialNum;
		        }
	        }
        }
    }

    /* �������ݰ�Ŀ�ĺ�Դ����� */
    taSetPktOffset(packet,TA_PROTOCOL_HEAD_SIZE+TA_PROTOCOL_CHECKSUM_SIZE);
    taSetPktSize(packet,rawGetpktSize);
    taSetPktHeadSender(&packet->header,srcAid);
    taSetPktHeadReceiver(&packet->header,desAid);
    taSetPktHeadSendChannel(&packet->header,srcSaid);
    taSetPktHeadRecvChannel(&packet->header,desSaid);
    taSetPktSeq(packet, serialNum);

    /* ���������¿�ʼ */
    if (++taRxPacketNum >= TA_RX_PACKET_MAX)
    {
        taRxPacketNum = TA_RX_PACKET_MIN;
    }

    return (TA_OK);
}

/*
 * @brief
 *     ����ϢЭ�飬����ָ�������ݰ�
 * @param[in]: comm: Э�������ָ��
 * @param[in]: packet: ָ�������ݰ���ָ��
 * @return:
 *    TA_OK: �ɹ�
 *    TA_GET_CTRL_PACKET_FAIL: �յ����ݰ�����ȷ�ϰ����յ����ݰ���ʽ����ȷ��
 */
static T_TA_ReturnCode taPutpktWithPacketProtocol(T_TA_COMMIf* comm, T_TA_Packet* packet)
{
    UINT8* pData = NULL;
    UINT32 retry = 0;
    T_TA_ReturnCode ret = TA_FAIL;
    UINT32 realCount = 0;

    pData = taGetPktRawData(packet);

    /* �������ݰ���ͨ�Ű�ͷ */
    taSetPacketHead(packet);

    /* ���㷢�����ݰ���У��� */
    *(UINT16*)pData = taCheckSum((void *)pData,(UINT16)TA_PROTOCOL_HEAD_SIZE + (UINT16)taGetPktSize(packet)
                                 + (UINT16)TA_PROTOCOL_CHECKSUM_SIZE);

    /* ���豸д�����ݰ� */
    comm->write(taGetPktRawData(packet),TA_PROTOCOL_CHECKSUM_SIZE+
                         TA_PROTOCOL_HEAD_SIZE+taGetPktSize(packet),&realCount);

    /* ѭ������ȷ�ϰ� */
    while (retry++ < taWaitRetry)
    {
    	/* ����ȷ�ϰ� */
    	ret = taGetCtrlPacketProtocol(comm);
        if (ret == TA_OK)
        {
            break;
        }

        /* ���·������ݰ�  */
        comm->write(taGetPktRawData(packet),TA_PROTOCOL_CHECKSUM_SIZE+
                             TA_PROTOCOL_HEAD_SIZE+taGetPktSize(packet),&realCount);
        ret = TA_FAIL;
    }

    /* ��1�η���ʱ,ʹ���������,��ֹ��������ʱ������ */
    if ( (TA_TX_START == taTxPacketNum)
        || (TA_TX_PACKET_MAX <= ++taTxPacketNum))
    {
        taTxPacketNum = TA_TX_PACKET_MIN;
    }

    return (ret);
}

/*
 * @brief
 *     UDP ��ϢЭ��ĳ�ʼ��,��Э����������������ϢЭ�������
 * @return:
 *    ��
 */
void taTreatyUDPInit(void)
{
    /* ��ʼ����ϢЭ������� */
    taTreatPacketOps.type= TA_COMM_UDP;
    taTreatPacketOps.offset = TA_PROTOCOL_CHECKSUM_SIZE+TA_PROTOCOL_HEAD_SIZE;
    taTreatPacketOps.putpkt = taPutpktWithPacketProtocol;
    taTreatPacketOps.getpkt = taGetpktWithPacketProtocol;

    /* ����������������ϢЭ������� */
    taTreatyAdd(&taTreatPacketOps);
}
#ifdef __PPC__
/**********************���溯������ͬ�������*****************************/
#ifdef _KERNEL_DEBUG_
/*
 * @brief
 *     ��ȡtaTxPacketNum��taRxPacketNum��ֵ
 * @param[out]: pTaTxPacketNum: ���ָ��
 * @param[out]: pTaRxPacketNum: ���ָ��
 * @return:
 *            ��
 */
void TaPacketNumGet(UINT32* pTaTxPacketNum, UINT32* pTaRxPacketNum)
{
   *pTaTxPacketNum = taTxPacketNum;
   *pTaRxPacketNum = taRxPacketNum;
}
#else
/*
 * @brief
 *     ��ȡtaTxPacketNum��taRxPacketNum��ֵ
 * @param[out]: pTaTxPacketNum: ���ָ��
 * @param[out]: pTaRxPacketNum: ���ָ��
 * @return:
 *            ��
 */
void TaPacketNumSet(UINT32 TxPacketNum, UINT32 TaRxPacketNum)
{
    taTxPacketNum = TxPacketNum;
    taRxPacketNum = TaRxPacketNum;
}
#endif
#endif
