/***************************************************************************
 * 
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 * 
***************************************************************************/


/**
 * &Log:resoure.h,v $
*/


/**
 * @file: resoure.h
 * @brief: 
 * <li>功能: 定义traServer管理</li>
 * @author: tangxp
 * @date: 2009-2-6
 * <p>部门:系统部
 */




/*****************************引用部分********************************/
#include <list>
#include <map>
#include "common.h"
#include "tssConfig.h"
#include "tsServer.h"

/*****************************声明部分********************************/
extern T_TSS_Eviroment_Variables *g_envVar;

/*****************************定义部分********************************/
typedef struct _TSS_Tda
{
    int tdaSaid;    //TDA SAID
    int saAid;      //TDA对应目标机AID
}T_TSS_Tda;

typedef struct _TSS_Gdb
{
    HANDLE hProcess;    //GDB进程句柄
    HANDLE hStdinRd;    //GDB标准输入管道读取端句柄
    HANDLE hStdinWrDup; //GDB标准输入管道写端复制句柄
    HANDLE hStdoutWr;   //GDB标准输出管道写端句柄
    HANDLE hStdoutRdDup;    //GDB标准输出管道读取端复制句柄
}T_TSS_Gdb;

/**
 * @brief:
 * <li>功能:管理traServer,gdb,tda
 * @author: tangxp
 * @version: 1.0
 * @date: 2009-2-6
*/
class resourceManager
{
public:
    /**
    * @brief: 
    *    获取resourceManager对象
    */
    static resourceManager* getInstance() 
    {
        if (m_rm == NULL)
        {
            TMutexLocker lock(&m_rmMutex);
            if (m_rm == NULL)
            {
                m_rm = new resourceManager();
                atexit(destroy);
            }            
        }

        return m_rm;
    }

    /**
    * @brief: 
    *    构造函数,连接TS
    */
    resourceManager();

    /**
    * @brief: 
    *    析构函数,关闭连接,释放创建的traServer
    */
    ~resourceManager();

    /**
     * @brief: 
     *    分配一个traServer
     * @param desAid: traServer通信desAid
     * @param desAid: traServer通信desSaid
     * @return 成功返回traServerId，失败返回FAIL
    */
    int allocTraServer(int desAid, int desSaid);
    
    /**
    * @brief: 
    *   释放一个traServer
    * @param traServerId: traServer的ID
    */
    void freeTraServer(int traServerId);

    /**
    * @brief: 
    *   获取traServer监听的TCP端口
    * @param traServerId: traServer的ID
    * @return 成功返回端口，失败返回FAIL
    */
    int getDllPort(int traServerId);

    /**
    * @brief: 
    *   释放所有的traServer
    */
    void freeAllTraServer();

    /**
     * @brief: 
     *    创建一个TDA
     * @param saAid: 目标机aid
     * @return 成功返回TDA的SAID，失败返回FAIL
    */
    T_TSS_Tda* createTda(int saAid);

    /**
    * @brief: 
    *   删除一个TDA
    * @param tpItem: 包含TdaSaid和对应目标机aid
    * @return 成功返回SUCC，失败返回FAIL
    */
    int deleteTda(T_TSS_Tda* tpItem);

    /**
    * @brief: 
    *   删除所有的TDA
    */
    void deleteAllTda();

    /**
     * @brief: 
     *    注册一个GDB
     * @param saAid: 目标机aid
     * @return 成功返回包含GDB进程句柄及输入,输出句柄的对象
     *      失败返回NULL
    */
    T_TSS_Gdb* createGdb(int saAid);

    /**
    * @brief: 
    *   注销一个GDB
    * @param tpItem: 包含TdaSaid和对应目标机aid
    * @return 成功返回SUCC，失败返回FAIL
    */
    int deleteGdb(T_TSS_Gdb* tpItem);

    /**
    * @brief: 
    *   删除所有的GDB
    */
    void deleteAllGdb();    

private:

    /**
    * @brief: 
    *   设置traServer通信desSaid
    * @param traServerId: traServer的ID
    * @param desAid: traServer通信desSaid
    * @return 成功返回SUCC，失败返回FAIL
    */
    int setDesId(int traServerId, int newDesAid, int newDesSaid);

    /**
    * @brief: 
    *   释放资源
    */
    static void destroy()
    {
        if (NULL != m_rm)
        {
            delete m_rm;
            m_rm = NULL;
        }
    }

    static resourceManager *m_rm;
    static TMutex m_rmMutex;                        //资源管理员互斥对象
    TMutex m_traMutex;                        //tra互斥对象
    TMutex m_tdaMutex;                        //tda互斥对象
    TMutex m_gdbMutex;                        //gdb互斥对象
    static const int LOW_LEVEL = 5;     //最低水位标志
    static const int HIGH_LEVEL = 20;    //最高位标志
    unsigned long m_traServerSN;     //traServer序号
    list<int> m_traFreeList;        //保存空闲traServer链表
    list<T_TSS_Tda*> m_tdaList;        //保存创建的TDA及其对应目标机AID
    list<T_TSS_Gdb*> m_gdbList;        //保存创建的GDB进程句柄及其读写端句柄
    int m_traWaterLevel;                //当前水位值 
    waitCondition m_traOverHighLevel;   //条件变量,用来判断是否低于高水位
    int m_fd;     //连接句柄    

};

