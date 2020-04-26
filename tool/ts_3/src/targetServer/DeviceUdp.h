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
 * @file     deviceUdp.h
 * @brief     udp��ʽ���豸����
 * @author     ���
 * @date     2004-12-6
 * @version 3.0
 * @warning �Ͻ������ļ������ַ�
 *
 * ���ţ�ϵͳ�� 
 *
 */

#ifndef DEVICE_UDP_INCLUDE
#define DEVICE_UDP_INCLUDE

#include "device.h"
class QUdpSocket;
#include "tsInclude.h"
#include <winsock.h>
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

#endif //DEVICE_UDP_INCLUDE
