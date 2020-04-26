/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/


/**
 * @filename      :  Thread.h
 * @Version        :  1.0.0
 * @Brief           :  基础线程类的定义
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2006年11月30日 11:06:44
 * @History        : 
 *   
 */

#ifndef _THREAD_INCLUDE_
#define _THREAD_INCLUDE_

#define MAX_OB_NAME_LEN     50
#define NORMAL_QUEUE_SIZE   15
#define DEFAULT_PRIORITY        -20

//#define IS_LOG
/************************引用部分*****************************/

//#include <queue>
//#include "stdlib.h"
//#include "windows.h"
#include "mutex.h"
#include "log.h"

//using namespace std;
/************************前向声明部分***********************/


/************************定义部分*****************************/



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
    //TS_Mutex threadMutex;           //互斥锁
};

/************************发消息线程*************************/



///************************前向声明部分***********************/
//class threadWithQueue:public TS_Thread
//{
//public:
//    
//    threadWithQueue(US size = NORMAL_QUEUE_SIZE)
//    {
//        maxqueuesize = size;
//        nrmmsg = new TS_WaitCondition(size);
//    }
//    ~threadWithQueue()
//    {
//        for(int i = 0; i < normalMsg.size(); i++)
//        {
//            normalMsg.pop();
//        }
//        delete nrmmsg;
//    }
//
//    bool msgQueueIsFull()
//    {
//        return (normalMsg.size() >= maxqueuesize);
//    }
//    bool msgQueueIsEmpty()
//    { return normalMsg.empty();}
//
//    UC appendMsg(Packet *pack);
//    
//    Packet* getMsg();
//    
//    void run(); 
//    virtual void process(Packet *pack)=0;
//    bool sendMsg(int lnkId, Packet *pack);
//
//    void stopThread();
//private:
//
//    US  maxqueuesize;  
//    queue <Packet*> normalMsg;
//    TS_Mutex Msgmtx;
//    TS_WaitCondition *nrmmsg;
//};


/************************实现部分****************************/

#endif /*_THREAD_INCLUDE_*/
