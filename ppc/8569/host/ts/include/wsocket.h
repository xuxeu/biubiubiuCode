/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  wsock.h
* @brief
*       ���ܣ�
*       <li>tcpʵ����,</li>
*/



#ifndef _HEGANG_WSOCKET_H_
#define _HEGANG_WSOCKET_H_
/************************ͷ�ļ�********************************/

#include <winsock.h>

/************************�궨��********************************/
/************************���Ͷ���******************************/
typedef int             socklen_t;
class WSocket
{

public:
    SOCKET m_sock;
    struct sockaddr_in sockaddrin;
    int timeout;
    WSocket(SOCKET sock = INVALID_SOCKET);
    ~WSocket();
    bool acceptSocket(WSocket& s);
    bool createServerSocket(int port, int iFlag = 0) ;
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
    int Send(const char* buf, int len, int flags = 0)
    {
        int bytes;
        int count = 0;

        while ( count < len )
        {

            bytes = send(m_sock, buf + count, len - count, flags);

            if (bytes == SOCKET_ERROR )
            {
                return -1;
            }

            count += bytes;
        }

        return count;
    }


    // Recv socket
    int Recv(char* buf, int len, int flags = 0)
    {
        flags = 0;
        int Len = 0;
        int count = 0;

        while ( count < len )
        {
            Len = recv(m_sock, buf + count, len - count, flags);

            //�������
            if ((SOCKET_ERROR == Len && WSAGetLastError() != WSAEWOULDBLOCK))
            {
                return -1;
            }

            if(Len == 0)
            {
                return -2;
            }

            count += Len;
        }

        return count;
    }

    int GetLocalPort()
    {
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
/************************�ӿ�����******************************/
#endif

