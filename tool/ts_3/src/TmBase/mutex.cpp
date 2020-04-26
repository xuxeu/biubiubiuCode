/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  mutex.cpp
 * @Version        :  1.0.0
 * @Brief           :  互斥类的实现
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2006年11月30日 9:15:31
 * @History        : 
 *   
 */




/************************引用部分*****************************/
//#include "stdafx.h"
#include "stdlib.h"
#include "windows.h"
#include "mutex.h"
//#include "log.h"

/************************前向声明部分***********************/

/************************定义部分*****************************/

/************************实现部分****************************/


TMutexPrivate::TMutexPrivate(SignalMode mode = Signaled)
    : owner(0), count(0), state(mode),
      event((mode == Signaled)?CreateEvent(0, false, true, 0):
                               CreateEvent(0, false, false, 0))
{
    
}
TMutexPrivate::~TMutexPrivate()
{ 
    CloseHandle(event); 
}

UL TMutexPrivate::self()
{     
    DWORD threadId = 0;
    try
    {
        threadId = GetCurrentThreadId(); 
    }
    catch(...)
    {
        printf("TMutexPrivate::self: get current thread id failed\n"
                );
        threadId = 0L;
    }
    return threadId;
}

bool TMutexPrivate::wait(DWORD dwMilliseconds)
{ 
    bool ret = false;
    DWORD waitret = WaitForSingleObject(event, dwMilliseconds);
    switch(waitret)
    {
        case WAIT_OBJECT_0:
            ret = true;
            owner = self();
            state = NonSignaled;
            if(0 == owner )
            {
                printf("TMutexPrivate::wait: get thread id fialed!\n");
            }
            break;
            
        case WAIT_ABANDONED:                
           // break;                
        case WAIT_TIMEOUT:                
            //break;
        case WAIT_FAILED:
            //break;                
        default:     
            ret = false;
            break;
    }
    
    return ret; 
}

void TMutexPrivate::wakeUp()
{ 
    try
    {
        SetEvent(event); 
        owner = 0L;
        state = Signaled; 
    }
    catch(...)
    {
        printf("TMutexPrivate::wakeUp: failed!\n");
    }
}


bool TS_Mutex::lock(DWORD milliSec)
{
    DWORD threadId = mtxpri->self();
    if(0 == threadId)
    {
        printf("TS_Mutex::lock:: get thread id failed!\n");
        threadId = 100;
    }
    
    if(mtxpri && (mtxpri->owner != mtxpri->self()))
    {
        bool ret = mtxpri->wait(milliSec);        
        return ret; 
    }
    return true;      //已经被自己锁定了，就不能再次锁定
}

void TS_Mutex::lock()
{
    DWORD threadId = mtxpri->self();
    if(0 == threadId)
    {
        printf("TS_Mutex::lock:: get thread id failed!\n");
        threadId = 100;
    }
    
    if(mtxpri && (mtxpri->owner != mtxpri->self()))
    {
        mtxpri->wait(INFINITE);  
    }
    return;      //已经被自己锁定了，就不能再次锁定
}

void TS_Mutex::unlock() 
{
    if(mtxpri)
    {
        mtxpri->wakeUp();
    }    
}

bool TS_Mutex::tryLock()
{
    if(mtxpri && (mtxpri->owner == 0L) && (mtxpri->state == Signaled))
    {
        return true;
    }
    return false;
}


/*************************
 *TS_WaitCondition 类的实现*
 *************************/

 /*
 构造器
 */

TS_WaitCondition::TS_WaitCondition(long max)
{
    hSemaphore = CreateSemaphore(NULL, 0, max, NULL);
    if(NULL == hSemaphore)
    {
        //sysLoger(LERROR, "waitcondtion created semaphore failure, errorNo:%d! FILE:%s, LINE:%d",
        //                                GetLastError(), __MYFILE__, __LINE__);
        return;
    }                                                  
}

/*
析构器
*/
TS_WaitCondition::~TS_WaitCondition()
{
    ReleaseSemaphore(hSemaphore, 1L, NULL);
    CloseHandle(hSemaphore);
}

/**
 * @Funcname:  wakeup
 * @brief        :  增加信号量的值,唤醒等待线程
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月5日 14:12:50
 *   
**/
void TS_WaitCondition::wakeup()
{
    
    if(!ReleaseSemaphore(hSemaphore, 1L, NULL))
    {
        //sysLoger(LWARNING, "ReleaseSemaphore failure! errorNo:%d, FILE:%s, LINE:%d",
        //                                    GetLastError(), __MYFILE__, __LINE__);
    }    
}

/**
 * @Funcname:  wait
 * @brief        :  等待信号量
 * @para1      : millisec：等待超时的时间
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月5日 14:17:59
 *   
**/
bool TS_WaitCondition::wait(DWORD millisec)
{
    DWORD dwWaitResult;  
    bool ret = false;
    
    millisec = (millisec >= 0)?millisec:0;

    dwWaitResult = WaitForSingleObject(hSemaphore, millisec);
    switch (dwWaitResult) 
    { 
        // The semaphore object was signaled.
        case WAIT_OBJECT_0: 
            ret = true;
            break; 

        // Semaphore was nonsignaled, so a time-out occurred.
        case WAIT_TIMEOUT: 
            ret = false;
            //sysLoger(LWARNING, "wait for semaphore timeout! FILE:%s, LINE:%d",
            //                                __MYFILE__, __LINE__);
            break;
            
        default:
            ret = false;
            //sysLoger(LWARNING, "wait for semaphore failed! FILE:%s, LINE:%d",
            //                                __MYFILE__, __LINE__);
            break;
    }

    return ret;
}

/**
 * @Funcname:  wait
 * @brief        :  超时等待信号量后加锁互斥器
 * @para1      : millisec：等待超时的时间
 * @return      : 
 * @Author     :  tangxp
 *  
 * @History: 1.  Created this function on 2008.1.10
**/
bool TS_WaitCondition::wait(TS_Mutex *mutex, unsigned long millisec)
{
    if(NULL == mutex)
    {
        return false;
    }
    bool ret = false;

   mtx.lock();

    //申请一个信号量
    waitObject waitObj;
    
    waitObj.hEvent= CreateEvent(0, false, false, 0);//CreateSemaphore(NULL, 0, 1, NULL);
    if(NULL == waitObj.hEvent)
    {
        mtx.unlock();
        return false;
    }

    //设置信号量优先级
    waitObj.eventPriority= GetThreadPriority(GetCurrentThread());

    //将等待信号量插入队列
    waitObjectQueue::iterator iter;    
    for(iter = queue.begin(); iter != queue.end(); ++iter)
    {
        if(waitObj.eventPriority> (*iter)->eventPriority)
        {
            break;
        }
    }
    queue.insert(iter, &waitObj);
    
    mtx.unlock();

    //解锁互斥器
    mutex->unlock();

    //超时等待信号量
    if(WAIT_OBJECT_0 == WaitForSingleObject(waitObj.hEvent, millisec))
    {
        ret = true;
    }

    //加锁互斥器
    mutex->lock();

    //从队列删除信号量
    mtx.lock();
    
    queue.remove(&waitObj);

    CloseHandle(waitObj.hEvent);

    mtx.unlock();

    return ret;
}

/**
 * @Funcname:  wakeOne
 * @brief        :  释放一个加锁互斥器的等待信号量
 * @return      : 
 * @Author     :  tangxp
 *  
 * @History: 1.  Created this function on 2008.1.10
**/
void TS_WaitCondition::wakeOne()
{
    TS_MutexLocker locker(&mtx);
    if(queue.empty())
    {
        return;
    }

    //释放信号量
    SetEvent(queue.front()->hEvent);
    //ReleaseSemaphore(queue.front()->hSemphore, 1L, NULL);
}
/**
 * @Funcname:  wakeAll
 * @brief        :  释放所有加锁互斥器的等待信号量
 * @return      : 
 * @Author     :  tangxp
 *  
 * @History: 1.  Created this function on 2008.1.10
**/
void TS_WaitCondition::wakeAll()
{
    TS_MutexLocker locker(&mtx);
    if(queue.empty())
    {
        return;
    }

    //释放所有信号量
    waitObjectQueue::iterator iter;    
    for(iter = queue.begin(); iter != queue.end(); ++iter)
    {
        SetEvent((*iter)->hEvent);
        //ReleaseSemaphore((*iter)->hSemphore, 1L, NULL);
    }
}
