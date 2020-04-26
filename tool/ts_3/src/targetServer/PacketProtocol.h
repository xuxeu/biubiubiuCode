/*****************************************************************
 *    This source code has been made available to you by CoreTek on an AS-IS
 *    basis. Anyone receiving this source is licensed under CoreTek
 *    copyrights to use it in any way he or she deems fit, including
 *    copying it, modifying it, compiling it, and redistributing it either
 *    with or without modifications.
 *
 *    Any person who transfers this source code or any derivative work
 *    must include the CoreTek copyright notice, this paragraph, and the
 *    preceding two paragraphs in the transferred software.
 *
 *    COPYRIGHT   CoreTek  CORPORATION 2004
 *    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
 ***********************************************************************/

/**
 * 
 *
 * @file     packetProtocol.h
 * @brief     消息协议层
 * @author     彭宏
 * @date     2006-1-6
 * @version 3.0
 * @warning 严禁将本文件到处分发
 *
 * 部门：系统部 
 *
 */
#pragma once

#include "protocol.h"
#include "tsInclude.h"
class Packet;
class PacketProtocol : public ProtocolBase
{
    bool isPutEnd;            //发送结束的标记,对本协议主要的起到时候收到确认包信息,false表示没有收到,true表示收到
    bool isExit;            //退出标志
    int txPacketNum;    //当前序列号
    int rxPacketNum;        //接收包序列
    waitCondition *surePacket;    //确认回复包
    TMutex *mutex;                //对缓冲区进行锁定,
    waitCondition *pWaitCondition;    //确认回复包
    TMutex *waitMutex;                //对缓冲区进行锁定,
    int timeout;                //等待确认包超时时间
    bool isAck;                    //是否收到确认包,初始化时为ture,每发送一次设置为false,收到回复包设置为true
    int retry;            //重试次数
    /**
    发送确认包
    */
    void sendAck(int serialNum) ;
public:
    bool getPutState() 
    {
        return isPutEnd;
    }
    PacketProtocol(DeviceBase* pDevice);
    /**
    * 功能：发送数据包
    * @param  Packet* 数据包指针
    * @return true表示发送成功,false表示发送失败
    */
    virtual int putpkt(Packet* pack) ;
    /**
    * 功能：发送数据包
    * @param  Packet* 数据包指针
    * @return true表示发送成功,false表示发送失败
    */
    virtual int getpkt(Packet* pack) ;
    /**
    * 功能：释放对象对应的资源
    */
    virtual void close();
    /**
    * 功能：释放对象对应的资源
    */
    ~PacketProtocol();
};
