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
* @file     ServerAgent.cpp
* @brief     server�����ʵ��
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/
#include "ServerAgent.h"
#include "Config.h"
#include "Device.h"
#include "Protocol.h"
#include "common.h"
#include "router.h"
//#include "Log.h"
#include "tcpDevice.h"
////////////////////////////////////////ServerAgent
ServerAgent::ServerAgent(unsigned short aid) : AgentWithQueue(aid) 
{
    state = SASTATE_INACTIVE;
    pConfig = NULL;
    pThreadRX = NULL;
    pThreadTX = NULL;
    pProtocol = NULL;
    //pPacketLogger = NULL;
    type = AgentBase::SERVER_AGENT;
    timeout = 2000;
}

ServerAgent::~ServerAgent()
{
    if (pConfig != NULL ) 
    {
        delete pConfig;    
        pConfig = NULL;
    }
    //�ͷŹ������豸
    if (pProtocol != NULL) 
    {
        if (pProtocol->getDevice()!= NULL)
            delete  pProtocol->getDevice();
        delete pProtocol; 
        pProtocol = NULL;
    }
    #if 0
    //�ر���־����
    if (pPacketLogger != NULL) 
    {
        pPacketLogger->deleteLogFile();
        delete pPacketLogger;
        pPacketLogger = NULL;
    }
    #endif

} 

/**
* ���ܣ���routerע��,����server���պͷ����߳�
* @return true��ʾ�ɹ�
*/
bool ServerAgent::run() 
{
    if (pProtocol != NULL)
    {
        if (pProtocol->getDevice()!= NULL) 
        {
            pProtocol->getDevice()->close();
            delete  pProtocol->getDevice();
        }
        delete pProtocol; 
    }
    //�����豸
    if (!initDevice())
    {
        sysLoger(LWARNING, "ServerAgent::run: Initalize device for Agent %s failure!",
                                        getName().toAscii().data());
        return false;
    }
    
    Router::getInstance()->registerAgent(this->getAID(),this);    //ע�ᵽrouter
    /*Modified by tangxp for BUG NO.0002803 on 2008.2.29 [begin]*/
    /*  Modified brief: ����SAʱ�ر���־*/
    if (this->getLogFlag()) 
    {
        #if 0
        pPacketLogger = new PackLog();
        #endif
        pPacketLogger->open((this->getName() + QString(".log")).toAscii().data());
    }
    /*Modified by tangxp for BUG NO.0002803 on 2008.2.29 [end]*/
    if (pThreadRX == NULL) 
    {
        pThreadRX = new RXThread(this);
        /*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [begin]*/
        /*  Modifed brief:�õ���д���߳������ȼ��滻QT�߳������ȼ�*/
        pThreadRX->start(baseThread::HighPriority);
        /*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [end]*/
    }
    if (pThreadTX == NULL) 
    {
        pThreadTX = new TXThread(this);
        pThreadTX->start(); 
    }
    return true;
}

/**
* ���ܣ�ֹͣserver�߳�
*/
void ServerAgent::stop() 
{
    this->setState(SASTATE_INACTIVE);
    if (pThreadTX != NULL) 
    {
        pThreadTX->stop();
        pThreadTX->wait();
        delete  pThreadTX;
        pThreadTX = NULL;
    }
    if (pThreadRX != NULL) 
    {
        pThreadRX->stop();
        
        pThreadRX->wait();    //�ȴ��߳��˳�
        delete  pThreadRX;
        pThreadRX = NULL;
    }
    close();

    /*Modified by tangxp for BUG NO.0002803 on 2008.2.29 [begin]*/
    /*  Modified brief: ֹͣSAʱ�ر���־*/
    //�ر���־�ļ�
    if(pPacketLogger != NULL)
    {        
        pPacketLogger->close();
        #if 0
        /*Modified by tangxp for BUG No.0002432 on 2008.1.8 [begin]*/
        /*  Modified breif: ����ֹͣserver�߳�ʱɾ����־����,
        ��������ʱ�½���־��������ڴ�й©*/
        delete pPacketLogger;
        pPacketLogger = NULL;
        /*Modified by tangxp for BUG No.0002432 on 2008.1.8 [end]*/
        #endif
    }
    /*Modified by tangxp for BUG NO.0002803 on 2008.2.29 [end]*/

}
/**
* ���ܣ��ر�sa��Ӧ��Դ
*/
void ServerAgent::close() 
{
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.26 [begin]*/
    /*  Modified brief: ���ӷ��ͻ���*/
    TMutexLocker locker(&sendMutex);
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.26 [end]*/
    //�ر��̶߳�Ӧ���豸
    if (pProtocol != NULL) 
    {
        DeviceBase* pDevice = pProtocol->getDevice();
        //�ͷ�Э���ڷ���ʱ,�п��ܶ������ź���
        pProtocol->close();
        //�ر�Э���Ӧ���豸,���ͷ���Դ
        if (pDevice != NULL) 
        {
            pDevice->close();
            delete pDevice;
            pDevice = NULL;
        }
        //�ͷ�Э���Ӧ����Դ
        delete pProtocol;
        pProtocol = NULL;
    }
}
/**
* ���ܣ���������
*/
int ServerAgent::sendPacket(Packet* packet) 
{
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.26 [begin]*/
    /*  Modified brief: ���ӷ��ͻ���*/
    TMutexLocker sendLocker(&sendMutex);
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.26 [end]*/
    if (pProtocol != NULL)
    {
        if(NULL == packet)
        {
            sysLoger(LWARNING, "ServerAgent::sendPacket: Bad pointer of packet!");
            return 0;
        }
        /*Modified by tangxp for BUG NO.0002550 on 2008.3.17 [begin]*/
        /*  Modified brief: ����¼��Ϣ��־���ɵ�����������
        ������¼λ�õ������Ժ�,�ɼ�¼�������к�*/
        int size = pProtocol->putpkt(packet);
        
        //��¼��Ϣ��־
        if(getLogFlag() && getPacketLogger())
        {
            getPacketLogger()->write(SEND_PACK, *packet);
        }

        return size;
        /*Modified by tangxp for BUG NO.0002550 on 2008.3.17[end]*/
    }
    else 
    {
        sysLoger(LWARNING, "ServerAgent::sendPacket: No Protocol for using, SA name:%s",
                                        getName().toAscii().data());
        return 0;
    }
}
/**
* ���ܣ���������
* @return ���հ��Ĵ�С
*/
int ServerAgent::receivePacket(Packet* packet) 
{
    if (pProtocol != NULL)
    {
        if(NULL == packet)
        {
            sysLoger(LWARNING, "ServerAgent::receivePacket: Bad pointer of packet!");
            return -1;
        }
        int recvSize = pProtocol->getpkt(packet);
        /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [begin]*/
        /*  Modified brief: ����¼��Ϣ��־���ɵ�����������*/
        //��¼��Ϣ��־
        if(getLogFlag() && getPacketLogger() && recvSize > 0)
        {
            getPacketLogger()->write(RECV_PACK, *packet);
        }
        /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [end]*/
        return recvSize;
    }
    else 
    {
        sysLoger(LWARNING, "ServerAgent::receivePacket: No Protocol for using, SA name:%s",
                                        getName().toAscii().data());
        return -1;
    }
}

/**
* ���ܣ���������Ϣ���л���OwnerArchive����
*/
void ServerAgent::serialize(OwnerArchive& ar) 
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    AgentWithQueue::serialize(ar);
}

/**
* ���ܣ����豸�ͳ�ʼ��Э��
* @return true��ʾ�ɹ�
*/
bool ServerAgent::initDevice() 
{
    //�����豸
    DeviceBase* pDevice = NULL;
    if((pConfig != NULL) && (pConfig->getDeviceConfig() != NULL)\
        && (DeviceManage::getInstance() != NULL))
    {
        pDevice = DeviceManage::getInstance()->createDevice(
                pConfig->getDeviceConfig()->getDeviceType());
    }
    else
    {
        sysLoger(LWARNING, "ServerAgent::initDevice: No usable device config or server config for SA:%s",
                                            name.toAscii().data());
        return false;
    }
    if (pDevice == NULL)
    {
        sysLoger(LWARNING, "ServerAgent::initDevice: Created device for Agent %s failure!",
                                        getName().toAscii().data());
        return false;
    }

    if (!pDevice->open(pConfig)) 
    {    //���豸
        delete  pDevice;
        pDevice = NULL;
        sysLoger(LWARNING, "ServerAgent::initDevice: Open device for Agent %s failure!",
                                        getName().toAscii().data());
        return false;
    }
    //����sa������Э��
    pProtocol = ProtocolManage::getInstance()->createProtocol(pConfig->getProtocolType(), pDevice);
    if(NULL == pProtocol)
    {
        sysLoger(LWARNING, "ServerAgent::initDevice: Created protocol for Agent %s failure!",
                                        getName().toAscii().data());
        return false;
    }
    return true;
}
