/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  tcpDevice.cpp
 * @Version        :  1.0.0
 * @Brief           :  封装TCP通信方式,对外提供简洁的接口
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月9日 15:36:57
 * @History        : 
 *   l
 */
 
#include "tcpDevice.h"
#include "sysType.h"
#include "comfunc.h"

/************************引用部分*****************************/

/************************前向声明部分***********************/

/************************定义部分*****************************/

/************************实现部分****************************/

/**
* 通过配置信息打开设备.
* @param ServerConfig* 配置信息
* @return true打开成功,false打开失败
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
* 关闭设备.
* @return true关闭成功,false关闭失败
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
* 设置socket号.
* @param socketDescriptor socket号
*/
void tcpDevice::setTcpSocket(int socketDescriptor) 
{
    m_socket.m_sock = socketDescriptor;
}

//创建服务端socket
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
    //接收包头
    ret = m_socket.Recv(head, PACKET_HEADSIZE, 0);
    if (ret <= 0) 
    {
        sysLoger(SLWARNING, "tcpDevice::read: socket error encounterd when receiving data head!");
        return -1;
    }
    
    int size;
    short src_aid, src_said, des_said, des_aid;
    //对接收到的包头信息进行解码
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
        return -1;    //包有问题
    }

    //如果没有内容则不进行包体的接收
    if(size > 0)
    {
        //接收包体
        ret = m_socket.Recv(pack->getData(), size, 0) ;
        if (ret <= 0) 
        {
            //连接失败
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
    //对包头进行字节的排列
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
        return -1;    //包有问题
    else
        size = pack->getSize();
    
    m_socket.Send(sendBuf, PACKET_HEADSIZE);

    //发送包体
    m_socket.Send(pack->getData(), size);
    return size;

}

