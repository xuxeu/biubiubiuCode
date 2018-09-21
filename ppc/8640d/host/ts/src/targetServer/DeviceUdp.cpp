/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  deviceUdp.cpp
* @brief
*       功能：
*       <li> udp方式的设备对象</li>
*/

/************************头文件********************************/
#include ".\deviceudp.h"
#include "common.h"
/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/
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

DeviceUdp::~DeviceUdp(void)
{
    if(pUdpSocket != INVALID_SOCKET)
    {
        closesocket( pUdpSocket);
        pUdpSocket = INVALID_SOCKET;
    }
}
/**
* 通过配置信息打开设备.
* @param ServerConfig* 配置信息
* @return true打开成功,false打开失败
*/
bool DeviceUdp::open(ServerConfig* config)
{
    if(NULL == config)
    {
        sysLoger(LWARNING, "DeviceUdp::open: Server Config is null, can't get device config from it!");
        return false;
    }

    if (pUdpSocket != INVALID_SOCKET)
    {
        closesocket( pUdpSocket);
    }

    UdpConfig* pUdpconfig = (UdpConfig*)config->getDeviceConfig();

    if(NULL == pUdpconfig)
    {
        sysLoger(LWARNING, "DeviceUdp::open: No usable UDP config!");
        return false;
    }

    IP = pUdpconfig->getIP().toAscii().data();
    port = pUdpconfig->getPort();
    retry = pUdpconfig->getRetry();
    timeout = pUdpconfig->getTimeout();

    pUdpSocket = socket(AF_INET, SOCK_DGRAM, 0); //创建数据包形式

    if(pUdpSocket < 0)
    {
        sysLoger(LWARNING, "DeviceUdp::open: Created the DGRAM socket failure!");
        return false;
    }

    /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [begin]*/
    /*  Modified brief: 更正清空对象为localAddr*/
    memset(&localAddr, 0, sizeof(sockaddr_in));
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [end]*/
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    //sockaddrin.sin_addr.s_addr = inet_addr(IP.toAscii());
    localAddr.sin_port = htons(0);

    //绑定本机地址,UDP作为客户端不需要绑定端口
    if (bind(pUdpSocket, (sockaddr *)&localAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
        sysLoger(LWARNING, "DeviceUdp::open: Bind the socket to local machine failure!");
        close();
        return false;
    }

    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP.toAscii());
    //绑定本机地址,UDP作为客户端不需要绑定端口
    addr.sin_port = htons(port);

    sysLoger(LDEBUG, "DeviceUdp::open: Open UDP device, IP:%s, Port:%d, Timeout:%d, Retry:%d",
             IP.toAscii().data(), port, timeout, retry);
    return true;
}
/**
* 关闭设备.
* @return true关闭成功,false关闭失败
*/
bool DeviceUdp::close()
{
	sysLoger(LWARNING, "DeviceUdp::close");
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
* @param timeout 等待时间
* @return int读取的数量
*/
int  DeviceUdp::read(char* pBuff, int size)
{
    int recSize = 0;
    sockaddr_in sockAddr;
    int fromlen = sizeof(sockaddr_in);

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

    recSize = recvfrom(pUdpSocket, pBuff, size, 0, (struct sockaddr*)&sockAddr, &fromlen);

    if (recSize == -1)
    {
        int error = WSAGetLastError();
        sysLoger(LWARNING, "DeviceUdp::read: Read the data error, errorNo: %d.",
                 error);

        if ((error == 10040) || (error == 10054))
        {
            return -error;    //接收的数据包大小小于这次消息的大小
        }

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

    sendSize = sendto(pUdpSocket, pBuff, size, 0, (struct sockaddr*)&addr, sizeof( sockaddr_in));

    if (sendSize == -1)
    {
        int error = WSAGetLastError();
        sysLoger(LWARNING, "DeviceUdp::write: Write the data error, errorNo: %d. detial config.."\
                 "IP:%s, Port:%d, TimeOut:%d, Retry:%d",
                 error, IP.toAscii().data(), port, timeout, retry);

        if (error == 10040)
        {
            return ;    //发送的数据包的大小大于缓冲包的默认大小
        }

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
