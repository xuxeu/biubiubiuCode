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
* @file     protocol.h
* @brief     TS协议抽象层
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/

#ifndef PROTOCOL_INCLUDE
#define PROTOCOL_INCLUDE


#include "common.h"
#include "Device.h"

class DeviceBase;
class ProtocolBase {
private:
    DeviceBase* pDevice;
public:
    static const int TCP_PROTOCOL = 0;
    static const int PACKET_PROTOCOL = 1;
    static const int STREAM_PROTOCOL = 2;
    static const int USB_PROTOCOL = 3;
    /**
    * 功能：通过缓冲区设置pack的包头
    * @param  pBuff 保存包头的缓冲区数据,至少要大于包头的大小
    * @param  pack[OUT] 设置包头
    */
    void readPacketHead(char* pBuff,Packet* pack) ;
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
    virtual int putpkt(Packet* pack) = 0;
    /**
    * 功能：得到数据包
    * @param  Packet* 返回数据包
    * @return true表示发送成功,false表示发送失败
    */
    virtual int getpkt(Packet* pack) = 0;
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

class RapidProtocol : public ProtocolBase 
    {
public:
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
    RapidProtocol(DeviceBase* pDevice);
    /**
    * 功能：释放对象对应的资源
    */
    virtual ~RapidProtocol(){
    }
};

/*USB设备所对应的协议*/
class UsbProtocol : public ProtocolBase 
{
public:
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
    UsbProtocol(DeviceBase* pDevice);
    /**
    * 功能：释放对象对应的资源
    */
    virtual ~UsbProtocol()
    {
    }
};

/**
* 模块：  协议抽象层
* 功能：  协议管理
*/
class ProtocolManage {
private:
    static ProtocolManage* pProtocolManage;
    static TMutex mutex;                        //互斥对象
public:
    static ProtocolManage* getInstance()
    {
        if (pProtocolManage == NULL) 
        {
            TMutexLocker lock(&mutex);
            if (pProtocolManage == NULL)
                pProtocolManage = new ProtocolManage(); 
        }

        return pProtocolManage;
    }
    /**
    * 功能：提供创建协议的方法
    * @param  协议类型
    * @param  协议对应的设备对象指针
    * @return 一个具体协议
    */
    static ProtocolBase* createProtocol(int type,DeviceBase* pDevice);
};

#endif







