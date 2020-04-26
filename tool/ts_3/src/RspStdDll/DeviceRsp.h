/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
* @file 	DeviceRsp.h
* @brief 	
*     <li>功能： 定义tcp,udp通信方式的设备对象的类</li>
* @author 	唐兴培
* @date 	2007-10-23
* <p>部门：系统部 
*/

#ifndef _DEVICERSP_H_
#define _DEVICERSP_H_

//#include "stdafx.h"
//#include "tsinclude.h"
//#include <winsock.h>
#include "windows.h"
#include "wsocket.h"
//#include <QString>
#include "common.h"

#define IP_LEN  16

class DeviceBase
{
private:
public:
    virtual ~DeviceBase(){}

    /**
     * 功能：打开具体设备
     * @return true表示成功,false表示失败
     */
    virtual bool open(char *ip ,int port ,int timeout ,int retry) = 0;

    /**
     * 功能：关闭具体设备
     * @return true表示成功,false表示失败
     */
    virtual bool close() = 0;

    /**
     * 功能：从设备中读取信息
     * @param  pBuff 得到信息缓冲区
     * @param  size  缓冲区的大小
     * @param  timeout 等待时间,-1表示无穷等待,
     * @return 从设备中得到的数据大小
     */
    virtual int  read(char* pBuff, int size) = 0;

    /**
     * 功能：向设备发送数据
     * @param  pBuff 发送信息包
     * @param  size 信息包大小
     */
    virtual void write(char* pBuff, int size) = 0;

    /**
     * 功能：检查设备连接情况
     * @return  true表示连接成功
     */
    virtual bool connect() = 0;
};

class DeviceUdp : public DeviceBase
{
    int port;                            //UDP端口号
    char IP[IP_LEN];                     //UDP的ip地址
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

    DeviceUdp();

    ~DeviceUdp();
    /**
    * 通过配置信息打开设备.
    * @param *inIp 接收方IP地址
    * @param inPort 接收方端口
    * @param inTimeout 超时时间
    * @param inRetry 重试次数
    * @return true打开成功,false打开失败
    */
    virtual bool open(char *ip ,int port ,int timeout ,int retry);
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

class DeviceTcp : public DeviceBase
{
private:
    //FOR WINDOW
    //bool connectServer(ServerConfig* config) ;
    WSocket        m_socket;
    //END
    int port;                            //TCP端口号
    char IP[IP_LEN];                     //TCP的ip地址
    //int socketDescriptor;              //连接的socket句柄,目前没有什么用处
public:

    DeviceTcp()
    {
        port = 0;
        memset(IP,0,sizeof(IP));
    }

    ~DeviceTcp();

    /**
    * 创建一个服务器端socket.
    * @param port 端口号
    */
    bool createServerSocket(int port);

    /**
    * 作为服务端等待客户端连接,堵塞等待连接
    * @param s 连接后的对象
    */
    bool acceptSocket(WSocket& s) ;

    /**
    * 设置socket号.
    * @param socketDescriptor socket号
    */
    void setTcpSocket(int socketDescriptor) ;

    /**
    * 通过配置信息打开设备.
    * @param *inIp 接收方IP地址
    * @param inPort 接收方端口
    * @param inTimeout 超时时间
    * @param inRetry 重试次数
    * @return true打开成功,false打开失败
    */
    virtual bool open(char *inIp ,int inPort ,int timeout ,int retry);

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
    virtual int  read(char* pBuff, int size) 
    {
        int ret = m_socket.Recv(pBuff,size,0);
        sysLoger(LDEBUG, "DeviceTCP::read: Recieved packet, size:%d", ret);
        return ret;
    }

    /**
    * 向设备写数据,当没有写完size数据一直等待
    * @param pBuff 写数据的缓冲区
    * @param size 写数据的大小
    */
    virtual void write(char* pBuff, int size) 
    {
        int ret = m_socket.Send(pBuff,size);
        sysLoger(LDEBUG, "DeviceTCP::write: Sended packet, size:%d", ret);
    }

    /**
    * 重新连接socket
    */
    virtual bool connect();
};



#endif