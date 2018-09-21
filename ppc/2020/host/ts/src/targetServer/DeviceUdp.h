/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  deviceUdp.h
* @brief
*       功能：
*       <li>udp方式的设备对象</li>
*/



#ifndef DEVICE_UDP_INCLUDE
#define DEVICE_UDP_INCLUDE
/************************头文件********************************/
#include "device.h"

#include "tsInclude.h"
#include <winsock.h>

/************************宏定义********************************/
/************************类型定义******************************/
class QUdpSocket;
typedef int                socklen_t;

class DeviceUdp : public DeviceBase
{
    int port;                            //UDP端口号
    QString IP;                            //UDP的ip地址
    waitCondition surePacket;            //添加变量,用来判断是否接收到ACK包
    TMutex mutex;                        //对缓冲区进行锁定,主要是配合条件变量使用
    int timeout;                        //超时时间,单位毫秒
    int retry;                            //重试次数
    struct sockaddr_in      addr;        //要发送的地址
    struct sockaddr_in      localAddr;    //本机地址
    SOCKET pUdpSocket;
public:
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
    DeviceUdp(void);
    virtual ~DeviceUdp(void);
    /**
    * 通过配置信息打开设备.
    * @param ServerConfig* 配置信息
    * @return true打开成功,false打开失败
    */
    virtual bool open(ServerConfig* config);
    /**
    * 关闭设备.
    * @return true关闭成功,false关闭失败
    */
    virtual bool close();

    /**
    * 从设备中读取数据.,当没有读取size数据一直等待
    * @param pBuff 读取数据填入的缓冲区
    * @param size 要读取数据的大小
    * @param timeout 等待时间
    * @return int读取的数量
    */
    virtual int  read(char* pBuff, int size);
    /**
    * 向设备写数据,当没有写完size数据一直等待
    * @param pBuff 写数据的缓冲区
    * @param size 写数据的大小
    */
    virtual void write(char* pBuff, int size) ;
    /**
    * 重新连接socket,目前还没有实现
    */
    virtual bool connect();
};
/************************接口声明******************************/
#endif //DEVICE_UDP_INCLUDE
