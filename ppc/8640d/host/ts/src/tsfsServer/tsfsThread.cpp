/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  tsfsThread.cpp
* @brief  
*       功能：
*       <li>基础线程类的实现</li>
*/

/************************头文件********************************/
#include "stdafx.h"
#include "stdlib.h"
#include "windows.h"
#include "process.h"
#include "tsfsThread.h"
#include "tsfsCommon.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************全局变量******************************/

/************************模块变量******************************/

/************************外部声明******************************/

/************************前向声明******************************/

/************************实现*********************************/

/**
 * @Funcname: start
 * @brief : 线程对象的启动函数
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年11月30日 11:36:16
 *   
**/
baseThread* baseThread::pobject = NULL;

void baseThread::start()
{
    //未启动则启动它，否则不再处理
    if(!brunning)
    {
        hthreadHanlde =(HANDLE)_beginthreadex(NULL,
                                              stack_size,
                                               &baseThread::prerun,//线程启动函数
                                               this,
                                               true,               //立即启动
                                               &uiThreadID);
        if(hthreadHanlde)
        {
            brunning = true;
            /*//sysLoger(LINFO, "Thread %s start running! FILE:%s, LINE:%d", threadname, 
                                                __FILE__, __LINE__);*/
        }        
        else
        {
            brunning = false;
            //sysLoger(LERROR, "Thread %s Init failed ! FILE:%s, LINE:%d", threadname, 
                                                //__MYFILE__, __LINE__);
        }
    }
    return;
}

/**
 * @Funcname: close
 * @brief : 线程的关闭函数
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年11月30日 11:42:36
 *   
**/
void baseThread::close( )
{
    //sysLoger(LINFO, "Thread %s stop running! FILE:%s, LINE:%d", threadname, 
                                           // __MYFILE__, __LINE__);
    _endthreadex(0);
    int closeStat = CloseHandle(hthreadHanlde);
        
}

/**
 * @Funcname: stop
 * @brief : 线程的停止函数
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年11月30日 11:42:36
 *   
**/
void baseThread::stop()
{
    brunning = false;  
    CloseHandle(hthreadHanlde);
}

/**
 * @Funcname: prerun
 * @brief : 线程对象的启动入口函数
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年11月30日 11:42:36
 *   
**/
UI baseThread::prerun(void *object)
{
    static_cast<baseThread*>(object)->run();

    return 0;
}

/**
 * @Funcname: appendMsg
 * @brief : 将消息加入到消息队列
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年11月30日 11:42:36
 *   
**/
UC threadWithQueue::appendMsg(Packet *pack)
{
    if(!msgQueueIsFull())
    {
        Msgmtx.lock();
        normalMsg.push(pack);
        Msgmtx.unlock();
        nrmmsg->wakeup();
        return 0;
    }

    //sysLoger(LWARNING, "Object msgQueue is full!FILE:%s, LINE:%d", __MYFILE__, __LINE__);

    return -1;
}

/**
 * @Funcname: getMsg
 * @brief : 从队列中取出消息
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年11月30日 11:42:36
 *   
**/
Packet* threadWithQueue::getMsg()
{
    if(!msgQueueIsEmpty())
    {
        Msgmtx.lock();
        Packet *pack = normalMsg.front();
        normalMsg.pop();
        Msgmtx.unlock();
        return pack;
    }    
    return NULL;
}

/*
 * @brief
 *   发送消息
 * @param pack[in]: 要发送的数据包
 * @return：收到数据包，则返回接收的数据包字节大小
 */
bool threadWithQueue::sendMsg(int serverAid, Packet *pack)
{
    return TClient_putpkt(  serverAid, 
                            pack->getDes_aid(), 
                            pack->getDes_said(), 
                            pack->getSrc_said(), 
                            pack->getData(), 
                            pack->getSize());
     
}

/**
 * @Funcname: run
 * @brief : 运行函数,从消息队列中取出消息进行处理
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年11月30日 11:42:36
 *   
**/
void threadWithQueue::run()
{
    Packet *pack = NULL;

    while(brun())
    {
        if(nrmmsg->wait())
        {
            pack = getMsg();
            if( NULL == pack )
            {
                continue;
            }
            process(pack);
        }
    }

    return;
}

/**
 * @Funcname: stopThread
 * @brief : 停止线程
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年11月30日 11:42:36
 *   
**/
void threadWithQueue::stopThread()
{
    this->stop();
    nrmmsg->wakeup();
}