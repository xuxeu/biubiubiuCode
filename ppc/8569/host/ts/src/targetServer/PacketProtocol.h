/*
* ������ʷ��
* 2006-1-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  packetProtocol.h
* @brief
*       ���ܣ�
*       <li>��ϢЭ���</li>
*/



#pragma once
/************************ͷ�ļ�********************************/
#include "protocol.h"
#include "tsInclude.h"

#ifndef PACKETPROTOCOL_H
#define PACKETPROTOCOL_H
/************************�궨��********************************/

#define PACKET_CHECKSUMSIZE 2
/************************���Ͷ���******************************/
class Packet;
class PacketProtocol : public ProtocolBase
{
    bool isPutEnd;            //���ͽ����ı��,�Ա�Э����Ҫ����ʱ���յ�ȷ�ϰ���Ϣ,false��ʾû���յ�,true��ʾ�յ�
    bool isExit;            //�˳���־
    int txPacketNum;    //��ǰ���к�
    int rxPacketNum;        //���հ�����
    waitCondition *surePacket;    //ȷ�ϻظ���
    TMutex *mutex;                //�Ի�������������,
    waitCondition *pWaitCondition;    //ȷ�ϻظ���
    TMutex *waitMutex;                //�Ի�������������,
    int timeout;                //�ȴ�ȷ�ϰ���ʱʱ��
    bool isAck;                    //�Ƿ��յ�ȷ�ϰ�,��ʼ��ʱΪture,ÿ����һ������Ϊfalse,�յ��ظ�������Ϊtrue
    int retry;            //���Դ���
    /**
    ����ȷ�ϰ�
    */
    void sendAck(int serialNum) ;
public:
    bool getPutState()
    {
        return isPutEnd;
    }
    PacketProtocol(DeviceBase* pDevice);
    /**
    * ���ܣ��������ݰ�
    * @param  Packet* ���ݰ�ָ��
    * @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
    */
    virtual int putpkt(Packet* pack) ;
    /**
    * ���ܣ��������ݰ�
    * @param  Packet* ���ݰ�ָ��
    * @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
    */
    virtual int getpkt(Packet* pack) ;
    /**
    * ���ܣ��ͷŶ����Ӧ����Դ
    */
    virtual void close();
    /**
    * ���ܣ��ͷŶ����Ӧ����Դ
    */
    ~PacketProtocol();

private:
    unsigned short  getCheckSum(void *ptr, unsigned short count);
};

/************************�ӿ�����******************************/
#endif