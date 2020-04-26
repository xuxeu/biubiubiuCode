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
 * @file     tcpDevice.cpp
 * @brief     tcp��ʽ���豸����
 * @author     ���
 * @date     2004-12-6
 * @version 3.0
 * @warning �Ͻ������ļ������ַ�
 *
 * ���ţ�ϵͳ�� 
 *
 */


#include "tcpDevice.h"
#include "common.h"

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
    /*Modified by tangxp for BUG NO.0002553 on 2008.1.15 [begin]*/
    /*  Modified brief: �޸�socket�������ж�*/
    if (m_socket.m_sock != INVALID_SOCKET)
    /*Modified by tangxp for BUG NO.0002553 on 2008.1.15 [end]*/
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

//////////////////////////
