/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  config.h
* @brief
*       ���ܣ�
*       <li>TS�����ö���</li>
*/



#ifndef CONFIG_INCLUDE
#define CONFIG_INCLUDE
/************************ͷ�ļ�********************************/
#include "tsInclude.h"
#include "OwnerArchive.h"

/************************�궨��********************************/
/************************���Ͷ���******************************/

///////////////////////////////�豸��������Ϣ
class DeviceConfigBase
{
private:
    int  deviceType;    //ͨ�ŷ�ʽ,��Ӧ���豸������
public:
    DeviceConfigBase(int deviceType);
    ~DeviceConfigBase()
    {
        deviceType = 0;
    }
    //ͨ�ŷ�ʽ
    static const int TCP_DEVICE = 0;        //TCP��ʽ
    static const int UDP_DEVICE = 1;        //UDP��ʽ
    static const int UART_DEVICE = 2;       //���ڷ�ʽ
    static const int DLL_DEVICE = 4;        //��̬�ⷽʽ

    int getDeviceType()
    {
        return deviceType;
    }
    void setDeviceType(int deviceType)
    {
        this->deviceType = deviceType;
    }
    /**
    * ���ܣ������豸���ʹ�����Ӧ���豸������Ϣ
    * @param type �豸����
    * @return DeviceConfigBase* �����豸����
    */
    static DeviceConfigBase* newDeviceConfig(int type);

    /**
    * ���ܣ���������Ϣ���л���OwnerArchive����
    */
    virtual void serialize(OwnerArchive& ar);
};


/////////////////tcpͨ�ŷ�ʽ��/////////////////////
class TcpConfig : public DeviceConfigBase
{
private:
    int port;        //�˿�
    QString IP;        //IP��ַ
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
    * ���ܣ���������Ϣ���л���OwnerArchive����
    */
    virtual void serialize(OwnerArchive& ar);
    TcpConfig(int deviceType, int port, QString IP);
    TcpConfig(int deviceType = DeviceConfigBase::TCP_DEVICE);
    ~TcpConfig()
    {
        IP.clear();
    }
};

/////////////////udpͨ�ŷ�ʽ��/////////////////////

class UdpConfig : public DeviceConfigBase
{
private:
    int port;        //�˿�
    QString IP;        //IP��ַ
    int            timeout;        //��ʱ
    int            retry;            //���Դ���
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
    * ���ܣ��õ���ʱ
    */
    int getTimeout()
    {
        return timeout;
    }
    /**
    * ���ܣ����ó�ʱ
    */
    void setTimeout(int timeout)
    {
        this->timeout = timeout;
    }

    /**
    * ���ܣ��õ����Դ���
    */
    int getRetry()
    {
        return retry;
    }
    /**
    * ���ܣ��������Դ���
    */
    void setRetry(int retry)
    {
        this->retry = retry;
    }
    /**
    * ���ܣ���������Ϣ���л���OwnerArchive����
    */
    virtual void serialize(OwnerArchive& ar);//���л�
    UdpConfig(int deviceType, int port, QString IP, int timeout = 5000, int retry = 3);
    UdpConfig(int deviceType = DeviceConfigBase::UDP_DEVICE);
    ~UdpConfig()
    {
        IP.clear();
    }
};


/////////////////����ͨ�ŷ�ʽ��/////////////////////
class UartConfig : public DeviceConfigBase
{
private:
    QString        com;            //���ں�1~255
    int            baud;            //������
    int            timeout;        //��ʱ
    int            retry;            //���Դ���
public:
    /**
    * ���ܣ���������Ϣ���л���COwnerArchive����
    */
    virtual void serialize(OwnerArchive& ar);

    UartConfig(int deviceType = DeviceConfigBase::UART_DEVICE, QString port = "COM1",
               int baud = 9600, int timeout = 5000, int retry = 3);
    ~UartConfig()
    {
        com.clear();
    }
    /**
    * ���ܣ��õ��˿ں�
    */
    QString getCom()
    {
        return com;
    }
    /**
    * ���ܣ����ö˿ں�
    */
    void setCom(QString com)
    {
        this->com = com;
    }

    /**
    * ���ܣ��õ�������
    */
    int getBaud()
    {
        return baud;
    }
    /**
    * ���ܣ����ò�����
    */
    void setBaud(int baud)
    {
        this->baud = baud;
    }

    /**
    * ���ܣ��õ���ʱ
    */
    int getTimeout()
    {
        return timeout;
    }
    /**
    * ���ܣ����ó�ʱ
    */
    void setTimeout(int timeout)
    {
        this->timeout = timeout;
    }

    /**
    * ���ܣ��õ����Դ���
    */
    int getRetry()
    {
        return retry;
    }
    /**
    * ���ܣ��������Դ���
    */
    void setRetry(int retry)
    {
        this->retry = retry;
    }

};


////////////////////Server��������Ϣ//////////////////////////
class ServerConfig
{
    DeviceConfigBase* pDevice;
    bool isLog;    //�Ƿ���Ҫ��־��¼
    int  protocolType;            //��Ӧ��Э��
public:
    ServerConfig(DeviceConfigBase* pDevice = NULL,
                 int protocolType = 0, bool isLog = true);
    ~ServerConfig();
    /**
    * ���ܣ��õ��豸����������Ϣ
    */
    DeviceConfigBase* getDeviceConfig()
    {
        return pDevice;
    }
    /**
    * ���ܣ������豸����������Ϣ
    */
    void setDeviceConfig(DeviceConfigBase* pDevice) ;
    /**
    * ���ܣ��õ��Ƿ���Ҫ��־��¼
    */
    bool getLogFlag()
    {
        return isLog;
    }
    /**
    * ���ܣ������Ƿ���Ҫ��־
    */
    void setLogFlag(bool isLog) ;
    /**
    * ���ܣ�Э������
    */
    int getProtocolType() ;
    /**
    * ���ܣ�����Э������
    */
    void setProtocolType(int protocolType) ;

    /**
    * ���ܣ��õ��豸����
    */
    int getDeviceType()
    {
        if (pDevice == NULL)    //û�о�����豸���Ƕ�̬������
        {
            return DeviceConfigBase::DLL_DEVICE;
        }

        else
        {
            return pDevice->getDeviceType();
        }
    }
    /**
    * ���ܣ���������Ϣ���л���OwnerArchive����
    */
    void serialize(OwnerArchive& ar);
};
/************************�ӿ�����******************************/
#endif


