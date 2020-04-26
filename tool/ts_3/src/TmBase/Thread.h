/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/


/**
 * @filename      :  Thread.h
 * @Version        :  1.0.0
 * @Brief           :  �����߳���Ķ���
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2006��11��30�� 11:06:44
 * @History        : 
 *   
 */

#ifndef _THREAD_INCLUDE_
#define _THREAD_INCLUDE_

#define MAX_OB_NAME_LEN     50
#define NORMAL_QUEUE_SIZE   15
#define DEFAULT_PRIORITY        -20

//#define IS_LOG
/************************���ò���*****************************/

//#include <queue>
//#include "stdlib.h"
//#include "windows.h"
#include "mutex.h"
#include "log.h"

//using namespace std;
/************************ǰ����������***********************/


/************************���岿��*****************************/



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
    //TS_Mutex threadMutex;           //������
};

/************************����Ϣ�߳�*************************/



///************************ǰ����������***********************/
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


/************************ʵ�ֲ���****************************/

#endif /*_THREAD_INCLUDE_*/
