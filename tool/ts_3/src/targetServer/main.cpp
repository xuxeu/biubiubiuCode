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
* TS的主线程
*
* @file     main.cpp
* @brief     TS的主线程
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
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

//TS配置文件名称
/*Modified by tangxp for BUG NO.0002714 on 2008.3.3 [begin]*/
/*  Modified brief: 增加ts配置模板文件*/
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
    //默认的端口号
    listenPort = socketlistenPort;
    //初始化空闲ID列表
    initFreeIDList();
    //初始化映射表
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

    //释放agentArray
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
* 功能：把二级代理插入
* @param pSubAgent 把SubAgent对象插入map对象mapSAID2SubAgent中
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
* 功能：把serverAgent插入
* @param pServerAgent 把sa对象插入map对象mapAID2SA中
* @param regType 注册类型
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
        {    //没有找到则注册
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
* 功能：处理TClient_setRegisterType函数,设置sa的注册类型
* @param aid sa的aid号
* @param regType 要注册的类型号
* @return true表示注册成功,false表示已注册了此aid
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
               
        //向对象注册类型
        pSa->setRegisterType(regType);
        sysLoger(LINFO, "Main::setRegisterType: set SA [aid:%d] to Regist type %d successful!", 
                                    aid, regType);
        //进行该SA 的上报
        sendSAReportToCA(pSa);  
        if ((isStore) && (regType != INVALID_REG_TYPE))
            storeConfigure();
        return true;
    }
    sysLoger(LINFO, "Main:setRegisterType: set SA [aid:%d] to RegType %d failure!", aid, regType);
    return false;
}
/**
* 功能：处理TClient_clearRegisterType函数,清楚sa的注册类型
* @param aid sa的aid号
* @param regType 要清楚的类型号,w为-1表示删除手有的对象
* @return true表示注册成功,false表示已注册了此aid
*/
bool Manager::clearRegisterType(short aid) 
{
    AgentWithQueue *pAgentQ = dynamic_cast<AgentWithQueue*>(getServerAgentFromID(aid));
    if(NULL == pAgentQ)
    {
        sysLoger(LWARNING, "Main::clearRegisterType: Didn't find the Agent object by aid :%d .", aid);
        return false;
    }
    //上报
    sendSAReportToCA(pAgentQ);    //上报状态
    pAgentQ->setRegisterType(INVALID_REG_TYPE);
        
    storeConfigure();
    sysLoger(LINFO, "Main::ClearRegisterType: SA unregistered the type :%d successful.", regType);
    return true;
}

bool Manager::mfCreateDirectory( QString &strDirectory )
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());    //识别中文    
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
* 功能：退出manager
*/
void Manager::stop() 
{
    runFlag = false;
    wakeUp();
}

void Manager::main()
{
    //有个线程循环 
    //向router注册manager对象
    char p[256];
    GetCurrentDirectory(255,p);    //读入当前路径
    appDir =  QString::fromLocal8Bit(p);
    createDirectory(const_cast<QString &>(appDir + "\\ts\\log"));
    createDirectory(const_cast<QString &>(appDir + QString::fromStdString(tmpfilepath)));
    runFlag = true;
    //向router 注册自身
    Router::getInstance()->registerAgent(getAID(),this);
    //加载配置文件
    loadConfigure();
    Packet* pack =  NULL;
    //处理接收的包
    QMap<short, SubAgentBase*>::const_iterator iSubA;

    while (runFlag) 
    {
        while(true) 
        {
            pack = getpkt();    //取包
            if (pack == NULL)
                break;
            //加一个防止消息循环的保护
            if(pack->getSrc_aid() == MANAGER_AID)
            {
                sysLoger(LWARNING, "Main::main(): Received packet from itself, dead loop! FILE:%s, LINE:%d",
                                                    __MYFILE__, __LINE__);
                //释放分配的包
                PacketManager::getInstance()->free(pack);
                continue;
            }
            iSubA = mapSAID2SubAgent.find(pack->getDes_said());
            if (iSubA != mapSAID2SubAgent.end()) 
            {    //找到了
                iSubA.value()->process(pack); 
            }
            else 
            {    //出问题进行日志记录
                sysLoger(LWARNING, "Main::main(): subAgent with des_said %d did not exit!", 
                                                                        pack->getDes_said());
                
                //释放分配的包
                PacketManager::getInstance()->free(pack);
            }

        }
        if (runFlag)
            waitUp();
    }  
    storeConfigure();    //保存配置信息
    deleteAllServer();    //删除manager保存的所有服务    
    pListen->close();    
    /*Modified by tangxp for BUG NO.0002547 on 2008.1.10 [begin]*/
    /*  Modifide brief: 增加删除缓冲包管理对象*/
    delete PacketManager::getInstance();
    /*Modified by tangxp for BUG NO.0002547 on 2008.1.10 [end]*/
}


/**
* 功能：删除指定server
* @param aid serverAgent的aid号
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
            //如果删除的是CA，
            if(pAgent->getType() == AgentBase::CLIENT_AGENT)
            {
                //删除CA注册列表中的对象
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
                //删除CA对象
                QMap<short, AgentBase* >::iterator i = mapAID2SA.find(aid);
                mapAID2SA.remove(i.key());
                /*Modified by tangxp for BUG NO. 0002551 on 2008.1.15 [begin]*/
                /*  Modified brief: 删除CA对象时,需先停止CA运行*/
                pAgent->stop();
                /*Modified by tangxp for BUG NO. 0002551 on 2008.1.15 [end]*/
                delete pAgent;
                pAgent = NULL;
                //释放aid
                freeAid(aid);
                return TS_SUCCESS;
            }
            //如果是SA对象
            AgentWithQueue* pServerAgent = dynamic_cast<AgentWithQueue*>(pAgent);
            
            if (pServerAgent != NULL) 
            {
                //强制转换成sa对象
                switch (pServerAgent->getType()) 
                {               
                    case AgentWithQueue::SERVER_AGENT: 
                        {
                            ServerAgent* pSa = dynamic_cast<ServerAgent*>(pServerAgent);                        
                            pSa->stop();
                            //上报删除对象
                            pServerAgent->setState(SASTATE_DELETED);
                            sendSAReportToCA(pServerAgent);
                            TMutexLocker locker(&mtxMapName2ID);
                            mapName2ID.remove(pSa->getName());    //删除名字到ID的映射
                        }
                        break;
                        
                    case AgentBase::SERVER_AGENT_WITH_DLL:
                        {
                            ServerAgentWithDll* pSa = dynamic_cast<ServerAgentWithDll*>(pServerAgent);
                            pSa->stop();
                            //上报删除对象
                            pServerAgent->setState(SASTATE_DELETED);
                            sendSAReportToCA(pServerAgent);
                            TMutexLocker locker(&mtxMapName2ID);
                            mapName2ID.remove(pSa->getName());    //删除名字到ID的映射
                        }
                        break;
                        
                    default:
                        //即不是CA也不是SA，则可能发生了系统错误
                        sysLoger(LWARNING, "Main::deleteServer: agent is unknow type!");
                        return TS_SYSTEM_ERROR;
                }        

                Router::getInstance()->unRegisterAgent(pServerAgent->getAID());    //删除到router的注册
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
* 功能：删除所有server
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
            //删除注册类型数组登记
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
            //删除所有代理时不删除CA,否则发送删除命令的客户端无法收到回复
            continue;
        }
        pAgent = dynamic_cast<AgentWithQueue*>(itrSA.value());
        switch(pAgent->getType()) 
        {
            case AgentWithQueue::SERVER_AGENT:
                {
                    ServerAgent* pServerAgent = dynamic_cast<ServerAgent*>(pAgent);
                    pServerAgent->stop();        //停止server的线程
                    //上报删除状态
                    pServerAgent->setState(SASTATE_DELETED);
                    sendSAReportToCA(pServerAgent);
                    mapName2ID.remove(pServerAgent->getName());    //删除名字到ID的映射
                }
                break;
            case AgentBase::SERVER_AGENT_WITH_DLL:
                {
                    ServerAgentWithDll* pServerAgent = dynamic_cast<ServerAgentWithDll*>(pAgent);
                    pServerAgent->stop();        //停止server的线程
                    //上报删除状态
                    pServerAgent->setState(SASTATE_DELETED);
                    sendSAReportToCA(pServerAgent);
                    mapName2ID.remove(pServerAgent->getName());    //删除名字到ID的映射
                }
                break;
            default:
                sysLoger(LWARNING, "Main::deleteAllServer: agent %d is not server Agent!", 
                                                        pAgent->getAID());
                continue;
                break;
        }
        //取消在Router模块的注册信息
        Router::getInstance()->unRegisterAgent(pAgent->getAID());    //删除到router的注册
        shortList.append(itrSA.key());
        delete pAgent;
        pAgent = NULL;
    }
    
    TMutexLocker lock(&mtxMapAID2SA);
    for(int j=0; j<shortList.size();++j) 
    {
        mapAID2SA.remove(shortList[j]);
        /*Modified by tangxp for BUG NO.0002545 on 2008.1.17 [begin]*/
        /*  Modified brief: 更改删除所有代理时,不释放CA的AID*/
        freeAid(shortList[j]);
        /*Modified by tangxp for BUG NO.0002545 on 2008.1.17 [end]*/
    }
  
    return;
}

/**
* 功能：当读配置文件成功后加载配置启动对应ServerAgent和监听线程ListenerThread
* @return
*
*/
void Manager::loadConfigure()
{
    //读取TS.ini文件信息
    // fn = filename;
    //启动监听线程
    if (pListen != NULL)
        delete pListen;
    pListen = new Listen(listenPort);    //监听端口
    pListen->start();

    /*Modified by tangxp for BUG NO.0002714 on 2008.3.3 [begin]*/
    /*  Modified brief: 若打开配置文件失败,则使用模板恢复,再重新打开*/
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

        //拷贝模板配置文件
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
    //处理从文件中读取中文的问题
    QString buf = QString::fromLocal8Bit(block.data(),block.size());
    QXmlInputSource in;//(&f);
    in.setData(buf);
    QXmlSimpleReader reader;
    // don't click on these!
    reader.setFeature("http://xml.org/sax/features/namespaces", false);
    reader.setFeature("http://xml.org/sax/features/namespace-prefixes", true);
    reader.setFeature("http://trolltech.com/xml/features/report-whitespace"
        "-only-CharData", false);
    XmlHandler *hand = new XmlHandler();    //关联对象
    reader.setContentHandler(hand);
    reader.setErrorHandler(hand);

    reader.parse(in);    //读取文件内容
    reader.setContentHandler(0);
    reader.setErrorHandler(0);
    delete hand;
    f.close();
}

/**
* 功能：处理TClient_register函数,TS进行注册,注册后,TS会向TC自动发送消息和状态
* @param pack 接收的控制包
* @return true表示注册成功,false表示已注册了此aid
*/
int Manager::registerServer(Packet* pack,int regType)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "Main::registerServer : Parameter pack is null pointer!");
        return TS_NULLPTR_ENCONTERED;
    }
    //查找aid号, 并找出该AID对象的CA对象，看是否已经在agentarray
    //里注册
    if(regType < 0 || regType > MAX_REG_TYPE)
    {
        sysLoger(LWARNING, "Main::registerServer : Invalid regType %d", regType);
        return TS_INVALID_REG_TYPE;
    }
    AgentBase*pCa = getServerAgentFromID(pack->getSrc_aid());
    /*Modified by tangxp for BOG NO.0002814 on 2008.3.3 [begin]*/
    /*  Modified brief: 检查注册对象是否是CA*/
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
        //分配链表
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
        //还未在agentArray里注册该CA
        agentArray[regType]->append(pCa);        
    }
    sysLoger(LINFO, "Main::registerServer: Registered for RegType %d Successfully!", regType);
    return TS_SUCCESS;
}
/**
* 功能：处理TClient_unregister函数,注册CA对象的类型注册
* @param pack 接收的控制包
* @return true表示注册成功,false表示已注册了此aid
*/
int Manager::unRegisterServer(Packet* pack,int regType) 
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "Main::unRegisterServer : Parameter pack is null pointer!");
        return TS_NULLPTR_ENCONTERED;
    }
    //查找aid号
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
        //已在agentArray里注册该CA
        agentArray[regType]->removeAt(agentArray[regType]->indexOf(pCa));        
    }
    sysLoger(LINFO, "Main::unRegisterServer:Unregistered ClientAgent %d from this type %d successfully", 
                                    pCa->getAID(), regType);
    mtxAgentArray.unlock();
    return TS_SUCCESS;
}

/**
 * @Funcname:  sendSAReportToCA
 * @brief        :  发送SA的上报信息给注册该类型的CA
 * @para1      : AgentWithQueue * pSA 待上报信息的SA对象
 * @para2      : caAid 需要发送消息的指定CAaid, 如果为0，
 *                       则向所有注册了该类型的CA都发送
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年1月8日 11:26:18
 *   
**/
void Manager::sendSAReportToCA(AgentWithQueue * pSA, US caAid)
{
    if(NULL == pSA)
    {
        sysLoger(LWARNING, "Main::sendSAReportToCA: Parameter pSA is NULL pointer!");
        return;
    }
    //构造SA的上报数据，发送给所有的CA
    char *pBuf = NULL;
    size_t tmpLen = 0;
    comConfig *pcomCfg = NULL;
    char extInfo[EXT_INFO_LEN] = "\0";
    //如果注册类型无效则不进行上报
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
    pack->setDes_aid(0);     //暂时设置为0,发送的时候设为CAaid
    pack->setSrc_said(0);
    pack->setDes_said(0);
    pBuf = pack->getData();
    pack->clearData();
    /**
    上报报文格式：
    ----------------------------------------------------------------------------
    |aid,|name,|extInfo|,|reportType,|sate,|islog,|channelCount,|currentChannel,|config of each channel,|
    ----------------------------------------------------------------------------
    */
    sysLoger(LDEBUG, "Main::sendSAReportToCA:Send SA's [%s] config to CA %d", 
                                    pSA->getName().toAscii().data(), caAid);
    //DID,先占位，后面发包的时候再根据具体的CA设置
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
            //直接将每个通道配置字符串拷贝
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
        //发送给所有注册了该类型的CA
        /*Modified by tangxp for BUG NO.0002814 on 2008.3.3 [begin]*/
        /*  Modified brief: 增加注册类型到CA对象列表访问互斥*/
        TMutexLocker locker(&mtxAgentArray);
        /*Modified by tangxp for BUG NO.0002814 on 2008.3.3 [end]*/
        QList<AgentBase*>::iterator m_ItrCA = agentArray[pSA->getRegisterType()]->begin();
        for(; m_ItrCA != agentArray[pSA->getRegisterType()]->end(); m_ItrCA++)
        {
            //设置包的目的地为注册了该类型的CA
            pack->setDes_aid((*m_ItrCA)->getAID());
            //根据具体的CA设置DID
            __store(CURDID((*m_ItrCA)->getAID()), pack->getData(), DIDLEN);
            //进行数据日志
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
            //发包
            Router::getInstance()->putpkt(pack, false);
        }
    }
    else
    {
        //发送给指定的CA
        pack->setDes_aid(caAid);
        //根据CA设置DID
        __store(CURDID(caAid), pack->getData(), DIDLEN);
        //进行数据日志
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
        //发包
        Router::getInstance()->putpkt(pack, false);
    }
    //free the pack
    PacketManager::getInstance()->free(pack);
    return;
}
/**
*功能：进行某种类型SA的上报
* regType: 需要上报的注册类型号
* caAid： 上报信息目的地，0表示所有的CA都发送
*/
void Manager::sendRegTypeSAToCA(int regType, US caAid)
{
    //查询mapAid2SA映射表，找出所有注册了该类型的SA,
    //进行上报
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
        //是该类型，并且是目标机代理服务,则进行上报
        if((pAgentQ!= NULL) && (pAgentQ->getRegisterType() == regType))
        {
            sendSAReportToCA(pAgentQ, caAid);
        }
        cItrAgent++;
    }
    return;
}

/**
* 功能：将配置信息保存到文件中去
* @return 成功返回true，失败返回false
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
        //端口号
        t << "    <ListenConfig";
        t <<  " port=\"" << listenPort << "\">\n";
        t << "    </ListenConfig>\n";
        //系统日志输出级别
        t << "    <SystemLogConfig";
        t << " currentLevel=\"" << SystemLog::getInstance()->getLogLevel()<< "\">\n";
        t << "    </SystemLogConfig>\n";

        AgentWithQueue* pAgent = NULL;
        QMap<short,AgentBase*>::const_iterator m_CitrSA = mapAID2SA.constBegin();
        //创建TS.xml文件格式
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
            //遍历此sa对象所有的类型        
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
                            //名字
                            t << " name=\"" << pServerAgent->getName() << "\" ";
                            //上报类型
                            QString str ;                            
                            t << " reportType=\"" << reportType << "\"";
                            //状态
                            str = QString("%1").arg(pServerAgent->getState());
                            t << " state=\"" << str << "\"";
                            //是否日志记录
                            str = QString("%1").arg(pServerAgent->getLogFlag());
                            t << " logFlag=\"" << str << "\"";
                            //当前通道号
                            str = QString("%1").arg(pServerAgent->getChannelType());
                            t << " channelType=\"" << str << "\"";
                            //扩展信息
                            t << " extInfo=\"" << pServerAgent->getExtInfo() << "\">\n";
                            //保存通道配置信息
                            QVector<QString> channelInfo = pServerAgent->getChannelInfo();
                            for (int i=0; i<pServerAgent->getChannelCount();++i) 
                            {
                                QVector<QString> vec;
                                //QString buf(block) ;
                                //中文转换,转换成UTF8
                                QString strCh;
                                QString strProtocol;
                                int next = 0;    //查询的开始位置
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
                                strProtocol = vec[iCount++];    //得到协议类型
                                t << " way=\"" << vec[iCount] << "\"";
                                //设备类型
                                switch(vec[iCount++].toInt()) 
                                {
                                    case DeviceConfigBase::TCP_DEVICE:    //TCP
                                        t << " IP=\"" << vec[iCount++] << "\"";                                                //端口

                                        t << " port=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                                //IP
                                        break;
                                    case DeviceConfigBase::UDP_DEVICE:    //UDP
                                        t << " IP=\"" << vec[iCount++] << "\"";                                            //端口

                                        t << " port=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                            //IP
                                        t << " timeout=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                    //超时
                                        t << " retry=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                    //重试次数
                                        break;
                                    case DeviceConfigBase::UART_DEVICE:    //串口
                                        t << " com=\"" << vec[iCount++] << "\"";                            //串口号

                                        t << " baud=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                        //波特率
                                        t << " timeout=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                    //超时
                                        t << " retry=\"" << vec[iCount++].toInt(&ok, 16) << "\"";                    //重试次数
                                        break;
                                        
                                    case DeviceConfigBase::USB_DEVICE:
                                        t << " GUID=\"" <<  vec[iCount] << "\""; 
                                        break;
                                        
                                    default:
                                        sysLoger(LWARNING, "Main::storeConfigure: unknow deviceType [%d] of SA");
                                        break;
                                }

                                t << " protocolType=\"" << strProtocol << "\">\n";                                        //协议类型
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
                            /*  brief: 保存常驻模块配置信息*/
                            if( PSA_REG_TYPE == pServerAgent->getRegisterType() )
                            { 
                                //常驻模块
                                t << "    <permanentServerAgent>\n";
                                t << "        <psa";
                                t << " name=\"" << pServerAgent->getName() << "\"";             //动态库名字

                                t << " dllName=\"" << pServerAgent->getDllName() << "\"";       //动态库文件名

                                QString str = QString("%1").arg(pServerAgent->getLogFlag());    
                                t << " logFlag=\"" << str << "\"";                              //日志标记

                                t << " extInfo=\"" << pServerAgent->getInBuf() << "\">\n";      //动态库初始化参数
                                t << "        </psa>\n";
                                t << "    </permanentServerAgent>\n";                        
                            }
                            else
                            {
                                //普通动态库
                                t << "    <serverAgentWithDll";
                                t << " name=\"" << pServerAgent->getName() << "\" ";                            //名字
                                
                                t << " reportType=\"" << reportType << "\"";                                         //注册类型
                                
                                t << " dllName=\"" << pServerAgent->getDllName() << "\"";                        //动态库名字                        //状态

                                QString str = QString("%1").arg(pServerAgent->getState());                                //状态
                                t << " state=\"" << str << "\"";                                                //状态

                                str = QString("%1").arg(pServerAgent->getLogFlag());                            //是否日志记录
                                t << " logFlag=\"" << str << "\"\n";                                            //是否日志记录
                                //扩展信息
                                t << " extInfo=\"" << pServerAgent->getExtInfo() << "\"";

                                t << "        dllBuf=\"" << pServerAgent->getInBuf() << "\">\n";                    //状态
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
* 功能: 通过名字得到对应的ID号
* @param name 名字
* return ID 号-1表示没有
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
* 功能: 通过AID号得到serverAgent对象
* @param aid aid号
* return ServerAgent* NULL表示没有此aid号,
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
* 功能：修改映射mapAID2SA
* @param oldName 要替换的原来名字
* @param newName 替换后的名字
*/
void Manager::modifyName2AIDMap(QString oldName,QString newName) 
{
    QMap<QString ,short>::const_iterator cItrName = mapName2ID.find(oldName);
    if (cItrName != mapName2ID.end()) 
    {    //找到需要修改的名字，则进行修改
        int aid = cItrName.value();
        //修改对象里保存的名字
        AgentWithQueue *pAgent = dynamic_cast<AgentWithQueue*>(getServerAgentFromID(aid));
        if(NULL == pAgent)
        {
            sysLoger(LERROR, "Main::modifyName2AIDMap: dynamic cast pointer of server agent from"\
                                           "AgentBase to AgentWithQueue failure, modified name from %s to %s failure!",
                                            oldName.toAscii().data(), newName.toAscii().data());
            return;
        }
        //修改对象里保存的名字
        pAgent->setName(newName);
        //修改mapName2ID映射表里的名字
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
 * @brief        :  初始化空闲ID列表
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年1月8日 10:15:08
 *   
**/
void Manager::initFreeIDList()
{
    mtxFreeIDList.lock();
    //0被Manager占用, ID只用到short型的正整数部分, 
    //因为数据包等都是用的short型格式，如果用unsigned short 型会造成溢出
    freeIDList.clear();
    for(int i = 1; i <= 32767; i++)
    {
        freeIDList.append(i);
    }
    mtxFreeIDList.unlock();
    return;
}
/**
* 功能：给对象分配ID号，
* @return ID号: 0, 表示暂时无可用ID号, >=1 表示有效的ID号
*/
unsigned short Manager::createID()
{
    US aid = 0;
    TMutexLocker locker(&mtxFreeIDList);
    if(freeIDList.size() > 0)
    {
        aid = freeIDList.first();
        freeIDList.removeFirst();
        //对象计数器加1
        ++counter;
    }
    return aid; 
}

/**
* 功能：释放指定的AID，将其添加到空闲AID链表
* @return 
*/
void Manager::freeAid(US aid)
{
    if((aid > 1) && (aid < 32767))
    {
        TMutexLocker locker(&mtxFreeIDList);
        freeIDList.append(aid);
        //将对象计数器减1
        --counter;
    }
    return;
}

/**
* 功能：处理TClient_changeChannelType函数,处理通道切换
* @param aid sa的aid号
* @param channelType 要切换的通道类型
* @param srcAid 要返回的aid号
* @return true切换成功,false表示切换不成功
*/
int Manager::changeChannel(short aid, int channelType, short srcAid) 
{
    QMap<short,AgentBase*>::const_iterator m_CitrSA = mapAID2SA.find(aid);
    if (m_CitrSA == mapAID2SA.end() ) 
    {    //没有找到
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
    /*  Modified brief: 去掉通道类型检查*/
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
    //如果通道切换功能已被禁用
    if(pSa->getChannelSwitchState() == false)
    {
        //并且锁定该SA通道切换功能的CA没有销毁就不允许通道切换
        if(getServerAgentFromID(pSa->getDisableChannelSwitchCAaid()) != NULL)
        {
            sysLoger(LINFO, "Main::changeChannel: ServerAgent's channel switching function had been"\
                                       " disabled by aid %d", pSa->getDisableChannelSwitchCAaid());
            //如果是自己锁定的
            if(srcAid == pSa->getDisableChannelSwitchCAaid())
            {
                return TS_SRV_CHANNEL_SWITCH_DISABLE;
            }
            return TS_CH_SW_FUNC_LOCKED_BY_OHTER;
        }
        //否则就是客户端退出时忘记解锁,需要系统解锁
        pSa->setChannelSwitchState(true, MANAGER_AID);
    }
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [begin]*/
    /*  Modified brief: 发送通道切换命令之前,先检查通道是否能用,
    若是同样的通道类型则不需检查*/
    if (pSa->getChannelType() != channelType)
    {
        ServerConfig *pConfig = new ServerConfig();
        if((NULL == pConfig) || !getNewConfig(pConfig, channelType, aid))
        {
            changeChannelState = 0;     //没有切换
            return TS_UNKNOW_ERROR;
        }
        DeviceBase* pDevice = DeviceManage::getInstance()->createDevice(
                    pConfig->getDeviceConfig()->getDeviceType());
        if(NULL == pDevice)
        {
            delete pConfig;
            pConfig = NULL;
            changeChannelState = 0;     //没有切换
            return TS_UNKNOW_ERROR;
        }

        //测试通道是否可用
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
    //向ta发送一个包用来表明是通道切换
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
    pSa->sendPacket(&pack);    //向目标机发送
    return TS_SUCCESS;
}

/**
 * @Funcname:  insertFileToMap
 * @brief        :  将文件名对应的文件句柄保存至表mapName2File中
 * @param     : fn,文件名;  fp文件句柄
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年3月2日 17:38:37
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
 * @brief        :  根据文件名从mapName2File中获取文件句柄
 * @param     : fn,文件名; 返回 fp文件句柄
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年3月2日 17:38:37
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
 * @brief        :  根据文件名从表mapName2File中移出文件句柄并关闭文件
 * @param     : fn,文件名
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年3月2日 17:38:37
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
/* Modifide brief:在TS中加入关机中断信号*/
/*
 * @Funcname:  tsExit
 * @brief        :使manager退出
 * @param     : ctrlType,关机信号
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
    /* Modifide brief:在TS中加入关机中断信号*/
    //获取控制台句柄
    HWND hWnd = GetConsoleWindow();

    //隐藏控制台窗口
    ShowWindow(hWnd,SW_HIDE);

    //注册退出函数
    SetConsoleCtrlHandler(tsExit, TRUE);
    /*Modified by tangxp on 2007.11.8 [end]*/

    /*Modified by tangxp for BUG NO.0002555 on 2008.1.22 [begin]*/
    /*  Modified breif: 打开系统日志*/
    SystemLog::getInstance()->openLogFile(string("system.log"));

    Manager::getInstance()->main();
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.22 [end]*/
    return 1;

}
