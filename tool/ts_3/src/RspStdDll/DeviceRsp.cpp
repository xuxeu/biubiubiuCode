/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
* @file 	DeviceRsp.cpp
* @brief 	
*     <li>���ܣ� ʵ��tcp,udpͨ�ŷ�ʽ���豸</li>
* @author 	������
* @date 	2007-10-23
* <p>���ţ�ϵͳ�� 
*/

//#include "stdafx.h"
#include "DeviceRsp.h"
//#include "log.h"

//**************************************UDP�豸**********************************************//
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
* ͨ��������Ϣ���豸.
* @param *inIp ���շ�IP��ַ
* @param inPort ���շ��˿�
* @param inTimeout ��ʱʱ��
* @param inRetry ���Դ���
* @return true�򿪳ɹ�,false��ʧ��
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

    pUdpSocket=socket(AF_INET,SOCK_DGRAM,0);    //�������ݰ���ʽ
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
    //�󶨱�����ַ,UDP��Ϊ�ͻ��˲���Ҫ�󶨶˿�
    if (bind(pUdpSocket,(sockaddr *)&localAddr,sizeof(sockaddr_in)) == SOCKET_ERROR) 
    {
        sysLoger(LWARNING, "DeviceUdp::open: Bind the socket to local machine failure!");
        close(); 
        return false;
    }
    memset(&addr,0,sizeof(sockaddr_in)); 
    addr.sin_family=AF_INET; 
    addr.sin_addr.s_addr = inet_addr(IP);    
    //�󶨱�����ַ,UDP��Ϊ�ͻ��˲���Ҫ�󶨶˿�
    addr.sin_port=htons(port); 
    
    sysLoger(LDEBUG, "DeviceUdp::open: Open UDP device, IP:%s, Port:%d, Timeout:%d, Retry:%d",
                                   IP, port, timeout, retry);
    return true;
}
/**
* �ر��豸.
* @return true�رճɹ�,false�ر�ʧ��
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
* ���豸�ж�ȡ����.,��û�ж�ȡsize����һֱ�ȴ�
* @param pBuff ��ȡ��������Ļ�����
* @param size Ҫ��ȡ���ݵĴ�С
* @return int��ȡ������
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
            return -error;    //���յ����ݰ���СС�������Ϣ�Ĵ�С
        return -1;
    }

    return recSize;
    
}

/**
* ���豸д����,��û��д��size����һֱ�ȴ�
* @param pBuff д���ݵĻ�����
* @param size д���ݵĴ�С
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
            return ;    //���͵����ݰ��Ĵ�С���ڻ������Ĭ�ϴ�С
        return ;
    }
    
    return ;
}

/**
* ��������socket
*/
bool DeviceUdp::connect() 
{
    return true;
}

//***********************************************TCP�豸******************************************************//

/**
* ͨ��������Ϣ���豸.
* @param *inIp ���շ�IP��ַ
* @param inPort ���շ��˿�
* @param inTimeout ��ʱʱ��
* @param inRetry ���Դ���
* @return true�򿪳ɹ�,false��ʧ��
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
* �ر��豸.
* @return true�رճɹ�,false�ر�ʧ��
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
* ����socket��.
* @param socketDescriptor socket��
*/
void DeviceTcp::setTcpSocket(int socketDescriptor) 
{
    m_socket.m_sock = socketDescriptor;
}

//���������socket
bool DeviceTcp::createServerSocket(int port) 
{
    return m_socket.createServerSocket(port);

}

bool DeviceTcp::acceptSocket(WSocket& s) 
{
    return m_socket.acceptSocket(s);
}
