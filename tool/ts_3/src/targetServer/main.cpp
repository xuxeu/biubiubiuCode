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
* TS�����߳�
*
* @file     main.cpp
* @brief     TS�����߳�
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/

#include "Manager.h"
#include "ServerAgent.h"
#include "OwnerArchive.h"
#include "Config.h"
#include "router.h"
//#include "util.h"
#include "Device.h"
#include "common.h"
#include "SubAgentBase.h"
#include "tsConfig.h"
#include "serverAgentWithDll.h"
//#include <qapplication.h>
#include <qfile.h>
#include <QList>
#include <qregexp.h>
#include <qtextstream.h>
#include <qxml.h>
#include "xmlHandler.h"
#include "listen.h"
#include <stdlib.h>
#include <time.h>

////////////////////////
#include "Windows.h"
////////////////////////
using namespace std;

//TS�����ļ�����
/*Modified by tangxp for BUG NO.0002714 on 2008.3.3 [begin]*/
/*  Modified brief: ����ts����ģ���ļ�*/
const QString TS_TEMPLATE_FILENAME = "ts_template.xml";
/*Modified by tangxp for BUG NO.0002714 on 2008.3.3 [end]*/
const QString TS_FILENAME = "ts.xml";
const int socketlistenPort = 8000;
const char* CODEC_TYPE = "GBK";
const char CONTROL_FLAG_END[] = "#";

unsigned short Manager::counter = 0;
Manager* Manager::pManager = NULL;
const string Manager::tmpfilepath = "\\LoadFile\\";

extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow();

Manager::Manager(unsigned short aid) : AgentWithQueue(aid) 
{
    pListen = NULL;
    setLogFlag(true);
    setPacketLogger(NULL);
    for (int i=0; i<MAX_REG_TYPE + 2; i++)
    {
        agentArray[i] = NULL;
    }
    //Ĭ�ϵĶ˿ں�
    listenPort = socketlistenPort;
    //��ʼ������ID�б�
    initFreeIDList();
    //��ʼ��ӳ���
    mapAID2SA.clear();
    mapLnk2DID.clear();
    mapName2File.clear();
    mapName2ID.clear();
    mapSAID2SubAgent.clear();
    
    return;
}

Manager::~Manager() 
{
    delete pListen;
    pListen = NULL;
    mtxMapAID2SA.lock();
    QMap<short,AgentBase*>::const_iterator itr = mapAID2SA.constBegin();
    while (itr != mapAID2SA.constEnd()) 
    {
        delete itr.value();
        ++itr;
    }
    mapAID2SA.clear();
    mtxMapAID2SA.unlock();

    mtxMapName2ID.lock();
    mapName2ID.clear();
    mtxMapName2ID.unlock();

    //�ͷ�agentArray
    TMutexLocker locker(&mtxAgentArray);
    for(int i = 0; i < MAX_REG_TYPE + 2; i++)
    {
        if(agentArray[i] != NULL)
        {
            delete agentArray[i];
            agentArray[i] = NULL;
        }
    }
}


/**
* ���ܣ��Ѷ����������
* @param pSubAgent ��SubAgent�������map����mapSAID2SubAgent��
*/
void Manager::addSubAgent(SubAgentBase* pSubAgent) 
{
    if(NULL == pSubAgent)
    {
        sysLoger(LWARNING, "Manager::addSubAgent: Parameter pSubAgent is null pointer!");
        return;
    }
    try
    {
        mtxMapSAID2SubAgent.lock();
        mapSAID2SubAgent.insert(pSubAgent->getSaid(),pSubAgent);
        mtxMapSAID2SubAgent.unlock();
    }
    catch(...)
    {
        sysLoger(LERROR, "Manager::addSubAgent:Catch an exception when add sub agent to mapSAID2SubAgent!");
        return;
    }
}


/**
* ���ܣ���serverAgent����
* @param pServerAgent ��sa�������map����mapAID2SA��
* @param regType ע������
*/
void Manager::addServerAgent(AgentBase* pServerAgent) 
{
    TMutexLocker lockerAID2SA(&mtxMapAID2SA);
    if(mapAID2SA.size() > MAX_AGENT_NUM)
    {
        sysLoger(LWARNING, "Manager::addServerAgent: serverAgent's count expired!");
        return;
    }
    if (pServerAgent != NULL )
    {
        sysLoger(LDEBUG, "Manager::addserverAgent:Starting add the agent , type is %d, aid is %d .",
                                                pServerAgent->getType(), pServerAgent->getAID());
        QMap<short,AgentBase*>::const_iterator i = mapAID2SA.find((short)(pServerAgent->getAID()));
        if (i == mapAID2SA.constEnd()) 
        {    //û���ҵ���ע��
            mapAID2SA.insert(pServerAgent->getAID(), pServerAgent);
            switch (pServerAgent->getType()) 
            {
                case AgentBase::CLIENT_AGENT: 
                    {
                        sysLoger(LDEBUG, "Manager::addserverAgent:Add a clientAgent successfully!");
                    }
                    return;
                    
                case AgentBase::SERVER_AGENT: 
                    {
                        TMutexLocker lockerNm2ID(&mtxMapName2ID);
                        ServerAgent* pSa = dynamic_cast<ServerAgent*>(pServerAgent);
                        if(NULL == pSa)
                        {
                            sysLoger(LERROR, "Manager::addserverAgent: Dynamic cast pointer from AgentBase"\
                                                        " to ServerAgent Dll type failure!");
                            return;
                        }
                        mapName2ID.insert(pSa->getName(), pServerAgent->getAID());
                        sysLoger(LDEBUG, "Manager::addserverAgent:Add a ServerAgent successfully!");
                    }
                    return;
                    
                case AgentBase::SERVER_AGENT_WITH_DLL:
                    {
                        TMutexLocker lockerNm2ID(&mtxMapName2ID);
                        ServerAgentWithDll* pSa = dynamic_cast<ServerAgentWithDll*>(pServerAgent);
                        if(NULL == pSa)
                        {
                            sysLoger(LERROR, "Manager::addserverAgent: Dynamic cast pointer from AgentBase"\
                                                        " to ServerAgentWith Dll type failure!");
                            return;
                        }
                        mapName2ID.insert(pSa->getName(), pServerAgent->getAID());
                        sysLoger(LDEBUG, "Manager::addserverAgent:Add a ServerAgentWithDll successfully!");
                    }
                    return;
                    
                default:
                    sysLoger(LWARNING, "Manager::addserverAgent: Unknow type [%d] of server agent:%d!",
                                                        pServerAgent->getType(), pServerAgent->getAID());
                    return;
            }
        }
        sysLoger(LINFO, "Manager::addserverAgent: Already exited the agent ,  aid is %d .",
                                                pServerAgent->getAID());
        return;
    }
    sysLoger(LWARNING, "Manager::addServerAgent: Parameter pServerAgent is NULL pointer!");
    return;
}

/**
* ���ܣ�����TClient_setRegisterType����,����sa��ע������
* @param aid sa��aid��
* @param regType Ҫע������ͺ�
* @return true��ʾע��ɹ�,false��ʾ��ע���˴�aid
*/
bool Manager::setRegisterType(short aid,int regType,bool isStore) 
{
    sysLoger(LDEBUG, "Main::setRegisterType: received RegisterType command: aid: %d, Regtype:%d"\
                                    "isStore:%d", aid, regType, isStore);
    TMutexLocker locker(&mtxMapAID2SA);
    QMap<short,AgentBase*>::const_iterator m_CitrSA = mapAID2SA.find(aid);
    
    if ((m_CitrSA != mapAID2SA.end()) && (regType <= MAX_REG_TYPE && regType>=0)) 
    {    
        if (m_CitrSA.value()->getType() == AgentBase::CLIENT_AGENT)
        {
            sysLoger(LWARNING, "Main::setRegistertype: tried to set the type for Client agent !");
            return false;
        }
        AgentWithQueue* pSa = dynamic_cast<AgentWithQueue*>(m_CitrSA.value());
        if(NULL == pSa)
        {
            sysLoger(LWARNING, "Main::setRegisterType: serverAgent object is null, aid = %d", aid);
            return false;
        }
               
        //�����ע������
        pSa->setRegisterType(regType);
        sysLoger(LINFO, "Main::setRegisterType: set SA [aid:%d] to Regist type %d successful!", 
                                    aid, regType);
        //���и�SA ���ϱ�
        sendSAReportToCA(pSa);  
        if ((isStore) && (regType != INVALID_REG_TYPE))
            storeConfigure();
        return true;
    }
    sysLoger(LINFO, "Main:setRegisterType: set SA [aid:%d] to RegType %d failure!", aid, regType);
    return false;
}
/**
* ���ܣ�����TClient_clearRegisterType����,���sa��ע������
* @param aid sa��aid��
* @param regType Ҫ��������ͺ�,wΪ-1��ʾɾ�����еĶ���
* @return true��ʾע��ɹ�,false��ʾ��ע���˴�aid
*/
bool Manager::clearRegisterType(short aid) 
{
    AgentWithQueue *pAgentQ = dynamic_cast<AgentWithQueue*>(getServerAgentFromID(aid));
    if(NULL == pAgentQ)
    {
        sysLoger(LWARNING, "Main::clearRegisterType: Didn't find the Agent object by aid :%d .", aid);
        return false;
    }
    //�ϱ�
    sendSAReportToCA(pAgentQ);    //�ϱ�״̬
    pAgentQ->setRegisterType(INVALID_REG_TYPE);
        
    storeConfigure();
    sysLoger(LINFO, "Main::ClearRegisterType: SA unregistered the type :%d successful.", regType);
    return true;
}

bool Manager::mfCreateDirectory( QString &strDirectory )
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());    //ʶ������    
    QByteArray nameBlock = strDirectory.toAscii();
    WIN32_FIND_DATA d;
    HANDLE hHandle;
    if ((hHandle = FindFirstFile( nameBlock.data(), &d )) == INVALID_HANDLE_VALUE )
    {
        if ( CreateDirectory( nameBlock.data(), NULL ) == FALSE )
            return false;
    }
    else
    {
        FindClose( hHandle );
        return true;
    }
    return true;
}

bool Manager::createDirectory( QString &strDirectory )
{
    bool bFirst = true;
    for ( int i = 0; i < strDirectory.size(); i++ )
    {
        if ( strDirectory.at(i) == QChar('\\') )
        {
            if ( bFirst == true ) 
            {
                bFirst = false;
                continue;
            }
            QString strPath = strDirectory.left( i );
            if ( !mfCreateDirectory( strPath ) )
                return FALSE;
        }
    }
    return mfCreateDirectory( strDirectory );
}
/**
* ���ܣ��˳�manager
*/
void Manager::stop() 
{
    runFlag = false;
    wakeUp();
}

void Manager::main()
{
    //�и��߳�ѭ�� 
    //��routerע��manager����
    char p[256];
    GetCurrentDirectory(255,p);    //���뵱ǰ·��
    appDir =  QString::fromLocal8Bit(p);
    createDirectory(const_cast<QString &>(appDir + "\\ts\\log"));
    createDirectory(const_cast<QString &>(appDir + QString::fromStdString(tmpfilepath)));
    runFlag = true;
    //��router ע������
    Router::getInstance()->registerAgent(getAID(),this);
    //���������ļ�
    loadConfigure();
    Packet* pack =  NULL;
    //������յİ�
    QMap<short, SubAgentBase*>::const_iterator iSubA;

    while (runFlag) 
    {
        while(true) 
        {
            pack = getpkt();    //ȡ��
            if (pack == NULL)
                break;
            //��һ����ֹ��Ϣѭ���ı���
            if(pack->getSrc_aid() == MANAGER_AID)
            {
                sysLoger(LWARNING, "Main::main(): Received packet from itself, dead loop! FILE:%s, LINE:%d",
                                                    __MYFILE__, __LINE__);
                //�ͷŷ���İ�
                PacketManager::getInstance()->free(pack);
                continue;
            }
            iSubA = mapSAID2SubAgent.find(pack->getDes_said());
            if (iSubA != mapSAID2SubAgent.end()) 
            {    //�ҵ���
                iSubA.value()->process(pack); 
            }
            else 
            {    //�����������־��¼
                sysLoger(LWARNING, "Main::main(): subAgent with des_said %d did not exit!", 
                                                                        pack->getDes_said());
                
                //�ͷŷ���İ�
                PacketManager::getInstance()->free(pack);
            }

        }
        if (runFlag)
            waitUp();
    }  
    storeConfigure();    //����������Ϣ
    deleteAllServer();    //ɾ��manager��������з���    
    pListen->close();    
    /*Modified by tangxp for BUG NO.0002547 on 2008.1.10 [begin]*/
    /*  Modifide brief: ����ɾ��������������*/
    delete PacketManager::getInstance();
    /*Modified by tangxp for BUG NO.0002547 on 2008.1.10 [end]*/
}


/**
* ���ܣ�ɾ��ָ��server
* @param aid serverAgent��aid��
*/
int Manager::deleteServer(int aid) 
{
    try
    {
        if (aid == 0) 
        {
            deleteAllServer();
            storeConfigure();
            return TS_SUCCESS;
        }
        else 
        {
            sysLoger(LDEBUG, "Manager::deleteServer:Start to delete Agent object by aid %d",aid);
            TMutexLocker lock(&mtxMapAID2SA);
            AgentBase* pAgent = getServerAgentFromID(aid);
            if(NULL == pAgent)
            {
                sysLoger(LINFO, "Manager::deleteServer:Didn't find the Agent object by aid %d",aid);
                return TS_SERVICE_NOT_EXIST;
            }
            //���ɾ������CA��
            if(pAgent->getType() == AgentBase::CLIENT_AGENT)
            {
                //ɾ��CAע���б��еĶ���
                mtxAgentArray.lock();
                for(int i = 0; i < MAX_REG_TYPE; i++)
                {
                    if(agentArray[i] != NULL)
                    {
                        if(agentArray[i]->indexOf(pAgent) != 0)
                        {
                            agentArray[i]->removeAt(agentArray[i]->indexOf(pAgent));
                        }
                    }
                }
                mtxAgentArray.unlock();
                //ɾ��CA����
                QMap<short, AgentBase* >::iterator i = mapAID2SA.find(aid);
                mapAID2SA.remove(i.key());
                /*Modified by tangxp for BUG NO. 0002551 on 2008.1.15 [begin]*/
                /*  Modified brief: ɾ��CA����ʱ,����ֹͣCA����*/
                pAgent->stop();
                /*Modified by tangxp for BUG NO. 0002551 on 2008.1.15 [end]*/
                delete pAgent;
                pAgent = NULL;
                //�ͷ�aid
                freeAid(aid);
                return TS_SUCCESS;
            }
            //�����SA����
            AgentWithQueue* pServerAgent = dynamic_cast<AgentWithQueue*>(pAgent);
            
            if (pServerAgent != NULL) 
            {
                //ǿ��ת����sa����
                switch (pServerAgent->getType()) 
                {               
                    case AgentWithQueue::SERVER_AGENT: 
                        {
                            ServerAgent* pSa = dynamic_cast<ServerAgent*>(pServerAgent);                        
                            pSa->stop();
                            //�ϱ�ɾ������
                            pServerAgent->setState(SASTATE_DELETED);
                            sendSAReportToCA(pServerAgent);
                            TMutexLocker locker(&mtxMapName2ID);
                            mapName2ID.remove(pSa->getName());    //ɾ�����ֵ�ID��ӳ��
                        }
                        break;
                        
                    case AgentBase::SERVER_AGENT_WITH_DLL:
                        {
                            ServerAgentWithDll* pSa = dynamic_cast<ServerAgentWithDll*>(pServerAgent);
                            pSa->stop();
                            //�ϱ�ɾ������
                            pServerAgent->setState(SASTATE_DELETED);
                            sendSAReportToCA(pServerAgent);
                            TMutexLocker locker(&mtxMapName2ID);
                            mapName2ID.remove(pSa->getName());    //ɾ�����ֵ�ID��ӳ��
                        }
                        break;
                        
                    default:
                        //������CAҲ����SA������ܷ�����ϵͳ����
                        sysLoger(LWARNING, "Main::deleteServer: agent is unknow type!");
                        return TS_SYSTEM_ERROR;
                }        

                Router::getInstance()->unRegisterAgent(pServerAgent->getAID());    //ɾ����router��ע��
                mapAID2SA.remove(pServerAgent->getAID());
                if(pServerAgent->getRegisterType() != INVALID_REG_TYPE)
                {
                    storeConfigure();
                }
                delete pServerAgent;
                pServerAgent = NULL;

                freeAid(aid);
                sysLoger(LDEBUG, "Manager::deleteServer:Successs to delete Agent object by aid %d",aid);
                return TS_SUCCESS;
            }
            else
            {
                sysLoger(LWARNING, "Main::deleteserver: dynamic_cast object from AgentBase to "\
                                                    "AgentwithQueue failure! ");            
            }

        }
    }
    catch(...)
    {
        sysLoger(LWARNING, "Main::deleteserver:Catch an exception when proscessing delete server!");
        return TS_SYSTEM_ERROR;
    }
    return TS_SYSTEM_ERROR;
}
/**
* ���ܣ�ɾ������server
*/
void Manager::deleteAllServer() 
{
    QMap<short,AgentBase*>::const_iterator itrSA = mapAID2SA.constBegin();
    AgentWithQueue* pAgent = NULL;
    QVector<short> shortList;
    for(itrSA = mapAID2SA.constBegin(); itrSA != mapAID2SA.constEnd(); itrSA++)
    {
        if(itrSA == mapAID2SA.constEnd())
        {
            break;
        }
        if(itrSA.value() == NULL)
        {
            shortList.append(itrSA.key());
            continue;
        }
        if (itrSA.value()->getType() == AgentBase::CLIENT_AGENT) 
        {
            //ɾ��ע����������Ǽ�
            mtxAgentArray.lock();
            for(int i = 0; i < MAX_REG_TYPE; i++)
            {
                if(agentArray[i] != NULL)
                {
                    if(agentArray[i]->indexOf(pAgent) != 0)
                    {
                        agentArray[i]->removeAt(agentArray[i]->indexOf(pAgent));
                    }
                }
            }
            mtxAgentArray.unlock();            
            //ɾ�����д���ʱ��ɾ��CA,������ɾ������Ŀͻ����޷��յ��ظ�
            continue;
        }
        pAgent = dynamic_cast<AgentWithQueue*>(itrSA.value());
        switch(pAgent->getType()) 
        {
            case AgentWithQueue::SERVER_AGENT:
                {
                    ServerAgent* pServerAgent = dynamic_cast<ServerAgent*>(pAgent);
                    pServerAgent->stop();        //ֹͣserver���߳�
                    //�ϱ�ɾ��״̬
                    pServerAgent->setState(SASTATE_DELETED);
                    sendSAReportToCA(pServerAgent);
                    mapName2ID.remove(pServerAgent->getName());    //ɾ�����ֵ�ID��ӳ��
                }
                break;
            case AgentBase::SERVER_AGENT_WITH_DLL:
                {
                    ServerAgentWithDll* pServerAgent = dynamic_cast<ServerAgentWithDll*>(pAgent);
                    pServerAgent->stop();        //ֹͣserver���߳�
                    //�ϱ�ɾ��״̬
                    pServerAgent->setState(SASTATE_DELETED);
                    sendSAReportToCA(pServerAgent);
                    mapName2ID.remove(pServerAgent->getName());    //ɾ�����ֵ�ID��ӳ��
                }
                break;
            default:
                sysLoger(LWARNING, "Main::deleteAllServer: agent %d is not server Agent!", 
                                                        pAgent->getAID());
                continue;
                break;
        }
        //ȡ����Routerģ���ע����Ϣ
        Router::getInstance()->unRegisterAgent(pAgent->getAID());    //ɾ����router��ע��
        shortList.append(itrSA.key());
        delete pAgent;
        pAgent = NULL;
    }
    
    TMutexLocker lock(&mtxMapAID2SA);
    for(int j=0; j<shortList.size();++j) 
    {
        mapAID2SA.remove(shortList[j]);
        /*Modified by tangxp for BUG NO.0002545 on 2008.1.17 [begin]*/
        /*  Modified brief: ����ɾ�����д���ʱ,���ͷ�CA��AID*/
        freeAid(shortList[j]);
        /*Modified by tangxp for BUG NO.0002545 on 2008.1.17 [end]*/
    }
  
    return;
}

/**
* ���ܣ����������ļ��ɹ����������������ӦServerAgent�ͼ����߳�ListenerThread
* @return
*
*/
void Manager::loadConfigure()
{
    //��ȡTS.ini�ļ���Ϣ
    // fn = filename;
    //���������߳�
    if (pListen != NULL)
        delete pListen;
    pListen = new Listen(listenPort);    //�����˿�
    pListen->start();

    /*Modified by tangxp for BUG NO.0002714 on 2008.3.3 [begin]*/
    /*  Modified brief: ���������ļ�ʧ��,��ʹ��ģ��ָ�,�����´�*/
    QString tsFilename(appDir + QString("\\configuration\\") + TS_FILENAME);
    QFile f(tsFilename);
    
    if (!f.open(QIODevice::ReadOnly))
    {
        QDir dir;
        if(!dir.exists(appDir + QString("\\configuration\\")))
        {
            dir.mkdir(appDir + QString("\\configuration\\"));
        }
        
        SystemLog::getInstance()->setLogLevel(LWARNING);        
        sysLoger(LWARNING, "Manager::loadConfigure:Open targetServer's config file failure!");

        //����ģ�������ļ�
        bool ret = f.copy(appDir + QString("\\..\\pub\\") + TS_TEMPLATE_FILENAME, 
                                tsFilename);
        if(!ret)
        {
            sysLoger(LWARNING, "Manager::loadConfigure:Recover targetServer's config file from template failure!");
            return ;
        }
        sysLoger(LWARNING, "Manager::loadConfigure:Recover targetServer's config file from template success!");

        if(!f.open(QIODevice::ReadOnly))
        {
            sysLoger(LWARNING, "Manager::loadConfigure:Open targetServer's config file failure!");
            return;
        }
    }

    /*Modified by tangxp for BUG NO.0002714 on 2008.3.3 [end]*/
    
    QByteArray block = f.readAll();
    //������ļ��ж�ȡ���ĵ�����
    QString buf = QString::fromLocal8Bit(block.data(),block.size());
    QXmlInputSource in;//(&f);
    in.setData(buf);
    QXmlSimpleReader reader;
    // don't click on these!
    reader.setFeature("http://xml.org/sax/features/namespaces", false);
    reader.setFeature("http://xml.org/sax/features/namespace-prefixes", true);
    reader.setFeature("http://trolltech.com/xml/features/report-whitespace"
        "-only-CharData", false);
    XmlHandler *hand = new XmlHandler();    //��������
    reader.setContentHandler(hand);
    reader.setErrorHandler(hand);

    reader.parse(in);    //��ȡ�ļ�����
    reader.setContentHandler(0);
    reader.setErrorHandler(0);
    delete hand;
    f.close();
}

/**
* ���ܣ�����TClient_register����,TS����ע��,ע���,TS����TC�Զ�������Ϣ��״̬
* @param pack ���յĿ��ư�
* @return true��ʾע��ɹ�,false��ʾ��ע���˴�aid
*/
int Manager::registerServer(Packet* pack,int regType)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "Main::registerServer : Parameter pack is null pointer!");
        return TS_NULLPTR_ENCONTERED;
    }
    //����aid��, ���ҳ���AID�����CA���󣬿��Ƿ��Ѿ���agentarray
    //��ע��
    if(regType < 0 || regType > MAX_REG_TYPE)
    {
        sysLoger(LWARNING, "Main::registerServer : Invalid regType %d", regType);
        return TS_INVALID_REG_TYPE;
    }
    AgentBase*pCa = getServerAgentFromID(pack->getSrc_aid());
    /*Modified by tangxp for BOG NO.0002814 on 2008.3.3 [begin]*/
    /*  Modified brief: ���ע������Ƿ���CA*/
    if((NULL == pCa) || (AgentBase::CLIENT_AGENT != pCa->getType()))
    {
        sysLoger(LERROR, "Main::registerServer: Didn't find the Clinet Agent by aid %d", 
                                        pack->getSrc_aid());
        return TS_INVALID_SERVICE_ID;
    }
    /*Modified by tangxp for BOG NO.0002814 on 2008.3.3 [end]*/
    TMutexLocker lockerArray(&mtxAgentArray);
    if(NULL == agentArray[regType])
    {
        //��������
        sysLoger(LINFO, "Main::registerServer: No other ClientAgent registered this type %d before!",
                                        regType);
        QList<AgentBase*> *pRegList = new QList<AgentBase*>();
        if(NULL == pRegList)
        {
            sysLoger(LWARNING, "Main::registerServer: Alloc register list for RegType %d failure!", regType);
            return TS_SYSTEM_ERROR;
        }
        agentArray[regType] = pRegList;
        pRegList->append(pCa);
        sysLoger(LINFO, "Main::registerServer: Registered for RegType %d Successfully!", regType);
        return TS_SUCCESS;
    }
    if(agentArray[regType]->indexOf(pCa) == -1)
    {
        //��δ��agentArray��ע���CA
        agentArray[regType]->append(pCa);        
    }
    sysLoger(LINFO, "Main::registerServer: Registered for RegType %d Successfully!", regType);
    return TS_SUCCESS;
}
/**
* ���ܣ�����TClient_unregister����,ע��CA���������ע��
* @param pack ���յĿ��ư�
* @return true��ʾע��ɹ�,false��ʾ��ע���˴�aid
*/
int Manager::unRegisterServer(Packet* pack,int regType) 
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "Main::unRegisterServer : Parameter pack is null pointer!");
        return TS_NULLPTR_ENCONTERED;
    }
    //����aid��
    if(regType < 0 || regType > MAX_REG_TYPE)
    {
        sysLoger(LWARNING, "Main::unRegisterServer : Invalid regType %d", regType);
        return TS_INVALID_REG_TYPE;
    }
    AgentBase *pCa = getServerAgentFromID(pack->getSrc_aid());
    if(NULL == pCa)
    {
        sysLoger(LERROR, "Main::unRegisterServer: Didn't find the Client Agent by aid %d",
                                     pack->getSrc_aid());
        return TS_INVALID_SERVICE_ID;
    }
    if(NULL ==  agentArray[regType])
    {
        sysLoger(LINFO, "Main::unRegisterServer: No ClientAgent registered to this type %d", 
                                        regType);
        return TS_SUCCESS;
    }
    mtxAgentArray.lock();
    if(agentArray[regType]->indexOf(pCa) != -1)
    {
        //����agentArray��ע���CA
        agentArray[regType]->removeAt(agentArray[regType]->indexOf(pCa));        
    }
    sysLoger(LINFO, "Main::unRegisterServer:Unregistered ClientAgent %d from this type %d successfully", 
                                    pCa->getAID(), regType);
    mtxAgentArray.unlock();
    return TS_SUCCESS;
}

/**
 * @Funcname:  sendSAReportToCA
 * @brief        :  ����SA���ϱ���Ϣ��ע������͵�CA
 * @para1      : AgentWithQueue * pSA ���ϱ���Ϣ��SA����
 * @para2      : caAid ��Ҫ������Ϣ��ָ��CAaid, ���Ϊ0��
 *                       ��������ע���˸����͵�CA������
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��1��8�� 11:26:18
 *   
**/
void Manager::sendSAReportToCA(AgentWithQueue * pSA, US caAid)
{
    if(NULL == pSA)
    {
        sysLoger(LWARNING, "Main::sendSAReportToCA: Parameter pSA is NULL pointer!");
        return;
    }
    //����SA���ϱ����ݣ����͸����е�CA
    char *pBuf = NULL;
    size_t tmpLen = 0;
    comConfig *pcomCfg = NULL;
    char extInfo[EXT_INFO_LEN] = "\0";
    //���ע��������Ч�򲻽����ϱ�
    if(pSA->getRegisterType() == INVALID_REG_TYPE)
    {
        return;
    }
    Packet *pack = PacketManager::getInstance()->alloc();
    if(NULL == pack)
    {
        Packet packet;
        pack = &packet;
    }
    pack->setSrc_aid(MANAGER_AID);
    pack->setDes_aid(0);     //��ʱ����Ϊ0,���͵�ʱ����ΪCAaid
    pack->setSrc_said(0);
    pack->setDes_said(0);
    pBuf = pack->getData();
    pack->clearData();
    /**
    �ϱ����ĸ�ʽ��
    ----------------------------------------------------------------------------
    |aid,|name,|extInfo|,|reportType,|sate,|islog,|channelCount,|currentChannel,|config of each channel,|
    ----------------------------------------------------------------------------
    */
    sysLoger(LDEBUG, "Main::sendSAReportToCA:Send SA's [%s] config to CA %d", 
                                    pSA->getName().toAscii().data(), caAid);
    //DID,��ռλ�����淢����ʱ���ٸ��ݾ����CA����
    __store(0, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //aid
    pBuf += int2hex(pBuf, 10, pSA->getAID());
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //name
    tmpLen = min(NAME_SIZE, pSA->getName().toAscii().length());
    memcpy(pBuf, pSA->getName().toAscii().data(), tmpLen);
    pBuf += tmpLen;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //extInfo
    memcpy(extInfo, pSA->getExtInfo().toAscii().data(), 
                    min(EXT_INFO_LEN, pSA->getExtInfo().toAscii().length()));
    memcpy(pBuf, extInfo, sizeof(extInfo));
    pBuf += sizeof(extInfo);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //reportType          4 bytes
    pBuf += int2hex(pBuf, 10, pSA->getRegisterType());
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //state
    /*Attention: set the state to 2 when delete the server */
    pBuf += int2hex(pBuf, 10, pSA->getState());
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //isLog
    pBuf += int2hex(pBuf, 10, pSA->getLogFlag());
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //channelcount
    pBuf += int2hex(pBuf, 10, pSA->getChannelCount());
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //currentchannel
    pBuf += int2hex(pBuf, 10, pSA->getChannelType());
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    if(pSA->getType() == AgentBase::SERVER_AGENT)
    {
        //config of each channel
        QVector<QString> channelInfo = pSA->getChannelInfo();
        for(int i = 0; i < pSA->getChannelCount(); i++)
        {
            if(channelInfo.size() < pSA->getChannelCount())
            {
                sysLoger(LERROR, "Main::sendSAReportToCA: the server Agent lost channel config!");
                break;
            }
            //ֱ�ӽ�ÿ��ͨ�������ַ�������
            memcpy(pBuf, channelInfo[i].toAscii().data(), channelInfo[i].toAscii().length());
            pBuf += channelInfo[i].toAscii().length();
        }
    }
    else if(pSA->getType() == AgentBase::SERVER_AGENT_WITH_DLL)
    {
        //protocol,way,dllName,dllinbuf, dlloutbuf
        ServerAgentWithDll *pSaDll = dynamic_cast<ServerAgentWithDll*>(pSA);
        if(pSaDll == NULL)
        {
            sysLoger(LERROR, "Main::sendSAReportToCA: Dynamic cast pointer pSA from AgentWithQueue"\
                                        " to ServerAgentWithDll failure!");
            PacketManager::getInstance()->free(pack);
            return;
        }
        pBuf += int2hex(pBuf, 10, DLL);
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        pBuf += int2hex(pBuf, 10, DLL);
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        //dllname
        memcpy(pBuf, pSaDll->getDllName().toAscii().data(), 
                                    pSaDll->getDllName().toAscii().length());
        pBuf += pSaDll->getDllName().toAscii().length();
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        //dllInbuf
        memcpy(pBuf, pSaDll->getInBuf().toAscii().data(), pSaDll->getInBuf().toAscii().length());
        pBuf += pSaDll->getInBuf().toAscii().length();
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        //dllOutbuf
        memcpy(pBuf, pSaDll->getOutBuf(), strlen(pSaDll->getOutBuf()));
        pBuf += strlen(pSaDll->getOutBuf());
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
    }
    
    //set the data size of the pack
    pack->setSize(pBuf - pack->getData());
    //send to the pack to each of the CA in vcCAList
    if((agentArray[pSA->getRegisterType()] == NULL)
        || (agentArray[pSA->getRegisterType()]->size() <= 0))
    {
        sysLoger(LINFO, "Main::sendSAReportToCA: no CA registered to this type %d",
                                   pSA->getRegisterType());
        PacketManager::getInstance()->free(pack);
        return;
    }
    if(0 == caAid)
    {
        //���͸�����ע���˸����͵�CA
        /*Modified by tangxp for BUG NO.0002814 on 2008.3.3 [begin]*/
        /*  Modified brief: ����ע�����͵�CA�����б���ʻ���*/
        TMutexLocker locker(&mtxAgentArray);
        /*Modified by tangxp for BUG NO.0002814 on 2008.3.3 [end]*/
        QList<AgentBase*>::iterator m_ItrCA = agentArray[pSA->getRegisterType()]->begin();
        for(; m_ItrCA != agentArray[pSA->getRegisterType()]->end(); m_ItrCA++)
        {
            //���ð���Ŀ�ĵ�Ϊע���˸����͵�CA
            pack->setDes_aid((*m_ItrCA)->getAID());
            //���ݾ����CA����DID
            __store(CURDID((*m_ItrCA)->getAID()), pack->getData(), DIDLEN);
            //����������־
            if(getLogFlag())
            {
                TMutexLocker lockerSubAgent(&mtxMapSAID2SubAgent);
                QMap<short, SubAgentBase*>::const_iterator cItrSubAgent = 
                                                                mapSAID2SubAgent.find(pack->getSrc_said());
                ControlSubAgent *pCtrlAgent = NULL;
                if(cItrSubAgent != mapSAID2SubAgent.constEnd())
                {
                    pCtrlAgent = dynamic_cast<ControlSubAgent*>(cItrSubAgent.value());
                    if(pCtrlAgent != NULL)
                    {
                        pCtrlAgent->writePackForManager(SEND_PACK, *pack);
                    }
                }
                pCtrlAgent = NULL;
            }
            //����
            Router::getInstance()->putpkt(pack, false);
        }
    }
    else
    {
        //���͸�ָ����CA
        pack->setDes_aid(caAid);
        //����CA����DID
        __store(CURDID(caAid), pack->getData(), DIDLEN);
        //����������־
        if(getLogFlag())
        {
            TMutexLocker lockerSubAgent(&mtxMapSAID2SubAgent);
            QMap<short, SubAgentBase*>::const_iterator cItrSubAgent = 
                                                            mapSAID2SubAgent.find(pack->getSrc_said());
            ControlSubAgent *pCtrlAgent = NULL;
            if(cItrSubAgent != mapSAID2SubAgent.constEnd())
            {
                pCtrlAgent = dynamic_cast<ControlSubAgent*>(cItrSubAgent.value());
                if(pCtrlAgent != NULL)
                {
                    pCtrlAgent->writePackForManager(SEND_PACK, *pack);
                }
            }
            pCtrlAgent = NULL;
        }
        //����
        Router::getInstance()->putpkt(pack, false);
    }
    //free the pack
    PacketManager::getInstance()->free(pack);
    return;
}
/**
*���ܣ�����ĳ������SA���ϱ�
* regType: ��Ҫ�ϱ���ע�����ͺ�
* caAid�� �ϱ���ϢĿ�ĵأ�0��ʾ���е�CA������
*/
void Manager::sendRegTypeSAToCA(int regType, US caAid)
{
    //��ѯmapAid2SAӳ����ҳ�����ע���˸����͵�SA,
    //�����ϱ�
    AgentWithQueue *pAgentQ = NULL;
    QMap<short, AgentBase*>::const_iterator cItrAgent = mapAID2SA.constBegin();
    while(cItrAgent != mapAID2SA.constEnd())
    {
        if((cItrAgent.value()->getType() != AgentBase::SERVER_AGENT)
            && (cItrAgent.value()->getType() != AgentBase::SERVER_AGENT_WITH_DLL))
        {
            cItrAgent++;
            continue;
        }
        pAgentQ = dynamic_cast<AgentWithQueue*>(cItrAgent.value());
        if(NULL == pAgentQ)
        {
            sysLoger(LWARNING, "Manager::sendRegTypeSAToCA: dynamic cast pointer of class agentbase"\
                                                " to class AgentWithQueue failure!");
            cItrAgent++;
            continue;
        }
        //�Ǹ����ͣ�������Ŀ����������,������ϱ�
        if((pAgentQ!= NULL) && (pAgentQ->getRegisterType() == regType))
        {
            sendSAReportToCA(pAgentQ, caAid);
        }
        cItrAgent++;
    }
    return;
}

/**
* ���ܣ���������Ϣ���浽�ļ���ȥ
* @return �ɹ�����true��ʧ�ܷ���false
*/
bool Manager::storeConfigure() 
{
    try
    {
        QFile f(appDir + QString("\\configuration\\") + TS_FILENAME);
        if(!f.open(QIODevice::WriteOnly))
        {
            sysLoger(LWARNING, "Main:: storeconfigure failure, because of opening config file failure!");
            return false;
        }
        QTextStream t(&f);
        t << "<!DOCTYPE TS>\n";
        t << "<TS>\n";
        //�˿ں�
        t << "    <ListenConfig";
        t <<  " port=\"" << listenPort << "\">\n";
        t << "    </ListenConfig>\n";
        //ϵͳ��־�������
        t << "    <SystemLogConfig";
        t << " currentLevel=\"" << SystemLog::getInstance()->getLogLevel()<< "\">\n";
        t << "    </SystemLogConfig>\n";

        AgentWithQueue* pAgent = NULL;
        QMap<short,AgentBase*>::const_iterator m_CitrSA = mapAID2SA.constBegin();
        //����TS.xml�ļ���ʽ
        while (m_CitrSA != mapAID2SA.constEnd())
        {

            if((m_CitrSA.value() == NULL) || (m_CitrSA.value()->getType() == AgentBase::CLIENT_AGENT)) 
            {
                ++m_CitrSA;
                continue;
            }
            AgentBase *pAgentBs = m_CitrSA.value();
            try
            {
                pAgent = dynamic_cast<AgentWithQueue*>(pAgentBs);//(m_CitrSA.value());
            }
            catch(std::__non_rtti_object)
            {
                sysLoger(LWARNING, "Main:: storeconfigure:Catch the std::__non_rtti_object exception.");
                ++m_CitrSA;
                continue;
            }
            if(NULL == pAgent)
            {
                sysLoger(LWARNING, "Main:: storeconfigure: Dynamic cast pointer of class AgentBase to "\
                                                "class AgentWithQueue failure!");
                ++m_CitrSA;
                continue;
            }
            QString reportType;
            reportType = QString("%1").arg(pAgent->getRegisterType());
            //������sa�������е�����        
            if(pAgent->getRegisterType() == INVALID_REG_TYPE)
            {
                ++m_CitrSA;
                continue;
            }
            switch(pAgent->getType()) 
            {
                case AgentBase::SERVER_AGENT:
                    {
                        ServerAgent* pServerAgent = dynamic_cast<ServerAgent*>(pAgent);
                        if(pServerAgent != NULL)
                        {
                            t << "    <serverAgent";
                            //����
                            t << " name=\"" << pServerAgent->getName() << "\" ";
                            //�ϱ�����
                            QString str ;                            
                            t << " reportType=\"" << reportType << "\"";
                            //״̬
                            str = QString("%1").arg(pServerAgent->getState());
                            t << " state=\"" << str << "\"";
                            //�Ƿ���־��¼
                            str = QString("%1").arg(pServerAgent->getLogFlag());
                            t << " logFlag=\"" << str << "\"";
                            //��ǰͨ����
                            str = QString("%1").arg(pServerAgent->getChannelType());
                            t << " channelType=\"" << str << "\"";
                            //��չ��Ϣ
                            t << " extInfo=\"" << pServerAgent->getExtInfo() << "\">\n";
                            //����ͨ��������Ϣ
                            QVector<QString> channelInfo = pServerAgent->getChannelInfo();
                            for (int i=0; i<pServerAgent->getChannelCount();++i) 
                            {
                                QVector<QString> vec;
                                //QString buf(block) ;
                                //����ת��,ת����UTF8
                                QString strCh;
                                QString strProtocol;
                                int next = 0;    //��ѯ�Ŀ�ʼλ��
                                while(true) 
                                {
                                    strCh = channelInfo[i].section(CONTROL_FLAG,next,next);
                                    if (strCh.size() == 0)
                                        break;
                                    vec.append(strCh);
                                    next++;
                                }
                                int iCount = 0;
                                bool ok = false;
                                t << "        <channelInfo" << i ;
                                strProtocol = vec[iCount++];    //�õ�Э������
                                t << " way=\"" << vec[iCount] << "\"";
                                //�豸����
                                switch(vec[iCount++].toInt()) 
                                {
                                    case DeviceConfigBase::TCP_DEVICE:    //TCP
                                        t << " IP=\"" << vec[iCount++] << "\"";                                                //�˿�

                                        t << " port=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                                //IP
                                        break;
                                    case DeviceConfigBase::UDP_DEVICE:    //UDP
                                        t << " IP=\"" << vec[iCount++] << "\"";                                            //�˿�

                                        t << " port=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                            //IP
                                        t << " timeout=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                    //��ʱ
                                        t << " retry=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                    //���Դ���
                                        break;
                                    case DeviceConfigBase::UART_DEVICE:    //����
                                        t << " com=\"" << vec[iCount++] << "\"";                            //���ں�

                                        t << " baud=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                        //������
                                        t << " timeout=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                    //��ʱ
                                        t << " retry=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                    //���Դ���
                                        break;
                                        
                                    case DeviceConfigBase::USB_DEVICE:
                                        t << " GUID=\"" <<  vec[iCount] << "\""; 
                                        break;
                                        
                                    default:
                                        sysLoger(LWARNING, "Main::storeConfigure: unknow deviceType [%d] of SA");
                                        break;
                                }

                                t << " protocolType=\"" << strProtocol << "\">\n";                                        //Э������
                                t << "        </channelInfo"<< i << ">" << "\n";
                            }
                            t << "    </serverAgent>\n";    
                        }
                    }
                    break;
                case AgentBase::SERVER_AGENT_WITH_DLL:
                    {
                        ServerAgentWithDll* pServerAgent = dynamic_cast<ServerAgentWithDll*>(pAgent);
                        if(pServerAgent != NULL)
                        {
                            /*Modified by tangxp on 2007.11.12 [begin]*/
                            /*  brief: ���泣פģ��������Ϣ*/
                            if( PSA_REG_TYPE == pServerAgent->getRegisterType() )
                            { 
                                //��פģ��
                                t << "    <permanentServerAgent>\n";
                                t << "        <psa";
                                t << " name=\"" << pServerAgent->getName() << "\"";             //��̬������

                                t << " dllName=\"" << pServerAgent->getDllName() << "\"";       //��̬���ļ���

                                QString str = QString("%1").arg(pServerAgent->getLogFlag());    
                                t << " logFlag=\"" << str << "\"";                              //��־���

                                t << " extInfo=\"" << pServerAgent->getInBuf() << "\">\n";      //��̬���ʼ������
                                t << "        </psa>\n";
                                t << "    </permanentServerAgent>\n";                        
                            }
                            else
                            {
                                //��ͨ��̬��
                                t << "    <serverAgentWithDll";
                                t << " name=\"" << pServerAgent->getName() << "\" ";                            //����
                                
                                t << " reportType=\"" << reportType << "\"";                                         //ע������
                                
                                t << " dllName=\"" << pServerAgent->getDllName() << "\"";                        //��̬������                        //״̬

                                QString str = QString("%1").arg(pServerAgent->getState());                                //״̬
                                t << " state=\"" << str << "\"";                                                //״̬

                                str = QString("%1").arg(pServerAgent->getLogFlag());                            //�Ƿ���־��¼
                                t << " logFlag=\"" << str << "\"\n";                                            //�Ƿ���־��¼
                                //��չ��Ϣ
                                t << " extInfo=\"" << pServerAgent->getExtInfo() << "\"";

                                t << "        dllBuf=\"" << pServerAgent->getInBuf() << "\">\n";                    //״̬
                                t << "    </serverAgentWithDll>\n";
                            }
                            /*Modified by tangxp on 2007.11.12 [end]*/    
                        }
                    }
                    break;
                default:
                    sysLoger(LWARNING, "Main::storeConfigure: unknow type [] of server agent!",
                                                        pAgent->getType());
                    break;
            }
            ++m_CitrSA;
        }

        t << "</TS>\n";
        f.close();
    }
    catch(...)
    {
        sysLoger(LWARNING, "storeConfigure: Catch an exception when store targetServer configure file!");
    }
    return true;
}
/**
* ����: ͨ�����ֵõ���Ӧ��ID��
* @param name ����
* return ID ��-1��ʾû��
*/
int Manager::getIDFromName(const QString& name) 
{
    QMap<QString, short>::const_iterator cItrID = mapName2ID.find(name);
    if (cItrID != mapName2ID.end())
        return cItrID.value();
    sysLoger(LINFO, "Main::getIDFromName: didn't find the name [%s] in mapName2ID",
                                name.toAscii().data());
    return -1; 
}

/**
* ����: ͨ��AID�ŵõ�serverAgent����
* @param aid aid��
* return ServerAgent* NULL��ʾû�д�aid��,
*/
AgentBase* Manager::getServerAgentFromID(short aid) 
{
    QMap<short,AgentBase* >::const_iterator cItrSA = mapAID2SA.find(aid);
    if (cItrSA != mapAID2SA.end())
        return cItrSA.value();
    sysLoger(LINFO, "Main::getServerAgentFromID: didn't find the ID [%d] in mapAID2SA",
                                aid);
    return NULL; 
    
}
/**
* ���ܣ��޸�ӳ��mapAID2SA
* @param oldName Ҫ�滻��ԭ������
* @param newName �滻�������
*/
void Manager::modifyName2AIDMap(QString oldName,QString newName) 
{
    QMap<QString ,short>::const_iterator cItrName = mapName2ID.find(oldName);
    if (cItrName != mapName2ID.end()) 
    {    //�ҵ���Ҫ�޸ĵ����֣�������޸�
        int aid = cItrName.value();
        //�޸Ķ����ﱣ�������
        AgentWithQueue *pAgent = dynamic_cast<AgentWithQueue*>(getServerAgentFromID(aid));
        if(NULL == pAgent)
        {
            sysLoger(LERROR, "Main::modifyName2AIDMap: dynamic cast pointer of server agent from"\
                                           "AgentBase to AgentWithQueue failure, modified name from %s to %s failure!",
                                            oldName.toAscii().data(), newName.toAscii().data());
            return;
        }
        //�޸Ķ����ﱣ�������
        pAgent->setName(newName);
        //�޸�mapName2IDӳ����������
        mtxMapName2ID.lock();
        mapName2ID.remove(oldName);
        mapName2ID.insert(newName,aid);
        mtxMapName2ID.unlock();
        return;
    }
    sysLoger(LINFO, "Main::modifyName2AIDMap: didn't find the name [%s] to be modified!", 
                                oldName.toAscii().data());
    return;
}

/**
 * @Funcname:  initFreeIDList
 * @brief        :  ��ʼ������ID�б�
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��1��8�� 10:15:08
 *   
**/
void Manager::initFreeIDList()
{
    mtxFreeIDList.lock();
    //0��Managerռ��, IDֻ�õ�short�͵�����������, 
    //��Ϊ���ݰ��ȶ����õ�short�͸�ʽ�������unsigned short �ͻ�������
    freeIDList.clear();
    for(int i = 1; i <= 32767; i++)
    {
        freeIDList.append(i);
    }
    mtxFreeIDList.unlock();
    return;
}
/**
* ���ܣ����������ID�ţ�
* @return ID��: 0, ��ʾ��ʱ�޿���ID��, >=1 ��ʾ��Ч��ID��
*/
unsigned short Manager::createID()
{
    US aid = 0;
    TMutexLocker locker(&mtxFreeIDList);
    if(freeIDList.size() > 0)
    {
        aid = freeIDList.first();
        freeIDList.removeFirst();
        //�����������1
        ++counter;
    }
    return aid; 
}

/**
* ���ܣ��ͷ�ָ����AID��������ӵ�����AID����
* @return 
*/
void Manager::freeAid(US aid)
{
    if((aid > 1) && (aid < 32767))
    {
        TMutexLocker locker(&mtxFreeIDList);
        freeIDList.append(aid);
        //�������������1
        --counter;
    }
    return;
}

/**
* ���ܣ�����TClient_changeChannelType����,����ͨ���л�
* @param aid sa��aid��
* @param channelType Ҫ�л���ͨ������
* @param srcAid Ҫ���ص�aid��
* @return true�л��ɹ�,false��ʾ�л����ɹ�
*/
int Manager::changeChannel(short aid, int channelType, short srcAid) 
{
    QMap<short,AgentBase*>::const_iterator m_CitrSA = mapAID2SA.find(aid);
    if (m_CitrSA == mapAID2SA.end() ) 
    {    //û���ҵ�
        sysLoger(LWARNING, "Main::changeChannel: didn't find the agent by aid [%d]. ", aid);
        return TS_SERVICE_NOT_EXIST;
    }
    
    if (m_CitrSA.value()->getType() == AgentBase::CLIENT_AGENT)
    {
        sysLoger(LWARNING, "Main::changeChannel: Client Agent can't change channel!");
        return TS_INVALID_SERVICE_ID;
    }

    AgentWithQueue* pSa = dynamic_cast<AgentWithQueue*>(m_CitrSA.value());
    if (pSa == NULL)
    {
        sysLoger(LERROR, "Main::changeChannel: dynamic cast pSa from AgentBase to "\
                                        "AgentWithQueue failure!");
        return TS_SYSTEM_ERROR;
    }
    
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [begin]*/
    /*  Modified brief: ȥ��ͨ�����ͼ��*/
    #if 0
    if (pSa->getChannelType() == channelType)
    {
        sysLoger(LINFO, "Main::changeChannel: server Agent already is the channel Type %d",
                                    channelType);
        return TS_SUCCESS;
    }
    #endif
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [end]*/

    if (channelType >= pSa->getChannelCount())
    {
        sysLoger(LWARNING, "Main::changeChannel: invalid channel Type [%d], larger than channel"\
                                            " count [%d]. ", channelType, pSa->getChannelCount());
        return TS_INVALID_CH_ID;        
    }
    //���ͨ���л������ѱ�����
    if(pSa->getChannelSwitchState() == false)
    {
        //����������SAͨ���л����ܵ�CAû�����پͲ�����ͨ���л�
        if(getServerAgentFromID(pSa->getDisableChannelSwitchCAaid()) != NULL)
        {
            sysLoger(LINFO, "Main::changeChannel: ServerAgent's channel switching function had been"\
                                       " disabled by aid %d", pSa->getDisableChannelSwitchCAaid());
            //������Լ�������
            if(srcAid == pSa->getDisableChannelSwitchCAaid())
            {
                return TS_SRV_CHANNEL_SWITCH_DISABLE;
            }
            return TS_CH_SW_FUNC_LOCKED_BY_OHTER;
        }
        //������ǿͻ����˳�ʱ���ǽ���,��Ҫϵͳ����
        pSa->setChannelSwitchState(true, MANAGER_AID);
    }
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [begin]*/
    /*  Modified brief: ����ͨ���л�����֮ǰ,�ȼ��ͨ���Ƿ�����,
    ����ͬ����ͨ������������*/
    if (pSa->getChannelType() != channelType)
    {
        ServerConfig *pConfig = new ServerConfig();
        if((NULL == pConfig) || !getNewConfig(pConfig, channelType, aid))
        {
            changeChannelState = 0;     //û���л�
            return TS_UNKNOW_ERROR;
        }
        DeviceBase* pDevice = DeviceManage::getInstance()->createDevice(
                    pConfig->getDeviceConfig()->getDeviceType());
        if(NULL == pDevice)
        {
            delete pConfig;
            pConfig = NULL;
            changeChannelState = 0;     //û���л�
            return TS_UNKNOW_ERROR;
        }

        //����ͨ���Ƿ����
        if (pDevice->open(pConfig)) 
        {
            pDevice->close();
            delete pDevice;
            pDevice = NULL;        
            delete pConfig;
            pConfig = NULL;
        }
        else
        {
            delete  pDevice;
            pDevice = NULL;
            delete pConfig;
            pConfig = NULL;
            sysLoger(LWARNING, "Manager::changeChannel: Open device failure!");
            
            return TS_DEVICE_CANNOT_USE;
        }    
    }    
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [end]*/
    //��ta����һ��������������ͨ���л�
    Packet pack;
    pack.setSrc_aid(0);
    pack.setDes_aid(pSa->getAID());
    pack.setSrc_said(0);
    pack.setDes_said(0);
    pack.setSize(2);
    pack.clearData();
    pSa->setAckAid(srcAid);
    pSa->setChangeChannelState(1);
    pSa->setChannelType(channelType);
    sprintf(pack.getData(), "c%x", channelType);
    pSa->sendPacket(&pack);    //��Ŀ�������
    return TS_SUCCESS;
}

/**
 * @Funcname:  insertFileToMap
 * @brief        :  ���ļ�����Ӧ���ļ������������mapName2File��
 * @param     : fn,�ļ���;  fp�ļ����
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��3��2�� 17:38:37
 *   
**/
bool Manager::insertFileToMap(string fn, FILE *fp)
{
    if((NULL == fp) || (fn.empty()))
    {
        return false;
    }
    QMap<string, FILE*>::iterator itrFile = mapName2File.find(fn);
    if(itrFile != mapName2File.end())
    {
        itrFile.value() = fp;
    }
    else
    {
        mapName2File.insert(fn, fp);
    }
    return true;
}

/**
 * @Funcname:  getFileFromName
 * @brief        :  �����ļ�����mapName2File�л�ȡ�ļ����
 * @param     : fn,�ļ���; ���� fp�ļ����
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��3��2�� 17:38:37
 *   
**/
FILE* Manager::getFileFromName(string fn)
{
    if(fn.empty())
    {
        return NULL;
    }
    QMap<string, FILE*>::iterator itrFile = mapName2File.find(fn);
    if(itrFile != mapName2File.end())
    {
        return itrFile.value();
    }
    else
    {
        return NULL;
    }
}
/**
 * @Funcname:  removeAndCloseFileFromMap
 * @brief        :  �����ļ����ӱ�mapName2File���Ƴ��ļ�������ر��ļ�
 * @param     : fn,�ļ���
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��3��2�� 17:38:37
 *   
**/
bool Manager::removeAndCloseFileFromMap(string fn)
{
    if(fn.empty())
    {
        return false;
    }
    QMap<string, FILE*>::iterator itrFile = mapName2File.find(fn);
    if(itrFile != mapName2File.end())
    {
        FILE *fp = itrFile.value();
        fclose(fp);
        fp = NULL;
        mapName2File.remove(itrFile.key());
        return true;
    }
    else
    {
        return false;
    }
}

/*Modified by tangxp on 2007.11.8 [begin]*/
/* Modifide brief:��TS�м���ػ��ж��ź�*/
/*
 * @Funcname:  tsExit
 * @brief        :ʹmanager�˳�
 * @param     : ctrlType,�ػ��ź�
 * @return      : 
*/
BOOL WINAPI tsExit(DWORD ctrlType)
{
    switch(ctrlType)
    {
        case CTRL_LOGOFF_EVENT:
            Manager::getInstance()->stop();
            break;
    }

    return FALSE;
} 
/*Modified by tangxp on 2007.11.8 [end]*/

int main(int , char *)
{
    /*Modified by tangxp on 2007.11.8 [begin]*/
    /* Modifide brief:��TS�м���ػ��ж��ź�*/
    //��ȡ����̨���
    HWND hWnd = GetConsoleWindow();

    //���ؿ���̨����
    ShowWindow(hWnd,SW_HIDE);

    //ע���˳�����
    SetConsoleCtrlHandler(tsExit, TRUE);
    /*Modified by tangxp on 2007.11.8 [end]*/

    /*Modified by tangxp for BUG NO.0002555 on 2008.1.22 [begin]*/
    /*  Modified breif: ��ϵͳ��־*/
    SystemLog::getInstance()->openLogFile(string("system.log"));

    Manager::getInstance()->main();
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.22 [end]*/
    return 1;

}
