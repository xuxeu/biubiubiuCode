/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  protocol.h
* @brief
*       ���ܣ�
*       <li> TSЭ������</li>
*/


#ifndef PROTOCOL_INCLUDE
#define PROTOCOL_INCLUDE

/************************ͷ�ļ�********************************/

#include "common.h"
#include "Device.h"
/************************�궨��********************************/
/*Modified by tangxp for BUG NO.3669 on 2008.7.21 [begin]*/
/*  Modified brief: ��ͨ��Э�鹫���궨������Protocl.h,
                            ������Ŷθ�Ϊ�뷢����Ŷ���ͬ
                            ����TS,TA��ʼ����,�������*/
//ͬ������Χ
#define SYNC_RANGE 0x1

//�������кŷ�Χ
#define RX_PACKET_MAX 0x3fffffff
#define RX_PACKET_MIN 0x00000001

//�����кŷ�Χ
#define TX_PACKET_MAX RX_PACKET_MAX
#define TX_PACKET_MIN RX_PACKET_MIN

//TS��ʼ����,�������
#define TX_START 0x40000001
#define RX_START 0x40000002

//TA��ʼ�������
#define TA_TX_START 0x40000001
/*Modified by tangxp for BUG NO.3669 on 2008.7.21 [end]*/


/************************���Ͷ���******************************/
class DeviceBase;
class ProtocolBase
{
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
    virtual ~RapidProtocol()
    {
    }
};



/**
* ģ�飺  Э������
* ���ܣ�  Э�����
*/
class ProtocolManage
{
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
            {
                pProtocolManage = new ProtocolManage();
            }
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
/************************�ӿ�����******************************/
#endif







