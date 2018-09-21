/*
* 更改历史：
* 2006-11-30 qingxiaohai  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  Thread.h
* @brief
*       功能：
*       <li>基础线程类的定义</li>
*/


/************************头文件********************************/


#include "mutex.h"
#include "log.h"


#ifndef _THREAD_INCLUDE_
#define _THREAD_INCLUDE_

/************************宏定义********************************/
#define MAX_OB_NAME_LEN     50
#define NORMAL_QUEUE_SIZE   15
#define DEFAULT_PRIORITY        -20




/************************类型定义******************************/



//基础线程类
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

    bool brunning;                                           //是滞在运行
    static TS_Thread *pobject;                     //线程的静态对象指针
    HANDLE hthreadHanlde;                           //线程的句柄
    unsigned  uiThreadID;                                        //线程的标识
    unsigned  stack_size;                                         //线程的栈空间
    char threadname[MAX_OB_NAME_LEN];   //线程对象的名字
    HANDLE hSemaphore;                  //信号量

};



/************************接口声明******************************/

#endif /*_THREAD_INCLUDE_*/
