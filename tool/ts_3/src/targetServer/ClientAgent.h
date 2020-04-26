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
* 
*
* @file     ClientAgent.h
* @brief     处理与客户端通信
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/
#ifndef CLIENTAGENT_INCLUDE
#define CLIENTAGENT_INCLUDE

#include "tsInclude.h"
#include "Agent.h"
//#include "log.h"

//#define CLIENT_AGENT_DEBUG

class PackLog;
class Packet;
class ProtocolBase;
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [begin]*/
/*  Modifed brief:用单独写的线程类替换QT线程类*/
class ClientAgent : public baseThread,public AgentBase 
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [end]*/
{
private:
    TMutex mutex;                //对发送和读取数据进行同步
    int socketDescriptor;        //对应的连接socket号	//未使用
    ProtocolBase* pProtocol;    //对应的通信协议
    bool runFlag ;                //线程运行标志
    void close() ;
    int exitState;                //ca线程离开标志,0表示没有离开,1表示退出线程
#ifdef CLIENT_AGENT_DEBUG
    PackLog PacketLogger;    //日志
#endif

public:
    ~ClientAgent();
    ClientAgent(ProtocolBase* pProtocol,short aid);
    /**
    * 功能：将数据包发送到工具
    * @param pack
     * @param isRelease是否释放包资源false表示不释放,true表示释放
    */
    void putpkt(Packet* pack,bool isRelease = true);
    /**
    * 功能：置反线程运行标志，用于停止线程
    */
    void stop();
    void run();
};


#endif









