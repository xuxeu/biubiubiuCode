/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  tcpDevice.cpp
* @brief
*       功能：
*       <li> tcp方式的设备对象</li>
*/


/************************头文件********************************/

#include "tcpDevice.h"
#include "common.h"
/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/
///////////////////////////////////////////////DeviceTcp
/**
* 通过配置信息打开设备.
* @param ServerConfig* 配置信息
* @return true打开成功,false打开失败
*/
bool DeviceTcp::open(ServerConfig* config)
{
    if(NULL == config)
    {
        sysLoger(LWARNING, "DeviceTcp::open: No usable Server config!");
        return false;
    }

    TcpConfig* pTcpconfig = (TcpConfig*)config->getDeviceConfig();
    IP = pTcpconfig->getIP().toAscii().data();
    port = pTcpconfig->getPort();
    m_socket.Create(AF_INET, SOCK_STREAM, 0);

    if (m_socket.Connect(IP.toAscii().constData() , port))
    {
        return true;
    }

    else
    {
        close();
        return false;
    }

}

/**
* 关闭设备.
* @return true关闭成功,false关闭失败
*/
bool DeviceTcp::close()
{
    /*Modified by tangxp for BUG NO.0002553 on 2008.1.15 [begin]*/
    /*  Modified brief: 修改socket描述符判断*/
    if (m_socket.m_sock != INVALID_SOCKET)
        /*Modified by tangxp for BUG NO.0002553 on 2008.1.15 [end]*/
    {
        if (m_socket.Close() == 0)
        {
            return false;
        }

        else
        {
            m_socket.m_sock = INVALID_SOCKET;
            return true;
        }
    }

    return true;
}

bool DeviceTcp::connect()
{
    return false;
}

DeviceTcp::~DeviceTcp()
{
    close();
}

/**
* 设置socket号.
* @param socketDescriptor socket号
*/
void DeviceTcp::setTcpSocket(int socketDescriptor)
{
    m_socket.m_sock = socketDescriptor;
}

//创建服务端socket
bool DeviceTcp::createServerSocket(int port)
{
    return m_socket.createServerSocket(port);

}

bool DeviceTcp::acceptSocket(WSocket& s)
{
    return m_socket.acceptSocket(s);
}

//////////////////////////
