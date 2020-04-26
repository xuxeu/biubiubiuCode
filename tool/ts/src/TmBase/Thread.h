/*
* ������ʷ��
* 2006-11-30 qingxiaohai  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  Thread.h
* @brief
*       ���ܣ�
*       <li>�����߳���Ķ���</li>
*/


/************************ͷ�ļ�********************************/


#include "mutex.h"
#include "log.h"


#ifndef _THREAD_INCLUDE_
#define _THREAD_INCLUDE_

/************************�궨��********************************/
#define MAX_OB_NAME_LEN     50
#define NORMAL_QUEUE_SIZE   15
#define DEFAULT_PRIORITY        -20




/************************���Ͷ���******************************/



//�����߳���
class TS_Thread
{

public:

    enum Priority
    {
        IdlePriority,

        LowestPriority,
        LowPriority,
        NormalPriority,
        HighPriority,
        HighestPriority,

        TimeCriticalPriority,

        InheritPriority
    };

    TS_Thread(const char* name = NULL, unsigned size = 4096)
    {
        brunning = false;
        hthreadHanlde = NULL;
        uiThreadID = 0;
        /*pobject = NULL;*/
        stack_size = size;

        if(NULL == name)
        {
            strcpy_s(threadname, sizeof(threadname), "");
        }

        else
        {
            strcpy_s(threadname, sizeof(threadname), name);
        }

        hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
    }

    ~TS_Thread()
    {
        close();
    }

    inline HANDLE getThreadHanlde()
    {
        return hthreadHanlde;
    }
    inline unsigned getThreadID()
    {
        return uiThreadID;
    }
    inline char *getThreadName()
    {
        return threadname;
    }
    inline bool isRunning()
    {
        return brunning;
    }

    void start(TS_Thread::Priority = InheritPriority);
    void close();
    bool wait(unsigned long time = INFINITE);
    void sleep(unsigned long secs);
    void msleep(unsigned long msecs);
    void usleep(unsigned long usecs);
    virtual void run()=0;
    static unsigned _stdcall prerun(void *object);

private:
    void finished(void *object);

    bool brunning;                                           //����������
    static TS_Thread *pobject;                     //�̵߳ľ�̬����ָ��
    HANDLE hthreadHanlde;                           //�̵߳ľ��
    unsigned  uiThreadID;                                        //�̵߳ı�ʶ
    unsigned  stack_size;                                         //�̵߳�ջ�ռ�
    char threadname[MAX_OB_NAME_LEN];   //�̶߳��������
    HANDLE hSemaphore;                  //�ź���

};



/************************�ӿ�����******************************/

#endif /*_THREAD_INCLUDE_*/
