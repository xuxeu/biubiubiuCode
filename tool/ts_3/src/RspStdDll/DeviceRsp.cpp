/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
* @file 	DeviceRsp.cpp
* @brief 	
*     <li>功能： 实现tcp,udp通信方式的设备</li>
* @author 	唐兴培
* @date 	2007-10-23
* <p>部门：系统部 
*/

//#include "stdafx.h"
#include "DeviceRsp.h"
//#include "log.h"

//**************************************UDP设备**********************************************//
DeviceUdp::DeviceUdp(void)
{
#ifdef WIN32
    WSADATA wsaData;
    WORD version = MAKEWORD(2, 0);
    int ret = WSAStartup(version, &wsaData);
    if (ret) 
    {
        sysLoger(LWARNING, "DeviceUdp::Initialized the socket failure! FILE:%s, LINE:%d", 
                                            __MYFILE__, __LINE__);
    }
#endif
    pUdpSocket = INVALID_SOCKET;
}

DeviceUdp::~DeviceUdp()
{
    if(pUdpSocket != INVALID_SOCKET)
    {
        closesocket( pUdpSocket);
        pUdpSocket = INVALID_SOCKET;
    }
}
/**
* 通过参数信息打开设备.
* @param *inIp 接收方IP地址
* @param inPort 接收方端口
* @param inTimeout 超时时间
* @param inRetry 重试次数
* @return true打开成功,false打开失败
*/
bool DeviceUdp::open(char *inIp ,int inPort ,int inTimeout ,int inRetry)
{
    if(NULL == inIp)
    {
        sysLoger(LWARNING, "DeviceUdp::open: IP is null, can't get IP from it!");
        return false;
    }
    if (pUdpSocket != INVALID_SOCKET)
    {
        closesocket( pUdpSocket);
    }
    
    memcpy(IP,inIp,IP_LEN);
    port = inPort;
    timeout = inTimeout;
    retry = inRetry;

    pUdpSocket=socket(AF_INET,SOCK_DGRAM,0);    //创建数据包形式
    if(pUdpSocket<0) 
    {    
        sysLoger(LWARNING, "DeviceUdp::open: Created the DGRAM socket failure!");
        return false;
    } 
    memset(&localAddr,0,sizeof(sockaddr_in)); 
    localAddr.sin_family=AF_INET; 
    localAddr.sin_addr.s_addr = INADDR_ANY;
    //sockaddrin.sin_addr.s_addr = inet_addr(IP.toAscii());    
    localAddr.sin_port=htons(0);     
    //绑定本机地址,UDP作为客户端不需要绑定端口
    if (bind(pUdpSocket,(sockaddr *)&localAddr,sizeof(sockaddr_in)) == SOCKET_ERROR) 
    {
        sysLoger(LWARNING, "DeviceUdp::open: Bind the socket to local machine failure!");
        close(); 
        return false;
    }
    memset(&addr,0,sizeof(sockaddr_in)); 
    addr.sin_family=AF_INET; 
    addr.sin_addr.s_addr = inet_addr(IP);    
    //绑定本机地址,UDP作为客户端不需要绑定端口
    addr.sin_port=htons(port); 
    
    sysLoger(LDEBUG, "DeviceUdp::open: Open UDP device, IP:%s, Port:%d, Timeout:%d, Retry:%d",
                                   IP, port, timeout, retry);
    return true;
}
/**
* 关闭设备.
* @return true关闭成功,false关闭失败
*/
bool DeviceUdp::close()
{
    if (pUdpSocket != INVALID_SOCKET)
    {
        closesocket(pUdpSocket);
        pUdpSocket = INVALID_SOCKET;
    }
    return true;
}

/**
* 从设备中读取数据.,当没有读取size数据一直等待
* @param pBuff 读取数据填入的缓冲区
* @param size 要读取数据的大小
* @return int读取的数量
*/
int  DeviceUdp::read(char* pBuff, int size)
{
    int recSize = 0;
    sockaddr_in sockAddr;
    int fromlen =sizeof(sockaddr_in);    
    if(NULL == pBuff)
    {
        sysLoger(LWARNING, "DeviceUdp::read:Bad received data pointer, exit read!");
        return -1;
    }
    if(size <= 0)
    {
        sysLoger(LWARNING, "DeviceUdp::read:Invalid received data size [%d], exit read!", size);
        return -1;
    }
    recSize = recvfrom(pUdpSocket,pBuff,size,0,(struct sockaddr*)&sockAddr,&fromlen);

    if (recSize == -1)
    {
        int error = WSAGetLastError();
        sysLoger(LWARNING, "DeviceUdp::read: Read the data error, errorNo: %d.", 
                                            error);
        if (error == 10040)
            return -error;    //接收的数据包大小小于这次消息的大小
        return -1;
    }

    return recSize;
    
}

/**
* 向设备写数据,当没有写完size数据一直等待
* @param pBuff 写数据的缓冲区
* @param size 写数据的大小
*/
void DeviceUdp::write(char* pBuff, int size) 
{
    int sendSize = 0;

    if(NULL == pBuff)
    {
        sysLoger(LWARNING, "DeviceUdp::write: Bad pointer of pBuff, exit write!");
        return;
    }
    if(size <= 0)
    {
        sysLoger(LWARNING, "DeviceUdp::write: Invalid data size [%d] for write, exit write!", size);
        return;
    }
    
    sendSize = sendto(pUdpSocket,pBuff,size,0,(struct sockaddr*)&addr,sizeof( sockaddr_in));

    if (sendSize == -1) 
    {
        int error = WSAGetLastError();
        sysLoger(LWARNING, "DeviceUdp::write: Write the data error, errorNo: %d. detial config.."\
                                            "IP:%s, Port:%d, TimeOut:%d, Retry:%d", 
                                            error, IP, port, timeout, retry);
        if (error == 10040)
            return ;    //发送的数据包的大小大于缓冲包的默认大小
        return ;
    }
    
    return ;
}

/**
* 重新连接socket
*/
bool DeviceUdp::connect() 
{
    return true;
}

//***********************************************TCP设备******************************************************//

/**
* 通过配置信息打开设备.
* @param *inIp 接收方IP地址
* @param inPort 接收方端口
* @param inTimeout 超时时间
* @param inRetry 重试次数
* @return true打开成功,false打开失败
*/
bool DeviceTcp::open(char *inIp ,int inPort,int timeout ,int retry) 
{
    if(NULL == inIp)
    {
        sysLoger(LWARNING, "DeviceTcp::open: No usable Server config!");
        return false;
    }
    memcpy(IP,inIp,IP_LEN);
    port = inPort;
    m_socket.Create(AF_INET, SOCK_STREAM, 0);
    if (m_socket.Connect(IP, port))
        return true;
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
bool DeviceTcp::close() {
    if (m_socket != INVALID_SOCKET)
    {
        if (m_socket.Close() == 0)
            return false;
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
