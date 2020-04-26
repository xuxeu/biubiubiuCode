/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  tcpDevice.cpp
 * @Version        :  1.0.0
 * @Brief           :  ��װTCPͨ�ŷ�ʽ,�����ṩ���Ľӿ�
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007��4��9�� 15:36:57
 * @History        : 
 *   l
 */
 
#include "tcpDevice.h"
#include "sysType.h"
#include "comfunc.h"

/************************���ò���*****************************/

/************************ǰ����������***********************/

/************************���岿��*****************************/

/************************ʵ�ֲ���****************************/

/**
* ͨ��������Ϣ���豸.
* @param ServerConfig* ������Ϣ
* @return true�򿪳ɹ�,false��ʧ��
*/
bool tcpDevice::open(QString IP, int port) 
{
    m_IP = IP;
    m_port = port;
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
bool tcpDevice::close() 
{
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


tcpDevice::tcpDevice(QString IP, int port)
{
    m_IP = IP;
    m_port = port;
}
tcpDevice::~tcpDevice()
{
    close();
    m_IP.clear();
}

/**
* ����socket��.
* @param socketDescriptor socket��
*/
void tcpDevice::setTcpSocket(int socketDescriptor) 
{
    m_socket.m_sock = socketDescriptor;
}

//���������socket
bool tcpDevice::createServerSocket(int port) 
{
    return m_socket.createServerSocket(port);

}

bool tcpDevice::acceptSocket(WSocket& s) 
{
    return m_socket.acceptSocket(s);
}

int tcpDevice::read(Packet  *pack)
{

    //QMutexLocker  locker(&m_mtxSocket);
    
    char head[PACKET_HEADSIZE] = "\0";     
    char* pBuff = head;
    int ret = 0;
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "tcpDevice::read:Argument pack is bad pointer!");
        return -1;
    }
    //���հ�ͷ
    ret = m_socket.Recv(head, PACKET_HEADSIZE, 0);
    if (ret <= 0) 
    {
        sysLoger(SLWARNING, "tcpDevice::read: socket error encounterd when receiving data head!");
        return -1;
    }
    
    int size;
    short src_aid, src_said, des_said, des_aid;
    //�Խ��յ��İ�ͷ��Ϣ���н���
    __fetch(&size, pBuff, sizeof(int));
    pBuff += sizeof(int);

    __fetch(&des_aid, pBuff, sizeof(short));
    pBuff += sizeof(short);
    __fetch(&src_aid, pBuff, sizeof(short));
    pBuff += sizeof(short);
    __fetch(&des_said, pBuff, sizeof(short));
    pBuff += sizeof(short);
    __fetch(&src_said, pBuff, sizeof(short));
    pBuff += sizeof(short);
    
    if((size > PACKET_DATASIZE) || (size < 0))
    {
        sysLoger(SLWARNING,  "tcpDevice::read: data size error !");
        return -1;    //��������
    }

    //���û�������򲻽��а���Ľ���
    if(size > 0)
    {
        //���հ���
        ret = m_socket.Recv(pack->getData(), size, 0) ;
        if (ret <= 0) 
        {
            //����ʧ��
            sysLoger(SLWARNING,  "tcpDevice::read: socket error encounterd when receiving data content!");
            return -1;
        }
    }

    pack->setSrc_said(src_said);
    pack->setDes_said(des_said);
    pack->setDes_aid(des_aid);
    pack->setSrc_aid(src_aid);
    pack->setSize(size);

    return size;
}

int tcpDevice::write(Packet *pack)
{
    //QMutexLocker  locker(&m_mtxSocket);

    char sendBuf[PACKET_HEADSIZE + PACKET_DATASIZE] = "\0";
    char* pBuf = sendBuf;
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RapidProtocol::putpkt: Parameter pack is null pointer!");
        return -1;
    }
    //�԰�ͷ�����ֽڵ�����
    __store(pack->getSize(),pBuf,sizeof(int));
    pBuf += sizeof(int);
    __store(pack->getDes_aid(),pBuf,sizeof(short));
    pBuf += sizeof(short);
    __store(pack->getSrc_aid(),pBuf,sizeof(short));

    pBuf += sizeof(short);
    __store(pack->getDes_said(),pBuf,sizeof(short));
    pBuf += sizeof(short);
    __store(pack->getSrc_said(),pBuf,sizeof(short));
    pBuf += sizeof(short);

    __store(pack->getSerialNum(),pBuf,sizeof(int));
    pBuf += sizeof(int);
    
    int size = 0;
    if (pack->getSize() > PACKET_DATASIZE)
        return -1;    //��������
    else
        size = pack->getSize();
    
    m_socket.Send(sendBuf, PACKET_HEADSIZE);

    //���Ͱ���
    m_socket.Send(pack->getData(), size);
    return size;

}

