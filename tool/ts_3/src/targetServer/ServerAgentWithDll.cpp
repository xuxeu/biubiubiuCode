#include "serveragentwithdll.h"
#include "common.h"
#include "router.h"
const char* STRGETPKT = "getpkt";
const char* STRPUTPKT = "putpkt";
const char* STROPEN = "open";
const char* STRCLOSE = "close";
const char* STRRELEASEDLL = "releaseDll";

ServerAgentWithDll::ServerAgentWithDll(unsigned short aid) : AgentWithQueue(aid) 
{
    m_hDll = NULL;
    get = NULL;
    put = NULL;
    open  = NULL;
    closeDll = NULL;
    releaseDll = NULL;
    type = AgentBase::SERVER_AGENT_WITH_DLL;
    state = SASTATE_INACTIVE;
    isLog = false;
    memset(outBuff,0,PACKET_DATASIZE);
    timeout = 1000;
    pThreadRX = NULL;
    pThreadTX = NULL;
    pProtocol = NULL;
    //pPacketLogger = NULL;
}

ServerAgentWithDll::~ServerAgentWithDll(void)
{
    //卸载动态库
    if (m_hDll != NULL)
    {
    //    releaseDll();        //释放库中监听socket
        FreeLibrary(m_hDll);
        m_hDll = NULL;
    }
    #if 0
    if(pPacketLogger != NULL)
    {
        pPacketLogger->deleteLogFile();
        delete pPacketLogger;
        pPacketLogger = NULL;
    }
    #endif
}


/**
* 功能：向router注册,运行server接收和发送线程
* @return true表示成功
*/
bool ServerAgentWithDll::run() 
{
    if (m_hDll != NULL) 
    {
        FreeLibrary(m_hDll);
    }
//    m_hDll = LoadLibrary(getDllName().utf16());    //装载制定的动态库
    
#ifdef _UNICODE
    m_hDll = LoadLibrary(getDllName().utf16());    //装载制定的动态库
#else
    m_hDll = LoadLibrary(getDllName().toAscii());    //装载制定的动态库
#endif

    if (m_hDll == NULL)        //装载库失败
        return false;
    get = (GETPKT)GetProcAddress(m_hDll,STRGETPKT);
    put = (PUTPKT)GetProcAddress(m_hDll,STRPUTPKT);
    open = (OPEN)GetProcAddress(m_hDll,STROPEN);
    closeDll = (CLOSE)GetProcAddress(m_hDll,STRCLOSE);
//    releaseDll = (RELEASEDLL)GetProcAddress(m_hDll,STRRELEASEDLL);

    if (get == NULL || put == NULL || open == NULL || closeDll == NULL )
        return false;
    sysLoger(LDEBUG, "ServerAgentWithDll:dll [%d] inBuf:%s", getAID(), getInBuf().toAscii().data());
    int ret = open(getAID(), getInBuf().toAscii().data(), outBuff);    //函数的调用
    if (ret == -1) 
    {
        return false;
    }

    Router::getInstance()->registerAgent(this->getAID(), this);    //注册到router
    /*Modified by tangxp for BUG NO.0002803 on 2008.2.29 [begin]*/
    /*  Modified brief: 运行SA时关闭日志*/
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
        /*  Modifed brief:用单独写的线程类优先级替换QT线程类优先级*/
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
* 功能：停止server线程
*/
void ServerAgentWithDll::stop()
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
    //    releaseDll();            //释放库中监听socket
        pThreadRX->wait();        //等待线程退出
        delete  pThreadRX;
        pThreadRX = NULL;
    }
    
    //卸载动态库
    if (closeDll != NULL)
    {
        closeDll(this->getAID());
        closeDll = NULL;
    }
    if (m_hDll != NULL) 
    {
        FreeLibrary(m_hDll);
        m_hDll = NULL;
    }
    /*Modified by tangxp for BUG NO.0002803 on 2008.2.29 [begin]*/
    /*  Modified brief: 运行SA时关闭日志*/    
    //关闭日志文件
    if (pPacketLogger != NULL)
    {
        pPacketLogger->close();
        #if 0
        /*Modified by tangxp for BUG No.0002432 on 2008.1.8 [begin]*/
        /*  Modified breif: 增加停止server线程时删除日志对象,
        避免启动时新建日志对象造成内存泄漏*/
        delete pPacketLogger;
        pPacketLogger = NULL;
        /*Modified by tangxp for BUG No.0002432 on 2008.1.8 [end]*/
        #endif
    }    
    /*Modified by tangxp for BUG NO.0002803 on 2008.2.29 [end]*/
}
/**
* 功能：关闭sa对应资源
*/
void ServerAgentWithDll::close() 
{
    if (closeDll != NULL)
    {
        closeDll(getAID());
        closeDll = NULL;
    }
}
/**
* 功能：发送数据
*/
int ServerAgentWithDll::sendPacket(Packet* packet) 
{
    if(packet == NULL)
    {
        sysLoger(LWARNING, "ServerAgentWithDll::sendPacket:Parameter packet is null pointer!");
        return 0;
    }
    if (put != NULL) 
    {
        /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [begin]*/
        /*  Modified brief: 将记录消息日志集成到接收数据中*/
        //记录消息日志
        if(getLogFlag() && getPacketLogger())
        {
            getPacketLogger()->write(SEND_PACK, *packet);
        }
        /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [end]*/
        put(getAID(),packet->getDes_said(),
            packet->getSrc_aid(),packet->getSrc_said(),
            packet->getData(),packet->getSize()) ;
    }
    else 
        return 0;
    return packet->getSize();
}
/**
* 功能：发送数据
* @return 接收包的大小
*/
int ServerAgentWithDll::receivePacket(Packet* packet) 
{
    if(packet == NULL)
    {
        sysLoger(LWARNING, "ServerAgentWithDll::receivePacket:Parameter packet is null pointer!");
        return -1;
    }
    if (get != NULL) 
    {
        short des_said,src_said,des_aid;
        int recvSize = get(getAID(),&des_aid,
            &des_said,&src_said,
            packet->getData(),PACKET_DATASIZE);
        if (recvSize >= 0) 
        {
            packet->setSrc_aid(getAID());
            packet->setDes_aid(des_aid);
            packet->setDes_said(des_said);
            packet->setSrc_said(src_said);
            packet->setSize(recvSize);
            /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [begin]*/
            /*  Modified brief: 将记录消息日志集成到接收数据中*/
            //记录消息日志
            if(getLogFlag() && getPacketLogger())
            {
                getPacketLogger()->write(RECV_PACK, *packet);
            }
            /*Modified by tangxp for BUG NO.0002550 on 2008.1.8 [end]*/
        }
        return recvSize;
    }
    else 
        return -1;
}
/**
* 功能：将配置信息序列化到OwnerArchive对象
*/
void ServerAgentWithDll::serialize(OwnerArchive& ar) 
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
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
}
#if 0
///////////////////////////TXThreadWithDll
TXThreadWithDll::TXThreadWithDll(ServerAgentWithDll* pServerAgent) {
    runFlag = true;
    this->pServerAgent = pServerAgent;
}

/**
* 功能：置反线程运行标志，调用此方法可停止线程
*/
void TXThreadWithDll::stop() {
    runFlag = false;
    pServerAgent->wakeUp();
}

/**
* 功能：线程运行函数,用来发送数据到server上
*/
void TXThreadWithDll::run() {
    while (runFlag) {
        pServerAgent->waitUp();
        while(true) {
            //从sa的缓冲区中读取一个包
            if (pServerAgent->getState() == 0)    {//sa的状态是退出,则退出线程
                stop();    
                break;
            }
            else {
                Packet* pack = pServerAgent->getpkt();
                if (pack == NULL)
                    break;
                //调用动态库的函数对包进行发送
                (pServerAgent->getPutFunc())(pServerAgent->getAID(),pack->getDes_said(),
                    pack->getSrc_aid(),pack->getSrc_said(),pack->getData(),pack->getSize()) ;
                
                if (pServerAgent->getLogFlag())
                    pServerAgent->getPacketLogger()->write(QString("TX:"),pack);
                PacketManager::getInstance()->free(pack);    //释放包
            }
         }
    }
}

///////////////////////////RXThreadWithDll
RXThreadWithDll::RXThreadWithDll(ServerAgentWithDll* pServerAgent) {
    runFlag = true;
    this->pServerAgent = pServerAgent;
}

/**
* 功能：置反线程运行标志，调用此方法可停止线程
*/
void RXThreadWithDll::stop() {
    runFlag = false;
    if (pServerAgent != NULL) {
        pServerAgent->close();
    }
}

/**
* 功能：线程运行函数
*/
void RXThreadWithDll::run() {
    int ret;
//    QByteArray BBB = pServerAgent->getBuf().toAscii();
//    char* pBuf = BBB.data(); 

    Packet* pack = PacketManager::getInstance()->alloc();
    short des_said,src_said,des_aid;
    while(runFlag){
        //调用动态库接收函数
        ret = (pServerAgent->getGetFunc())(pServerAgent->getAID(),&des_aid,
            &des_said,&src_said,pack->getData(),PACKET_DATASIZE) ;        
        
        if (ret <= 0 || !runFlag) {    //连接失败
            if (pServerAgent != NULL) {
                if (pServerAgent->getState() != 0) {
                    pServerAgent->setState(0);            //设置sa的状态为非活动型
                    //注销sa在router的注册
                    Router::getInstance()->unRegisterAgent(pServerAgent->getAID());
                }
                if (runFlag)
                    stop();
                //释放分配的包
                PacketManager::getInstance()->free(pack);
            }
            
            runFlag = false;
        } else {
            //设置源id号
            pack->setSize(ret);    //设置大小
            pack->setSrc_aid(pServerAgent->getAID());
            pack->setDes_said(des_said);
            pack->setSrc_said(src_said);
            pack->setDes_aid(des_aid);
            //调用Router的函数进行数据包的转发
            Router::getInstance()->putpkt(pack);     
            if (pServerAgent->getLogFlag())
                pServerAgent->getPacketLogger()->write(QString("RX:"),pack);
        }        
    }
    PacketManager::getInstance()->free(pack);
}
#endif
