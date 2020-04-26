/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  tcpDevice.h
 * @Version        :  1.0.0
 * @Brief           :  封装TCP通信方式,对外提供简洁的读写接口
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月9日 15:36:20
 * @History        : 
 *   
 */

#ifndef _TCP_DEVICE_H_
#define _TCP_DEVICE_H_

#include "wsocket.h"
#include "log.h"
#include "packet.h"

class tcpDevice
{
    private:
        WSocket        m_socket;
        QMutex          m_mtxSocket;
        int                  m_port;
        QString          m_IP;
    public:

        tcpDevice()
        {
            m_IP.clear();
            m_port = 0;
        }
        tcpDevice(QString IP, int port);
        ~tcpDevice();
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
        bool open(QString IP, int port);
        /**
        * 关闭设备.
        * @return true关闭成功,false关闭失败
        */
        bool close();

        /**
        * 从设备中读取数据.,当没有读取size数据一直等待
        * @param pBuff 读取数据填入的缓冲区
        * @param size 要读取数据的大小
        * @param timeout 等待时间
        * @return int读取的数量
        */
        int  read(Packet *pack) ;

        /**
        * 向设备写数据,当没有写完size数据一直等待
        * @param pBuff 写数据的缓冲区
        * @param size 写数据的大小
        */
        int write(Packet *pack) ;
            
};

#endif/*_TCP_DEVICE_H_*/
