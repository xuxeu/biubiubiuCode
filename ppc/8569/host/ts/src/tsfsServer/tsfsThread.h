/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  tsfsWorker.h
* @brief  
*       ���ܣ�
*       <li>�����߳���Ķ���</li>
*/
/************************ͷ�ļ�********************************/
#ifndef _THREAD_INCLUDE_
#define _THREAD_INCLUDE_

#include <queue>
#include "tsfsMutex.h"
using namespace std;

/************************�궨��********************************/
#define MAX_OB_NAME_LEN     50

#define NORMAL_QUEUE_SIZE   15

#define TSFS_PACKET_DATASIZE   1024 //���ݰ���С

#define TSFS_MAX_DATA_SIZE     1000//��������ݰ���С

/************************���Ͷ���******************************/
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
     *  ����:���ð�ԴAID
     * @param des_said  ����ԴAID
     */
    void setSrc_aid(US src_aid) 
	{
		this->src_aid = src_aid;
    }

    /**
     *  ����:�õ���ԴAID
     *  @return des_said  ����ԴAID
     */
    US getSrc_aid() 
	{
		return src_aid;
    }

    /**
     *  ����:���ð�ԴSAID
     * @param des_said  ����ԴSAID
     */
    void setSrc_said(US src_said) 
	{
		this->src_said = src_said;
    }

    /**
     *  ����:�õ���ԴSAID
     *  @return des_said  ����ԴSAID
     */
    US getSrc_said() 
	{
		return src_said;
    }


    /**
     *  ����:���ð�Ŀ��AID
     * @param des_said  ����Ŀ��AID
     */
    void setDes_aid(US des_aid) 
	{
		this->des_aid = des_aid;
    }

    /**
     *  ����:�õ���Ŀ��AID
     *  @return US  ����Ŀ��AID
     */
    US getDes_aid() 
	{
		return des_aid;
    }

    /**
     *  ����:���ð�Ŀ��SAID
     * @param des_said  ����Ŀ��SAID
     */
    void setDes_said(US des_said) 
	{
		this->des_said = des_said;
    }


    /**
     *  ����:�õ���Ŀ��SAID
     *  @return US  ����Ŀ��SAID
     */
    US getDes_said() 
	{
		return des_said;
    }

    /**
     *  ����:�õ���������
     *  @return char*  ������
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
     *  ����:�õ����Ĵ�С
     *  @return size ����С
     */
    US getSize()
	{
		return size;
    }

    /**
     *  ����:�õ��������Ĵ�С
     *  @return size ��������С
     */
    int getDataSize()
	{
        return sizeof(data);
    }

    /**
     *  ����:���ð��Ĵ�С
     * @param size ���ð���С
     */
    void setSize(US size) 
	{
		this->size = size;
    }
private:
    US  des_aid; //Ŀ��AID
    US  src_aid; //ԴAID
    US  des_said;//Ŀ��SAID
    US  src_said;//ԴSAID
    US  size;    //��Ϣ����С
    char data[TSFS_PACKET_DATASIZE] ;//����������
};

//�����߳���
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
    bool brunning;//����������
    static baseThread *pobject;//�̵߳ľ�̬����ָ��
    HANDLE hthreadHanlde;//�̵߳ľ��
    UI  uiThreadID;//�̵߳ı�ʶ
    UI  stack_size; //�̵߳�ջ�ռ�
    char threadname[MAX_OB_NAME_LEN];//�̶߳��������
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