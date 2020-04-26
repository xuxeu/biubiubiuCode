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
 * @file     packetProtocol.cpp
 * @brief     消息协议层
 * @author     彭宏
 * @date     2006-1-6
 * @version 3.0
 * @warning 严禁将本文件到处分发
 *
 * 部门：系统部 
 *
 */
#include ".\packetprotocol.h"
#include "Manager.h"
#include "common.h"
//#include "util.h"
//#include "Packet.h"
#include "deviceUdp.h"

//同步允许范围
const int SYNC_RANGE = 0x3;

//接收序列号范围
const int RX_PACKET_MAX = 0x3fffffff;
const int RX_PACKET_MIN = 0x00000001;

//发送列号范围
const int TX_PACKET_MAX = 0x7fffffff;
const int TX_PACKET_MIN = 0x40000001;
const int RETRY_COUNT = -1;

PacketProtocol::PacketProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice)
{
    txPacketNum = TX_PACKET_MIN;
    isAck = true;
    retry = RETRY_COUNT;    //重试次数默认是-1
    timeout = 5000;
    rxPacketNum = RX_PACKET_MIN;
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
* @return true表示发送成功,false表示发送失败
*/
const char ACK[] = "+";
int PacketProtocol::getpkt(Packet* pack) 
{
    char recvBuf[PACKET_HEADSIZE+PACKET_DATASIZE];     
    memset(recvBuf,0,PACKET_HEADSIZE+PACKET_DATASIZE);
    int size;
    short src_aid,src_said,des_said,des_aid;
    char* pBuff = NULL;
    int recSize = 0;
    int serialNum = 0;    //序列包
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
    if (retry != pDeviceUdp->getRetry())    //不如直接赋值更快
        retry = pDeviceUdp->getRetry();
    if (timeout != pDeviceUdp->getTimeout())
        timeout = pDeviceUdp->getTimeout();
start:
    pBuff = recvBuf;
    //接收一个完整的数据包
    recSize = getDevice()->read(recvBuf,PACKET_HEADSIZE+PACKET_DATASIZE) ;

	if(recSize == -10054)	// 对于收到10054的错误，直接返回
	{
		return recSize;
	}
    if (recSize <= 0) 
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: Socket error encountered when receiving pack."\
                                            " ErrorNo: %d", recSize);
        return -1;
    }

    if (recSize == strlen(ACK) + sizeof(int))
    {    //判断收到的是否是确认包
        if (strstr(recvBuf+sizeof(int),ACK) != NULL) 
        {    //接收到ACK包,则释放条件信号
            //读取序列号
            char *pBuf = recvBuf;
            int txNum;
            //得到确认包的序列号
            __fetch(&txNum,pBuf,sizeof(int));
            //收到不期望的消息包
            if (txPacketNum != txNum)    
            {
                sysLoger(LWARNING, "PacketProtocol::getpkt: Received data was unknow");
                goto start;
            }
            //txPacketNum++;
            {
                TMutexLocker lock(mutex);
                isAck = true;            //收到确认包
                surePacket->wakeAll();    //唤醒发送包
            }
            goto start;
        }
    }
    
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
    __fetch(&serialNum,pBuff,sizeof(int));    //得到序列号
    pBuff += sizeof(int);
    //收到已经处理过的包
    /*
    if((rxPacketNum > serialNum) || 
        (rxPacketNum == RX_PACKET_MIN && serialNum != RX_PACKET_MIN))            
    {//只是发送一个确认包
        sendAck(serialNum);
        goto start;
    }
    */
    if (rxPacketNum == serialNum)
    {
        //成功接收,并返回ACK
        sendAck(serialNum);    //发送确认包
    }
    else if( (rxPacketNum>serialNum) && (rxPacketNum<serialNum+SYNC_RANGE) )
    {
        //重新返回ACK
        sendAck(serialNum);
        goto start;
    }
    else
    {
        //数据包不同步
        rxPacketNum = serialNum;
        sendAck(serialNum);    //发送确认包
    }

    if (size > PACKET_HEADSIZE+PACKET_DATASIZE) 
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: The size of packet data is error!");
        goto start;    //继续重收包
    }

    if (size != recSize-PACKET_HEADSIZE) 
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: the packet content size didn't match the "\
                                            "size of packet head");
        goto start;    //继续重收包
    }
    
    memcpy(pack->getData(),recvBuf+PACKET_HEADSIZE,size);    //设置包体数据

    if (++rxPacketNum == RX_PACKET_MAX)    //大于则重新开始
        rxPacketNum = RX_PACKET_MIN;
    pack->setSrc_said(src_said);
    pack->setDes_said(des_said);
    pack->setDes_aid(des_aid);
    pack->setSrc_aid(src_aid);
    pack->setSize(size);
    //设置序列号
    pack->setSerialNum(serialNum);

    return size;
}
/**
发送确认包
*/
void PacketProtocol::sendAck(int serialNum) 
{
    char buff[PACKET_HEADSIZE];
    char* pBuf = buff;
    __store(serialNum,pBuf,sizeof(int));
    pBuf += sizeof(int);
    memcpy(pBuf, ACK,strlen(ACK));
    pBuf += strlen(ACK);
    getDevice()->write(buff, pBuf-buff);    //发送确认包
}
/**
* 功能：发送数据包
* @param  Packet* 数据包指针
* @return true表示发送成功,false表示发送失败
*/
int PacketProtocol::putpkt(Packet* pack) 
{
    char sendBuf[PACKET_HEADSIZE+PACKET_DATASIZE];
    int putCount = 0;            //发送计数
    isAck = false;            //还没有收到回复包
    char* pBuf = NULL;
    int size = 0;
    if(NULL == pack)
    {
        sysLoger(LWARNING, "PacketProtocol::putpkt: Bad pointer of pack, eixt sending data!");
        return -1;
    }
    //得到超时和重试次数
    DeviceUdp* pDeviceUdp = dynamic_cast<DeviceUdp*>(getDevice());
    if(NULL == pDeviceUdp)
    {
        sysLoger(LWARNING, "PacketProtocol::putpkt: No usable device!");
        return -1;
    }
    if (retry != pDeviceUdp->getRetry())    //不如直接赋值更快
        retry = pDeviceUdp->getRetry();
    if (timeout != pDeviceUdp->getTimeout())
        timeout = pDeviceUdp->getTimeout();
    pBuf = sendBuf;
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
    __store(txPacketNum,pBuf,sizeof(int));    //保存包的序列号
    //设置序列号
    pack->setSerialNum(txPacketNum);
    //发送包头
    size =     pack->getSize();    

    if (size > PACKET_DATASIZE)
        return -1;    //包有问题
    else
        size = pack->getSize();

    //把数据拷贝到包头
    memcpy(sendBuf+PACKET_HEADSIZE,pack->getData(),size);
    isPutEnd = false;
start:
    //发送包
    getDevice()->write(sendBuf, PACKET_HEADSIZE+size);

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
           /*Modified by tangxp for BUG NO.0002555 on 2008.3.17 [begin]*/
            /*  Modified brief: 重发时记录发送包的序列号*/
            sysLoger(LINFO, "PacketProtocol::putpkt: Send packet again! retried times:%d, des_aid: %d, SN: %d",
                                putCount, pack->getDes_aid(), pack->getSerialNum());
            /*Modified by tangxp for BUG NO.0002555 on 2008.3.17 [end]*/
            goto start;    //继续发送包
        }
        return -1;
    }
    txPacketNum++;
    if (txPacketNum == TX_PACKET_MAX)
        txPacketNum = TX_PACKET_MIN;
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
        pWaitCondition = NULL;
    }
    if (waitMutex) 
    {
        delete waitMutex;
        waitMutex = NULL;
    }
}
