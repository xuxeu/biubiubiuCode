/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  Agent.cpp
* @brief
*       ���ܣ�
*       <li>�����Ĵ������</li>
*/

/************************ͷ�ļ�********************************/
#include "Agent.h"
#include "common.h"
#include "serverAgent.h"
#include "serverAgentwithDll.h"
#include "router.h"
#include "protocol.h"
#include "config.h"

/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/

AgentBase::~AgentBase()
{}
///////////////////////////////////////AgentWithQueue
AgentWithQueue::AgentWithQueue(unsigned short aid) : AgentBase(aid)
{
    pPacketLogger = NULL;
    pConfig = NULL;
    timeout = 2000;
    channelType = 0;

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

    mutex.lock();

    if (packetQueue.size() > 0)
    {
        pack = packetQueue.first ();
        packetQueue.pop_front();
        sysLoger(LDEBUG, "AgentwithQueue::getpkt: Agent [%s] get a packet! FILE:%s, LINE:%d",
                 getName().toAscii().data(), __MYFILE__, __LINE__);
    }

    mutex.unlock();

    return pack;
}
 /**
 * ���ܣ�����������е����ݰ����
 */
void AgentWithQueue::clearPktQueue()
{
	 Packet* pack = NULL;

     mutex.lock();

	 for (int i = 0; i < packetQueue.size(); ++i)
	{
		pack = packetQueue.first ();
        packetQueue.pop_front();
		PacketManager::getInstance()->free(pack);
	}

    mutex.unlock();
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
* ���ܣ������Ƿ��¼��־
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
            pPacketLogger->open(QString(name + ".log").toAscii().data());
        }

        else
        {
            /*
            * ������ΪpPacketLogger����ʵ���Ĺ����У��������ʧ�ܣ�Ӧ������this->isLog�ı�־Ϊfalse��
            * ��ΪPackLogһ������ʧ�ܾͲ����ټ�����¼��־�ļ���PackLog����ʧ�ܺ󣬺��������this->isLog��ֵ���ü�¼��־�ļ��������δ֪����
            */
            this->isLog = false;
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

    //����������Ϣ
    if (pConfig != NULL)
    {
        pConfig->serialize(ar);
    }
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
* ���ܣ����豸�ͳ�ʼ��Э��
* @return true��ʾ�ɹ�
*/
bool AgentWithQueue::initDevice()
{
    return true;
}

/*Modified by tangxp for BUG NO.3452 on 2008.6.23 [begin]*/
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
        {
            break;
        }

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
/*Modified by tangxp for BUG NO.3452 on 2008.6.23 [end]*/

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
    pServerAgent->wakeUp();
}

/**
* ���ܣ��߳����к���, �����������ݵ�server��
*/
void TXThread::run()
{
	int ret = 0;
    while (runFlag)
    {
        pServerAgent->waitUp();

        while(runFlag)
        {
            //��sa�Ļ������ж�ȡһ����
            if (pServerAgent->getState() == SASTATE_INACTIVE)
            {
                //sa��״̬���˳�, ���˳��߳�
                if (runFlag)
                {
                    stop();
                }

                break;
            }

            else
            {
                Packet* pack = pServerAgent->getpkt();

                if (pack == NULL || !runFlag)
                {
                    break;
                }

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
                    ret = pServerAgent->sendPacket(pack);        //�Ѱ�������Ӧ��server��ȥ
					if((ret == -2) && (pServerAgent->getType() == AgentBase::SERVER_AGENT))
					{
						//��sendPacket�����ݰ����͸�Ŀ�����ʱʱ,����ǰ��Ϣ�����е�����ȫ�����,����Ŀ����ظ�����,����ͨ������
						sysLoger(LWARNING, "Agent [%s] send pack timeout, free the pack list!",
                             pServerAgent->getName().toAscii().data());
						pServerAgent->clearPktQueue();
					}
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
    {
        pServerAgent->close();
    }
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
        {
            //����ʧ��

            //�����л�, ��ȴ�һ��ʱ��
            if (pServerAgent->getState() != SASTATE_INACTIVE)
            {
                pServerAgent->setState(SASTATE_INACTIVE);            //����sa��״̬Ϊ�ǻ��

                //ע��sa��router��ע��
                Router::getInstance()->unRegisterAgent(pServerAgent->getAID());
            }

            if (runFlag)
            {
                stop();
            }

            //�ͷŷ���İ�
            PacketManager::getInstance()->free(pack);
        }
        else if(ret > 0)
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
                /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [end]*/
                //����Router�ĺ����������ݰ���ת��
                Router::getInstance()->putpkt(pack);
            }
        }
        else
        {
            sysLoger(LWARNING, "RXThread::run:Agent [%s] Rxthread received invalid size pack, size:%d! Need to free pack!",
                         pServerAgent->getName().toAscii().data(), ret);

            //�յ���С��Ч��հ����ͷŷ���İ�
            PacketManager::getInstance()->free(pack);
        }
    }
}


