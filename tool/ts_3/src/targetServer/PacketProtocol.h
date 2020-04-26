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
 * @file     packetProtocol.h
 * @brief     ��ϢЭ���
 * @author     ���
 * @date     2006-1-6
 * @version 3.0
 * @warning �Ͻ������ļ������ַ�
 *
 * ���ţ�ϵͳ�� 
 *
 */
#pragma once

#include "protocol.h"
#include "tsInclude.h"
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
};
