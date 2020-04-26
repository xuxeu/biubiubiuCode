/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
* @file 	DeviceRsp.h
* @brief 	
*     <li>���ܣ� ����tcp,udpͨ�ŷ�ʽ���豸�������</li>
* @author 	������
* @date 	2007-10-23
* <p>���ţ�ϵͳ�� 
*/

#ifndef _DEVICERSP_H_
#define _DEVICERSP_H_

//#include "stdafx.h"
//#include "tsinclude.h"
//#include <winsock.h>
#include "windows.h"
#include "wsocket.h"
//#include <QString>
#include "common.h"

#define IP_LEN  16

class DeviceBase
{
private:
public:
    virtual ~DeviceBase(){}

    /**
     * ���ܣ��򿪾����豸
     * @return true��ʾ�ɹ�,false��ʾʧ��
     */
    virtual bool open(char *ip ,int port ,int timeout ,int retry) = 0;

    /**
     * ���ܣ��رվ����豸
     * @return true��ʾ�ɹ�,false��ʾʧ��
     */
    virtual bool close() = 0;

    /**
     * ���ܣ����豸�ж�ȡ��Ϣ
     * @param  pBuff �õ���Ϣ������
     * @param  size  �������Ĵ�С
     * @param  timeout �ȴ�ʱ��,-1��ʾ����ȴ�,
     * @return ���豸�еõ������ݴ�С
     */
    virtual int  read(char* pBuff, int size) = 0;

    /**
     * ���ܣ����豸��������
     * @param  pBuff ������Ϣ��
     * @param  size ��Ϣ����С
     */
    virtual void write(char* pBuff, int size) = 0;

    /**
     * ���ܣ�����豸�������
     * @return  true��ʾ���ӳɹ�
     */
    virtual bool connect() = 0;
};

class DeviceUdp : public DeviceBase
{
    int port;                            //UDP�˿ں�
    char IP[IP_LEN];                     //UDP��ip��ַ
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

    DeviceUdp();

    ~DeviceUdp();
    /**
    * ͨ��������Ϣ���豸.
    * @param *inIp ���շ�IP��ַ
    * @param inPort ���շ��˿�
    * @param inTimeout ��ʱʱ��
    * @param inRetry ���Դ���
    * @return true�򿪳ɹ�,false��ʧ��
    */
    virtual bool open(char *ip ,int port ,int timeout ,int retry);
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

class DeviceTcp : public DeviceBase
{
private:
    //FOR WINDOW
    //bool connectServer(ServerConfig* config) ;
    WSocket        m_socket;
    //END
    int port;                            //TCP�˿ں�
    char IP[IP_LEN];                     //TCP��ip��ַ
    //int socketDescriptor;              //���ӵ�socket���,Ŀǰû��ʲô�ô�
public:

    DeviceTcp()
    {
        port = 0;
        memset(IP,0,sizeof(IP));
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
    * @param *inIp ���շ�IP��ַ
    * @param inPort ���շ��˿�
    * @param inTimeout ��ʱʱ��
    * @param inRetry ���Դ���
    * @return true�򿪳ɹ�,false��ʧ��
    */
    virtual bool open(char *inIp ,int inPort ,int timeout ,int retry);

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