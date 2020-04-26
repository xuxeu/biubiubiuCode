#ifndef SERVERAGENTWITHDLL_INCLUDE
#define SERVERAGENTWITHDLL_INCLUDE

#include "agent.h"
#include "Windows.h"
#include "OwnerArchive.h"
#include "tsInclude.h"

/**
功能: 动态库向TS注册的回调函数, SA DLL的TX线程会调用此方法对收到数据进行处理
* @param id sa的源said号
* @param des_said 目标said号
* @param src_aid  ca的aid号
* @param src_said ca的said号
* @param pBuf 发送的缓冲区
* @param size 缓冲区大小
* @return0表示失败,非0表示成功
*/
typedef int (*PUTPKT)(short id,short des_said,short src_aid,short src_said,char* pBuf,int size);

/**
功能: 动态库向TS注册的回调函数, SA DLL的RX线程会调用此方法接收动态库的数据进行转发
* @param id sa的源said号
* @param des_said[out] 目标said号
* @param src_aid[out]  ca的aid号
* @param src_said[out] ca的said号
* @param pBuf[out] 发送的缓冲区
* @param size 缓冲区大小
* @return 数据的大小
*/
typedef int (*GETPKT)(short id,short *des_said,short *src_aid,short *src_said,char* pBuf,int size);

/**
功能: 打开动作,便于动态库内部做初始化工作
* @param id sa的源said号
* @param name 传递的参数
* @return 返回端口号,小于0则表示失败
*/
typedef int (*OPEN)(short id,char* pBuf,char* pOutBuf);

/**
功能: 关闭动作,便于动态库内部释放占用资源
* @param id sa的源said号
* @param name 传递的参数
* @return 成功或失败
*/
typedef int (*CLOSE)(short id);
/**
功能: 释放整个动态库资源
* @return 0表示成功
*/
typedef int (*RELEASEDLL)();

class ServerAgentWithDll :
    public AgentWithQueue
{
private:
    HMODULE m_hDll;
    QString dllName;                        //对应动态库的名字
    QString inBuff;                            //对应传递给动态库的字符串,传入参数
    char outBuff[PACKET_DATASIZE];                        //动态库缓冲区返回给调用者,传出参数
    //对应的动态库函数指针
    GETPKT get;
    PUTPKT put;
    OPEN open;
    CLOSE closeDll;
    RELEASEDLL releaseDll;    
public:
    PUTPKT getPutFunc() 
    {
        return put;
    }
    GETPKT getGetFunc() 
    {
        return get;
    }
    OPEN getOpenFunc() 
    {
        return open;
    }
    CLOSE getCloseFunc() 
    {
        return closeDll;
    }

    ServerAgentWithDll(unsigned short aid);
    virtual ~ServerAgentWithDll(void);
    /**
    * 功能：设置传递给动态库的字符串
    * @param buf 字符串
    */
    void setInBuf(const QString& inBuff) 
    {
        this->inBuff = inBuff;
    }
    /**
    * 功能：得到传递给动态库的字符串
    * @return 动态库的使用参数字符串
    */
    QString getInBuf() 
    {
        return inBuff;
    }

    /**
    * 功能：得到从动态库返回的缓冲区
    * @return 动态库的使用参数字符串
    */
    char* getOutBuf() 
    {
        return outBuff;
    }
    /**
    * 功能：设置动态库名字
    * @param QString 名字
    */
    void setDllName(const QString& dllName) 
    {
        this->dllName = dllName;
    }
    /**
    * 功能：得到动态库名字
    * @return QString 名字
    */
    QString getDllName() 
    {
        return dllName;
    }

    /**
    * 功能：向router注册,运行server接收和发送线程
    * @return true表示成功
    */
    bool run() ;

    /**
    * 功能：得到动态库句柄
    * @return 动态库句柄
    */
    HMODULE getDllHandler()
    {
        return m_hDll;
    }
    /**
    * 功能：停止server线程
    */
    void stop();
    /**
    * 功能：关闭sa对应资源
    */
    void close() ;
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
    * 功能：将配置信息序列化到OwnerArchive对象
    */
    void serialize(OwnerArchive& ar) ;
};
#if 0
//////////////ServerAgent接收线程
class RXThreadWithDll : public QThread {
private:
    bool runFlag ;                //线程运行的标志,true表示运行,false表示退出线程
    ServerAgentWithDll*  pServerAgent;    //保存产生本对象的ServerAgent指针
public:
    RXThreadWithDll (ServerAgentWithDll* pServerAgnet);
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
class TXThreadWithDll : public QThread {
private:
    ServerAgentWithDll*  pServerAgent;    //保存产生本对象的ServerAgent指针
    bool runFlag ;        //线程运行的标志,true表示运行,false表示退出线程
    waitCondition queueNotFull;    //队列不为空条件变量
    TMutex mutex;
public:
    TXThreadWithDll(ServerAgentWithDll* pServerAgnet);
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
#endif
