
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
* @file     config.h
* @brief     TS的配置对象
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/

#ifndef CONFIG_INCLUDE 
#define CONFIG_INCLUDE

#include "tsInclude.h"
#include "OwnerArchive.h"

///////////////////////////////设备的配置信息 
class DeviceConfigBase 
{
private:
    int  deviceType;    //通信方式,对应的设备的类型
//protected:
//    DeviceConfigBase(int deviceType);
public:
    DeviceConfigBase(int deviceType);
    ~DeviceConfigBase()
    {
        deviceType = 0;
    }
    //通信方式
    static const int TCP_DEVICE = 0;        //TCP方式
    static const int UDP_DEVICE = 1;        //UDP方式
    static const int UART_DEVICE = 2;       //串口方式
    static const int USB_DEVICE = 3;        //USB方式
    static const int DLL_DEVICE = 4;        //动态库方式

    int getDeviceType() 
    {
        return deviceType;
    }
    void setDeviceType(int deviceType) 
    {
        this->deviceType = deviceType;
    }
    /**
    * 功能：根据设备类型创建相应的设备配置信息
    * @param type 设备类型
    * @return DeviceConfigBase* 具体设备配置
    */
    static DeviceConfigBase* newDeviceConfig(int type);

    /**
    * 功能：将配置信息序列化到OwnerArchive对象
    */
    virtual void serialize(OwnerArchive& ar);
};


/////////////////tcp通信方式类/////////////////////
class TcpConfig : public DeviceConfigBase 
{
private:
    int port;        //端口
    QString IP;        //IP地址
public:
    int getPort() 
    {
        return port;
    }
    QString getIP() 
    {
        return IP;
    }
    void setPort(int port) 
    {
        this->port = port;
    }
    void setIP(const QString& IP) 
    {
        this->IP = IP;
    }
    /**
    * 功能：将配置信息序列化到OwnerArchive对象
    */
    virtual void serialize(OwnerArchive& ar);
    TcpConfig(int deviceType,int port,QString IP);
    TcpConfig(int deviceType=DeviceConfigBase::TCP_DEVICE);
    ~TcpConfig()
    {
        IP.clear();
    }
};

/////////////////udp通信方式类/////////////////////

class UdpConfig : public DeviceConfigBase 
{
private:
    int port;        //端口
    QString IP;        //IP地址
    int            timeout;        //超时
    int            retry;            //重试次数
public:
    int getPort() 
    {
        return port;
    }
    QString getIP()
    {
        return IP;
    }
    void setPort(int port) 
    {
        this->port = port;
    }
    void setIP(const QString& IP) 
    {
        this->IP = IP;
    }
    /**
    * 功能：得到超时
    */
    int getTimeout() 
    {
        return timeout;
    }
    /**
    * 功能：设置超时
    */
    void setTimeout(int timeout) 
    {
        this->timeout = timeout;
    }

    /**
    * 功能：得到重试次数
    */
    int getRetry() 
    {
        return retry;
    }
    /**
    * 功能：设置重试次数
    */
    void setRetry(int retry) 
    {
        this->retry = retry;
    }
    /**
    * 功能：将配置信息序列化到OwnerArchive对象
    */
    virtual void serialize(OwnerArchive& ar);//序列化
    UdpConfig(int deviceType,int port,QString IP,int timeout=5000,int retry=3);
    UdpConfig(int deviceType=DeviceConfigBase::UDP_DEVICE);
    ~UdpConfig()
    {
        IP.clear();
    }
};


/////////////////串口通信方式类/////////////////////
class UartConfig : public DeviceConfigBase 
{
private:
    QString        com;            //串口号1~255
    int            baud;            //波特率
    int            timeout;        //超时
    int            retry;            //重试次数
public:
    /**
    * 功能：将配置信息序列化到COwnerArchive对象
    */
    virtual void serialize(OwnerArchive& ar);

    UartConfig(int deviceType=DeviceConfigBase::UART_DEVICE, QString port="COM1",
                int baud=9600,int timeout=5000,int retry=3);
    ~UartConfig()
    {
        com.clear();
    }
    /**
    * 功能：得到端口号
    */
    QString getCom() 
    {
        return com;
    }
    /**
    * 功能：设置端口号
    */
    void setCom(QString com) 
    {
        this->com = com;
    }

    /**
    * 功能：得到波特率
    */
    int getBaud() 
    {
        return baud;
    }
    /**
    * 功能：设置波特率
    */
    void setBaud(int baud)
    {
        this->baud = baud;
    }

    /**
    * 功能：得到超时
    */
    int getTimeout()
    {
        return timeout;
    }
    /**
    * 功能：设置超时
    */
    void setTimeout(int timeout) 
    {
        this->timeout = timeout;
    }

    /**
    * 功能：得到重试次数
    */
    int getRetry()
    {
        return retry;
    }
    /**
    * 功能：设置重试次数
    */
    void setRetry(int retry) 
    {
        this->retry = retry;
    }

};

/*************************USB 通信方式类****************************/
class USBConfig : public DeviceConfigBase 
{
private:
    GUID    m_guid;
    QString m_PipeIn;
    QString m_PipeOut;
    
public:
    USBConfig(QString pipeNameIn, QString pipeNameOut, 
        GUID guid = LambdaICE_GUID): DeviceConfigBase(USB)
    {
        m_PipeIn = pipeNameIn;
        m_PipeOut = pipeNameOut;
        m_guid = guid;
    }
    USBConfig(): DeviceConfigBase(USB)
    {
        m_PipeIn = QString::fromLocal8Bit("PIPE00", strlen("PIPE00"));
        m_PipeOut = QString::fromLocal8Bit("PIPE01", strlen("PIPE01"));
    }

    ~USBConfig()
    {}
    void setGUID(GUID guid)
    {
        m_guid = guid;
        return;
    }
    void setGUID(QString strGuid)
    {
        stringToGUID(strGuid.toAscii().data(), strGuid.toAscii().length(), &m_guid);
        return;
    }
    void setPipeInName(QString nameIn)
    {
        m_PipeIn = nameIn;
        return;
    }
    void setPipeOutName(QString nameOut)
    {
        m_PipeOut = nameOut;
        return;
    }
    GUID getGUID()
    {
        return m_guid;
    }
    QString getPipeInname()
    {
        return m_PipeIn;
    }
    QString getPipeOutName()
    {
        return m_PipeOut;
    }
};

////////////////////Server端配置信息//////////////////////////
class ServerConfig 
{
    DeviceConfigBase* pDevice;
    bool isLog;    //是否需要日志记录
    int  protocolType;            //对应的协议 
public:
    ServerConfig(DeviceConfigBase* pDevice = NULL,
                    int protocolType = 0,bool isLog=true);
    ~ServerConfig();
    /**
    * 功能：得到设备类型配置信息
    */
    DeviceConfigBase* getDeviceConfig() 
    {
        return pDevice;
    }
    /**
    * 功能：设置设备类型配置信息
    */
    void setDeviceConfig(DeviceConfigBase* pDevice) ;
    /**
    * 功能：得到是否需要日志记录
    */
    bool getLogFlag() 
    {
        return isLog;
    }
    /**
    * 功能：设置是否需要日志
    */
    void setLogFlag(bool isLog) ;
    /**
    * 功能：协议类型 
    */
    int getProtocolType() ;
    /**
    * 功能：设置协议类型
    */
    void setProtocolType(int protocolType) ;

    /**
    * 功能：得到设备类型
    */
    int getDeviceType() 
    {
        if (pDevice == NULL)    //没有具体的设备则是动态库类型
            return DeviceConfigBase::DLL_DEVICE;
        else
            return pDevice->getDeviceType();
    }
    /**
    * 功能：将配置信息序列化到OwnerArchive对象
    */
    void serialize(OwnerArchive& ar);
};
#endif


