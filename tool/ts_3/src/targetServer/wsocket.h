
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
* @file     wsock.h
* @brief     tcp实现类,window版
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/

#ifndef _HEGANG_WSOCKET_H_
#define _HEGANG_WSOCKET_H_

//#ifdef WIN32
    #include <winsock.h>
    typedef int                socklen_t;
/*#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    typedef int                SOCKET;
    #define INVALID_SOCKET    -1
    #define SOCKET_ERROR    -1
#endif*/


class WSocket {

public:
    SOCKET m_sock;
    struct sockaddr_in sockaddrin;
    int timeout;
    WSocket(SOCKET sock = INVALID_SOCKET);
    ~WSocket();
    bool acceptSocket(WSocket& s);
    bool createServerSocket(int port,int iFlag=0) ;
    // Create socket
    bool Create(int af, int type, int protocol = 0);

    // Connect socket
    bool Connect(const char* ip, unsigned short port);

    // Bind socket
    bool Bind(unsigned short port);

    // Listen socket
    bool Listen(int backlog = 5); 

    // Accept socket
    bool Accept(WSocket& s, char* fromip = NULL);

    // Send socket
    int Send(const char* buf, int len, int flags = 0) {
        int bytes;
        int count = 0;

        while ( count < len ) {

            bytes = send(m_sock, buf + count, len - count, flags);
            if (bytes == SOCKET_ERROR )
                return -1;
            count += bytes;
        } 

        return count;
    }


    // Recv socket
    int Recv(char* buf, int len, int flags = 0) {
        flags = 0;
        int Len = 0;
        int count = 0;
        while ( count < len ) {
            Len = recv(m_sock, buf+count, len- count, flags);
            //网络断了
            if ((SOCKET_ERROR == Len && WSAGetLastError() != WSAEWOULDBLOCK)) {
                return -1;
            }
            if(Len == 0)
                return -2;
            count += Len;
        }

        return count;
    }    

    int GetLocalPort() {
        return localPort;
    }
    // Close socket
    int Close();

    // Get errno
    int GetError();

    // Init winsock DLL 
    static int Init();    
    // Clean winsock DLL
    static int Clean();

    WSocket& operator = (SOCKET s);

    operator SOCKET ();

protected:
    u_short localPort;
};

#endif

