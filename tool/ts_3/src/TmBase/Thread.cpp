/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  Thread.cpp
 * @Version        :  1.0.0
 * @Brief           :  基础线程类的实现
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2006年11月30日 11:05:53
 * @History        : 
 *   
 */




/************************引用部分*****************************/

#include "stdlib.h"
#include "windows.h"
#include "process.h"
#include "Thread.h"

/************************前向声明部分***********************/

/************************定义部分*****************************/

/************************实现部分****************************/

//TS_Thread::TS_Thread()
//{
//    brunning = false;
//    hthreadHanlde = NULL;
//    uiThreadID = 0;
//    stack_size = 0;
//    pobject = NULL;
//}
//
//TS_Thread::TS_Thread(const char* name, unsigned size)
//{
//    stack_size = size;
//    strcpy_s(threadname, sizeof(threadname), name);
//}
/**
 * @Funcname:  start
 * @brief        :  线程对象的启动函数
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年11月30日 11:36:16
 *   
**/
TS_Thread* TS_Thread::pobject = NULL;

void TS_Thread::start(Priority Priority)
{
    //TS_MutexLocker locker(&threadMutex);
    
    //未启动则启动它，否则不再处理
    if(brunning)
    {      
        return;
    }

    hthreadHanlde =(HANDLE)_beginthreadex(NULL,
                                      stack_size,
                                      &TS_Thread::prerun,            //线程启动函数
                                      this,
                                      CREATE_SUSPENDED ,          //线程暂停
                                      &uiThreadID);
    if(!hthreadHanlde)
    {                
        sysLoger(LERROR, "Thread %s Init failed!", threadname);
        brunning = false;
        return;
    }        

    brunning = true;

    int prio = THREAD_PRIORITY_NORMAL;                    //线程优先级

    switch(Priority)
    {
        case IdlePriority:
            prio = THREAD_PRIORITY_IDLE;
            break;

        case LowestPriority:
            prio = THREAD_PRIORITY_LOWEST;
            break;

        case LowPriority:
            prio = THREAD_PRIORITY_BELOW_NORMAL;
            break;

        case NormalPriority:
            prio = THREAD_PRIORITY_NORMAL;
            break;

        case HighPriority:
            prio = THREAD_PRIORITY_ABOVE_NORMAL;
            break;

        case HighestPriority:
            prio = THREAD_PRIORITY_HIGHEST;
            break;

        case TimeCriticalPriority:
            prio = THREAD_PRIORITY_TIME_CRITICAL;
            break;

        case InheritPriority:
        default:
            prio = GetThreadPriority(hthreadHanlde);
            break;
    }

    SetThreadPriority(hthreadHanlde, prio);

    ResumeThread(hthreadHanlde);            
#ifdef IS_LOG
    sysLoger(LDEBUG, "Thread %s start running! ID:%d", threadname, 
                                        uiThreadID);
#endif
    return;
}

/**
 * @Funcname:  close
 * @brief        :  线程的关闭函数
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年11月30日 11:42:36
 *   
**/
void TS_Thread::close( )
{
    //TS_MutexLocker locker(&threadMutex);
    
    if(hthreadHanlde)
    {   
        bool ret = false;
        if(brunning)
        {
            ret = TerminateThread(hthreadHanlde, 0);
#ifdef IS_LOG
            sysLoger(LDEBUG, "Thread %s Terminated! ID:%d, ret:%d", threadname, 
                                        uiThreadID, ret);
#endif
        }
        if(hthreadHanlde)
        {
            ret = CloseHandle(hthreadHanlde);
            hthreadHanlde = NULL;
#ifdef IS_LOG
            sysLoger(LDEBUG, "Thread %s stop running! ID:%d, ret:%d", threadname, 
                                            uiThreadID, ret);
#endif         
        }
    }
}

/*
 * @Funcname:  prerun
 * @brief        :  线程已经退出
 * @return      : 
 * @Author     :  tangxingpei
 *  
 * @History: 1.  Created this function on 2007年1月2日
 *   
*/
void TS_Thread::finished(void *object)
{
    if(NULL == object)
    {
        return;
    }
    
    TS_Thread* pThread =  static_cast<TS_Thread*>(object);
    
    //pThread->threadMutex.lock();    

    if(pThread->hSemaphore)
    {
        ReleaseSemaphore(pThread->hSemaphore, 1L, NULL);
    }

    pThread->brunning = false;

    //pThread->threadMutex.unlock();
}

/**
 * @Funcname:  prerun
 * @brief        :  线程对象的启动入口函数
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年11月30日 11:48:59
 *   
**/
unsigned TS_Thread::prerun(void *object)
{   
    static_cast<TS_Thread*>(object)->run(); 
    
    static_cast<TS_Thread*>(object)->finished(object);
    
    return 0;
}

/**
 * @Funcname:  prerun
 * @brief        :  等待线程退出
 * @return      : 
 * @Author     :  tangxingpei
 *  
 * @History: 1.  Created this function on 2007年1月2日
 *   
**/
bool TS_Thread::wait(unsigned long time)
{    
#ifdef IS_LOG
    sysLoger(LDEBUG, "Thread %s waiting single object! ID:%d", threadname, 
                                        uiThreadID);
#endif

    if(brunning && (WAIT_OBJECT_0 == WaitForSingleObject(hSemaphore, time)))
    {
#ifdef IS_LOG
        sysLoger(LDEBUG, "Thread %s get single object! ID:%d", threadname, 
                                        uiThreadID);
#endif
        return true;     
    }
#ifdef IS_LOG
    sysLoger(LDEBUG, "Thread %s do not running or tiem out! ID:%d, isRuning:%d", threadname, 
                                uiThreadID, brunning);
#endif
    return false;
}

/**
 * @Funcname:  prerun
 * @brief        :  休眠secs秒
 * @return      : 
 * @Author     :  tangxingpei
 *  
 * @History: 1.  Created this function on 2007年1月4日
 *   
**/
void TS_Thread::sleep(unsigned long secs)
{
    Sleep(secs * 1000);
}

/**
 * @Funcname:  prerun
 * @brief        :  休眠msecs毫秒
 * @return      : 
 * @Author     :  tangxingpei
 *  
 * @History: 1.  Created this function on 2007年1月4日
 *   
**/
void TS_Thread::msleep(unsigned long msecs)
{
    Sleep(msecs);
}

/**
 * @Funcname:  prerun
 * @brief        :  休眠usecs微秒
 * @return      : 
 * @Author     :  tangxingpei
 *  
 * @History: 1.  Created this function on 2007年1月4日
 *   
**/
void TS_Thread::usleep(unsigned long usecs)
{
    Sleep((usecs / 1000) + 1);
}


///******************threadWithQueue*****************************/
///**
// * @Funcname:  appendMsg
// * @brief        :  将消息加入到消息队列
// * @return      : 
// * @Author     :  qingxiaohai
// *  
// * @History: 1.  Created this function on 2006年12月1日 11:16:02
// *   
//**/
//unsigned char threadWithQueue::appendMsg(Packet *pack)
//{
//    if(!msgQueueIsFull())
//    {
//        Msgmtx.lock();
//        normalMsg.push(pack);
//        Msgmtx.unlock();
//        nrmmsg->wakeup();
//        return 0;
//    }
//    //sysLoger(LWARNING, "Object msgQueue is full!FILE:%s, LINE:%d", __MYFILE__, __LINE__);
//    return -1;
//}
//
///**
// * @Funcname:  getMsg
// * @brief        :  从队列中取出消息
// * @return      : 
// * @Author     :  qingxiaohai
// *  
// * @History: 1.  Created this function on 2006年12月1日 11:23:44
// *   
//**/
//Packet* threadWithQueue::getMsg()
//{
//    //Packet* tmpmsg;
//    if(!msgQueueIsEmpty())
//    {
//        Msgmtx.lock();
//        Packet *pack = normalMsg.front();
//        normalMsg.pop();
//        Msgmtx.unlock();
//        return pack;
//    }    
//    return NULL;
//}
//
///*
// * @brief
// *   发送消息
// * @param pack[in]: 要发送的数据包
// * @return：收到数据包，则返回接收的数据包字节大小
// */
//bool threadWithQueue::sendMsg(int serverAid, Packet *pack)
//{
//    return TClient_putpkt(  serverAid, 
//                            pack->getDes_aid(), 
//                            pack->getDes_said(), 
//                            pack->getSrc_said(), 
//                            pack->getData(), 
//                            pack->getSize());
//     
//}
//
///**
// * @Funcname:  threadWithQueue::run()
// * @brief        :  运行函数,从消息队列中取出消息进行处理
// * @return      : 
// * @Author     :  qingxiaohai
// *  
// * @History: 1.  Created this function on 2006年12月1日 11:28:50
// *   
//**/
//void threadWithQueue::run()
//{
//    Packet *pack = NULL;
//    while(brun())
//    {
//        if(nrmmsg->wait())
//        {
//            pack = getMsg();
//            if( NULL == pack )
//            {
//                continue;
//            }
//            process(pack);
//        }
//    }
//    return;
//}
//
///**
// * @Funcname:  threadWithQueue::stopThread()
// * @brief        :  停止线程
// * @return      : 
// * @Author     :  tangxp
// *  
// * @History: 1.  Created this function on 2007年11月12日 11:28:50
// *   
//**/
//void threadWithQueue::stopThread()
//{
//    this->stop();
//    nrmmsg->wakeup();
//}


/*******************    JUST FOR TEST   ****************************/
//测试类
#ifdef THREAD_CLASS_TEST
void TS_Thread::run()
{
    static int idx = 0;
    while(brunning && ++idx)
    {
        /*printf("\nThis is thread %s running! %d", threadname, idx);*/
        printf("\nThis is thread %s running! %d,  FILE:%s, LINE:%d", threadname, idx, 
                                __MYFILE__, __LINE__);
        //sysLoger(LINFO, "This is thread %s running! %d,  FILE:%s, LINE:%d", threadname, idx, 
                                __MYFILE__, __LINE__);
        //if(!brunning)
            //close();
    }
    //sysLoger(LINFO, "Thread %s stop running! FILE:%s, LINE:%d", threadname, 
                                                __MYFILE__, __LINE__);
    return;
}

#endif
/**
 * @Funcname:  threadWithQueue::run
 * @brief        :  带消息队列的线程的运行函数,这里是测试函数
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月1日 10:55:58
 *   
**/
#if 0
void threadWithQueue::process(Packet & pack)
{
    char buf[1024] = "\0";

    sprintf_s(buf, sizeof(buf), "msgname:%d, src_aid=%d,des_aid=%d,msgsize=%d,"\
                                             "DID=%d, msg is:%s", pack.getPackName(), pack.getSrc_aid(), 
                                             pack.getDes_aid(), pack.getSize(), pack.getSerialNum(), 
                                             pack.getData());
    //sysLoger(LINFO, buf);

    return;
}

#endif 
