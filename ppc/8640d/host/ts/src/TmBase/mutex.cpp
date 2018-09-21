/*
* ������ʷ��
* 2006-11-30 qingxiaohai  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  log.h
* @brief
*       ���ܣ�
*       <li>�������ʵ��</li>
*/



/************************ͷ�ļ�********************************/

#include "stdlib.h"
#include "windows.h"
#include "mutex.h"

/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/


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

    return true;      //�Ѿ����Լ������ˣ��Ͳ����ٴ�����
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

    return;      //�Ѿ����Լ������ˣ��Ͳ����ٴ�����
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
 *TS_WaitCondition ���ʵ��*
 *************************/

/*
������
*/

TS_WaitCondition::TS_WaitCondition(long max)
{
    /*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [begin]*/
    /*  Modified brief: ����Ȼ��Ѻ�ȴ�ʱ��*/
    waitSemaphoreCount = 0;
    wakeSemaphoreCount = 0;

    hQueueSemaphore = CreateSemaphore(NULL, 0, MAX_SEMAPHORE_NUM, NULL);
    /*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [end]*/

    hSemaphore = CreateSemaphore(NULL, 0, max, NULL);

    if(NULL == hSemaphore)
    {
        //sysLoger(LERROR, "waitcondtion created semaphore failure, errorNo:%d! FILE:%s, LINE:%d",
        //                                GetLastError(), __MYFILE__, __LINE__);
        return;
    }
}

/*
������
*/
TS_WaitCondition::~TS_WaitCondition()
{
    ReleaseSemaphore(hSemaphore, 1L, NULL);
    CloseHandle(hSemaphore);
}

/**
 * @Funcname:  wakeup
 * @brief        :  �����ź�����ֵ,���ѵȴ��߳�
 * @para1      :
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006��12��5�� 14:12:50
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
 * @brief        :  �ȴ��ź���
 * @para1      : millisec���ȴ���ʱ��ʱ��
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006��12��5�� 14:17:59
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
 * @brief        :  ��ʱ�ȴ��ź��������������
 * @para1      : millisec���ȴ���ʱ��ʱ��
 * @return      :
 * @Author     :  tangxp
 *
 * @History: 1.  Created this function on 2008.1.10
**/
bool TS_WaitCondition::wait(TS_Mutex *mutex, unsigned long millisec)
{
    /*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [begin]*/
    /*  Modified brief: ����Ȼ��Ѻ�ȴ�ʱ��*/
    if(NULL == mutex || NULL == hQueueSemaphore)
    {
        return false;
    }

    bool ret = false;

    //��¼�ȴ��ź�������
    mtx.lock();
    ++waitSemaphoreCount;
    mtx.unlock();

    //����������
    mutex->unlock();

    //��ʱ�ȴ��ź���
    if(WAIT_OBJECT_0 == WaitForSingleObject(hQueueSemaphore, millisec))
    {
        //��¼�ͷ��ź�������
        mtx.lock();
        --wakeSemaphoreCount;
        mtx.unlock();

        ret = true;
    }

    //��¼�ȴ��ź�������
    mtx.lock();
    --waitSemaphoreCount;
    mtx.unlock();

    //����������
    mutex->lock();

    return ret;
    /*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [end]*/
}

/**
 * @Funcname:  wakeOne
 * @brief        :  �ͷ�һ�������������ĵȴ��ź���
 * @return      :
 * @Author     :  tangxp
 *
 * @History: 1.  Created this function on 2008.1.10
**/
void TS_WaitCondition::wakeOne()
{
    TS_MutexLocker locker(&mtx);

    /*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [begin]*/
    /*  Modified brief: ����Ȼ��Ѻ�ȴ�ʱ��*/
    if(wakeSemaphoreCount < MAX_SEMAPHORE_NUM)
    {
        //�ͷ��ź���
        ReleaseSemaphore(hQueueSemaphore, 1L, NULL);

        //��¼�ͷ��ź�������
        ++wakeSemaphoreCount;
    }

    /*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [end]*/
}
/**
 * @Funcname:  wakeAll
 * @brief        :  �ͷ����м����������ĵȴ��ź���
 * @return      :
 * @Author     :  tangxp
 *
 * @History: 1.  Created this function on 2008.1.10
**/
void TS_WaitCondition::wakeAll()
{
    TS_MutexLocker locker(&mtx);
    /*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [begin]*/
    /*  Modified brief: ����Ȼ��Ѻ�ȴ�ʱ��*/
    //������Ҫ�ͷŵ��ź�������
    long releaseCount = (waitSemaphoreCount > 0) ? (long)waitSemaphoreCount : 1L;
    releaseCount = (releaseCount < (long)(MAX_SEMAPHORE_NUM - wakeSemaphoreCount))\
                   ? releaseCount : (long)(MAX_SEMAPHORE_NUM - wakeSemaphoreCount);

    if(releaseCount > 0)
    {
        //�ͷ����еȴ����ź���
        ReleaseSemaphore(hQueueSemaphore, releaseCount, NULL);

        //��¼�ͷ��ź�������
        wakeSemaphoreCount += releaseCount;
    }

    /*Modified by tangxp for BUG NO.3244 on 2008��5��22�� [end]*/
}
