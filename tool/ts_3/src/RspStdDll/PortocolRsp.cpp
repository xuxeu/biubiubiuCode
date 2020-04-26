/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
* @file 	PortocolRsp.cpp
* @brief 	
*     <li>功能： tcp,udp通信方式的协议的实现</li>
* @author 	唐兴培
* @date 	2007-10-23
* <p>部门：系统部 
*/

//#include "stdafx.h"
#include "ProtocolRsp.h"
#include "DeviceRsp.h"
//#include "log.h"
#include "common.h"
//#include "Packet.h"


//同步允许范围
const int SYNC_RANGE = 0x3;

//接收序列号范围
const int RX_PACKET_MAX = 0x3fffffff;
const int RX_PACKET_MIN = 0x00000001;

//发送列号范围
const int TX_PACKET_MAX = 0x7fffffff;
const int TX_PACKET_MIN = 0x40000001;
const int RETRY_COUNT = -1;


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


/**************************************UDP协议************************************************/

PacketProtocol::PacketProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice)
{
    isAck = true;
    retry = RETRY_COUNT;    //重试次数默认是-1
    timeout = 5000;
    isExit = false;
    surePacket = new waitCondition();    //确认回复包
    mutex = new TMutex();                //对缓冲区进行锁定,
    pWaitCondition = new waitCondition();    //确认回复包
    waitMutex = new TMutex();                //对缓冲区进行锁定,
    isPutEnd = true;        //默认情况是不需要释放发送线程资源
}

/**
* 功能：发送数据包
* @param  Packet* 数据包指针
* @return 收到的数据包大小
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

    //得到超时和重试次数
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
    //接收一个完整的数据包
    recSize = getDevice()->read(pack, size) ;

    if (recSize <= 0) 
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: Socket error encountered when receiving pack."\
                                            " ErrorNo: %d", recSize);
        return -1;
    }

    if (recSize == strlen(ACK))
    {    //判断收到的是否是确认包
        if (strstr(pack, ACK) != NULL) 
        {   
            //接收到ACK包,则释放条件信号            
            {
                TMutexLocker lock(mutex);
                isAck = true;            //收到确认包
                surePacket->wakeAll();    //唤醒发送包
            }
            goto start;
        }
    }
    
    if (recSize > size) 
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: The size of packet data is error!");
        goto start;    //继续重收包
    }
    return recSize;
}

/**
* 功能：发送确认包
*/
//void PacketProtocol::sendAck() 
//{
//    char buff[PACKET_HEADSIZE];
//    char* pBuf = buff;
//    memcpy(pBuf, ACK,strlen(ACK));
//    pBuf += strlen(ACK);
//    getDevice()->write(buff, pBuf-buff);    //发送确认包
//}

/**
* 功能：发送数据包
* @param  Packet* 数据包指针
* @return true表示发送成功,false表示发送失败
*/
int PacketProtocol::putpkt(T_CHAR* pack,T_WORD size) 
{

    int putCount = 0;         //发送计数
    isAck = false;            //每发送一个包，置为false，表示还没有收到回复包
    if(NULL == pack)
    {
        sysLoger(LWARNING, "PacketProtocol::putpkt: Bad pointer of pack, eixt sending data!");
        return -1;
    }

    //得到超时和重试次数
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
        return -1;    //包有问题
    }

    isPutEnd = false;
start:
    //发送包
    getDevice()->write(pack, size);

    {
        bool isSure = true;
        {
            TMutexLocker lock(mutex);  
            if (!isAck)    //如果还没有收到确认包,则进行堵塞
                isSure = surePacket->wait(mutex,timeout);    //超时堵塞ack包
        }
        if (isExit) 
        {
            TMutexLocker lock(waitMutex);  
            pWaitCondition->wakeAll();    //超时堵塞ack包
            isPutEnd = true;
            return -1;    //表示退出
        }
        if (!isSure)
        {
            putCount++;        //超时则进行重试次数加一
        }
    }
    if (!isAck)    
    {//重发此包
        if (putCount < retry )
        {    //重新发送包
            sysLoger(LINFO, "PacketProtocol::putpkt: Send packet again! retried times:%d", putCount);
            goto start;    //继续发送包
        }
        return -1;
    }

    isExit = false;
    isPutEnd = true;
    return size;
}

/**
* 功能：释放对象对应的资源
*/
void PacketProtocol::close() 
{
    isExit = true;            //收到确认包
    {
        TMutexLocker lock(mutex);
        surePacket->wakeAll();    //唤醒发送包
    }

    {
        TMutexLocker lock(waitMutex);  
        if (!isPutEnd)
            pWaitCondition->wait(waitMutex,1000);    //使发送包超时条件变量唤醒,以退出发送线程
    }
}
/**
* 功能：释放对象对应的资源
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

/**************************************TCP协议************************************************/

RapidProtocol::RapidProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice) 
{
}

/**
* 功能：发送数据包
* @param  Packet* 数据包指针
* @return true表示发送成功,false表示发送失败
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
        return -1;    //包有问题 
    }  
    
    //发送数据包
    pDev->write(pack, size);
    return size;
}

/**
* 功能：发送数据包
* @param  Packet* 数据包指针
* @return true表示发送成功,false表示发送失败
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

    //接收数据包
    int recSize = pDev->read(pack,size) ;
    if (recSize <= 0) 
    {
        //连接失败
        sysLoger(LWARNING, "RapidProtocol::getpkt: socket error encounterd when receiving data content!");
        return -1;
    }
    return size;
}