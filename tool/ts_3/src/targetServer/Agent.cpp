/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit,  including
*    copying it,  modifying it,  compiling it,  and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice,  this paragraph,  and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* 
*
* @file     AgentBase.cpp
* @brief     �����Ĵ������
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/


#include "Agent.h"
#include "common.h"
#include "serverAgent.h"
#include "serverAgentwithDll.h"
#include "router.h"
#include "protocol.h" 
#include "config.h"
AgentBase::~AgentBase() 
{}
///////////////////////////////////////AgentWithQueue
AgentWithQueue::AgentWithQueue(unsigned short aid) : AgentBase(aid)
{
    pPacketLogger = NULL;
    pConfig = NULL;
    timeout = 2000;
    channelType = 0;
    changeChannelState = 0;
    regType = INVALID_REG_TYPE;
    pProtocol = NULL;
    state = SASTATE_INACTIVE;
}

//���հ������������� 
AgentWithQueue::~AgentWithQueue() 
{
    TMutexLocker lock(&mutex);
    for (int i = 0; i < packetQueue.size(); ++i) 
    {
        PacketManager::getInstance()->free(packetQueue[i]);
    }
    packetQueue.clear();
    /*ɾ����־�ļ�*/
    if(pPacketLogger != NULL)
    {
        pPacketLogger->deleteLogFile();
        delete pPacketLogger;
        pPacketLogger = NULL;
    }
}    

/**
* ���ܣ��ӻ��������ȡ��һ�������
* @return ȡ���ɹ�����Packet����ʧ�ܷ���null
*/
Packet* AgentWithQueue::getpkt()
{
    Packet* pack = NULL;
    //���л������в����а��ķ���
    if (changeChannelState != 1)
    {
        mutex.lock();  
        if (packetQueue.size() > 0) 
        {
            pack = packetQueue.first ();
            packetQueue.pop_front();
            sysLoger(LDEBUG, "AgentwithQueue::getpkt: Agent [%s] get a packet! FILE:%s, LINE:%d",
                                            getName().toAscii().data(), __MYFILE__, __LINE__);
        }
        mutex.unlock();
    }
    else
    {
        TMutexLocker lock(&mutexChannel);
        bool isChangeChannel = channelCondition.wait(&mutexChannel, timeout);
        if (!isChangeChannel) 
        {//�ȴ���ʱ
            if (changeChannelState==1) 
            {//�ظ���ԭ����ͨ��״̬
                changeChannelState = 0;
                channelType = proChannelType;    //��ԭ��ǰ��ͨ��
                //�л�ʧ�ܵ�ʱ��ظ�
                Packet* pPacket = PacketManager::getInstance()->alloc();
                pPacket->setDes_aid(ackAid);
                //���ͨ���л�ʧ�ܻظ���
                char *pBuf = pPacket->getData();
                memcpy(pBuf, ERR_CHANGE_CHANNEL_FAILURE, FAILURE_ACK_LEN);   
                pBuf += FAILURE_ACK_LEN;
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
                pBuf += CONTROL_FLAG_LEN;
                pPacket->setSize(pBuf - pPacket->getData());  
                //������Ϣ��־��¼
                if(getLogFlag())
                {
                    pPacketLogger->write(SEND_PACK, *pPacket);
                }
                Router::getInstance()->putpkt(pPacket);    //���Ͱ�
                //ϵͳ��־
                sysLoger(LWARNING, "AgentWithQueue::getpkt: Agent [%s] change channel failure!"\
                                                    "  FILE:%s, LINE:%d.",
                                                    getName().toAscii().data(), __MYFILE__, __LINE__);
            }
        }
        else      //ͨ���л��ɹ�, ȡ��Ϣ��
        {
            mutex.lock();  
            if (packetQueue.size() > 0) 
            {
                pack = packetQueue.first ();
                packetQueue.pop_front();
                sysLoger(LDEBUG, "AgentwithQueue::getpkt: Agent [%s] get a packet! FILE:%s, LINE:%d",
                                                getName().toAscii().data(), __MYFILE__, __LINE__);
            }
            mutex.unlock();
        }
    }
    return pack;
}

/**
* ���ܣ��ر�sa��Ӧ��Դ
*/
void AgentWithQueue::close() 
{
}
/**
* ���ܣ���������
*/
int AgentWithQueue::sendPacket(Packet* packet) 
{
    packet;
    return 1;
}
/**
* ���ܣ���������
* @return ���հ��Ĵ�С
*/
int AgentWithQueue::receivePacket(Packet* packet)
{
    packet;
    return 1;
}
/**
* ���ܣ���routerע��, ����server���պͷ����߳�
* @return true��ʾ�ɹ�
*/
bool AgentWithQueue::run() 
{
    return true;
}

/**
* ���ܣ�����״̬
* @param state 1�Ǵ���Active 0��passive
*/
void AgentWithQueue::setState(int state) 
{
        this->state = state;
}
/**
* ���ܣ��õ�״̬
* @return int ״̬, 1�Ǵ���Active 0��passive
*/
int AgentWithQueue::getState() 
{
    return state;
}
/**
* ���ܣ������Ƿ���־
* @param isLog true��ʾ��־, flase��ʾ����¼
*/
void AgentWithQueue::setLogFlag(bool isLog) 
{
    this->isLog = isLog;

    /*�����manager,��ֻ������־��־,
    ����־����controlsubagent����¼��*/
    if(getAID() == 0)
    {
        return;
    }
    if(isLog && (NULL == pPacketLogger))
    {
        pPacketLogger = new PackLog();
        if(pPacketLogger != NULL)
        {
            sysLoger(LDEBUG, "AgentWithQueue::setLogFlag: open log file for agent:%s, id:%d", 
                name.toAscii().data(), getAID());
            pPacketLogger->open(QString(name+".log").toAscii().data());
        }
    }
    else if(!isLog && (pPacketLogger != NULL))
    {
        pPacketLogger->close();
        delete pPacketLogger;
        pPacketLogger = NULL;
    }
    else
    {

    }
}
/**
* ���ܣ��õ���־��־
* @param isLog true��ʾ��־, flase��ʾ����¼
*/
bool AgentWithQueue::getLogFlag() 
{
    return isLog;
}


/**
* ���ܣ���������Ϣ���л���OwnerArchive����
*/
void AgentWithQueue::serialize(OwnerArchive& ar)
{

    if(ar.IsStoring()) 
    {
        (*ar.pDataStream) << name;
        (*ar.pDataStream) << state;
    } 
    else 
    {
        (*ar.pDataStream) >> name;    
        (*ar.pDataStream) >> state;        
    }

    //��������Ϣ
    if (pConfig != NULL)
        pConfig->serialize(ar);
}

/**
* ���ܣ�������Ӧ��������Ϣ
* @param mode ������Ϣ����
* @param ServerConfig* ������������Ϣ
*/
ServerConfig* AgentWithQueue::createConfig(int way) 
{
    if (pConfig != NULL) 
    {
        delete pConfig;
        pConfig = NULL;
    }
    DeviceConfigBase* pDeviceConfig = NULL;
    switch(way) 
    {
        case DeviceConfigBase::TCP_DEVICE:
            pDeviceConfig = new TcpConfig(way);
            break;
            
        case DeviceConfigBase::UDP_DEVICE:
            pDeviceConfig = new UdpConfig(way);
            break;
            
        case DeviceConfigBase::UART_DEVICE:
            pDeviceConfig = new UartConfig(way);
            break;
            
        case DeviceConfigBase::USB_DEVICE:        //USB��ʽ
            break;
            
        case DeviceConfigBase::DLL_DEVICE:        //��̬�ⷽʽ
            break;
            
        default:
            sysLoger(LINFO, "AgentWithQueue::createConfig: unknow way, create TCP config default!");
            pDeviceConfig = new TcpConfig(way);
            break; 
    }
    pConfig = new ServerConfig(pDeviceConfig, way, isLog);   
    return pConfig;
}  

/**
* ���ܣ�����������Ϣ
* @param strChannel ͨ��������Ϣ
*/
void AgentWithQueue::addChannel(QString& strChannel) 
{
    sysLoger(LINFO, "AgentWithQueue::addChannel: Channel config info: %s .", 
                                strChannel.toAscii());
    vectorChannel.append(strChannel);
}

/**
* ���ܣ��л�ͨ��
* @param channelType ͨ����
*/
bool AgentWithQueue::changeChannel() 
{
    if(!getChannelSwitchState())
    {
        sysLoger(LINFO, "AgentWithQueue::changeChannel: The Channel Switch function is disable "\
                                  "by CA %d", getDisableChannelSwitchCAaid());
        return false;
    }
    //�ر�Э����豸
    if (channelType >= vectorChannel.size())
    {
        sysLoger(LWARNING, "AgentWithQueue::changeChannel: ChannelType expired!");
        return false;
    }
    close();
    //���豸���д���
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [begin]*/
    /*  Modified brief: ���ӻ�ȡ�����ú���*/
    ServerConfig * pConfig = this->getConfig();
    if(!getNewConfig(pConfig, channelType, getAID()))
    {
        changeChannelState = 0;     //û���л�
        return false;
    }
    
    #if 0
    {
        QString buf = vectorChannel[channelType];
        QString str;
        QVector<QString> vec;

        int next = 0;    //��ѯ�Ŀ�ʼλ��
        while(true) 
        {
            str = buf.section(CONTROL_FLAG, next, next);
            if (str.size() == 0)
                break;
            vec.append(str);
            next++;
        }
        
        int iCount = 0;
        bool ok = false;
        ServerConfig * pConfig = this->getConfig();
        int protocol = vec[iCount++].toInt(&ok, 16);            //ͨ�ŷ�ʽ
        int way = vec[iCount++].toInt(&ok, 16);            //ͨ�ŷ�ʽ
        switch(way) 
        {
            case DeviceConfigBase::TCP_DEVICE:    //tcp
                {
                    TcpConfig *tcpConfig = new TcpConfig();                        //tcp��������Ϣ
                    tcpConfig->setIP(vec[iCount++]);                //����IP��ַ
                    int port = vec[iCount++].toInt(&ok, 16);        //�˿�
                    int timeout = vec[iCount++].toInt(&ok, 16);                //��ʱ
                    int retry = vec[iCount++].toInt(&ok, 16);                //����    
                    tcpConfig->setPort(port);                    //���ö˿�

                    pConfig->setProtocolType(ProtocolBase::TCP_PROTOCOL);
                    pConfig->setDeviceConfig(tcpConfig);
                }
                break;
                
            case DeviceConfigBase::UDP_DEVICE:    //udp
                {
                    UdpConfig *udpConfig = new UdpConfig();    
                    udpConfig->setIP(vec[iCount++]);                //����IP��ַ
                    int port = vec[iCount++].toInt(&ok, 16);        //�˿�
                    int timeout = vec[iCount++].toInt(&ok, 16);    //��ʱ
                    int retry = vec[iCount++].toInt(&ok, 16);    //����    
                    udpConfig->setPort(port);                    //����UDP�˿�
                    udpConfig->setTimeout(timeout);                //���ó�ʱ
                    udpConfig->setRetry(retry);                    //�������Դ���
                    pConfig->setProtocolType(ProtocolBase::PACKET_PROTOCOL);
                    pConfig->setDeviceConfig(udpConfig);
                }
                break;
                
            case DeviceConfigBase::UART_DEVICE:                        //����
                {
                    UartConfig *uartConfig = new UartConfig();                        //tcp��������Ϣ
                    uartConfig->setCom(vec[iCount++]);            //����IP��ַ
                    int baud = vec[iCount++].toInt(&ok, 16);        //�˿�
                    int timeout = vec[iCount++].toInt(&ok, 16);    //��ʱ
                    int retry = vec[iCount++].toInt(&ok, 16);    //����    
                    uartConfig->setBaud(baud);                    //���ô��ڲ�����
                    uartConfig->setTimeout(timeout);                //���ó�ʱ
                    uartConfig->setRetry(retry);                    //�������Դ���
                    pConfig->setProtocolType(ProtocolBase::STREAM_PROTOCOL);
                    pConfig->setDeviceConfig(uartConfig);
                }
                break;
                
            default:
                /*Modified by tangxp for BUG NO.0002555 on 2008.1.18 [begin]*/
                /*  Modified brief: ��û���л�ͨ��,�����л�״̬Ϊδ�л�*/
                changeChannelState = 0;     //û���л�
                /*Modified by tangxp for BUG NO.0002555 on 2008.1.18 [end]*/
                return true;
                break;
        }    

    }
    #endif
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [end]*/
    
    Packet* pack = PacketManager::getInstance()->alloc();
    //�����豸
    if (!initDevice()) 
    {
        //����ʧ����Ϣ
        pack->clearData();
        pack->setDes_aid(ackAid);
        char *pBuf = pack->getData();
        US  did = Manager::getInstance()->getCurDID(ackAid);
        //�洢DID
        __store(did, pBuf, DIDLEN);
        pBuf += DIDLEN;
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;        
        memcpy(pBuf, ERR_DEVICE_CANNOT_USE, FAILURE_ACK_LEN);
        pBuf += FAILURE_ACK_LEN;
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;        
        pack->setSize(pBuf - pack->getData());                //���°��Ĵ�С    
        if(getLogFlag())
        {
            pPacketLogger->write(SEND_PACK, *pack);
        }
        Router::getInstance()->putpkt(pack);    //���Ͱ�
        changeChannelState = 0;
        sysLoger(LWARNING, "AgentWithQueue:changeChannel: Agent [%s] ChangeChannel failure, "\
                                        "because of initializing the device failure!", getName().toAscii().data());
        return false;
    }
    //����ͨ���л��ɹ��ظ�
    pack->clearData();
    pack->setDes_aid(ackAid);
    char *pBuf = pack->getData();
    US  did = Manager::getInstance()->getCurDID(ackAid);
    //�洢DID
    __store(did, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;  
    memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);    //���°�������
    pBuf += SUCCESS_ACK_LEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;  
    pack->setSize(pBuf - pack->getData());
    //������Ϣ��־
    if(getLogFlag())
    {
        pPacketLogger->write(SEND_PACK, *pack);
    }
    Router::getInstance()->putpkt(pack);    //���Ͱ�
    changeChannelState = 0;
    //����ϵͳ��־
    sysLoger(LINFO, "AgentWithQueue::changeChannel: Agent [%s] change channel successfully!",
                                getName().toAscii().data());
    
    //�����ϱ��������еǼ��˸ô���ע�����͵�CA�ϱ�
    Manager::getInstance()->sendSAReportToCA(this);    //�ϱ�����
    //�洢����
    if(this->getRegisterType() != INVALID_REG_TYPE)
    {
    Manager::getInstance()->storeConfigure();
    }
    {
        TMutexLocker locker(&mutexChannel);
        channelCondition.wakeAll();
    }
    return true;
}

/**
* ���ܣ����豸�ͳ�ʼ��Э��
* @return true��ʾ�ɹ�
*/
bool AgentWithQueue::initDevice() 
{
    return true;
}

/*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [begin]*/
    /*  Modified brief: ���ӻ�ȡ�����ú���*/
    /**
    * ���ܣ���ȡ������
    * @param pConfig ������
    * @param channelType ��ͨ������
    * @param aid ��ȡ���õ�SA��AID
    * @return true��ʾ�ɹ�
    */
int AgentWithQueue::getNewConfig(ServerConfig * pConfig, int channelType, int aid)
{
    if(NULL == pConfig)
    {
        return false;
    }

    AgentBase* pAgent = Manager::getInstance()->getServerAgentFromID(aid);
    if(NULL == pAgent)
    {
        sysLoger(LWARNING, "AgentWithQueue::getNewConfig: get agent failed!");
        return false;
    }
    AgentWithQueue *pAgentQ = dynamic_cast<AgentWithQueue*>(pAgent);
    if(NULL == pAgentQ)
    {
        sysLoger(LWARNING, "AgentWithQueue::getNewConfig: Dynamic cast pointer "\
                                            " of Agent class to AgentWithQueue Class failure!");
        return false;
    }
    if(pAgentQ->getType() != AgentBase::SERVER_AGENT)
    {
        sysLoger(LINFO, "AgentWithQueue::getNewConfig: Invalid serverAgent type %d,"\
                                    " the aid %d", pAgentQ->getType(), aid);
        return false;
    }

    QVector<QString> vectorChannel = pAgentQ->getChannelInfo();
    
    if (channelType >= vectorChannel.size())
    {
        sysLoger(LWARNING, "AgentWithQueue::getNewConfig: ChannelType expired!");
        return false;
    }
    
    QString buf = vectorChannel[channelType];
    QString str;
    QVector<QString> vec;

    int next = 0;    //��ѯ�Ŀ�ʼλ��
    while(true) 
    {
        str = buf.section(CONTROL_FLAG, next, next);
        if (str.size() == 0)
            break;
        vec.append(str);
        next++;
    }
    
    int iCount = 0;
    bool ok = false;
    int protocol = vec[iCount++].toInt(&ok, 16);            //ͨ�ŷ�ʽ
    int way = vec[iCount++].toInt(&ok, 16);            //ͨ�ŷ�ʽ
    switch(way) 
    {
        case DeviceConfigBase::TCP_DEVICE:    //tcp
            {
                TcpConfig *tcpConfig = new TcpConfig();                        //tcp��������Ϣ
                if(NULL == tcpConfig)
                {
                    return false;
                }
                tcpConfig->setIP(vec[iCount++]);                //����IP��ַ
                int port = vec[iCount++].toInt(&ok, 16);        //�˿�
                int timeout = vec[iCount++].toInt(&ok, 16);                //��ʱ
                int retry = vec[iCount++].toInt(&ok, 16);                //����    
                tcpConfig->setPort(port);                    //���ö˿�

                pConfig->setProtocolType(ProtocolBase::TCP_PROTOCOL);
                pConfig->setDeviceConfig(tcpConfig);
            }
            break;
            
        case DeviceConfigBase::UDP_DEVICE:    //udp
            {
                UdpConfig *udpConfig = new UdpConfig(); 
                if(NULL == udpConfig)
                {
                    return false;
                }
                udpConfig->setIP(vec[iCount++]);                //����IP��ַ
                int port = vec[iCount++].toInt(&ok, 16);        //�˿�
                int timeout = vec[iCount++].toInt(&ok, 16);    //��ʱ
                int retry = vec[iCount++].toInt(&ok, 16);    //����    
                udpConfig->setPort(port);                    //����UDP�˿�
                udpConfig->setTimeout(timeout);                //���ó�ʱ
                udpConfig->setRetry(retry);                    //�������Դ���
                pConfig->setProtocolType(ProtocolBase::PACKET_PROTOCOL);
                pConfig->setDeviceConfig(udpConfig);
            }
            break;
            
        case DeviceConfigBase::UART_DEVICE:                        //����
            {
                UartConfig *uartConfig = new UartConfig();                        //tcp��������Ϣ
                if(NULL == uartConfig)
                {
                    return false;
                }
                uartConfig->setCom(vec[iCount++]);            //����IP��ַ
                int baud = vec[iCount++].toInt(&ok, 16);        //�˿�
                int timeout = vec[iCount++].toInt(&ok, 16);    //��ʱ
                int retry = vec[iCount++].toInt(&ok, 16);    //����    
                uartConfig->setBaud(baud);                    //���ô��ڲ�����
                uartConfig->setTimeout(timeout);                //���ó�ʱ
                uartConfig->setRetry(retry);                    //�������Դ���
                pConfig->setProtocolType(ProtocolBase::STREAM_PROTOCOL);
                pConfig->setDeviceConfig(uartConfig);
            }
            break;
            
        default:
            return false;
            break;
    }    

    return true;
}
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [end]*/

///////////////////////////TXThread
TXThread::TXThread(AgentWithQueue* pServerAgent)
{
    runFlag = true;
    this->pServerAgent = pServerAgent;
}

/**
* ���ܣ��÷��߳����б�־�����ô˷�����ֹͣ�߳�
*/
void TXThread::stop()
{
    runFlag = false;
#if 0
    //�����̲߳�Ҫȥ�رշ�����̼��豸
    pServerAgent->wakeUp();
    if (pServerAgent->getProtocol() != NULL) 
    {
        pServerAgent->getProtocol()->close();
    }
#endif
}

/**
* ���ܣ��߳����к���, �����������ݵ�server��
*/
void TXThread::run() 
{
    while (runFlag) 
    {
        pServerAgent->waitUp();

        while(runFlag) 
        {
            //��sa�Ļ������ж�ȡһ����
            if (pServerAgent->getState() == SASTATE_INACTIVE)   
            {//sa��״̬���˳�, ���˳��߳�
                if (runFlag)
                    stop();    
                break;
            }
            else 
            {
                Packet* pack = pServerAgent->getpkt();
                if (pack == NULL || !runFlag)
                    break;

                if (pack->getSrc_aid() == pServerAgent->getAID())
                {
                    //������Ϣѭ������Ҫ�ͷŴ����
                    sysLoger(LWARNING, "Agent [%s] received pack from itself, free the Error pack!",
                                                        pServerAgent->getName().toAscii().data());
                    //�ͷŷ���İ�
                    PacketManager::getInstance()->free(pack);
                }
                else 
                {
                    /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [begin]*/
                    /*  Modified brief: ����Ϣ��¼���ɵ�SA/SA_DLL�������ݲ�����*/
                    //if ((pServerAgent->getLogFlag()) && (pServerAgent->getPacketLogger() != NULL))
                    //    pServerAgent->getPacketLogger()->write(SEND_PACK, *pack);
                    /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [end]*/
                    pServerAgent->sendPacket(pack);        //�Ѱ�������Ӧ��server��ȥ
                    PacketManager::getInstance()->free(pack);    //�ͷŰ�
                }
            }
        }
    }
}

///////////////////////////RXThread
RXThread::RXThread(AgentWithQueue* pServerAgent)
{
    runFlag = true;
    this->pServerAgent = pServerAgent;
}

/**
* ���ܣ��÷��߳����б�־�����ô˷�����ֹͣ�߳�
*/
void RXThread::stop() 
{
    runFlag = false;
    //�����߳�Ҫ�˳�, ǿ�ƹر��̶߳�Ӧ���豸
    if (pServerAgent != NULL)
        pServerAgent->close();
}

/**
* ���ܣ��߳����к���
*/
void RXThread::run() 
{
    int ret = 0;
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.18 [begin]*/
    /*  Modified brief: �������ָ�붨������ѭ���ⶨ��*/
    Packet* pack = NULL;
    while((runFlag) && (pServerAgent != NULL)) 
    {
        //���÷�����պ���
        pack = PacketManager::getInstance()->alloc();
        /*Modified by tangxp for BUG NO.0002555 on 2008.1.18 [end]*/
        ret = pServerAgent->receivePacket(pack) ;        

        if (ret == -1 || !runFlag)
        {    //����ʧ��
            //����ͨ���л�
            if (pServerAgent->getChangeChannelState() == 1) 
            {
                pServerAgent->wakeUpChannel();
                pServerAgent->setChangeChannelState(0);
            }
            else 
            {
                //�����л�, ��ȴ�һ��ʱ��
                if (pServerAgent->getState() != SASTATE_INACTIVE) 
                {
                    pServerAgent->setState(SASTATE_INACTIVE);            //����sa��״̬Ϊ�ǻ��
                    //ע��sa��router��ע��
                    Router::getInstance()->unRegisterAgent(pServerAgent->getAID());
                }
                if (runFlag)
                    stop();
            }
            //�ͷŷ���İ�
            PacketManager::getInstance()->free(pack);
        }
		else if(ret == -10054)
		{
			// �����յ�10054�Ĵ��󣬲��ر��豸ͨ�ţ�ֱ�Ӻ���
			 PacketManager::getInstance()->free(pack);
		}
        /*else if(ret == TS_TIME_OUT)
        {
            //���windows 2000�µ�USB���᷵�س�ʱ�����ֲ��ܹر��豸
            PacketManager::getInstance()->free(pack);
        }*/
        else 
        {
            //����Դid��
            pack->setSrc_aid(pServerAgent->getAID());
            //������������������ᵼ��ts��Ŀ���һֱ����
            if (pack->getDes_aid() == pServerAgent->getAID())
            {
                sysLoger(LWARNING, "RXThread::run:Agent [%s] Rxthread received pack from itself! Need to free pack!",
                                                    pServerAgent->getName().toAscii().data());
                PacketManager::getInstance()->free(pack);
            }
            else
            {
                //Ҫ��ͨ���л�����һЩ�ж�
                if (pServerAgent->getChangeChannelState() == 1 )    //�õ�ta��ȷ�ϰ�
                {
                    //�����豸���л�
                    if (pack->getDes_aid() == 0) 
                    {
                        pServerAgent->changeChannel();
                    }
                    //�ͷŷ���İ�
                    PacketManager::getInstance()->free(pack);
                }
                else 
                {
                    /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [begin]*/
                    /*  Modified brief: ����Ϣ��¼���ɵ�SA/SA_DLL�������ݲ�����*/
                    //if ((pServerAgent->getLogFlag()) && (pServerAgent->getPacketLogger() != NULL))
                    //{
                    //    pServerAgent->getPacketLogger()->write(RECV_PACK, *pack);
                    //}
                    /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [end]*/
                    //����Router�ĺ����������ݰ���ת��
                    Router::getInstance()->putpkt(pack);     
                }
            }
        }    
    }
}


