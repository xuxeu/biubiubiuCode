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
* @brief     TSЭ������
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
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
* ���ܣ��ͷŶ����Ӧ����Դ
*/
void ProtocolBase::close() 
{
}

/**
* ���ܣ�ͨ������������pack�İ�ͷ
* @param  pBuff �����ͷ�Ļ���������,����Ҫ���ڰ�ͷ�Ĵ�С
* @param  pack[OUT] ���ð�ͷ
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
* ���ܣ��ṩ����Э��ķ���
* @param  Э������
* @param  Э���Ӧ���豸����ָ��
* @return һ������Э��
*/
ProtocolBase* ProtocolManage::createProtocol(int type,DeviceBase* pDevice)
{
   switch (type) 
    {
       case ProtocolBase::TCP_PROTOCOL:
            return new RapidProtocol(pDevice);
            break;
       case ProtocolBase::PACKET_PROTOCOL:
            return new PacketProtocol(pDevice);    //������ϢЭ��
            break;
       case ProtocolBase::STREAM_PROTOCOL:
            return new StreamProtocol(pDevice);    //������Э��
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
* ���ܣ��������ݰ�
* @param  Packet* ���ݰ�ָ��
* @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
*/
int RapidProtocol::putpkt(Packet* pack) 
{
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [begin]*/
    /*  Modified brief: ������ͷ����sendBuf����*/
    char sendBuf[PACKET_HEADSIZE] = "\0";
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [end]*/
    char* pBuf = sendBuf;
    if(NULL == pack)
    {
        sysLoger(LWARNING, "RapidProtocol::putpkt: Parameter pack is null pointer!");
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
    DeviceBase *pDev = getDevice();
    if(NULL == pDev)
    {
        sysLoger(LWARNING, "RapidProtocol::putpkt: No device is usable!");
        return -1;
    }
    if (pack->getSize() > PACKET_DATASIZE)
        return -1;    //��������
    else
        size = pack->getSize();
    
    pDev->write(sendBuf, PACKET_HEADSIZE);

    //���Ͱ���
    pDev->write(pack->getData(), size);
    return size;
}

/**
* ���ܣ��������ݰ�
* @param  Packet* ���ݰ�ָ��
* @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
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
    //���հ�ͷ
    int recSize = pDev->read(head,PACKET_HEADSIZE) ;
    if (recSize <= 0) 
    {
        sysLoger(LWARNING, "RapidProtocol::getpkt: socket error encounterd when receiving data head!");
        return -1;
    }
    int size;
    short src_aid,src_said,des_said,des_aid;
    //�Խ��յ��İ�ͷ��Ϣ���н���
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
        return -1;    //��������
    }

    //���û�������򲻽��а���Ľ���
    if(size > 0)
    {
        //���հ���
        recSize = pDev->read(pack->getData(),size) ;
        if (recSize <= 0) 
        {
            //����ʧ��
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
        return -1;    //��������
    else
        size = pack->getSize();
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
    //������
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
    //��������
    int recSize = pDev->read(pBuf, (PACKET_HEADSIZE + PACKET_DATASIZE)) ;
    if (recSize <= PACKET_HEADSIZE) 
    {
        sysLoger(LWARNING, "UsbProtocol::getpkt: device error encounterd when receiving data!");
        return -1;
    }
    int size, hold;
    short src_aid,src_said,des_said,des_aid;
    //�Խ��յ��İ�ͷ��Ϣ���н���
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
    //��������
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

