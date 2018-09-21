/*
* 更改历史：
* 2006-11-30 qingxiaohai  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  log.h
* @brief
*       功能：
*       <li>互斥类的定义</li>
*/



#ifndef _MUTEX_INCLUDE_
#define _MUTEX_INCLUDE_

/************************头文件********************************/

#include "windows.h"
#include <list>

using namespace std;

/************************宏定义********************************/
#define UC  unsigned char
#define UL  unsigned long
#define US  unsigned short
#define UI  unsigned int

/*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [begin]*/
/*  Modified brief: 信号量队列最大数量,等于缓冲包最高水位*/
#define MAX_SEMAPHORE_NUM 512

/*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [end]*/


#define DISABLE_COPY(Class) \
     Class(const Class &); \
     Class &operator=(const Class &);

#define MUTEX_WAIT_SECOND     3         //3//单位秒
#define MUTEX_WAIT_MILISEC     (MUTEX_WAIT_SECOND * 1000)

/************************类型定义******************************/
typedef enum {NonSignaled, Signaled} SignalMode;

typedef struct _waitObject
{
    HANDLE hEvent;
    int eventPriority;
} waitObject;

typedef list<waitObject*> waitObjectQueue;

class TMutexPrivate
{
public:
    TMutexPrivate(SignalMode mode);
    ~TMutexPrivate();

    UL self();
    bool wait(UL dwMilliseconds);
    void wakeUp();

    //QAtomic lock;
    UL owner;
    UI count;

    SignalMode state;
private:
    HANDLE event;
};

//互斥器类定义
class  TS_Mutex
{
public:

    inline explicit TS_Mutex(SignalMode mode = Signaled):mtxpri(mode)
    {

    }
    inline ~TS_Mutex()
    {

    }

    bool lock(DWORD milliSec);
    void lock();
    bool tryLock();
    void unlock();


private:
    TMutexPrivate mtxpri;
    DISABLE_COPY(TS_Mutex)
};


//互斥锁的定义
class TS_MutexLocker
{
public:

    TS_MutexLocker(TS_Mutex *m, DWORD time = INFINITE):mtx(m)
    {
        if(mtx != NULL)
        {
            mtx->lock(time);
            lockflag = true;
        }

        else
        {
            lockflag = false;
        }

    }
    ~TS_MutexLocker()
    {
        if((mtx != NULL) && (lockflag))
        {
            mtx->unlock();
        }
    }

    TS_Mutex* getMutex()
    {
        return mtx;
    }


private:

    TS_Mutex *mtx;
    bool        lockflag;
    DISABLE_COPY(TS_MutexLocker)
};

/****************************
*条件等待变量的定义*
*****************************/

class TS_WaitCondition
{
public:
    TS_WaitCondition(long max = 65535);
    ~TS_WaitCondition();

    bool wait(DWORD millisec = INFINITE);
    bool wait(TS_Mutex *mutex, unsigned long millisec = INFINITE);
    void wakeOne() ;
    void wakeAll() ;
    void wakeup();

private:
    HANDLE hSemaphore;
    TS_Mutex mtx;                       //信号量队列互斥

    /*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [begin]*/
    /*  Modified brief: 解决先唤醒后等待时序*/
    HANDLE hQueueSemaphore;        //信号量队列句柄
    int waitSemaphoreCount;        //等待信号量数量
    int wakeSemaphoreCount;         //已释放信号量数量
    /*Modified by tangxp for BUG NO.3244 on 2008年5月22日 [end]*/

};
/************************接口声明******************************/

#endif /*_MUTEX_INCLUDE_*/
