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
* @brief     抽象层的代理对象
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
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
    //在切换过程中不进行包的发送
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
        {//等待超时
            if (changeChannelState==1) 
            {//回复到原来的通道状态
                changeChannelState = 0;
                channelType = proChannelType;    //还原以前的通道
                //切换失败的时候回复
                Packet* pPacket = PacketManager::getInstance()->alloc();
                pPacket->setDes_aid(ackAid);
                //填充通道切换失败回复包
                char *pBuf = pPacket->getData();
                memcpy(pBuf, ERR_CHANGE_CHANNEL_FAILURE, FAILURE_ACK_LEN);   
                pBuf += FAILURE_ACK_LEN;
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
                pBuf += CONTROL_FLAG_LEN;
                pPacket->setSize(pBuf - pPacket->getData());  
                //进行消息日志记录
                if(getLogFlag())
                {
                    pPacketLogger->write(SEND_PACK, *pPacket);
                }
                Router::getInstance()->putpkt(pPacket);    //发送包
                //系统日志
                sysLoger(LWARNING, "AgentWithQueue::getpkt: Agent [%s] change channel failure!"\
                                                    "  FILE:%s, LINE:%d.",
                                                    getName().toAscii().data(), __MYFILE__, __LINE__);
            }
        }
        else      //通道切换成功, 取消息包
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
* 功能：设置是否日志
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

    //硫化配置信息
    if (pConfig != NULL)
        pConfig->serialize(ar);
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
            
        case DeviceConfigBase::USB_DEVICE:        //USB方式
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
* 功能：切换通道
* @param channelType 通道号
*/
bool AgentWithQueue::changeChannel() 
{
    if(!getChannelSwitchState())
    {
        sysLoger(LINFO, "AgentWithQueue::changeChannel: The Channel Switch function is disable "\
                                  "by CA %d", getDisableChannelSwitchCAaid());
        return false;
    }
    //关闭协议和设备
    if (channelType >= vectorChannel.size())
    {
        sysLoger(LWARNING, "AgentWithQueue::changeChannel: ChannelType expired!");
        return false;
    }
    close();
    //对设备进行创建
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [begin]*/
    /*  Modified brief: 增加获取新配置函数*/
    ServerConfig * pConfig = this->getConfig();
    if(!getNewConfig(pConfig, channelType, getAID()))
    {
        changeChannelState = 0;     //没有切换
        return false;
    }
    
    #if 0
    {
        QString buf = vectorChannel[channelType];
        QString str;
        QVector<QString> vec;

        int next = 0;    //查询的开始位置
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
        int protocol = vec[iCount++].toInt(&ok, 16);            //通信方式
        int way = vec[iCount++].toInt(&ok, 16);            //通信方式
        switch(way) 
        {
            case DeviceConfigBase::TCP_DEVICE:    //tcp
                {
                    TcpConfig *tcpConfig = new TcpConfig();                        //tcp的配置信息
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
                /*Modified by tangxp for BUG NO.0002555 on 2008.1.18 [begin]*/
                /*  Modified brief: 若没有切换通道,设置切换状态为未切换*/
                changeChannelState = 0;     //没有切换
                /*Modified by tangxp for BUG NO.0002555 on 2008.1.18 [end]*/
                return true;
                break;
        }    

    }
    #endif
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [end]*/
    
    Packet* pack = PacketManager::getInstance()->alloc();
    //创建设备
    if (!initDevice()) 
    {
        //发送失败信息
        pack->clearData();
        pack->setDes_aid(ackAid);
        char *pBuf = pack->getData();
        US  did = Manager::getInstance()->getCurDID(ackAid);
        //存储DID
        __store(did, pBuf, DIDLEN);
        pBuf += DIDLEN;
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;        
        memcpy(pBuf, ERR_DEVICE_CANNOT_USE, FAILURE_ACK_LEN);
        pBuf += FAILURE_ACK_LEN;
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;        
        pack->setSize(pBuf - pack->getData());                //更新包的大小    
        if(getLogFlag())
        {
            pPacketLogger->write(SEND_PACK, *pack);
        }
        Router::getInstance()->putpkt(pack);    //发送包
        changeChannelState = 0;
        sysLoger(LWARNING, "AgentWithQueue:changeChannel: Agent [%s] ChangeChannel failure, "\
                                        "because of initializing the device failure!", getName().toAscii().data());
        return false;
    }
    //发送通道切换成功回复
    pack->clearData();
    pack->setDes_aid(ackAid);
    char *pBuf = pack->getData();
    US  did = Manager::getInstance()->getCurDID(ackAid);
    //存储DID
    __store(did, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;  
    memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);    //更新包的数据
    pBuf += SUCCESS_ACK_LEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;  
    pack->setSize(pBuf - pack->getData());
    //进行消息日志
    if(getLogFlag())
    {
        pPacketLogger->write(SEND_PACK, *pack);
    }
    Router::getInstance()->putpkt(pack);    //发送包
    changeChannelState = 0;
    //进行系统日志
    sysLoger(LINFO, "AgentWithQueue::changeChannel: Agent [%s] change channel successfully!",
                                getName().toAscii().data());
    
    //进行上报，向所有登记了该代理注册类型的CA上报
    Manager::getInstance()->sendSAReportToCA(this);    //上报类型
    //存储配置
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
* 功能：打开设备和初始化协议
* @return true表示成功
*/
bool AgentWithQueue::initDevice() 
{
    return true;
}

/*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [begin]*/
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
            break;
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
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [end]*/

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
#if 0
    //接收线程不要去关闭服务进程及设备
    pServerAgent->wakeUp();
    if (pServerAgent->getProtocol() != NULL) 
    {
        pServerAgent->getProtocol()->close();
    }
#endif
}

/**
* 功能：线程运行函数, 用来发送数据到server上
*/
void TXThread::run() 
{
    while (runFlag) 
    {
        pServerAgent->waitUp();

        while(runFlag) 
        {
            //从sa的缓冲区中读取一个包
            if (pServerAgent->getState() == SASTATE_INACTIVE)   
            {//sa的状态是退出, 则退出线程
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
                    pServerAgent->sendPacket(pack);        //把包发到对应的server上去
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
        pServerAgent->close();
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
        {    //连接失败
            //正在通道切换
            if (pServerAgent->getChangeChannelState() == 1) 
            {
                pServerAgent->wakeUpChannel();
                pServerAgent->setChangeChannelState(0);
            }
            else 
            {
                //正在切换, 则等待一段时间
                if (pServerAgent->getState() != SASTATE_INACTIVE) 
                {
                    pServerAgent->setState(SASTATE_INACTIVE);            //设置sa的状态为非活动型
                    //注销sa在router的注册
                    Router::getInstance()->unRegisterAgent(pServerAgent->getAID());
                }
                if (runFlag)
                    stop();
            }
            //释放分配的包
            PacketManager::getInstance()->free(pack);
        }
		else if(ret == -10054)
		{
			// 对于收到10054的错误，不关闭设备通信，直接忽略
			 PacketManager::getInstance()->free(pack);
		}
        /*else if(ret == TS_TIME_OUT)
        {
            //针对windows 2000下的USB读会返回超时，但又不能关闭设备
            PacketManager::getInstance()->free(pack);
        }*/
        else 
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
                //要对通道切换进行一些判断
                if (pServerAgent->getChangeChannelState() == 1 )    //得到ta的确认包
                {
                    //进行设备的切换
                    if (pack->getDes_aid() == 0) 
                    {
                        pServerAgent->changeChannel();
                    }
                    //释放分配的包
                    PacketManager::getInstance()->free(pack);
                }
                else 
                {
                    /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [begin]*/
                    /*  Modified brief: 将消息记录集成到SA/SA_DLL接收数据操作中*/
                    //if ((pServerAgent->getLogFlag()) && (pServerAgent->getPacketLogger() != NULL))
                    //{
                    //    pServerAgent->getPacketLogger()->write(RECV_PACK, *pack);
                    //}
                    /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [end]*/
                    //调用Router的函数进行数据包的转发
                    Router::getInstance()->putpkt(pack);     
                }
            }
        }    
    }
}


