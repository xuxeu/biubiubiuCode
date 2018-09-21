/*
* 更改历史：
* 2006-1-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  packetProtocol.cpp
* @brief
*       功能：
*       <li>消息协议层</li>
*/

/************************头文件********************************/
#include ".\packetprotocol.h"
#include "Manager.h"
#include "common.h"
#include "deviceUdp.h"
/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/
const int RETRY_COUNT = -1;

/*Modified by tangxp for BUG NO.3669 on 2008.7.22 [begin]*/
/*  Modified brief: 设置TS起始发送,接收序号为特殊序号*/
PacketProtocol::PacketProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice)
{
    txPacketNum = TX_START;
    isAck = true;
    retry = RETRY_COUNT;    //重试次数默认是-1
    timeout = 5000;
    rxPacketNum = RX_START;
    isExit = false;
    surePacket = new waitCondition();    //确认回复包
    mutex = new TMutex();                //对缓冲区进行锁定,
    pWaitCondition = new waitCondition();    //确认回复包
    waitMutex = new TMutex();                //对缓冲区进行锁定,
    isPutEnd = true;        //默认情况是不需要释放发送线程资源
}
/*Modified by tangxp for BUG NO.3669 on 2008.7.22 [end]*/

/**
* 功能：发送数据包
* @param  Packet* 数据包指针
* @return true表示发送成功,false表示发送失败
*/
const char ACK[] = "+";
int PacketProtocol::getpkt(Packet* pack)
{
    char recvBuf[PACKET_CHECKSUMSIZE+PACKET_HEADSIZE+PACKET_DATASIZE];
    memset(recvBuf,0,PACKET_CHECKSUMSIZE+PACKET_HEADSIZE+PACKET_DATASIZE);
    int size;
    short src_aid;
    short src_said;
    short des_said;
    short des_aid;
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
    {
        retry = pDeviceUdp->getRetry();
    }

    if (timeout != pDeviceUdp->getTimeout())
    {
        timeout = pDeviceUdp->getTimeout();
    }

start:
    pBuff = recvBuf;
    //接收一个完整的数据包
    recSize = getDevice()->read(recvBuf,PACKET_CHECKSUMSIZE+PACKET_HEADSIZE+PACKET_DATASIZE) ;

    if (recSize <= 0)
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: Socket error encountered when receiving pack."\
                 " ErrorNo: %d", recSize);
        return recSize;
    }

    if (recSize == strlen(ACK) + sizeof(int))
    {
        //判断收到的是否是确认包
        if (strstr(recvBuf+sizeof(int),ACK) != NULL)
        {
            //接收到ACK包,则释放条件信号
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

    //跳过接收包中的校验和
    pBuff += sizeof(short);

    //对接收到的包头信息进行解码
    __fetch(&size,pBuff,sizeof(int));
    pBuff += sizeof(int);

    /* 计算校验和并确认接收包是否完整*/
    if(getCheckSum(recvBuf, PACKET_CHECKSUMSIZE+PACKET_HEADSIZE+size) != 0)
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: check sum error\n");
        goto start;
    }

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

    /*Modified by tangxp for BUG NO.3669 on 2008.7.24 [begin]*/
    /*  Modified brief: 将TS和TA的通信序号段改为为0x00000001~0x3fffffff,TA第1次发包给TS的序号固定为0x40000001.
        TS收到序号为0x40000001的数据包后,更新自己的接收序号为0x00000001,并处理发送回复.
        TS起始接收序号为0x40000002,TS重启后,收到任何数据包立即同步序号
        修正发送序号达到上限后的序号同步的逻辑*/
    sendAck(serialNum);    //发送确认包

    if(rxPacketNum == serialNum)
    {
        //成功接收
    }
    else if(TA_TX_START == serialNum)
    {
        //TA第一次发包
        rxPacketNum = RX_PACKET_MIN - 1;
    }

    else if(RX_START == rxPacketNum)
    {
        //TS重启,同步序号
        rxPacketNum = serialNum;
    }

    else if( ((rxPacketNum>serialNum) && (rxPacketNum<=serialNum+SYNC_RANGE))
             || (serialNum+SYNC_RANGE >= RX_PACKET_MAX))
    {
		sysLoger(LWARNING, "PacketProtocol::getpkt: Received data was handled serialNum:%d",serialNum);
        //重新返回ACK
        goto start;
    }

    else
    {
        //数据包不同步
        rxPacketNum = serialNum;
    }

    /*Modified by tangxp for BUG NO.3669 on 2008.7.24 [end]*/

    if (size > PACKET_HEADSIZE+PACKET_DATASIZE)
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: The size of packet data is error!");
        goto start;    //继续重收包
    }

    if (size != recSize-PACKET_HEADSIZE-PACKET_CHECKSUMSIZE)
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: the packet content size didn't match the "\
                 "size of packet head");
        goto start;    //继续重收包
    }

    memcpy(pack->getData(),recvBuf+PACKET_CHECKSUMSIZE+PACKET_HEADSIZE,size);    //设置包体数据

    if (++rxPacketNum == RX_PACKET_MAX)    //大于则重新开始
    {
        rxPacketNum = RX_PACKET_MIN;
    }

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
    char sendBuf[PACKET_CHECKSUMSIZE+PACKET_HEADSIZE+PACKET_DATASIZE];
    int putCount = 0;            //发送计数
    /*Modified by tangxp for BUG NO.3668 on 2008年7月18日 [begin]*/
    /*  Modified brief: 对isAck增加互斥保护*/
    mutex->lock();
    isAck = false;            //还没有收到回复包
    mutex->unlock();
    /*Modified by tangxp for BUG NO.3668 on 2008年7月18日 [end]*/
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

    size =     pack->getSize();

    if (size > TA_PACKET_DATASIZE)
    {
        return -1;    //包有问题
    }

    if (retry != pDeviceUdp->getRetry())    //不如直接赋值更快
    {
        retry = pDeviceUdp->getRetry();
    }

    if (timeout != pDeviceUdp->getTimeout())
    {
        timeout = pDeviceUdp->getTimeout();
    }

    *(short*)sendBuf = 0;
    pBuf = sendBuf+PACKET_CHECKSUMSIZE;

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

    //把数据拷贝到包头
    memcpy(sendBuf+PACKET_CHECKSUMSIZE+PACKET_HEADSIZE,pack->getData(),size);
    isPutEnd = false;

    //计算校验和，并将校验和写入发送缓冲
    *(short*)sendBuf = getCheckSum(sendBuf, PACKET_CHECKSUMSIZE+PACKET_HEADSIZE+size);

start:
    //发送包
    getDevice()->write(sendBuf, PACKET_CHECKSUMSIZE+PACKET_HEADSIZE+size);

    {
        bool isSure = true;
        {
            TMutexLocker lock(mutex);

            if (!isAck)    //如果还没有收到确认包,则进行堵塞
            {
                isSure = surePacket->wait(mutex,timeout);    //超时堵塞ack包
            }
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
    {
        //重发此包
        if (putCount < retry )
        {
            //重新发送包
            /*Modified by tangxp for BUG NO.0002555 on 2008.3.17 [begin]*/
            /*  Modified brief: 重发时记录发送包的序列号*/
            sysLoger(LINFO, "PacketProtocol::putpkt: Send packet again! retried times:%d, des_aid: %d, SN: %d",
                     putCount, pack->getDes_aid(), pack->getSerialNum());
            /*Modified by tangxp for BUG NO.0002555 on 2008.3.17 [end]*/
            goto start;    //继续发送包
        }

        return -2;//超时
    }

    /*Modified by tangxp for BUG NO.3669 on 2008.7.22 [begin]*/
    /*  Modified brief: 第1次发包时,使用特殊序号,防止反复连接时,被过滤*/
    if((TX_START == txPacketNum)
            || (TX_PACKET_MAX == ++txPacketNum))
    {
        txPacketNum = TX_PACKET_MIN;
    }

    /*Modified by tangxp for BUG NO.3669 on 2008.7.22 [end]*/
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
        {
            pWaitCondition->wait(waitMutex,1000);    //使发送包超时条件变量唤醒,以退出发送线程
        }
    }
}

/**
* 功能:计算网络协议中的校验和
*/
unsigned short PacketProtocol::getCheckSum(void * ptr,unsigned short count)
{
    unsigned short  tsCheckSum;
    unsigned short *addr = (unsigned short*)ptr;

    /* 按照rfc1071标准计算指定字符的校验和 */

    /* 定义长整形变量保存校验和 */
    unsigned long sum = 0;

    while (count > 1)
    {
        sum += *addr++;
        count -= 2;
    }

    if (count > 0)
    {
        sum += * (unsigned char *) addr;
    }

    while (sum>>16)
    {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    tsCheckSum = (unsigned short)sum;

    /* 返回取反后的计算值 */
    return(~tsCheckSum);
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
