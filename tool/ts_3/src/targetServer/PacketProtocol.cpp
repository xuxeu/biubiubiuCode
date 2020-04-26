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
 * @brief     ��ϢЭ���
 * @author     ���
 * @date     2006-1-6
 * @version 3.0
 * @warning �Ͻ������ļ������ַ�
 *
 * ���ţ�ϵͳ�� 
 *
 */
#include ".\packetprotocol.h"
#include "Manager.h"
#include "common.h"
//#include "util.h"
//#include "Packet.h"
#include "deviceUdp.h"

//ͬ������Χ
const int SYNC_RANGE = 0x3;

//�������кŷ�Χ
const int RX_PACKET_MAX = 0x3fffffff;
const int RX_PACKET_MIN = 0x00000001;

//�����кŷ�Χ
const int TX_PACKET_MAX = 0x7fffffff;
const int TX_PACKET_MIN = 0x40000001;
const int RETRY_COUNT = -1;

PacketProtocol::PacketProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice)
{
    txPacketNum = TX_PACKET_MIN;
    isAck = true;
    retry = RETRY_COUNT;    //���Դ���Ĭ����-1
    timeout = 5000;
    rxPacketNum = RX_PACKET_MIN;
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
* @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
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
        retry = pDeviceUdp->getRetry();
    if (timeout != pDeviceUdp->getTimeout())
        timeout = pDeviceUdp->getTimeout();
start:
    pBuff = recvBuf;
    //����һ�����������ݰ�
    recSize = getDevice()->read(recvBuf,PACKET_HEADSIZE+PACKET_DATASIZE) ;

	if(recSize == -10054)	// �����յ�10054�Ĵ���ֱ�ӷ���
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
    {    //�ж��յ����Ƿ���ȷ�ϰ�
        if (strstr(recvBuf+sizeof(int),ACK) != NULL) 
        {    //���յ�ACK��,���ͷ������ź�
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
    __fetch(&serialNum,pBuff,sizeof(int));    //�õ����к�
    pBuff += sizeof(int);
    //�յ��Ѿ�������İ�
    /*
    if((rxPacketNum > serialNum) || 
        (rxPacketNum == RX_PACKET_MIN && serialNum != RX_PACKET_MIN))            
    {//ֻ�Ƿ���һ��ȷ�ϰ�
        sendAck(serialNum);
        goto start;
    }
    */
    if (rxPacketNum == serialNum)
    {
        //�ɹ�����,������ACK
        sendAck(serialNum);    //����ȷ�ϰ�
    }
    else if( (rxPacketNum>serialNum) && (rxPacketNum<serialNum+SYNC_RANGE) )
    {
        //���·���ACK
        sendAck(serialNum);
        goto start;
    }
    else
    {
        //���ݰ���ͬ��
        rxPacketNum = serialNum;
        sendAck(serialNum);    //����ȷ�ϰ�
    }

    if (size > PACKET_HEADSIZE+PACKET_DATASIZE) 
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: The size of packet data is error!");
        goto start;    //�������հ�
    }

    if (size != recSize-PACKET_HEADSIZE) 
    {
        sysLoger(LWARNING, "PacketProtocol::getpkt: the packet content size didn't match the "\
                                            "size of packet head");
        goto start;    //�������հ�
    }
    
    memcpy(pack->getData(),recvBuf+PACKET_HEADSIZE,size);    //���ð�������

    if (++rxPacketNum == RX_PACKET_MAX)    //���������¿�ʼ
        rxPacketNum = RX_PACKET_MIN;
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
    char sendBuf[PACKET_HEADSIZE+PACKET_DATASIZE];
    int putCount = 0;            //���ͼ���
    isAck = false;            //��û���յ��ظ���
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
    if (retry != pDeviceUdp->getRetry())    //����ֱ�Ӹ�ֵ����
        retry = pDeviceUdp->getRetry();
    if (timeout != pDeviceUdp->getTimeout())
        timeout = pDeviceUdp->getTimeout();
    pBuf = sendBuf;
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
    //���Ͱ�ͷ
    size =     pack->getSize();    

    if (size > PACKET_DATASIZE)
        return -1;    //��������
    else
        size = pack->getSize();

    //�����ݿ�������ͷ
    memcpy(sendBuf+PACKET_HEADSIZE,pack->getData(),size);
    isPutEnd = false;
start:
    //���Ͱ�
    getDevice()->write(sendBuf, PACKET_HEADSIZE+size);

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
           /*Modified by tangxp for BUG NO.0002555 on 2008.3.17 [begin]*/
            /*  Modified brief: �ط�ʱ��¼���Ͱ������к�*/
            sysLoger(LINFO, "PacketProtocol::putpkt: Send packet again! retried times:%d, des_aid: %d, SN: %d",
                                putCount, pack->getDes_aid(), pack->getSerialNum());
            /*Modified by tangxp for BUG NO.0002555 on 2008.3.17 [end]*/
            goto start;    //�������Ͱ�
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
        pWaitCondition = NULL;
    }
    if (waitMutex) 
    {
        delete waitMutex;
        waitMutex = NULL;
    }
}
