/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  serveragent.h
* @brief
*       功能：
*       <li> server代理的声明</li>
*/




#ifndef SERVERAGENT_INCLUDE
#define SERVERAGENT_INCLUDE

/************************头文件********************************/
#include "tsInclude.h"


#include "Agent.h"

/************************宏定义********************************/

/************************类型定义******************************/
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

/************************接口声明******************************/

#endif








