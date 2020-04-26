/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
* @file 	PortocolRsp.h
* @brief 	
*     <li>���ܣ� ����tcp,udpͨ�ŷ�ʽ��Э��������</li>
* @author 	������
* @date 	2007-10-23
* <p>���ţ�ϵͳ�� 
*/

#ifndef _PROTOCOLRSP_H_
#define _PROTOCOLRSP_H_

//#include "stdafx.h"
#include "DeviceRsp.h"
#include "../../include/sysTypes.h"
#include "common.h"

class ProtocolBase 
{
private:
    DeviceBase* pDevice;
public:
    static const int TCP_PROTOCOL = 0;
    static const int PACKET_PROTOCOL = 1;
    //static const int STREAM_PROTOCOL = 2;
    //static const int USB_PROTOCOL = 3;

    /**
    * ���ܣ��õ�Э���Ӧ���豸����
    * @return DeviceBase* �豸����ָ��
    */
    DeviceBase* getDevice() 
    {
        return pDevice;
    }

    /**
    * ���ܣ��������ݰ�
    * @param  Packet* ���ݰ�ָ��
    * @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
    */
    virtual int putpkt(T_CHAR* pack,T_WORD size) = 0;

    /**
    * ���ܣ��õ����ݰ�
    * @param  Packet* �������ݰ�
    * @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
    */
    virtual int getpkt(T_CHAR* pack,T_WORD size) = 0;

    /**
    * ���ܣ��ͷŶ����Ӧ����Դ
    */
    virtual void close();

    /**
    * ���ܣ����캯��,����Э���Ӧ���豸
    */
    ProtocolBase(DeviceBase* pDevice) ;

    /**
    * ���ܣ��ͷŶ����Ӧ����Դ
    */
    virtual ~ProtocolBase()
    {
    }

};

class PacketProtocol : public ProtocolBase
{
    bool isPutEnd;            //���ͽ����ı��,�Ա�Э����Ҫ����ʱ���յ�ȷ�ϰ���Ϣ,false��ʾû���յ�,true��ʾ�յ�
    bool isExit;            //�˳���־
    waitCondition *surePacket;    //ȷ�ϻظ���
    TMutex *mutex;                //�Ի�������������,
    waitCondition *pWaitCondition;    //ȷ�ϻظ���
    TMutex *waitMutex;                //�Ի�������������,
    int timeout;                //�ȴ�ȷ�ϰ���ʱʱ��
    bool isAck;                    //�Ƿ��յ�ȷ�ϰ�,��ʼ��ʱΪture,ÿ����һ������Ϊfalse,�յ��ظ�������Ϊtrue
    int retry;            //���Դ���

    /**
    * ���ܣ�����ȷ�ϰ�
    */
    //void sendAck() ;
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
    virtual int putpkt(T_CHAR* pack,T_WORD size) ;

    /**
    * ���ܣ��������ݰ�
    * @param  Packet* ���ݰ�ָ��
    * @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
    */
    virtual int getpkt(T_CHAR* pack,T_WORD size) ;

    /**
    * ���ܣ��ͷŶ����Ӧ����Դ
    */
    virtual void close();

    /**
    * ���ܣ��ͷŶ����Ӧ����Դ
    */
    ~PacketProtocol();
};


class RapidProtocol : public ProtocolBase 
    {
public:
    RapidProtocol(DeviceBase* pDevice);

    /**
    * ���ܣ��������ݰ�
    * @param  Packet* ���ݰ�ָ��
    * @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
    */
    virtual int putpkt(T_CHAR* pack,T_WORD size) ;
    /**
    * ���ܣ��������ݰ�
    * @param  Packet* ���ݰ�ָ��
    * @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
    */
    virtual int getpkt(T_CHAR* pack,T_WORD size) ;

    /**
    * ���ܣ��ͷŶ����Ӧ����Դ
    */
    virtual ~RapidProtocol()
    {
    }
};

#endif