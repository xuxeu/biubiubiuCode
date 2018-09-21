/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  Manager.h
* @brief
*       功能：
*       <li>TS的主线程声明</li>
*/




#ifndef MANAGER_INCLUDE
#define MANAGER_INCLUDE

/************************头文件********************************/
#include "tsInclude.h"
#include "Agent.h"
#include "config.h"
/************************宏定义********************************/
//TS允许创建的最大服务数量
#define MAX_AGENT_NUM 256

//获取当前连接上的DID
#define CURDID(a) (Manager::getInstance()->getCurDID(a))

//MANAGER模块的ID号
#define MANAGER_AID     0
/************************类型定义******************************/

class Packet;
class AgentWithQueue;
class Listen;

////////配置信息的文件名为
//主线程要有一个循环一直监听数据包的到达

class SubAgentBase;
typedef QList<AgentWithQueue*> AGENTLIST;
typedef QMap<short,QList<int>*> SHORT2LIST;
typedef QList<int> INTLIST;



class Manager : public AgentWithQueue
{
private:
    QString appDir;            //当前程序运行目录
    int listenPort;            //监听端口号

    bool runFlag;
    Listen* pListen;
    static Manager* pManager;
    static TMutex mutexInstance;            //确保初始化操作互斥

    /****************映射表******************/
    TMutex mtxMapSAID2SubAgent;             //mapSAID2SubAgent的互斥量
    QMap<short,SubAgentBase*> mapSAID2SubAgent;    //二级代理号到二级代理对象的映射

    //配置信息数组
    TMutex mtxMapAID2SA;            //对mapAID2SA进行互斥操作
    QMap<short,AgentBase*> mapAID2SA;    //管理aid到ServerAgent关联

    //名字到ID号
    TMutex mtxMapName2ID;           //mapName2ID的互斥量
    QMap<QString ,short> mapName2ID;

    //文件传送功能时用于保存传送期间的文件名和句柄
    QMap<string, FILE*> mapName2File;

    //注册类型到CA对象列表
    TMutex mtxAgentArray;
    QList<AgentBase*>* agentArray[MAX_REG_TYPE + 2];

    TMutex mtxFreeIDList;
    QList<US> freeIDList;              //保存空闲ID 的链表

    //计数器,统计当前TS中有多少个SA/CA对象
    static unsigned short counter ;

    //CA连接到对话ID的映射
    map<US, US> mapLnk2DID;
    TMutex mtxMapDID;

    /**
    *功能：初始化空闲ID链表
    */
    void initFreeIDList();

    /**
    * 功能：删除所有server
    */
    void deleteAllServer() ;

    /**
    * 功能：发送新的sa的状态
    */
    void setNewState(Packet *pack, int regCount,char** pBuf,int putSize) ;
    bool mfCreateDirectory( QString &strDirectory );

    bool createDirectory( QString &strDirectory );
public:
    //TS保存文件的临时路径
    static const string tmpfilepath;

    /**
    * 功能：得到监听端口号
    */
    QString getAppDir()
    {
        return appDir;
    }

    /**
    * 功能：得到监听端口号
    */
    int getListenPort()
    {
        return listenPort;
    }

    /**
    * 功能：设置监听端口号
    */
    void setListenPort(int port)
    {
        listenPort = port;
    }

    /**
     * @Funcname:  sendSAReportToCA
     * @brief        :  发送SA的上报信息给指定的CA
     * @para1      : AgentWithQueue * pSA 待上报信息的SA对象
     * @para2      : caAid： 上报信息目的地，0表示所有的CA都发送
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007年1月8日 11:26:18
     *
    **/
    void sendSAReportToCA(AgentWithQueue * pSA, US caAid = 0);

    /**
    *功能：进行某种类型SA的上报
    * regType: 需要上报的注册类型号
    * caAid： 上报信息目的地，0表示所有的CA都发送
    */
    void sendRegTypeSAToCA(int regType, US caAid = 0);

    /**
    * 功能: 通过AID号得到serverAgent对象
    * @param aid aid号
    * return ServerAgent* NULL表示没有此aid号,
    */
    AgentBase* getServerAgentFromID(short aid);

    /**
    * 功能：删除指定server
    * @param aid serverAgent的aid号
    */
    int deleteServer(int aid);
    /**
    * 功能：返回名字到SA的AID的映射
    * @param aid serverAgent的aid号
    */
    QMap<QString ,short>& getName2AIDMap()
    {
        return mapName2ID;
    }

    /**
    * 功能：修改映射mapAID2SA
    * @param oldName 要替换的原来名字
    * @param newName 替换后的名字
    */
    void modifyName2AIDMap(QString oldName,QString newName) ;

    /**
    * 功能：把二级代理插入
    * @param pSubAgent 把SubAgent对象插入map对象mapSAID2SubAgent中
    */
    void addSubAgent(SubAgentBase* pSubAgent) ;

    /**
    * 功能：把serverAgent插入
    * @param pServerAgent 把sa对象插入map对象mapAID2SA中
    */
    void addServerAgent(AgentBase* pServerAgent) ;

    /**
    * 得到唯一实例.
    * @return Manager* 唯一实例
    */
    static Manager* getInstance()
    {
        if (pManager == NULL)
        {
            TMutexLocker locker(&mutexInstance);

            if (pManager == NULL)
            {
                pManager = new Manager();
            }
        }

        return pManager;
    }
    Manager(unsigned short aid = MANAGER_AID);
    ~Manager();

    /**
    * 功能：程序的主线程,入口函数
    */
    void main();

    /**
    * 功能：读取配置文件，并加载配置启动对应ServerAgent
    * @param name 配置文件名，含绝对路径
    * @return
    *       TS_INVALID_CFG_FILE: 打开文件失败
    *       TS_SUCCESS: 加载配置成功
    */
    int loadConfigure(const QString name) ;

    /**
    * 功能：将配置信息保存到文件中去
    * @return 成功返回true，失败返回false
    */
    bool storeConfigure();

    /**
    * 功能: 通过名字得到对应的ID号
    * @param name 名字
    * return ID 号-1表示没有
    */
    int getIDFromName(const QString& name);

    /**
    * 功能：产生ID号
    * @return ID号
    */
    unsigned short createID();

    /**
    * 功能：释放指定的AID，将其添加到空闲AID链表
    * @return
    */
    void freeAid(US aid);

    /**
    * 功能：退出manager
    */
    void stop();

    /**
    * 功能：处理TClient_register函数,TS进行注册,注册后,TS会向TC自动发送消息和状态
    * @param pack 接收的控制包
    * @return TS_SUCCESS表示成功，否则失败
    */
    int registerServer(Packet* pack,int regType);

    /**
    * 功能：处理TClient_unregister函数,注销会话ID,关闭SOCKET
    * @param pack 接收的控制包
    * @return TS_SUCCESS表示成功，否则失败
    */
    int unRegisterServer(Packet* pack,int regType) ;

    /**
    * 功能：处理TClient_setRegisterType函数,设置sa的注册类型
    * @param aid sa的aid号
    * @param regType 要注册的类型号
    * @param isStore 是否保存,默认true为写文件,false为不写文件
    * @return true表示注册成功,false表示已注册了此aid
    */
    bool setRegisterType(short aid,int regType,bool isStore = true);

    /**
    * 功能：处理TClient_unregister函数,注销会话ID,关闭SOCKET
    * @param aid sa的aid号
    * @param regType 要清楚的类型号
    * @return true表示注册成功,false表示已注册了此aid
    */
    bool clearRegisterType(short aid) ;

    /**
     * @Funcname:  getCurDID
     * @brief        :  获取当前连接的对话ID
     * @para1      : fd, 指定的连接
     * @return      : 返回指定连接的当前对话号
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2006年12月26日 15:41:57
     *
    **/
    int getCurDID(US fd)
    {
        TMutexLocker locker(&mtxMapDID);
        map<US, US>::iterator m_ItrDID = mapLnk2DID.find(fd);

        if(m_ItrDID == mapLnk2DID.end())
        {
            sysLoger(LWARNING, "Main::getCurDID: error linker id %d.", fd);
            return -1;
        }

        return m_ItrDID->second;
    }

    /**
         * @Funcname:  setCurDID
         * @brief        : 设置当前连接的对话ID
         * @para1      : fd, 指定的连接
         * @return      : 无
         * @Author     :  qingxiaohai
         *
         * @History: 1.  Created this function on 2006年12月26日 15:41:57
         *
        **/
    void setCurDID(US fd, US DID)
    {
        TMutexLocker locker(&mtxMapDID);
        map<US, US>::iterator m_ItrDID = mapLnk2DID.find(fd);

        if(m_ItrDID == mapLnk2DID.end())
        {
            sysLoger(LINFO, "Main::setCurDID: New link id %d.", fd);
            mapLnk2DID.insert(pair<US, US>(fd, DID));
            return;
        }

        m_ItrDID->second = DID;
        return;
    }
    /**
     * @Funcname:  isSACountReachedToMAX
     * @brief        :  判断当前对象的数量是否达到TS允许的最大值
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007年1月8日 14:38:37
     *
    **/
    bool isAgentCountReachedToMAX()
    {
        if(counter >= MAX_AGENT_NUM)
        {
            return true;
        }

        return false;
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
    bool insertFileToMap(string fn, FILE *fp);

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
    FILE* getFileFromName(string fn);
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
    bool removeAndCloseFileFromMap(string fn);

};

/************************接口声明******************************/
#endif // !defined(MANAGER_INCLUDE)
