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
 * @file     tcpDevice.h
 * @brief     tcp��ʽ���豸����
 * @author     ���
 * @date     2004-12-6
 * @version 3.0
 * @warning �Ͻ������ļ������ַ�
 *
 * ���ţ�ϵͳ�� 
 *
 */

#ifndef TCPDEVICE_INCLUDE
#define TCPDEVICE_INCLUDE


#include "device.h"
#include "tsInclude.h"
//#include "log.h"
#include "../include/wsocket.h"
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

#endif
