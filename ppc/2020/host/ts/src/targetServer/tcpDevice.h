/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  tcpDevice.h
* @brief
*       ���ܣ�
*       <li> tcp��ʽ���豸����</li>
*/



#ifndef TCPDEVICE_INCLUDE
#define TCPDEVICE_INCLUDE

/************************ͷ�ļ�********************************/
#include "device.h"
#include "tsInclude.h"
//#include "log.h"
#include "../include/wsocket.h"
/************************�궨��********************************/
/************************���Ͷ���******************************/
class DeviceTcp : public DeviceBase
{
    //    Q_OBJECT
private:
    //FOR WINDOW
    bool connectServer(ServerConfig* config) ;
    WSocket        m_socket;
    //END
    int port;                            //TCP�˿ں�
    QString IP;                            //TCP��ip��ַ
    //int socketDescriptor;                //���ӵ�socket���,Ŀǰû��ʲô�ô�
public:

    DeviceTcp()
    {
        port = 0;
        IP = QString("");
    }
    ~DeviceTcp();
    /**
    * ����һ����������socket.
    * @param port �˿ں�
    */
    bool createServerSocket(int port);
    /**
    * ��Ϊ����˵ȴ��ͻ�������,�����ȴ�����
    * @param s ���Ӻ�Ķ���
    */
    bool acceptSocket(WSocket& s) ;
    /**
    * ����socket��.
    * @param socketDescriptor socket��
    */
    void setTcpSocket(int socketDescriptor) ;
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
    virtual int  read(char* pBuff, int size)
    {
        int ret = m_socket.Recv(pBuff,size,0);
        sysLoger(LDEBUG, "DeviceTCP::read: Recieved packet, size:%d", ret);
        return ret;
    }

    /**
    * ���豸д����,��û��д��size����һֱ�ȴ�
    * @param pBuff д���ݵĻ�����
    * @param size д���ݵĴ�С
    */
    virtual void write(char* pBuff, int size)
    {
        int ret = m_socket.Send(pBuff,size);
        sysLoger(LDEBUG, "DeviceTCP::write: Sended packet, size:%d", ret);
    }
    /**
    * ��������socket
    */
    virtual bool connect();
};
/************************�ӿ�����******************************/
#endif
