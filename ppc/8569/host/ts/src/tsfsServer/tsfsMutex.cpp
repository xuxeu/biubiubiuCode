/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  tsfsMutex.cpp
* @brief  
*       功能：
*       <li> 互斥类的实现</li>
*/
/************************头文件********************************/
#include "stdafx.h"
#include "stdlib.h"
#include "windows.h"
#include "tsfsMutex.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************全局变量******************************/

/************************模块变量******************************/

/************************外部声明******************************/

/************************前向声明******************************/

/************************实现*********************************/

TMutexPrivate::TMutexPrivate(SignalMode mode = Signaled)
    : owner(0), count(0), state(mode),
      event((mode == Signaled)?CreateEvent(0, false, false, 0):
                               CreateEvent(0, false, true, 0))
{
    
}

TMutexPrivate::~TMutexPrivate()
{ 
    CloseHandle(event); 
}

UL TMutexPrivate::self()
{ 
    return GetCurrentThreadId(); 
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
            state = Signaled;
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
    SetEvent(event); 
    owner = 0L;
    state = Signaled;
}


bool TMutex::lock(DWORD milliSec)
{
    bool ret = false;
    if(mtxpri->owner != mtxpri->self())
    {
        bool ret = mtxpri->wait(milliSec);        
        return ret; 
    }
    return true; //已经被自己锁定了，就不能再次锁定
}

void TMutex::lock()
{
    if(mtxpri->owner != mtxpri->self())
    {
        mtxpri->wait(INFINITE);  
    }
    return;//已经被自己锁定了，就不能再次锁定
}

void TMutex::unlock() 
{
    mtxpri->wakeUp();
}

bool TMutex::tryLock()
{
    if((mtxpri->owner == 0L) && (mtxpri->state == Signaled))
    {
        return true;
    }
    return false;
}

 /*
 构造器
 */
 waitcondition::waitcondition(long max)
{
    hSemaphore = CreateSemaphore(NULL,
                                 0,
                                 max,
                                 NULL);
    if(NULL == hSemaphore)
    {
        //sysLoger(LERROR, "waitcondtion created semaphore failure, errorNo:%d! FILE:%s, LINE:%d",
                                        //GetLastError(), __MYFILE__, __LINE__);
        return;
    }                                       
}

/*
析构器
*/
waitcondition::~waitcondition()
{
    CloseHandle(hSemaphore);
}

/**
 * @Funcname:  wakeup
 * @brief: 增加信号量的值,唤醒等待线程
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年12月5日 14:12:50
 *   
**/
void waitcondition::wakeup()
{
    if(!ReleaseSemaphore(hSemaphore, 1L, NULL))
    {
        //sysLoger(LWARNING, "ReleaseSemaphore failure! errorNo:%d, FILE:%s, LINE:%d",
                                            //GetLastError(), __MYFILE__, __LINE__);
    }    
}

/**
 * @Funcname: wait
 * @brief:等待信号量
 * @para1: millisec：等待超时的时间
 * @Author:  qingxiaohai
 * @History: 1.  Created this function on 2006年12月5日 14:17:59
**/
bool waitcondition::wait(DWORD millisec)
{
    DWORD dwWaitResult;  
    bool ret = false;
    
    millisec = (millisec >= 0)?millisec:0;

    dwWaitResult = WaitForSingleObject( hSemaphore,millisec);          
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
                                            //__MYFILE__, __LINE__);
            break;
            
        default:
            ret = false;
            //sysLoger(LWARNING, "wait for semaphore failed! FILE:%s, LINE:%d",
                                        //    __MYFILE__, __LINE__);
            break;
    }

    return ret;
}

