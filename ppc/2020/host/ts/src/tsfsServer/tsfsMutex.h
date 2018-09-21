/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  tsfsMutex.h
* @brief  
*       ���ܣ�
*       <li>������Ķ���</li>
*/
/************************ͷ�ļ�********************************/
#ifndef _MUTEX_INCLUDE_
#define _MUTEX_INCLUDE_

/************************�궨��********************************/
#define UC      unsigned char
#define US      unsigned short
#define UI      unsigned int
#define UL      unsigned long

#define DISABLE_COPY(Class) \
     Class(const Class &); \
     Class &operator=(const Class &);
     
#define MUTEX_WAIT_SECOND     3         //3//��λ��
#define MUTEX_WAIT_MILISEC     (MUTEX_WAIT_SECOND * 1000)     

/************************���Ͷ���******************************/
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

//�������ඨ��
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

//�������Ķ���
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

/*�����ȴ������Ķ���*/
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
