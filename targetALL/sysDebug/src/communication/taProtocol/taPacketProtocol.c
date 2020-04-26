/************************************************************************
*				北京科银京成技术有限公司 版权所有
* 	 Copyright (C)  2013 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/*
 * 修改历史：
 * 2013-06-28         彭元志，北京科银京成技术有限公司
 *                               创建该文件。
*/

/**
* @file： taPacketProtocol.c
* @brief：
*	    <li>该模块实现了一个面向消息的协议</li>
*/

/************************头 文 件******************************/
#include <string.h>
#include "taPacketProtocol.h"
#include "taProtocol.h"
#include "taUtil.h"

/************************宏 定 义******************************/

/* 定义小端环境 */
#ifdef __LITTLE_ENDIAN__
#define adjustCS(s) ((UINT16)(s) )
#else /* 大端环境 */
#define adjustCS(s) (((UINT16)(s) << 8) & 0xFF00)
#endif

/************************类型定义******************************/

/************************外部声明******************************/

/************************前向声明******************************/

/************************模块变量******************************/

/* 确认包字符 */
static UINT8 taAnswer[2] = "+";

/* 发送包序列号值 */
static UINT32 taTxPacketNum = TA_TX_START;

/* 接收包序列号值 */
static UINT32 taRxPacketNum = TA_RX_START;

/* 超时重试次数 */
static UINT32 taWaitRetry = 10;

/* 消息协议操作表 */
static T_TA_TREATY_OPS taTreatPacketOps = {0};

/************************全局变量******************************/

/************************函数实 现******************************/

/*
 * @brief
 *     计算网络协议中的校验和
 * @param[in]: ptr: 指向待校验数据的指针
 * @param[in]: count: 需要检验数据的大小
 * @return:
 *    计算得到的校验和。
 */
UINT16 taCheckSum(void *ptr, UINT16 count)
{
    UINT16 checkSum = 0;
    UINT16 *addr = (UINT16*)ptr;

    /* 按照rfc1071标准计算指定字符的校验和 */
    /* 定义长整形变量保存校验和 */
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

    /* 返回取反后的计算值 */
    return (~checkSum);
}

/*
 * @brief
 *     向设备发送确认包
 * @param[in]: comm: 协议操作表指针
 * @param[in]: serialNum: 数据包的序列号
 * @return:
 *    无
 */
static void taSendCtrlPacketProtocol(T_TA_COMMIf* comm, UINT32 serialNum)
{
    UINT8 buff[10] = {0};
    UINT8 *pBuf = buff;
    UINT32 realCount = 0;

    /* 字节流转换 */
    __store_long(serialNum, pBuf, sizeof(UINT32));
    pBuf += sizeof(UINT32);

    /* 添加应答符号+ */
    memcpy((void *)pBuf, (void *)taAnswer, strlen((const char *)taAnswer));
    pBuf += strlen((const char *)taAnswer);

    /* 发送数据包 */
    comm->write(buff, pBuf-buff,&realCount);
}

/*
 * @brief
 *      从设备接收确认包
 * @param[in]: comm: 协议操作表指针
 * @return:
 *    TA_OK: 成功。
 *    TA_GET_CTRL_PACKET_FAIL: 收到数据包不是确认包或收到数据包格式不正确
 */
static T_TA_ReturnCode taGetCtrlPacketProtocol(T_TA_COMMIf* comm)
{
    UINT32 ackSize = strlen((const char *)taAnswer)+sizeof(UINT32); /* 接收确认包的缓冲区大小 */
    UINT8 ackBuf[ackSize];
    UINT8 *pBuff = ackBuf; /* 对应包头数据 */
    UINT32 recSize = 0;
    INT32 txNum = 0;

    /* 读取确认包 */
    comm->read(pBuff,ackSize,&recSize);

    /* 检查数据包的格式  */
    if ((recSize == strlen((const char *)taAnswer) + sizeof(UINT32)) && (*(pBuff+sizeof(UINT32)) == (UINT8)'+'))
    {
        /* 字节流转换得到确认包的序列号 */
        __fetch_long(&txNum,pBuff,sizeof(UINT32));

        if (taTxPacketNum == txNum)
        {
            return (TA_OK);
        }
    }

    /* 数据包格式或序列号不正确，则返回FAIL */
    return (TA_GET_CTRL_PACKET_FAIL);
}

/*
 * @brief
 *     向指定数据包中填写通信包头，添加发送序列号
 * @param[in]: packet: 指向数据包的指针
 * @return:
 *    无
 */
static void taSetPacketHead(T_TA_Packet* packet)
{
    UINT32 rawSetSize = 0;
    UINT16 srcAid = 0;
    UINT16 srcSaid = 0;
    UINT16 desSaid = 0;
    UINT16 desAid = 0;
    UINT8 *pBuff = NULL;

    /* 获取原始通信数据 */
    pBuff = taGetPktRawData(packet);

    /* 获取数据包目的和源代理号 */
    rawSetSize = taGetPktSize(packet);
    srcAid = taGetPktHeadSender(&(packet->header));
    desAid = taGetPktHeadReceiver(&(packet->header));
    srcSaid = taGetPktHeadSendChannel(&(packet->header));
    desSaid = taGetPktHeadRecvChannel(&(packet->header));

    /* 预留校验和 */
    *(UINT16*)pBuff = 0;
    pBuff = pBuff+TA_PROTOCOL_CHECKSUM_SIZE;

    /* 把数据包大小转换字节流 */
    __store_long(rawSetSize,pBuff,sizeof(UINT32));
    pBuff += sizeof(UINT32);

    /* 把desAid转换为字节流 */
    __store_short(desAid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

    /* 把srcAid转换为字节流 */
    __store_short(srcAid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

     /* 把desSaid转换为字节流 */
    __store_short(desSaid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

     /* 把srcSaid转换为字节流 */
    __store_short(srcSaid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

    /* 转换确认包序列 */
    __store_long(taTxPacketNum,pBuff,sizeof(UINT32));
    pBuff += sizeof(UINT32);
}

/*
 * @brief
 *     按消息协议，接收一个数据包
 * @param[in]: comm: 协议操作表指针
 * @param[out]: packet: 指向数据包缓冲的指针
 * @return:
 *    TA_GET_PACKET_DATA_FAIL: 接收UDP数据包失败。
 *    TA_CTRL_PACKET：收到数据包为确认数据包
 *    TA_PACKET_CHECK_SUM_FAIL: 收到数据包为校验和失败包。
 *    TA_OK: 收到确认包或收到UDP数据包。
 */
static T_TA_ReturnCode taGetpktWithPacketProtocol(T_TA_COMMIf* comm, T_TA_Packet* packet)
{
    UINT8* pBuff = NULL; /* 获取数据包原始数据 */
    UINT32 rawGetpktSize = 0;
    UINT16 srcAid  = 0;
    UINT16 srcSaid = 0;
    UINT16 desSaid = 0;
    UINT16 desAid  = 0;
    UINT32 recSize = 0;
    UINT32 serialNum = 0;

    /* 获取数据 */
    pBuff = taGetPktRawData(packet);

    /* 接收一个数据包 */
    comm->read(pBuff, taGetPktCapacity(packet), &recSize);
    if (recSize <= 0)
    {
        /* 读取失败 */
        return (TA_GET_PACKET_DATA_FAIL);
    }
    else
    {
        if ((recSize == strlen((const char *)taAnswer) + sizeof(UINT32)) && (*(pBuff+sizeof(UINT32)) == (UINT8)'+'))
        {
            /* 数据包为确认数据包 */
            return (TA_CTRL_PACKET);
        }
    }
    
    /* 跳过数据包中解析校验和 */
    pBuff += sizeof(UINT16);
    
    /* 对接收到的包头信息进行解码，转换字节流 */
    __fetch_long((INT32 *)&rawGetpktSize,pBuff,sizeof(UINT32));
    pBuff += sizeof(UINT32);

     /* 计算接收包的校验和并验证，如果接收数据错误，不继续解析数据 */
    if((rawGetpktSize > TA_PKT_DATA_SIZE)
        || ( 0 != taCheckSum((void *)taGetPktRawData(packet), (UINT16)TA_PROTOCOL_CHECKSUM_SIZE + (UINT16)TA_PROTOCOL_HEAD_SIZE + (UINT16)rawGetpktSize)))
    {
        return (TA_PACKET_CHECK_SUM_FAIL);
    }

     /* 转换desAid */
    __fetch_short(&desAid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

     /* 转换srcAid */
    __fetch_short(&srcAid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

    /* 转换desSaid */
    __fetch_short(&desSaid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

    /* 转换srcSaid */
    __fetch_short(&srcSaid,pBuff,sizeof(UINT16));
    pBuff += sizeof(UINT16);

    /* 转换确认包序列号 */
    __fetch_long((INT32 *)&serialNum,pBuff,sizeof(UINT32));
    pBuff += sizeof(UINT32);

    /* 发送确认包 */
    taSendCtrlPacketProtocol(comm, (UINT32)serialNum);

    /* 比较数据包序列号 */
    if (taRxPacketNum != serialNum)
    {
        if (TA_TS_TX_START == serialNum)
        {
            /* 主机端第一次发包 */
            taRxPacketNum = (UINT32)TA_RX_PACKET_MIN - (UINT32)1;
        }
        else
		{
			if (TA_RX_START == taRxPacketNum)
	        {
	            /* 目标机重启,同步序列号 */
	            taRxPacketNum = serialNum;
	        }
	        else
			{
				if (((taRxPacketNum>serialNum) && (taRxPacketNum<=serialNum+TA_SYNC_RANGE))
		        || (serialNum+TA_SYNC_RANGE >= TA_RX_PACKET_MAX))
		        {
		            /* 已经处理过的包，丢弃 */
		            return (TA_HANDLED_PACKET);
		        }
		        else
		        {
		            /* 数据包不同步 */
		            taRxPacketNum = serialNum;
		        }
	        }
        }
    }

    /* 设置数据包目的和源代理号 */
    taSetPktOffset(packet,TA_PROTOCOL_HEAD_SIZE+TA_PROTOCOL_CHECKSUM_SIZE);
    taSetPktSize(packet,rawGetpktSize);
    taSetPktHeadSender(&packet->header,srcAid);
    taSetPktHeadReceiver(&packet->header,desAid);
    taSetPktHeadSendChannel(&packet->header,srcSaid);
    taSetPktHeadRecvChannel(&packet->header,desSaid);
    taSetPktSeq(packet, serialNum);

    /* 大于则重新开始 */
    if (++taRxPacketNum >= TA_RX_PACKET_MAX)
    {
        taRxPacketNum = TA_RX_PACKET_MIN;
    }

    return (TA_OK);
}

/*
 * @brief
 *     按消息协议，发送指定的数据包
 * @param[in]: comm: 协议操作表指针
 * @param[in]: packet: 指向发送数据包的指针
 * @return:
 *    TA_OK: 成功
 *    TA_GET_CTRL_PACKET_FAIL: 收到数据包不是确认包或收到数据包格式不正确。
 */
static T_TA_ReturnCode taPutpktWithPacketProtocol(T_TA_COMMIf* comm, T_TA_Packet* packet)
{
    UINT8* pData = NULL;
    UINT32 retry = 0;
    T_TA_ReturnCode ret = TA_FAIL;
    UINT32 realCount = 0;

    pData = taGetPktRawData(packet);

    /* 设置数据包的通信包头 */
    taSetPacketHead(packet);

    /* 计算发送数据包的校验和 */
    *(UINT16*)pData = taCheckSum((void *)pData,(UINT16)TA_PROTOCOL_HEAD_SIZE + (UINT16)taGetPktSize(packet)
                                 + (UINT16)TA_PROTOCOL_CHECKSUM_SIZE);

    /* 向设备写入数据包 */
    comm->write(taGetPktRawData(packet),TA_PROTOCOL_CHECKSUM_SIZE+
                         TA_PROTOCOL_HEAD_SIZE+taGetPktSize(packet),&realCount);

    /* 循环接收确认包 */
    while (retry++ < taWaitRetry)
    {
    	/* 接收确认包 */
    	ret = taGetCtrlPacketProtocol(comm);
        if (ret == TA_OK)
        {
            break;
        }

        /* 重新发送数据包  */
        comm->write(taGetPktRawData(packet),TA_PROTOCOL_CHECKSUM_SIZE+
                             TA_PROTOCOL_HEAD_SIZE+taGetPktSize(packet),&realCount);
        ret = TA_FAIL;
    }

    /* 第1次发包时,使用特殊序号,防止反复连接时被过滤 */
    if ( (TA_TX_START == taTxPacketNum)
        || (TA_TX_PACKET_MAX <= ++taTxPacketNum))
    {
        taTxPacketNum = TA_TX_PACKET_MIN;
    }

    return (ret);
}

/*
 * @brief
 *     UDP 消息协议的初始化,向协议操作表链中添加消息协议操作表
 * @return:
 *    无
 */
void taTreatyUDPInit(void)
{
    /* 初始化消息协议操作表 */
    taTreatPacketOps.type= TA_COMM_UDP;
    taTreatPacketOps.offset = TA_PROTOCOL_CHECKSUM_SIZE+TA_PROTOCOL_HEAD_SIZE;
    taTreatPacketOps.putpkt = taPutpktWithPacketProtocol;
    taTreatPacketOps.getpkt = taGetpktWithPacketProtocol;

    /* 向操作表链中添加消息协议操作表 */
    taTreatyAdd(&taTreatPacketOps);
}
#ifdef __PPC__
/**********************下面函数用于同步包序号*****************************/
#ifdef _KERNEL_DEBUG_
/*
 * @brief
 *     获取taTxPacketNum、taRxPacketNum的值
 * @param[out]: pTaTxPacketNum: 输出指针
 * @param[out]: pTaRxPacketNum: 输出指针
 * @return:
 *            无
 */
void TaPacketNumGet(UINT32* pTaTxPacketNum, UINT32* pTaRxPacketNum)
{
   *pTaTxPacketNum = taTxPacketNum;
   *pTaRxPacketNum = taRxPacketNum;
}
#else
/*
 * @brief
 *     获取taTxPacketNum、taRxPacketNum的值
 * @param[out]: pTaTxPacketNum: 输出指针
 * @param[out]: pTaRxPacketNum: 输出指针
 * @return:
 *            无
 */
void TaPacketNumSet(UINT32 TxPacketNum, UINT32 TaRxPacketNum)
{
    taTxPacketNum = TxPacketNum;
    taRxPacketNum = TaRxPacketNum;
}
#endif
#endif
