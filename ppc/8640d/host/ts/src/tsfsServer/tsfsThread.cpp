/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  tsfsThread.cpp
* @brief  
*       ���ܣ�
*       <li>�����߳����ʵ��</li>
*/

/************************ͷ�ļ�********************************/
#include "stdafx.h"
#include "stdlib.h"
#include "windows.h"
#include "process.h"
#include "tsfsThread.h"
#include "tsfsCommon.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************ȫ�ֱ���******************************/

/************************ģ�����******************************/

/************************�ⲿ����******************************/

/************************ǰ������******************************/

/************************ʵ��*********************************/

/**
 * @Funcname: start
 * @brief : �̶߳������������
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��11��30�� 11:36:16
 *   
**/
baseThread* baseThread::pobject = NULL;

void baseThread::start()
{
    //δ�������������������ٴ���
    if(!brunning)
    {
        hthreadHanlde =(HANDLE)_beginthreadex(NULL,
                                              stack_size,
                                               &baseThread::prerun,//�߳���������
                                               this,
                                               true,               //��������
                                               &uiThreadID);
        if(hthreadHanlde)
        {
            brunning = true;
            /*//sysLoger(LINFO, "Thread %s start running! FILE:%s, LINE:%d", threadname, 
                                                __FILE__, __LINE__);*/
        }        
        else
        {
            brunning = false;
            //sysLoger(LERROR, "Thread %s Init failed ! FILE:%s, LINE:%d", threadname, 
                                                //__MYFILE__, __LINE__);
        }
    }
    return;
}

/**
 * @Funcname: close
 * @brief : �̵߳Ĺرպ���
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��11��30�� 11:42:36
 *   
**/
void baseThread::close( )
{
    //sysLoger(LINFO, "Thread %s stop running! FILE:%s, LINE:%d", threadname, 
                                           // __MYFILE__, __LINE__);
    _endthreadex(0);
    int closeStat = CloseHandle(hthreadHanlde);
        
}

/**
 * @Funcname: stop
 * @brief : �̵߳�ֹͣ����
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��11��30�� 11:42:36
 *   
**/
void baseThread::stop()
{
    brunning = false;  
    CloseHandle(hthreadHanlde);
}

/**
 * @Funcname: prerun
 * @brief : �̶߳����������ں���
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��11��30�� 11:42:36
 *   
**/
UI baseThread::prerun(void *object)
{
    static_cast<baseThread*>(object)->run();

    return 0;
}

/**
 * @Funcname: appendMsg
 * @brief : ����Ϣ���뵽��Ϣ����
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��11��30�� 11:42:36
 *   
**/
UC threadWithQueue::appendMsg(Packet *pack)
{
    if(!msgQueueIsFull())
    {
        Msgmtx.lock();
        normalMsg.push(pack);
        Msgmtx.unlock();
        nrmmsg->wakeup();
        return 0;
    }

    //sysLoger(LWARNING, "Object msgQueue is full!FILE:%s, LINE:%d", __MYFILE__, __LINE__);

    return -1;
}

/**
 * @Funcname: getMsg
 * @brief : �Ӷ�����ȡ����Ϣ
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��11��30�� 11:42:36
 *   
**/
Packet* threadWithQueue::getMsg()
{
    if(!msgQueueIsEmpty())
    {
        Msgmtx.lock();
        Packet *pack = normalMsg.front();
        normalMsg.pop();
        Msgmtx.unlock();
        return pack;
    }    
    return NULL;
}

/*
 * @brief
 *   ������Ϣ
 * @param pack[in]: Ҫ���͵����ݰ�
 * @return���յ����ݰ����򷵻ؽ��յ����ݰ��ֽڴ�С
 */
bool threadWithQueue::sendMsg(int serverAid, Packet *pack)
{
    return TClient_putpkt(  serverAid, 
                            pack->getDes_aid(), 
                            pack->getDes_said(), 
                            pack->getSrc_said(), 
                            pack->getData(), 
                            pack->getSize());
     
}

/**
 * @Funcname: run
 * @brief : ���к���,����Ϣ������ȡ����Ϣ���д���
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��11��30�� 11:42:36
 *   
**/
void threadWithQueue::run()
{
    Packet *pack = NULL;

    while(brun())
    {
        if(nrmmsg->wait())
        {
            pack = getMsg();
            if( NULL == pack )
            {
                continue;
            }
            process(pack);
        }
    }

    return;
}

/**
 * @Funcname: stopThread
 * @brief : ֹͣ�߳�
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��11��30�� 11:42:36
 *   
**/
void threadWithQueue::stopThread()
{
    this->stop();
    nrmmsg->wakeup();
}