/*
* ������ʷ��
* 2006-11-30 qingxiaohai  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  log.h
* @brief
*       ���ܣ�
*       <li>������Ķ���</li>
*/



#ifndef _MUTEX_INCLUDE_
#define _MUTEX_INCLUDE_

/************************ͷ�ļ�********************************/

#include "windows.h"
#include <list>

using namespace std;

/************************�궨��********************************/
#define UC  unsigned char
#define UL  unsigned long
#define US  unsigned short
#define UI  unsigned int

/*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [begin]*/
/*  Modified brief: �ź��������������,���ڻ�������ˮλ*/
#define MAX_SEMAPHORE_NUM 512

/*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [end]*/


#define DISABLE_COPY(Class) \
     Class(const Class &); \
     Class &operator=(const Class &);

#define MUTEX_WAIT_SECOND     3         //3//��λ��
#define MUTEX_WAIT_MILISEC     (MUTEX_WAIT_SECOND * 1000)

/************************���Ͷ���******************************/
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

//�������ඨ��
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

    /*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [begin]*/
    /*  Modified brief: ����Ȼ��Ѻ�ȴ�ʱ��*/
    HANDLE hQueueSemaphore;        //�ź������о��
    int waitSemaphoreCount;        //�ȴ��ź�������
    int wakeSemaphoreCount;         //���ͷ��ź�������
    /*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [end]*/

};
/************************�ӿ�����******************************/

#endif /*_MUTEX_INCLUDE_*/
