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
 * @file     deviceUdp.h
 * @brief     udp方式的设备对象
 * @author     彭宏
 * @date     2004-12-6
 * @version 3.0
 * @warning 严禁将本文件到处分发
 *
 * 部门：系统部 
 *
 */

#ifndef DEVICE_UDP_INCLUDE
#define DEVICE_UDP_INCLUDE

#include "device.h"
class QUdpSocket;
#include "tsInclude.h"
#include <winsock.h>
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

#endif //DEVICE_UDP_INCLUDE
