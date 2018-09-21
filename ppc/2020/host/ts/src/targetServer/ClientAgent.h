/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  ClientAgent.h
* @brief
*       功能：
*       <li>处理与客户端通信</li>
*/


#ifndef CLIENTAGENT_INCLUDE
#define CLIENTAGENT_INCLUDE

/************************头文件********************************/
#include "tsInclude.h"
#include "Agent.h"

/************************宏定义********************************/
/************************类型定义******************************/

class PackLog;
class Packet;
class ProtocolBase;

/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [begin]*/
/*  Modifed brief:用单独写的线程类替换QT线程类*/
class ClientAgent : public baseThread, public AgentBase
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [end]*/
{
private:
    enum ClientAgentStatus
    {
        Initialized = 0,
        Started,
        Finished
    };
    TMutex mutex;                //对发送和读取数据进行同步
    int socketDescriptor;        //对应的连接socket号   //未使用
    ProtocolBase* pProtocol;    //对应的通信协议
    bool runFlag ;                //线程运行标志
    void close() ;
    ClientAgentStatus exitState;   //ca线程离开标志,0表示没有离开,1表示退出线程
#ifdef CLIENT_AGENT_DEBUG
    PackLog PacketLogger;    //日志
#endif

public:
    ~ClientAgent();
    ClientAgent(ProtocolBase* pProtocol, short aid);

    /**
    * 功能：将数据包发送到工具
    * @param pack
     * @param isRelease是否释放包资源false表示不释放,true表示释放
    */
    void putpkt(Packet* pack, bool isRelease = true);

    /**
    * 功能：置反线程运行标志，用于停止线程
    */
    void stop();
    void run();
};

/************************接口声明******************************/
#endif









