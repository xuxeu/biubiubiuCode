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
* @file     ServerAgent.cpp
* @brief     server代理的实现
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/



#ifndef SERVERAGENT_INCLUDE
#define SERVERAGENT_INCLUDE


#include "tsInclude.h"


#include "Agent.h"
class DeviceBase;
class Packet;
class ProtocolBase;
class ServerAgent;
class ServerConfig;

class ServerAgent : public AgentWithQueue 
{
private:
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.26 [begin]*/
    /*  Modified brief: 增加发送互斥*/
    TMutex sendMutex;
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.26 [end]*/
public:
    ServerAgent (unsigned short aid);
    virtual ~ServerAgent();     
    /**
    * 功能：向router注册,运行server接收和发送线程
    * @return true表示成功
    */
    bool run();
    /**
    * 功能：停止server线程
    */
    void stop();
    /**
    * 功能：关闭sa对应资源
    */
    void close() ;
    /**
    * 功能：将配置信息序列化到OwnerArchive对象
    */
    void serialize(OwnerArchive& ar) ;
    /**
    * 功能：发送数据
    */
    int sendPacket(Packet* pack);
    /**
    * 功能：发送数据
    * @return 接收包的大小
    */
    int receivePacket(Packet* pack);
    /**
    * 功能：打开设备和初始化协议
    * @return true表示成功
    */
    bool initDevice();
};


#endif








