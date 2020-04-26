/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  mutex.h
 * @Version        :  1.0.0
 * @Brief           :  ������Ķ���
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2006��11��30�� 8:58:37
 * @History        : 
 *   
 */

#ifndef _MUTEX_INCLUDE_
#define _MUTEX_INCLUDE_

#define UC  unsigned char
#define UL  unsigned long
#define US  unsigned short
#define UI  unsigned int

/************************���ò���*****************************/

#include "windows.h"
#include <list>

using namespace std;

/************************ǰ����������***********************/
#define DISABLE_COPY(Class) \
     Class(const Class &); \
     Class &operator=(const Class &);
     
#define MUTEX_WAIT_SECOND     3         //3//��λ��
#define MUTEX_WAIT_MILISEC     (MUTEX_WAIT_SECOND * 1000)     

//enum _SignalMode
//{ 
//    NonSignaled,
//    Signaled 
//};
typedef enum{NonSignaled, Signaled} SignalMode;

typedef struct _waitObject
{
    HANDLE hEvent;
    int eventPriority;
}waitObject;

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

//�������ඨ��
class  TS_Mutex
{
public:
    
    inline explicit TS_Mutex(SignalMode mode = Signaled) 
        {
            mtxpri = new TMutexPrivate(mode); 
        }
    inline ~TS_Mutex() 
        {
            delete mtxpri;
        }

    bool lock(DWORD milliSec);
    void lock();
    bool tryLock();
    void unlock();


private:
    TMutexPrivate *mtxpri;
    DISABLE_COPY(TS_Mutex)
};


//�������Ķ���
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
*�����ȴ������Ķ���*
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
    TS_Mutex mtx;                       //�ź������л���
    waitObjectQueue queue;      //�ź�����

};
/************************���岿��*****************************/

/************************ʵ�ֲ���****************************/

#endif /*_MUTEX_INCLUDE_*/
