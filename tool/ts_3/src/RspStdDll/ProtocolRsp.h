/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
* @file 	PortocolRsp.h
* @brief 	
*     <li>功能： 定义tcp,udp通信方式的协议对象的类</li>
* @author 	唐兴培
* @date 	2007-10-23
* <p>部门：系统部 
*/

#ifndef _PROTOCOLRSP_H_
#define _PROTOCOLRSP_H_

//#include "stdafx.h"
#include "DeviceRsp.h"
#include "../../include/sysTypes.h"
#include "common.h"

class ProtocolBase 
{
private:
    DeviceBase* pDevice;
public:
    static const int TCP_PROTOCOL = 0;
    static const int PACKET_PROTOCOL = 1;
    //static const int STREAM_PROTOCOL = 2;
    //static const int USB_PROTOCOL = 3;

    /**
    * 功能：得到协议对应的设备对象
    * @return DeviceBase* 设备对象指针
    */
    DeviceBase* getDevice() 
    {
        return pDevice;
    }

    /**
    * 功能：发送数据包
    * @param  Packet* 数据包指针
    * @return true表示发送成功,false表示发送失败
    */
    virtual int putpkt(T_CHAR* pack,T_WORD size) = 0;

    /**
    * 功能：得到数据包
    * @param  Packet* 返回数据包
    * @return true表示发送成功,false表示发送失败
    */
    virtual int getpkt(T_CHAR* pack,T_WORD size) = 0;

    /**
    * 功能：释放对象对应的资源
    */
    virtual void close();

    /**
    * 功能：构造函数,设置协议对应的设备
    */
    ProtocolBase(DeviceBase* pDevice) ;

    /**
    * 功能：释放对象对应的资源
    */
    virtual ~ProtocolBase()
    {
    }

};

class PacketProtocol : public ProtocolBase
{
    bool isPutEnd;            //发送结束的标记,对本协议主要的起到时候收到确认包信息,false表示没有收到,true表示收到
    bool isExit;            //退出标志
    waitCondition *surePacket;    //确认回复包
    TMutex *mutex;                //对缓冲区进行锁定,
    waitCondition *pWaitCondition;    //确认回复包
    TMutex *waitMutex;                //对缓冲区进行锁定,
    int timeout;                //等待确认包超时时间
    bool isAck;                    //是否收到确认包,初始化时为ture,每发送一次设置为false,收到回复包设置为true
    int retry;            //重试次数

    /**
    * 功能：发送确认包
    */
    //void sendAck() ;
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
    virtual int putpkt(T_CHAR* pack,T_WORD size) ;

    /**
    * 功能：发送数据包
    * @param  Packet* 数据包指针
    * @return true表示发送成功,false表示发送失败
    */
    virtual int getpkt(T_CHAR* pack,T_WORD size) ;

    /**
    * 功能：释放对象对应的资源
    */
    virtual void close();

    /**
    * 功能：释放对象对应的资源
    */
    ~PacketProtocol();
};


class RapidProtocol : public ProtocolBase 
    {
public:
    RapidProtocol(DeviceBase* pDevice);

    /**
    * 功能：发送数据包
    * @param  Packet* 数据包指针
    * @return true表示发送成功,false表示发送失败
    */
    virtual int putpkt(T_CHAR* pack,T_WORD size) ;
    /**
    * 功能：发送数据包
    * @param  Packet* 数据包指针
    * @return true表示发送成功,false表示发送失败
    */
    virtual int getpkt(T_CHAR* pack,T_WORD size) ;

    /**
    * 功能：释放对象对应的资源
    */
    virtual ~RapidProtocol()
    {
    }
};

#endif