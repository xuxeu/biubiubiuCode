/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  protocol.h
* @brief
*       功能：
*       <li> TS协议抽象层</li>
*/


#ifndef PROTOCOL_INCLUDE
#define PROTOCOL_INCLUDE

/************************头文件********************************/

#include "common.h"
#include "Device.h"
/************************宏定义********************************/
/*Modified by tangxp for BUG NO.3669 on 2008.7.21 [begin]*/
/*  Modified brief: 将通信协议公共宏定义移至Protocl.h,
                            接收序号段改为与发送序号段相同
                            增加TS,TA起始发送,接收序号*/
//同步允许范围
#define SYNC_RANGE 0x1

//接收序列号范围
#define RX_PACKET_MAX 0x3fffffff
#define RX_PACKET_MIN 0x00000001

//发送列号范围
#define TX_PACKET_MAX RX_PACKET_MAX
#define TX_PACKET_MIN RX_PACKET_MIN

//TS起始发送,接收序号
#define TX_START 0x40000001
#define RX_START 0x40000002

//TA起始发送序号
#define TA_TX_START 0x40000001
/*Modified by tangxp for BUG NO.3669 on 2008.7.21 [end]*/


/************************类型定义******************************/
class DeviceBase;
class ProtocolBase
{
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
    virtual ~RapidProtocol()
    {
    }
};



/**
* 模块：  协议抽象层
* 功能：  协议管理
*/
class ProtocolManage
{
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
            {
                pProtocolManage = new ProtocolManage();
            }
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
/************************接口声明******************************/
#endif







