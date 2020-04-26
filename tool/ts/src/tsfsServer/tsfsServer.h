/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/


/**
* @file     tsfsServer.h
* @brief  
*       功能：
*       <li>定义Server，管理文件描述符，收包，创建及激活worker对象</li>
*/
/************************头文件********************************/
#ifndef _TSFSSERVER_H
#define _TSFSSERVER_H

#include <iostream>
#include <list>
#include <queue>
#include "tsfsWorker.h"
#include "tsfsMutex.h"
using namespace std;

/************************宏定义********************************/
//Worker线程池大小
#define TSFS_WORKER_MAX_NUM         20

//Packet数量上限
#define TSFS_PACKET_MAX_NUM         TSFS_WORKER_MAX_NUM

//Worker处理队列大小
#define TSFS_WORKER_QUEUE_SIZE      1    

//IP地址长度
#define TSFS_IP4_LEN                16      

//Packet异常
#define TSFS_INVALID_PACKET         -40

//Server退出
#define TSFS_LINK_ClOSED            -41

#define TSFS_SUCC                   1
#define TSFS_FAIL                   0  

/************************类型定义******************************/
class TSFS_Server
{
private:
    US m_lnkId;                             //连接句柄
    char m_tsIpAddr[TSFS_IP4_LEN];          //TS IP地址
    char m_userDir[MAX_PATH];               //用户工作路径
    bool m_runFlag;                         //Server运行标志

    list <int> m_fileHandle;                //打开的文件句柄链表
    TMutex m_fpMutex;                       //文件句柄互斥锁  

    //Worker线程池
    TSFS_Worker* m_workerThreadPool[TSFS_WORKER_MAX_NUM];
    list <TSFS_Worker*> m_freeWorkerList;   //空闲Worker链表
    TMutex m_workerMutex;                   //Worker互斥锁
    waitcondition *m_pWorkerSemaphore;      //Worker的信号量
    int m_curWorkerNum;                     //计数器,记录当前Worker数量

    //Packet缓冲池
    Packet* m_packetPool[TSFS_PACKET_MAX_NUM];
    list <Packet*> m_emptyPacketList;       //空Packet链表
    TMutex m_packMutex;                     //packet互斥锁
    waitcondition *m_pPacketSemaphore;      //Packet的信号量
    int m_curPacketNum;                     //计数器,记录目前Packet数量

    /*
     * @brief
     *   阻塞接收处理请求
     * @param packet[out]: 接收请求的数据包
     * @return：收到数据包，则返回接收的数据包字节大小
     */
    int recvData(Packet *packet);

    /*
     * @brief
     *   获取一个Pakcet
     * @return：Pakcet
     */
    Packet* getPacket();

    /*
     * @brief
     *   接收处理请求
     * @param packet[out]: 接收请求的数据包
     * @return：接收到处理请求,回复TSFS_SUCC
                连接断开,回复TS_SOCKET_ERROR
     */
    int recvPacket(Packet** packet);

    /*
     * @brief
     *   获取空闲Worker，
     * @return：空闲Worker
     */
    TSFS_Worker* getFreeWorker();

    /*
     * @brief
     *   让Worker处理请求
     * @param packet[in]: 请求数据包
     * @param worker[in]: 处理请求的Worker
     */
    void processData(Packet *packet, TSFS_Worker *worker);

    /*
     * @brief
     *   获取Server运行状态标志
     * @return：Server运行状态标志
     */
    bool bRuning();

public:
    /**
     * @brief
     *   接收请求数据，创建和分配Worker，转交给Worker处理。
     */
    void main();

    /*
     * @brief
     *   退出Server
     */
    void stop();

    /**
     * @brief
     *   获取用户工作路径
     * @return：用户工作路径
     */
    char* getUserDir();

    /**
     * @brief
     *   获取TS IP地址
     * @return：TS IP地址
     */
    char* getTsIPAddr();

    /**
     * @brief
     *   将打开文件的句柄插入句柄管理链表
     * @param fp[in]: 打开的文件句柄
     */
    void insertFileHandle(int fp);

    /**
     * @brief
     *   从句柄管理链表中删除已经关闭的文件句柄
     * @param fp[in]:  已关闭的文件句柄
     */
    void removeFileHandle(int fp);

    /**
     * @brief
     *   在句柄管理链表中查找文件句柄
     * @param fp[in]:   待查询的文件句柄
     * @return：查找成功，则返回TSFS_SUCC
     *          查找失败，则返回TSFS_FAIL
     */
    int searchFileHandle(int fp);

    /**
     * @brief
     *   将使用完毕Packet放入空Packet链表
     * @param pack[in]:   使用完毕的packet
     */
    void recyclePacket(Packet *pack);

    /**
     * @brief
     *   将空闲Worker放入空闲Worker链表
     * @param pack[in]:   使用完毕的packet
     */
    void recycleWorker(TSFS_Worker *worker);

    /**
     * @brief
     *    构造函数，完成TSFS_Server对象的初始化，创建缓冲包放入Server缓冲池
     * @param linkId[in]: TSFS Server主程序连接TS的连接句柄
     * @param tsIp[in]:  TS IP地址
     * @param path[in]: 用户工作目录
     */
    TSFS_Server(US linkId, char *tsIp, char *path);

    /**
     * @brief
     *    析构函数，释放Worker线程池、句柄管理链表所占用的内存
     */
    ~TSFS_Server();

};

#endif