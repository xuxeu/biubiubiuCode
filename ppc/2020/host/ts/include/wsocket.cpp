/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  wsock.cpp
* @brief
*       功能：
*       <li>tcp实现类,</li>
*/


/************************头文件********************************/
#include <iostream>
using namespace std;
#include "wsocket.h"


#ifdef WIN32
#pragma comment(lib, "wsock32")
#endif
using namespace std;

/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/
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
    unsigned long ul = 1;   //非堵塞方式

    if(SOCKET_ERROR == ioctlsocket(m_sock, FIONBIO, (unsigned long*)&ul))
    {
        return false;
    }

    struct sockaddr_in svraddr;

    svraddr.sin_family = AF_INET;

    svraddr.sin_addr.s_addr = inet_addr(ip);

    svraddr.sin_port = htons(port);

    int ret = connect(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));

    struct timeval overtime;

    overtime.tv_sec = 1;    //连接超时设置为1秒

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
        return false ;      //超时

    }

    else if (ret < 0)    //socket关闭
    {
        return false;
    }

    int iFlag = 1;//设置地址重用

    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iFlag, sizeof(iFlag)))
    {
        return false;
    }

    int bNodelay = 1;
    int err;
    err = setsockopt( m_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&bNodelay, sizeof(bNodelay));//不采用延时算法

    if (err != NO_ERROR)
    {
        return false;
    }

    ul = 0; //堵塞方式

    if(SOCKET_ERROR == ioctlsocket(m_sock, FIONBIO, (unsigned long*)&ul))
    {
        return false;
    }

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
    {
        return false;
    }

    int ret = bind(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));

    if ( ret == SOCKET_ERROR )
    {
        return false;
    }

    SOCKADDR_IN sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    int nSockAddrLen = sizeof(sockAddr);

    if (getsockname(m_sock, (SOCKADDR*)&sockAddr, &nSockAddrLen))
    {
        return false;
    }

    if(port == 0)
    {
        localPort = sockAddr.sin_port;    //保存本地端口
    }

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
    int iFlag = 1;//设置地址重用

    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iFlag, sizeof(iFlag)))
    {
        return false;
    }

    SOCKET sock = accept(m_sock, (struct sockaddr*)&cliaddr, &addrlen);

    if ( sock == SOCKET_ERROR )
    {
        return false;
    }

    int bNodelay = 1;
    int err;
    err = setsockopt( sock, IPPROTO_TCP, TCP_NODELAY, (char *)&bNodelay, sizeof(bNodelay));//不采用延时算法

    if (err != NO_ERROR)
    {
        return false;
    }

    s = sock;

    if ( fromip != NULL )
    {
        sprintf(fromip, "%s", inet_ntoa(cliaddr.sin_addr));
    }

    return true;
}

bool WSocket::acceptSocket(WSocket& s)
{
    int ilenofsockaddr = sizeof(sockaddrin);

    int iFlag = 1;//设置地址重用

    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iFlag, sizeof(iFlag)))
    {
        return false;
    }

    SOCKET sock = accept(m_sock, (struct sockaddr far *)&sockaddrin, &ilenofsockaddr);

    if(sock == INVALID_SOCKET)
    {
        return false;
    }

    int bNodelay = 1;
    int err;
    err = setsockopt( sock, IPPROTO_TCP, TCP_NODELAY, (char *)&bNodelay, sizeof(bNodelay));//不采用延时算法

    if (err != NO_ERROR)
    {
        return false;
    }

	//设置当前客服端在使用closesocket关闭套接字时，立即释放。 否则当前客服端套接字不会立即释放,在windows下当客服端连接次数超过3900多次时，就会无法连接
	BOOL opt_b =  FALSE;
	if (setsockopt(sock, SOL_SOCKET, SO_DONTLINGER, (char*)&opt_b, sizeof(BOOL)) < 0){
		return false;
	} 
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

//创建服务端socket
bool WSocket::createServerSocket(int port, int iFlag)
{

    m_sock = socket(AF_INET, SOCK_STREAM, 0);

    if(m_sock == INVALID_SOCKET)
    {
        return false;
    }

    //设置地址重用
    if(SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iFlag, sizeof(iFlag)))
    {
        return false;
    }

    // socket bind
    sockaddrin.sin_addr.s_addr = INADDR_ANY;
    sockaddrin.sin_family = AF_INET;
    sockaddrin.sin_port = htons(port);

    if(bind(m_sock, (struct sockaddr far *)&sockaddrin, sizeof(sockaddrin)))
    {
        return false;
    }

    SOCKADDR_IN sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    int nSockAddrLen = sizeof(sockAddr);

    if (getsockname(m_sock, (SOCKADDR*)&sockAddr, &nSockAddrLen))
    {
        return false;
    }

    if(port == 0)
    {
        localPort = ntohs (sockAddr.sin_port);  //保存本地端口
    }

    // socket listen
    if(listen(m_sock, 10))
    {
        return false;
    }

    return true;
}
