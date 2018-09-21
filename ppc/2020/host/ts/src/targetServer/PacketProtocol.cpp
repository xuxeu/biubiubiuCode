/*
* ������ʷ��
* 2006-1-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  packetProtocol.cpp
* @brief
*       ���ܣ�
*       <li>��ϢЭ���</li>
*/

/************************ͷ�ļ�********************************/
#include ".\packetprotocol.h"
#include "Manager.h"
#include "common.h"
#include "deviceUdp.h"
/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/
const int RETRY_COUNT = -1;

/*Modified by tangxp for BUG NO.3669 on 2008.7.22 [begin]*/
/*  Modified brief: ����TS��ʼ����,�������Ϊ�������*/
PacketProtocol::PacketProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice)
{
    txPacketNum = TX_START;
    isAck = true;
    retry = RETRY_COUNT;    //���Դ���Ĭ����-1
    timeout = 5000;
    rxPacketNum = RX_START;
    isExit = false;
    surePacket = new waitCondition();    //ȷ�ϻظ���
    mutex = new TMutex();                //�Ի�������������,
    pWaitCondition = new waitCondition();    //ȷ�ϻظ���
    waitMutex = new TMutex();                //�Ի�������������,
    isPutEnd = true;        //Ĭ������ǲ���Ҫ�ͷŷ����߳���Դ
}
/*Modified by tangxp for BUG NO.3669 on 2008.7.22 [end]*/

/**
* ���ܣ��������ݰ�
* @param  Packet* ���ݰ�ָ��
* @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
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
    int serialNum = 0;    //���а�

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

    if (retry != pDeviceUdp->getRetry())    //����ֱ�Ӹ�ֵ����
    {
        retry = pDeviceUdp->getRetry();
    }

    if (timeout != pDeviceUdp->getTimeout())
    {
        timeout = pDeviceUdp->getTimeout();
    }

start:
    pBuff = recvBuf;
    //����һ�����������ݰ�
    recSize = getDevice()->read(recvBuf,PACKET_CHECKSUMSIZE+PACKET_HEADSIZE+PACKET_DATASIZE) ;

    if (recSize <= 0)
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: Socket error encountered when receiving pack."\
                 " ErrorNo: %d", recSize);
        return recSize;
    }

    if (recSize == strlen(ACK) + sizeof(int))
    {
        //�ж��յ����Ƿ���ȷ�ϰ�
        if (strstr(recvBuf+sizeof(int),ACK) != NULL)
        {
            //���յ�ACK��,���ͷ������ź�
            //��ȡ���к�
            char *pBuf = recvBuf;
            int txNum;
            //�õ�ȷ�ϰ������к�
            __fetch(&txNum,pBuf,sizeof(int));

            //�յ�����������Ϣ��
            if (txPacketNum != txNum)
            {
                sysLoger(LWARNING, "PacketProtocol::getpkt: Received data was unknow");
                goto start;
            }

            //txPacketNum++;
            {
                TMutexLocker lock(mutex);
                isAck = true;            //�յ�ȷ�ϰ�
                surePacket->wakeAll();    //���ѷ��Ͱ�
            }
            goto start;
        }
    }

    //�������հ��е�У���
    pBuff += sizeof(short);

    //�Խ��յ��İ�ͷ��Ϣ���н���
    __fetch(&size,pBuff,sizeof(int));
    pBuff += sizeof(int);

    /* ����У��Ͳ�ȷ�Ͻ��հ��Ƿ�����*/
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
    __fetch(&serialNum,pBuff,sizeof(int));    //�õ����к�
    pBuff += sizeof(int);

    /*Modified by tangxp for BUG NO.3669 on 2008.7.24 [begin]*/
    /*  Modified brief: ��TS��TA��ͨ����Ŷθ�ΪΪ0x00000001~0x3fffffff,TA��1�η�����TS����Ź̶�Ϊ0x40000001.
        TS�յ����Ϊ0x40000001�����ݰ���,�����Լ��Ľ������Ϊ0x00000001,�������ͻظ�.
        TS��ʼ�������Ϊ0x40000002,TS������,�յ��κ����ݰ�����ͬ�����
        ����������Ŵﵽ���޺�����ͬ�����߼�*/
    sendAck(serialNum);    //����ȷ�ϰ�

    if(rxPacketNum == serialNum)
    {
        //�ɹ�����
    }
    else if(TA_TX_START == serialNum)
    {
        //TA��һ�η���
        rxPacketNum = RX_PACKET_MIN - 1;
    }

    else if(RX_START == rxPacketNum)
    {
        //TS����,ͬ�����
        rxPacketNum = serialNum;
    }

    else if( ((rxPacketNum>serialNum) && (rxPacketNum<=serialNum+SYNC_RANGE))
             || (serialNum+SYNC_RANGE >= RX_PACKET_MAX))
    {
		sysLoger(LWARNING, "PacketProtocol::getpkt: Received data was handled serialNum:%d",serialNum);
        //���·���ACK
        goto start;
    }

    else
    {
        //���ݰ���ͬ��
        rxPacketNum = serialNum;
    }

    /*Modified by tangxp for BUG NO.3669 on 2008.7.24 [end]*/

    if (size > PACKET_HEADSIZE+PACKET_DATASIZE)
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: The size of packet data is error!");
        goto start;    //�������հ�
    }

    if (size != recSize-PACKET_HEADSIZE-PACKET_CHECKSUMSIZE)
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: the packet content size didn't match the "\
                 "size of packet head");
        goto start;    //�������հ�
    }

    memcpy(pack->getData(),recvBuf+PACKET_CHECKSUMSIZE+PACKET_HEADSIZE,size);    //���ð�������

    if (++rxPacketNum == RX_PACKET_MAX)    //���������¿�ʼ
    {
        rxPacketNum = RX_PACKET_MIN;
    }

    pack->setSrc_said(src_said);
    pack->setDes_said(des_said);
    pack->setDes_aid(des_aid);
    pack->setSrc_aid(src_aid);
    pack->setSize(size);
    //�������к�
    pack->setSerialNum(serialNum);

    return size;
}
/**
����ȷ�ϰ�
*/
void PacketProtocol::sendAck(int serialNum)
{
    char buff[PACKET_HEADSIZE];
    char* pBuf = buff;
    __store(serialNum,pBuf,sizeof(int));
    pBuf += sizeof(int);
    memcpy(pBuf, ACK,strlen(ACK));
    pBuf += strlen(ACK);
    getDevice()->write(buff, pBuf-buff);    //����ȷ�ϰ�
}
/**
* ���ܣ��������ݰ�
* @param  Packet* ���ݰ�ָ��
* @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
*/
int PacketProtocol::putpkt(Packet* pack)
{
    char sendBuf[PACKET_CHECKSUMSIZE+PACKET_HEADSIZE+PACKET_DATASIZE];
    int putCount = 0;            //���ͼ���
    /*Modified by tangxp for BUG NO.3668 on 2008��7��18�� [begin]*/
    /*  Modified brief: ��isAck���ӻ��Ᵽ��*/
    mutex->lock();
    isAck = false;            //��û���յ��ظ���
    mutex->unlock();
    /*Modified by tangxp for BUG NO.3668 on 2008��7��18�� [end]*/
    char* pBuf = NULL;
    int size = 0;

    if(NULL == pack)
    {
        sysLoger(LWARNING, "PacketProtocol::putpkt: Bad pointer of pack, eixt sending data!");
        return -1;
    }

    //�õ���ʱ�����Դ���
    DeviceUdp* pDeviceUdp = dynamic_cast<DeviceUdp*>(getDevice());

    if(NULL == pDeviceUdp)
    {
        sysLoger(LWARNING, "PacketProtocol::putpkt: No usable device!");
        return -1;
    }

    size =     pack->getSize();

    if (size > TA_PACKET_DATASIZE)
    {
        return -1;    //��������
    }

    if (retry != pDeviceUdp->getRetry())    //����ֱ�Ӹ�ֵ����
    {
        retry = pDeviceUdp->getRetry();
    }

    if (timeout != pDeviceUdp->getTimeout())
    {
        timeout = pDeviceUdp->getTimeout();
    }

    *(short*)sendBuf = 0;
    pBuf = sendBuf+PACKET_CHECKSUMSIZE;

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
    __store(txPacketNum,pBuf,sizeof(int));    //����������к�
    //�������к�
    pack->setSerialNum(txPacketNum);

    //�����ݿ�������ͷ
    memcpy(sendBuf+PACKET_CHECKSUMSIZE+PACKET_HEADSIZE,pack->getData(),size);
    isPutEnd = false;

    //����У��ͣ�����У���д�뷢�ͻ���
    *(short*)sendBuf = getCheckSum(sendBuf, PACKET_CHECKSUMSIZE+PACKET_HEADSIZE+size);

start:
    //���Ͱ�
    getDevice()->write(sendBuf, PACKET_CHECKSUMSIZE+PACKET_HEADSIZE+size);

    {
        bool isSure = true;
        {
            TMutexLocker lock(mutex);

            if (!isAck)    //�����û���յ�ȷ�ϰ�,����ж���
            {
                isSure = surePacket->wait(mutex,timeout);    //��ʱ����ack��
            }
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
    {
        //�ط��˰�
        if (putCount < retry )
        {
            //���·��Ͱ�
            /*Modified by tangxp for BUG NO.0002555 on 2008.3.17 [begin]*/
            /*  Modified brief: �ط�ʱ��¼���Ͱ������к�*/
            sysLoger(LINFO, "PacketProtocol::putpkt: Send packet again! retried times:%d, des_aid: %d, SN: %d",
                     putCount, pack->getDes_aid(), pack->getSerialNum());
            /*Modified by tangxp for BUG NO.0002555 on 2008.3.17 [end]*/
            goto start;    //�������Ͱ�
        }

        return -2;//��ʱ
    }

    /*Modified by tangxp for BUG NO.3669 on 2008.7.22 [begin]*/
    /*  Modified brief: ��1�η���ʱ,ʹ���������,��ֹ��������ʱ,������*/
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
        {
            pWaitCondition->wait(waitMutex,1000);    //ʹ���Ͱ���ʱ������������,���˳������߳�
        }
    }
}

/**
* ����:��������Э���е�У���
*/
unsigned short PacketProtocol::getCheckSum(void * ptr,unsigned short count)
{
    unsigned short  tsCheckSum;
    unsigned short *addr = (unsigned short*)ptr;

    /* ����rfc1071��׼����ָ���ַ���У��� */

    /* ���峤���α�������У��� */
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

    /* ����ȡ����ļ���ֵ */
    return(~tsCheckSum);
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
        pWaitCondition = NULL;
    }

    if (waitMutex)
    {
        delete waitMutex;
        waitMutex = NULL;
    }
}
