/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  deviceUdp.h
* @brief
*       ���ܣ�
*       <li>udp��ʽ���豸����</li>
*/



#ifndef DEVICE_UDP_INCLUDE
#define DEVICE_UDP_INCLUDE
/************************ͷ�ļ�********************************/
#include "device.h"

#include "tsInclude.h"
#include <winsock.h>

/************************�궨��********************************/
/************************���Ͷ���******************************/
class QUdpSocket;
typedef int                socklen_t;

class DeviceUdp : public DeviceBase
{
    int port;                            //UDP�˿ں�
    QString IP;                            //UDP��ip��ַ
    waitCondition surePacket;            //��ӱ���,�����ж��Ƿ���յ�ACK��
    TMutex mutex;                        //�Ի�������������,��Ҫ�������������ʹ��
    int timeout;                        //��ʱʱ��,��λ����
    int retry;                            //���Դ���
    struct sockaddr_in      addr;        //Ҫ���͵ĵ�ַ
    struct sockaddr_in      localAddr;    //������ַ
    SOCKET pUdpSocket;
public:
    /**
    * ���ܣ��õ���ʱ
    */
    int getTimeout()
    {
        return timeout;
    }
    /**
    * ���ܣ����ó�ʱ
    */
    void setTimeout(int timeout)
    {
        this->timeout = timeout;
    }

    /**
    * ���ܣ��õ����Դ���
    */
    int getRetry()
    {
        return retry;
    }
    /**
    * ���ܣ��������Դ���
    */
    void setRetry(int retry)
    {
        this->retry = retry;
    }
    DeviceUdp(void);
    virtual ~DeviceUdp(void);
    /**
    * ͨ��������Ϣ���豸.
    * @param ServerConfig* ������Ϣ
    * @return true�򿪳ɹ�,false��ʧ��
    */
    virtual bool open(ServerConfig* config);
    /**
    * �ر��豸.
    * @return true�رճɹ�,false�ر�ʧ��
    */
    virtual bool close();

    /**
    * ���豸�ж�ȡ����.,��û�ж�ȡsize����һֱ�ȴ�
    * @param pBuff ��ȡ��������Ļ�����
    * @param size Ҫ��ȡ���ݵĴ�С
    * @param timeout �ȴ�ʱ��
    * @return int��ȡ������
    */
    virtual int  read(char* pBuff, int size);
    /**
    * ���豸д����,��û��д��size����һֱ�ȴ�
    * @param pBuff д���ݵĻ�����
    * @param size д���ݵĴ�С
    */
    virtual void write(char* pBuff, int size) ;
    /**
    * ��������socket,Ŀǰ��û��ʵ��
    */
    virtual bool connect();
};
/************************�ӿ�����******************************/
#endif //DEVICE_UDP_INCLUDE
