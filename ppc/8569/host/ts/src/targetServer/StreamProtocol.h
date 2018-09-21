/*
* 更改历史：
*  2006-1-11 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  streamProtocol.h
* @brief
*       功能：
*       <li>流协议层</li>
*/
#ifndef STREAMPROTOCOL_H
#define STREAMPROTOCOL_H

/************************头文件********************************/
#pragma once
#include "protocol.h"
/************************宏定义********************************/
/************************类型定义******************************/

class DeviceBase;
class StreamProtocol :
    public ProtocolBase
{
    bool isExit;            //发送函数退出标志
    int txPacketNum;    //当前序列号
    int rxPacketNum;        //接收包序列
    int timeout;                //等待确认包超时时间
    int retry;            //重试次数
    waitCondition surePacket;    //确认回复包
    TMutex mutex;                //对缓冲区进行锁定,
    //    TMutex mutexLock;                //对缓冲区进行锁定,
    waitCondition m_waitCondition;    //确认回复包
    TMutex waitMutex;                //对缓冲区进行锁定,
    TMutex putMutex;                //发送函数互斥
    volatile int  waiting_for_ack;            //得到确认包
public:
    virtual ~StreamProtocol(void);
    /* 功能：发送数据包
    * @param  Packet* 数据包指针
    * @return true表示发送成功,false表示发送失败
    */
    virtual int putpkt(Packet* pack) ;
    /**
    * 功能：得到数据包
    * @param  Packet* 返回数据包
    * @return true表示发送成功,false表示发送失败
    */
    virtual int getpkt(Packet* pack);
    /**
    * 功能：释放对象对应的资源
    */
    virtual void close();
    /**
    * 功能：构造函数,设置协议对应的设备
    */
    StreamProtocol(DeviceBase* pDevice);
private:
    void DataError(int type);

    /**
    * 功能：发送控制包
    * @param  control ---待发送的控制信号
    * @return true表示发送成功,false表示发送失败
    */
    void sendCtrlPack(char control,int serialNum);
    /**
    * 功能：发包后等待host的响应，并判断是否需要重发。
    * @return 如果需要重发则返回true，否则返回false。
    */
    bool NeedSendAgain(void);

    /**
    * 功能：接收控制包
    * @return 返回控制信号
    */
    char GetCtrlPack(void);
/************************接口声明******************************/

};
#endif