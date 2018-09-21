/*
* ������ʷ��
*			  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  serverAgentWithDll.cpp
* @brief
*       ���ܣ�
*       <li>��̬��server�����ʵ��</li>
*/

/************************ͷ�ļ�********************************/
#include "serveragentwithdll.h"
#include "common.h"
#include "router.h"

/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/

const char* STRGETPKT = "getpkt";
const char* STRPUTPKT = "putpkt";
const char* STROPEN = "open";
const char* STRCLOSE = "close";
const char* STRRELEASEDLL = "releaseDll";

/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/

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
    {
        return false;
    }

    get = (GETPKT)GetProcAddress(m_hDll,STRGETPKT);
    put = (PUTPKT)GetProcAddress(m_hDll,STRPUTPKT);
    open = (OPEN)GetProcAddress(m_hDll,STROPEN);
    closeDll = (CLOSE)GetProcAddress(m_hDll,STRCLOSE);
    //    releaseDll = (RELEASEDLL)GetProcAddress(m_hDll,STRRELEASEDLL);

    if (get == NULL || put == NULL || open == NULL || closeDll == NULL )
    {
        return false;
    }

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
    mutex.lock();

    if (closeDll != NULL)
    {
        closeDll(this->getAID());
        closeDll = NULL;
    }

    mutex.unlock();

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

    }

    /*Modified by tangxp for BUG NO.0002803 on 2008.2.29 [end]*/
}
/**
* ���ܣ��ر�sa��Ӧ��Դ
*/
void ServerAgentWithDll::close()
{
    mutex.lock();

    if (closeDll != NULL)
    {
        closeDll(getAID());
        closeDll = NULL;
    }

    mutex.unlock();
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
    {
        return 0;
    }

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
        short des_said;
        short src_said;
        short des_aid;
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
    {
        return -1;
    }
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

