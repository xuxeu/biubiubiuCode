/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  tcpDevice.h
 * @Version        :  1.0.0
 * @Brief           :  ��װTCPͨ�ŷ�ʽ,�����ṩ���Ķ�д�ӿ�
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007��4��9�� 15:36:20
 * @History        : 
 *   
 */

#ifndef _TCP_DEVICE_H_
#define _TCP_DEVICE_H_

#include "wsocket.h"
#include "log.h"
#include "packet.h"

class tcpDevice
{
    private:
        WSocket        m_socket;
        QMutex          m_mtxSocket;
        int                  m_port;
        QString          m_IP;
    public:

        tcpDevice()
        {
            m_IP.clear();
            m_port = 0;
        }
        tcpDevice(QString IP, int port);
        ~tcpDevice();
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
        bool open(QString IP, int port);
        /**
        * �ر��豸.
        * @return true�رճɹ�,false�ر�ʧ��
        */
        bool close();

        /**
        * ���豸�ж�ȡ����.,��û�ж�ȡsize����һֱ�ȴ�
        * @param pBuff ��ȡ��������Ļ�����
        * @param size Ҫ��ȡ���ݵĴ�С
        * @param timeout �ȴ�ʱ��
        * @return int��ȡ������
        */
        int  read(Packet *pack) ;

        /**
        * ���豸д����,��û��д��size����һֱ�ȴ�
        * @param pBuff д���ݵĻ�����
        * @param size д���ݵĴ�С
        */
        int write(Packet *pack) ;
            
};

#endif/*_TCP_DEVICE_H_*/
