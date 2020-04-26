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
 * @file     tcpDevice.h
 * @brief     tcp方式的设备对象
 * @author     彭宏
 * @date     2004-12-6
 * @version 3.0
 * @warning 严禁将本文件到处分发
 *
 * 部门：系统部 
 *
 */

#ifndef TCPDEVICE_INCLUDE
#define TCPDEVICE_INCLUDE


#include "device.h"
#include "tsInclude.h"
//#include "log.h"
#include "../include/wsocket.h"
class DeviceTcp : public DeviceBase
{
//    Q_OBJECT
private:
    //FOR WINDOW
    bool connectServer(ServerConfig* config) ;
    WSocket        m_socket;
    //END
    int port;                            //TCP端口号
    QString IP;                            //TCP的ip地址
    //int socketDescriptor;                //连接的socket句柄,目前没有什么用处
public:

    DeviceTcp()
    {
        port = 0;
        IP = QString("");
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
