/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  tsfsServer.cpp
* @brief  
*       功能：
*       <li>实现Server，管理文件描述符，收包，创建及激活worker对象</li>
*/

/************************头文件********************************/
#include "stdafx.h"
#include <io.h>
#include "tsfscommon.h"
#include "tsfsServer.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************全局变量******************************/

/************************模块变量******************************/

/************************外部声明******************************/

/************************前向声明******************************/

/************************实现*********************************/

/**
 * @brief
 *    构造函数，完成TSFS_Server对象的初始化，创建缓冲包放入Server缓冲池
 * @param linkId[in]: TSFS Server主程序连接TS的连接句柄
 * @param tsIp[in]:  TS IP地址
 * @param path[in]: 用户工作目录
 */
TSFS_Server::TSFS_Server(US linkId, char *tsIp, char *path)
{
    memset(m_workerThreadPool, 0, sizeof(m_workerThreadPool));
    memset(m_packetPool, 0, sizeof(m_packetPool));
    memset(m_userDir, 0, sizeof(m_userDir));
    memset(m_tsIpAddr, 0, sizeof(m_tsIpAddr));

    memcpy(m_userDir, path, sizeof(m_userDir));     //保存用户工作目录
    memcpy(m_tsIpAddr, tsIp, sizeof(m_tsIpAddr));   //保存TS IP地址
    m_lnkId             = linkId;                   //保存连接句柄
    m_runFlag           = true;                     //设置Server运行
    m_curWorkerNum      = 0;                        //尚未创建Worker 
    m_curPacketNum      = 0;                        //尚未创建Packet
    
    //创建Packet信号量
    m_pPacketSemaphore  = new waitcondition(TSFS_PACKET_MAX_NUM);

    //创建Worker信号量
    m_pWorkerSemaphore  = new waitcondition(TSFS_WORKER_MAX_NUM);
}

/**
 * @brief
 *    析构函数，释放Worker线程池、句柄管理链表所占用的内存
 */
TSFS_Server::~TSFS_Server()
{
    //释放Worker线程池
    for(int i=0; i < m_curWorkerNum; ++i)
    {
        delete m_workerThreadPool[i];
        m_workerThreadPool[i] = NULL;
    }

    //清空Worker链表
    m_freeWorkerList.clear();

    //释放Packet
    for(int i=0; i< m_curPacketNum; ++i)
    {
        delete m_packetPool[i];
        m_packetPool[i] = NULL;
    }

    //清空Packet链表
    m_emptyPacketList.clear();

    //清空文件句柄链表
    list <int>::iterator iter;
    for ( iter = m_fileHandle.begin( ); iter != m_fileHandle.end( ); iter++ )
    {
        //关闭未关闭的文件
        _close((*iter)); 
    }
    m_fileHandle.clear();

    //删除Worker信号量
    delete m_pWorkerSemaphore;

    //删除Packet信号量
    delete m_pPacketSemaphore;
}

/**
 * @brief
 *   接收请求数据，创建和分配Worker，转交给Worker处理。
 */
void TSFS_Server::main()
{
    int ret = 0;
    Packet *packet = NULL;
    TSFS_Worker *worker = NULL;

    while(bRuning())
    {
        //接收请求
        ret = recvPacket(&packet);

        if(TSFS_INVALID_PACKET == ret)
        {
            //数据包出现异常,重新收包
            continue;
        }
        else if( TSFS_LINK_ClOSED == ret)
        {
            //连接断开,退出
            break;
        }        

        //获取Worker
        worker = getFreeWorker();

        //处理请求
        processData(packet, worker);
    }
}

/*
 * @brief
 *   阻塞接收处理请求
 * @param packet[out]: 接收请求的数据包
 * @return：收到数据包，则返回接收的数据包字节大小
 */
int TSFS_Server::recvData(Packet *packet)
{
    /*Modified by tangxp for BUG NO.0002964 on 2008.3.19 [begin]*/
    /*  Modified brief: 改善日志记录*/
    if(NULL == packet)
    {
        sysLoger(LDEBUG, "TSFS_Server::recvData: packet is invalid" );

        return TSFS_INVALID_PACKET;
    }

    int dataSize = -1;
    US src_aid = 0; 
    US des_said = 0;
    US src_said = 0;

    sysLoger(LDEBUG, "TSFS_Server::recvData: wait for data" );
    
    //循环接收处理请求
    while( m_runFlag )
    {  
        //阻塞接收处理请求
        dataSize = TClient_getpkt(m_lnkId,
                                    &src_aid,
                                    &des_said,
                                    &src_said,
                                    packet->getData(),
                                    packet->getDataSize());

        if( dataSize > 0 )
        {     
            //接收到处理请求，保存目标机AID
            packet->setSrc_aid( src_aid );

            //保存目标机SAID
            packet->setSrc_said( src_said );

            //保存处理请求数据大小
            packet->setSize((US)dataSize);

            sysLoger(LINFO, "TSFS_Server::recvData: receive a packet, [aid:%d], [said:%d], [size:%d]",
                            src_aid, src_said, dataSize);

            sysLoger(LDEBUG, "TSFS_Server::recvData: receive a packet, data:[%c]",
                            packet->getData()[0]);

            return dataSize;
        }
        else if( TS_SOCKET_ERROR == dataSize 
                 || TS_INVALID_LINK_HANDLE == dataSize )
        {
            //连接已关闭
            sysLoger(LWARNING, "TSFS_Server::recvData: link closed, [ret:%d]",
                                dataSize);
            return TSFS_LINK_ClOSED;
        }
    }
    /*Modified by tangxp for BUG NO.0002964 on 2008.3.19 [end]*/    
}

/*
 * @brief
 *   获取空闲Worker，
 * @return：空闲Worker
 */
TSFS_Worker* TSFS_Server::getFreeWorker()
{     
    TSFS_Worker *freeWorker  = NULL;

    sysLoger(LDEBUG, "TSFS_Server::getFreeWorker: begin get free worker" );

    while(1)
    {
        //若没有空闲Worker,且没有达到Worker数量上限,则新建Worker
        if( m_freeWorkerList.empty() && m_curWorkerNum < TSFS_WORKER_MAX_NUM )
        {
            //若没有新建过Worker,则必须新建成功
            do
            {
                //新建一个Worker
                freeWorker = new TSFS_Worker( TSFS_WORKER_QUEUE_SIZE );
                if( freeWorker )
                {
                    //新建Worker成功
                    m_workerThreadPool[m_curWorkerNum] = freeWorker;

                    //记录Worker数量
                    ++m_curWorkerNum;

                    sysLoger(LDEBUG, "TSFS_Server::getFreeWorker: create new worker success, [ThreadID = %d], [worker sum:%d]",
                                freeWorker->getThreadID(), m_curWorkerNum);
                    return freeWorker;
                }

                sysLoger(LDEBUG, "TSFS_Server::getFreeWorker: create new worker failed, [worker sum:%d]",
                                m_curWorkerNum);
            }while( 0 == m_curWorkerNum );
        }

        //从空闲Worker链表中取Worker
        if( m_pWorkerSemaphore->wait() )
        {
            m_workerMutex.lock();

            freeWorker = m_freeWorkerList.front();

            m_freeWorkerList.pop_front();

            m_workerMutex.unlock();

            sysLoger(LDEBUG, "TSFS_Server::getFreeWorker: get free worker success, [ThreadID = %d]",
                                freeWorker->getThreadID());
            return freeWorker;
        }

        sysLoger(LDEBUG, "TSFS_Server::getFreeWorker: wait for free worker failed");
    }    
}        

/**
 * @brief
 *   将打开文件的句柄插入管理链表
 * @param fp[in]: 文件句柄
 */
void TSFS_Server::insertFileHandle(int fp)
{
    m_fpMutex.lock();

    m_fileHandle.push_back(fp);

    m_fpMutex.unlock();
}

/**
 * @brief
 *   从管理链表中删除已经关闭的文件句柄
 * @param fp[in]:   文件句柄
 */
void TSFS_Server::removeFileHandle(int fp)
{
    m_fpMutex.lock();

    m_fileHandle.remove(fp);

    m_fpMutex.unlock();
}

/**
 * @brief
 *   在句柄管理链表中查找文件句柄
 * @param fp[in]:   待查询的文件句柄
 * @return：查找成功，则返回TSFS_SUCC
 *          查找失败，则返回TSFS_FAIL
 */
int TSFS_Server::searchFileHandle(int fp)
{
    list<int>::iterator iter;

    //在句柄管理链表中查找文件句柄
    for(iter=m_fileHandle.begin(); iter!=m_fileHandle.end(); iter++)
    {
        //若查找成功，返回TSFS_SUCC
        if( (*iter) == fp )
        {
            return TSFS_SUCC;
        }
    }

    //句柄管理链表中没有所查询的文件句柄，返回TSFS_FAIL
    return TSFS_FAIL;
}

/*
 * @brief
 *   退出Server
 */
void TSFS_Server::stop()
{
    //设置停止运行
    m_runFlag = false;
    
    //释放Worker线程池
    for(int i=0;i < m_curWorkerNum ;i++)
    {
        m_workerThreadPool[i]->stopThread();
    }
}

/**
 * @brief
 *   获取用户工作路径
 * @return：用户工作路径

 */
char* TSFS_Server::getUserDir()
{
    return this->m_userDir;
}

/**
 * @brief
 *   获取TS IP地址
 * @return：TS IP地址
 */
char* TSFS_Server::getTsIPAddr()
{
    return this->m_tsIpAddr;
}

/*
 * @brief
 *   获取一个Pakcet
 * @return：Pakcet
 */
Packet* TSFS_Server::getPacket()
{
    Packet *packet = NULL;

    sysLoger(LDEBUG, "TSFS_Server::getPacket: begin get empty packet" );

    while(1)
    {
        //若没有空Pakcet且Packet没有达到上限数量,则新建
        if( m_emptyPacketList.empty() && m_curPacketNum < TSFS_PACKET_MAX_NUM )
        {  
            //若还没有Packet,则必须新建成功
            do
            {
                //新建一个Pakcet
                packet = new Packet;
                if( packet )
                {
                    //新建Packet成功
                    m_packetPool[m_curPacketNum] = packet;

                    //记录Packet数量
                    ++m_curPacketNum;

                    sysLoger(LDEBUG, "TSFS_Server::getPacket: create new packet success, [packet sum:%d]",
                                    m_curPacketNum);
                    return packet;
                }

                sysLoger(LDEBUG, "TSFS_Server::getPacket: create new packet failed, [packet sum:%d]",
                                    m_curPacketNum);
            }while( 0 == m_curPacketNum );
        }

        //从空Pakcet链表中取Packet
        if( m_pPacketSemaphore->wait() )
        {
            m_packMutex.lock();

            packet = m_emptyPacketList.front();

            m_emptyPacketList.pop_front();

            m_packMutex.unlock();

            sysLoger(LDEBUG, "TSFS_Server::getPacket: get empty packet success" );
            return packet;
        }

        sysLoger(LDEBUG, "TSFS_Server::getPacket: wait for empty packet failed" );
    }
}

/*
 * @brief
 *   接收处理请求
 * @param packet[out]: 接收请求的数据包
 * @return：接收成功,回复接收数据字节大小
            连接断开,回复TSFS_SERVER_EXIT
 */
int TSFS_Server::recvPacket(Packet** packet)
{
    //获取一个Packet
    *packet = getPacket();

    //接收请求数据包
    int ret = recvData(*packet);
    if( ret < 0 )
    {
        //接收数据出现异常,回收Packet
        recyclePacket(*packet);
    }
    
    return ret;
}

/*
 * @brief
 *   让Worker处理请求
 * @param packet[in]: 请求数据包
 * @param worker[in]: 处理请求的Worker
 */
void TSFS_Server::processData(Packet *packet, TSFS_Worker *worker)
{
    if(NULL != packet && NULL != worker)
    {
        //将请求数据包交给Worker处理
        worker->appendMsg(packet);
    }
}


/*
 * @brief
 *   获取Server运行状态标志
 * @return：Server运行状态标志
 */
bool TSFS_Server::bRuning()
{
    return this->m_runFlag;
}

/**
 * @brief
 *   将使用完毕Packet放入空Packet链表
 * @param pack[in]:   使用完毕的packet
 */
void TSFS_Server::recyclePacket(Packet *pack)
{
    if(NULL != pack)
    {
        //清空Packet
        memset(pack, 0, sizeof(Packet));

        m_packMutex.lock();

        //将使用完毕Packet放入空Packet链表
        m_emptyPacketList.push_back(pack);

        sysLoger(LDEBUG, "TSFS_Server::recyclePacket: recycle packet success");
    
        //释放信号量
        m_pPacketSemaphore->wakeup();
        
        m_packMutex.unlock();
    }
}

/**
 * @brief
 *   将空闲Worker放入空闲Worker链表
 * @param worker[in]:   空闲Worker
 */
void TSFS_Server::recycleWorker(TSFS_Worker *worker)
{
    if(NULL != worker)
    {
        m_workerMutex.lock();

        //将使用完毕Packet放入空Packet链表
        m_freeWorkerList.push_back(worker);

        sysLoger(LDEBUG, "TSFS_Server::recycleWorker: recycle worker success, [ThreadID = %d]",
                        worker->getThreadID() );
    
        //释放信号量
        m_pWorkerSemaphore->wakeup();
        
        m_workerMutex.unlock();
    }
}

