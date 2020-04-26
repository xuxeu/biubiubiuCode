/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
* @file 	PortocolRsp.cpp
* @brief 	
*     <li>���ܣ� tcp,udpͨ�ŷ�ʽ��Э���ʵ��</li>
* @author 	������
* @date 	2007-10-23
* <p>���ţ�ϵͳ�� 
*/

//#include "stdafx.h"
#include "ProtocolRsp.h"
#include "DeviceRsp.h"
//#include "log.h"
#include "common.h"
//#include "Packet.h"


//ͬ������Χ
const int SYNC_RANGE = 0x3;

//�������кŷ�Χ
const int RX_PACKET_MAX = 0x3fffffff;
const int RX_PACKET_MIN = 0x00000001;

//�����кŷ�Χ
const int TX_PACKET_MAX = 0x7fffffff;
const int TX_PACKET_MIN = 0x40000001;
const int RETRY_COUNT = -1;


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


/**************************************UDPЭ��************************************************/

PacketProtocol::PacketProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice)
{
    isAck = true;
    retry = RETRY_COUNT;    //���Դ���Ĭ����-1
    timeout = 5000;
    isExit = false;
    surePacket = new waitCondition();    //ȷ�ϻظ���
    mutex = new TMutex();                //�Ի�������������,
    pWaitCondition = new waitCondition();    //ȷ�ϻظ���
    waitMutex = new TMutex();                //�Ի�������������,
    isPutEnd = true;        //Ĭ������ǲ���Ҫ�ͷŷ����߳���Դ
}

/**
* ���ܣ��������ݰ�
* @param  Packet* ���ݰ�ָ��
* @return �յ������ݰ���С
*/
const char ACK[] = "+";
int PacketProtocol::getpkt(T_CHAR* pack,T_WORD size) 
{
    int recSize = 0;
    if(NULL == pack)
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: Bad pointer of pack, eixt receiving data!");
        return -1;
    }

    //�õ���ʱ�����Դ���
    DeviceUdp* pDeviceUdp = dynamic_cast<DeviceUdp*>(getDevice());
    if(NULL == pDeviceUdp)
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: No usable device!");
        return -1;
    }
    if (retry != pDeviceUdp->getRetry())
        retry = pDeviceUdp->getRetry();
    if (timeout != pDeviceUdp->getTimeout())
        timeout = pDeviceUdp->getTimeout();
start:
    //����һ�����������ݰ�
    recSize = getDevice()->read(pack, size) ;

    if (recSize <= 0) 
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: Socket error encountered when receiving pack."\
                                            " ErrorNo: %d", recSize);
        return -1;
    }

    if (recSize == strlen(ACK))
    {    //�ж��յ����Ƿ���ȷ�ϰ�
        if (strstr(pack, ACK) != NULL) 
        {   
            //���յ�ACK��,���ͷ������ź�            
            {
                TMutexLocker lock(mutex);
                isAck = true;            //�յ�ȷ�ϰ�
                surePacket->wakeAll();    //���ѷ��Ͱ�
            }
            goto start;
        }
    }
    
    if (recSize > size) 
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: The size of packet data is error!");
        goto start;    //�������հ�
    }
    return recSize;
}

/**
* ���ܣ�����ȷ�ϰ�
*/
//void PacketProtocol::sendAck() 
//{
//    char buff[PACKET_HEADSIZE];
//    char* pBuf = buff;
//    memcpy(pBuf, ACK,strlen(ACK));
//    pBuf += strlen(ACK);
//    getDevice()->write(buff, pBuf-buff);    //����ȷ�ϰ�
//}

/**
* ���ܣ��������ݰ�
* @param  Packet* ���ݰ�ָ��
* @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
*/
int PacketProtocol::putpkt(T_CHAR* pack,T_WORD size) 
{

    int putCount = 0;         //���ͼ���
    isAck = false;            //ÿ����һ��������Ϊfalse����ʾ��û���յ��ظ���
    if(NULL == pack)
    {
        sysLoger(LWARNING, "PacketProtocol::putpkt: Bad pointer of pack, eixt sending data!");
        return -1;
    }

    //�õ���ʱ�����Դ���
    DeviceUdp* pDeviceUdp = dynamic_cast<DeviceUdp*>(getDevice());
    if(NULL == pDeviceUdp)
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: No usable device!");
        return -1;
    }
    if (retry != pDeviceUdp->getRetry())
        retry = pDeviceUdp->getRetry();
    if (timeout != pDeviceUdp->getTimeout())
        timeout = pDeviceUdp->getTimeout(); 

    if (size > PACKET_DATASIZE)
    {
        return -1;    //��������
    }

    isPutEnd = false;
start:
    //���Ͱ�
    getDevice()->write(pack, size);

    {
        bool isSure = true;
        {
            TMutexLocker lock(mutex);  
            if (!isAck)    //�����û���յ�ȷ�ϰ�,����ж���
                isSure = surePacket->wait(mutex,timeout);    //��ʱ����ack��
        }
        if (isExit) 
        {
            TMutexLocker lock(waitMutex);  
            pWaitCondition->wakeAll();    //��ʱ����ack��
            isPutEnd = true;
            return -1;    //��ʾ�˳�
        }
        if (!isSure)
        {
            putCount++;        //��ʱ��������Դ�����һ
        }
    }
    if (!isAck)    
    {//�ط��˰�
        if (putCount < retry )
        {    //���·��Ͱ�
            sysLoger(LINFO, "PacketProtocol::putpkt: Send packet again! retried times:%d", putCount);
            goto start;    //�������Ͱ�
        }
        return -1;
    }

    isExit = false;
    isPutEnd = true;
    return size;
}

/**
* ���ܣ��ͷŶ����Ӧ����Դ
*/
void PacketProtocol::close() 
{
    isExit = true;            //�յ�ȷ�ϰ�
    {
        TMutexLocker lock(mutex);
        surePacket->wakeAll();    //���ѷ��Ͱ�
    }

    {
        TMutexLocker lock(waitMutex);  
        if (!isPutEnd)
            pWaitCondition->wait(waitMutex,1000);    //ʹ���Ͱ���ʱ������������,���˳������߳�
    }
}
/**
* ���ܣ��ͷŶ����Ӧ����Դ
*/
PacketProtocol::~PacketProtocol() 
{
    if (surePacket) 
    {
        delete surePacket;
        surePacket = NULL;
    }
    if (mutex) 
    {
        delete mutex;
        mutex = NULL;
    }
    if (pWaitCondition)
    {
        delete pWaitCondition;
        pWaitCondition= NULL;
    }
    if (waitMutex) 
    {
        delete waitMutex;
        waitMutex = NULL;
    }
}

/**************************************TCPЭ��************************************************/

RapidProtocol::RapidProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice) 
{
}

/**
* ���ܣ��������ݰ�
* @param  Packet* ���ݰ�ָ��
* @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
*/
int RapidProtocol::putpkt(T_CHAR* pack,T_WORD size) 
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "RapidProtocol::putpkt: Parameter pack is null pointer!");
        return -1;
    }

    DeviceBase *pDev = getDevice();
    if(NULL == pDev)
    {
        sysLoger(LWARNING, "RapidProtocol::putpkt: No device is usable!");
        return -1;
    }
    if (size > PACKET_DATASIZE)
    {
        return -1;    //�������� 
    }  
    
    //�������ݰ�
    pDev->write(pack, size);
    return size;
}

/**
* ���ܣ��������ݰ�
* @param  Packet* ���ݰ�ָ��
* @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
*/
int RapidProtocol::getpkt(T_CHAR* pack,T_WORD size) 
{  
    if(NULL == pack)
    {
        sysLoger(LWARNING, "RapidProtocol::getpkt: Parameter pack is null pointer!");
        return -1;
    }
    
    DeviceBase *pDev = getDevice();
    if(NULL == pDev)
    {
        sysLoger(LWARNING, "RapidProtocol::getpkt: No device is usable!");
        return -1;
    }  

    //�������ݰ�
    int recSize = pDev->read(pack,size) ;
    if (recSize <= 0) 
    {
        //����ʧ��
        sysLoger(LWARNING, "RapidProtocol::getpkt: socket error encounterd when receiving data content!");
        return -1;
    }
    return size;
}