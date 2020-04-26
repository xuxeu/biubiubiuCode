
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
* @file     wsock.cpp
* @brief     tcpʵ����, window��
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/


#include <iostream>
using namespace std;
#include "wsocket.h"


#ifdef WIN32
    #pragma comment(lib, "wsock32")
#endif
using namespace std;

WSocket::WSocket(SOCKET sock)
{
    m_sock = sock;
    Init();
    localPort = 0;
}

WSocket::~WSocket()
{
}

int WSocket::Init()
{
#ifdef WIN32
    WSADATA wsaData;
    memset(&wsaData, 0, sizeof(wsaData));
    WORD version = MAKEWORD(2, 0);
    int ret = WSAStartup(version, &wsaData);
    if ( ret ) 
    {
        cerr << "Initilize winsock error !" << endl;
        return -1;
    }
#endif
    
    return 0;
}

int WSocket::Clean()
{
#ifdef WIN32
        return (WSACleanup());
#endif
        return 0;
}

WSocket& WSocket::operator = (SOCKET s)
{
    m_sock = s;
    return (*this);
}

WSocket::operator SOCKET ()
{
    return m_sock;
}

bool WSocket::Create(int af, int type, int protocol)
{
    m_sock = socket(af, type, protocol);
    if ( m_sock == INVALID_SOCKET )
    {
        return false;
    }
    return true;
}

bool WSocket::Connect(const char* ip, unsigned short port)
{

    //����������ʽ����
/*    unsigned long ul = 0;
    if(SOCKET_ERROR == ioctlsocket(m_sock, FIONBIO, (unsigned long*)&ul))
        return false;

    struct sockaddr_in svraddr;
    svraddr.sin_family = AF_INET;
    svraddr.sin_addr.s_addr = inet_addr(ip);
    svraddr.sin_port = htons(port);
    int ret = connect(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));
    if ( ret == SOCKET_ERROR ) {
        return false;
    }

    int iFlag = 1;//���õ�ַ����
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iFlag, sizeof(iFlag)))
        return false;

    int bNodelay = 1; 
    int err; 
    err = setsockopt( m_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&bNodelay, sizeof(bNodelay));//��������ʱ�㷨 
    if (err != NO_ERROR)
        return false;*/


    unsigned long ul = 1;    //�Ƕ�����ʽ
    if(SOCKET_ERROR == ioctlsocket(m_sock, FIONBIO, (unsigned long*)&ul))
        return false;

    struct sockaddr_in svraddr;
    svraddr.sin_family = AF_INET;
    svraddr.sin_addr.s_addr = inet_addr(ip);
    svraddr.sin_port = htons(port);
    int ret = connect(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));

    struct timeval overtime;
    overtime.tv_sec = 0;    //���ӳ�ʱ����Ϊ1��
    overtime.tv_usec = 500;
    FD_SET RSet;
    FD_SET WSet;
    FD_ZERO(&RSet);
    FD_ZERO(&WSet);
    FD_SET(m_sock, &RSet);
    FD_SET(m_sock, &WSet);

    ret = select(0, &RSet, &WSet, NULL, &overtime);
    if ( ret == 0 ) 
    {
        return false ;        //��ʱ
    }
    else if (ret < 0)    //socket�ر�
        return false;

    int iFlag = 1;//���õ�ַ����
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iFlag, sizeof(iFlag)))
        return false;

    int bNodelay = 1; 
    int err; 
    err = setsockopt( m_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&bNodelay, sizeof(bNodelay));//��������ʱ�㷨 
    if (err != NO_ERROR)
        return false;
    //���÷��ͻ���Ĵ�С(TS��25�����Ĵ�С[4K/��])
    int bufsize = 100*1024;
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize)))
    {
        return false;
    }
    //���ý��ջ���Ĵ�С(TS��25�����Ĵ�С[4K/��])
    bufsize = 100*1024;
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&bufsize, sizeof(bufsize)))
    {
        return false;
    }
    //���÷��ͳ�ʱʱ��(5��)
    int timeout = 5*1000;
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)))
    {
        return false;
    }
    ul = 0;    //������ʽ
    if(SOCKET_ERROR == ioctlsocket(m_sock, FIONBIO, (unsigned long*)&ul))
        return false;

    return true;
}

bool WSocket::Bind(unsigned short port)
{
    struct sockaddr_in svraddr;
    svraddr.sin_family = AF_INET;
    svraddr.sin_addr.s_addr = INADDR_ANY;
    svraddr.sin_port = htons(port);

    int opt =  1;
    if ( setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0 ) 
        return false;

    int ret = bind(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));
    if ( ret == SOCKET_ERROR ) 
    {
        return false;
    }

    SOCKADDR_IN sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    int nSockAddrLen = sizeof(sockAddr);
    if (getsockname(m_sock, (SOCKADDR*)&sockAddr, &nSockAddrLen))
        return false;

    if(port == 0)
        localPort = sockAddr.sin_port;    //���汾�ض˿�

    return true;
}

bool WSocket::Listen(int backlog)
{
    int ret = listen(m_sock, backlog);
    if ( ret == SOCKET_ERROR ) 
    {
        return false;
    }
    return true;
}

bool WSocket::Accept(WSocket& s, char* fromip)
{
    struct sockaddr_in cliaddr;
    socklen_t addrlen = sizeof(cliaddr);
    int iFlag = 1;//���õ�ַ����
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iFlag, sizeof(iFlag)))
        return false;
    SOCKET sock = accept(m_sock, (struct sockaddr*)&cliaddr, &addrlen);
    if ( sock == SOCKET_ERROR ) 
    {
        return false;
    }
    int bNodelay = 1; 
    int err; 
    err = setsockopt( sock, IPPROTO_TCP, TCP_NODELAY, (char *)&bNodelay, sizeof(bNodelay));//��������ʱ�㷨 
    if (err != NO_ERROR)
        return false;

    //���÷��ͻ���Ĵ�С(TS��25�����Ĵ�С[4K/��])
    int bufsize = 100*1024;
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize)))
    {
        return false;
    }
    //���ý��ջ���Ĵ�С(TS��25�����Ĵ�С[4K/��])
    bufsize = 100*1024;
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&bufsize, sizeof(bufsize)))
    {
        return false;
    }
    //���÷��ͳ�ʱʱ��(5��)
    int timeout = 5*1000;
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)))
    {
        return false;
    }
    //���÷�������ʽ����
/*    unsigned long ul = 1;
    if(SOCKET_ERROR == ioctlsocket(sock, FIONBIO, (unsigned long*)&ul))
    {
        return false;
    }*/
    s = sock;
    if ( fromip != NULL )
        sprintf(fromip, "%s", inet_ntoa(cliaddr.sin_addr));

    return true;
}

bool WSocket::acceptSocket(WSocket& s) 
{
    int ilenofsockaddr=sizeof(sockaddrin);
/*    //���÷�������ʽ����
    unsigned long ul = 1;
    if(SOCKET_ERROR == ioctlsocket(m_sock, FIONBIO, (unsigned long*)&ul))
    {
        return false;
    }*/
    
    int iFlag = 1;//���õ�ַ����
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iFlag, sizeof(iFlag)))
        return false;
    SOCKET sock = accept(m_sock, (struct sockaddr far *)&sockaddrin, &ilenofsockaddr);
    if(sock == INVALID_SOCKET)
        return false;
    int bNodelay = 1; 
    int err; 
    err = setsockopt( sock, IPPROTO_TCP, TCP_NODELAY, (char *)&bNodelay, sizeof(bNodelay));//��������ʱ�㷨 
    if (err != NO_ERROR)
        return false;
    //���÷��ͻ���Ĵ�С(TS��25�����Ĵ�С[4K/��])
    int bufsize = 100*1024;
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize)))
    {
        return false;
    }
    //���ý��ջ���Ĵ�С(TS��25�����Ĵ�С[4K/��])
    bufsize = 100*1024;
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&bufsize, sizeof(bufsize)))
    {
        return false;
    }
    //���÷��ͳ�ʱʱ��(5��)
    int timeout = 5*1000;
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)))
    {
        return false;
    }
    //���÷�������ʽ����
/*    unsigned long ul = 1;
    if(SOCKET_ERROR == ioctlsocket(sock, FIONBIO, (unsigned long*)&ul))
    {
        return false;
    }*/
    s.m_sock = sock;
    return true;
}

int WSocket::Close()
{
#ifdef WIN32
    return (closesocket(m_sock));
#else
    return (close(m_sock));
#endif
}

int WSocket::GetError()
{
#ifdef WIN32
    return (WSAGetLastError());
#else
    return (errno);
#endif
}

//���������socket
bool WSocket::createServerSocket(int port, int iFlag) 
{
    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(m_sock==INVALID_SOCKET) 
        return false;
    //���õ�ַ����
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iFlag, sizeof(iFlag)))
        return false;

    // socket bind
    sockaddrin.sin_addr.s_addr = INADDR_ANY;
    sockaddrin.sin_family = AF_INET;
    sockaddrin.sin_port = htons(port);
    
    if(bind(m_sock, (struct sockaddr far *)&sockaddrin, sizeof(sockaddrin)))
        return false;
    SOCKADDR_IN sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    int nSockAddrLen = sizeof(sockAddr);
    if (getsockname(m_sock, (SOCKADDR*)&sockAddr, &nSockAddrLen))
        return false;

    if(port == 0) 
        localPort = ntohs (sockAddr.sin_port);    //���汾�ض˿�
    // socket listen
    if(listen(m_sock, 10))
        return false;

    return true;
}
