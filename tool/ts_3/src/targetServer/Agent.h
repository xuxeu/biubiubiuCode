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
* @file     AgentBase.h
* @brief     抽象层的代理对象
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/

#ifndef AGENT_INCLUDE
#define AGENT_INCLUDE

#include "tsInclude.h"
//#include "Packet.h"
#include "OwnerArchive.h"
//#include "thread.h"



#include <QtCore>

#define SASTATE_ACTIVE          1
#define SASTATE_INACTIVE      0
#define SASTATE_DELETED       2

//class ServerConfig;
#include "config.h"

class RXThread;
class TXThread;
class AgentBase : public QObject
{
private:
    unsigned short aid;                    //每个agent对应的aid号
protected:
    int type;                                //二级代理类型,1表示不是动态库,2表示是动态库，3表示CA类型对象
public:
    AgentBase(unsigned short aid)
    {
        this->aid = aid;
    }
    virtual ~AgentBase();
    /**
    * 功能：得到AID号
    * @return short AID号
    */
    unsigned short getAID() 
    {
        return aid; 
    } 
    virtual void putpkt(Packet* pack,bool isRealse) = 0;
    /**
    * 功能：得到二级代理的类型
    * @return 二级代理类型
    */
    int getType() 
    {
        return type;
    }
    void setType(UC type)
    {
        this->type = type;
        return;
    }
    /**
    * 功能：停止sa
    */
    virtual void stop() = 0;

    //通信方式
    static const int SERVER_AGENT = 1;                //不是动态库
    static const int SERVER_AGENT_WITH_DLL = 2;        //是动态库形式的sa
    static const int CLIENT_AGENT = 3;               //CA类型对象
};

const int TIMEOUT = 5000;
class PackLog;
class ProtocolBase;
class AgentWithQueue : public AgentBase 
{
    QList<Packet*> packetQueue;                //保存要发送到Server数据包缓冲区
    waitCondition queueNotFull;            //条件变量,用来判断队列不为空
    TMutex mutex;                            //对缓冲区进行锁定,
    waitCondition channelCondition;            //条件变量,用来判断队列不为空
    TMutex mutexChannel;                            //对缓冲区进行锁定,
    short ackAid;                            //切换通道ide发送的id号
protected:
    ProtocolBase* pProtocol;                //与Server对应的协议    
    PackLog* pPacketLogger;            //数据包日志服务是跟随SA服务选择创建
    RXThread* pThreadRX;                    //对应的接收线程
    TXThread* pThreadTX;                    //对应的发送线程
    int  state;                                //1是处于Active 0是passive.
    bool isLog;                                //日志记录标志
    QString name;                            //对应server的名字
    ServerConfig* pConfig;                    //配置信息
    int timeout;                            //超时
    int channelType;                        //默认的情况是0表示默认通道类型
    int proChannelType;                        //切换通道之前的类型
    QVector<QString> vectorChannel;            //通道属性 
    QString strData;                        //用户定义的数据
    int changeChannelState;                    //通道切换状态,0表示没有切换,1表示正在切换,2表示切换成功
    QString extInfo;                            //用于存放体系结构等
    UC regType;                             //对象注册类型

    bool canSwitchCh;                          //是否允许通道切换true,表示允许通道切换，false表示禁止
    US   lockChCA;                               //禁止通道切换功能的客户端ID
    
public:

    AgentWithQueue(unsigned short aid);
    ~AgentWithQueue();
    /**
    * 功能：情况通道
    */
    void clearChannel() 
    {
        channelType = 0;
        vectorChannel.clear();
    }
    /**
    * 功能：切换通道
    * @return true表示切换成功,false表示切换失败
    */
    bool changeChannel();
    /**
    * 功能：增加通道配置信息
    * @param : strChCfg  通道的配置信息
    * @return
    */
    void appendChannelConfig(QString strChCfg)
    {
        vectorChannel.append(strChCfg);
    }
    /**
    * 功能：得到通道配置信息数组
    * @return 通道配置信息数组
    */
    QVector<QString>& getChannelInfo() 
    {
        return vectorChannel;
    }

    /**
    * 功能：得到通道数
    * @return 得到通道数
    */
    int getChannelCount() 
    {
        return vectorChannel.size();
    }

    void setExtInfo(QString extInfo)
    {
        this->extInfo = extInfo;
        return;
    }

    QString getExtInfo()
    {
        return extInfo;
    }
    
    /**
    * 功能：设置通道切换使能标记
    * @param flag, 是否允许切换
    * @param aid , 锁定通道切换功能的CA
    * @return 
    */
    void setChannelSwitchState(bool flag, US aid)
    {
        canSwitchCh = flag;
        if(!flag)
        {
            lockChCA = aid;
        }
        else
        {
            lockChCA = 0;          //无效CA的AID，表示通道没有被锁定
        }
    }
    /**
    * 功能：获取禁用通道切换功能的CA aid
    * @return 禁用通道切换功能的CA aid
    */
    US getDisableChannelSwitchCAaid()
    {
        return lockChCA;
    }
    /**
    * 功能：得到通道切换使能标记
    * @return 得到通道切换使能标记
    */
    bool getChannelSwitchState()
    {
        return canSwitchCh;
    }
    /**
    * 功能：注册服务类型
    * @return 
    */
    void setRegisterType(UC regType)
    {
        this->regType = regType;
    }
    /**
    * 功能：得到服务的注册类型
    * @return 服务的注册类型
    */
    UC getRegisterType()
    {
        return regType;
    }
    /**
    * 功能：设置配置信息
    * @param strChannel 通道配置信息
    */
    void addChannel(QString& strChannel);
    /**
    * 功能：设置切换状态信息
    * @param 切换状态信息
    */
    void setChangeChannelState(int changeChannelState) 
    {
        this->changeChannelState = changeChannelState;
    }
    /**
    * 功能：得到切换状态信息
    * @return   切换状态信息
    */
    int getChangeChannelState() 
    {
        return changeChannelState;
    }
    /**
    * 功能：设置发送切换通道命令的源aid
    * @param 发送切换通道命令的源aid
    */
    void setAckAid(short ackAid)
    {
        this->ackAid = ackAid;
    }
    /**
    * 功能：得到发送切换通道命令的源aid
    * @return   发送切换通道命令的源aid
    */
    int getAckAid()
    {
        return ackAid;
    }

    /**
    * 功能：得到协议
    * @return   协议
    */
    ProtocolBase* getProtocol()
    {
        return pProtocol;
    }

    /**
    * 功能：得到协议
    * @return   协议
    */
    void setProtocol(ProtocolBase* pProtocol)
    {
        this->pProtocol = pProtocol;
    }
    /**
    * 功能：设置配置信息
    * @param ServerConfig* 配置信息指针
    */
    void setConfig(ServerConfig* pConfig)
    {
        this->pConfig = pConfig;
    }
    /**
    * 功能：得到配置信息
    * @return ServerConfig*  配置信息指针
    */
    ServerConfig* getConfig() 
    {
        return pConfig;
    }
    /**
    * 功能：释放配置信息
    * @return 
    */
    void clearConfig()
    {
        if(pConfig != NULL)
        {
            delete pConfig;
        }
        return;
    }
    /**
    * 功能：设置名字
    * @param QString 名字
    */
    void setName(const QString& name)
    {
        this->name = name;
    }
    /**
    * 功能：得到名字
    * @return QString 名字
    */
    QString getName()
    {
        return name;
    }
    /**
    * 功能：设置状态
    * @param state 1是处于Active 0是passive
    */
    void setState(int state) ;
    /**
    * 功能：得到状态
    * @return int 状态,1是处于Active 0是passive
    */
    int getState();
    /**
    * 功能：设置是否日志
    * @param isLog true表示日志,flase表示不记录
    */
    void setLogFlag(bool isLog);
    /**
    * 功能：得到日志标志
    * @param isLog true表示日志,flase表示不记录
    */
    bool getLogFlag();

    /**
    * 功能：得到对应的日志对象
    * @return PackLog* 日志对象
    */
    PackLog* getPacketLogger() 
    {
        return pPacketLogger;
    }

    /**
    * 功能：设置对应的日志对象
    */
    void setPacketLogger(PackLog* pPacketLogger) 
    {
        this->pPacketLogger = pPacketLogger;
    }


    /**
    * 功能：设置用户定义的数据
    */
    void setData(QString data) 
    {
        this->strData = data;
    }

    /**
    * 功能：得到用户定义的数据
    */
    QString getData() 
    {
        return strData ;
    }

    int getChannelType() 
    {
        return channelType;
    }

    void setChannelType(int channelType) 
    {
        proChannelType = this->channelType;
        this->channelType = channelType;
    }
    /**
    * 功能：创建相应的配置信息
    * @param mode 配置信息类型
    * @param ServerConfig* 创建的配置信息
    */
    ServerConfig* createConfig(int way) ;

    /**
    * 功能：从缓冲队列中取出一个缓冲包
    * @return 取出成功返回Packet对象，失败返回null
    */
    Packet* getpkt();

    /**
    * 功能：将缓冲包插入到队列中去
    * @param pack 缓冲包
     * @param isRelease是否释放包资源false表示不释放,true表示释放
    */
    void putpkt(Packet* pack,bool isRelease) 
    {
        mutex.lock();  
        packetQueue.append(pack);
        queueNotFull.wakeAll();
        mutex.unlock();
    }
    /**
    * 功能：取包时,等待有包的条件,没有包则一直等待
    */
    void waitUp()
    {    //等待条件信号量
        mutex.lock();  
        queueNotFull.wait(&mutex,timeout);    //等待5秒的超时
        mutex.unlock();
    }  
    /**
    * 功能：有包则激活条件信号对象queueNotFull
    */
    void wakeUp() 
    {    
        mutex.lock();  
        queueNotFull.wakeAll();
        mutex.unlock();
    } 
    /**
    * 功能：唤醒通道切换线程
    */
    void wakeUpChannel()                 
    {
        TMutexLocker lock(&mutexChannel);
        channelCondition.wakeAll();
    }

    /**
    * 功能：关闭sa对应资源
    */
    virtual void close() ;
    /**
    * 功能：发送数据
    */
    virtual int sendPacket(Packet* pack) ;
    /**
    * 功能：发送数据
    */
    virtual int receivePacket(Packet* pack) ;
    /**
    * 功能：将配置信息序列化到OwnerArchive对象
    */
    virtual void serialize(OwnerArchive& ar) ;

    /**
    * 功能：向router注册,运行server接收和发送线程
    * @return true表示成功
    */
    virtual bool run();
    /**
    * 功能：打开设备和初始化协议
    * @return true表示成功
    */
    virtual bool initDevice();

    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [begin]*/
    /*  Modified brief: 增加获取新配置函数*/
    /**
    * 功能：获取新配置
    * @param pConfig 新配置
    * @param channelType 新通道类型
    * @param aid 获取配置的SA的AID
    * @return true表示成功
    */
    int getNewConfig(ServerConfig * pConfig, int channelType, int aid);
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [end]*/
};

class ProtocolBase;
//////////////ServerAgent接收线程
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [begin]*/
/*  Modifed brief:用单独写的线程类替换QT线程类*/
class RXThread : public baseThread 
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [end]*/
{
private:
    bool runFlag ;                    //线程运行的标志,true表示运行,false表示退出线程
    AgentWithQueue*  pServerAgent;    //保存产生本对象的ServerAgent指针
    waitCondition m_waitCondition;    //确认回复包//未使用
    TMutex waitMutex;                //对缓冲区进行锁定,
public:
    RXThread (AgentWithQueue* pServerAgnet);
    /**
     * 功能：置反线程启动标志，用于停止线程
     */
    void stop();
    /**
     * 功能：线程运行函数
     */
    void run();

};

//////////////ServerAgent发送线程
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [begin]*/
/*  Modifed brief:用单独写的线程类替换QT线程类*/
class TXThread : public baseThread
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [end]*/
{
private:
    AgentWithQueue*  pServerAgent;    //保存产生本对象的ServerAgent指针
    bool runFlag ;        //线程运行的标志,true表示运行,false表示退出线程
    waitCondition queueNotFull;    //队列不为空条件变量
    TMutex mutex;
public:
    TXThread(AgentWithQueue* pServerAgnet);
    /**
     * 功能：置反线程运行标志，调用此方法可停止线程
     */
    void stop();
    /**
     * 功能：线程运行函数
     */
    void run();
};

#endif
