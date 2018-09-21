/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  SubAgentBase.h
* @brief
*       功能：
*       <li> manager的二级代理抽象类和控制命令类</li>
*/


#ifndef SUBAGENTBASE_INCLUDE
#define SUBAGENTBASE_INCLUDE

/************************头文件********************************/

#include "tsInclude.h"

/************************宏定义********************************/

/************************类型定义******************************/

class Manager;

class ServerAgent;
class Packet;
class DeviceConfigBase;
class SubAgentBase
{
private:
    short SAID;    //二级代理号
    bool logflag;  // 日志标记
public:

    /**
    * 功能：构造函数,用来初始化二级代理的aid号
    * @param said 二级代理号
    */
    SubAgentBase(short said)
    {
        SAID = said;
    }
    virtual ~SubAgentBase(void);
    /**
    * 功能：返回二级代理号
    */
    short getSaid()
    {
        return SAID;
    }
    /**
    * 功能：设置日志标记
    */
    void setLogFlag(bool flag)
    {
        logflag = flag;
    }
    /**
    * 功能：获取是否需要记录日志
    */
    bool getLogFlag()
    {
        return logflag;
    }
    /**
    * 功能：初始化当前的二级代理
    */
    virtual void init() = 0;
    /**
    * 功能：解析当前的数据包,并执行操作
    * @param pack 数据保
    */
    virtual void process(Packet* pack) = 0;
};


class ControlSubAgent : public SubAgentBase
{

protected:

    PackLog packLoger;
    //end
    /**
    * 功能：解析包格式
    * @param pack 接收的控制包
    */
    bool parsePacket(Packet* pack);
    /**
    * 功能：处理TClient发送过来的GetID控制命令,并把得到的AID返回给TClient
    * @param pack 接收的控制包
    */
    void handleGetID(Packet* pack) ;
    /**
    * 功能：处理TClient发送过来的createServer控制命令,并把得到的AID返回给TClient
    * @param pack 接收的控制包
    */
    void handleCreateServer(Packet* pack);
    /**
    * 功能：处理TClient发送过来的createServer控制命令,并把得到的AID返回给TClient
    * @param block 去掉createserver数据
    * @param pack 接收的控制包
    */
    void handleCreateServerWithDll(Packet* pack) ;
    /**
    * 功能：处理TClient发送过来的createServer控制命令,并把得到的AID返回给TClient
    * @param block 去掉createserver标志符数据
    * @param pack 接收的控制包
    */
    void handleDeleteServer(Packet* pack);
    /**
    * 功能：发送失败回复消息
    * @param pack 要发送的数据包


    * @param strErrMsg: 错误信息
    * @param size: 错误信息长度
    */
    void sendErrorAck(Packet* pack, char *strErrMsg, UI size = FAILURE_ACK_LEN);
    /**
    * 功能：设置成功包信息
    * @param pack 要发送的数据包
    */
    void setSuccessPacket(Packet* pack) ;
    /**
    * 功能：设置并发送成功的回复包
    * @param pack 要发送的数据包，带有目的和源ID号
    */
    void sendSuccessPacket(Packet* pack) ;
    /**
    * 功能：交换包的目标SAID和AID
    * @param pack 接收的控制包
    */
    void changePacketID(Packet* pack);
    /**
    * 功能：处理TCLIENT_OPEN函数,返回源AID给调用着
    * @param pack 接收的控制包
    */
    void handleTClientOpen(Packet* pack) ;
    ////////////////////////////控制包start
    /**
    * 功能：处理TClient_queryServer函数,指定目标服务ID号查找该目标机的配置参数和状态信息
    * @param block 去掉queryServerState标志符数据
    * @param pack 接收的控制包
    */
    void handleQueryServer(Packet* pack);
    /**
    * 功能：处理TClient_searchServer函数,查询TS上面存在的所有的可用目标服务ID
    * @param pack 接收的控制包
    */
    void handleSearchServer(Packet* pack);
    /**
    * 功能：处理TClient_activeServer函数,激活指定的处于休眠状态的目标服务
    * @param block 去掉SearchServer标志符数据
    * @param pack 接收的控制包
    */
    void handleActiveServer(Packet* pack);
    /**
    * 功能：处理TClient_inActiveServer函数,休眠指定的处于激活状态的目标服务
    * @param block 去掉ActiveServer标志符数据
    * @param pack 接收的控制包
    */
    void handleInactiveServer(Packet* pack) ;
    /**
    * 功能：处理TClient_getReport函数,工具可选用单独的线程阻塞在该方法。得到TS自动上报的信息
    * @param pack 接收的控制包,去掉RegisterServer标志符数据
    * @param offset 数据偏移位置
    */
    void handleRegisterServer(Packet* pack) ;
    /**
    * 功能：处理TClient_unregister函数,注销会话ID,关闭SOCKET
    * @param pack 接收的控制包
    * @param offset 数据偏移位置
    */
    void handleUnregisterServer(Packet* pack) ;
    /**
    * 功能：处理TClient_modifyServer函数,修改一个已经存在在TS中的目标服务配置
    * @param block 去掉modifyServer标志符数据
    * @param pack 接收的控制包
    */
    void handleModifyServer(Packet* pack) ;
    /**
    * 功能：处理TClient_setRegisterType函数,设置sa的类型,并进行上报
    * @param pack 接收的控制包
    * @param offset 数据偏移位置
    */
    void handleSetRegisterType(Packet* pack) ;

    /**
    * 功能：处理TClient_clearRegisterType函数,清除sa的注册类型,并进行上报
    * @param pack 接收的控制包
    * @param offset 数据偏移位置
    */
    void handleClearRegisterType(Packet* pack) ;

    /**
     * @Funcname:  handleQueryHostSerialPort
     * @brief        :  处理查询TS所在主机串口配置
     * @para1      :  pack 接收的控制包
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007年1月10日 11:57:38
     *
    **/
    void handleQueryHostSerialPort(Packet* pack);

    /**
     * @Funcname:  handleSetSystemLogLevel
     * @brief        :  处理设置系统日志输出的级别
     * @para1      : pack 接收的控制包
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007年1月10日 11:58:42
     *
    **/
    void handleSetSystemLogLevel(Packet* pack);

    /**
     * @Funcname:  handleSetAgentLogFlag
     * @brief        :  处理设置服务代理的日志标记
     * @para1      : pack 接收的控制包
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007年1月10日 11:59:41
     *
    **/
    void handleSetAgentLogFlag(Packet* pack);

    /**
     * @Funcname:  handleCloseClient
     * @brief        :  处理客户端关闭连接的命令,需要清除系统中与之相关的数据
     * @para1      : pack 接收的控制包
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007年1月11日 11:27:49
     *
    **/
    void handleCloseClient(Packet* pack);

    /**
     * @Funcname:  handleUpLoadFile
     * @brief        :  处理TSAPI上传文件
     * @para1      : pack命令包
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007年3月2日 18:00:06
     *
    **/
    void handleUpLoadFile(Packet* pack);

    /**
     * @Funcname:  handleDownLoadFile
     * @brief        :  处理TSAPI下载文件
     * @para1      : pack命令包
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007年3月2日 18:00:06
     *
    **/
    void handleDownLoadFile(Packet* pack);

    /**
     * @Funcname:  handleDeleteFile
     * @brief        :  处理删除文件命令
     * @para1      : pack命令包
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007年3月12日 11:31:31
     *
    **/
    void handleDeleteFile(Packet* pack);

    /**
     * @Funcname:  queryHostSerialPort
     * @brief        : 查询TS所在主机串口配置
     * @para1      :  pBuf 接收查询数据的缓冲
     * @para2      :  bufsize: 缓冲的字节大小
     * @return      :  查询成功则返回字节长度，否则返回-1
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007年1月10日 11:57:38
     *
    **/
    int queryHostSerialPort(char* pBuf, US bufsize);


    ////////////////////////////控制包end
    /**
    * 功能：处理TClient发送过来的createServer控制命令,并把得到的AID返回给TClient
    * @param deviceConfig 设备配置信息
    * @param name sa的名字
    * @param isLog sa是否需要日志记录
    * @param protocolType sa对应的协议
    * @param state sa的状态
    * @return ServerAgent* 返回sa对象指针
    */
    ServerAgent* createSA(DeviceConfigBase* deviceConfig, QString name, bool isLog=0,
                          int protocolType=0, int state=1) ;

public:
    ControlSubAgent(short said=0);

    ~ControlSubAgent()
    {
    }

    /**
    * 功能：初始化当前的二级代理
    * @param SAID 二级代理号
    */
    virtual void init() ;

    /**
    * 功能：解析当前的数据包,并执行操作
    * @param pack 数据保
    */
    virtual void process(Packet* pack);
    /**
    * 功能：提供给MANAGER模块来记录控制包日志
    *
    */
    void writePackForManager(PackAct act, const Packet &pack)
    {
        packLoger.write(act, pack);
    }
};

/************************接口声明******************************/
#endif
