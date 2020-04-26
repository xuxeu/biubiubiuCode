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
 * @file     streamProtocol.h
 * @brief     ��Э���
 * @author     ���
 * @date     2006-1-11
 * @version 3.0
 * @warning �Ͻ������ļ������ַ�
 *
 * ���ţ�ϵͳ�� 
 *
 */
#pragma once
#include "protocol.h"
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

};
