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
    //ж�ض�̬��
    if (m_hDll != NULL)
    {
    //    releaseDll();        //�ͷſ��м���socket
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
* ���ܣ���routerע��,����server���պͷ����߳�
* @return true��ʾ�ɹ�
*/
bool ServerAgentWithDll::run() 
{
    if (m_hDll != NULL) 
    {
        FreeLibrary(m_hDll);
    }
//    m_hDll = LoadLibrary(getDllName().utf16());    //װ���ƶ��Ķ�̬��
    
#ifdef _UNICODE
    m_hDll = LoadLibrary(getDllName().utf16());    //װ���ƶ��Ķ�̬��
#else
    m_hDll = LoadLibrary(getDllName().toAscii());    //װ���ƶ��Ķ�̬��
#endif

    if (m_hDll == NULL)        //װ�ؿ�ʧ��
        return false;
    get = (GETPKT)GetProcAddress(m_hDll,STRGETPKT);
    put = (PUTPKT)GetProcAddress(m_hDll,STRPUTPKT);
    open = (OPEN)GetProcAddress(m_hDll,STROPEN);
    closeDll = (CLOSE)GetProcAddress(m_hDll,STRCLOSE);
//    releaseDll = (RELEASEDLL)GetProcAddress(m_hDll,STRRELEASEDLL);

    if (get == NULL || put == NULL || open == NULL || closeDll == NULL )
        return false;
    sysLoger(LDEBUG, "ServerAgentWithDll:dll [%d] inBuf:%s", getAID(), getInBuf().toAscii().data());
    int ret = open(getAID(), getInBuf().toAscii().data(), outBuff);    //�����ĵ���
    if (ret == -1) 
    {
        return false;
    }

    Router::getInstance()->registerAgent(this->getAID(), this);    //ע�ᵽrouter
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
    //    releaseDll();            //�ͷſ��м���socket
        pThreadRX->wait();        //�ȴ��߳��˳�
        delete  pThreadRX;
        pThreadRX = NULL;
    }
    
    //ж�ض�̬��
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
    /*  Modified brief: ����SAʱ�ر���־*/    
    //�ر���־�ļ�
    if (pPacketLogger != NULL)
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
void ServerAgentWithDll::close() 
{
    if (closeDll != NULL)
    {
        closeDll(getAID());
        closeDll = NULL;
    }
}
/**
* ���ܣ���������
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
        /*  Modified brief: ����¼��Ϣ��־���ɵ�����������*/
        //��¼��Ϣ��־
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
* ���ܣ���������
* @return ���հ��Ĵ�С
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
            /*  Modified brief: ����¼��Ϣ��־���ɵ�����������*/
            //��¼��Ϣ��־
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
* ���ܣ���������Ϣ���л���OwnerArchive����
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
* ���ܣ��÷��߳����б�־�����ô˷�����ֹͣ�߳�
*/
void TXThreadWithDll::stop() {
    runFlag = false;
    pServerAgent->wakeUp();
}

/**
* ���ܣ��߳����к���,�����������ݵ�server��
*/
void TXThreadWithDll::run() {
    while (runFlag) {
        pServerAgent->waitUp();
        while(true) {
            //��sa�Ļ������ж�ȡһ����
            if (pServerAgent->getState() == 0)    {//sa��״̬���˳�,���˳��߳�
                stop();    
                break;
            }
            else {
                Packet* pack = pServerAgent->getpkt();
                if (pack == NULL)
                    break;
                //���ö�̬��ĺ����԰����з���
                (pServerAgent->getPutFunc())(pServerAgent->getAID(),pack->getDes_said(),
                    pack->getSrc_aid(),pack->getSrc_said(),pack->getData(),pack->getSize()) ;
                
                if (pServerAgent->getLogFlag())
                    pServerAgent->getPacketLogger()->write(QString("TX:"),pack);
                PacketManager::getInstance()->free(pack);    //�ͷŰ�
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
* ���ܣ��÷��߳����б�־�����ô˷�����ֹͣ�߳�
*/
void RXThreadWithDll::stop() {
    runFlag = false;
    if (pServerAgent != NULL) {
        pServerAgent->close();
    }
}

/**
* ���ܣ��߳����к���
*/
void RXThreadWithDll::run() {
    int ret;
//    QByteArray BBB = pServerAgent->getBuf().toAscii();
//    char* pBuf = BBB.data(); 

    Packet* pack = PacketManager::getInstance()->alloc();
    short des_said,src_said,des_aid;
    while(runFlag){
        //���ö�̬����պ���
        ret = (pServerAgent->getGetFunc())(pServerAgent->getAID(),&des_aid,
            &des_said,&src_said,pack->getData(),PACKET_DATASIZE) ;        
        
        if (ret <= 0 || !runFlag) {    //����ʧ��
            if (pServerAgent != NULL) {
                if (pServerAgent->getState() != 0) {
                    pServerAgent->setState(0);            //����sa��״̬Ϊ�ǻ��
                    //ע��sa��router��ע��
                    Router::getInstance()->unRegisterAgent(pServerAgent->getAID());
                }
                if (runFlag)
                    stop();
                //�ͷŷ���İ�
                PacketManager::getInstance()->free(pack);
            }
            
            runFlag = false;
        } else {
            //����Դid��
            pack->setSize(ret);    //���ô�С
            pack->setSrc_aid(pServerAgent->getAID());
            pack->setDes_said(des_said);
            pack->setSrc_said(src_said);
            pack->setDes_aid(des_aid);
            //����Router�ĺ����������ݰ���ת��
            Router::getInstance()->putpkt(pack);     
            if (pServerAgent->getLogFlag())
                pServerAgent->getPacketLogger()->write(QString("RX:"),pack);
        }        
    }
    PacketManager::getInstance()->free(pack);
}
#endif
