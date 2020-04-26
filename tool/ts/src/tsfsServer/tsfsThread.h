/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  tsfsWorker.h
* @brief  
*       功能：
*       <li>基础线程类的定义</li>
*/
/************************头文件********************************/
#ifndef _THREAD_INCLUDE_
#define _THREAD_INCLUDE_

#include <queue>
#include "tsfsMutex.h"
using namespace std;

/************************宏定义********************************/
#define MAX_OB_NAME_LEN     50

#define NORMAL_QUEUE_SIZE   15

#define TSFS_PACKET_DATASIZE   1024 //数据包大小

#define TSFS_MAX_DATA_SIZE     1000//最大发送数据包大小

/************************类型定义******************************/
class Packet 
{

public:

    Packet()
    {
        des_aid = src_aid = 0;
        des_said = src_said = 0;
        memset(data, 0, sizeof(data));
    }

    ~Packet(){}

    /**
     *  功能:设置包源AID
     * @param des_said  包的源AID
     */
    void setSrc_aid(US src_aid) 
	{
		this->src_aid = src_aid;
    }

    /**
     *  功能:得到包源AID
     *  @return des_said  包的源AID
     */
    US getSrc_aid() 
	{
		return src_aid;
    }

    /**
     *  功能:设置包源SAID
     * @param des_said  包的源SAID
     */
    void setSrc_said(US src_said) 
	{
		this->src_said = src_said;
    }

    /**
     *  功能:得到包源SAID
     *  @return des_said  包的源SAID
     */
    US getSrc_said() 
	{
		return src_said;
    }


    /**
     *  功能:设置包目标AID
     * @param des_said  包的目标AID
     */
    void setDes_aid(US des_aid) 
	{
		this->des_aid = des_aid;
    }

    /**
     *  功能:得到包目标AID
     *  @return US  包的目标AID
     */
    US getDes_aid() 
	{
		return des_aid;
    }

    /**
     *  功能:设置包目标SAID
     * @param des_said  包的目标SAID
     */
    void setDes_said(US des_said) 
	{
		this->des_said = des_said;
    }


    /**
     *  功能:得到包目标SAID
     *  @return US  包的目标SAID
     */
    US getDes_said() 
	{
		return des_said;
    }

    /**
     *  功能:得到包的数据
     *  @return char*  包数据
     */
    char* getData() 
	{
		return data;
    }

    void setData(char *dataAddr, UI size)
    {
        memset(data, 0, sizeof(data));
        memcpy_s(data, sizeof(data), dataAddr, size);
    }

    /**
     *  功能:得到包的大小
     *  @return size 包大小
     */
    US getSize()
	{
		return size;
    }

    /**
     *  功能:得到缓冲区的大小
     *  @return size 缓冲区大小
     */
    int getDataSize()
	{
        return sizeof(data);
    }

    /**
     *  功能:设置包的大小
     * @param size 设置包大小
     */
    void setSize(US size) 
	{
		this->size = size;
    }
private:
    US  des_aid; //目标AID
    US  src_aid; //源AID
    US  des_said;//目标SAID
    US  src_said;//源SAID
    US  size;    //消息包大小
    char data[TSFS_PACKET_DATASIZE] ;//缓冲区数据
};

//基础线程类
class baseThread
{
public:
    baseThread(const char* name = NULL, UI size = 2048)
    {
        brunning = false;
        hthreadHanlde = NULL;
        uiThreadID = 0;
        stack_size = size;
        if(NULL == name)
        {
            strcpy_s(threadname, sizeof(threadname), "");
        }
        else
        {
            strcpy_s(threadname, sizeof(threadname), name);
        }
    }

    ~baseThread(){}

    inline HANDLE getThreadHanlde()
    {
        return hthreadHanlde;
    }

    inline UI getThreadID()
    {
        return uiThreadID;
    }

    inline char *getThreadName()
    {
        return threadname;
    }

    inline bool brun()
    {
        return brunning;
    }
     
    void start();
    void stop();
    void close();
    virtual void run()=0;
    static UI _stdcall prerun(void *object);
private:
    bool brunning;//是滞在运行
    static baseThread *pobject;//线程的静态对象指针
    HANDLE hthreadHanlde;//线程的句柄
    UI  uiThreadID;//线程的标识
    UI  stack_size; //线程的栈空间
    char threadname[MAX_OB_NAME_LEN];//线程对象的名字
};

class threadWithQueue:public baseThread
{
public:
    
    threadWithQueue(US size = NORMAL_QUEUE_SIZE)
    {
        maxqueuesize = size;
        nrmmsg = new waitcondition(size);
    }

    ~threadWithQueue()
    {
        for(int i = 0; i < normalMsg.size(); i++)
        {
            normalMsg.pop();
        }
        delete nrmmsg;
    }

    bool msgQueueIsFull()
    {
        return (normalMsg.size() >= maxqueuesize);
    }

    bool msgQueueIsEmpty()
    { 
		return normalMsg.empty();
	}

    UC appendMsg(Packet *pack);
    Packet* getMsg();
    void run(); 
    virtual void process(Packet *pack)=0;
    bool sendMsg(int lnkId, Packet *pack);
    void stopThread();
private:
    US maxqueuesize;  
    queue <Packet*> normalMsg;
    TMutex Msgmtx;
    waitcondition *nrmmsg;
};
#endif