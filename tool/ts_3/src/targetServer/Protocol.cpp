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
* @file     protocol.cpp
* @brief     TS协议抽象层
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/

#include "Protocol.h"
#include "packetProtocol.h"
#include "Manager.h"
#include "common.h"
//#include "util.h"
#include "streamProtocol.h"
/////////////////////////////ProtocolBase
ProtocolBase::ProtocolBase(DeviceBase* pDevice) 
{
   this->pDevice = pDevice;
}
/**
* 功能：释放对象对应的资源
*/
void ProtocolBase::close() 
{
}

/**
* 功能：通过缓冲区设置pack的包头
* @param  pBuff 保存包头的缓冲区数据,至少要大于包头的大小
* @param  pack[OUT] 设置包头
*/
void ProtocolBase::readPacketHead(char* pBuff,Packet* pack) 
{
    int size = 0;
    short src_aid = 0, src_said = 0, des_said = 0, des_aid = 0;
    int serialNum = 0;
    if((NULL == pack) || (NULL == pBuff))
    {
        sysLoger(LWARNING, "ProtocolBase::readPacketHead: Parameter pack or pBuff is bad pointer!");
        return;
    }
    __fetch(&size,pBuff,sizeof(int));
    pBuff += sizeof(int);

    __fetch(&des_aid,pBuff,sizeof(short));
    pBuff += sizeof(short);
    __fetch(&src_aid,pBuff,sizeof(short));
    pBuff += sizeof(short);
    __fetch(&des_said,pBuff,sizeof(short));
    pBuff += sizeof(short);
    __fetch(&src_said,pBuff,sizeof(short));
    pBuff += sizeof(short);
    __fetch(&serialNum,pBuff,sizeof(int));
    pBuff += sizeof(int);
     
    pack->setSrc_said(src_said);
    pack->setDes_said(des_said);
    pack->setDes_aid(des_aid);
    pack->setSrc_aid(src_aid);
    pack->setSerialNum(serialNum);
    pack->setSize(size);
}

//////////////////////////////////////////////ProtocolManage
ProtocolManage* ProtocolManage::pProtocolManage = NULL;
TMutex ProtocolManage::mutex;


/**
* 功能：提供创建协议的方法
* @param  协议类型
* @param  协议对应的设备对象指针
* @return 一个具体协议
*/
ProtocolBase* ProtocolManage::createProtocol(int type,DeviceBase* pDevice)
{
   switch (type) 
    {
       case ProtocolBase::TCP_PROTOCOL:
            return new RapidProtocol(pDevice);
            break;
       case ProtocolBase::PACKET_PROTOCOL:
            return new PacketProtocol(pDevice);    //创建消息协议
            break;
       case ProtocolBase::STREAM_PROTOCOL:
            return new StreamProtocol(pDevice);    //创建流协议
            break;
        case ProtocolBase::USB_PROTOCOL:
            return new UsbProtocol(pDevice);
            break;
            
       default:
            break;      
   }
   return NULL;
}
//////////////////////////RapidProtocol

RapidProtocol::RapidProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice) 
{
}
template<class T> extern int __store(T value,char *dest,int bytes);
template<class T> extern  int __fetch(T *dest,char *source,int bytes);

/**
* 功能：发送数据包
* @param  Packet* 数据包指针
* @return true表示发送成功,false表示发送失败
*/
int RapidProtocol::putpkt(Packet* pack) 
{
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [begin]*/
    /*  Modified brief: 修正包头缓冲sendBuf过大*/
    char sendBuf[PACKET_HEADSIZE] = "\0";
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [end]*/
    char* pBuf = sendBuf;
    if(NULL == pack)
    {
        sysLoger(LWARNING, "RapidProtocol::putpkt: Parameter pack is null pointer!");
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
    DeviceBase *pDev = getDevice();
    if(NULL == pDev)
    {
        sysLoger(LWARNING, "RapidProtocol::putpkt: No device is usable!");
        return -1;
    }
    if (pack->getSize() > PACKET_DATASIZE)
        return -1;    //包有问题
    else
        size = pack->getSize();
    
    pDev->write(sendBuf, PACKET_HEADSIZE);

    //发送包体
    pDev->write(pack->getData(), size);
    return size;
}

/**
* 功能：发送数据包
* @param  Packet* 数据包指针
* @return true表示发送成功,false表示发送失败
*/
int RapidProtocol::getpkt(Packet* pack) 
{
    char head[PACKET_HEADSIZE] = "\0";     
    char* pBuff = head;
    DeviceBase *pDev = getDevice();
    if(NULL == pack)
    {
        sysLoger(LWARNING, "RapidProtocol::getpkt: Parameter pack is null pointer!");
        return -1;
    }
    if(NULL == pDev)
    {
        sysLoger(LWARNING, "RapidProtocol::getpkt: No device is usable!");
        return -1;
    }
    //接收包头
    int recSize = pDev->read(head,PACKET_HEADSIZE) ;
    if (recSize <= 0) 
    {
        sysLoger(LWARNING, "RapidProtocol::getpkt: socket error encounterd when receiving data head!");
        return -1;
    }
    int size;
    short src_aid,src_said,des_said,des_aid;
    //对接收到的包头信息进行解码
    __fetch(&size,pBuff,sizeof(int));
    pBuff += sizeof(int);

    __fetch(&des_aid,pBuff,sizeof(short));
    pBuff += sizeof(short);
    __fetch(&src_aid,pBuff,sizeof(short));
    pBuff += sizeof(short);
    __fetch(&des_said,pBuff,sizeof(short));
    pBuff += sizeof(short);
    __fetch(&src_said,pBuff,sizeof(short));
    pBuff += sizeof(short);
    
    if (size > PACKET_DATASIZE) 
    {
        sysLoger(LWARNING, "RapidProtocol::getpkt: data size error !");
        return -1;    //包有问题
    }

    //如果没有内容则不进行包体的接收
    if(size > 0)
    {
        //接收包体
        recSize = pDev->read(pack->getData(),size) ;
        if (recSize <= 0) 
        {
            //连接失败
            sysLoger(LWARNING, "RapidProtocol::getpkt: socket error encounterd when receiving data content!");
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

/***********************USB PROTOCOL*************************/
UsbProtocol::UsbProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice) 
{
}

int UsbProtocol::putpkt(Packet * pack)
{
    char sendBuf[PACKET_HEADSIZE + PACKET_DATASIZE] = "\0";
    char* pBuf = sendBuf;
    if(NULL == pack)
    {
        sysLoger(LWARNING, "UsbProtocol::putpkt: Parameter pack is null pointer!");
        return -1;
    }
    int size = 0;
    DeviceBase *pDev = getDevice();
    if(NULL == pDev)
    {
        sysLoger(LWARNING, "UsbProtocol::putpkt: No device is usable!");
        return -1;
    }
    if (pack->getSize() > PACKET_DATASIZE)
        return -1;    //包有问题
    else
        size = pack->getSize();
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
    //填充包体
    memcpy(pBuf, pack->getData(), pack->getSize());
    pBuf += pack->getSize();
    pDev->write(sendBuf, (pBuf - sendBuf));
    return size;
}

int UsbProtocol::getpkt(Packet* pack) 
{
    char recvBuf[PACKET_HEADSIZE + PACKET_DATASIZE] = "\0";
    char* pBuf = recvBuf;
    DeviceBase *pDev = getDevice();
    if(NULL == pack)
    {
        sysLoger(LWARNING, "UsbProtocol::getpkt: Parameter pack is null pointer!");
        return -1;
    }
    if(NULL == pDev)
    {
        sysLoger(LWARNING, "UsbProtocol::getpkt: No device is usable!");
        return -1;
    }
    //接收数据
    int recSize = pDev->read(pBuf, (PACKET_HEADSIZE + PACKET_DATASIZE)) ;
    if (recSize <= PACKET_HEADSIZE) 
    {
        sysLoger(LWARNING, "UsbProtocol::getpkt: device error encounterd when receiving data!");
        return -1;
    }
    int size, hold;
    short src_aid,src_said,des_said,des_aid;
    //对接收到的包头信息进行解码
    __fetch(&size, pBuf, sizeof(int));
    pBuf += sizeof(int);
    __fetch(&des_aid, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __fetch(&src_aid, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __fetch(&des_said, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __fetch(&src_said, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __fetch(&hold, pBuf, sizeof(int));
    pBuf += sizeof(int);
    //解析包体
    if(size != recSize - PACKET_HEADSIZE)
    {
        sysLoger(LWARNING, "UsbProtocol::getpkt: Recieved invalid packet, the packet size didn't "\
                                        "equal the size of the recieved buffer len!Recieved buffered len:%d, packet size:%d",
                                         recSize, size);
        return -1;
    }
    pack->setSrc_said(src_said);
    pack->setDes_said(des_said);
    pack->setDes_aid(des_aid);
    pack->setSrc_aid(src_aid);
    pack->setSize(size);
    pack->setSerialNum(hold);
    memcpy(pack->getData(), &recvBuf[PACKET_HEADSIZE], size);
    
    return size;
}

