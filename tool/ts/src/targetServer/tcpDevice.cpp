/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  tcpDevice.cpp
* @brief
*       ���ܣ�
*       <li> tcp��ʽ���豸����</li>
*/


/************************ͷ�ļ�********************************/

#include "tcpDevice.h"
#include "common.h"
/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/
///////////////////////////////////////////////DeviceTcp
/**
* ͨ��������Ϣ���豸.
* @param ServerConfig* ������Ϣ
* @return true�򿪳ɹ�,false��ʧ��
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
* �ر��豸.
* @return true�رճɹ�,false�ر�ʧ��
*/
bool DeviceTcp::close()
{
    /*Modified by tangxp for BUG NO.0002553 on 2008.1.15 [begin]*/
    /*  Modified brief: �޸�socket�������ж�*/
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

//////////////////////////
