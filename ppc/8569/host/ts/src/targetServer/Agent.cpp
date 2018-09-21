/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  Agent.cpp
* @brief
*       功能：
*       <li>抽象层的代理对象</li>
*/

/************************头文件********************************/
#include "Agent.h"
#include "common.h"
#include "serverAgent.h"
#include "serverAgentwithDll.h"
#include "router.h"
#include "protocol.h"
#include "config.h"

/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/

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

//回收包到包缓冲区中
AgentWithQueue::~AgentWithQueue()
{
    TMutexLocker lock(&mutex);

    for (int i = 0; i < packetQueue.size(); ++i)
    {
        PacketManager::getInstance()->free(packetQueue[i]);
    }

    packetQueue.clear();

    /*删除日志文件*/
    if(pPacketLogger != NULL)
    {
        pPacketLogger->deleteLogFile();
        delete pPacketLogger;
        pPacketLogger = NULL;
    }
}

/**
* 功能：从缓冲队列中取出一个缓冲包
* @return 取出成功返回Packet对象，失败返回null
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
 * 功能：将缓冲队列中的数据包清空
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
* 功能：关闭sa对应资源
*/
void AgentWithQueue::close()
{

}

/**
* 功能：发送数据
*/
int AgentWithQueue::sendPacket(Packet* packet)
{
    packet;
    return 1;
}

/**
* 功能：发送数据
* @return 接收包的大小
*/
int AgentWithQueue::receivePacket(Packet* packet)
{
    packet;
    return 1;
}

/**
* 功能：向router注册, 运行server接收和发送线程
* @return true表示成功
*/
bool AgentWithQueue::run()
{
    return true;
}

/**
* 功能：设置状态
* @param state 1是处于Active 0是passive
*/
void AgentWithQueue::setState(int state)
{
    this->state = state;
}

/**
* 功能：得到状态
* @return int 状态, 1是处于Active 0是passive
*/
int AgentWithQueue::getState()
{
    return state;
}

/**
* 功能：设置是否记录日志
* @param isLog true表示日志, flase表示不记录
*/
void AgentWithQueue::setLogFlag(bool isLog)
{
    this->isLog = isLog;

    /*如果是manager,则只设置日志标志,
    其日志是由controlsubagent来记录的*/
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
            * 这里在为pPacketLogger创建实例的过程中，如果创建失败，应该设置this->isLog的标志为false，
            * 因为PackLog一旦创建失败就不能再继续记录日志文件。PackLog创建失败后，后面继续用this->isLog的值设置记录日志文件，会出现未知错误；
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
* 功能：得到日志标志
* @param isLog true表示日志, flase表示不记录
*/
bool AgentWithQueue::getLogFlag()
{
    return isLog;
}


/**
* 功能：将配置信息序列化到OwnerArchive对象
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

    //流化配置信息
    if (pConfig != NULL)
    {
        pConfig->serialize(ar);
    }
}

/**
* 功能：创建相应的配置信息
* @param mode 配置信息类型
* @param ServerConfig* 创建的配置信息
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

    case DeviceConfigBase::DLL_DEVICE:        //动态库方式
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
* 功能：设置配置信息
* @param strChannel 通道配置信息
*/
void AgentWithQueue::addChannel(QString& strChannel)
{
    sysLoger(LINFO, "AgentWithQueue::addChannel: Channel config info: %s .",
             strChannel.toAscii());
    vectorChannel.append(strChannel);
}

/**
* 功能：打开设备和初始化协议
* @return true表示成功
*/
bool AgentWithQueue::initDevice()
{
    return true;
}

/*Modified by tangxp for BUG NO.3452 on 2008.6.23 [begin]*/
/*  Modified brief: 增加获取新配置函数*/
/**
* 功能：获取新配置
* @param pConfig 新配置
* @param channelType 新通道类型
* @param aid 获取配置的SA的AID
* @return true表示成功
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

    int next = 0;    //查询的开始位置

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
    int protocol = vec[iCount++].toInt(&ok, 16);            //通信方式
    int way = vec[iCount++].toInt(&ok, 16);            //通信方式

    switch(way)
    {
    case DeviceConfigBase::TCP_DEVICE:    //tcp
    {
        TcpConfig *tcpConfig = new TcpConfig();                        //tcp的配置信息

        if(NULL == tcpConfig)
        {
            return false;
        }

        tcpConfig->setIP(vec[iCount++]);                //设置IP地址
        int port = vec[iCount++].toInt(&ok, 16);        //端口
        int timeout = vec[iCount++].toInt(&ok, 16);                //超时
        int retry = vec[iCount++].toInt(&ok, 16);                //重试
        tcpConfig->setPort(port);                    //设置端口

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

        udpConfig->setIP(vec[iCount++]);                //设置IP地址
        int port = vec[iCount++].toInt(&ok, 16);        //端口
        int timeout = vec[iCount++].toInt(&ok, 16);    //超时
        int retry = vec[iCount++].toInt(&ok, 16);    //重试
        udpConfig->setPort(port);                    //设置UDP端口
        udpConfig->setTimeout(timeout);                //设置超时
        udpConfig->setRetry(retry);                    //设置重试次数
        pConfig->setProtocolType(ProtocolBase::PACKET_PROTOCOL);
        pConfig->setDeviceConfig(udpConfig);
    }
    break;

    case DeviceConfigBase::UART_DEVICE:                        //串口
    {
        UartConfig *uartConfig = new UartConfig();                        //tcp的配置信息

        if(NULL == uartConfig)
        {
            return false;
        }

        uartConfig->setCom(vec[iCount++]);            //设置IP地址
        int baud = vec[iCount++].toInt(&ok, 16);        //端口
        int timeout = vec[iCount++].toInt(&ok, 16);    //超时
        int retry = vec[iCount++].toInt(&ok, 16);    //重试
        uartConfig->setBaud(baud);                    //设置串口波特率
        uartConfig->setTimeout(timeout);                //设置超时
        uartConfig->setRetry(retry);                    //设置重试次数
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
* 功能：置反线程运行标志，调用此方法可停止线程
*/
void TXThread::stop()
{
    runFlag = false;
    pServerAgent->wakeUp();
}

/**
* 功能：线程运行函数, 用来发送数据到server上
*/
void TXThread::run()
{
	int ret = 0;
    while (runFlag)
    {
        pServerAgent->waitUp();

        while(runFlag)
        {
            //从sa的缓冲区中读取一个包
            if (pServerAgent->getState() == SASTATE_INACTIVE)
            {
                //sa的状态是退出, 则退出线程
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
                    //出现消息循环，需要释放错误包
                    sysLoger(LWARNING, "Agent [%s] received pack from itself, free the Error pack!",
                             pServerAgent->getName().toAscii().data());
                    //释放分配的包
                    PacketManager::getInstance()->free(pack);
                }

                else
                {
                    /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [begin]*/
                    /*  Modified brief: 将消息记录集成到SA/SA_DLL发送数据操作中*/
                    //if ((pServerAgent->getLogFlag()) && (pServerAgent->getPacketLogger() != NULL))
                    //    pServerAgent->getPacketLogger()->write(SEND_PACK, *pack);
                    /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [end]*/
                    ret = pServerAgent->sendPacket(pack);        //把包发到对应的server上去
					if((ret == -2) && (pServerAgent->getType() == AgentBase::SERVER_AGENT))
					{
						//当sendPacket将数据包发送给目标机超时时,将当前消息队列中的数据全部清空,避免目标机回复数据,导致通信乱序
						sysLoger(LWARNING, "Agent [%s] send pack timeout, free the pack list!",
                             pServerAgent->getName().toAscii().data());
						pServerAgent->clearPktQueue();
					}
					PacketManager::getInstance()->free(pack);    //释放包
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
* 功能：置反线程运行标志，调用此方法可停止线程
*/
void RXThread::stop()
{
    runFlag = false;

    //表明线程要退出, 强制关闭线程对应的设备
    if (pServerAgent != NULL)
    {
        pServerAgent->close();
    }
}

/**
* 功能：线程运行函数
*/
void RXThread::run()
{
    int ret = 0;
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.18 [begin]*/
    /*  Modified brief: 将缓冲包指针定义移至循环外定义*/
    Packet* pack = NULL;

    while((runFlag) && (pServerAgent != NULL))
    {
        //调用服务接收函数
        pack = PacketManager::getInstance()->alloc();
        
        /*Modified by tangxp for BUG NO.0002555 on 2008.1.18 [end]*/
        ret = pServerAgent->receivePacket(pack) ;

        if (ret == -1 || !runFlag)
        {
            //连接失败

            //正在切换, 则等待一段时间
            if (pServerAgent->getState() != SASTATE_INACTIVE)
            {
                pServerAgent->setState(SASTATE_INACTIVE);            //设置sa的状态为非活动型

                //注销sa在router的注册
                Router::getInstance()->unRegisterAgent(pServerAgent->getAID());
            }

            if (runFlag)
            {
                stop();
            }

            //释放分配的包
            PacketManager::getInstance()->free(pack);
        }
        else if(ret > 0)
        {
            //设置源id号
            pack->setSrc_aid(pServerAgent->getAID());

            //如果出现了这种情况则会导致ts和目标机一直发送
            if (pack->getDes_aid() == pServerAgent->getAID())
            {
                sysLoger(LWARNING, "RXThread::run:Agent [%s] Rxthread received pack from itself! Need to free pack!",
                         pServerAgent->getName().toAscii().data());
                PacketManager::getInstance()->free(pack);
            }
            else
            {
                /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [end]*/
                //调用Router的函数进行数据包的转发
                Router::getInstance()->putpkt(pack);
            }
        }
        else
        {
            sysLoger(LWARNING, "RXThread::run:Agent [%s] Rxthread received invalid size pack, size:%d! Need to free pack!",
                         pServerAgent->getName().toAscii().data(), ret);

            //收到大小无效或空包，释放分配的包
            PacketManager::getInstance()->free(pack);
        }
    }
}


