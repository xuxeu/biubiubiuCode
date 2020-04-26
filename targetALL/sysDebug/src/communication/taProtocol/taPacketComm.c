/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:taPacketComm.c
 * @brief:
 *             <li>消息通用收发包函数，对外提供的收发包函数接口</li>
 */

/************************头 文 件*****************************/
#include "taPacketComm.h"
#include "taProtocol.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/*当前设备操作表*/
static T_TA_COMMIf *taCurCOMMIf = NULL;

/************************全局变量*****************************/

/************************实   现********************************/

/**
 * @brief:
 *      设备操作表初始化
 * @param[in]:interface: 设备操作表
 * @return:
 *        TA_OK:操作成功
 *        TA_INVALID_PARAM:无效的输入参数
 */
T_TA_ReturnCode taCOMMInit(T_TA_COMMIf *interface)
{
    /* 参数合法性检查 */
    if (( NULL == interface) || (NULL == interface->read) || (NULL == interface->write))
    {
        return (TA_INVALID_PARAM);
    }

    /* 设置当前设备操作表 */
    taCurCOMMIf = interface;

    return (TA_OK);
}

/*
 * @brief:
 *      接收消息包函数
 * @param[in]:packet:存储收到的消息包指针
 * @return:
 *      同taGetpkt的返回值
 */
T_TA_ReturnCode taCOMMGetPkt(T_TA_Packet *packet)
{
    /* 调用taGetpkt函数接收消息包 */
    return (taGetpkt(taCurCOMMIf, packet));
}

/*
 * @brief:
 *      发送消息包函数
 * @param[in]:packet:发送的消息包指针
 * @return:
 *      同taPutpkt的返回值
 */
T_TA_ReturnCode taCOMMPutPkt(T_TA_Packet *packet)
{
    /* 调用taGetpkt函数发送消息包 */
    return (taPutpkt(taCurCOMMIf, packet));
}

/*
 * @brief:
 *      设置包大小并发送消息包
 * @param[in]:packet :要发送的的消息包指针
 * @param[in]:packetSize :包大小
 * @return:
 *      taCOMMPutPkt的返回值
 */
T_TA_ReturnCode taCOMMSendPacket(T_TA_Packet *packet,UINT32 packetSize)
{
    /* 设置消息包的大小为 */
    taSetPktSize(packet, packetSize);

    /* 调用taCOMMPutPkt函数发送消息包 */
    return (taCOMMPutPkt(packet));
}
