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


/*************************
 *TS_WaitCondition 类的实现*
 *************************/

/*
构造器
*/

TS_WaitCondition::TS_WaitCondition(long max)
{
    /*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [begin]*/
    /*  Modified brief: 解决先唤醒后等待时序*/
    waitSemaphoreCount = 0;
    wakeSemaphoreCount = 0;

    hQueueSemaphore = CreateSemaphore(NULL, 0, MAX_SEMAPHORE_NUM, NULL);
    /*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [end]*/

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
    /*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [begin]*/
    /*  Modified brief: 解决先唤醒后等待时序*/
    if(NULL == mutex || NULL == hQueueSemaphore)
    {
        return false;
    }

    bool ret = false;

    //记录等待信号量数量
    mtx.lock();
    ++waitSemaphoreCount;
    mtx.unlock();

    //解锁互斥器
    mutex->unlock();

    //超时等待信号量
    if(WAIT_OBJECT_0 == WaitForSingleObject(hQueueSemaphore, millisec))
    {
        //记录释放信号量数量
        mtx.lock();
        --wakeSemaphoreCount;
        mtx.unlock();

        ret = true;
    }

    //记录等待信号量数量
    mtx.lock();
    --waitSemaphoreCount;
    mtx.unlock();

    //加锁互斥器
    mutex->lock();

    return ret;
    /*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [end]*/
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

    /*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [begin]*/
    /*  Modified brief: 解决先唤醒后等待时序*/
    if(wakeSemaphoreCount < MAX_SEMAPHORE_NUM)
    {
        //释放信号量
        ReleaseSemaphore(hQueueSemaphore, 1L, NULL);

        //记录释放信号量数量
        ++wakeSemaphoreCount;
    }

    /*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [end]*/
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
    /*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [begin]*/
    /*  Modified brief: 解决先唤醒后等待时序*/
    //计算需要释放的信号量数量
    long releaseCount = (waitSemaphoreCount > 0) ? (long)waitSemaphoreCount : 1L;
    releaseCount = (releaseCount < (long)(MAX_SEMAPHORE_NUM - wakeSemaphoreCount))\
                   ? releaseCount : (long)(MAX_SEMAPHORE_NUM - wakeSemaphoreCount);

    if(releaseCount > 0)
    {
        //释放所有等待的信号量
        ReleaseSemaphore(hQueueSemaphore, releaseCount, NULL);

        //记录释放信号量数量
        wakeSemaphoreCount += releaseCount;
    }

    /*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [end]*/
}
