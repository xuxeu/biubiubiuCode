/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  tsfsMutex.h
* @brief  
*       功能：
*       <li>互斥类的定义</li>
*/
/************************头文件********************************/
#ifndef _MUTEX_INCLUDE_
#define _MUTEX_INCLUDE_

/************************宏定义********************************/
#define UC      unsigned char
#define US      unsigned short
#define UI      unsigned int
#define UL      unsigned long

#define DISABLE_COPY(Class) \
     Class(const Class &); \
     Class &operator=(const Class &);
     
#define MUTEX_WAIT_SECOND     3         //3//单位秒
#define MUTEX_WAIT_MILISEC     (MUTEX_WAIT_SECOND * 1000)     

/************************类型定义******************************/
typedef enum
{
	NonSignaled, 
	Signaled
} SignalMode;

class TMutexPrivate 
{
public:
    TMutexPrivate(SignalMode mode);
    ~TMutexPrivate();

    UL self();
    bool wait(UL dwMilliseconds);
    void wakeUp();
    UL owner;
    UI count;
    bool state;
private:
    HANDLE event;
};

//互斥器类定义
class  TMutex
{
public:
    
    inline explicit TMutex(SignalMode mode = NonSignaled) 
	{
		mtxpri = new TMutexPrivate(mode); 
	}

    inline ~TMutex() 
	{
		delete mtxpri;
	}

    bool lock(DWORD milliSec);
    void lock();
    bool tryLock();
    void unlock();
private:
    TMutexPrivate *mtxpri;
    DISABLE_COPY(TMutex)
};

//互斥锁的定义
class TMutexLocker
{
public:

    TMutexLocker(TMutex *m, DWORD time = INFINITE):mtx(m)
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

    ~TMutexLocker()
    {
        if((mtx != NULL) && (lockflag))
        {
            mtx->unlock();
        }
    }

    TMutex* getMutex()
    {
        return mtx;
    }
private:
    TMutex *mtx;
    bool        lockflag;
    DISABLE_COPY(TMutexLocker)
};

/*条件等待变量的定义*/
class waitcondition
{
public:
    waitcondition(long max = 1);
    ~waitcondition();

    bool wait(DWORD millisec = INFINITE);
    void wakeup();

private:
    HANDLE hSemaphore;
    
};
#endif /*_MUTEX_INCLUDE_*/
