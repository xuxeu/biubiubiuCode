/*
* ������ʷ��
*  2006-1-11 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  streamProtocol.h
* @brief
*       ���ܣ�
*       <li>��Э���</li>
*/
#ifndef STREAMPROTOCOL_H
#define STREAMPROTOCOL_H

/************************ͷ�ļ�********************************/
#pragma once
#include "protocol.h"
/************************�궨��********************************/
/************************���Ͷ���******************************/

class DeviceBase;
class StreamProtocol :
    public ProtocolBase
{
    bool isExit;            //���ͺ����˳���־
    int txPacketNum;    //��ǰ���к�
    int rxPacketNum;        //���հ�����
    int timeout;                //�ȴ�ȷ�ϰ���ʱʱ��
    int retry;            //���Դ���
    waitCondition surePacket;    //ȷ�ϻظ���
    TMutex mutex;                //�Ի�������������,
    //    TMutex mutexLock;                //�Ի�������������,
    waitCondition m_waitCondition;    //ȷ�ϻظ���
    TMutex waitMutex;                //�Ի�������������,
    TMutex putMutex;                //���ͺ�������
    volatile int  waiting_for_ack;            //�õ�ȷ�ϰ�
public:
    virtual ~StreamProtocol(void);
    /* ���ܣ��������ݰ�
    * @param  Packet* ���ݰ�ָ��
    * @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
    */
    virtual int putpkt(Packet* pack) ;
    /**
    * ���ܣ��õ����ݰ�
    * @param  Packet* �������ݰ�
    * @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
    */
    virtual int getpkt(Packet* pack);
    /**
    * ���ܣ��ͷŶ����Ӧ����Դ
    */
    virtual void close();
    /**
    * ���ܣ����캯��,����Э���Ӧ���豸
    */
    StreamProtocol(DeviceBase* pDevice);
private:
    void DataError(int type);

    /**
    * ���ܣ����Ϳ��ư�
    * @param  control ---�����͵Ŀ����ź�
    * @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
    */
    void sendCtrlPack(char control,int serialNum);
    /**
    * ���ܣ�������ȴ�host����Ӧ�����ж��Ƿ���Ҫ�ط���
    * @return �����Ҫ�ط��򷵻�true�����򷵻�false��
    */
    bool NeedSendAgain(void);

    /**
    * ���ܣ����տ��ư�
    * @return ���ؿ����ź�
    */
    char GetCtrlPack(void);
/************************�ӿ�����******************************/

};
#endif