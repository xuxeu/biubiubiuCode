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
* @file     protocol.h
* @brief     TSЭ������
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/

#ifndef PROTOCOL_INCLUDE
#define PROTOCOL_INCLUDE


#include "common.h"
#include "Device.h"

class DeviceBase;
class ProtocolBase {
private:
    DeviceBase* pDevice;
public:
    static const int TCP_PROTOCOL = 0;
    static const int PACKET_PROTOCOL = 1;
    static const int STREAM_PROTOCOL = 2;
    static const int USB_PROTOCOL = 3;
    /**
    * ���ܣ�ͨ������������pack�İ�ͷ
    * @param  pBuff �����ͷ�Ļ���������,����Ҫ���ڰ�ͷ�Ĵ�С
    * @param  pack[OUT] ���ð�ͷ
    */
    void readPacketHead(char* pBuff,Packet* pack) ;
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
    virtual int putpkt(Packet* pack) = 0;
    /**
    * ���ܣ��õ����ݰ�
    * @param  Packet* �������ݰ�
    * @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
    */
    virtual int getpkt(Packet* pack) = 0;
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

class RapidProtocol : public ProtocolBase 
    {
public:
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
    RapidProtocol(DeviceBase* pDevice);
    /**
    * ���ܣ��ͷŶ����Ӧ����Դ
    */
    virtual ~RapidProtocol(){
    }
};

/*USB�豸����Ӧ��Э��*/
class UsbProtocol : public ProtocolBase 
{
public:
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
    UsbProtocol(DeviceBase* pDevice);
    /**
    * ���ܣ��ͷŶ����Ӧ����Դ
    */
    virtual ~UsbProtocol()
    {
    }
};

/**
* ģ�飺  Э������
* ���ܣ�  Э�����
*/
class ProtocolManage {
private:
    static ProtocolManage* pProtocolManage;
    static TMutex mutex;                        //�������
public:
    static ProtocolManage* getInstance()
    {
        if (pProtocolManage == NULL) 
        {
            TMutexLocker lock(&mutex);
            if (pProtocolManage == NULL)
                pProtocolManage = new ProtocolManage(); 
        }

        return pProtocolManage;
    }
    /**
    * ���ܣ��ṩ����Э��ķ���
    * @param  Э������
    * @param  Э���Ӧ���豸����ָ��
    * @return һ������Э��
    */
    static ProtocolBase* createProtocol(int type,DeviceBase* pDevice);
};

#endif







