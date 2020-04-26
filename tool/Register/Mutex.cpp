/*
* 更改历史：
* 2006-11-30 qingxiaohai  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  log.h
* @brief
*       功能：
*       <li>互斥类的实现</li>
*/



/************************头文件********************************/
#include "stdafx.h"
#include "stdlib.h"
#include "windows.h"
#include "mutex.h"

/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/


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
    DWORD threadId = mtxpri.self();

    if(0 == threadId)
    {
        printf("TS_Mutex::lock:: get thread id failed!\n");
        threadId = 100;
    }

    if((mtxpri.owner != mtxpri.self()))
    {
        bool ret = mtxpri.wait(milliSec);
        return ret;
    }

    return true;      //已经被自己锁定了，就不能再次锁定
}

void TS_Mutex::lock()
{
    DWORD threadId = mtxpri.self();

    if(0 == threadId)
    {
        printf("TS_Mutex::lock:: get thread id failed!\n");
        threadId = 100;
    }

    if((mtxpri.owner != mtxpri.self()))
    {
        mtxpri.wait(INFINITE);
    }

    return;      //已经被自己锁定了，就不能再次锁定
}

void TS_Mutex::unlock()
{
    mtxpri.wakeUp();
}

bool TS_Mutex::tryLock()
{
    if((mtxpri.owner == 0L) && (mtxpri.state == Signaled))
    {
        return true;
    }

    return false;
}

